#include "StdAfx.h"
#include "../SDM.h"
#include "../SDMDlg.h"
#include "./DataCount.h"
#include "../database/DataExport.h"

#define IDC_DATACOUNT_COUNT WM_USER+0x100
#define IDC_DATACOUNT_COUNTALL WM_USER+0x101
#define IDC_DATACOUNT_DEL WM_USER+0x102
#define IDC_DATACOUNT_CLEAR WM_USER+0x103
#define IDC_DATACOUNT_EXPORT WM_USER+0x104
#define IDC_DATACOUNT_DELALL WM_USER+0x105

extern CDataBaseT* g_pDBT;

IMPLEMENT_DYNAMIC(CDataCountDlg, CBaseDialog)

CDataCountDlg::CDataCountDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(CDataCountDlg::IDD, pParent)
    , m_idChecked(FALSE)
{
    m_strTitle = "DataCount";
}

CDataCountDlg::~CDataCountDlg()
{
}

void CDataCountDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COUNT_IDCMB, m_idCombo);
    DDX_Control(pDX, IDC_COUNT_LIST, m_countList);
    DDX_Check(pDX, IDC_COUNT_IDCHECK, m_idChecked);
}


BEGIN_MESSAGE_MAP(CDataCountDlg, CBaseDialog)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_COUNT_BTN_QUERY, &CDataCountDlg::OnBnClickedBtnCount)
    ON_BN_CLICKED(IDC_COUNT_BTN_QUERYALL, &CDataCountDlg::OnBnClickedBtnCountall)
    ON_BN_CLICKED(IDC_COUNT_BTN_DEL, &CDataCountDlg::OnBnClickedBtnDel)
    ON_BN_CLICKED(IDC_COUNT_BTN_DELALL, OnBnClickedBtnDelall)
    ON_BN_CLICKED(IDC_COUNT_BTN_EXPORT, OnBnClickedBtnExport)
    ON_BN_CLICKED(IDC_BTN_CLR, &CDataCountDlg::OnBnClickedBtnClr)
    ON_BN_CLICKED(IDC_COUNT_IDCHECK, &CDataCountDlg::OnBnClickedCheckDoorid)
    ON_CBN_SELCHANGE(IDC_COUNT_IDCMB, &CDataCountDlg::OnCbnSelchangeCmbDoorid)
    
    ON_BN_CLICKED(IDC_DATACOUNT_COUNT, OnBnClickedBtnCount)
    ON_BN_CLICKED(IDC_DATACOUNT_COUNTALL, OnBnClickedBtnCountall)
    ON_BN_CLICKED(IDC_DATACOUNT_DEL, OnBnClickedBtnDel)
    ON_BN_CLICKED(IDC_DATACOUNT_CLEAR, OnBnClickedBtnClr)
    ON_BN_CLICKED(IDC_DATACOUNT_EXPORT, OnBnClickedBtnExport)
    ON_BN_CLICKED(IDC_DATACOUNT_DELALL, OnBnClickedBtnDelall)
    ON_NOTIFY(NM_CLICK, IDC_COUNT_LIST, &CDataCountDlg::OnNMClickCountList)
END_MESSAGE_MAP()


// CDataQueryDlg ��Ϣ�������


