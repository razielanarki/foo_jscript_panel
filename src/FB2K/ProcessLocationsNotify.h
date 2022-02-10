#pragma once

class ProcessLocationsNotifyCallback : public process_locations_notify
{
public:
	ProcessLocationsNotifyCallback(CWindow hwnd, int task_id) : m_hwnd(hwnd), m_task_id(task_id) {}

	void on_aborted() override {}

	void on_completion(metadb_handle_list_cref handles) override
	{
		if (m_hwnd.IsWindow())
		{
			auto data = new MetadbCallbackData(handles);
			data->refcount_add_ref();
			m_hwnd.PostMessageW(std::to_underlying(CallbackID::on_locations_added), reinterpret_cast<WPARAM>(data), m_task_id);
		}
	}

private:
	CWindow m_hwnd;
	int m_task_id = 0;
};

class ProcessLocationsNotifyPlaylist : public process_locations_notify
{
public:
	ProcessLocationsNotifyPlaylist(uint32_t playlist, uint32_t base, bool to_select) : m_playlist(playlist), m_base(base), m_to_select(to_select) {}

	void on_aborted() override {}

	void on_completion(metadb_handle_list_cref handles) override
	{
		const uint32_t mask = Plman::theAPI()->playlist_lock_get_filter_mask(m_playlist);

		if (m_playlist < Plman::theAPI()->get_playlist_count() && !(mask & playlist_lock::filter_add))
		{
			Plman::theAPI()->playlist_insert_items(m_playlist, m_base, handles, pfc::bit_array_val(m_to_select));
			if (m_to_select)
			{
				Plman::theAPI()->set_active_playlist(m_playlist);
				Plman::theAPI()->playlist_set_focus_item(m_playlist, m_base);
			}
		}
	}

private:
	bool m_to_select = true;
	uint32_t m_base = 0, m_playlist = 0;
};
