#pragma once

namespace Component
{
	static const std::string name = "JScript Panel";
	static const std::string version = "2.8.7";
	static const std::string name_version = name + " v" + version;
	static const std::string home_page = "https://github.com/marc2k3/foo_jscript_panel";
	static constexpr uint32_t version_number = 2870;

	std::string get_resource_text(int id);
	std::wstring get_path();
	std::wstring get_fb2k_path();
	std::wstring get_profile_path();
}
