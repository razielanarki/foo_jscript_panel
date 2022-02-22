#include "ui_extension.h"

namespace win32_helpers {

void send_message_to_all_children(HWND wnd_parent, UINT msg, WPARAM wp, LPARAM lp)
{
    HWND wnd = GetWindow(wnd_parent, GW_CHILD);
    if (wnd)
        do {
            send_message_to_all_children(wnd, msg, wp, lp);
            SendMessage(wnd, msg, wp, lp);
        } while (wnd = GetWindow(wnd, GW_HWNDNEXT));
}

void send_message_to_direct_children(HWND wnd_parent, UINT msg, WPARAM wp, LPARAM lp)
{
    HWND wnd = GetWindow(wnd_parent, GW_CHILD);
    if (wnd)
        do {
            SendMessage(wnd, msg, wp, lp);
        } while (wnd = GetWindow(wnd, GW_HWNDNEXT));
}

int message_box(HWND wnd, const TCHAR* text, const TCHAR* caption, UINT type)
{
    modal_dialog_scope scope(wnd);
    return MessageBox(wnd, text, caption, type);
}

}; // namespace win32_helpers

int uHeader_SetItemWidth(HWND wnd, int n, UINT cx)
{
    uHDITEM hdi;
    memset(&hdi, 0, sizeof(hdi));
    hdi.mask = HDI_WIDTH;
    hdi.cxy = cx;
    return uHeader_InsertItem(wnd, n, &hdi, false);
}
int uHeader_SetItemText(HWND wnd, int n, const char* text)
{
    uHDITEM hdi;
    memset(&hdi, 0, sizeof(hdi));
    hdi.mask = HDI_TEXT;
    hdi.cchTextMax = NULL;
    hdi.pszText = const_cast<char*>(text);
    return uHeader_InsertItem(wnd, n, &hdi, false);
}

int uHeader_InsertItem(HWND wnd, int n, uHDITEM* hdi, bool insert)
{
    pfc::stringcvt::string_wide_from_utf8 text_utf16;

    HDITEMW hdw{};
    hdw.cxy = hdi->cxy;
    hdw.fmt = hdi->fmt;
    hdw.hbm = hdi->hbm;
    hdw.iImage = hdi->iImage;
    hdw.iOrder = hdi->iOrder;
    hdw.lParam = hdi->lParam;
    hdw.mask = hdi->mask;

    if (hdi->mask & HDI_TEXT) {
        if (hdi->pszText == LPSTR_TEXTCALLBACKA) {
            hdw.pszText = LPSTR_TEXTCALLBACKW;
        } else {
            text_utf16.convert(hdi->pszText);
            hdw.pszText = const_cast<WCHAR*>(text_utf16.get_ptr());
            hdw.cchTextMax = text_utf16.length();            
        }
    }

    return SendMessage(wnd, insert ? HDM_INSERTITEMW : HDM_SETITEMW, n, (LPARAM)&hdw);
}

BOOL uRebar_InsertItem(HWND wnd, int n, uREBARBANDINFO* rbbi, bool insert)
{
    pfc::stringcvt::string_wide_from_utf8 text_utf16;

    REBARBANDINFOW rbw{};
    rbw.cbSize = REBARBANDINFOW_V6_SIZE;

    rbw.fMask = rbbi->fMask;
    rbw.fStyle = rbbi->fStyle;
    rbw.clrFore = rbbi->clrFore;
    rbw.clrBack = rbbi->clrBack;

    if (rbbi->fMask & RBBIM_TEXT) {
        text_utf16.convert(rbbi->lpText);
        rbw.lpText = const_cast<WCHAR*>(text_utf16.get_ptr());
    }
    
    rbw.iImage = rbbi->iImage;
    rbw.hwndChild = rbbi->hwndChild;
    rbw.cxMinChild = rbbi->cxMinChild;
    rbw.cyMinChild = rbbi->cyMinChild;
    rbw.cx = rbbi->cx;
    rbw.hbmBack = rbbi->hbmBack;
    rbw.wID = rbbi->wID;
    rbw.cyChild = rbbi->cyChild;
    rbw.cyMaxChild = rbbi->cyMaxChild;
    rbw.cyIntegral = rbbi->cyIntegral;
    rbw.cxIdeal = rbbi->cxIdeal;
    rbw.lParam = rbbi->lParam;
    rbw.cxHeader = rbbi->cxHeader;

    return SendMessage(wnd, insert ? RB_INSERTBANDW : RB_SETBANDINFOW, n, (LPARAM)&rbw);
}

namespace win32_helpers {
BOOL tooltip_add_tool(HWND wnd, TOOLINFO* ti, bool update)
{
    BOOL rv = FALSE;
    rv = SendMessage(wnd, update ? TTM_UPDATETIPTEXT : TTM_ADDTOOL, 0, (LPARAM)ti);
    return rv;
}
} // namespace win32_helpers

BOOL uToolTip_AddTool(HWND wnd, uTOOLINFO* ti, bool update)
{
    pfc::stringcvt::string_wide_from_utf8 text_utf16;
    TOOLINFOW tiw{};

    tiw.cbSize = sizeof(tiw);
    tiw.uFlags = ti->uFlags;
    tiw.hwnd = ti->hwnd;
    tiw.uId = ti->uId;
    tiw.rect = ti->rect;
    tiw.hinst = ti->hinst;
    tiw.lParam = ti->lParam;

    if (ti->lpszText == LPSTR_TEXTCALLBACKA) {
        tiw.lpszText = LPSTR_TEXTCALLBACKW;
    } else if (IS_INTRESOURCE(ti->lpszText)) {
        tiw.lpszText = reinterpret_cast<LPWSTR>(ti->lpszText);
    } else {
        text_utf16.convert(ti->lpszText);
        tiw.lpszText = const_cast<WCHAR*>(text_utf16.get_ptr());
    }

    return SendMessage(wnd, update ? TTM_UPDATETIPTEXTW : TTM_ADDTOOLW, 0, (LPARAM)&tiw);
}

