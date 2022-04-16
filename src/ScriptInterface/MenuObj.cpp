#include "stdafx.h"
#include "MenuObj.h"

MenuObj::MenuObj(CWindow wnd_parent) : m_wnd_parent(wnd_parent), m_hMenu(CreatePopupMenu()) {}

STDMETHODIMP MenuObj::get__HMENU(HMENU* out)
{
	if (!is_menu() || !out) return E_POINTER;

	*out = m_hMenu.get();
	return S_OK;
}

STDMETHODIMP MenuObj::AppendMenuItem(UINT flags, UINT item_id, BSTR text)
{
	if (!is_menu()) return E_POINTER;
	if (flags & MF_POPUP || flags & MF_OWNERDRAW || flags & MF_BITMAP) return E_INVALIDARG;

	AppendMenuW(m_hMenu.get(), flags, item_id, text);
	return S_OK;
}

STDMETHODIMP MenuObj::AppendMenuSeparator()
{
	if (!is_menu()) return E_POINTER;

	AppendMenuW(m_hMenu.get(), MF_SEPARATOR, 0, 0);
	return S_OK;
}

STDMETHODIMP MenuObj::AppendTo(IMenuObj* obj, UINT flags, BSTR text)
{
	if (!is_menu()) return E_POINTER;

	HMENU menu;
	RETURN_IF_FAILED(obj->get__HMENU(&menu));

	AppendMenuW(menu, flags | MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(m_hMenu.get()), text);
	return S_OK;
}

STDMETHODIMP MenuObj::CheckMenuItem(UINT item_id, VARIANT_BOOL check)
{
	if (!is_menu()) return E_POINTER;

	::CheckMenuItem(m_hMenu.get(), item_id, to_bool(check) ? MF_CHECKED : MF_UNCHECKED);
	return S_OK;
}

STDMETHODIMP MenuObj::CheckMenuRadioItem(UINT first, UINT last, UINT selected)
{
	if (!is_menu()) return E_POINTER;

	::CheckMenuRadioItem(m_hMenu.get(), first, last, selected, MF_BYCOMMAND);
	return S_OK;
}

STDMETHODIMP MenuObj::TrackPopupMenu(int x, int y, UINT flags, UINT* out)
{
	if (!is_menu() || !out) return E_POINTER;

	flags |= TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON;
	flags &= ~TPM_RECURSE;

	CPoint pt(x, y);
	m_wnd_parent.ClientToScreen(&pt);
	*out = ::TrackPopupMenu(m_hMenu.get(), flags, pt.x, pt.y, 0, m_wnd_parent, nullptr);
	return S_OK;
}

bool MenuObj::is_menu()
{
	return m_hMenu && IsMenu(m_hMenu.get()) != FALSE;
}

void MenuObj::FinalRelease()
{
	m_hMenu.release();
}
