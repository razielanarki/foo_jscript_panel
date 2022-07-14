#include "stdafx.h"
#include "DialogConfigure.h"
#include "DialogProperties.h"
#include "DropTargetImpl.h"
#include "FontHelper.h"
#include "PanelManager.h"
#include "PanelTimer.h"
#include "PanelWindow.h"
#include "Timer.h"

PanelWindow::PanelWindow() : m_script_host(new ScriptHost(this)), m_timer_queue(CreateTimerQueue()) {}

PanelWindow::~PanelWindow()
{
	DeleteTimerQueue(m_timer_queue);
}

bool PanelWindow::create_tooltip()
{
	if (m_tooltip.IsWindow()) return false;

	m_tooltip = CreateWindowExW(
		WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		nullptr,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		m_hwnd,
		nullptr,
		core_api::get_my_instance(),
		nullptr);

	return true;
}

bool PanelWindow::handle_message(HWND hwnd, uint32_t msg, WPARAM wp, LPARAM lp)
{
	using enum CallbackID;

	switch (msg)
	{
	case WM_CREATE:
		m_hwnd = hwnd;
		m_id = reinterpret_cast<UINT_PTR>(hwnd);
		m_gr_wrap = new ImplementCOMRefCounter<GdiGraphics>();
		PanelManager::get().add_window(m_hwnd);
		load_script();
		return true;
	case WM_DESTROY:
		unload_script();
		PanelManager::get().remove_window(m_hwnd);
		m_gr_wrap.reset();
		return true;
	case WM_DISPLAYCHANGE:
	case WM_THEMECHANGED:
		update_script();
		return true;
	case WM_PAINT:
		if (is_transparent() && m_refreshbk) refresh_background();
		else this->on_paint();
		return true;
	case WM_SIZE:
		m_hwnd.GetClientRect(&m_rect);
		this->on_size();
		return true;
	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO info = reinterpret_cast<LPMINMAXINFO>(lp);
			info->ptMaxTrackSize = m_max;
			info->ptMinTrackSize = m_min;
			return true;
		}
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (m_grabfocus) m_hwnd.SetFocus();
		m_hwnd.SetCapture();
		return m_script_host->InvokeMouseCallback(msg, wp, lp);
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		{
			auto scope = wil::scope_exit([] { ReleaseCapture(); });
			if (msg == WM_RBUTTONUP && IsKeyPressed(VK_LSHIFT) && IsKeyPressed(VK_LWIN)) return false;
			return m_script_host->InvokeMouseCallback(msg, wp, lp);
		}
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		return m_script_host->InvokeMouseCallback(msg, wp, lp);
	case WM_MOUSEMOVE:
		{
			if (!m_is_mouse_tracked)
			{
				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hwnd, HOVER_DEFAULT };
				TrackMouseEvent(&tme);
				m_is_mouse_tracked = true;
				SetCursor(LoadCursorW(nullptr, IDC_ARROW));
			}

			return m_script_host->InvokeMouseCallback(msg, wp, lp);
		}
	case WM_MOUSELEAVE:
		m_is_mouse_tracked = false;
		SetCursor(LoadCursorW(nullptr, IDC_ARROW));
		m_script_host->InvokeCallback(on_mouse_leave);
		return false;
	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		m_script_host->InvokeCallback(g_msg_callback_map.at(msg), { GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 1 : -1 });
		return false;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		m_script_host->InvokeCallback(g_msg_callback_map.at(msg), { wp });
		return true;
	case WM_SETFOCUS:
		m_selection_holder = ui_selection_manager::get()->acquire();
		m_script_host->InvokeCallback(on_focus, { true });
		return false;
	case WM_KILLFOCUS:
		m_selection_holder.release();
		m_script_host->InvokeCallback(on_focus, { false });
		return false;
	case uwm::refreshbk:
		redraw(true);
		return true;
	case uwm::invoke_timer:
		invoke_timer(wp);
		return true;
	case uwm::unload:
		unload_script();
		return true;
	}

	if (msg < WM_USER) return false;

	const CallbackID id = static_cast<CallbackID>(msg);

	switch (id)
	{
	case on_dsp_preset_changed:
	case on_output_device_changed:
	case on_playback_dynamic_info:
	case on_playlist_items_selection_change:
	case on_playlist_switch:
	case on_playlists_changed:
	case on_selection_changed:
		m_script_host->InvokeCallback(id);
		return true;
	case on_main_menu:
	case on_playback_dynamic_info_track:
	case on_playback_order_changed:
	case on_playback_queue_changed:
	case on_playback_stop:
	case on_playback_time:
	case on_playlist_items_added:
	case on_playlist_items_reordered:
	case on_replaygain_mode_changed:
		m_script_host->InvokeCallback(id, { wp });
		return true;
	case on_always_on_top_changed:
	case on_cursor_follow_playback_changed:
	case on_playback_follow_cursor_changed:
	case on_playback_pause:
	case on_playlist_stop_after_current_changed:
		m_script_host->InvokeCallback(id, { wp == 1 });
		return true;
	case on_download_file_done:
	case on_http_request_done:
	case on_item_focus_change:
	case on_notify_data:
	case on_playback_seek:
	case on_playback_starting:
	case on_playlist_item_ensure_visible:
	case on_playlist_items_removed:
	case on_volume_change:
		{
			auto data = CallbackDataReleaser<CallbackData>(wp);

			m_script_host->InvokeCallback(id, data->m_args);
			return true;
		}
	case on_item_played:
	case on_playback_edited:
	case on_playback_new_track:
		{
			auto data = CallbackDataReleaser<MetadbCallbackData>(wp);
			auto handle = new ComObjectImpl<MetadbHandle>(data->m_handles[0]);
			m_script_host->InvokeCallback(id, { handle });
			if (handle) handle->Release();
			return true;
		}
	case on_library_items_added:
	case on_library_items_changed:
	case on_library_items_removed:
	case on_locations_added:
	case on_metadb_changed:
		{
			auto data = CallbackDataReleaser<MetadbCallbackData>(wp);
			auto handles = new ComObjectImpl<MetadbHandleList>(data->m_handles);

			if (id == on_locations_added)
			{
				m_script_host->InvokeCallback(id, { lp, handles }); // lp is the task_id for this callback
			}
			else
			{
				m_script_host->InvokeCallback(id, { handles });
			}
			if (handles) handles->Release();
			return true;
		}
	case on_get_album_art_done:
		{
			AsyncArtData* data = reinterpret_cast<AsyncArtData*>(wp);
			m_script_host->InvokeCallback(id, { data->m_handle, data->m_art_id, data->m_bitmap, data->m_path });
			return true;
		}
	case on_load_image_done:
		{
			AsyncImageData* data = reinterpret_cast<AsyncImageData*>(wp);
			m_script_host->InvokeCallback(id, { data->m_task_id, data->m_bitmap, data->m_path });
			return true;
		}
	}
	return false;
}

