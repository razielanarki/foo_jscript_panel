#pragma once

class ScriptInfo
{
public:
	ScriptInfo();

	void update(uint32_t id, jstring str);

	Strings m_imports;
	std::string m_build_string, m_name;

private:
	std::string extract_value(const std::string& str);
	void add_import(const std::string& str);

	inline static std::vector<StringPair> s_replacements;
};
