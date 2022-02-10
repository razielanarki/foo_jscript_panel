#pragma once

using UniqueBitmap = std::unique_ptr<Gdiplus::Bitmap>;

template <typename T>
concept NotEnum = !std::is_scoped_enum_v<T>;

static bool to_bool(VARIANT_BOOL vb) { return vb != VARIANT_FALSE; }
static int to_int(NotEnum auto num) { return static_cast<int>(num); }
static uint32_t to_uint(NotEnum auto num) { return static_cast<uint32_t>(num); }
static COLORREF to_colorref(auto argb) { return Gdiplus::Color(to_uint(argb)).ToCOLORREF(); }
static HWND to_hwnd(uint32_t window_id) { return reinterpret_cast<HWND>(window_id); }
static VARIANT_BOOL to_variant_bool(auto b) { return b ? VARIANT_TRUE : VARIANT_FALSE; }

static int to_argb(COLORREF colour)
{
	Gdiplus::Color c;
	c.SetFromCOLORREF(colour);
	return to_int(Gdiplus::Color::MakeARGB(UINT8_MAX, c.GetRed(), c.GetGreen(), c.GetBlue()));
}

struct GuidHelper
{
	static std::string print(const GUID& g)
	{
		return (PFC_string_formatter() << "{" << pfc::print_guid(g) << "}").get_ptr();
	}

	static uint64_t hasher(const GUID& g)
	{
		return hasher_md5::get()->process_single_string(pfc::print_guid(g)).xorHalve();
	}

	uint64_t operator()(const GUID& g) const
	{
		return hasher(g);
	}
};