BOOL CDataCountDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();
    InitStyle();
    SetDevIdFormDB();
    InitListData();
    GetDataFormDB(_T("all"));
    ((CButton*)GetDlgItem(IDC_COUNT_IDCHECK))->SetCheck(1);
    OnBnClickedCheckDoorid();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CDataCountDlg::InitStyle(void)
{
    HINSTANCE hInstance = AfxGetInstanceHandle();
    LoadBackSkin(hInstance, TEXT("DATAQUERY_DLG_BG"));
    SetWindowPos(NULL, 0, 0, 1080, 700, SWP_NOMOVE);
    m_titleImage.LoadImage(hInstance, TEXT("DATAQUERY_DLG_TITLE"));
    //�Ի������С������Ļ����
    CRect rect;
    //��1��
    GetClientRect(&rect);//��ô���Ĵ�С
    //��2��
    //GetWindowRect(rectDlg);//��ô�������Ļ�ϵ�λ��
    //ScreenToClient(rectDlg);
    int left = rect.left;
    int top = rect.top;
    int right = rect.right;
    int bottom = rect.bottom;
    int deviceH = rect.Width() / 5;
    int deviceV = rect.Height() / 10;
    GetDlgItem(IDC_COUNT_CTRL_STATIC)->MoveWindow(left + 10, top + 40, deviceH * 4 - 10, deviceV * 2 - 40);
    m_countList.MoveWindow(left + 10, deviceV * 2 + 10, deviceH * 4 - 10, deviceV * 8 - 20);
    GetDlgItem(IDC_COUNT_STATIC_INFO)->MoveWindow(deviceH * 4 + 10, top + 40, deviceH - 20, rect.Height() - 50);
    GetDlgItem(IDC_COUNT_CTRL_STATIC)->GetWindowRect(&rect);
    ScreenToClient(&rect);
    top = rect.top;
    int devy = rect.Height() / 5;
    int posy = 3 * devy + top;
    GetDlgItem(IDC_COUNT_IDCHECK)->MoveWindow(30, top + 10, 150, 30);
    GetDlgItem(IDC_COUNT_IDCMB)->MoveWindow(180, top + 15, 200, 150);
    m_btnCount.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(30, posy, 0, 0), this, IDC_DATACOUNT_COUNT);
    m_btnCount.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnCount.SetWindowText("ͳ����ѡ");
    m_btnCountAll.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(180, posy, 0, 0), this, IDC_DATACOUNT_COUNTALL);
    m_btnCountAll.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnCountAll.SetWindowText("ͳ������");
    m_btnDel.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(330, posy, 0, 0), this, IDC_DATACOUNT_DEL);
    m_btnDel.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnDel.SetWindowText("ɾ��ָ��");
    m_btnDelAll.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(480, posy, 0, 0), this, IDC_DATACOUNT_DELALL);
    m_btnDelAll.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnDelAll.SetWindowText("ɾ��ȫ��");
    m_btnExport.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(630, posy, 0, 0), this, IDC_DATACOUNT_EXPORT);
    m_btnExport.SetButtonImage(hInstance, TEXT("BTN_EXIT"));
    m_btnExport.SetWindowText("���ݵ���");
    GetDlgItem(IDC_COUNT_STATIC_INFO)->GetWindowRect(&rect);
    ScreenToClient(&rect);
    left = rect.left + 5;
    top = rect.top + 5;
    right = rect.right;
    bottom = rect.bottom;
    deviceH = rect.Width() - 10;
    deviceV = rect.Height() / 20;
    GetDlgItem(IDC_COUNT_SELECT_STATIC_PASSED)->MoveWindow(left, top, deviceH, deviceV);
    GetDlgItem(IDC_COUNT_STATIC_PASSED_SHOW)->MoveWindow(left, top + deviceV, deviceH, deviceV * 4);
    GetDlgItem(IDC_COUNT_SELECT_STATIC_ALARM)->MoveWindow(left, top + deviceV * 5, deviceH, deviceV);
    GetDlgItem(IDC_COUNT_STATIC_ALARM_SHOW)->MoveWindow(left, top + deviceV * 6, deviceH, deviceV * 4);
    GetDlgItem(IDC_COUNT_ALL_STATIC_PASSED)->MoveWindow(left, top + deviceV * 10, deviceH, deviceV);
    GetDlgItem(IDC_COUNT_ALL_STATIC_PASSED_SHOW)->MoveWindow(left, top + deviceV * 11, deviceH, deviceV * 4);
    GetDlgItem(IDC_COUNT_ALL_STATIC_ALARM)->MoveWindow(left, top + deviceV * 15, deviceH, deviceV);
    GetDlgItem(IDC_COUNT_ALL_STATIC_ALARM_SHOW)->MoveWindow(left, top + deviceV * 16, deviceH, deviceV * 4);
    //CWnd *pWnd;
    //pWnd = GetDlgItem(IDC_BUTTON1);    //��ȡ�ؼ�ָ�룬IDC_BUTTON1Ϊ�ؼ�ID��
    //pWnd->SetWindowPos(NULL, 50, 80, 0, 0, SWP_NOZORDER | SWP_NOSIZE);    //�Ѱ�ť�Ƶ����ڵ�(50,80)��
    //pWnd = GetDlgItem(IDC_EDIT1);
    //pWnd->SetWindowPos(NULL, 0, 0, 100, 80, SWP_NOZORDER | SWP_NOMOVE);    //�ѱ༭�ؼ��Ĵ�С��Ϊ(100,80)��λ�ò���
    //pWnd = GetDlgItem(IDC_EDIT1);
    //pWnd->SetWindowPos(NULL, 0, 0, 100, 80, SWP_NOZORDER);
}

