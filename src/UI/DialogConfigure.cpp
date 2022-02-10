#include "stdafx.h"
#include "Config.h"
#include "DialogConfigure.h"
#include "PanelWindow.h"

namespace
{
	static const CDialogResizeHelper::Param resize_data[] =
	{
		{ IDC_SCINTILLA, 0, 0, 1, 1 },
		{ IDC_BTN_RESET, 0, 1, 0, 1 },
		{ IDC_LABEL_EDGE, 0, 1, 0, 1 },
		{ IDC_COMBO_EDGE, 0, 1, 0, 1 },
		{ IDC_CHECK_PSEUDO_TRANSPARENT, 0, 1, 0, 1 },
		{ IDOK, 1, 1, 1, 1 },
		{ IDCANCEL, 1, 1, 1, 1 },
		{ IDC_BTN_APPLY, 1, 1, 1, 1 },
	};

	static const CRect resize_min_max(620, 400, 0, 0);
}

CDialogConfigure::CDialogConfigure(PanelWindow* panel) : m_panel(panel), m_resizer(resize_data, resize_min_max) {}

BOOL CDialogConfigure::OnInitDialog(CWindow, LPARAM)
{
	BuildMenu();

	m_caption << Component::name << " Configuration";
	if (m_panel->m_id != 0) m_caption << " (id:" << m_panel->m_id << ")";
	pfc::setWindowText(m_hWnd, m_caption);

	g_config.set_window_placement(m_hWnd);

	m_combo_edge = GetDlgItem(IDC_COMBO_EDGE);
	m_combo_edge.AddString(L"None");
	m_combo_edge.AddString(L"Sunken");
	m_combo_edge.AddString(L"Grey");

	if (m_panel->m_is_default_ui)
	{
		m_combo_edge.SetCurSel(0);
		m_combo_edge.EnableWindow(false);
	}
	else
	{
		m_combo_edge.SetCurSel(m_panel->m_config.m_style);
	}

	m_check_transparent = GetDlgItem(IDC_CHECK_PSEUDO_TRANSPARENT);
	if (m_panel->m_supports_transparency)
	{
		m_check_transparent.SetCheck(m_panel->m_config.m_transparent);
	}
	else
	{
		m_check_transparent.SetCheck(false);
		m_check_transparent.EnableWindow(false);
	}

	m_editorctrl.SubclassWindow(GetDlgItem(IDC_SCINTILLA));
	m_editorctrl.Init();
	m_editorctrl.SetContent(m_panel->m_config.m_code);
	m_editorctrl.EmptyUndoBuffer();
	m_editorctrl.SetSavePoint();

	if (m_panel->m_hwnd == nullptr) // not a proper instance if spawned from CUI layout tab, disable Apply button
	{
		GetDlgItem(IDC_BTN_APPLY).EnableWindow(false);
	}

	return FALSE;
}

LRESULT CDialogConfigure::OnNotify(int, LPNMHDR pnmh)
{
	const Notification code = static_cast<Notification>(pnmh->code);

	switch (code)
	{
	case Notification::SavePointLeft:
		pfc::setWindowText(m_hWnd, PFC_string_formatter() << "*" << m_caption);
		break;
	case Notification::SavePointReached:
		pfc::setWindowText(m_hWnd, m_caption);
		break;
	}
	SetMsgHandled(FALSE);
	return 0;
}

void CDialogConfigure::BuildMenu()
{
	auto PopulateMenu = [](wil::unique_hmenu& menu, WStrings& files, const std::wstring& folder, uint32_t id_begin)
	{
		files = FileHelper(folder).list_files();
		for (uint32_t i = id_begin; FileHelper path : files)
		{
			AppendMenuW(menu.get(), MF_STRING, i++, path.filename().data());
		}
	};

	auto menu = wil::unique_hmenu(GetMenu());
	auto file = wil::unique_hmenu(CreateMenu());
	auto samples = wil::unique_hmenu(CreateMenu());
	auto basic = wil::unique_hmenu(CreatePopupMenu());
	auto test = wil::unique_hmenu(CreateMenu());
	auto docs = wil::unique_hmenu(CreateMenu());
	auto licenses = wil::unique_hmenu(CreateMenu());
	auto github = wil::unique_hmenu(CreateMenu());

	AppendMenuW(file.get(), MF_STRING, ID_FILE_IMPORT, L"Import...");
	AppendMenuW(file.get(), MF_STRING, ID_FILE_EXPORT, L"Export...");
	AppendMenuW(github.get(), MF_STRING, ID_GITHUB_CODE, L"Code");
	AppendMenuW(github.get(), MF_STRING, ID_GITHUB_RELEASES, L"Releases");
	AppendMenuW(github.get(), MF_STRING, ID_GITHUB_WIKI, L"Wiki");

	const std::wstring component_folder = Component::get_path();
	PopulateMenu(basic, m_basic, component_folder + L"samples\\basic", ID_BASIC_BEGIN);
	PopulateMenu(samples, m_samples, component_folder + L"samples", ID_SAMPLES_BEGIN);
	PopulateMenu(test, m_test, L"E:\\Junk\\foobar2000\\test", ID_TEST_BEGIN);
	PopulateMenu(docs, m_docs, component_folder + L"docs", ID_DOCS_BEGIN);
	PopulateMenu(licenses, m_licenses, component_folder + L"licenses", ID_LICENSES_BEGIN);

	RemoveMenu(menu.get(), 0, MF_BYPOSITION);
	AppendMenuW(menu.get(), MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(file.get()), L"File");
	InsertMenuW(samples.get(), ID_SAMPLES_BEGIN, MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(basic.get()), L"basic");
	AppendMenuW(menu.get(), MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(samples.get()), L"Samples");
	if (m_test.size() > 0) AppendMenuW(menu.get(), MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(test.get()), L"Test");
	AppendMenuW(menu.get(), MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(docs.get()), L"Docs");
	AppendMenuW(menu.get(), MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(licenses.get()), L"Licenses");
	AppendMenuW(menu.get(), MF_STRING | MF_POPUP, reinterpret_cast<UINT_PTR>(github.get()), L"GitHub");
}

