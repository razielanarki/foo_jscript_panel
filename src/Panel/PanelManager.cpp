#include "stdafx.h"
#include "PanelManager.h"

PanelManager::PanelManager() {}

PanelManager& PanelManager::get()
{
	static PanelManager instance;
	return instance;
}

void PanelManager::add_window(CWindow hwnd)
{
	m_hwnds.insert(hwnd);
}

void PanelManager::post_msg_to_all(CallbackID id, WPARAM wp)
{
	for (CWindow hwnd : m_hwnds)
	{
		hwnd.PostMessageW(std::to_underlying(id), wp);
	}
}

void PanelManager::post_msg_to_all_pointer(CallbackID id, pfc::refcounted_object_root* param, HWND except)
{
	for (CWindow hwnd : m_hwnds)
	{
		if (hwnd != except)
		{
			param->refcount_add_ref();
			hwnd.PostMessageW(std::to_underlying(id), reinterpret_cast<WPARAM>(param));
		}
	}
}

void PanelManager::remove_window(CWindow hwnd)
{
	m_hwnds.erase(hwnd);
}

void PanelManager::unload_all()
{
	for (CWindow hwnd : m_hwnds)
	{
		hwnd.SendMessageW(uwm::unload);
	}
}
