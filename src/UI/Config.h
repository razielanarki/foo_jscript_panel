#pragma once

class Config : public cfg_var
{
public:
	Config(const GUID& guid);

	void get_data_raw(stream_writer* writer, abort_callback& abort) override;
	void get_window_placement(HWND hwnd);
	void init_data();
	void load(jstring content);
	void merge_map(const StringMap& map);
	void save(jstring path);
	void set_data_raw(stream_reader* reader, size_t, abort_callback& abort) override;
	void set_window_placement(HWND hwnd);

	WINDOWPLACEMENT m_wndpl{};
	std::vector<StringPair> m_data;
};

extern Config g_config;