void CDataCountDlg::InitListData(void)
{
    m_countList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_countList.SetBkColor(RGB(247, 247, 255));
    m_countList.SetTextColor(RGB(0, 0, 255));
    m_countList.SetTextBkColor(RGB(247, 247, 255));
    CString str[] = { _T(""), _T("��ID"), _T("��ͨ����"), _T("�ܱ�����"), _T("���һ��ͨ����"), _T("���һ�챨����"), _T("���һ������") };
    
    for (int i = 0; i < sizeof(str) / sizeof(str[0]); i++)
    {
        int len = 150;
        
        if (i <= 1)
        {
            len = 100;
        }
        
        m_countList.InsertColumn(i, str[i], LVCFMT_CENTER, len);
    }
    
    m_countList.DeleteColumn(0);
}

void CDataCountDlg::OnClientDraw(CDC*pDC, INT nWidth, INT nHeight)
{
    m_titleImage.DrawImage(pDC, 5, 4, 24, 24);
    pDC->SetTextColor(RGB(255, 255, 255));
    CRect rcText(35, 10, 250, 26);
    DrawText(pDC, rcText, m_strTitle, DT_LEFT | DT_END_ELLIPSIS);
}

void CDataCountDlg::DrawText(CDC* pDC, CRect rcText, LPCTSTR lpszText, UINT uFormat, INT nHeight/*=18*/, bool bBold/*=true*/, LPCTSTR lpszName/*=TEXT("΢���ź�")*/)
{
    CFont font;
    font.CreateFont(nHeight, 0, 0, 0, bBold ? FW_BOLD : FW_NORMAL, 0, FALSE, 0, 0, 0, 0, 0, 0, lpszName);
    CFont*  pOldFont = pDC->SelectObject(&font);
    pDC->DrawText(lpszText, &rcText, uFormat);
    pDC->SelectObject(pOldFont);
}


void CDataCountDlg::OnBnClickedBtnCount()
{
    CString str;
    
    if (m_idChecked)
    {
        GetDlgItem(IDC_COUNT_IDCMB)->GetWindowText(str);
    }
    
    GetDataFormDB(str);
	CountDataShow();
}

void CDataCountDlg::CountDataShow()
{
	int ttpass = 0;
	int ttalarm = 0;
	int tdpass = 0;
	int tdalarm = 0;
	CString str1, str2, str3, str4;

	int lineCnt = m_countList.GetItemCount();
	for (int i = 0; i < lineCnt; i++)
	{
		str1 = m_countList.GetItemText(i, 1);
		str2 = m_countList.GetItemText(i, 2);
		str3 = m_countList.GetItemText(i, 3);
		str4 = m_countList.GetItemText(i, 4);
		ttpass += atoi(str1);
		ttalarm += atoi(str2);
		tdpass += atoi(str3);
		tdalarm += atoi(str4);
	}

	UpdateCountDataShow(tdpass, tdalarm, ttpass, ttalarm);
}

