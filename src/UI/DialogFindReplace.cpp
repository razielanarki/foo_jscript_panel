#include "stdafx.h"
#include "DialogFindReplace.h"

namespace
{
	FindOption operator ~(FindOption rhs)
	{
		return static_cast<FindOption>(~std::to_underlying(rhs));
	}

	FindOption& operator &=(FindOption& lhs, FindOption rhs)
	{
		lhs = static_cast<FindOption>(std::to_underlying(lhs) & std::to_underlying(rhs));
		return lhs;
	}

	static const std::map<int, FindOption> id_option_map =
	{
		{ IDC_CHECK_MATCHCASE, FindOption::MatchCase },
		{ IDC_CHECK_WHOLEWORD, FindOption::WholeWord },
		{ IDC_CHECK_REGEXP, FindOption::RegExp | FindOption::Cxx11RegEx }
	};

	static constexpr std::array ids =
	{
		IDC_EDIT_FIND,
		IDC_EDIT_REPLACE,
		IDC_BTN_NEXT,
		IDC_BTN_PREVIOUS,
		IDC_BTN_REPLACE,
		IDC_BTN_REPLACE_ALL,
		IDC_LABEL_REPLACE,
		IDCANCEL
	};
}

CDialogFindReplace::CDialogFindReplace(CEditorCtrl* parent) : m_parent(parent)
{
	m_hacks.assign(id_option_map.size() + ids.size(), KeyHack());
}

BOOL CDialogFindReplace::OnInitDialog(CWindow, LPARAM)
{
	auto it = m_hacks.begin();

	for (const int id : id_option_map | std::views::keys)
	{
		CCheckBox hwnd(GetDlgItem(id));
		m_check_box_map.emplace(id, hwnd);

		it->SubclassWindow(hwnd);
		it->m_ret_cmd = IDC_BTN_NEXT;
		it++;
	}

	for (const int id : ids)
	{
		CWindow hwnd = GetDlgItem(id);
		m_window_map.emplace(id, hwnd);

		it->SubclassWindow(hwnd);
		it->m_ret_cmd = id == IDC_EDIT_FIND || id == IDC_EDIT_REPLACE ? IDC_BTN_NEXT : id;
		it++;
	}

	return TRUE;
}

void CDialogFindReplace::OnCancel(UINT, int, CWindow)
{
	ShowWindow(SW_HIDE);
}

void CDialogFindReplace::OnCheckBoxChange(UINT, int nID, CWindow)
{
	const FindOption option = id_option_map.at(nID);

	if (m_check_box_map.at(nID).IsChecked())
		m_flags |= option;
	else
		m_flags &= ~option;

	const bool regexp = FlagSet(m_flags, FindOption::RegExp);
	m_check_box_map.at(IDC_CHECK_WHOLEWORD).EnableWindow(!regexp);
	m_window_map.at(IDC_BTN_PREVIOUS).EnableWindow(m_find_text.get_length() > 0 && !regexp);
}

void CDialogFindReplace::OnFindNext(UINT, int, CWindow)
{
	m_havefound = m_parent->Find(true);
}

void CDialogFindReplace::OnFindPrevious(UINT, int, CWindow)
{
	m_parent->Find(false);
}

void CDialogFindReplace::OnFindTextChange(UINT, int, CWindow)
{
	m_find_text = pfc::getWindowText(m_window_map.at(IDC_EDIT_FIND));
	const bool enabled = m_find_text.get_length() > 0;
	m_window_map.at(IDC_BTN_NEXT).EnableWindow(enabled);
	m_window_map.at(IDC_BTN_PREVIOUS).EnableWindow(enabled && !FlagSet(m_flags, FindOption::RegExp));
	m_window_map.at(IDC_BTN_REPLACE).EnableWindow(enabled);
	m_window_map.at(IDC_BTN_REPLACE_ALL).EnableWindow(enabled);
}

void CDialogFindReplace::OnReplace(UINT, int, CWindow)
{
	if (m_havefound)
	{
		m_parent->Replace();
		m_havefound = false;
	}

	OnFindNext(0, 0, nullptr);
}

void CDialogFindReplace::OnReplaceAll(UINT, int, CWindow)
{
	m_parent->ReplaceAll();
}

void CDialogFindReplace::OnReplaceTextChange(UINT, int, CWindow)
{
	m_replace_text = pfc::getWindowText(m_window_map.at(IDC_EDIT_REPLACE));
}

void CDialogFindReplace::SetMode(Mode m)
{
	const bool find = m == Mode::find;
	const int cmd = find ? SW_HIDE : SW_SHOW;
	m_window_map.at(IDC_LABEL_REPLACE).ShowWindow(cmd);
	m_window_map.at(IDC_EDIT_REPLACE).ShowWindow(cmd);
	m_window_map.at(IDC_BTN_REPLACE).ShowWindow(cmd);
	m_window_map.at(IDC_BTN_REPLACE_ALL).ShowWindow(cmd);

	pfc::setWindowText(m_hWnd, find ? "Find" : "Replace");
	ShowWindow(SW_SHOW);
	SetFocus();
}
