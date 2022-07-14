#include "stdafx.h"
#include "PanelWindow.h"
#include "ScriptHost.h"

ScriptHost::ScriptHost(PanelWindow* panel)
	: m_panel(panel)
	, m_console(ComObjectSingleton<Console>::get())
	, m_fb(ComObjectSingleton<Fb>::get())
	, m_gdi(ComObjectSingleton<Gdi>::get())
	, m_plman(ComObjectSingleton<Plman>::get())
	, m_utils(ComObjectSingleton<Utils>::get())
	, m_window(new ImplementCOMRefCounter<Window>(panel)) {}

HRESULT ScriptHost::InitCallbackMap()
{
	if (!m_script_root) return E_POINTER;
	for (const auto& [id, name] : g_callback_name_map)
	{
		auto cname = const_cast<LPOLESTR>(name.data());
		DISPID dispId{};
		if (SUCCEEDED(m_script_root->GetIDsOfNames(IID_NULL, &cname, 1, LOCALE_USER_DEFAULT, &dispId)))
		{
			m_callback_map.emplace(id, dispId);

			switch (id)
			{
			case CallbackID::on_char:
			case CallbackID::on_focus:
			case CallbackID::on_key_down:
			case CallbackID::on_key_up:
				m_panel->m_grabfocus = true;
				break;
			case CallbackID::on_drag_drop:
			case CallbackID::on_drag_enter:
			case CallbackID::on_drag_leave:
			case CallbackID::on_drag_over:
				m_panel->m_dragdrop = true;
				break;
			}
		}
	}
	return S_OK;
}

HRESULT ScriptHost::Initialise()
{
	static constexpr CLSID jscript9clsid = { 0x16d51579, 0xa30b, 0x4c8b, { 0xa2, 0x76, 0x0f, 0xf4, 0xdc, 0x41, 0xe7, 0x55 } };
	m_script_engine = wil::CoCreateInstanceNoThrow<IActiveScript>(jscript9clsid);

	if (!m_script_engine)
	{
		FB2K_console_formatter() << Component::name_version << ": This component requires a system with IE9 or later.";
		return E_FAIL;
	}

	wil::com_ptr_t<IActiveScriptProperty> script_property;
	auto version = _variant_t(1L + SCRIPTLANGUAGEVERSION_5_8);

	RETURN_IF_FAILED(m_script_engine->QueryInterface(&script_property));
	RETURN_IF_FAILED(script_property->SetProperty(SCRIPTPROP_INVOKEVERSIONING, nullptr, &version));
	RETURN_IF_FAILED(m_script_engine->SetScriptSite(this));
	RETURN_IF_FAILED(m_script_engine->QueryInterface(&m_parser));
	RETURN_IF_FAILED(m_parser->InitNew());
	RETURN_IF_FAILED(m_script_engine->AddNamedItem(L"console", SCRIPTITEM_ISVISIBLE));
	RETURN_IF_FAILED(m_script_engine->AddNamedItem(L"fb", SCRIPTITEM_ISVISIBLE));
	RETURN_IF_FAILED(m_script_engine->AddNamedItem(L"gdi", SCRIPTITEM_ISVISIBLE));
	RETURN_IF_FAILED(m_script_engine->AddNamedItem(L"plman", SCRIPTITEM_ISVISIBLE));
	RETURN_IF_FAILED(m_script_engine->AddNamedItem(L"utils", SCRIPTITEM_ISVISIBLE));
	RETURN_IF_FAILED(m_script_engine->AddNamedItem(L"window", SCRIPTITEM_ISVISIBLE));
	RETURN_IF_FAILED(m_script_engine->SetScriptState(SCRIPTSTATE_CONNECTED));
	RETURN_IF_FAILED(m_script_engine->GetScriptDispatch(nullptr, &m_script_root));
	RETURN_IF_FAILED(ParseImports());
	RETURN_IF_FAILED(ParseScript(m_panel->m_config.m_code, "<main>"));
	RETURN_IF_FAILED(InitCallbackMap());
	return S_OK;
}

HRESULT ScriptHost::ParseImports()
{
	for (const std::string& path : m_info.m_imports)
	{
		FileHelper f(path);
		if (f.is_file())
		{
			RETURN_IF_FAILED(ParseScript(f.read(), path));
		}
		else
		{
			FB2K_console_formatter() << m_info.m_build_string << ": file not found " << path;
		}
	}
	return S_OK;
}

HRESULT ScriptHost::ParseScript(jstring code, jstring path)
{
	m_context_to_path_map.emplace(++m_last_source_context, path);
	return m_parser->ParseScriptText(to_wide(code).data(), nullptr, nullptr, nullptr, m_last_source_context, 0, SCRIPTTEXT_HOSTMANAGESSOURCE | SCRIPTTEXT_ISVISIBLE, nullptr, nullptr);
}