void CDataCountDlg::OnBnClickedBtnCountall()
{
    GetDataFormDB(_T("all"));
	CountDataShow();
    UpdateData(FALSE);
}


void CDataCountDlg::OnBnClickedBtnDel()
{
    CString str;
    CString doorId;
    GetDlgItem(IDC_COUNT_IDCMB)->GetWindowText(doorId);
    str.Format("ȷ��ɾ��������IDΪ '%s' ���������ݣ�", doorId);
    
    if (MessageBox(str, "ȷ��ɾ��", MB_YESNO | MB_DEFBUTTON1) == IDNO)
    {
        return;
    }
    
    CString strWhere;
    strWhere.Format(" where DOORID ='%s'", doorId);
    BOOL res;
    res = g_pDBT->DeleteRecord(Tb_ANNUNCIATOR, strWhere);
    
    if (res)
    {
        GetDataFormDB();
    }
}

void CDataCountDlg::OnBnClickedBtnDelall()
{
    CString str;
    str = "ȷ��ɾ�����а����ŵ����ݣ�";
    
    if (MessageBox(str, "ȷ��ɾ��", MB_YESNO | MB_DEFBUTTON1) == IDNO)
    {
        return;
    }
    
    CString strWhere = "";
    BOOL res;
    res = g_pDBT->DeleteRecord(Tb_ANNUNCIATOR, strWhere);
    
    if (res)
    {
        GetDataFormDB();
    }
}

void CDataCountDlg::OnBnClickedBtnExport()
{
    CString strReturnPath = DataExport::GetExePath();
    CTime curTime;
    curTime = CTime::GetCurrentTime();             //��ȡ��ǰʱ������
    CString filename = curTime.Format(_T("%Y%m%d%H%M%S"));
    filename.Format("%s\\%s.csv", strReturnPath, filename);
    //TRUEΪopen�Ի���false Ϊsave as �Ի���
    CFileDialog fg(false, _T("*.csv"), filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                   _T("Excel CSV File(*.csv)|*.csv|All File(*.*)|*.*||"), NULL);
                   
    if (fg.DoModal() == IDOK)
    {
        CString name = fg.GetFileName();
        
        if (name != "")
        {
            filename = fg.GetPathName();
        }
        
        if (DataExport::ExportCountToCsv(filename) == 1)
        {
            this->MessageBox("�ļ���ʧ�ܣ�");
            return;
        }
        else
        {
            this->MessageBox("�����ɹ���");
        }
    }
}


void CDataCountDlg::OnBnClickedCheckDoorid()
{
    m_idChecked = ((CButton*)GetDlgItem(IDC_COUNT_IDCHECK))->GetCheck();
    GetDlgItem(IDC_COUNT_IDCMB)->EnableWindow(m_idChecked);
    GetDlgItem(IDC_DATACOUNT_COUNT)->EnableWindow(m_idChecked);
    GetDlgItem(IDC_DATACOUNT_DEL)->EnableWindow(m_idChecked);
}

void CDataCountDlg::SetDevIdFormDB()
{
    CString str;
    char pWhere[150] = "";
    char pOrder[30] = "";
    LPCTSTR TableName = "DEVICES";
    m_idCombo.ResetContent();
    
    if (g_pDBT->OpenSql(TableName, "*", pWhere, pOrder))
    {
        int nItem = g_pDBT->GetDBRecordSet()->GetRecordCount();
        
        if (nItem > 0)
        {
            g_pDBT->GetDBRecordSet()->MoveFirst();
            CString ID, strBeizhu, passForwd, passBack, waring, data, time;
            
            while (!g_pDBT->GetDBRecordSet()->IsEOF())
            {
                g_pDBT->GetDBRecordSet()->GetValueString(ID, cnt_Endevices[0]);
                g_pDBT->GetDBRecordSet()->GetValueString(strBeizhu, cnt_Endevices[1]);
                m_idCombo.AddString(ID);
                g_pDBT->GetDBRecordSet()->MoveNext();
            }
        }
        
        g_pDBT->GetDBRecordSet()->Close();
    }
    
    m_idCombo.SetCurSel(0);
    UpdateData(FALSE);
}

