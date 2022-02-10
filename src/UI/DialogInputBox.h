#pragma once

class CDialogInputBox : public CDialogImpl<CDialogInputBox>
{
public:
	CDialogInputBox(const std::wstring& prompt, const std::wstring& caption, const std::wstring& value) : m_prompt(prompt), m_caption(caption), m_value(value) {}

	BEGIN_MSG_MAP_EX(CDialogInputBox)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_INPUT };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		m_label_prompt = GetDlgItem(IDC_LABEL_PROMPT);
		m_edit_value = GetDlgItem(IDC_EDIT_VALUE);

		::SetWindowTextW(m_hWnd, m_caption.data());
		::SetWindowTextW(m_label_prompt, m_prompt.data());
		::SetWindowTextW(m_edit_value, m_value.data());
		CenterWindow();
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		m_value = to_wide(pfc::getWindowText(m_edit_value));
		EndDialog(nID);
	}

	CEdit m_edit_value;
	CWindow m_label_prompt;
	std::wstring m_caption, m_prompt, m_value;
};
