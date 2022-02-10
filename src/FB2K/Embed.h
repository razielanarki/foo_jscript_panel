#pragma once

class Embed : public threaded_process_callback
{
public:
	enum class Action
	{
		attach,
		remove,
		remove_all
	};

	Embed(Action action, metadb_handle_list_cref handles, uint32_t id = 0, const album_art_data_ptr& data = album_art_data_ptr()) : m_action(action), m_handles(handles), m_id(id), m_data(data) {}

	void run(threaded_process_status& status, abort_callback& abort) override
	{
		const GUID what = *guids::art[m_id];
		auto api = file_lock_manager::get();
		const uint32_t count = m_handles.get_count();
		std::set<string8> paths;

		for (const uint32_t i : std::views::iota(0U, count))
		{
			const string8 path = m_handles[i]->get_path();
			if (!paths.emplace(path).second) continue;

			status.set_progress(i + 1, count);
			status.set_item_path(path);
			album_art_editor::ptr ptr;
			if (album_art_editor::g_get_interface(ptr, path))
			{
				try
				{
					auto lock = api->acquire_write(path, abort);
					album_art_editor_instance_ptr aaep = ptr->open(nullptr, path, abort);
					switch (m_action)
					{
					case Action::attach:
						aaep->set(what, m_data, abort);
						break;
					case Action::remove:
						aaep->remove(what);
						break;
					case Action::remove_all:
						aaep->remove_all_();
						break;
					}
					aaep->commit(abort);
				}
				catch (...) {}
			}
		}
	}

private:
	Action m_action;
	album_art_data_ptr m_data;
	metadb_handle_list m_handles;
	uint32_t m_id = 0;
};