void CDialogConfigure::OnApplyOrOK(UINT, int nID, CWindow)
{
	m_panel->m_config.m_style = m_combo_edge.GetCurSel();
	m_panel->m_config.m_transparent = m_check_transparent.IsChecked();
	m_panel->m_config.m_code = m_editorctrl.GetContent();
	m_panel->update_script();
	m_editorctrl.SetSavePoint();
	g_config.get_window_placement(m_hWnd);

	if (nID == IDC_BTN_APPLY) return;
	EndDialog(nID);
}

void CDialogConfigure::OnCancel(UINT, int nID, CWindow)
{
	if (m_editorctrl.GetModify() && MessageBoxW(L"Unsaved changes will be lost. Are you sure?", to_wide(Component::name).data(), MB_ICONWARNING | MB_SETFOREGROUND | MB_YESNO) != IDYES)
	{
		return;
	}
	EndDialog(nID);
}

void CDialogConfigure::OnMenuBasic(UINT, int nID, CWindow)
{
	m_editorctrl.SetContent(FileHelper(m_basic[nID - ID_BASIC_BEGIN]).read());
}

void CDialogConfigure::OnMenuDocs(UINT, int nID, CWindow)
{
	ShellExecuteW(nullptr, L"open", m_docs[nID - ID_DOCS_BEGIN].data(), nullptr, nullptr, SW_SHOW);
}

void CDialogConfigure::OnMenuFile(UINT, int nID, CWindow)
{
	string8 path;
	if (nID == ID_FILE_IMPORT)
	{
		if (uGetOpenFileName(m_hWnd, "Text files|*.txt|JScript files|*.js|All files|*.*", 0, "txt", "Import from", nullptr, path, FALSE))
		{
			m_editorctrl.SetContent(FileHelper(path).read());
		}
	}
	else if (nID == ID_FILE_EXPORT)
	{
		if (uGetOpenFileName(m_hWnd, "Text files|*.txt|All files|*.*", 0, "txt", "Save as", nullptr, path, TRUE))
		{
			FileHelper(path).write(m_editorctrl.GetContent());
		}
	}
}

void CDialogConfigure::OnMenuGitHub(UINT, int nID, CWindow)
{
	std::wstring url = to_wide(Component::home_page);
	if (nID == ID_GITHUB_RELEASES) url += L"/releases";
	else if (nID == ID_GITHUB_WIKI) url += L"/wiki";
	ShellExecuteW(nullptr, L"open", url.data(), nullptr, nullptr, SW_SHOW);
}

void CDialogConfigure::OnMenuLicenses(UINT, int nID, CWindow)
{
	FileHelper f(m_licenses[nID - ID_LICENSES_BEGIN]);
	popup_message::g_show(f.read().c_str(), from_wide(f.filename()).c_str());
}

void CDialogConfigure::OnMenuSamples(UINT, int nID, CWindow)
{
	m_editorctrl.SetContent(FileHelper(m_samples[nID - ID_SAMPLES_BEGIN]).read());
}

void CDialogConfigure::OnMenuTest(UINT, int nID, CWindow)
{
	m_editorctrl.SetContent(FileHelper(m_test[nID - ID_TEST_BEGIN]).read());
}

void CDialogConfigure::OnReset(UINT, int, CWindow)
{
	m_combo_edge.SetCurSel(0);
	m_check_transparent.SetCheck(false);
	m_editorctrl.SetContent(Component::get_resource_text(IDR_SCRIPT));
}
