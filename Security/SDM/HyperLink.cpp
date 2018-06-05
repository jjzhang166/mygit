// HyperLink.cpp : implementation file
//
// HyperLink static control. Will open the default browser with the given URL
// when the user clicks on the link.
//
// Copyright (C) 1997, 1998 Chris Maunder (chrismaunder@codeguru.com)
// All rights reserved. May not be sold for profit.
//
// Thanks to P�l K. T�nder for auto-size and window caption changes.
//
// "GotoURL" function by Stuart Patterson
// As seen in the August, 1997 Windows Developer's Journal.
// Copyright 1997 by Miller Freeman, Inc. All rights reserved.
// Modified by Chris Maunder to use TCHARs instead of chars.
//
// "Default hand cursor" from Paul DiLascia's Jan 1998 MSJ article.

#include "stdafx.h"
#include "HyperLink.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

#define TOOLTIP_ID 1

/////////////////////////////////////////////////////////////////////////////
// CHyperLink

CHyperLink::CHyperLink()
{
    m_hLinkCursor       = NULL;                 // No cursor as yet
    m_crLinkColour      = RGB(85, 136, 232); // Blue
    m_crVisitedColour   = RGB(0, 0, 255);  // Purple
    m_crHoverColour     = ::GetSysColor(COLOR_HIGHLIGHT);
    m_bOverControl      = FALSE;                // Cursor not yet over control
    m_bVisited          = FALSE;                // Hasn't been visited yet.
    m_bUnderline        = TRUE;                 // Underline the link?
    m_bAdjustToFit      = TRUE;                 // Resize the window to fit the text?
    m_strURL.Empty();
    m_bOpenURL          = TRUE;
}

CHyperLink::~CHyperLink()
{
    m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CHyperLink, CStatic)
    //{{AFX_MSG_MAP(CHyperLink)
    //ON_CONTROL_REFLECT(STN_CLICKED, OnClicked)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHyperLink message handlers

BOOL CHyperLink::PreTranslateMessage(MSG* pMsg)
{
    m_ToolTip.RelayEvent(pMsg);
    return CStatic::PreTranslateMessage(pMsg);
}

//void CHyperLink::OnClicked()
//{
//	if (m_bOpenURL)
//	{
//		int result = (int)GotoURL(m_strURL, SW_SHOW);
//		m_bVisited = (result > HINSTANCE_ERROR);
//		if (!m_bVisited) {
//			MessageBeep(MB_ICONEXCLAMATION);     // Unable to follow link
//			ReportError(result);
//		} else
//			SetVisited();                        // Repaint to show visited colour
//	}
//}

HBRUSH CHyperLink::CtlColor(CDC* pDC, UINT nCtlColor)
{
    ASSERT(nCtlColor == CTLCOLOR_STATIC);
    
    if(m_bOverControl)
    {
        pDC->SetTextColor(m_crHoverColour);
    }
    else if(m_bVisited)
    {
        pDC->SetTextColor(m_crVisitedColour);
    }
    else
    {
        pDC->SetTextColor(m_crLinkColour);
    }
    
    // transparent text.
    pDC->SetBkMode(TRANSPARENT);
    return (HBRUSH)GetStockObject(NULL_BRUSH);
}

//����ƶ��¼�
void CHyperLink::OnMouseMove(UINT nFlags, CPoint point)
{
    CStatic::OnMouseMove(nFlags, point);
    
    //�ж��Ƿ�����ڿؼ��Ϸ�
    if(m_bOverControl)
    {
        CRect rect;
        GetClientRect(rect);
        
        if(!rect.PtInRect(point))
        {
            m_bOverControl = FALSE;
            ReleaseCapture();
            RedrawWindow();
            return;
        }
    }
    else                      // ����ƹ��ؼ�
    {
        m_bOverControl = TRUE;
        RedrawWindow();
        SetCapture();
    }
}

BOOL CHyperLink::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
    if(m_hLinkCursor)
    {
        ::SetCursor(m_hLinkCursor);
        return TRUE;
    }
    
    return FALSE;
}

