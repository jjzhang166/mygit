
// SDMDlg.h : ͷ�ļ�
//

#pragma once
#include "./controls/BaseDialog.h"
#include "./network/IOCPServer.h"
#include "afxwin.h"
#include "afxcmn.h"

typedef struct UnitCount
{
	int id;				//��¼���������ŵ�id��
	int pass;			//��¼�����������ϴ���ͨ����
	int alarm;			//��¼�����������ϴ��ı�����
	int todayPass;		//��¼���������ŵ����ͨ������
	int todayAlarm;		//��¼���������ŵ���ı�������
	int totalPass;		//��¼������������ʷ�ϴ���ͨ����
	int totalAlarm;		//��¼������������ʷ�ϴ��ı�����
	CString todayDate;	//��¼�������������ݸ����������
}UnitCountSt;

// CSDMDlg �Ի���
class CSDMDlg : public CBaseDialog
{
// ����
    public:
        CSDMDlg(CWnd* pParent = NULL);  // ��׼���캯��
        
// �Ի�������
        enum { IDD = IDD_SDM_DIALOG };
        
        afx_msg void OnBnClickedDataQuery();
        afx_msg void OnBnClickedUserManage();
        afx_msg void OnBnClickedDevManage();
        afx_msg void OnBnClickedSystemSet();
		afx_msg void OnBnClickedCountInfo();
        afx_msg void OnBnClickedExit();
        afx_msg void OnBnClickedExport();
        afx_msg void OnPaint();
        afx_msg void OnNMClickListDevinfo(NMHDR *pNMHDR, LRESULT *pResult);
        afx_msg void OnTvnSelchangedDevTree(NMHDR *pNMHDR, LRESULT *pResult);
        
        void InitStyle(void);
        void AddClient();
        void DecreaseCliect();
        
        CIOCPServer m_IOCPServer;
        CListBox m_LogListBox;
        CStatic m_SDDataCtrl;
        CImageList m_ImageList;
        
        DetectData m_data;  //���ݸ����û���
        CTreeCtrl m_DevTree;
        CListCtrl m_DevInfoList;
        BYTE m_alarmStatus[16]; //6�������״̬

        int m_clientNum;
        int m_areaCnt;		//�������
		int m_totalPassedToday;	//�������ͨ������
		int m_totalAlarmedToday;		//������ı�������
        
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
        virtual void OnClientDraw(CDC*pDC, INT nWidth, INT nHeight);
        virtual BOOL OnInitDialog();
        void DrawText(CDC* pDC, CRect rcText, LPCTSTR lpszText, UINT uFormat, INT nHeight = 18, bool bBold = true, LPCTSTR lpszName = TEXT("΢���ź�"));
        
        
        // ���ɵ���Ϣӳ�亯��
		afx_msg void OnClose(/*UINT nID, LPARAM lParam*/);
        afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
        //afx_msg LRESULT MyMessage(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnUpdateTable(WPARAM wParam, LPARAM lParam);    //�����Զ�����Ϣ������
        afx_msg LRESULT OnGetDoorId(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnDeleteUpdate(WPARAM wParam, LPARAM lParam);
        //afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnDestroy();
        afx_msg void OnTimer(UINT nIDEvent);
        
        void DeviceSync();
        void OpenOrCloseTime(BOOL res);
        
        HICON m_hIcon;
        CImageEx m_titleImage;
        CString  m_strTitle;
        CButtonEx m_btnDataQuery;
        CButtonEx m_btnUserManage;
        CButtonEx m_btnDevManage;
        CButtonEx m_btnSystemSet;
		CButtonEx m_btnCountInfo;
        CButtonEx m_btnExit;
        CButtonEx m_btnExport;
        
        DECLARE_MESSAGE_MAP()
        
        
    private:
        int  GetFullWidth();
        int  GetFullHeight();
        BOOL InitNetWork();
        void DeInitializeNetWork();
        char* WCharToChar(const wchar_t* pwch, char* pch,
                          size_t* pnewlen = NULL);
        wchar_t* CharToWChar(const char* pch, wchar_t* pwch,
                             size_t* pnewlen = NULL);
        void InitTreeCtrl();
        void InitDevinfoList();
        void LoadSecurityDoor();
        void LoadBitmap();
        void GetDataFormDB();
		void ReadCountInfo();
        void GetDataFormDB(CString doorID);
        void GetDevFormDB(CString strName, CString &returnName);
		void GetDataFormCountTb();
		UnitCountSt GetDataFormCountTb(CString doorID);	//�����ݿ��ж�ȡ�����ŵ�ͳ����Ϣ
        
        CRect       m_clientRect;       //�����������λ��
        CRect       m_screenRect;       //������Ļ�����λ��
        CDC			m_dcMem;//�����ڴ�DCָ��
        BOOL		m_isFirstEnter;
		CList<UnitCountSt, UnitCountSt&> m_cntList;
        
};
