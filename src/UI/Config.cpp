#include "stdafx.h"
#include "Config.h"

static const std::vector<StringPair> init_table =
{
	{"style.default", "font:Consolas,size:11"},
	{"style.comment", "fore:#008000"},
	{"style.keyword", "bold,fore:#0000FF"},
	{"style.identifier", ""},
	{"style.string", "fore:#FF0000"},
	{"style.number", "fore:#FF0000"},
	{"style.operator", ""},
	{"style.linenumber", "font:Consolas,size:9,fore:#2B91AF"},
	{"style.bracelight", "bold,fore:#000000,back:#FFEE62"},
	{"style.bracebad", "bold,fore:#FF0000"},
	{"style.caret.fore", ""},
	{"style.selection.back", "#C0C0C0"},
};

Config g_config(guids::config);

Config::Config(const GUID& guid) : cfg_var(guid)
{
	init_data();
}

void Config::get_data_raw(stream_writer* writer, abort_callback& abort)
{
	try
	{
		writer->write_object_t(m_data.size(), abort);
		for (const auto& [name, value] : m_data)
		{
			writer->write_string(name.c_str(), abort);
			writer->write_string(value.c_str(), abort);
		}
		writer->write_object(&m_wndpl, sizeof(WINDOWPLACEMENT), abort);
	}
	catch (...) {}
}

void Config::get_window_placement(HWND hwnd)
{
	if (m_wndpl.length == 0)
	{
		memset(&m_wndpl, 0, sizeof(WINDOWPLACEMENT));
		m_wndpl.length = sizeof(WINDOWPLACEMENT);
	}
	GetWindowPlacement(hwnd, &m_wndpl);
}

void Config::init_data()
{
	m_data = init_table;
}

void Config::load(jstring content)
{
	StringMap map;
	for (const std::string& line : split_string(content, CRLF))
	{
		Strings tmp = split_string(line, "=");
		if (tmp.size() == 2)
		{
			map.emplace(tmp[0], tmp[1]);
		}
	}
	merge_map(map);
}

void Config::merge_map(const StringMap& map)
{
	for (auto& [name, value] : m_data)
	{
		const auto it = map.find(name);
		if (it != map.end())
		{
			value = it->second;
		}
		else
		{
			value.clear();
		}
	}
}

void Config::save(jstring path)
{
	std::string content;
	for (const auto& [name, value] : m_data)
	{
		content += name + "=" + value + CRLF;
	}
	FileHelper(path).write(content);
}

void Config::set_data_raw(stream_reader* reader, size_t sizehint, abort_callback& abort)
{
	try
	{
		StringMap map;
		string8 name, value;
		uint32_t count = 0;

		reader->read_object_t(count, abort);

		for (const uint32_t i : std::views::iota(0U, count))
		{
			reader->read_string(name, abort);
			reader->read_string(value, abort);
			map.emplace(name.get_ptr(), value.get_ptr());
		}

		reader->read_object(&m_wndpl, sizeof(WINDOWPLACEMENT), abort);
		merge_map(map);
	}
	catch (...)
	{
		init_data();
	}
}

void Config::set_window_placement(HWND hwnd)
{
	if (m_wndpl.length)
	{
		SetWindowPlacement(hwnd, &m_wndpl);
	}
}
