#pragma once
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7
#define NOMINMAX

#include <algorithm>
#include <array>
#include <filesystem>
#include <map>
#include <memory>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#include <WinSock2.h>
#include <Windows.h>
#include <windowsx.h>
#include <Uxtheme.h>
#include <ActivScp.h>
#include <ComDef.h>

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/win32_helpers.h>

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <json.hpp>
using json = nlohmann::json;

#include <foobar2000/helpers/foobar2000+atl.h>
#include <foobar2000/helpers/atl-misc.h>
#include <libPPUI/CDialogResizeHelper.h>
#include <libPPUI/CListControlComplete.h>
#include <libPPUI/CListControl-Cells.h>
#include <libPPUI/CListControlOwnerData.h>
#include <libPPUI/Controls.h>
#include <libPPUI/gdiplus_helpers.h>
#include <libPPUI/pp-COM-macros.h>
#include <columns_ui-sdk/ui_extension.h>

using VariantArgs = std::vector<_variant_t>;
using pfc::string8;

#include "foo_jscript_panel.h"
#include "resource.h"

#include "Misc.h"
#include "StringHelpers.h"
#include "FileHelper.h"

#include "ScriptInterface.h"
