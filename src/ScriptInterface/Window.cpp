#include "stdafx.h"
#include "Window.h"
#include "PanelManager.h"
#include "PanelWindow.h"

Window::Window(PanelWindow* panel) : m_panel(panel) {}

STDMETHODIMP Window::ClearInterval(UINT id)
{
	m_panel->clear_timer(id);
	return S_OK;
}

STDMETHODIMP Window::ClearTimeout(UINT id)
{
	m_panel->clear_timer(id);
	return S_OK;
}

STDMETHODIMP Window::CreatePopupMenu(IMenuObj** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<MenuObj>(m_panel->m_hwnd);
	return S_OK;
}

STDMETHODIMP Window::CreateThemeManager(BSTR classid, IThemeManager** out)
{
	if (!out) return E_POINTER;

	auto theme = wil::unique_htheme(OpenThemeData(m_panel->m_hwnd, classid));
	*out = theme ? new ComObjectImpl<ThemeManager>(std::move(theme)) : nullptr;
	return S_OK;
}

STDMETHODIMP Window::CreateTooltip(BSTR name, int pxSize, int style, ITooltip** out)
{
	if (!out) return E_POINTER;

	if (!m_panel->create_tooltip()) return E_NOTIMPL;
	m_panel->set_tooltip_font(name, pxSize, style);

	*out = new ComObjectImpl<Tooltip>(m_panel->m_tooltip, m_panel->m_hwnd);
	return S_OK;
}

STDMETHODIMP Window::GetColourCUI(UINT type, int* out)
{
	if (!out) return E_POINTER;
	if (m_panel->m_is_default_ui) return E_NOTIMPL;
	if (type > 6) return E_INVALIDARG;

	*out = m_panel->get_colour_ui(type);
	return S_OK;
}

STDMETHODIMP Window::GetColourDUI(UINT type, int* out)
{
	if (!out) return E_POINTER;
	if (!m_panel->m_is_default_ui) return E_NOTIMPL;
	if (type >= guids::colours.size()) return E_INVALIDARG;

	*out = m_panel->get_colour_ui(type);
	return S_OK;
}

STDMETHODIMP Window::GetFontCUI(UINT type, IGdiFont** out)
{
	if (!out) return E_POINTER;
	if (m_panel->m_is_default_ui) return E_NOTIMPL;
	if (type > 1) return E_INVALIDARG;

	*out = m_panel->get_font_ui(type);
	return S_OK;
}

STDMETHODIMP Window::GetFontDUI(UINT type, IGdiFont** out)
{
	if (!out) return E_POINTER;
	if (!m_panel->m_is_default_ui) return E_NOTIMPL;
	if (type >= guids::fonts.size()) return E_INVALIDARG;

	*out = m_panel->get_font_ui(type);
	return S_OK;
}

STDMETHODIMP Window::GetProperty(BSTR name, VARIANT defaultval, VARIANT* out)
{
	if (!out) return E_POINTER;

	const string8 uname = from_wide(name);
	if (m_panel->m_config.m_properties.get_property(uname, *out))
	{
		return S_OK;
	}

	if (!m_panel->m_config.m_properties.set_property(uname, defaultval))
	{
		defaultval.vt = VT_NULL;
	}
	return VariantCopy(out, &defaultval);
}

STDMETHODIMP Window::NotifyOthers(BSTR name, VARIANT info)
{
	if (info.vt & VT_BYREF) return E_INVALIDARG;

	auto data = new CallbackData({ name, info });
	PanelManager::get().post_msg_to_all_pointer(CallbackID::on_notify_data, data, m_panel->m_hwnd);
	return S_OK;
}

STDMETHODIMP Window::Reload(VARIANT_BOOL clear_properties)
{
	if (to_bool(clear_properties))
	{
		m_panel->m_config.m_properties.m_data.clear();
	}
	m_panel->update_script();
	return S_OK;
}

STDMETHODIMP Window::Repaint()
{
	m_panel->repaint();
	return S_OK;
}

STDMETHODIMP Window::RepaintRect(int x, int y, int w, int h)
{
	m_panel->repaint(CRect(x, y, x + w, y + h));
	return S_OK;
}

STDMETHODIMP Window::SetCursor(UINT id)
{
	::SetCursor(LoadCursorW(nullptr, MAKEINTRESOURCE(id)));
	return S_OK;
}

STDMETHODIMP Window::SetInterval(IDispatch* func, UINT delay, UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->create_timer(func, delay, false);
	return S_OK;
}

STDMETHODIMP Window::SetProperty(BSTR name, VARIANT val)
{
	m_panel->m_config.m_properties.set_property(from_wide(name), val);
	return S_OK;
}

STDMETHODIMP Window::SetTimeout(IDispatch* func, UINT delay, UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->create_timer(func, delay, true);
	return S_OK;
}

STDMETHODIMP Window::SetTooltipFont(BSTR name, int pxSize, int style)
{
	m_panel->set_tooltip_font(name, pxSize, style);
	return S_OK;
}

STDMETHODIMP Window::ShowConfigure()
{
	fb2k::inMainThread([&]()
		{
			m_panel->show_configure_popup(m_panel->m_hwnd);
		});
	return S_OK;
}

STDMETHODIMP Window::ShowProperties()
{
	fb2k::inMainThread([&]()
		{
			m_panel->show_property_popup(m_panel->m_hwnd);
		});
	return S_OK;
}

STDMETHODIMP Window::get_Height(int* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_rect.Height();
	return S_OK;
}

STDMETHODIMP Window::get_ID(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_id;
	return S_OK;
}

STDMETHODIMP Window::get_InstanceType(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_is_default_ui ? 1 : 0;
	return S_OK;
}

STDMETHODIMP Window::get_IsTransparent(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(m_panel->is_transparent());
	return S_OK;
}

STDMETHODIMP Window::get_IsVisible(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(m_panel->m_hwnd.IsWindowVisible());
	return S_OK;
}

STDMETHODIMP Window::get_MaxHeight(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_max.y;
	return S_OK;
}

STDMETHODIMP Window::get_MaxWidth(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_max.x;
	return S_OK;
}

STDMETHODIMP Window::get_MinHeight(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_min.y;
	return S_OK;
}

STDMETHODIMP Window::get_MinWidth(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_min.x;
	return S_OK;
}

STDMETHODIMP Window::get_Name(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(m_panel->m_script_host->m_info.m_name);
	return S_OK;
}

STDMETHODIMP Window::get_Width(int* out)
{
	if (!out) return E_POINTER;

	*out = m_panel->m_rect.Width();
	return S_OK;
}

STDMETHODIMP Window::put_MaxHeight(UINT height)
{
	m_panel->m_max.y = height;
	m_panel->notify_size_limit_changed();
	return S_OK;
}

STDMETHODIMP Window::put_MaxWidth(UINT width)
{
	m_panel->m_max.x = width;
	m_panel->notify_size_limit_changed();
	return S_OK;
}

STDMETHODIMP Window::put_MinHeight(UINT height)
{
	m_panel->m_min.y = height;
	m_panel->notify_size_limit_changed();
	return S_OK;
}

STDMETHODIMP Window::put_MinWidth(UINT width)
{
	m_panel->m_min.x = width;
	m_panel->notify_size_limit_changed();
	return S_OK;
}
