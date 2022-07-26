#include "stdafx.h"
#define SDK_STRING_(x) #x
#define SDK_STRING(x) SDK_STRING_(x)

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

	std::string get_resource_text(int id)
	{
		const auto res = uLoadResource(core_api::get_my_instance(), uMAKEINTRESOURCE(id), "TEXT");
		return std::string(static_cast<const char*>(res->GetPointer()), res->GetSize());
	}

	std::wstring get_path()
	{
		const auto path = wil::GetModuleFileNameW(core_api::get_my_instance());
		return FileHelper(path.get()).parent_path();
	}

	std::wstring get_fb2k_path()
	{
		const auto path = wil::GetModuleFileNameW();
		return FileHelper(path.get()).parent_path();
	}

	std::wstring get_profile_path()
	{
		string8 path;
		filesystem::g_get_display_path(core_api::get_profile_path(), path);
		return to_wide(path) + std::filesystem::path::preferred_separator;
	}
}
