#pragma once

class ThemeManager : public JSDisposableImpl<IThemeManager>
{
public:
	ThemeManager(wil::unique_htheme theme);

	STDMETHODIMP DrawThemeBackground(IGdiGraphics* gr, int x, int y, int w, int h, int clip_x, int clip_y, int clip_w, int clip_h) override;
	STDMETHODIMP GetThemeColour(int propid, int* out) override;
	STDMETHODIMP IsThemePartDefined(int partid, int stateid, VARIANT_BOOL* out) override;
	STDMETHODIMP SetPartAndStateID(int partid, int stateid) override;

protected:
	void FinalRelease() override;

private:
	int m_partid = 0, m_stateid = 0;
	wil::unique_htheme m_theme;
};