void CDataCountDlg::GetDataFormDB(CString requireStr)
{
    m_countList.DeleteAllItems();
    char pWhere[150] = "";
    char pOrder[30] = "";
    LPCTSTR TableName = "CountTb";
    
    if (requireStr == "all")
    {
    }
    else
    {
        sprintf(pWhere, "where DOORID=%d", atoi(requireStr));
    }
    
    if (g_pDBT->OpenSql(TableName/*cnt_TableName[Tb_ANNUNCIATOR]*/, "*", pWhere, pOrder))
    {
        int nItem = g_pDBT->GetDBRecordSet()->GetRecordCount();
        
        if (nItem > 0)
        {
            g_pDBT->GetDBRecordSet()->MoveFirst();
            CString ID, doorID, totalPass, totalAlarm, todayPass, todayAlarm, date;
            m_countList.SetRedraw(FALSE);
            
            //��������
            while (!g_pDBT->GetDBRecordSet()->IsEOF())
            {
                g_pDBT->GetDBRecordSet()->GetValueString(ID, cnt_CountTb[0]);
                g_pDBT->GetDBRecordSet()->GetValueString(doorID, cnt_CountTb[1]);
                g_pDBT->GetDBRecordSet()->GetValueString(totalPass, cnt_CountTb[2]);
                g_pDBT->GetDBRecordSet()->GetValueString(totalAlarm, cnt_CountTb[3]);
                g_pDBT->GetDBRecordSet()->GetValueString(todayPass, cnt_CountTb[4]);
                g_pDBT->GetDBRecordSet()->GetValueString(todayAlarm, cnt_CountTb[5]);
                g_pDBT->GetDBRecordSet()->GetValueString(date, cnt_CountTb[6]);
                int nRow = m_countList.InsertItem(0, doorID);//������
                m_countList.SetItemText(nRow, 1, totalPass);//������
                m_countList.SetItemText(nRow, 2, totalAlarm);
                m_countList.SetItemText(nRow, 3, todayPass);//������
                m_countList.SetItemText(nRow, 4, todayAlarm);
                m_countList.SetItemText(nRow, 5, date);//������
                g_pDBT->GetDBRecordSet()->MoveNext();
            }
        }
        
        m_countList.SetRedraw(TRUE);
        m_countList.UpdateWindow();
        g_pDBT->GetDBRecordSet()->Close();
    }
}

void CDataCountDlg::OnCbnSelchangeCmbDoorid()
{
    CString strNew;
    //CString strPre, strNew;
    int nSel = m_idCombo.GetCurSel();
    m_idCombo.GetLBText(nSel, strNew);
    //m_comboBox.GetWindowText(strPre);
    //GetDlgItem(IDC_CMB_DOORID)->GetWindowText(strNew);
    GetDataFormDB(strNew);
}

void CDataCountDlg::OnBnClickedBtnClr()
{
    m_countList.DeleteAllItems();
}

