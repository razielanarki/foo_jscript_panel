#include "stdafx.h"
#include "Fb.h"
#include "ContextMenuCommand.h"
#include "DropSourceImpl.h"
#include "MainMenuCommand.h"
#include "ProcessLocationsNotify.h"

STDMETHODIMP Fb::AcquireUiSelectionHolder(ISelectionHolder** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<SelectionHolder>();
	return S_OK;
}

STDMETHODIMP Fb::AddDirectory()
{
	standard_commands::main_add_directory();
	return S_OK;
}

STDMETHODIMP Fb::AddFiles()
{
	standard_commands::main_add_files();
	return S_OK;
}

STDMETHODIMP Fb::AddLocationsAsync(UINT window_id, VARIANT locations, UINT* out)
{
	if (!out) return E_POINTER;

	pfc::string_list_impl list;
	ComArrayReader reader;
	RETURN_IF_FAILED(reader.convert(locations, list));

	constexpr uint32_t flags = playlist_incoming_item_filter_v2::op_flag_no_filter | playlist_incoming_item_filter_v2::op_flag_delay_ui;
	auto obj = fb2k::service_new<ProcessLocationsNotifyCallback>(to_hwnd(window_id), ++m_task_id);
	playlist_incoming_item_filter_v2::get()->process_locations_async(list, flags, nullptr, nullptr, nullptr, obj);
	*out = m_task_id;
	return S_OK;
}

STDMETHODIMP Fb::CheckClipboardContents(UINT /* FFS */, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = VARIANT_FALSE;

	wil::com_ptr_t<IDataObject> obj;
	if (SUCCEEDED(OleGetClipboard(&obj)))
	{
		bool native;
		DWORD drop_effect = DROPEFFECT_COPY;
		*out = to_variant_bool(SUCCEEDED(ole_interaction::get()->check_dataobject(obj.get(), drop_effect, native)));
	}
	return S_OK;
}

STDMETHODIMP Fb::ClearPlaylist()
{
	standard_commands::main_clear_playlist();
	return S_OK;
}

STDMETHODIMP Fb::CopyHandleListToClipboard(IMetadbHandleList* handles, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	auto obj = ole_interaction::get()->create_dataobject(*handles_ptr);
	*out = to_variant_bool(SUCCEEDED(OleSetClipboard(obj.get_ptr())));
	return S_OK;
}

STDMETHODIMP Fb::CreateContextMenuManager(IContextMenuManager** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<ContextMenuManager>();
	return S_OK;
}

