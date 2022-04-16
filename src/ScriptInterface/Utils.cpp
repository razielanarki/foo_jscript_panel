#include "stdafx.h"
#include "Utils.h"
#include "AlbumArt.h"
#include "AsyncArtTask.h"
#include "DialogInputBox.h"
#include "DownloadFileTask.h"
#include "FontHelper.h"
#include "HTTPRequestTask.h"

#include <foobar2000/helpers/filetimetools.h>

STDMETHODIMP Utils::Chardet(BSTR path, UINT* out)
{
	if (!out) return E_POINTER;

	*out = FileHelper(path).guess_codepage();
	return S_OK;
}

STDMETHODIMP Utils::CheckComponent(BSTR name, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = VARIANT_FALSE;

	const string8 uname = from_wide(name);

	for (auto e = service_enum_t<componentversion>(); !e.finished(); ++e)
	{
		string8 str;
		e.get()->get_file_name(str);
		if (_stricmp(str, uname) == 0)
		{
			*out = VARIANT_TRUE;
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP Utils::CheckFont(BSTR name, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(FontHelper::get().check_name(name));
	return S_OK;
}

STDMETHODIMP Utils::ColourPicker(UINT window_id, int default_colour, int* out)
{
	if (!out) return E_POINTER;

	COLORREF colour = to_colorref(default_colour);
	uChooseColor(&colour, to_hwnd(window_id), m_colours.data());
	*out = to_argb(colour);
	return S_OK;
}

STDMETHODIMP Utils::DateStringToTimestamp(BSTR str, UINT64* out)
{
	if (!out) return E_POINTER;

	const string8 ustr = from_wide(str);
	*out = pfc::fileTimeWtoU(filetimestamp_from_string(ustr));
	return S_OK;
}

STDMETHODIMP Utils::DownloadFileAsync(UINT window_id, BSTR url, BSTR path)
{
	auto task = std::make_unique<DownloadFileTask>(to_hwnd(window_id), url, path);
	SimpleThreadPool::get().add_task(std::move(task));
	return S_OK;
}

STDMETHODIMP Utils::FormatDuration(double seconds, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(pfc::format_time_ex(seconds, 0));
	return S_OK;
}

STDMETHODIMP Utils::FormatFileSize(UINT64 bytes, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(pfc::format_file_size_short(bytes));
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtAsync(UINT window_id, IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, VARIANT_BOOL only_embed, VARIANT_BOOL /* FFS */)
{
	if (!AlbumArt::check_id(art_id)) return E_INVALIDARG;

	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	auto task = std::make_unique<AsyncArtTask>(to_hwnd(window_id), handle_ptr, art_id, to_bool(need_stub), to_bool(only_embed));
	SimpleThreadPool::get().add_task(std::move(task));
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtEmbedded(BSTR rawpath, UINT art_id, IGdiBitmap** out)
{
	if (!out) return E_POINTER;
	if (!AlbumArt::check_id(art_id)) return E_INVALIDARG;

	album_art_data_ptr data = AlbumArt::get_embedded(from_wide(rawpath), art_id);
	*out = AlbumArt::data_to_bitmap(data);
	return S_OK;
}

STDMETHODIMP Utils::GetAlbumArtV2(IMetadbHandle* handle, UINT art_id, VARIANT_BOOL need_stub, IGdiBitmap** out)
{
	if (!out) return E_POINTER;
	if (!AlbumArt::check_id(art_id)) return E_INVALIDARG;

	metadb_handle* handle_ptr = nullptr;
	RETURN_IF_FAILED(handle->get(arg_helper(&handle_ptr)));

	string8 dummy;
	album_art_data_ptr data = AlbumArt::get(handle_ptr, art_id, to_bool(need_stub), dummy);
	*out = AlbumArt::data_to_bitmap(data);
	return S_OK;
}

STDMETHODIMP Utils::GetFileSize(BSTR path, __int64* out)
{
	if (!out) return E_POINTER;

	LARGE_INTEGER file_size = { 0 };
	auto hFile = wil::unique_hfile(CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
	if (hFile) GetFileSizeEx(hFile.get(), &file_size);

	*out = file_size.QuadPart;
	return S_OK;
}

STDMETHODIMP Utils::GetRequestAsync(UINT window_id, BSTR url, BSTR user_agent_or_headers, UINT* out)
{
	if (!out) return E_POINTER;

	auto task = std::make_unique<HTTPRequestTask>(HTTPRequestTask::Type::GET, to_hwnd(window_id), ++m_task_id, url, user_agent_or_headers);
	SimpleThreadPool::get().add_task(std::move(task));
	*out = m_task_id;
	return S_OK;
}

STDMETHODIMP Utils::GetSysColour(UINT index, int* out)
{
	if (!out) return E_POINTER;

	*out = 0;
	if (GetSysColorBrush(index) != nullptr)
	{
		*out = to_argb(GetSysColor(index));
	}
	return S_OK;
}

STDMETHODIMP Utils::GetSystemMetrics(UINT index, int* out)
{
	if (!out) return E_POINTER;

	*out = ::GetSystemMetrics(index);
	return S_OK;
}

STDMETHODIMP Utils::Glob(BSTR pattern, UINT exc_mask, UINT inc_mask, VARIANT* out)
{
	if (!out) return E_POINTER;

	WStrings files;
	WIN32_FIND_DATA data;
	auto hFindFile = wil::unique_hfind(FindFirstFileW(pattern, &data));
	if (hFindFile)
	{
		std::wstring folder = FileHelper(pattern).parent_path();

		while (true)
		{
			const DWORD attr = data.dwFileAttributes;
			if ((attr & inc_mask) && !(attr & exc_mask))
			{
				files.emplace_back(folder + data.cFileName);
			}
			if (FindNextFileW(hFindFile.get(), &data) == FALSE) break;
		}
	}

	ComArrayWriter writer(files.size());

	for (const std::wstring& file : files)
	{
		RETURN_IF_FAILED(writer.add_item(file));
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.data();
	return S_OK;
}

STDMETHODIMP Utils::InputBox(UINT window_id, BSTR prompt, BSTR caption, BSTR def, VARIANT_BOOL error_on_cancel, BSTR* out)
{
	if (!out) return E_POINTER;

	modal_dialog_scope scope;
	if (scope.can_create())
	{
		HWND hwnd = to_hwnd(window_id);
		scope.initialize(hwnd);

		CDialogInputBox dlg(prompt, caption, def);
		const int status = dlg.DoModal(hwnd);
		if (status == IDOK)
		{
			*out = SysAllocString(dlg.m_value.data());
		}
		else if (status == IDCANCEL)
		{
			if (to_bool(error_on_cancel))
			{
				return E_FAIL;
			}
			*out = def;
		}
	}
	return S_OK;
}

STDMETHODIMP Utils::IsFile(BSTR path, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(FileHelper(path).is_file());
	return S_OK;
}

STDMETHODIMP Utils::IsFolder(BSTR folder, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(FileHelper(folder).is_folder());
	return S_OK;
}

STDMETHODIMP Utils::IsKeyPressed(UINT vkey, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(::IsKeyPressed(vkey));
	return S_OK;
}

STDMETHODIMP Utils::ListFiles(BSTR folder, VARIANT_BOOL recur, VARIANT* out)
{
	if (!out) return E_POINTER;

	WStrings files = FileHelper(folder).list_files(to_bool(recur));
	ComArrayWriter writer(files.size());

	for (const std::wstring& file : files)
	{
		RETURN_IF_FAILED(writer.add_item(file));
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.data();
	return S_OK;
}

STDMETHODIMP Utils::ListFolders(BSTR folder, VARIANT_BOOL recur, VARIANT* out)
{
	if (!out) return E_POINTER;

	WStrings folders = FileHelper(folder).list_folders(to_bool(recur));
	ComArrayWriter writer(folders.size());

	for (const std::wstring& folder : folders)
	{
		RETURN_IF_FAILED(writer.add_item(folder + std::filesystem::path::preferred_separator));
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.data();
	return S_OK;
}

STDMETHODIMP Utils::ListFonts(VARIANT* out)
{
	if (!out) return E_POINTER;

	FontHelper::Names names = FontHelper::get().get_names();
	ComArrayWriter writer(names.size());

	for (const FontHelper::Name& name : names)
	{
		RETURN_IF_FAILED(writer.add_item(name.data()));
	}

	out->vt = VT_ARRAY | VT_VARIANT;
	out->parray = writer.data();
	return S_OK;
}

STDMETHODIMP Utils::MapString(BSTR str, UINT lcid, UINT flags, BSTR* out)
{
	if (!out) return E_POINTER;

	int r = LCMapStringW(lcid, flags, str, wcslen(str) + 1, nullptr, 0);
	if (r)
	{
		std::wstring dst(r, '\0');
		r = LCMapStringW(lcid, flags, str, wcslen(str) + 1, dst.data(), dst.size());
		if (r) *out = SysAllocString(dst.data());
		return S_OK;
	}
	return E_INVALIDARG;
}

STDMETHODIMP Utils::PathWildcardMatch(BSTR pattern, BSTR str, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(PathMatchSpecW(str, pattern));
	return S_OK;
}

STDMETHODIMP Utils::PostRequestAsync(UINT window_id, BSTR url, BSTR user_agent_or_headers, BSTR post_data, BSTR content_type, UINT* out)
{
	if (!out) return E_POINTER;

	auto task = std::make_unique<HTTPRequestTask>(HTTPRequestTask::Type::POST, to_hwnd(window_id), ++m_task_id, url, user_agent_or_headers, post_data, content_type);
	SimpleThreadPool::get().add_task(std::move(task));
	*out = m_task_id;
	return S_OK;
}

STDMETHODIMP Utils::ReadINI(BSTR path, BSTR section, BSTR key, BSTR defaultval, BSTR* out)
{
	if (!out) return E_POINTER;

	std::array<wchar_t, MAX_PATH> buffer;
	GetPrivateProfileStringW(section, key, defaultval, buffer.data(), buffer.size(), path);
	*out = SysAllocString(buffer.data());
	return S_OK;
}

STDMETHODIMP Utils::ReadTextFile(BSTR path, UINT codepage, BSTR* out)
{
	if (!out) return E_POINTER;

	std::wstring content;
	FileHelper(path).read_wide(codepage, content);
	*out = SysAllocString(content.data());
	return S_OK;
}

STDMETHODIMP Utils::ReadUTF8(BSTR path, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(FileHelper(path).read());
	return S_OK;
}

STDMETHODIMP Utils::ReplaceIllegalChars(BSTR str, VARIANT_BOOL modern, BSTR* out)
{
	if (!out) return E_POINTER;

	using namespace pfc::io::path;
	const string8 ustr = from_wide(str);
	const charReplace_t replacer = to_bool(modern) ? charReplaceModern : charReplaceDefault;
	*out = to_bstr(replaceIllegalNameChars(ustr, false, replacer));
	return S_OK;
}

STDMETHODIMP Utils::TimestampToDateString(UINT64 ts, BSTR* out)
{
	if (!out) return E_POINTER;

	*out = to_bstr(format_filetimestamp(pfc::fileTimeUtoW(ts)).get_ptr());
	return S_OK;
}

STDMETHODIMP Utils::WriteINI(BSTR path, BSTR section, BSTR key, BSTR val, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = to_variant_bool(WritePrivateProfileStringW(section, key, val, path));
	return S_OK;
}

STDMETHODIMP Utils::WriteTextFile(BSTR path, BSTR content, VARIANT_BOOL* out)
{
	if (!out) return E_POINTER;

	*out = VARIANT_FALSE;

	if (content != nullptr)
	{
		*out = to_variant_bool(FileHelper(path).write(from_wide(content)));
	}
	return S_OK;
}

STDMETHODIMP Utils::get_Version(UINT* out)
{
	if (!out) return E_POINTER;

	*out = Component::version_number;
	return S_OK;
}
