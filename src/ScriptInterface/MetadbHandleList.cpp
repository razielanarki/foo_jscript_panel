#include "stdafx.h"
#include "MetadbHandleList.h"
#include "db.h"

#include "AlbumArt.h"
#include "CustomSort.h"
#include "Embed.h"
#include "FileInfoFilter.h"
#include "OptimiseLayout.h"
#include "TrackPropertyCallback.h"

#include <random>
#include <foobar2000/helpers/metadb_handle_set.h>

MetadbHandleList::MetadbHandleList(metadb_handle_list_cref handles) : m_handles(handles) {}

STDMETHODIMP MetadbHandleList::get(void** out)
{
	if (!out) return E_POINTER;

	*out = &m_handles;
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Add(IMetadbHandle* handle)
{
	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	m_handles.add_item(handle_ptr);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::AddRange(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	m_handles.add_items(*handles_ptr);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::AttachImage(BSTR path, UINT art_id)
{
	if (m_handles.get_count() == 0) return E_POINTER;
	if (!AlbumArt::check_id(art_id)) return E_INVALIDARG;

	album_art_data_ptr data = AlbumArt::file_to_data(path);
	if (data.is_valid())
	{
		using enum Embed::Action;
		auto cb = fb2k::service_new<Embed>(attach, m_handles, art_id, data);
		threaded_process::get()->run_modeless(cb, flags, core_api::get_main_window(), "Embedding image...");
	}
	return S_OK;
}

STDMETHODIMP MetadbHandleList::BSearch(IMetadbHandle* handle, int* out)
{
	if (!out) return E_POINTER;

	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	m_handles.sort_by_pointer_remove_duplicates();
	*out = to_int(m_handles.bsearch_by_pointer(handle_ptr));
	return S_OK;
}

STDMETHODIMP MetadbHandleList::CalcTotalDuration(double* out)
{
	if (!out) return E_POINTER;

	*out = m_handles.calc_total_duration();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::CalcTotalSize(UINT64* out)
{
	if (!out) return E_POINTER;

	*out = metadb_handle_list_helper::calc_total_size(m_handles, true);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Clone(IMetadbHandleList** out)
{
	if (!out) return E_POINTER;

	*out = new ComObjectImpl<MetadbHandleList>(m_handles);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Convert(VARIANT* out)
{
	if (!out) return E_POINTER;

	ComArrayWriter writer(m_handles.get_count());

	for (const metadb_handle_ptr& handle : m_handles)
	{
		_variant_t var = new ComObjectImpl<MetadbHandle>(handle);
		RETURN_IF_FAILED(writer.add_item(var));
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.data();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Find(IMetadbHandle* handle, int* out)
{
	if (!out) return E_POINTER;

	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	*out = to_int(m_handles.find_item(handle_ptr));
	return S_OK;
}

STDMETHODIMP MetadbHandleList::GetLibraryRelativePaths(VARIANT* out)
{
	if (!out) return E_POINTER;

	auto api = library_manager::get();

	string8 str;
	str.prealloc(512);

	ComArrayWriter writer(m_handles.get_count());

	for (const metadb_handle_ptr& handle : m_handles)
	{
		if (!api->get_relative_path(handle, str)) str.reset();
		RETURN_IF_FAILED(writer.add_item(str));
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.data();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::GetOtherInfo(BSTR* out)
{
	if (!out) return E_POINTER;

	TrackPropertyCallback callback;

	for (auto e = service_enum_t<track_property_provider>(); !e.finished(); ++e)
	{
		try
		{
			e.get()->enumerate_properties_helper(m_handles, nullptr, callback, fb2k::noAbort);
		}
		catch (...) {}
	}

	json j = callback.m_data;
	*out = to_bstr(j.dump());
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Insert(UINT index, IMetadbHandle* handle)
{
	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	m_handles.insert_item(handle_ptr, index);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::InsertRange(UINT index, IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	m_handles.insert_items(*handles_ptr, index);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::MakeDifference(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	metadb_handle_list two(*handles_ptr);
	m_handles.sort_by_pointer_remove_duplicates();
	two.sort_by_pointer_remove_duplicates();

	metadb_handle_list r1, r2;
	metadb_handle_list_helper::sorted_by_pointer_extract_difference(m_handles, two, r1, r2);
	m_handles = r1;
	return S_OK;
}

STDMETHODIMP MetadbHandleList::MakeIntersection(IMetadbHandleList* handles)
{
	metadb_handle_list* handles_ptr = nullptr;
	RETURN_IF_FAILED(handles->get(arg_helper(&handles_ptr)));

	metadb_handle_list two(*handles_ptr);
	m_handles.sort_by_pointer_remove_duplicates();
	two.sort_by_pointer_remove_duplicates();

	metadb_handle_list result;
	size_t walk1 = 0;
	size_t walk2 = 0;
	const size_t last1 = m_handles.get_count();
	const size_t last2 = two.get_count();

	while (walk1 != last1 && walk2 != last2)
	{
		if (m_handles[walk1] < two[walk2])
			++walk1;
		else if (two[walk2] < m_handles[walk1])
			++walk2;
		else
		{
			result.add_item(m_handles[walk1]);
			++walk1;
			++walk2;
		}
	}

	m_handles = result;
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OptimiseFileLayout(VARIANT_BOOL minimise)
{
	if (m_handles.get_count() == 0) return E_POINTER;

	auto cb = fb2k::service_new<OptimiseLayout>(m_handles, to_bool(minimise));
	threaded_process::get()->run_modeless(cb, flags | threaded_process::flag_show_abort, core_api::get_main_window(), "Optimising...");
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OrderByFormat(ITitleFormat* script, int direction)
{
	titleformat_object* obj = nullptr;
	RETURN_IF_FAILED(script->get(arg_helper(&obj)));

	m_handles.sort_by_format(obj, nullptr, direction);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OrderByPath()
{
	m_handles.sort_by_path();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::OrderByRelativePath()
{
	auto api = library_manager::get();
	const uint32_t count = m_handles.get_count();
	CustomSort::Items items(count);

	string8 str;
	str.prealloc(512);

	for (const uint32_t i : std::views::iota(0U, count))
	{
		metadb_handle_ptr item = m_handles[i];
		if (!api->get_relative_path(item, str)) str.reset();
		str << item->get_subsong_index();
		items[i].index = i;
		items[i].text = CustomSort::make_sort_string(str);
	}

	CustomSort::Order order = CustomSort::custom_sort(items);
	m_handles.reorder(order.data());
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Randomise()
{
	const size_t count = m_handles.get_count();
	CustomSort::Order order(count);
	std::iota(order.begin(), order.end(), 0U);

	std::random_device rd;
	std::mt19937 g(rd());
	std::ranges::shuffle(order, g);
	m_handles.reorder(order.data());
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RefreshStats()
{
	db::refresh(m_handles);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::Remove(IMetadbHandle* handle)
{
	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	m_handles.remove_item(handle_ptr);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveAll()
{
	m_handles.remove_all();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveAttachedImage(UINT art_id)
{
	if (m_handles.get_count() == 0) return E_POINTER;
	if (!AlbumArt::check_id(art_id)) return E_INVALIDARG;

	using enum Embed::Action;
	auto cb = fb2k::service_new<Embed>(remove, m_handles, art_id);
	threaded_process::get()->run_modeless(cb, flags, core_api::get_main_window(), "Removing images...");
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveAttachedImages()
{
	if (m_handles.get_count() == 0) return E_POINTER;

	using enum Embed::Action;
	auto cb = fb2k::service_new<Embed>(remove_all, m_handles);
	threaded_process::get()->run_modeless(cb, flags, core_api::get_main_window(), "Removing images...");
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveById(UINT index)
{
	if (index >= m_handles.get_count()) return E_INVALIDARG;

	m_handles.remove_by_idx(index);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveDuplicates()
{
	metadb_handle_set set;
	const uint32_t count = m_handles.get_count();
	pfc::bit_array_bittable mask(count);

	for (const uint32_t i : std::views::iota(0U, count))
	{
		mask.set(i, set.add_item_check(m_handles[i]));
	}

	set.remove_all();
	m_handles.filter_mask(mask);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveDuplicatesByFormat(ITitleFormat* script)
{
	titleformat_object* obj = nullptr;
	RETURN_IF_FAILED(script->get(arg_helper(&obj)));

	std::set<string8> set;
	const uint32_t count = m_handles.get_count();
	pfc::bit_array_bittable mask(count);

	for (const uint32_t i : std::views::iota(0U, count))
	{
		string8 str;
		m_handles[i]->format_title(nullptr, str, obj, nullptr);
		mask.set(i, set.emplace(str).second);
	}

	m_handles.filter_mask(mask);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::RemoveRange(UINT from, UINT count)
{
	m_handles.remove_from_idx(from, count);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::SaveAs(BSTR path)
{
	try
	{
		const string8 upath = from_wide(path);
		playlist_loader::g_save_playlist(upath, m_handles, fb2k::noAbort);
	}
	catch (...) {}

	return S_OK;
}

STDMETHODIMP MetadbHandleList::UpdateFileInfoFromJSON(BSTR str)
{
	const uint32_t count = m_handles.get_count();
	if (count == 0) return E_POINTER;

	json j = json::parse(from_wide(str), nullptr, false);

	if (j.is_array() && j.size() == count)
	{
		std::vector<file_info_impl> info(count);

		for (const uint32_t i : std::views::iota(0U, count))
		{
			if (!j[i].is_object() || j[i].size() == 0) return E_INVALIDARG;

			info[i] = m_handles[i]->get_info_ref()->info();

			for (const auto& [name, value] : j[i].items())
			{
				if (name.empty()) return E_INVALIDARG;

				info[i].meta_remove_field(name.c_str());

				for (const std::string& v : strings_from_json(value))
				{
					info[i].meta_add(name.c_str(), v.c_str());
				}
			}
		}

		metadb_io_v2::get()->update_info_async_simple(
			m_handles,
			pfc::ptr_list_const_array_t<const file_info, file_info_impl*>(info.data(), info.size()),
			core_api::get_main_window(),
			metadb_io_v2::op_flag_delay_ui,
			nullptr
		);
		return S_OK;
	}
	else if (j.is_object() && j.size() > 0)
	{
		FileInfoFilter::Tags tags;

		for (const auto& [name, value] : j.items())
		{
			if (name.empty()) return E_INVALIDARG;

			FileInfoFilter::Tag tag;
			tag.name = name;
			tag.values = strings_from_json(value);
			tags.emplace_back(tag);
		}

		metadb_io_v2::get()->update_info_async(
			m_handles,
			fb2k::service_new<FileInfoFilter>(tags),
			core_api::get_main_window(),
			metadb_io_v2::op_flag_delay_ui,
			nullptr
		);
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP MetadbHandleList::get_Count(UINT* out)
{
	if (!out) return E_POINTER;

	*out = m_handles.get_count();
	return S_OK;
}

STDMETHODIMP MetadbHandleList::get_Item(UINT index, IMetadbHandle** out)
{
	if (!out) return E_POINTER;
	if (index >= m_handles.get_count()) return E_INVALIDARG;

	*out = new ComObjectImpl<MetadbHandle>(m_handles[index]);
	return S_OK;
}

STDMETHODIMP MetadbHandleList::put_Item(UINT index, IMetadbHandle* handle)
{
	if (index >= m_handles.get_count()) return E_INVALIDARG;

	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	m_handles.replace_item(index, handle_ptr);
	return S_OK;
}

void MetadbHandleList::FinalRelease()
{
	m_handles.remove_all();
}