BOOL uTabCtrl_InsertItemText(HWND wnd, int idx, const char* text, bool insert)
{
    pfc::string8 temp2;
    uTCITEM tabs;
    memset(&tabs, 0, sizeof(uTCITEM));
    tabs.mask = TCIF_TEXT;
    uFixAmpersandChars_v2(text, temp2);
    tabs.pszText = const_cast<char*>(temp2.get_ptr());
    return insert ? uTabCtrl_InsertItem(wnd, idx, &tabs) : uTabCtrl_SetItem(wnd, idx, &tabs);
}

BOOL uComboBox_GetText(HWND combo, UINT index, pfc::string_base& out)
{
    unsigned len = SendMessage(combo, CB_GETLBTEXTLEN, index, 0);
    if (len == CB_ERR || len > 16 * 1024 * 1024)
        return FALSE;
    if (len == 0) {
        out.reset();
        return TRUE;
    }

    pfc::array_t<WCHAR> temp;
    temp.set_size(len + 1);
    if (temp.get_ptr() == 0)
        return FALSE;
    temp.fill(0);
    len = SendMessage(combo, CB_GETLBTEXT, index, (LPARAM)temp.get_ptr());
    if (len == CB_ERR)
        return false;
    out.set_string(pfc::stringcvt::string_utf8_from_wide(temp.get_ptr()));
    return TRUE;
}

BOOL uComboBox_SelectString(HWND combo, const char* src)
{
    unsigned idx = CB_ERR;
    idx = uSendMessageText(combo, CB_FINDSTRINGEXACT, -1, src);
    if (idx == CB_ERR)
        return false;
    SendMessage(combo, CB_SETCURSEL, idx, 0);
    return TRUE;
}

BOOL uStatus_SetText(HWND wnd, int part, const char* text)
{
    const pfc::stringcvt::string_wide_from_utf8 text_utf16(text);
    return SendMessage(wnd, SB_SETTEXTW, part, (LPARAM)(text_utf16.get_ptr()));
}

HFONT uCreateIconFont()
{
    LOGFONTW lf;
    memset(&lf, 0, sizeof(LOGFONTW));
    SystemParametersInfoW(SPI_GETICONTITLELOGFONT, 0, &lf, 0);

    return CreateFontIndirectW(&lf);
}

HFONT uCreateMenuFont(bool vertical)
{
    NONCLIENTMETRICSW ncm;
    memset(&ncm, 0, sizeof(NONCLIENTMETRICSW));
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    if (vertical) {
        ncm.lfMenuFont.lfEscapement = 900;
        ncm.lfMenuFont.lfOrientation = 900;
    }

    return CreateFontIndirectW(&ncm.lfMenuFont);
}

void uGetMenuFont(LOGFONT* p_lf)
{
    NONCLIENTMETRICS ncm;
    memset(&ncm, 0, sizeof(NONCLIENTMETRICS));
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);

    *p_lf = ncm.lfMenuFont;
}

void uGetIconFont(LOGFONT* p_lf)
{
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, 0, p_lf, 0);
}

BOOL uFormatMessage(DWORD dw_error, pfc::string_base& out)
{
    BOOL rv = FALSE;
    pfc::array_t<WCHAR> buffer;
    buffer.set_size(256);
    // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    if (FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, dw_error, 0, buffer.get_ptr(),
            buffer.get_size(), 0)) {
        out.set_string(pfc::stringcvt::string_utf8_from_wide(buffer.get_ptr()));
        rv = TRUE;
    }
    return rv;
}

HWND uRecursiveChildWindowFromPoint(HWND parent, POINT pt_parent)
{
    HWND wnd = RealChildWindowFromPoint(parent, pt_parent);
    if (wnd && wnd != parent) {
        HWND wnd_last = wnd;
        POINT pt = pt_parent;
        MapWindowPoints(parent, wnd_last, &pt, 1);
        for (;;) {
            wnd = RealChildWindowFromPoint(wnd_last, pt);
            if (!wnd)
                return 0;
            if (wnd == wnd_last)
                return wnd;
            MapWindowPoints(wnd_last, wnd, &pt, 1);
            wnd_last = wnd;
            RECT rc;
            GetClientRect(wnd_last, &rc);
            if (!PtInRect(&rc, pt))
                return wnd_last;
        }
    }
    return 0;
}

BOOL uGetMonitorInfo(HMONITOR monitor, LPMONITORINFO lpmi)
{
    BOOL rv = FALSE;
    if (lpmi->cbSize == sizeof(uMONITORINFOEX)) {
        uLPMONITORINFOEX lpmiex = (uLPMONITORINFOEX)lpmi;

        MONITORINFOEXW mi;
        memset(&mi, 0, sizeof(MONITORINFOEXW));
        mi.cbSize = sizeof(MONITORINFOEXW);

        rv = GetMonitorInfoW(monitor, &mi);

        lpmi->rcMonitor = mi.rcMonitor;
        lpmi->rcWork = mi.rcWork;
        lpmi->dwFlags = mi.dwFlags;

        pfc::stringcvt::string_utf8_from_wide temp(mi.szDevice, CCHDEVICENAME);
        strcpy_utf8_truncate(temp, lpmiex->szDevice, CCHDEVICENAME);
    } else
        rv = GetMonitorInfoW(monitor, lpmi);
    return rv;
}
