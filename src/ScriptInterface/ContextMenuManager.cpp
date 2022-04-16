#include "stdafx.h"
#include "ContextMenuManager.h"

STDMETHODIMP ContextMenuManager::BuildMenu(IMenuObj* obj, int base_id)
{
	if (m_cm.is_empty()) return E_POINTER;

	HMENU menu;
	RETURN_IF_FAILED(obj->get__HMENU(&menu));

	contextmenu_node* parent = m_cm->get_root();
	m_cm->win32_build_menu(menu, parent, base_id, -1);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::ExecuteByID(UINT id, VARIANT_BOOL* out)
{
	if (m_cm.is_empty() || !out) return E_POINTER;

	*out = to_variant_bool(m_cm->execute_by_id(id));
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitContext(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	m_cm = contextmenu_manager::get();
	m_cm->init_context(*handles_ptr, contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitContextPlaylist()
{
	m_cm = contextmenu_manager::get();
	m_cm->init_context_playlist(contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

STDMETHODIMP ContextMenuManager::InitNowPlaying()
{
	m_cm = contextmenu_manager::get();
	m_cm->init_context_now_playing(contextmenu_manager::flag_show_shortcuts);
	return S_OK;
}

void ContextMenuManager::FinalRelease()
{
	m_cm.release();
}
