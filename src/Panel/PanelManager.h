#pragma once

class PanelManager
{
public:
	PanelManager();

	static PanelManager& get();

	void add_window(CWindow hwnd);
	void post_msg_to_all(CallbackID id, WPARAM wp = 0);
	void post_msg_to_all_pointer(CallbackID id, pfc::refcounted_object_root* param, HWND except = nullptr);
	void remove_window(CWindow hwnd);
	void unload_all();

private:
	std::set<CWindow> m_hwnds;

	PFC_CLASS_NOT_COPYABLE_EX(PanelManager)
};
