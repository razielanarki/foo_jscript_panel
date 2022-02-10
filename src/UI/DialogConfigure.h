#pragma once
#include "EditorCtrl.h"

#include <libPPUI/CDialogResizeHelper.h>

class PanelWindow;

class CDialogConfigure : public CDialogImpl<CDialogConfigure>
{
public:
	CDialogConfigure(PanelWindow* panel);

	BEGIN_MSG_MAP_EX(CDialogConfigure)
		CHAIN_MSG_MAP_MEMBER(m_resizer)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_NOTIFY(OnNotify)
		COMMAND_RANGE_HANDLER_EX(ID_FILE_IMPORT, ID_FILE_EXPORT, OnMenuFile)
		COMMAND_RANGE_HANDLER_EX(ID_SAMPLES_BEGIN, ID_SAMPLES_END, OnMenuSamples)
		COMMAND_RANGE_HANDLER_EX(ID_BASIC_BEGIN, ID_BASIC_END, OnMenuBasic)
		COMMAND_RANGE_HANDLER_EX(ID_TEST_BEGIN, ID_TEST_END, OnMenuTest)
		COMMAND_RANGE_HANDLER_EX(ID_DOCS_BEGIN, ID_DOCS_END, OnMenuDocs)
		COMMAND_RANGE_HANDLER_EX(ID_LICENSES_BEGIN, ID_LICENSES_END, OnMenuLicenses)
		COMMAND_RANGE_HANDLER_EX(ID_GITHUB_CODE, ID_GITHUB_WIKI, OnMenuGitHub)
		COMMAND_ID_HANDLER_EX(IDC_BTN_RESET, OnReset)
		COMMAND_ID_HANDLER_EX(IDOK, OnApplyOrOK)
		COMMAND_ID_HANDLER_EX(IDC_BTN_APPLY, OnApplyOrOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_CONFIGURE };

	BOOL OnInitDialog(CWindow, LPARAM);
	LRESULT OnNotify(int, LPNMHDR pnmh);
	void BuildMenu();
	void OnApplyOrOK(UINT, int nID, CWindow);
	void OnCancel(UINT, int nID, CWindow);
	void OnMenuBasic(UINT, int nID, CWindow);
	void OnMenuDocs(UINT, int nID, CWindow);
	void OnMenuFile(UINT, int nID, CWindow);
	void OnMenuGitHub(UINT, int nID, CWindow);
	void OnMenuLicenses(UINT, int nID, CWindow);
	void OnMenuSamples(UINT, int nID, CWindow);
	void OnMenuTest(UINT, int nID, CWindow);
	void OnReset(UINT, int, CWindow);

private:
	CCheckBox m_check_transparent;
	CComboBox m_combo_edge;
	CDialogResizeHelper m_resizer;
	CEditorCtrl m_editorctrl;
	PanelWindow* m_panel;
	WStrings m_basic, m_docs, m_licenses, m_samples, m_test;
	string8 m_caption;
};
