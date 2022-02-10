#include "stdafx.h"
#include "Gdi.h"
#include "AsyncImageTask.h"

STDMETHODIMP Gdi::CreateImage(int w, int h, IGdiBitmap** out)
{
	if (!out) return E_POINTER;

	auto bitmap = std::make_unique<Gdiplus::Bitmap>(w, h, PixelFormat32bppPARGB);
	*out = ImageHelpers::check_bitmap(bitmap) ? new ComObjectImpl<GdiBitmap>(std::move(bitmap)) : nullptr;
	return S_OK;
}

STDMETHODIMP Gdi::Font(BSTR name, int pxSize, int style, IGdiFont** out)
{
	if (!out) return E_POINTER;
	if (pxSize == 0) return E_INVALIDARG;

	*out = new ComObjectImpl<GdiFont>(name, pxSize, static_cast<Gdiplus::FontStyle>(style));
	return S_OK;
}

STDMETHODIMP Gdi::Image(BSTR path, IGdiBitmap** out)
{
	if (!out) return E_POINTER;

	*out = ImageHelpers::load(path);
	return S_OK;
}

STDMETHODIMP Gdi::LoadImageAsync(UINT window_id, BSTR path, UINT* out)
{
	if (!out) return E_POINTER;

	auto task = std::make_unique<AsyncImageTask>(to_hwnd(window_id), path, ++m_task_id);
	SimpleThreadPool::get().add_task(std::move(task));
	*out = m_task_id;
	return S_OK;
}
