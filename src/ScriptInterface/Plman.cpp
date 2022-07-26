#include "stdafx.h"
#include "Plman.h"

#include "CustomSort.h"
#include "PlaylistLock.h"
#include "DialogPlaylistLock.h"
#include "EnumItemsCallback.h"
#include "ProcessLocationsNotify.h"

namespace
{
	static playlist_manager_v4::ptr g_cachedAPI;

	class InitQuitPlman : public initquit
	{
	public:
		void on_quit() override
		{
			g_cachedAPI.release();
		}
	};

	FB2K_SERVICE_FACTORY(InitQuitPlman);
}

Plman::Plman()
{
	g_cachedAPI = playlist_manager_v4::get();
}

STDMETHODIMP Plman::AddLocations(UINT playlistIndex, VARIANT locations, VARIANT_BOOL select)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));
	if (theAPI()->playlist_lock_get_filter_mask(playlistIndex) & playlist_lock::filter_add)	return E_INVALIDARG;

	pfc::string_list_impl list;
	ComArrayReader reader;
	RETURN_IF_FAILED(reader.convert(locations, list));

	constexpr uint32_t flags = playlist_incoming_item_filter_v2::op_flag_no_filter | playlist_incoming_item_filter_v2::op_flag_delay_ui;
	auto obj = fb2k::service_new<ProcessLocationsNotifyPlaylist>(playlistIndex, theAPI()->playlist_get_item_count(playlistIndex), to_bool(select));
	playlist_incoming_item_filter_v2::get()->process_locations_async(list, flags, nullptr, nullptr, nullptr, obj);
	return S_OK;
}