void CHyperLink::PreSubclassWindow()
{
    // �����굥���¼�
    DWORD dwStyle = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
    
    // ���URLΪ�գ��趨Ϊ��������
    if(m_strURL.IsEmpty())
    {
        GetWindowText(m_strURL);
    }
    
    // ͬʱ��鴰������Ƿ�Ϊ�գ����Ϊ�����趨ΪURL
    CString strWndText;
    GetWindowText(strWndText);
    strWndText = "TEST";
    
    if(strWndText.IsEmpty())
    {
        ASSERT(!m_strURL.IsEmpty());
        SetWindowText(m_strURL);
    }
    
    // ��������
    LOGFONT lf;
    GetFont()->GetLogFont(&lf);
    lf.lfUnderline = m_bUnderline;
    m_Font.CreateFontIndirect(&lf);
    SetFont(&m_Font);
    PositionWindow();        // ���������С
    SetDefaultCursor();      // �趨Ĭ�������״
    //������ʾ��Ϣ
    CRect rect;
    GetClientRect(rect);
    m_ToolTip.Create(this);
    m_ToolTip.AddTool(this, m_strURL, rect, TOOLTIP_ID);
    CStatic::PreSubclassWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink operations

//�趨URL
void CHyperLink::SetURL(CString strURL)
{
    m_strURL = strURL;
    
    if(::IsWindow(GetSafeHwnd()))
    {
        SetWindowText(strURL);
        PositionWindow();
        m_ToolTip.UpdateTipText(strURL, this, TOOLTIP_ID);
    }
}

CString CHyperLink::GetURL() const
{
    return m_strURL;
}

//�趨��ɫ
void CHyperLink::SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,
                            COLORREF crHoverColour /* = -1 */)
{
    m_crLinkColour    = crLinkColour;
    m_crVisitedColour = crVisitedColour;
    
    if(crHoverColour == -1)
    {
        m_crHoverColour = ::GetSysColor(COLOR_HIGHLIGHT);
    }
    else
    {
        m_crHoverColour = crHoverColour;
    }
    
    if(::IsWindow(m_hWnd))
    {
        Invalidate();
    }
}

COLORREF CHyperLink::GetLinkColour() const
{
    return m_crLinkColour;
}

COLORREF CHyperLink::GetVisitedColour() const
{
    return m_crVisitedColour;
}

COLORREF CHyperLink::GetHoverColour() const
{
    return m_crHoverColour;
}

void CHyperLink::SetVisited(BOOL bVisited /* = TRUE */)
{
    m_bVisited = bVisited;
    
    if(::IsWindow(GetSafeHwnd()))
    {
        Invalidate();
    }
}

BOOL CHyperLink::GetVisited() const
{
    return m_bVisited;
}

void CHyperLink::SetLinkCursor(HCURSOR hCursor)
{
    m_hLinkCursor = hCursor;
    
    if(m_hLinkCursor == NULL)
    {
        SetDefaultCursor();
    }
}

HCURSOR CHyperLink::GetLinkCursor() const
{
    return m_hLinkCursor;
}

//�����»���
void CHyperLink::SetUnderline(BOOL bUnderline /* = TRUE */)
{
    m_bUnderline = bUnderline;
    
    if(::IsWindow(GetSafeHwnd()))
    {
        LOGFONT lf;
        GetFont()->GetLogFont(&lf);
        lf.lfUnderline = m_bUnderline;
        m_Font.DeleteObject();
        m_Font.CreateFontIndirect(&lf);
        SetFont(&m_Font);
        Invalidate();
    }
}

BOOL CHyperLink::GetUnderline() const
{
    return m_bUnderline;
}

void CHyperLink::SetAutoSize(BOOL bAutoSize /* = TRUE */)
{
    m_bAdjustToFit = bAutoSize;
    
    if(::IsWindow(GetSafeHwnd()))
    {
        PositionWindow();
    }
}

BOOL CHyperLink::GetAutoSize() const
{
    return m_bAdjustToFit;
}


// Move and resize the window so that the window is the same size
// as the hyperlink text. This stops the hyperlink cursor being active
// when it is not directly over the text. If the text is left justified
// then the window is merely shrunk, but if it is centred or right
// justified then the window will have to be moved as well.
//
// Suggested by P�l K. T�nder

void CHyperLink::PositionWindow()
{
    if(!::IsWindow(GetSafeHwnd()) || !m_bAdjustToFit)
    {
        return;
    }
    
    // Get the current window position
    CRect rect;
    GetWindowRect(rect);
    CWnd* pParent = GetParent();
    
    if(pParent)
    {
        pParent->ScreenToClient(rect);
    }
    
    // Get the size of the window text
    CString strWndText;
    GetWindowText(strWndText);
    CDC* pDC = GetDC();
    CFont* pOldFont = pDC->SelectObject(&m_Font);
    CSize Extent = pDC->GetTextExtent(strWndText);
    pDC->SelectObject(pOldFont);
    ReleaseDC(pDC);
    // Get the text justification via the window style
    DWORD dwStyle = GetStyle();
    
    // Recalc the window size and position based on the text justification
    if(dwStyle & SS_CENTERIMAGE)
    {
        rect.DeflateRect(0, (rect.Height() - Extent.cy) / 2);
    }
    else
    {
        rect.bottom = rect.top + Extent.cy;
    }
    
    if(dwStyle & SS_CENTER)
    {
        rect.DeflateRect((rect.Width() - Extent.cx) / 2, 0);
    }
    else if(dwStyle & SS_RIGHT)
    {
        rect.left  = rect.right - Extent.cx;
    }
    else // SS_LEFT = 0, so we can't test for it explicitly
    {
        rect.right = rect.left + Extent.cx;
    }
    
    // Move the window
    SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER);
}

