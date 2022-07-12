#pragma once

class CDialogGoto : public CDialogImpl<CDialogGoto>
{
public:
	CDialogGoto(int line_number) : m_line_number(line_number) {}

	BEGIN_MSG_MAP_EX(CDialogGoto)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_GOTO };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		m_edit_line_number = GetDlgItem(IDC_EDIT_LINE_NUMBER);
		::SetWindowTextW(m_edit_line_number, std::to_wstring(m_line_number).data());
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		const string8 text = pfc::getWindowText(m_edit_line_number);
		if (pfc::string_is_numeric(text))
		{
			m_line_number = std::stoi(text.get_ptr());
		}
		else
		{
			m_line_number = 1;
		}
		EndDialog(nID);
	}

	CEdit m_edit_line_number;
	int m_line_number = 0;
};
