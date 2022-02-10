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
#include <vector>

#include "../columns_ui-sdk/ui_extension.h"
#include <foobar2000/helpers/foobar2000+atl.h>
#include <foobar2000/helpers/atl-misc.h>
#include <libPPUI/gdiplus_helpers.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <wil/com.h>
#include <wil/resource.h>

#include "Misc.h"
#include "StringHelpers.h"
#include "FileHelper.h"
#include "ScriptInterface.h"

#include "foo_jscript_panel.h"
#include "guids.h"
#include "resource.h"

#include "CallbackID.h"
#include "CallbackData.h"