/////////////////////////////////////////////////////////////////////////////
// CHyperLink implementation

// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
// It loads a "hand" cursor from the winhlp32.exe module
void CHyperLink::SetDefaultCursor()
{
    if(m_hLinkCursor == NULL)                 // No cursor handle - load our own
    {
        // Get the windows directory
        CString strWndDir;
        GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
        strWndDir.ReleaseBuffer();
        strWndDir += _T("\\winhlp32.exe");
        // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
        HMODULE hModule = LoadLibrary(strWndDir);
        
        if(hModule)
        {
            HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
            
            if(hHandCursor)
            {
                m_hLinkCursor = CopyCursor(hHandCursor);
            }
        }
        
        FreeLibrary(hModule);
    }
}

LONG CHyperLink::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);
    
    if(retval == ERROR_SUCCESS)
    {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata, data);
        RegCloseKey(hkey);
    }
    
    return retval;
}

void CHyperLink::ReportError(int nError)
{
    CString str;
    
    switch(nError)
    {
        case 0:
            str = "The operating system is out\nof memory or resources.";
            break;
            
        case SE_ERR_PNF:
            str = "The specified path was not found.";
            break;
            
        case SE_ERR_FNF:
            str = "The specified file was not found.";
            break;
            
        case ERROR_BAD_FORMAT:
            str = "The .EXE file is invalid\n(non-Win32 .EXE or error in .EXE image).";
            break;
            
        case SE_ERR_ACCESSDENIED:
            str = "The operating system denied\naccess to the specified file.";
            break;
            
        case SE_ERR_ASSOCINCOMPLETE:
            str = "The filename association is\nincomplete or invalid.";
            break;
            
        case SE_ERR_DDEBUSY:
            str = "The DDE transaction could not\nbe completed because other DDE transactions\nwere being processed.";
            break;
            
        case SE_ERR_DDEFAIL:
            str = "The DDE transaction failed.";
            break;
            
        case SE_ERR_DDETIMEOUT:
            str = "The DDE transaction could not\nbe completed because the request timed out.";
            break;
            
        case SE_ERR_DLLNOTFOUND:
            str = "The specified dynamic-link library was not found.";
            break;
            
        case SE_ERR_NOASSOC:
            str = "There is no application associated\nwith the given filename extension.";
            break;
            
        case SE_ERR_OOM:
            str = "There was not enough memory to complete the operation.";
            break;
            
        case SE_ERR_SHARE:
            str = "A sharing violation occurred. ";
            
        default:
            str.Format("Unknown Error (%d) occurred.", nError);
            break;
    }
    
    str = "Unable to open hyperlink:\n\n" + str;
    AfxMessageBox(str, MB_ICONEXCLAMATION | MB_OK);
}

//���ӵ�Ŀ���ַ
HINSTANCE CHyperLink::GotoURL(LPCTSTR url, int showcmd)
{
    TCHAR key[MAX_PATH + MAX_PATH];
    // ���ú���ShellExecute()
    HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL, NULL, showcmd);
    
    // �����������ע�����.htm�ļ���ע���ֵ
    if((UINT)result <= HINSTANCE_ERROR)
    {
        if(GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS)
        {
            lstrcat(key, _T("\\shell\\open\\command"));
            
            if(GetRegKey(HKEY_CLASSES_ROOT, key, key) == ERROR_SUCCESS)
            {
                TCHAR *pos;
                pos = _tcsstr(key, _T("\"%1\""));
                
                if(pos == NULL)                        // û�з���
                {
                    pos = strstr(key, _T("%1"));       // ���%1
                    
                    if(pos == NULL)                    // û�в���
                    {
                        pos = key + lstrlen(key) - 1;
                    }
                    else
                    {
                        *pos = '\0';    // ɾ������
                    }
                }
                else
                {
                    *pos = '\0';    // ɾ������
                }
                
                lstrcat(pos, _T(" "));
                lstrcat(pos, url);
                result = (HINSTANCE) WinExec(key, showcmd);
            }
        }
    }
    
    return result;
}


void CHyperLink::SetOpenURL(BOOL bOpenURL/* =TRUE */)
{
    m_bOpenURL = bOpenURL;
}