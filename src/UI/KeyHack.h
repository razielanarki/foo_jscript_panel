#pragma once

class KeyHack : public CWindowImpl<KeyHack, CWindow>
{
public:
	BEGIN_MSG_MAP_EX(KeyHack)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		switch (wParam)
		{
		case '\n':
		case '\r':
		case '\t':
		case '\x1b':
			return 0;
		}
		return DefWindowProc(uMsg, wParam, lParam);
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		switch (wParam)
		{
		case VK_RETURN:
			GetParent().PostMessageW(WM_COMMAND, MAKEWPARAM(m_ret_cmd, BN_CLICKED), reinterpret_cast<LPARAM>(m_hWnd));
			return 0;
		case VK_ESCAPE:
			GetParent().PostMessageW(WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), reinterpret_cast<LPARAM>(m_hWnd));
			return 0;
		case VK_TAB:
			GetParent().PostMessageW(WM_NEXTDLGCTL, IsKeyPressed(VK_SHIFT) ? 1 : 0);
			return 0;
		}
		return DefWindowProc(uMsg, wParam, lParam);
	}

	int m_ret_cmd = 0;
};
