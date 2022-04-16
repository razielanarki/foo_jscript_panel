#pragma once
#include <fstream>
#include <MLang.h>

class FileHelper
{
public:
	FileHelper(const std::wstring& path) : m_path(path) {}
	FileHelper(jstring path) : m_path(to_wide(path)) {}

	WStrings list_files(bool recur = false)
	{
		if (recur) return list_t<std::filesystem::recursive_directory_iterator>(Entry::is_file);
		return list_t(Entry::is_file);
	}

	WStrings list_folders(bool recur = false)
	{
		if (recur) return list_t<std::filesystem::recursive_directory_iterator>(Entry::is_folder);
		return list_t(Entry::is_folder);
	}

	bool is_file()
	{
		std::error_code ec;
		return std::filesystem::is_regular_file(m_path, ec);
	}

	bool is_folder()
	{
		std::error_code ec;
		return std::filesystem::is_directory(m_path, ec);
	}

	bool write(jstring content)
	{
		std::ofstream f(m_path, std::ios::binary);
		if (f.is_open())
		{
			f << content;
			f.close();
			return true;
		}
		return false;
	}

	std::string read()
	{
		std::string content;
		std::ifstream f(m_path);
		if (f.is_open())
		{
			std::string line;
			while (std::getline(f, line))
			{
				content += line + CRLF;
			}
			f.close();
		}
		if (content.starts_with(UTF_8_BOM))
		{
			return content.substr(3);
		}
		return content;
	}

	std::wstring filename()
	{
		return m_path.stem().wstring();
	}

	std::wstring parent_path()
	{
		return m_path.parent_path().wstring() + std::filesystem::path::preferred_separator;
	}

	uint32_t guess_codepage()
	{
		return guess_codepage(read());
	}

	void read_wide(uint32_t codepage, std::wstring& content)
	{
		auto hFile = wil::unique_hfile(CreateFileW(m_path.wstring().data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
		if (!hFile) return;

		const DWORD dwFileSize = GetFileSize(hFile.get(), nullptr);
		if (dwFileSize == INVALID_FILE_SIZE) return;

		auto hFileMapping = wil::unique_handle(CreateFileMappingW(hFile.get(), nullptr, PAGE_READONLY, 0, 0, nullptr));
		if (!hFileMapping) return;

		auto pAddr = wil::unique_mapview_ptr<uint8_t>(static_cast<uint8_t*>(MapViewOfFile(hFileMapping.get(), FILE_MAP_READ, 0, 0, 0)));
		if (!pAddr) return;

		if (dwFileSize >= 2 && memcmp(pAddr.get(), UTF_16_LE_BOM, 2) == 0)
		{
			const wchar_t* str = reinterpret_cast<const wchar_t*>(pAddr.get() + 2);
			const uint32_t len = (dwFileSize - 2) >> 1;

			content.resize(len);
			pfc::__unsafe__memcpy_t(content.data(), str, len);
		}
		else
		{
			std::string str(reinterpret_cast<const char*>(pAddr.get()), dwFileSize);

			if (str.starts_with(UTF_8_BOM))
			{
				content = to_wide(str.substr(3));
			}
			else if (codepage == CP_UTF8 || guess_codepage(str) == CP_UTF8)
			{
				content = to_wide(str);
			}
			else
			{
				content.resize(pfc::stringcvt::estimate_codepage_to_wide(codepage, str.c_str(), dwFileSize));
				pfc::stringcvt::convert_codepage_to_wide(codepage, content.data(), content.size(), str.c_str(), dwFileSize);
			}
		}
	}

private:
	struct Entry
	{
		static bool is_file(const std::filesystem::directory_entry& entry) { return entry.is_regular_file(); }
		static bool is_folder(const std::filesystem::directory_entry& entry) { return entry.is_directory(); }
	};

	using CheckEntry = std::function<bool(const std::filesystem::directory_entry&)>;

	template <typename T = std::filesystem::directory_iterator>
	WStrings list_t(CheckEntry check_entry)
	{
		WStrings paths;
		if (is_folder())
		{
			for (const auto& p : T(m_path, m_options))
			{
				if (check_entry(p)) paths.emplace_back(p.path().wstring());
			}
		}
		return paths;
	}

	uint32_t guess_codepage(jstring content)
	{
		int size = to_int(content.length());
		if (size == 0) return 0;

		constexpr int maxEncodings = 1;
		int encodingCount = maxEncodings;
		std::array<DetectEncodingInfo, maxEncodings> encodings;

		auto lang = wil::CoCreateInstanceNoThrow<IMultiLanguage2>(CLSID_CMultiLanguage);
		if (!lang) return 0;
		if (FAILED(lang->DetectInputCodepage(MLDETECTCP_NONE, 0, const_cast<char*>(content.get()), &size, encodings.data(), &encodingCount))) return 0;

		const uint32_t codepage = encodings[0].nCodePage;
		if (codepage == 20127) return CP_UTF8;
		return codepage;
	}

	static constexpr const char* UTF_16_LE_BOM = "\xFF\xFE";
	static constexpr const char* UTF_8_BOM = "\xEF\xBB\xBF";

	std::filesystem::directory_options m_options = std::filesystem::directory_options::skip_permission_denied;
	std::filesystem::path m_path;
};
