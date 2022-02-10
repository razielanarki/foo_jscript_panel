#pragma once

class ContextMenuCommand
{
public:
	ContextMenuCommand(const std::wstring& command) : m_command(from_wide(command))
	{
		if (playback_control::get()->is_playing())
		{
			m_inited = true;
			m_cm = contextmenu_manager::get();
			m_cm->init_context_now_playing(contextmenu_manager::flag_view_full);
		}
	}

	ContextMenuCommand(const std::wstring& command, metadb_handle_list_cref handles) : m_command(from_wide(command))
	{
		if (handles.get_count() > 0)
		{
			m_inited = true;
			m_cm = contextmenu_manager::get();
			m_cm->init_context(handles, contextmenu_manager::flag_view_full);
		}
	}

	bool execute()
	{
		if (!m_inited) return false;
		return execute_recur(m_cm->get_root());
	}

private:
	bool execute_recur(contextmenu_node* parent, jstring parent_path = "")
	{
		const uint32_t count = parent->get_num_children();

		for (const uint32_t i : std::views::iota(0U, count))
		{
			contextmenu_node* child = parent->get_child(i);
			string8 path = parent_path;
			path.add_string(child->get_name());

			switch (child->get_type())
			{
			case contextmenu_item_node::type_group:
				path.add_char('/');
				if (execute_recur(child, path))
				{
					return true;
				}
				break;
			case contextmenu_item_node::type_command:
				if (match_command(path))
				{
					child->execute();
					return true;
				}
				break;
			}
		}
		return false;
	}

	bool match_command(jstring what)
	{
		return _stricmp(m_command, what) == 0;
	}

	bool m_inited = false;
	contextmenu_manager::ptr m_cm;
	string8 m_command;
};
