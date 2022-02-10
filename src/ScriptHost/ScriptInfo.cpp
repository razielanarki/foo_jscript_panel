#include "stdafx.h"
#include "ScriptInfo.h"

ScriptInfo::ScriptInfo()
{
	if (s_replacements.empty())
	{
		s_replacements =
		{
			{ "%fb2k_profile_path%", from_wide(Component::get_profile_path()) },
			{ "%fb2k_component_path%", from_wide(Component::get_path()) },
		};
	}
}

std::string ScriptInfo::extract_value(const std::string& str)
{
	constexpr char q = '"';
	const size_t first = str.find_first_of(q);
	const size_t last = str.find_last_of(q);
	if (first < last && last < str.length())
	{
		return str.substr(first + 1, last - first - 1);
	}
	return "";
}

void ScriptInfo::add_import(const std::string& str)
{
	std::string path = extract_value(str);
	if (path.length())
	{
		for (const auto& [what, with] : s_replacements)
		{
			if (path.starts_with(what))
			{
				path = with + path.substr(what.length());
				break;
			}
		}
		m_imports.emplace_back(path);
	}
}

void ScriptInfo::update(uint32_t id, jstring str)
{
	m_imports.clear();
	m_name.clear();

	std::string source = str;
	std::string author, version;
	const size_t start = source.find("// ==PREPROCESSOR==");
	const size_t end = source.find("// ==/PREPROCESSOR==");

	if (start < end && end < source.length())
	{
		std::string pre = source.substr(start + 21, end - start - 21);

		for (const std::string& line : split_string(pre, CRLF))
		{
			if (line.contains("@name"))
			{
				m_name = extract_value(line);
			}
			else if (line.contains("@version"))
			{
				version = extract_value(line);
			}
			else if (line.contains("@author"))
			{
				author = extract_value(line);
			}
			else if (line.contains("@import"))
			{
				add_import(line);
			}
		}
	}

	if (m_name.empty()) m_name = "id:" + std::to_string(id);
	if (version.length()) version.insert(0, " v");
	if (author.length()) author.insert(0, " by ");

	m_build_string = Component::name_version + " (" + m_name + version + author + ")";
}
