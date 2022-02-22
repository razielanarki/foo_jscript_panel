#pragma once

class OptimiseLayout : public threaded_process_callback
{
public:
	OptimiseLayout(metadb_handle_list_cref handles, bool minimise) : m_handles(handles), m_minimise(minimise) {}

	void run(threaded_process_status& status, abort_callback& abort) override
	{
		auto api = file_lock_manager::get();
		const uint32_t count = m_handles.get_count();
		std::set<string8> paths;

		for (const uint32_t i : std::views::iota(0U, count))
		{
			abort.check();

			const string8 path = m_handles[i]->get_path();
			if (!paths.emplace(path).second) continue;

			status.set_progress(i + 1, count);
			status.set_item_path(path);

			try
			{
				auto lock = api->acquire_write(path, abort);
				for (auto e = service_enum_t<file_format_sanitizer>(); !e.finished(); ++e)
				{
					if (e.get()->sanitize_file(path, m_minimise, abort)) break;
				}
			}
			catch (...) {}
		}
	}

private:
	bool m_minimise = false;
	metadb_handle_list m_handles;
};