bool PanelWindow::is_transparent()
{
	return m_supports_transparency && m_config.m_transparent;
}

uint32_t PanelWindow::create_timer(IDispatch* pdisp, DWORD delay, bool execute_once)
{
	auto timer = std::make_unique<PanelTimer>(m_hwnd, pdisp, delay, execute_once, ++m_cur_timer_id);
	if (timer->start(m_timer_queue) && m_timer_map.try_emplace(m_cur_timer_id, std::move(timer)).second)
	{
		return m_cur_timer_id;
	}
	return 0;
}

void PanelWindow::build_context_menu(HMENU menu, uint32_t id_base)
{
	AppendMenuW(menu, MF_STRING, id_base + 1, L"&Reload");
	AppendMenuW(menu, MF_SEPARATOR, 0, 0);
	AppendMenuW(menu, MF_STRING, id_base + 2, L"&Properties");
	AppendMenuW(menu, MF_STRING, id_base + 3, L"&Configure...");
}

void PanelWindow::clear_timer(uint32_t timer_id)
{
	const auto it = m_timer_map.find(timer_id);
	if (it != m_timer_map.end())
	{
		DeleteTimerQueueTimer(m_timer_queue, it->second->m_handle, nullptr);
		m_timer_map.erase(timer_id);
	}
}

void PanelWindow::clear_timers()
{
	for (const auto& [id, timer] : m_timer_map)
	{
		DeleteTimerQueueTimer(m_timer_queue, timer->m_handle, nullptr);
	}
	m_timer_map.clear();
}

