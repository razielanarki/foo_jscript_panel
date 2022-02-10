#include "stdafx.h"
#define SDK_STRING_(x) #x
#define SDK_STRING(x) SDK_STRING_(x)

wil::com_ptr_t<ITypeLib> g_type_lib;

namespace Component
{
	DECLARE_COMPONENT_VERSION(
		name,
		version,
		"Copyright (C) 2015-2022 marc2003\n"
		"Based on WSH Panel Mod by T.P. Wang\n"
		"Thanks for the contributions by TheQwertiest and kbuffington\n\n"
		"Build: " __TIME__ ", " __DATE__ "\n\n"
		"foobar2000 SDK: " SDK_STRING(FOOBAR2000_SDK_VERSION) "\n"
		"Columns UI SDK: " UI_EXTENSION_VERSION
	);

	VALIDATE_COMPONENT_FILENAME("foo_jscript_panel.dll");

	GdiplusScope scope;

	extern "C" BOOL WINAPI DllMain(HINSTANCE ins, DWORD reason, LPVOID)
	{
		if (reason == DLL_PROCESS_ATTACH)
		{
			PathString path;
			GetModuleFileNameW(ins, path.data(), path.size());
			return SUCCEEDED(LoadTypeLibEx(path.data(), REGKIND_NONE, &g_type_lib));
		}
		return TRUE;
	}

	std::string get_resource_text(int id)
	{
		puResource pures = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(id), "TEXT");
		return std::string(static_cast<const char*>(pures->GetPointer()), pures->GetSize());
	}

	std::wstring get_path()
	{
		PathString path;
		GetModuleFileNameW(core_api::get_my_instance(), path.data(), path.size());
		return FileHelper(path).parent_path();
	}

	std::wstring get_fb2k_path()
	{
		PathString path;
		GetModuleFileNameW(nullptr, path.data(), path.size());
		return FileHelper(path).parent_path();
	}

	std::wstring get_profile_path()
	{
		string8 path;
		filesystem::g_get_display_path(core_api::get_profile_path(), path);
		return to_wide(path) + std::filesystem::path::preferred_separator;
	}
}
