#pragma once
#include "afxwin.h"
#include "../controls/BaseDialog.h"
#include "../common/DataDefine.h"


// CLoginDlg �Ի���

class CLoginDlg : public CBaseDialog
{
        DECLARE_DYNAMIC(CLoginDlg)
        
    public:
        CLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
        virtual ~CLoginDlg();
        virtual BOOL OnInitDialog();
        
        void Init(int dayRemain);
        //void LoadImageLogo();
        void InitStyle(void);
        void FillUser();
        bool CheckUserAndPwd(CString username, CString pwd);
        
		afx_msg void OnBnClickedIpPort();
        afx_msg void OnBnClickedOk();
        afx_msg void OnBnClickedCancel();
        
        CComboBox m_cmbUserName;
        CString m_strpwd;
        CStatic m_loadUserImage;
        CMap<int, int, User, User> m_userLst;
        int m_dayRemain;
// �Ի�������
        enum { IDD = IDD_LOGIN_DIALOG };
        
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
        virtual void OnClientDraw(CDC*pDC, INT nWidth, INT nHeight);
        void DrawText(CDC* pDC, CRect rcText, LPCTSTR lpszText, UINT uFormat, INT nHeight = 18, bool bBold = true, LPCTSTR lpszName = TEXT("΢���ź�"));
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
        
        CImageEx m_titleImage;
        CImageEx m_UserImage;
        CString  m_strTitle;
		CButtonEx	m_btnIpPort;
        CButtonEx	m_btnLogin;
        CButtonEx	m_btExit;
        
        
        DECLARE_MESSAGE_MAP()
        
};