void PanelWindow::execute_context_menu_command(uint32_t id, uint32_t id_base)
{
	switch (id - id_base)
	{
	case 1:
		update_script();
		break;
	case 2:
		show_property_popup(m_hwnd);
		break;
	case 3:
		show_configure_popup(m_hwnd);
		break;
	}
}

void PanelWindow::invoke_timer(uint32_t timer_id)
{
	const auto it = m_timer_map.find(timer_id);
	if (it != m_timer_map.end())
	{
		const bool once = it->second->m_execute_once;
		it->second->invoke();
		if (once) m_timer_map.erase(timer_id);
	}
}

void PanelWindow::load_script()
{
	Timer timer;

	if (!m_is_default_ui)
	{
		LONG_PTR exstyle = 0L;
		if (m_config.m_style == 1) exstyle = WS_EX_CLIENTEDGE; // sunken
		else if (m_config.m_style == 2) exstyle = WS_EX_STATICEDGE; // grey
		m_hwnd.SetWindowLongPtrW(GWL_EXSTYLE, exstyle);
	}

	m_hwnd.SetWindowPos(nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	reset_min_max();
	notify_size_limit_changed();
	m_dragdrop = false;
	m_grabfocus = false;

	m_script_host->m_info.update(m_id, m_config.m_code);
	if (FAILED(m_script_host->Initialise()))
	{
		m_script_host->Reset();
		return;
	}

	if (m_dragdrop)
	{
		m_drop_target = new DropTargetImpl(this);
		RegisterDragDrop(m_hwnd, m_drop_target.get());
	}

	on_size();

	FB2K_console_formatter() << m_script_host->m_info.m_build_string << ": initialised in " << timer.query() << " ms";
}

void PanelWindow::on_context_menu(LPARAM lp)
{
	CPoint pt(lp);
	if (pt.x == -1 && pt.y == -1)
	{
		CRect rect;
		m_hwnd.GetWindowRect(&rect);
		pt = rect.CenterPoint();
	}

	auto menu = wil::unique_hmenu(CreatePopupMenu());
	constexpr uint32_t base_id = 0;
	build_context_menu(menu.get(), base_id);
	const int idx = TrackPopupMenuEx(menu.get(), TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, m_hwnd, nullptr);
	execute_context_menu_command(idx, base_id);
}

void PanelWindow::on_paint()
{
	if (!m_gr_bmp || !m_gr_wrap) return;

	auto paintdc = wil::BeginPaint(m_hwnd);
	auto memdc = wil::unique_hdc(CreateCompatibleDC(paintdc.get()));
	auto scope_bmp = wil::SelectObject(memdc.get(), m_gr_bmp.get());

	static constexpr COLORREF colour_bg_error = RGB(225, 60, 45);
	static constexpr COLORREF colour_white = RGB(255, 255, 255);

	if (m_script_host->CheckState())
	{
		if (is_transparent())
		{
			auto bkdc = wil::unique_hdc(CreateCompatibleDC(paintdc.get()));
			auto scope_bk = wil::SelectObject(bkdc.get(), m_gr_bmp_bk.get());
			BitBlt(memdc.get(), m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), bkdc.get(), m_rect.left, m_rect.top, SRCCOPY);
		}
		else
		{
			auto brush = wil::unique_hbrush(CreateSolidBrush(colour_white));
			FillRect(memdc.get(), &m_rect, brush.get());
		}

		Gdiplus::Graphics gr(memdc.get());
		gr.SetClip(Gdiplus::Rect(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height()));

		m_gr_wrap->put__ptr(&gr);
		m_script_host->InvokeCallback(CallbackID::on_paint, { m_gr_wrap.get() });
		m_gr_wrap->put__ptr(nullptr);
	}
	else
	{
		auto font = FontHelper::get().create(L"Segoe UI", 24, Gdiplus::FontStyleBold);
		auto scope = wil::SelectObject(memdc.get(), font.get());
		auto brush = wil::unique_hbrush(CreateSolidBrush(colour_bg_error));

		FillRect(memdc.get(), &m_rect, brush.get());
		SetBkMode(memdc.get(), TRANSPARENT);
		SetTextColor(memdc.get(), colour_white);
		DrawTextW(memdc.get(), L"JavaScript error", -1, &m_rect, DT_CENTER | DT_VCENTER | DT_NOPREFIX | DT_SINGLELINE);
	}
	BitBlt(paintdc.get(), 0, 0, m_rect.Width(), m_rect.Height(), memdc.get(), 0, 0, SRCCOPY);
	m_refreshbk = true;
}

