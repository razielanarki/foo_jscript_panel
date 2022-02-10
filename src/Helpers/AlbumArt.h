#pragma once
#include "ImageHelpers.h"

namespace AlbumArt
{
	static bool check_id(uint32_t id)
	{
		return id < guids::art.size();
	}

	static IGdiBitmap* data_to_bitmap(const album_art_data_ptr& data)
	{
		if (data.is_valid())
		{
			const uint8_t* ptr = static_cast<const uint8_t*>(data->get_ptr());
			const size_t bytes = data->get_size();

			UniqueBitmap bitmap;
			bool ok = ImageHelpers::libwebp_data_to_bitmap(ptr, bytes, bitmap);

			if (!ok)
			{
				wil::com_ptr_t<IStream> stream;
				stream.attach(SHCreateMemStream(ptr, bytes));
				ok = stream && ImageHelpers::gdiplus_istream_to_bitmap(stream.get(), bitmap);
			}
			if (ok) return new ComObjectImpl<GdiBitmap>(std::move(bitmap));
		}
		return nullptr;
	}

	static album_art_data_ptr file_to_data(const std::wstring& path)
	{
		album_art_data_ptr data;
		wil::com_ptr_t<IStream> stream;
		if (SUCCEEDED(SHCreateStreamOnFileEx(path.data(), STGM_READ | STGM_SHARE_DENY_WRITE, GENERIC_READ, FALSE, nullptr, &stream)))
		{
			ImageData image_data;
			if (ImageHelpers::istream_to_image_data(stream.get(), image_data))
			{
				data = album_art_data_impl::g_create(image_data.data(), image_data.size());
			}
		}
		return data;
	}

	static album_art_data_ptr get(const metadb_handle_ptr& handle, uint32_t id, bool need_stub, pfc::string_base& image_path)
	{
		auto update_image_path = [&image_path](const album_art_path_list::ptr& pathlist)
		{
			if (pathlist.is_valid() && pathlist->get_count() > 0)
			{
				filesystem::g_get_display_path(pathlist->get_path(0), image_path);
			}
		};

		const std::string path = handle->get_path();

		// get stream art if relevant conditions are met
		if (id == 0 && !path.starts_with("file://") && path == get_now_playing_path())
		{
			// tryGet because this is only available in fb2k v1.6.6 and later
			auto api = now_playing_album_art_notify_manager_v2::tryGet(); 
			if (api.is_valid())
			{
				auto info = api->current_v2();
				if (info)
				{
					update_image_path(info.paths);
					return info.data;
				}
			}
		}

		const GUID what = *guids::art[id];
		album_art_data_ptr data;
		album_art_extractor_instance_v2::ptr ptr;
		album_art_path_list::ptr pathlist;
		auto api = album_art_manager_v2::get();

		try
		{
			ptr = api->open(pfc::list_single_ref_t<metadb_handle_ptr>(handle), pfc::list_single_ref_t<GUID>(what), fb2k::noAbort);
			data = ptr->query(what, fb2k::noAbort);
			pathlist = ptr->query_paths(what, fb2k::noAbort);
		}
		catch (...)
		{
			if (need_stub)
			{
				try
				{
					ptr = api->open_stub(fb2k::noAbort);
					data = ptr->query(what, fb2k::noAbort);
					pathlist = ptr->query_paths(what, fb2k::noAbort);
				}
				catch (...) {}
			}
		}

		update_image_path(pathlist);
		return data;
	}

	static album_art_data_ptr get_embedded(jstring path, uint32_t id)
	{
		album_art_data_ptr data;
		album_art_extractor::ptr ptr;
		if (album_art_extractor::g_get_interface(ptr, path))
		{
			try
			{
				data = ptr->open(nullptr, path, fb2k::noAbort)->query(*guids::art[id], fb2k::noAbort);
			}
			catch (...) {}
		}
		return data;
	}
}
