#include "stdafx.h"
#include "SelectionHolder.h"

SelectionHolder::SelectionHolder() : m_holder(ui_selection_manager::get()->acquire()) {}

STDMETHODIMP SelectionHolder::SetPlaylistSelectionTracking()
{
	m_holder->set_playlist_selection_tracking();
	return S_OK;
}

STDMETHODIMP SelectionHolder::SetPlaylistTracking()
{
	m_holder->set_playlist_tracking();
	return S_OK;
}

STDMETHODIMP SelectionHolder::SetSelection(IMetadbHandleList* handles, UINT type)
{
	if (type >= guids::selections.size()) return E_INVALIDARG;

	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	m_holder->set_selection_ex(*handles_ptr, *guids::selections[type]);
	return S_OK;
}

void SelectionHolder::FinalRelease()
{
	m_holder.release();
}