void PanelWindow::on_size()
{
	if (m_rect.IsRectEmpty()) return;

	auto dc = wil::GetDC(m_hwnd);
	m_gr_bmp.reset(CreateCompatibleBitmap(dc.get(), m_rect.Width(), m_rect.Height()));

	if (is_transparent())
	{
		m_gr_bmp_bk.reset(CreateCompatibleBitmap(dc.get(), m_rect.Width(), m_rect.Height()));
		m_script_host->InvokeCallback(CallbackID::on_size);
		m_hwnd.PostMessageW(uwm::refreshbk);
	}
	else
	{
		m_gr_bmp_bk.reset();
		m_script_host->InvokeCallback(CallbackID::on_size);
		repaint();
	}
}

void PanelWindow::redraw(bool refreshbk)
{
	m_refreshbk = refreshbk;
	m_hwnd.RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
}

void PanelWindow::refresh_background()
{
	CWindow wnd_parent = GetAncestor(m_hwnd, GA_PARENT);

	if (!wnd_parent || IsIconic(core_api::get_main_window()) || !m_hwnd.IsWindowVisible())
		return;

	CPoint pt(0, 0);
	m_hwnd.ClientToScreen(&pt);
	wnd_parent.ScreenToClient(&pt);

	auto hrgn = wil::unique_hrgn(CreateRectRgn(0, 0, 0, 0));
	m_hwnd.SetWindowRgn(hrgn.get());
	wnd_parent.RedrawWindow(CRect(pt, m_rect.Size()), nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW);

	{
		auto dc_parent = wil::GetDC(wnd_parent);
		auto hdc_bk = wil::unique_hdc(CreateCompatibleDC(dc_parent.get()));
		auto scope = wil::SelectObject(hdc_bk.get(), m_gr_bmp_bk.get());
		BitBlt(hdc_bk.get(), m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), dc_parent.get(), pt.x, pt.y, SRCCOPY);
	}

	m_hwnd.SetWindowRgn(nullptr);
	if (m_config.m_style != 0) m_hwnd.SendMessageW(WM_NCPAINT, 1);
	redraw(false);
}

void PanelWindow::repaint(LPCRECT rect)
{
	m_refreshbk = false;
	m_hwnd.InvalidateRect(rect, FALSE);
}

void PanelWindow::reset_min_max()
{
	m_min.SetPoint(0, 0);
	m_max.SetPoint(INT_MAX, INT_MAX);
}

void PanelWindow::set_tooltip_font(const std::wstring& name, int pxSize, int style)
{
	if (!m_tooltip.IsWindow()) return;

	const std::wstring name_checked = FontHelper::get().get_name_checked(name);
	m_tooltip_font = FontHelper::get().create(name_checked, pxSize, static_cast<Gdiplus::FontStyle>(style));
	m_tooltip.SetFont(m_tooltip_font.get(), FALSE);
}

void PanelWindow::show_configure_popup(HWND parent)
{
	modal_dialog_scope scope;
	if (scope.can_create())
	{
		scope.initialize(parent);
		CDialogConfigure dlg(this);
		dlg.DoModal(parent);
	}
}

void PanelWindow::show_property_popup(HWND parent)
{
	modal_dialog_scope scope;
	if (scope.can_create())
	{
		scope.initialize(parent);
		CDialogProperties dlg(this);
		dlg.DoModal(parent);
	}
}

void PanelWindow::unload_script()
{
	if (m_script_host->CheckState())
	{
		m_script_host->InvokeCallback(CallbackID::on_script_unload);
	}

	clear_timers();
	m_script_host->Reset();
	m_selection_holder.release();

	if (m_tooltip.IsWindow())
	{
		m_tooltip.DestroyWindow();
	}

	if (m_dragdrop)
	{
		m_drop_target.reset();
		RevokeDragDrop(m_hwnd);
	}
}

void PanelWindow::update_script()
{
	if (m_hwnd == nullptr) return;

	unload_script();
	load_script();
}