STDMETHODIMP Fb::CreateHandleList(VARIANT handle, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	IMetadbHandle* temp = nullptr;
	metadb_handle_list items;

	if (handle.vt == VT_DISPATCH && handle.pdispVal && SUCCEEDED(handle.pdispVal->QueryInterface(__uuidof(IMetadbHandle), reinterpret_cast<void**>(&temp))))
	{
		metadb_handle* handle_ptr = nullptr;
		RETURN_IF_FAILED(temp->get(arg_helper(&handle_ptr)));
		items.add_item(handle_ptr);
	}
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::CreateMainMenuManager(IMainMenuManager** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<MainMenuManager>();
	return S_OK;
}

STDMETHODIMP Fb::CreateProfiler(BSTR name, IProfiler** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<Profiler>(name);
	return S_OK;
}

STDMETHODIMP Fb::DoDragDrop(IMetadbHandleList* handles, UINT okEffects, UINT* out)
{
	if (!out) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	*out = DROPEFFECT_NONE;

	if (handles_ptr->get_count() && okEffects != DROPEFFECT_NONE)
	{
		auto obj = ole_interaction::get()->create_dataobject(*handles_ptr);
		wil::com_ptr_t<DropSourceImpl> source = new DropSourceImpl();

		DWORD returnEffect;
		if (::DoDragDrop(obj.get_ptr(), source.get(), okEffects, &returnEffect) != DRAGDROP_S_CANCEL)
		{
			*out = returnEffect;
		}
	}
	return S_OK;
}

STDMETHODIMP Fb::Exit()
{
	standard_commands::main_exit();
	return S_OK;
}

STDMETHODIMP Fb::GetClipboardContents(UINT /* FFS */, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	auto api = ole_interaction::get();
	wil::com_ptr_t<IDataObject> obj;
	metadb_handle_list items;

	if (SUCCEEDED(OleGetClipboard(&obj)))
	{
		DWORD drop_effect = DROPEFFECT_COPY;
		bool native;
		if (SUCCEEDED(api->check_dataobject(obj.get(), drop_effect, native)))
		{
			dropped_files_data_impl data;
			if (SUCCEEDED(api->parse_dataobject(obj.get(), data)))
			{
				data.to_handles(items, native, core_api::get_main_window());
			}
		}
	}

	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::GetDSPPresets(BSTR* out)
{
	if (!out) return E_POINTER;

	json j = json::array();
	auto api = dsp_config_manager_v2::get();
	const uint32_t count = api->get_preset_count();
	string8 name;

	for (const uint32_t i : std::views::iota(0U, count))
	{
		api->get_preset_name(i, name);

		j.push_back(
			{
				{ "active", api->get_selected_preset() == i },
				{ "name", name.get_ptr() }
			});
	}

	*out = to_bstr(j.dump());
	return S_OK;
}

STDMETHODIMP Fb::GetFocusItem(IMetadbHandle** out)
{
	if (!out) return E_POINTER;

	metadb_handle_ptr metadb;
	*out = Plman::theAPI()->activeplaylist_get_focus_item_handle(metadb) ? new ComObjectImpl<MetadbHandle>(metadb) : nullptr;
	return S_OK;
}

STDMETHODIMP Fb::GetLibraryItems(IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	library_manager::get()->get_all_items(items);
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::GetNowPlaying(IMetadbHandle** out)
{
	if (!out) return E_POINTER;

	metadb_handle_ptr metadb;
	*out = playback_control::get()->get_now_playing(metadb) ? new ComObjectImpl<MetadbHandle>(metadb) : nullptr;
	return S_OK;
}

STDMETHODIMP Fb::GetOutputDevices(BSTR* out)
{
	if (!out) return E_POINTER;

	json j = json::array();
	auto api = output_manager_v2::get();
	outputCoreConfig_t config;
	api->getCoreConfig(config);

	api->listDevices([&j, config](auto&& name, auto&& output_id, auto&& device_id)
		{
			j.push_back(
				{
					{ "name", name },
					{ "output_id", GuidHelper::print(output_id) },
					{ "device_id", GuidHelper::print(device_id) },
					{ "active", config.m_output == output_id && config.m_device == device_id }
				});
		});
	*out = to_bstr(j.dump());
	return S_OK;
}

STDMETHODIMP Fb::GetQueryItems(IMetadbHandleList* handles, BSTR query, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	const string8 uquery = from_wide(query);
	search_filter_v2::ptr filter;

	try
	{
		filter = search_filter_manager_v2::get()->create_ex(uquery, fb2k::service_new<completion_notify_dummy>(), search_filter_manager_v2::KFlagSuppressNotify);
	}
	catch (...) {}

	if (filter.is_empty()) return E_INVALIDARG;

	metadb_handle_list copy(*handles_ptr);
	pfc::array_t<bool> mask;
	mask.set_size(copy.get_count());
	filter->test_multi(copy, mask.get_ptr());
	copy.filter_mask(mask.get_ptr());
	*out = new ComObjectImpl<MetadbHandleList>(copy);
	return S_OK;
}

STDMETHODIMP Fb::GetSelection(IMetadbHandle** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	ui_selection_manager::get()->get_selection(items);
	*out = items.get_count() ? new ComObjectImpl<MetadbHandle>(items[0]) : nullptr;
	return S_OK;
}

STDMETHODIMP Fb::GetSelections(UINT flags, IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	metadb_handle_list items;
	ui_selection_manager_v2::get()->get_selection(items, flags);
	*out = new ComObjectImpl<MetadbHandleList>(items);
	return S_OK;
}

STDMETHODIMP Fb::GetSelectionType(UINT* out)
{
	if (!out) return E_POINTER;

	const GUID type = ui_selection_manager_v2::get()->get_selection_type(0);
	const auto it = std::ranges::find_if(guids::selections, [type](const GUID* g) { return *g == type; });
	*out = std::ranges::distance(guids::selections.begin(), it);
	return S_OK;
}

STDMETHODIMP Fb::IsLibraryEnabled(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(library_manager::get()->is_library_enabled());
	return S_OK;
}

STDMETHODIMP Fb::IsMetadbInMediaLibrary(IMetadbHandle* handle, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	*out = to_variant_bool(library_manager::get()->is_item_in_library(handle_ptr));
	return S_OK;
}

STDMETHODIMP Fb::LoadPlaylist()
{
	standard_commands::main_load_playlist();
	return S_OK;
}

STDMETHODIMP Fb::Next()
{
	standard_commands::main_next();
	return S_OK;
}

STDMETHODIMP Fb::Pause()
{
	standard_commands::main_pause();
	return S_OK;
}

STDMETHODIMP Fb::Play()
{
	standard_commands::main_play();
	return S_OK;
}

STDMETHODIMP Fb::PlayOrPause()
{
	standard_commands::main_play_or_pause();
	return S_OK;
}

STDMETHODIMP Fb::Prev()
{
	standard_commands::main_previous();
	return S_OK;
}

STDMETHODIMP Fb::Random()
{
	standard_commands::main_random();
	return S_OK;
}

STDMETHODIMP Fb::RunContextCommand(BSTR command, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(ContextMenuCommand(command).execute());
	return S_OK;
}

STDMETHODIMP Fb::RunContextCommandWithMetadb(BSTR command, VARIANT handle_or_handles, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;
	if (handle_or_handles.vt != VT_DISPATCH || !handle_or_handles.pdispVal) return E_INVALIDARG;

	IMetadbHandle* temp1 = nullptr;
	IMetadbHandleList* temp2 = nullptr;
	metadb_handle_list items;

	if (SUCCEEDED(handle_or_handles.pdispVal->QueryInterface(__uuidof(IMetadbHandle), reinterpret_cast<void**>(&temp1))))
	{
		metadb_handle* handle_ptr = nullptr;
		RETURN_IF_FAILED(temp1->get(arg_helper(&handle_ptr)));
		items.add_item(handle_ptr);
	}
	else if (SUCCEEDED(handle_or_handles.pdispVal->QueryInterface(__uuidof(IMetadbHandleList), reinterpret_cast<void**>(&temp2))))
	{
		metadb_handle_list* handles_ptr = nullptr;
		RETURN_IF_FAILED(temp2->get(arg_helper(&handles_ptr)));
		items = *handles_ptr;
	}

	if (items.get_count() == 0) return E_INVALIDARG;
	*out = to_variant_bool(ContextMenuCommand(command, items).execute());
	return S_OK;
}

STDMETHODIMP Fb::RunMainMenuCommand(BSTR command, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(MainMenuCommand(command).execute());
	return S_OK;
}

STDMETHODIMP Fb::SavePlaylist()
{
	standard_commands::main_save_playlist();
	return S_OK;
}

STDMETHODIMP Fb::SetDSPPreset(UINT idx)
{
	auto api = dsp_config_manager_v2::get();
	const size_t count = api->get_preset_count();
	if (idx >= count) return E_INVALIDARG;

	api->select_preset(idx);
	return S_OK;
}

STDMETHODIMP Fb::SetOutputDevice(BSTR output, BSTR device)
{
	GUID output_id, device_id;
	if (CLSIDFromString(output, &output_id) == NOERROR && CLSIDFromString(device, &device_id) == NOERROR)
	{
		output_manager_v2::get()->setCoreConfigDevice(output_id, device_id);
	}
	return S_OK;
}

STDMETHODIMP Fb::ShowConsole()
{
	standard_commands::main_show_console();
	return S_OK;
}

STDMETHODIMP Fb::ShowLibrarySearchUI(BSTR query)
{
	const string8 uquery = from_wide(query);
	library_search_ui::get()->show(uquery);
	return S_OK;
}

STDMETHODIMP Fb::ShowPopupMessage(BSTR msg, BSTR title)
{
	const string8 umsg = from_wide(msg);
	const string8 utitle = from_wide(title);
	fb2k::inMainThread([umsg, utitle]()
		{
			popup_message::g_show(umsg, utitle);
		});
	return S_OK;
}

STDMETHODIMP Fb::ShowPreferences()
{
	standard_commands::main_preferences();
	return S_OK;
}

STDMETHODIMP Fb::Stop()
{
	standard_commands::main_stop();
	return S_OK;
}

STDMETHODIMP Fb::TitleFormat(BSTR pattern, ITitleFormat** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<::TitleFormat>(pattern);
	return S_OK;
}

STDMETHODIMP Fb::VolumeDown()
{
	standard_commands::main_volume_down();
	return S_OK;
}

STDMETHODIMP Fb::VolumeMute()
{
	standard_commands::main_volume_mute();
	return S_OK;
}

STDMETHODIMP Fb::VolumeUp()
{
	standard_commands::main_volume_up();
	return S_OK;
}

STDMETHODIMP Fb::get_AlwaysOnTop(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(config_object::g_get_data_bool_simple(standard_config_objects::bool_ui_always_on_top, false));
	return S_OK;
}

STDMETHODIMP Fb::get_ComponentPath(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = SysAllocString(Component::get_path().data());
	return S_OK;
}

STDMETHODIMP Fb::get_CursorFollowPlayback(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(config_object::g_get_data_bool_simple(standard_config_objects::bool_cursor_follows_playback, false));
	return S_OK;
}

STDMETHODIMP Fb::get_FoobarPath(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = SysAllocString(Component::get_fb2k_path().data());
	return S_OK;
}

STDMETHODIMP Fb::get_IsPaused(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(playback_control::get()->is_paused());
	return S_OK;
}

STDMETHODIMP Fb::get_IsPlaying(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(playback_control::get()->is_playing());
	return S_OK;
}

STDMETHODIMP Fb::get_PlaybackFollowCursor(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(config_object::g_get_data_bool_simple(standard_config_objects::bool_playback_follows_cursor, false));
	return S_OK;
}

STDMETHODIMP Fb::get_PlaybackLength(double* out)
{
	if (!out) return E_POINTER;

	*out = playback_control::get()->playback_get_length();
	return S_OK;
}

STDMETHODIMP Fb::get_PlaybackTime(double* out)
{
	if (!out) return E_POINTER;

	*out = playback_control::get()->playback_get_position();
	return S_OK;
}

STDMETHODIMP Fb::get_ProfilePath(BSTR* out)
{
	if (!out) return E_POINTER;

	*out = SysAllocString(Component::get_profile_path().data());
	return S_OK;
}

STDMETHODIMP Fb::get_ReplaygainMode(UINT* out)
{
	if (!out) return E_POINTER;

	t_replaygain_config rg;
	replaygain_manager::get()->get_core_settings(rg);
	*out = rg.m_source_mode;
	return S_OK;
}

STDMETHODIMP Fb::get_StopAfterCurrent(VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(playback_control::get()->get_stop_after_current());
	return S_OK;
}

STDMETHODIMP Fb::get_Volume(float* out)
{
	if (!out) return E_POINTER;

	*out = playback_control::get()->get_volume();
	return S_OK;
}

STDMETHODIMP Fb::put_AlwaysOnTop(VARIANT_BOOL b)
{
	config_object::g_set_data_bool(standard_config_objects::bool_ui_always_on_top, to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_CursorFollowPlayback(VARIANT_BOOL b)
{
	config_object::g_set_data_bool(standard_config_objects::bool_cursor_follows_playback, to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_PlaybackFollowCursor(VARIANT_BOOL b)
{
	config_object::g_set_data_bool(standard_config_objects::bool_playback_follows_cursor, to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_PlaybackTime(double time)
{
	playback_control::get()->playback_seek(time);
	return S_OK;
}

STDMETHODIMP Fb::put_ReplaygainMode(UINT mode)
{
	if (mode >= guids::rg_modes.size()) return E_INVALIDARG;

	standard_commands::run_main(*guids::rg_modes[mode]);
	playback_control_v3::get()->restart();
	return S_OK;
}

STDMETHODIMP Fb::put_StopAfterCurrent(VARIANT_BOOL b)
{
	playback_control::get()->set_stop_after_current(to_bool(b));
	return S_OK;
}

STDMETHODIMP Fb::put_Volume(float value)
{
	playback_control::get()->set_volume(value);
	return S_OK;
}
