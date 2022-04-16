#include "stdafx.h"
#include "TitleFormat.h"

TitleFormat::TitleFormat(const std::wstring& pattern)
{
	const string8 upattern = from_wide(pattern);
	titleformat_compiler::get()->compile_safe(m_obj, upattern);
}

STDMETHODIMP TitleFormat::get__ptr(void** out)
{
	if (!out) return E_POINTER;

	*out = m_obj.get_ptr();
	return S_OK;
}

STDMETHODIMP TitleFormat::Eval(BSTR* out)
{
	if (m_obj.is_empty() || !out) return E_POINTER;

	string8 str;
	playback_control::get()->playback_format_title(nullptr, str, m_obj, nullptr, playback_control::display_level_all);
	*out = to_bstr(str);
	return S_OK;
}

STDMETHODIMP TitleFormat::EvalActivePlaylistItem(UINT playlistItemIndex, BSTR* out)
{
	if (m_obj.is_empty() || !out) return E_POINTER;

	string8 str;
	Plman::theAPI()->activeplaylist_item_format_title(playlistItemIndex, nullptr, str, m_obj, nullptr, playback_control::display_level_all);
	*out = to_bstr(str);
	return S_OK;
}

STDMETHODIMP TitleFormat::EvalWithMetadb(IMetadbHandle* handle, BSTR* out)
{
	if (m_obj.is_empty() || !out) return E_POINTER;

	metadb_handle* ptr = nullptr;
	GET_PTR(handle, ptr);

	string8 str;
	ptr->format_title(nullptr, str, m_obj, nullptr);
	*out = to_bstr(str);
	return S_OK;
}

STDMETHODIMP TitleFormat::EvalWithMetadbs(IMetadbHandleList* handles, VARIANT* out)
{
	if (m_obj.is_empty() || !out) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	GET_PTR(handles, handles_ptr);

	ComArrayWriter writer(handles_ptr->get_count());

	for (const metadb_handle_ptr& handle : *handles_ptr)
	{
		string8 str;
		handle->format_title(nullptr, str, m_obj, nullptr);
		RETURN_IF_FAILED(writer.add_item(str));
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.data();
	return S_OK;
}

void TitleFormat::FinalRelease()
{
	m_obj.release();
}