STDMETHODIMP ScriptHost::GetDocVersionString(BSTR*)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::GetItemInfo(LPCOLESTR name, DWORD mask, IUnknown** ppunk, ITypeInfo** ppti)
{
	if (ppti) *ppti = nullptr;
	if (ppunk) *ppunk = nullptr;
	if (mask & SCRIPTINFO_IUNKNOWN)
	{
		if (!name) return E_INVALIDARG;
		if (!ppunk) return E_POINTER;

		if (wcscmp(name, L"console") == 0)
		{
			m_console->AddRef();
			*ppunk = m_console.get();
			return S_OK;
		}
		else if (wcscmp(name, L"fb") == 0)
		{
			m_fb->AddRef();
			*ppunk = m_fb.get();
			return S_OK;
		}
		else if (wcscmp(name, L"gdi") == 0)
		{
			m_gdi->AddRef();
			*ppunk = m_gdi.get();
			return S_OK;
		}
		else if (wcscmp(name, L"plman") == 0)
		{
			m_plman->AddRef();
			*ppunk = m_plman.get();
			return S_OK;
		}
		else if (wcscmp(name, L"utils") == 0)
		{
			m_utils->AddRef();
			*ppunk = m_utils.get();
			return S_OK;
		}
		else if (wcscmp(name, L"window") == 0)
		{
			m_window->AddRef();
			*ppunk = m_window.get();
			return S_OK;
		}
	}
	return TYPE_E_ELEMENTNOTFOUND;
}

STDMETHODIMP ScriptHost::GetLCID(LCID*)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::OnEnterScript()
{
	return S_OK;
}

STDMETHODIMP ScriptHost::OnLeaveScript()
{
	return S_OK;
}

STDMETHODIMP ScriptHost::OnScriptError(IActiveScriptError* err)
{
	if (!err) return E_POINTER;

	DWORD ctx{};
	EXCEPINFO excep{};
	LONG charpos{};
	ULONG line{};
	string8 error_text;
	wil::unique_bstr sourceline;

	error_text << m_info.m_build_string << CRLF;

	if (SUCCEEDED(err->GetExceptionInfo(&excep)))
	{
		if (excep.pfnDeferredFillIn)
		{
			(*excep.pfnDeferredFillIn)(&excep);
		}

		if (excep.bstrSource)
		{
			error_text << from_wide(excep.bstrSource) << ":" << CRLF;
			SysFreeString(excep.bstrSource);
		}

		if (excep.bstrDescription)
		{
			error_text << from_wide(excep.bstrDescription) << CRLF;
			SysFreeString(excep.bstrDescription);
		}

		if (excep.bstrHelpFile)
		{
			SysFreeString(excep.bstrHelpFile);
		}
	}

	if (SUCCEEDED(err->GetSourcePosition(&ctx, &line, &charpos)))
	{
		const auto it = m_context_to_path_map.find(ctx);
		if (it != m_context_to_path_map.end())
		{
			error_text << "File: " << it->second << CRLF;
		}
		error_text << "Line: " << (line + 1) << ", Col: " << (charpos + 1) << CRLF;
	}

	if (SUCCEEDED(err->GetSourceLineText(&sourceline)))
	{
		error_text << from_wide(sourceline.get());
	}

	FB2K_console_formatter() << error_text;

	fb2k::inMainThread([error_text]()
		{
			popup_message::g_show(error_text, Component::name_version.c_str());
		});

	MessageBeep(MB_ICONASTERISK);
	if (m_script_engine) m_script_engine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
	m_panel->unload_script();
	m_panel->repaint();
	return S_OK;
}

STDMETHODIMP ScriptHost::OnScriptTerminate(const VARIANT*, const EXCEPINFO*)
{
	return E_NOTIMPL;
}

STDMETHODIMP ScriptHost::OnStateChange(SCRIPTSTATE state)
{
	m_state = state;
	return S_OK;
}

bool ScriptHost::CheckState()
{
	return m_state == SCRIPTSTATE_CONNECTED;
}

bool ScriptHost::InvokeMouseCallback(uint32_t msg, WPARAM wp, LPARAM lp)
{
	const CallbackID id = g_msg_callback_map.at(msg);
	const auto dispId = GetDISPID(id);
	if (dispId)
	{
		VariantArgs args = { wp, GET_Y_LPARAM(lp),  GET_X_LPARAM(lp) }; // reversed
		DISPPARAMS params = { args.data(), nullptr, args.size(), 0 };
		_variant_t result;
		m_script_root->Invoke(dispId.value(), IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &result, nullptr, nullptr);
		if (id == CallbackID::on_mouse_rbtn_up && SUCCEEDED(VariantChangeType(&result, &result, 0, VT_BOOL)))
		{
			return to_bool(result.boolVal);
		}
	}
	return false;
}

std::optional<DISPID> ScriptHost::GetDISPID(CallbackID id)
{
	if (m_script_root)
	{
		const auto it = m_callback_map.find(id);
		if (it != m_callback_map.end())
		{
			return it->second;
		}
	}
	return std::nullopt;
}

void ScriptHost::InvokeCallback(CallbackID id, VariantArgs args)
{
	const auto dispId = GetDISPID(id);
	if (dispId)
	{
		std::ranges::reverse(args);
		DISPPARAMS params = { args.data(), nullptr, args.size(), 0 };
		m_script_root->Invoke(dispId.value(), IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, nullptr, nullptr, nullptr);
	}
}

void ScriptHost::Reset()
{
	if (m_script_engine && CheckState())
	{
		wil::com_ptr_t<IActiveScriptGarbageCollector> gc;
		if (SUCCEEDED(m_script_engine->QueryInterface(&gc)))
		{
			gc->CollectGarbage(SCRIPTGCTYPE_EXHAUSTIVE);
		}

		m_script_engine->SetScriptState(SCRIPTSTATE_DISCONNECTED);
		m_script_engine->SetScriptState(SCRIPTSTATE_CLOSED);
		m_script_engine->Close();
	}

	m_context_to_path_map.clear();
	m_callback_map.clear();
	m_script_engine.reset();
	m_script_root.reset();
}