//��̬�ؼ�����͸��������
HBRUSH CDataCountDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    //HBRUSH brush = CreateSolidBrush(RGB(127, 0, 0));
    CFont font;
    font.CreatePointFont(500, _T("����"));
    
    // TODO:  �ڴ˸��� DC ���κ�����
    
    switch (nCtlColor)
    {
        case CTLCOLOR_BTN://��ť��
            break;
            
        case CTLCOLOR_STATIC://��������Ҫ�Ļ��ľ�̬
            {
                if (pWnd->GetDlgCtrlID() == IDC_COUNT_STATIC_PASSED_SHOW ||
                        pWnd->GetDlgCtrlID() == IDC_COUNT_ALL_STATIC_PASSED_SHOW)
                {
                    pDC->SelectObject(&font);
                    pDC->SetBkColor(RGB(127, 0, 0));
                    pDC->SetBkMode(TRANSPARENT);
                    pDC->SetTextColor(RGB(0, 255, 0));
                    //return brush;
                }
                else if (pWnd->GetDlgCtrlID() == IDC_COUNT_STATIC_ALARM_SHOW ||
                         pWnd->GetDlgCtrlID() == IDC_COUNT_ALL_STATIC_ALARM_SHOW)
                {
                    pDC->SelectObject(&font);
                    pDC->SetBkColor(RGB(127, 0, 0));
                    pDC->SetBkMode(TRANSPARENT);
                    pDC->SetTextColor(RGB(255, 0, 0));
                    //return brush;
                }
                else
                {
                    pDC->SetBkMode(TRANSPARENT);//͸��
                    return  HBRUSH(GetStockObject(HOLLOW_BRUSH));
                }
            }
            break;
            
        default:
            break;
    }
    
    return hbr;
}

void CDataCountDlg::UpdateCountDataShow(int tdpass, int tdalarm, int ttpass, int ttalarm)
{
    //�������ʾ����
    GetDlgItem(IDC_COUNT_STATIC_PASSED_SHOW)->SetWindowText(_T(""));
    GetDlgItem(IDC_COUNT_STATIC_ALARM_SHOW)->SetWindowText(_T(""));
    GetDlgItem(IDC_COUNT_ALL_STATIC_PASSED_SHOW)->SetWindowText(_T(""));
    GetDlgItem(IDC_COUNT_ALL_STATIC_ALARM_SHOW)->SetWindowText(_T(""));
    CString str = _T("");
    
    if (tdpass != 0)
    {
        str.Format("%d", tdpass);
        GetDlgItem(IDC_COUNT_STATIC_PASSED_SHOW)->SetWindowText(str);
    }
    
    if (tdalarm != 0)
    {
        str.Format("%d", tdalarm);
        GetDlgItem(IDC_COUNT_STATIC_ALARM_SHOW)->SetWindowText(str);
    }
    
    if (ttpass != 0)
    {
        str.Format("%d", ttpass);
        GetDlgItem(IDC_COUNT_ALL_STATIC_PASSED_SHOW)->SetWindowText(str);
    }
    
    if (ttalarm != 0)
    {
        str.Format("%d", ttalarm);
        GetDlgItem(IDC_COUNT_ALL_STATIC_ALARM_SHOW)->SetWindowText(str);
    }
}

void CDataCountDlg::OnNMClickCountList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    POSITION pos = m_countList.GetFirstSelectedItemPosition(); //posѡ�е�����λ��
    int ttpass = 0;
    int ttalarm = 0;
    int tdpass = 0;
    int tdalarm = 0;
    
    //if (pos == NULL)
    //  AfxMessageBox("no item were selected!");
    //else
    if (pos != NULL)
    {
        CString str1, str2, str3, str4;
        
        while (pos)   //�����ѡ�����
        {
            int nIdx = -1;
            nIdx = m_countList.GetNextSelectedItem(pos);
            
            if (nIdx >= 0 && nIdx < m_countList.GetItemCount())
            {
                str1 = m_countList.GetItemText(nIdx, 1);//��ȡָ���У�Ϊ�ַ�����ʽ
                str2 = m_countList.GetItemText(nIdx, 2);
                str3 = m_countList.GetItemText(nIdx, 3);
                str4 = m_countList.GetItemText(nIdx, 4);
                ttpass += atoi(str1);
                ttalarm += atoi(str2);
                tdpass += atoi(str3);
                tdalarm += atoi(str4);
            }
        }
    }
    
    UpdateCountDataShow(tdpass, tdalarm, ttpass, ttalarm);
    *pResult = 0;
}
