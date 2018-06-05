// HyperLink.h : header file
//
//
// HyperLink static control. Will open the default browser with the given URL
// when the user clicks on the link.
//
// Copyright Chris Maunder, 1997, 1998
// Feel free to use and distribute. May not be sold for profit.

#if !defined(AFX_HYPERLINK_H__D1625061_574B_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_HYPERLINK_H__D1625061_574B_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
    #pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CHyperLink window

class CHyperLink : public CStatic
{
// Construction/destruction
    public:
        CHyperLink();
        virtual ~CHyperLink();
        
// Attributes
    public:
    
// Operations
    public:
        //�趨URL
        void SetURL(CString strURL);
        CString GetURL() const;
        //�趨��ɫ
        void SetColours(COLORREF crLinkColour, COLORREF crVisitedColour,
                        COLORREF crHoverColour = -1);
        //���������ɫ
        COLORREF GetLinkColour() const;
        //��ñ����ʺ����ɫ
        COLORREF GetVisitedColour() const;
        //�������ƶ����Ժ����ɫ
        COLORREF GetHoverColour() const;
        
        //�趨�Ƿ񱻷��ʹ�
        void SetVisited(BOOL bVisited = TRUE);
        //����Ƿ񱻷��ʹ�
        BOOL GetVisited() const;
        
        //�趨�����״
        void SetLinkCursor(HCURSOR hCursor);
        //��������״
        HCURSOR GetLinkCursor() const;
        //�趨�Ƿ����»���
        void SetUnderline(BOOL bUnderline = TRUE);
        //����Ƿ����»���
        BOOL GetUnderline() const;
        //�趨�Ƿ����Զ��ı��С
        void SetAutoSize(BOOL bAutoSize = TRUE);
        BOOL GetAutoSize() const;
        void SetOpenURL(BOOL bOpenURL = TRUE);
        
// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CHyperLink)	�����򵼶�λ�õ�
    public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
        virtual void PreSubclassWindow();
        //}}AFX_VIRTUAL
        
    protected:
        //���ӵ�URL
        HINSTANCE GotoURL(LPCTSTR url, int showcmd);
        //��ӡ����
        void ReportError(int nError);
        //���ע�����Ϣ
        LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
        //����λ��
        void PositionWindow();
        //�趨Ĭ�ϵ������״
        void SetDefaultCursor();
        
// ����
    protected:
        COLORREF m_crLinkColour, m_crVisitedColour;     // ����������ɫ
        COLORREF m_crHoverColour;                       // ���ͣ����ɫ
        BOOL     m_bOverControl;                        // �Ƿ�����Ƶ��ؼ���
        BOOL     m_bVisited;                            // �Ƿ񱻷���
        BOOL     m_bUnderline;                          // �Ƿ����»���
        BOOL     m_bAdjustToFit;                        // �Ƿ��Զ������ؼ���С
        CString  m_strURL;                              // URL
        CFont    m_Font;                                // �趨����
        HCURSOR  m_hLinkCursor;                         // ���
        CToolTipCtrl m_ToolTip;                         // ��ʾ����
        BOOL     m_bOpenURL;
        
    protected:
        //{{AFX_MSG(CHyperLink)
        afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
        afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        //}}AFX_MSG
        //afx_msg void OnClicked();
        DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPERLINK_H__D1625061_574B_11D1_ABBA_00A0243D1382__INCLUDED_)
