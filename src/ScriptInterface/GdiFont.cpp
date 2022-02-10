#include "stdafx.h"
#include "GdiFont.h"
#include "FontHelper.h"

GdiFont::GdiFont(LOGFONT& lf)
{
	if (FontHelper::get().check_name(lf.lfFaceName))
	{
		m_name = lf.lfFaceName;
	}
	else
	{
		wcsncpy_s(lf.lfFaceName, LF_FACESIZE, m_name.data(), m_name.length());
	}

	auto dc = wil::GetDC(nullptr);
	m_hFont.reset(CreateFontIndirectW(&lf));
	m_font = std::make_unique<Gdiplus::Font>(dc.get(), m_hFont.get());
}

GdiFont::GdiFont(const std::wstring& name, int pxSize, Gdiplus::FontStyle style)
{
	m_name = FontHelper::get().get_name_checked(name);
	m_hFont = FontHelper::get().create(m_name, pxSize, style);
	m_font = std::make_unique<Gdiplus::Font>(m_name.data(), static_cast<Gdiplus::REAL>(pxSize), style, Gdiplus::UnitPixel);
}

STDMETHODIMP GdiFont::get__HFONT(HFONT* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = m_hFont.get();
	return S_OK;
}

STDMETHODIMP GdiFont::get__ptr(void** out)
{
	*out = m_font.get();
	return S_OK;
}

STDMETHODIMP GdiFont::get_Height(UINT* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = to_uint(m_font->GetHeight(static_cast<Gdiplus::REAL>(QueryScreenDPI())));
	return S_OK;
}

STDMETHODIMP GdiFont::get_Name(BSTR* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = SysAllocString(m_name.data());
	return S_OK;
}

STDMETHODIMP GdiFont::get_Size(float* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = m_font->GetSize();
	return S_OK;
}

STDMETHODIMP GdiFont::get_Style(int* out)
{
	if (!m_font || !out) return E_POINTER;

	*out = m_font->GetStyle();
	return S_OK;
}

void GdiFont::FinalRelease()
{
	m_font.reset();
	m_hFont.reset();
}
