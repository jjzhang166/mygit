#pragma once
#include "afxwin.h"
#include "../controls/BaseDialog.h"
#include "HyperLink.h"

// CRegisterDlg �Ի���
class CRegisterDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRegisterDlg();
	virtual BOOL OnInitDialog();
	enum { IDD = IDD_REGISTER_DIALOG };

	void GetSNByNIC();  //��ȡ������
	void WriteLicenseInformation(CString License);
	void WriteExitTimeReg();
	void initRegStatus(void);
	void InitStyle(void);

	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedCancel();
	CString getMachineCode(void);
	CString getCurrentDateTime();
// �Ի�������

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnClientDraw(CDC*pDC,INT nWidth,INT nHeight);
	void DrawText( CDC* pDC, CRect rcText,LPCTSTR lpszText,UINT uFormat,INT nHeight=18,bool bBold=true,LPCTSTR lpszName=TEXT("΢���ź�") );

	CImageEx m_titleImage;
	CString  m_strTitle;
	CButtonEx m_btnReg;
	CButtonEx m_btExit;

	DECLARE_MESSAGE_MAP()

private:
	CEdit m_strMCode;
	CString m_strEditSN;

};
