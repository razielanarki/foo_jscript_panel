#pragma once

using StringMap = std::map<std::string, std::string>;
using StringPair = std::pair<std::string, std::string>;
using Strings = std::vector<std::string>;
using WStrings = std::vector<std::wstring>;

static constexpr const char* CRLF = "\r\n";

class jstring
{
public:
	jstring(const pfc::string_base& s) : m_ptr(s) {}
	jstring(const std::string& s) : m_ptr(s.c_str()) {}
	jstring(const char* s) : m_ptr(s) {}

	operator string8() const { return m_ptr; }
	operator std::string() const { return m_ptr; }
	operator const char* () const { return m_ptr; }

	const char* get() const { return m_ptr; }
	size_t length() const { return strlen(m_ptr); }

private:
	const char* const m_ptr;
};

static Strings split_string(const std::string& text, const std::string& delims)
{
	auto view = std::views::split(text, delims);
	return Strings(view.begin(), view.end());
}

static std::string get_now_playing_path()
{
	if (core_api::is_main_thread())
	{
		metadb_handle_ptr handle;
		if (playback_control::get()->get_now_playing(handle))
		{
			return handle->get_path();
		}
	}
	return "";
}

static std::string string_from_json(json j)
{
	if (j.is_null()) return std::string();
	return j.is_string() ? j.get<std::string>() : j.dump();
}

static Strings strings_from_json(json j)
{
	Strings values;

	auto add_value = [&values](json j)
	{
		const std::string str = string_from_json(j);
		if (str.length()) values.emplace_back(str);
	};

	if (j.is_array())
	{
		for (const auto& value : j)
		{
			add_value(value);
		}
	}
	else
	{
		add_value(j);
	}
	return values;
}

static std::string from_wide(const std::wstring& str)
{
	const int str_length = to_int(str.length());
	const int ret_length = WideCharToMultiByte(CP_UTF8, 0, str.data(), str_length, nullptr, 0, nullptr, nullptr);

	std::string ret(ret_length, '\0');
	WideCharToMultiByte(CP_UTF8, 0, str.data(), str_length, ret.data(), ret_length, nullptr, nullptr);
	return ret;
}

static std::wstring to_wide(jstring str)
{
	const int str_length = to_int(str.length());
	const int ret_length = MultiByteToWideChar(CP_UTF8, 0, str, str_length, nullptr, 0);

	std::wstring ret(ret_length, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, str, str_length, ret.data(), ret_length);
	return ret;
}

static BSTR to_bstr(jstring str) { return SysAllocString(to_wide(str).data()); }
