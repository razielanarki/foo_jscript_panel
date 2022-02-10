#pragma once
#include "PanelWindow.h"
#include "PropertyList.h"

#include <libPPUI/CDialogResizeHelper.h>

class CDialogProperties : public CDialogImpl<CDialogProperties>
{
public:
	CDialogProperties(PanelWindow* panel);

	BEGIN_MSG_MAP_EX(CDialogProperties)
		CHAIN_MSG_MAP_MEMBER(m_resizer)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDC_BTN_IMPORT, OnImport)
		COMMAND_ID_HANDLER_EX(IDC_BTN_EXPORT, OnExport)
		COMMAND_ID_HANDLER_EX(IDC_BTN_CLEAR, OnClear)
		COMMAND_ID_HANDLER_EX(IDOK, OnApplyOrOK)
		COMMAND_ID_HANDLER_EX(IDC_BTN_APPLY, OnApplyOrOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_PROPERTIES };

	BOOL OnInitDialog(CWindow, LPARAM);
	void LoadProperties();
	void OnApplyOrOK(UINT, int nID, CWindow);
	void OnCancel(UINT, int nID, CWindow);
	void OnClear(UINT, int, CWindow);
	void OnExport(UINT, int, CWindow);
	void OnImport(UINT, int, CWindow);

private:
	CButton m_btn_clear, m_btn_export;
	CDialogResizeHelper m_resizer;
	PanelProperties::PropertyData m_dup_data;
	PanelWindow* m_panel;
	PropertyList m_list;
};
