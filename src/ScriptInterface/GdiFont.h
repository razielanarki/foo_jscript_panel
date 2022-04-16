#pragma once

class GdiFont : public JSDisposableImpl<IGdiFont>
{
public:
	GdiFont(LOGFONT& lf);
	GdiFont(const std::wstring& name, int pxSize, Gdiplus::FontStyle style);

	STDMETHODIMP get__HFONT(HFONT* out) override;
	STDMETHODIMP get(void** out) override;

	STDMETHODIMP get_Height(UINT* out) override;
	STDMETHODIMP get_Name(BSTR* out) override;
	STDMETHODIMP get_Size(float* out) override;
	STDMETHODIMP get_Style(int* out) override;

protected:
	void FinalRelease() override;

private:
	std::unique_ptr<Gdiplus::Font> m_font;
	std::wstring m_name = L"Segoe UI";
	wil::unique_hfont m_hFont;
};
