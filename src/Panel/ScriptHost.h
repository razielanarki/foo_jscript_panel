#pragma once
#include "ScriptInfo.h"

class ScriptHost : public ImplementCOMRefCounter<IActiveScriptSite>
{
public:
	ScriptHost(PanelWindow* panel);

	COM_QI_SIMPLE(IActiveScriptSite)

	HRESULT Initialise();
	STDMETHODIMP GetDocVersionString(BSTR*) override;
	STDMETHODIMP GetItemInfo(LPCOLESTR name, DWORD mask, IUnknown** ppunk, ITypeInfo** ppti) override;
	STDMETHODIMP GetLCID(LCID*) override;
	STDMETHODIMP OnEnterScript() override;
	STDMETHODIMP OnLeaveScript() override;
	STDMETHODIMP OnScriptError(IActiveScriptError* err) override;
	STDMETHODIMP OnScriptTerminate(const VARIANT*, const EXCEPINFO*) override;
	STDMETHODIMP OnStateChange(SCRIPTSTATE state) override;
	bool CheckState();
	bool InvokeMouseCallback(uint32_t msg, WPARAM wp, LPARAM lp);
	void InvokeCallback(CallbackID id, VariantArgs args = {});
	void Reset();

	ScriptInfo m_info;

private:
	HRESULT InitCallbackMap();
	HRESULT ParseImports();
	HRESULT ParseScript(jstring code, jstring path);
	std::optional<DISPID> GetDISPID(CallbackID id);

	DWORD m_last_source_context{};
	PanelWindow* m_panel;
	SCRIPTSTATE m_state{};
	std::map<CallbackID, DISPID> m_callback_map;
	std::map<DWORD, std::string> m_context_to_path_map;
	wil::com_ptr_t<Console> m_console;
	wil::com_ptr_t<Fb> m_fb;
	wil::com_ptr_t<Gdi> m_gdi;
	wil::com_ptr_t<IActiveScript> m_script_engine;
	wil::com_ptr_t<IActiveScriptParse> m_parser;
	wil::com_ptr_t<IDispatch> m_script_root;
	wil::com_ptr_t<Plman> m_plman;
	wil::com_ptr_t<Utils> m_utils;
	wil::com_ptr_t<Window> m_window;
};
