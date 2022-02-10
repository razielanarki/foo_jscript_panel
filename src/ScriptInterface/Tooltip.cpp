#include "stdafx.h"
#include "Tooltip.h"

Tooltip::Tooltip(CWindow tooltip, CWindow parent) : m_tooltip(tooltip), m_parent(parent)
{
	memset(&m_ti, 0, sizeof(TTTOOLINFOW));
	m_ti.cbSize = sizeof(TTTOOLINFOW);
	m_ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
	m_ti.hinst = core_api::get_my_instance();
	m_ti.hwnd = m_parent;
	m_ti.uId = reinterpret_cast<UINT_PTR>(m_parent.m_hWnd);
	m_ti.lpszText = m_tip_buffer.data();

	m_tooltip.SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	m_tooltip.SendMessageW(TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&m_ti));
	m_tooltip.SendMessageW(TTM_ACTIVATE);
}

STDMETHODIMP Tooltip::Activate()
{
	m_tooltip.SendMessageW(TTM_ACTIVATE, TRUE);
	return S_OK;
}

STDMETHODIMP Tooltip::Deactivate()
{
	m_tooltip.SendMessageW(TTM_ACTIVATE);
	return S_OK;
}

STDMETHODIMP Tooltip::GetDelayTime(int type, int* out)
{
	if (!out) return E_POINTER;
	if (!check_type(type)) return E_INVALIDARG;

	*out = m_tooltip.SendMessageW(TTM_GETDELAYTIME, type);
	return S_OK;
}

STDMETHODIMP Tooltip::SetDelayTime(int type, int time)
{
	if (!check_type(type)) return E_INVALIDARG;

	m_tooltip.SendMessageW(TTM_SETDELAYTIME, type, time);
	return S_OK;
}

STDMETHODIMP Tooltip::SetMaxWidth(int width)
{
	m_tooltip.SendMessageW(TTM_SETMAXTIPWIDTH, 0, width);
	return S_OK;
}

STDMETHODIMP Tooltip::TrackPosition(int x, int y)
{
	CPoint pt(x, y);
	m_parent.ClientToScreen(&pt);
	m_tooltip.SendMessageW(TTM_TRACKPOSITION, 0, MAKELONG(pt.x, pt.y));
	return S_OK;
}

STDMETHODIMP Tooltip::get_Text(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = SysAllocString(m_tip_buffer.data());
	return S_OK;
}

STDMETHODIMP Tooltip::put_Text(BSTR text)
{
	m_tip_buffer = text;
	m_ti.lpszText = m_tip_buffer.data();
	m_tooltip.SendMessageW(TTM_SETTOOLINFO, 0, reinterpret_cast<LPARAM>(&m_ti));
	return S_OK;
}

STDMETHODIMP Tooltip::put_TrackActivate(VARIANT_BOOL activate)
{
	static constexpr uint32_t flags = TTF_TRACK | TTF_ABSOLUTE;

	if (to_bool(activate)) m_ti.uFlags |= flags;
	else m_ti.uFlags &= ~flags;

	m_tooltip.SendMessageW(TTM_TRACKACTIVATE, to_bool(activate) ? TRUE : FALSE, reinterpret_cast<LPARAM>(&m_ti));
	return S_OK;
}

bool Tooltip::check_type(int type)
{
	return type >= TTDT_AUTOMATIC && type <= TTDT_INITIAL;
}
