#pragma once
#ifdef NDEBUG
#include <libwebp/decode.h>
#pragma comment(lib, "../foobar2000-sdk/lib/libwebp/libwebp.lib")
#endif

using ImageData = std::vector<uint8_t>;

namespace ImageHelpers
{
	static bool check_bitmap(const UniqueBitmap& bitmap)
	{
		return bitmap && bitmap->GetLastStatus() == Gdiplus::Ok;
	}

	static bool istream_to_image_data(IStream* stream, ImageData& data)
	{
		STATSTG sts;
		if (FAILED(stream->Stat(&sts, STATFLAG_DEFAULT))) return false;

		const DWORD bytes = sts.cbSize.LowPart;
		data.resize(bytes);
		ULONG bytes_read = 0;
		return SUCCEEDED(stream->Read(data.data(), bytes, &bytes_read)) && bytes == bytes_read;
	}

	static bool gdiplus_istream_to_bitmap(IStream* stream, UniqueBitmap& bitmap)
	{
		bitmap = std::make_unique<Gdiplus::Bitmap>(stream, TRUE);
		return check_bitmap(bitmap);
	}

	static bool libwebp_data_to_bitmap(const uint8_t* data, size_t bytes, UniqueBitmap& bitmap)
	{
#ifdef NDEBUG
		WebPBitstreamFeatures bs;
		if (WebPGetFeatures(data, bytes, &bs) == VP8_STATUS_OK && !bs.has_animation)
		{
			const Gdiplus::PixelFormat pf = PixelFormat32bppARGB;
			const int width = bs.width;
			const int height = bs.height;
			bitmap = std::make_unique<Gdiplus::Bitmap>(width, height, pf);
			if (check_bitmap(bitmap))
			{
				const Gdiplus::Rect rect(0, 0, width, height);
				Gdiplus::BitmapData bmpdata;
				if (bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, pf, &bmpdata) == Gdiplus::Ok)
				{
					const bool success = !!WebPDecodeBGRAInto(data, bytes, static_cast<uint8_t*>(bmpdata.Scan0), bmpdata.Stride * height, bmpdata.Stride);
					return bitmap->UnlockBits(&bmpdata) == Gdiplus::Ok && success;
				}
			}
		}
#endif
		return false;
	}

	static bool libwebp_istream_to_bitmap(IStream* stream, UniqueBitmap& bitmap)
	{
		ImageData data;
		return istream_to_image_data(stream, data) && libwebp_data_to_bitmap(data.data(), data.size(), bitmap);
	}

	static IGdiBitmap* load(const std::wstring& path)
	{
		wil::com_ptr_t<IStream> stream;
		if (SUCCEEDED(SHCreateStreamOnFileEx(path.data(), STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, &stream)))
		{
			UniqueBitmap bitmap;
			bool ok = gdiplus_istream_to_bitmap(stream.get(), bitmap);
			if (!ok) ok = libwebp_istream_to_bitmap(stream.get(), bitmap);
			if (ok) return new ComObjectImpl<GdiBitmap>(std::move(bitmap));
		}
		return nullptr;
	}
}
