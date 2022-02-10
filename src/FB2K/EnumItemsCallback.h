#pragma once

class EnumItemsCallback : public playlist_manager::enum_items_callback
{
public:
	EnumItemsCallback(uint32_t new_pos) : m_new_pos(new_pos) {}

	bool on_item(uint32_t index, const metadb_handle_ptr&, bool b_selected) override
	{
		if (b_selected)
		{
			if (index < m_new_pos) m_selected_before++;
			m_selected.emplace_back(index);
		}
		else
		{
			m_not_selected.emplace_back(index);
		}
		return true;
	}

	bool reorder(uint32_t playlistIndex)
	{
		m_new_pos = std::min(m_new_pos - m_selected_before, m_not_selected.size());
		std::vector<uint32_t> new_order;
		std::ranges::copy(m_not_selected | std::views::take(m_new_pos), std::back_inserter(new_order));
		std::ranges::copy(m_selected, std::back_inserter(new_order));
		std::ranges::copy(m_not_selected | std::views::drop(m_new_pos), std::back_inserter(new_order));
		return Plman::theAPI()->playlist_reorder_items(playlistIndex, new_order.data(), new_order.size());
	}

private:
	std::vector<uint32_t> m_selected, m_not_selected;
	uint32_t m_new_pos = 0, m_selected_before = 0;
};
