#include "stdafx.h"
#include "GdiBitmap.h"
#include "KMeans.h"
#include "StackBlur.h"

GdiBitmap::GdiBitmap(UniqueBitmap bitmap) : m_bitmap(std::move(bitmap)) {}

STDMETHODIMP GdiBitmap::get__ptr(void** out)
{
	*out = m_bitmap.get();
	return S_OK;
}

STDMETHODIMP GdiBitmap::ApplyAlpha(UINT8 alpha, IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	Gdiplus::ImageAttributes ia;
	Gdiplus::ColorMatrix cm = { 0.f };
	cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = cm.m[4][4] = 1.f;
	cm.m[3][3] = static_cast<float>(alpha) / UINT8_MAX;
	ia.SetColorMatrix(&cm);

	auto bitmap = apply_attributes(ia);
	*out = new ComObjectImpl<GdiBitmap>(std::move(bitmap));
	return S_OK;
}

STDMETHODIMP GdiBitmap::ApplyMask(IGdiBitmap* image, VARIANT_BOOL* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	Gdiplus::Bitmap* bitmap_mask = nullptr;
	GET_PTR(image, bitmap_mask);

	if (bitmap_mask->GetHeight() != m_bitmap->GetHeight() || bitmap_mask->GetWidth() != m_bitmap->GetWidth()) return E_INVALIDARG;

	*out = VARIANT_FALSE;

	const Gdiplus::Rect rect(0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight());
	Gdiplus::BitmapData bmpdata_dst, bmpdata_mask;

	if (bitmap_mask->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpdata_mask) == Gdiplus::Ok)
	{
		if (m_bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpdata_dst) == Gdiplus::Ok)
		{
			uint32_t* mask = static_cast<uint32_t*>(bmpdata_mask.Scan0);
			uint32_t* dst = static_cast<uint32_t*>(bmpdata_dst.Scan0);
			const uint32_t* mask_end = mask + (rect.Width * rect.Height);

			while (mask < mask_end)
			{
				uint32_t alpha = (((~*mask & UINT8_MAX) * (*dst >> 24)) << 16) & 0xff000000;
				*dst = alpha | (*dst & 0xffffff);
				++mask;
				++dst;
			}

			m_bitmap->UnlockBits(&bmpdata_dst);
			*out = VARIANT_TRUE;
		}
		bitmap_mask->UnlockBits(&bmpdata_mask);
	}
	return S_OK;
}

STDMETHODIMP GdiBitmap::Clone(float x, float y, float w, float h, IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	UniqueBitmap bitmap(m_bitmap->Clone(x, y, w, h, PixelFormat32bppPARGB));
	*out = new ComObjectImpl<GdiBitmap>(std::move(bitmap));
	return S_OK;
}

STDMETHODIMP GdiBitmap::GetColourSchemeJSON(UINT count, BSTR* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	const int width = std::min<int>(m_bitmap->GetWidth(), 220);
	const int height = std::min<int>(m_bitmap->GetHeight(), 220);
	const Gdiplus::Rect rect(0, 0, width, height);
	Gdiplus::BitmapData bmpdata;

	auto resized = resize(width, width);
	if (resized->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpdata) != Gdiplus::Ok) return E_POINTER;

	const uint32_t colours_length = bmpdata.Width * bmpdata.Height;
	const uint32_t* colours = static_cast<const uint32_t*>(bmpdata.Scan0);
	static constexpr std::array shifts = { RED_SHIFT, GREEN_SHIFT, BLUE_SHIFT };
	std::map<ColourValues, uint32_t> colour_counters;

	for (const uint32_t i : std::views::iota(0U, colours_length))
	{
		ColourValues values;
		std::ranges::transform(shifts, values.begin(), [colour = colours[i]](const auto& shift)
			{
				const uint8_t value = (colour >> shift) & UINT8_MAX;
				return static_cast<double>(value > 251 ? UINT8_MAX : (value + 4) & 0xf8);
			});

		++colour_counters[values];
	}

	resized->UnlockBits(&bmpdata);

	KPoints points;
	for (uint32_t id = 0; const auto& [values, pixel_count] : colour_counters)
	{
		points.emplace_back(KPoint(id++, values, pixel_count));
	}

	KMeans kmeans(points, count);
	Clusters clusters = kmeans.run();

	json j = json::array();
	for (Cluster& cluster : clusters)
	{
		j.push_back(
			{
				{ "col", cluster.get_colour() },
				{ "freq", cluster.get_frequency(colours_length) },
			});
	}
	*out = to_bstr(j.dump());
	return S_OK;
}