STDMETHODIMP Plman::AddPlaylistItemToPlaybackQueue(UINT playlistIndex, UINT playlistItemIndex)
{
	RETURN_IF_FAILED(check_playlist_item(playlistIndex, playlistItemIndex));

	theAPI()->queue_add_item_playlist(playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::AddPlaylistLock(UINT playlistIndex, UINT flags, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = to_variant_bool(PlaylistLock::add(playlistIndex, flags));
	return S_OK;
}

STDMETHODIMP Plman::ClearPlaylist(UINT playlistIndex)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	theAPI()->playlist_clear(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::ClearPlaylistSelection(UINT playlistIndex)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	theAPI()->playlist_clear_selection(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::CreateAutoPlaylist(UINT playlistIndex, BSTR name, BSTR query, BSTR sort, UINT flags, int* out)
{
	if (!out) return E_POINTER;

	*out = -1;

	const string8 usort = from_wide(sort);
	const string8 uquery = from_wide(query);
	search_filter_v2::ptr filter;

	try
	{
		filter = search_filter_manager_v2::get()->create_ex(uquery, fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
	}
	catch (...) {}

	if (filter.is_valid())
	{
		const uint32_t pos = create_playlist(playlistIndex, name);
		autoplaylist_manager::get()->add_client_simple(uquery, usort, pos, flags);
		*out = to_int(pos);
	}
	return S_OK;
}

STDMETHODIMP Plman::CreatePlaylist(UINT playlistIndex, BSTR name, UINT* out)
{
	if (!out) return E_POINTER;

	*out = create_playlist(playlistIndex, name);
	return S_OK;
}

STDMETHODIMP Plman::DuplicatePlaylist(UINT playlistIndex, BSTR name, UINT* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	metadb_handle_list items;
	theAPI()->playlist_get_all_items(playlistIndex, items);

	string8 uname = from_wide(name);
	if (uname.is_empty())
	{
		theAPI()->playlist_get_name(playlistIndex, uname);
	}

	stream_reader_dummy dummy_reader;
	*out = theAPI()->create_playlist_ex(uname, uname.get_length(), playlistIndex + 1, items, &dummy_reader, fb2k::noAbort);
	return S_OK;
}

STDMETHODIMP Plman::EnsurePlaylistItemVisible(UINT playlistIndex, UINT playlistItemIndex)
{
	RETURN_IF_FAILED(check_playlist_item(playlistIndex, playlistItemIndex));

	theAPI()->playlist_ensure_visible(playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::ExecutePlaylistDefaultAction(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_item(playlistIndex, playlistItemIndex));

	*out = to_variant_bool(theAPI()->playlist_execute_default_action(playlistIndex, playlistItemIndex));
	return S_OK;
}

STDMETHODIMP Plman::FindOrCreatePlaylist(BSTR name, VARIANT_BOOL unlocked, UINT* out)
{
	if (!out) return E_POINTER;

	const string8 uname = from_wide(name);

	if (to_bool(unlocked))
	{
		*out = theAPI()->find_or_create_playlist_unlocked(uname);
	}
	else
	{
		*out = theAPI()->find_or_create_playlist(uname);
	}
	return S_OK;
}

STDMETHODIMP Plman::FindPlaylist(BSTR name, int* out)
{
	if (!out) return E_POINTER;

	const string8 uname = from_wide(name);
	*out = to_int(theAPI()->find_playlist(uname));
	return S_OK;
}

STDMETHODIMP Plman::FlushPlaybackQueue()
{
	theAPI()->queue_flush();
	return S_OK;
}

STDMETHODIMP Plman::GetPlaybackQueueHandles(IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	pfc::list_t<t_playback_queue_item> contents;
	theAPI()->queue_get_contents(contents);
	metadb_handle_list items;

	for (const t_playback_queue_item& queue_item : contents)
	{
		items.add_item(queue_item.m_handle);
	}

	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Plman::GetPlayingItemLocation(IPlayingItemLocation** out)
{
	if (!out) return E_POINTER;

	uint32_t playlistIndex = 0, playlistItemIndex = 0;
	bool isValid = theAPI()->get_playing_item_location(&playlistIndex, &playlistItemIndex);
	*out = new ComObjectImpl<PlayingItemLocation>(isValid, playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistFocusItemIndex(UINT playlistIndex, int* out)
{
	if (!out) return E_POINTER;

	*out = to_int(theAPI()->playlist_get_focus_item(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistItems(UINT playlistIndex, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	theAPI()->playlist_get_all_items(playlistIndex, items);
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistLockFilterMask(UINT playlistIndex, int* out)
{
	if (!out) return E_POINTER;

	*out = to_int(theAPI()->playlist_lock_get_filter_mask(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistLockName(UINT playlistIndex, BSTR* out)
{
	if (!out) return E_POINTER;

	string8 str;
	theAPI()->playlist_lock_query_name(playlistIndex, str);
	*out = to_bstr(str);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistName(UINT playlistIndex, BSTR* out)
{
	if (!out) return E_POINTER;

	string8 str;
	theAPI()->playlist_get_name(playlistIndex, str);
	*out = to_bstr(str);
	return S_OK;
}

STDMETHODIMP Plman::GetPlaylistSelectedItems(UINT playlistIndex, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	theAPI()->playlist_get_selected_items(playlistIndex, items);
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Plman::GetRecyclerItems(UINT index, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;
	if (index >= theAPI()->recycler_get_count()) return E_INVALIDARG;

	metadb_handle_list items;
	theAPI()->recycler_get_content(index, items);
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Plman::GetRecyclerName(UINT index, BSTR* out)
{
	if (!out) return E_POINTER;
	if (index >= theAPI()->recycler_get_count()) return E_INVALIDARG;

	string8 str;
	theAPI()->recycler_get_name(index, str);
	*out = to_bstr(str);
	return S_OK;
}

STDMETHODIMP Plman::InsertPlaylistItems(UINT playlistIndex, UINT base, IMetadbHandleList* handles, VARIANT_BOOL select)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	theAPI()->playlist_insert_items(playlistIndex, base, *handles_ptr, pfc::bit_array_val(to_bool(select)));
	return S_OK;
}

STDMETHODIMP Plman::InsertPlaylistItemsFilter(UINT playlistIndex, UINT base, IMetadbHandleList* handles, VARIANT_BOOL select)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	theAPI()->playlist_insert_items_filter(playlistIndex, base, *handles_ptr, to_bool(select));
	return S_OK;
}

STDMETHODIMP Plman::InvertSelection(UINT playlistIndex)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	pfc::bit_array_bittable mask(theAPI()->playlist_get_item_count(playlistIndex));
	theAPI()->playlist_get_selection_mask(playlistIndex, mask);
	theAPI()->playlist_set_selection(playlistIndex, pfc::bit_array_true(), pfc::bit_array_not(mask));
	return S_OK;
}

STDMETHODIMP Plman::IsAutoPlaylist(UINT playlistIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = to_variant_bool(autoplaylist_manager::get()->is_client_present(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::IsPlaylistItemSelected(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	//RETURN_IF_FAILED(check_playlist_item(playlistIndex, playlistItemIndex));

	*out = to_variant_bool(theAPI()->playlist_is_item_selected(playlistIndex, playlistItemIndex));
	return S_OK;
}

STDMETHODIMP Plman::IsPlaylistLocked(UINT playlistIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = to_variant_bool(theAPI()->playlist_lock_is_present(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::MovePlaylist(UINT from, UINT to, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = VARIANT_FALSE;

	const size_t count = theAPI()->get_playlist_count();

	if (from < count && to < count && from != to)
	{
		CustomSort::Order order(count);
		std::iota(order.begin(), order.end(), 0U);
		pfc::create_move_items_permutation(order.data(), count, pfc::bit_array_one(from), to - from);

		*out = to_variant_bool(theAPI()->reorder(order.data(), order.size()));
	}
	return S_OK;
}

STDMETHODIMP Plman::MovePlaylistSelection(UINT playlistIndex, int delta, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = to_variant_bool(theAPI()->playlist_move_selection(playlistIndex, delta));
	return S_OK;
}

STDMETHODIMP Plman::MovePlaylistSelectionV2(UINT playlistIndex, UINT new_pos, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = VARIANT_FALSE;

	const uint32_t count = theAPI()->playlist_get_item_count(playlistIndex);
	const uint32_t selected_count = theAPI()->playlist_get_selection_count(playlistIndex, UINT_MAX);

	if (count > 0 && selected_count > 0 && !theAPI()->playlist_is_item_selected(playlistIndex, new_pos))
	{
		EnumItemsCallback cb(std::min(new_pos, count));
		theAPI()->playlist_enum_items(playlistIndex, cb, pfc::bit_array_true());
		*out = to_variant_bool(cb.reorder(playlistIndex));
	}
	return S_OK;
}

STDMETHODIMP Plman::PlaylistItemCount(UINT playlistIndex, UINT* out)
{
	if (!out) return E_POINTER;

	*out = theAPI()->playlist_get_item_count(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::RecyclerPurge(VARIANT affectedItems)
{
	pfc::bit_array_bittable mask(theAPI()->recycler_get_count());
	ComArrayReader reader;
	RETURN_IF_FAILED(reader.convert(affectedItems, mask));

	if (mask.size())
	{
		theAPI()->recycler_purge(mask);
	}
	return S_OK;
}

STDMETHODIMP Plman::RecyclerRestore(UINT index)
{
	if (index >= theAPI()->recycler_get_count()) return E_INVALIDARG;

	theAPI()->recycler_restore(index);
	return S_OK;
}

STDMETHODIMP Plman::RemoveItemFromPlaybackQueue(UINT index)
{
	if (index >= theAPI()->queue_get_count()) return E_INVALIDARG;

	theAPI()->queue_remove_mask(pfc::bit_array_one(index));
	return S_OK;
}

STDMETHODIMP Plman::RemoveItemsFromPlaybackQueue(VARIANT affectedItems)
{
	pfc::bit_array_bittable mask(theAPI()->queue_get_count());
	ComArrayReader reader;
	RETURN_IF_FAILED(reader.convert(affectedItems, mask));

	if (mask.size())
	{
		theAPI()->queue_remove_mask(mask);
	}
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylist(UINT playlistIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = to_variant_bool(theAPI()->remove_playlist(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylistLock(UINT playlistIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = to_variant_bool(PlaylistLock::remove(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylists(VARIANT playlistIndexes, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	pfc::bit_array_bittable mask(theAPI()->get_playlist_count());
	ComArrayReader reader;
	RETURN_IF_FAILED(reader.convert(playlistIndexes, mask));

	*out = VARIANT_FALSE;

	if (mask.size())
	{
		*out = to_variant_bool(theAPI()->remove_playlists(mask));
	}
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylistSelection(UINT playlistIndex, VARIANT_BOOL crop)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	theAPI()->playlist_remove_selection(playlistIndex, to_bool(crop));
	return S_OK;
}

STDMETHODIMP Plman::RemovePlaylistSwitch(UINT playlistIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = to_variant_bool(theAPI()->remove_playlist_switch(playlistIndex));
	return S_OK;
}

STDMETHODIMP Plman::RenamePlaylist(UINT playlistIndex, BSTR name, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	const string8 uname = from_wide(name);
	*out = to_variant_bool(theAPI()->playlist_rename(playlistIndex, uname, uname.get_length()));
	return S_OK;
}

STDMETHODIMP Plman::SetActivePlaylistContext()
{
	ui_edit_context_manager::get()->set_context_active_playlist();
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistFocusItem(UINT playlistIndex, UINT playlistItemIndex)
{
	theAPI()->playlist_set_focus_item(playlistIndex, playlistItemIndex);
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistFocusItemByHandle(UINT playlistIndex, IMetadbHandle* handle)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	theAPI()->playlist_set_focus_by_handle(playlistIndex, handle_ptr);
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistSelection(UINT playlistIndex, VARIANT affectedItems, VARIANT_BOOL state)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	pfc::bit_array_bittable mask(theAPI()->playlist_get_item_count(playlistIndex));
	ComArrayReader reader;
	RETURN_IF_FAILED(reader.convert(affectedItems, mask));

	if (mask.size())
	{
		theAPI()->playlist_set_selection(playlistIndex, mask, pfc::bit_array_val(to_bool(state)));
	}
	return S_OK;
}

STDMETHODIMP Plman::SetPlaylistSelectionSingle(UINT playlistIndex, UINT playlistItemIndex, VARIANT_BOOL state)
{
	RETURN_IF_FAILED(check_playlist_item(playlistIndex, playlistItemIndex));

	theAPI()->playlist_set_selection_single(playlistIndex, playlistItemIndex, to_bool(state));
	return S_OK;
}

STDMETHODIMP Plman::ShowAutoPlaylistUI(UINT playlistIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = VARIANT_FALSE;

	auto api = autoplaylist_manager::get();
	if (api->is_client_present(playlistIndex))
	{
		api->query_client(playlistIndex)->show_ui(playlistIndex);
		*out = VARIANT_TRUE;
	}
	return S_OK;
}

STDMETHODIMP Plman::ShowPlaylistLockUI(UINT playlistIndex, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	*out = VARIANT_FALSE;

	string8 name;
	theAPI()->playlist_lock_query_name(playlistIndex, name);

	if (Component::name == name.get_ptr() || !theAPI()->playlist_lock_is_present(playlistIndex))
	{
		modal_dialog_scope scope;
		if (scope.can_create())
		{
			HWND hwnd = core_api::get_main_window();
			scope.initialize(hwnd);

			const uint32_t flags = theAPI()->playlist_lock_get_filter_mask(playlistIndex);

			CDialogPlaylistLock dlg(playlistIndex, flags);
			dlg.DoModal(hwnd);
			*out = VARIANT_TRUE;
		}
	}
	return S_OK;
}

STDMETHODIMP Plman::SortByFormat(UINT playlistIndex, BSTR pattern, VARIANT_BOOL selOnly, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	const string8 upattern = from_wide(pattern);
	*out = to_variant_bool(theAPI()->playlist_sort_by_format(playlistIndex, upattern.is_empty() ? nullptr : upattern, to_bool(selOnly)));
	return S_OK;
}

STDMETHODIMP Plman::SortByFormatV2(UINT playlistIndex, BSTR pattern, int direction, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	titleformat_object::ptr obj;
	const string8 upattern = from_wide(pattern);
	titleformat_compiler::get()->compile_safe(obj, upattern);
	
	metadb_handle_list items;
	theAPI()->playlist_get_all_items(playlistIndex, items);
	const size_t count = items.get_count();
	CustomSort::Order order(count);

	metadb_handle_list_helper::sort_by_format_get_order_v2(items, order.data(), obj, nullptr, direction, fb2k::noAbort);
	*out = to_variant_bool(theAPI()->playlist_reorder_items(playlistIndex, order.data(), count));
	return S_OK;
}

STDMETHODIMP Plman::SortPlaylistsByName(int direction)
{
	const uint32_t count = theAPI()->get_playlist_count();
	CustomSort::Items items(count);

	string8 str;
	str.prealloc(512);

	for (const uint32_t i : std::views::iota(0U, count))
	{
		theAPI()->playlist_get_name(i, str);
		items[i].index = i;
		items[i].text = to_wide(str);
	}

	CustomSort::Order order = CustomSort::custom_sort(items, direction);
	theAPI()->reorder(order.data(), order.size());
	return S_OK;
}

STDMETHODIMP Plman::UndoBackup(UINT playlistIndex)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	theAPI()->playlist_undo_backup(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::get_ActivePlaylist(int* out)
{
	if (!out) return E_POINTER;

	*out = to_int(theAPI()->get_active_playlist());
	return S_OK;
}

STDMETHODIMP Plman::get_PlaybackOrder(UINT* out)
{
	if (!out) return E_POINTER;

	*out = theAPI()->playback_order_get_active();
	return S_OK;
}

STDMETHODIMP Plman::get_PlayingPlaylist(int* out)
{
	if (!out) return E_POINTER;

	*out = to_int(theAPI()->get_playing_playlist());
	return S_OK;
}

STDMETHODIMP Plman::get_PlaylistCount(UINT* out)
{
	if (!out) return E_POINTER;

	*out = theAPI()->get_playlist_count();
	return S_OK;
}

STDMETHODIMP Plman::get_RecyclerCount(UINT* out)
{
	if (!out) return E_POINTER;

	*out = theAPI()->recycler_get_count();
	return S_OK;
}

STDMETHODIMP Plman::put_ActivePlaylist(UINT playlistIndex)
{
	RETURN_IF_FAILED(check_playlist_index(playlistIndex));

	theAPI()->set_active_playlist(playlistIndex);
	return S_OK;
}

STDMETHODIMP Plman::put_PlaybackOrder(UINT order)
{
	if (order >= theAPI()->playback_order_get_count()) return E_INVALIDARG;

	theAPI()->playback_order_set_active(order);
	return S_OK;
}

HRESULT Plman::check_playlist_index(uint32_t playlistIndex)
{
	if (playlistIndex >= theAPI()->get_playlist_count()) return E_INVALIDARG;
	return S_OK;
}

HRESULT Plman::check_playlist_item(uint32_t playlistIndex, uint32_t playlistItemIndex)
{
	if (playlistIndex >= theAPI()->get_playlist_count() || playlistItemIndex >= theAPI()->playlist_get_item_count(playlistIndex)) return E_INVALIDARG;
	return S_OK;
}

playlist_manager_v4::ptr Plman::theAPI()
{
	if (g_cachedAPI.is_valid()) return g_cachedAPI;
	return playlist_manager_v4::get();
}

uint32_t Plman::create_playlist(uint32_t playlistIndex, const std::wstring& name)
{
	const string8 uname = from_wide(name);

	if (uname.is_empty())
	{
		return theAPI()->create_playlist_autoname(playlistIndex);
	}
	return theAPI()->create_playlist(uname, uname.get_length(), playlistIndex);
}
