#pragma once
#include "afxcmn.h"
#include "CarCheckCommon.h"
#include "ShadeButtonST.h"
// CDialogParamSet �Ի���

class CDialogMsg : public CDialog
{
	DECLARE_DYNCREATE(CDialogMsg)

public:
	CDialogMsg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogMsg();


// �Ի�������
	enum { IDD = IDD_DIALOG_MSG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	void Init(const CString &msg,const CString &title="ϵͳ��ʾ",MsgBoxType msgType=eWarning);
	void SetIcon();
	void SetMsg();
	int  GetWindowWidth();
private:
	MsgBoxType  m_msgType;
	CString     m_msg;
	CString     m_title;
	bool        m_showCancelBtn;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CShadeButtonST m_btnOK;
	CShadeButtonST m_btnCancel;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
