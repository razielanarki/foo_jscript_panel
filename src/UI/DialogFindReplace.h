#pragma once
#include "EditorCtrl.h"
#include "KeyHack.h"

class CDialogFindReplace : public CDialogImpl<CDialogFindReplace>
{
public:
	enum class Mode
	{
		find,
		replace
	};

	CDialogFindReplace(CEditorCtrl* parent);

	BEGIN_MSG_MAP_EX(CDialogFindReplace)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDC_BTN_NEXT, OnFindNext)
		COMMAND_ID_HANDLER_EX(IDC_BTN_PREVIOUS, OnFindPrevious)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER_EX(IDC_BTN_REPLACE, OnReplace)
		COMMAND_ID_HANDLER_EX(IDC_BTN_REPLACE_ALL, OnReplaceAll)
		COMMAND_HANDLER_EX(IDC_EDIT_FIND, EN_CHANGE, OnFindTextChange)
		COMMAND_HANDLER_EX(IDC_EDIT_REPLACE, EN_CHANGE, OnReplaceTextChange)
		COMMAND_RANGE_HANDLER_EX(IDC_CHECK_MATCHCASE, IDC_CHECK_REGEXP, OnCheckBoxChange)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_FIND_REPLACE };

	BOOL OnInitDialog(CWindow, LPARAM);
	void OnCancel(UINT, int, CWindow);
	void OnCheckBoxChange(UINT, int nID, CWindow);
	void OnFindNext(UINT, int, CWindow);
	void OnFindPrevious(UINT, int, CWindow);
	void OnFindTextChange(UINT, int, CWindow);
	void OnReplace(UINT, int, CWindow);
	void OnReplaceAll(UINT, int, CWindow);
	void OnReplaceTextChange(UINT, int, CWindow);
	void SetMode(Mode m);

	FindOption m_flags = FindOption::None;
	string8 m_find_text, m_replace_text;

private:
	CEditorCtrl* m_parent;
	bool m_havefound = false;
	std::map<int, CCheckBox> m_check_box_map;
	std::map<int, CWindow> m_window_map;
	std::vector<KeyHack> m_hacks;
};
