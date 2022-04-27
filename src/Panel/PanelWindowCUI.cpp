#include "stdafx.h"
#include "PanelWindow.h"

namespace
{
	class PanelWindowCUI : public PanelWindow, public uie::container_uie_window_v3, public cui::fonts::common_callback, public cui::colours::common_callback
	{
	public:
		PanelWindowCUI()
		{
			m_is_default_ui = false;
		}

		IGdiFont* get_font_ui(uint32_t type) override
		{
			LOGFONT lf;
			fb2k::std_api_get<cui::fonts::manager>()->get_font(static_cast<cui::fonts::font_type_t>(type), lf);
			return new ComObjectImpl<GdiFont>(lf);
		}

		LRESULT on_message(HWND hwnd, uint32_t msg, WPARAM wp, LPARAM lp) override
		{
			switch (msg)
			{
			case WM_CREATE:
				{
					fb2k::std_api_get<cui::colours::manager>()->register_common_callback(this);
					fb2k::std_api_get<cui::fonts::manager>()->register_common_callback(this);

					HWND parent = GetParent(hwnd);
					string8 parent_name = pfc::getWindowClassName(parent);
					m_supports_transparency = parent_name.equals("PSSWindowContainer") || parent_name.equals("ReBarWindow32");
				}
				break;
			case WM_DESTROY:
				fb2k::std_api_get<cui::colours::manager>()->deregister_common_callback(this);
				fb2k::std_api_get<cui::fonts::manager>()->deregister_common_callback(this);
				break;
			case WM_SETCURSOR:
				return 1;
			case WM_ERASEBKGND:
				if (is_transparent()) m_hwnd.PostMessageW(uwm::refreshbk);
				return 1;
			case WM_CONTEXTMENU:
				on_context_menu(lp);
				return 1;
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
				if (uie::window::g_process_keydown_keyboard_shortcuts(wp))
					return 0;
				break;
			}

			if (handle_message(hwnd, msg, wp, lp)) return 0;
			return DefWindowProc(hwnd, msg, wp, lp);
		}

		bool have_config_popup() const override
		{
			return true;
		}

		bool show_config_popup(HWND parent) override
		{
			show_configure_popup(parent);
			return true;
		}

		const GUID& get_extension_guid() const override
		{
			return guids::window_cui;
		}

		int get_colour_ui(uint32_t type) override
		{
			return to_argb(m_colours_helper.get_colour(static_cast<cui::colours::colour_identifier_t>(type)));
		}

		uie::container_window_v3_config get_window_config() override
		{
			return { L"jsp_class_cui", false, CS_DBLCLKS };
		}

		uint32_t get_type() const override
		{
			return uie::type_toolbar | uie::type_panel;
		}

		void get_category(pfc::string_base& out) const override
		{
			out = "Panels";
		}

		void get_config(stream_writer* writer, abort_callback& abort) const override
		{
			m_config.get(writer, abort);
		}

		void get_name(pfc::string_base& out) const override
		{
			out = Component::name;
		}

		void notify_size_limit_changed() override
		{
			get_host()->on_size_limit_change(m_hwnd, uie::size_limit_all);
		}

		void on_bool_changed(size_t) const override {}

		void on_colour_changed(size_t) const override
		{
			m_script_host->InvokeCallback(CallbackID::on_colours_changed);
		}

		void on_font_changed(size_t) const override
		{
			m_script_host->InvokeCallback(CallbackID::on_font_changed);
		}

		void set_config(stream_reader* reader, size_t size, abort_callback& abort) override
		{
			m_config.set(reader, size, abort);
		}

	private:
		cui::colours::helper m_colours_helper;
	};

	static uie::window_factory<PanelWindowCUI> g_panel_window_cui;
}
