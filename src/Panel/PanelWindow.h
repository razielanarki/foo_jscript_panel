#pragma once
#include "PanelConfig.h"
#include "ScriptHost.h"

class DropTargetImpl;
class PanelTimer;

class PanelWindow
{
protected:
	PanelWindow();
	virtual ~PanelWindow();

	bool handle_message(HWND hwnd, uint32_t msg, WPARAM wp, LPARAM lp);
	void build_context_menu(HMENU menu, uint32_t id_base);
	void execute_context_menu_command(uint32_t id, uint32_t id_base);
	void on_context_menu(LPARAM lp);

public:
	virtual IGdiFont* get_font_ui(uint32_t type) = 0;
	virtual int get_colour_ui(uint32_t type) = 0;
	virtual void notify_size_limit_changed() = 0;

	bool create_tooltip();
	bool is_transparent();
	uint32_t create_timer(IDispatch* pdisp, DWORD delay, bool execute_once);
	void clear_timer(uint32_t timer_id);
	void repaint(LPCRECT rect = nullptr);
	void set_tooltip_font(const std::wstring& name, int pxSize, int style);
	void show_configure_popup(HWND parent);
	void show_property_popup(HWND parent);
	void unload_script(bool invoke_callback = true);
	void update_script();

	CPoint m_min, m_max;
	CRect m_rect{};
	CWindow m_hwnd, m_tooltip;
	PanelConfig m_config;
	bool m_dragdrop = false;
	bool m_grabfocus = false;
	bool m_is_default_ui = true;
	bool m_supports_transparency = false;
	uint32_t m_id = 0;
	wil::com_ptr_t<ScriptHost> m_script_host;

private:
	void clear_timers();
	void invoke_timer(uint32_t timer_id);
	void load_script();
	void on_paint();
	void on_size();
	void refresh_background();
	void redraw(bool refreshbk);
	void reset_min_max();

	HANDLE m_timer_queue;
	bool m_is_mouse_tracked = false;
	bool m_refreshbk = true;
	std::map<uint32_t, std::unique_ptr<PanelTimer>> m_timer_map;
	ui_selection_holder::ptr m_selection_holder;
	uint32_t m_cur_timer_id = 0;
	wil::com_ptr_t<DropTargetImpl> m_drop_target;
	wil::com_ptr_t<GdiGraphics> m_gr_wrap;
	wil::unique_hbitmap m_gr_bmp, m_gr_bmp_bk;
	wil::unique_hfont m_tooltip_font;
};