STDMETHODIMP GdiBitmap::GetGraphics(IGdiGraphics** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = new ComObjectImpl<GdiGraphics>();
	(*out)->put__ptr(new Gdiplus::Graphics(m_bitmap.get()));
	return S_OK;
}

STDMETHODIMP GdiBitmap::InvertColours(IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	Gdiplus::ImageAttributes ia;
	Gdiplus::ColorMatrix cm = { 0.f };
	cm.m[0][0] = cm.m[1][1] = cm.m[2][2] = -1.f;
	cm.m[3][3] = cm.m[4][0] = cm.m[4][1] = cm.m[4][2] = cm.m[4][4] = 1.f;
	ia.SetColorMatrix(&cm);

	auto bitmap = apply_attributes(ia);
	*out = new ComObjectImpl<GdiBitmap>(std::move(bitmap));
	return S_OK;
}

STDMETHODIMP GdiBitmap::ReleaseGraphics(IGdiGraphics* gr)
{
	Gdiplus::Graphics* graphics = nullptr;
	GET_PTR(gr, graphics);
	gr->put__ptr(nullptr);
	if (graphics) delete graphics;
	return S_OK;
}

STDMETHODIMP GdiBitmap::Resize(UINT width, UINT height, int interpolation_mode, IGdiBitmap** out)
{
	if (!m_bitmap || !out) return E_POINTER;

	auto resized = resize(to_int(width), to_int(height), static_cast<Gdiplus::InterpolationMode>(interpolation_mode));
	*out = new ComObjectImpl<GdiBitmap>(std::move(resized));
	return S_OK;
}

STDMETHODIMP GdiBitmap::RotateFlip(UINT mode)
{
	if (!m_bitmap) return E_POINTER;

	m_bitmap->RotateFlip(static_cast<Gdiplus::RotateFlipType>(mode));
	return S_OK;
}

STDMETHODIMP GdiBitmap::SaveAs(BSTR path, BSTR format, VARIANT_BOOL* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = VARIANT_FALSE;

	std::map<std::wstring, CLSID> encoder_map;
	uint32_t num = 0, size = 0;
	if (Gdiplus::GetImageEncodersSize(&num, &size) == Gdiplus::Ok && size > 0)
	{
		std::vector<Gdiplus::ImageCodecInfo> codecs(size);
		if (Gdiplus::GetImageEncoders(num, size, codecs.data()) == Gdiplus::Ok)
		{
			for (const auto& codec : std::ranges::views::take(codecs, num))
			{
				encoder_map.emplace(codec.MimeType, codec.Clsid);
			}
		}
	}

	const auto it = encoder_map.find(format);
	if (it != encoder_map.end())
	{
		*out = to_variant_bool(m_bitmap->Save(path, &it->second) == Gdiplus::Ok);
	}
	return S_OK;
}

STDMETHODIMP GdiBitmap::StackBlur(UINT8 radius)
{
	if (!m_bitmap) return E_POINTER;

	const Gdiplus::Rect rect(0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight());
	Gdiplus::BitmapData bmpdata;

	if (m_bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB, &bmpdata) == Gdiplus::Ok)
	{
		uint8_t* src = static_cast<uint8_t*>(bmpdata.Scan0);
		::StackBlur job(radius, rect.Width, rect.Height);
		job.Run(src);

		m_bitmap->UnlockBits(&bmpdata);
	}

	return S_OK;
}

STDMETHODIMP GdiBitmap::get_Height(UINT* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = m_bitmap->GetHeight();
	return S_OK;
}

STDMETHODIMP GdiBitmap::get_Width(UINT* out)
{
	if (!m_bitmap || !out) return E_POINTER;

	*out = m_bitmap->GetWidth();
	return S_OK;
}

UniqueBitmap GdiBitmap::apply_attributes(const Gdiplus::ImageAttributes& ia)
{
	const int width = to_int(m_bitmap->GetWidth());
	const int height = to_int(m_bitmap->GetHeight());
	auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(bitmap.get());
	g.DrawImage(m_bitmap.get(), Gdiplus::Rect(0, 0, width, height), 0, 0, width, height, Gdiplus::UnitPixel, &ia);
	return bitmap;
}

UniqueBitmap GdiBitmap::resize(int width, int height, Gdiplus::InterpolationMode interpolation_mode)
{
	auto bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(bitmap.get());
	g.SetInterpolationMode(interpolation_mode);
	g.DrawImage(m_bitmap.get(), 0, 0, width, height);
	return bitmap;
}

void GdiBitmap::FinalRelease()
{
	if (m_bitmap)
	{
		m_bitmap.reset();
	}
}
