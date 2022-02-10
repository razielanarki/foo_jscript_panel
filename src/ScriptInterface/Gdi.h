#pragma once

class Gdi : public JSDispatchImpl<IGdi>
{
public:
	STDMETHODIMP CreateImage(int w, int h, IGdiBitmap** out) override;
	STDMETHODIMP Font(BSTR name, int pxSize, int style, IGdiFont** out) override;
	STDMETHODIMP Image(BSTR path, IGdiBitmap** out) override;
	STDMETHODIMP LoadImageAsync(UINT window_id, BSTR path, UINT* out) override;

private:
	uint32_t m_task_id = 0;
};
