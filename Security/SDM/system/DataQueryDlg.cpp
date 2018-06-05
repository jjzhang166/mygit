#include "StdAfx.h"
#include "../SDM.h"
#include "../SDMDlg.h"
#include "./DataQueryDlg.h"
#include "../database/DataExport.h"
//#include "../common/CreateDump.h"

#define IDC_DATAQUERY_QUERY WM_USER+0x50
#define IDC_DATAQUERY_QUERYALL WM_USER+0x51
#define IDC_DATAQUERY_DEL WM_USER+0x52
#define IDC_DATAQUERY_CLEAR WM_USER+0x53
#define IDC_DATAQUERY_EXPORT WM_USER+0x54
#define IDC_DATAQUERY_DELALL WM_USER+0x55

extern CDataBaseT*	g_pDBT;
extern BYTE g_curDoorId;
extern BYTE g_address[];
extern int g_currentDoorNum;
//extern CString GetExePath();

//static UINT WM_DELETE_UPDATE = RegisterWindowMessage(_T("User"));

IMPLEMENT_DYNAMIC(CDataQueryDlg, CBaseDialog)

CDataQueryDlg::CDataQueryDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(CDataQueryDlg::IDD, pParent)
    , m_findById(FALSE)
    , m_findByTime(FALSE)
{
    m_strTitle = "DataQuery";
}

CDataQueryDlg::~CDataQueryDlg()
{
}

void CDataQueryDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_CHECKRESULT, m_dataList);
    DDX_Control(pDX, IDC_REQUIRE_GROUP, m_requireGrp);
    DDX_Check(pDX, IDC_CHECK_DOORID, m_findById);
    DDX_Check(pDX, IDC_CHECK_TIME, m_findByTime);
    DDX_Control(pDX, IDC_CMB_DOORID, m_idCombo);
}


BEGIN_MESSAGE_MAP(CDataQueryDlg, CBaseDialog)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BTN_QUERY, &CDataQueryDlg::OnBnClickedBtnQuery)
    ON_BN_CLICKED(IDC_BTN_QUERYALL, &CDataQueryDlg::OnBnClickedBtnQueryall)
    ON_BN_CLICKED(IDC_BTN_DEL, &CDataQueryDlg::OnBnClickedBtnDel)
    ON_BN_CLICKED(IDC_CHECK_DOORID, &CDataQueryDlg::OnBnClickedCheckDoorid)
    ON_BN_CLICKED(IDC_CHECK_TIME, &CDataQueryDlg::OnBnClickedCheckTime)
    ON_BN_CLICKED(IDC_BTN_CLR, &CDataQueryDlg::OnBnClickedBtnClr)
    ON_CBN_SELCHANGE(IDC_CMB_DOORID, &CDataQueryDlg::OnCbnSelchangeCmbDoorid)
    ON_BN_CLICKED(IDC_DATAQUERY_QUERY, OnBnClickedBtnQuery)
    ON_BN_CLICKED(IDC_DATAQUERY_QUERYALL, OnBnClickedBtnQueryall)
    ON_BN_CLICKED(IDC_DATAQUERY_DEL, OnBnClickedBtnDel)
    ON_BN_CLICKED(IDC_DATAQUERY_CLEAR, OnBnClickedBtnClr)
    ON_BN_CLICKED(IDC_DATAQUERY_EXPORT, OnBnClickedBtnExport)
    ON_BN_CLICKED(IDC_DATAQUERY_DELALL, OnBnClickedBtnDelall)
END_MESSAGE_MAP()


// CDataQueryDlg ��Ϣ�������


BOOL CDataQueryDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();
    InitStyle();
    SetDevIdFormDB();
    InitListData();
    ((CButton*)GetDlgItem(IDC_CHECK_DOORID))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_CHECK_TIME))->SetCheck(0);
    OnBnClickedCheckDoorid();
    OnBnClickedCheckTime();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CDataQueryDlg::InitStyle(void)
{
    HINSTANCE hInstance = AfxGetInstanceHandle();
    LoadBackSkin(hInstance, TEXT("DATAQUERY_DLG_BG"));
    SetWindowPos(NULL, 0, 0, 1080, 700, SWP_NOMOVE);
    m_titleImage.LoadImage(hInstance, TEXT("DATAQUERY_DLG_TITLE"));
    //�Ի������С������Ļ����
    CRect rectDlg;
    //��1��
    GetClientRect(rectDlg);//��ô���Ĵ�С
    //��2��
    //GetWindowRect(rectDlg);//��ô�������Ļ�ϵ�λ��
    //ScreenToClient(rectDlg);
    m_dataList.MoveWindow(rectDlg.left + 10, rectDlg.top + 180, rectDlg.Width() - 20, rectDlg.Height() - 200);
    //m_UserImage.LoadImage(hInstance, TEXT("USER"));
    m_btnQuery.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(30, 120, 0, 0), this, IDC_DATAQUERY_QUERY);
    m_btnQuery.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnQuery.SetWindowText("��ѯָ��");
    m_btnQueryAll.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(180, 120, 0, 0), this, IDC_DATAQUERY_QUERYALL);
    m_btnQueryAll.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnQueryAll.SetWindowText("��ѯ����");
    m_btnDel.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(330, 120, 0, 0), this, IDC_DATAQUERY_DEL);
    m_btnDel.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnDel.SetWindowText("ɾ��ָ��");
    m_btnDelAll.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(480, 120, 0, 0), this, IDC_DATAQUERY_DELALL);
    m_btnDelAll.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnDelAll.SetWindowText("ɾ��ȫ��");
    m_btnClear.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(630, 120, 0, 0), this, IDC_DATAQUERY_CLEAR);
    m_btnClear.SetButtonImage(hInstance, TEXT("BTN_EXIT"));
    m_btnClear.SetWindowText("��ձ��");
    m_btnExport.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(780, 120, 0, 0), this, IDC_DATAQUERY_EXPORT);
    m_btnExport.SetButtonImage(hInstance, TEXT("BTN_EXIT"));
    m_btnExport.SetWindowText("���ݵ���");
    //CWnd *pWnd;
    //pWnd = GetDlgItem(IDC_BUTTON1);    //��ȡ�ؼ�ָ�룬IDC_BUTTON1Ϊ�ؼ�ID��
    //pWnd->SetWindowPos(NULL, 50, 80, 0, 0, SWP_NOZORDER | SWP_NOSIZE);    //�Ѱ�ť�Ƶ����ڵ�(50,80)��
    //pWnd = GetDlgItem(IDC_EDIT1);
    //pWnd->SetWindowPos(NULL, 0, 0, 100, 80, SWP_NOZORDER | SWP_NOMOVE);    //�ѱ༭�ؼ��Ĵ�С��Ϊ(100,80)��λ�ò���
    //pWnd = GetDlgItem(IDC_EDIT1);
    //pWnd->SetWindowPos(NULL, 0, 0, 100, 80, SWP_NOZORDER);
}

void CDataQueryDlg::InitListData(void)
{
    m_dataList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_dataList.SetBkColor(RGB(247, 247, 255));
    m_dataList.SetTextColor(RGB(0, 0, 255));
    m_dataList.SetTextBkColor(RGB(247, 247, 255));
    CString str[] = { _T(""), _T("��ID"), _T("ǰͨ������"), _T("��ͨ������"), _T("��������"), _T("��������"), _T("ʱ��") };
    
    for(int i = 0; i < sizeof(str) / sizeof(str[0]); i++)
    {
        int len = 150;
        
        if(i <= 1)
        {
            len = 100;
        }
        
        m_dataList.InsertColumn(i, str[i], LVCFMT_CENTER, len);
        //m_dataList.InsertItem(i, _T(""));
        //m_dataList.SetItemText(i, 0, _T("AAA"));
    }
    
    m_dataList.DeleteColumn(0);
    //m_dataList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);// |//LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES //
    ////LVS_EX_HEADERDRAGDROP | LVS_EX_ONECLICKACTIVATE );
    //m_dataList.SetBkColor(RGB(247, 247, 255));
    //m_dataList.SetTextColor(RGB(0, 0, 255));
    //m_dataList.SetTextBkColor(RGB(247, 247, 255));
    //m_dataList.InsertColumn(0, "��ID", LVCFMT_CENTER, 100);
    //m_dataList.InsertColumn(1, "ǰͨ������", LVCFMT_CENTER, 150);
    //m_dataList.InsertColumn(2, "��ͨ������", LVCFMT_CENTER, 150);
    //m_dataList.InsertColumn(3, "��������", LVCFMT_CENTER, 150);
    //m_dataList.InsertColumn(4, "��������", LVCFMT_CENTER, 150);
    //m_dataList.InsertColumn(5, "ʱ��", LVCFMT_CENTER, 150);
}

void CDataQueryDlg::OnClientDraw(CDC*pDC, INT nWidth, INT nHeight)
{
    m_titleImage.DrawImage(pDC, 5, 4, 24, 24);
    pDC->SetTextColor(RGB(255, 255, 255));
    CRect rcText(35, 10, 250, 26);
    DrawText(pDC, rcText, m_strTitle, DT_LEFT | DT_END_ELLIPSIS);
}

void CDataQueryDlg::DrawText(CDC* pDC, CRect rcText, LPCTSTR lpszText, UINT uFormat, INT nHeight/*=18*/, bool bBold/*=true*/, LPCTSTR lpszName/*=TEXT("΢���ź�")*/)
{
    CFont font;
    font.CreateFont(nHeight, 0, 0, 0, bBold ? FW_BOLD : FW_NORMAL, 0, FALSE, 0, 0, 0, 0, 0, 0, lpszName);
    CFont*  pOldFont = pDC->SelectObject(&font);
    pDC->DrawText(lpszText, &rcText, uFormat);
    pDC->SelectObject(pOldFont);
}


void CDataQueryDlg::OnBnClickedBtnQuery()
{
    CString str;
    
    if(m_findById)
    {
        GetDlgItem(IDC_CMB_DOORID)->GetWindowText(str);
    }
    
    if(m_findByTime)
    {
    }
    
    GetDataFormDB(str);
}


void CDataQueryDlg::OnBnClickedBtnQueryall()
{
    GetDataFormDB("all");
    UpdateData(FALSE);
}


void CDataQueryDlg::OnBnClickedBtnDel()
{
    CString str;
	CString doorId;
	GetDlgItem(IDC_CMB_DOORID)->GetWindowText(doorId);
	str.Format("ȷ��ɾ��������IDΪ '%s' ���������ݣ�", doorId);
    if(MessageBox(str, "ȷ��ɾ��", MB_YESNO | MB_DEFBUTTON1) == IDNO)
    {
        return;
    }
    
    CString strWhere;
    strWhere.Format(" where DOORID ='%s'", doorId);
    BOOL res;
    res = g_pDBT->DeleteRecord(Tb_ANNUNCIATOR, strWhere);
    
    if(res)
    {
        GetDataFormDB();
        CSDMDlg* pwnd = (CSDMDlg*)AfxGetMainWnd();
        ::SendMessage(pwnd->GetSafeHwnd(), WM_DELETE_UPDATE, 0, 0);//ɾ������ˢ����Ϣ
        //GetParent()->SendMessage(WM_DELETE_UPDATE, 0, 0);	//�����÷�����
    }
}

void CDataQueryDlg::OnBnClickedBtnDelall()
{
	CString str;
	str = "ȷ��ɾ�����а����ŵ����ݣ�";
    if(MessageBox(str, "ȷ��ɾ��", MB_YESNO | MB_DEFBUTTON1) == IDNO)
    {
        return;
    }
    
    CString strWhere = "";
    //CString str;
    //GetDlgItem(IDC_CMB_DOORID)->GetWindowText(str);
    //strWhere.Format(" where DOORID ='%s'", str);
    //char pWhere[150] = "";
    //char pOrder[30] = "";
    //LPCTSTR TableName = "ANNUNCIATOR";
    //else
    //{
    //	sprintf(pWhere, "where DOORID='%s'", requireStr);
    //}
    //if (g_pDBT->OpenSql(TableName/*cnt_TableName[Tb_ANNUNCIATOR]*/, "*", pWhere, pOrder))
    BOOL res;
    res = g_pDBT->DeleteRecord(Tb_ANNUNCIATOR, strWhere);
    
    if(res)
    {
        GetDataFormDB();
        CSDMDlg* pwnd = (CSDMDlg*)AfxGetMainWnd();
        ::SendMessage(pwnd->GetSafeHwnd(), WM_DELETE_UPDATE, 0, 0);//ɾ������ˢ����Ϣ
        //���Access���ݿ⡰�Զ���š��ֶ��Զ���������������
        //sprintf(pWhere, "insert into CUser(UserName,Pwd,RoleID,Des) values('%s','%s',%d,'%s')", m_strEditUserName, m_strEditNewPwd, RoleId, destemp);
        //strWhere = "ALTER TABLE ANNUNCIATOR ALTER COLUMN OrderId COUNTER(1, 1)";
        //BOOL retVal = g_pDBT->ExecSql(strWhere);
		//res = g_pDBT->DeleteRecord(Tb_DEVICES, strWhere);	//��������ɾ���󣬶�Ӧ���豸��ϢҪ��Ҫɾ�������ʵ��Ӧ�ò���Ҫ
    }
}

void CDataQueryDlg::OnBnClickedBtnExport()
{
    CString strReturnPath = DataExport::GetExePath();
	//CString strReturnPath = GetExePath();
    CString filename = "20180515.csv";
    filename.Format("%s\\20180515.csv", strReturnPath);
    //TRUEΪopen�Ի���false Ϊsave as �Ի���
    CFileDialog fg(false, _T("*.csv"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                   _T("Excel CSV File(*.csv)|*.csv|All File(*.*)|*.*||"), NULL);
    //		_T("Excel CSV File(*.csv)|Text File(*.txt)|*.txt|All File(*.*)|*.*||"), NULL);
    
    if(fg.DoModal() == IDOK)
    {
        CString name = fg.GetFileName();
        
        if(name != "")
        {
            filename = fg.GetPathName();
        }
        
		if (DataExport::ExportToCsv(filename) == 1)
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

void CDataQueryDlg::OnBnClickedCheckTime()
{
    m_findByTime = ((CButton*)GetDlgItem(IDC_CHECK_TIME))->GetCheck();
    GetDlgItem(IDC_DATETIMEPICKER_STARTDATE)->EnableWindow(m_findByTime);
    GetDlgItem(IDC_DATETIMEPICKER_STARTTIME)->EnableWindow(m_findByTime);
    GetDlgItem(IDC_DATETIMEPICKER_ENDDATE)->EnableWindow(m_findByTime);
    GetDlgItem(IDC_DATETIMEPICKER_ENDTIME)->EnableWindow(m_findByTime);
}


void CDataQueryDlg::OnBnClickedCheckDoorid()
{
    m_findById = ((CButton*)GetDlgItem(IDC_CHECK_DOORID))->GetCheck();
    GetDlgItem(IDC_CMB_DOORID)->EnableWindow(m_findById);
	GetDlgItem(IDC_DATAQUERY_QUERY)->EnableWindow(m_findById);
	GetDlgItem(IDC_DATAQUERY_DEL)->EnableWindow(m_findById);
}

void CDataQueryDlg::SetDevIdFormDB()
{
    CString str;
    char pWhere[150] = "";
    char pOrder[30] = "";
    LPCTSTR TableName = "DEVICES";
    m_idCombo.ResetContent();
    
    if(g_pDBT->OpenSql(TableName, "*", pWhere, pOrder))
    {
        int nItem = g_pDBT->GetDBRecordSet()->GetRecordCount();
        
        //allreocd = nItem;
        if(nItem > 0)
        {
            g_pDBT->GetDBRecordSet()->MoveFirst();
            //int ID;
            CString ID, strBeizhu, passForwd, passBack, waring, data, time;
            
            //��������
            while(!g_pDBT->GetDBRecordSet()->IsEOF())
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

void CDataQueryDlg::GetDataFormDB(CString requireStr)
{
    m_dataList.DeleteAllItems();
    char pWhere[150] = "";
    char pOrder[30] = "";
    LPCTSTR TableName = "ANNUNCIATOR";
    
    if(requireStr == "all")
    {
    }
    else
    {
        sprintf(pWhere, "where DOORID='%s'", requireStr);
    }
    
    if(g_pDBT->OpenSql(TableName/*cnt_TableName[Tb_ANNUNCIATOR]*/, "*", pWhere, pOrder))
    {
        int nItem = g_pDBT->GetDBRecordSet()->GetRecordCount();
        
        if(nItem > 0)
        {
            g_pDBT->GetDBRecordSet()->MoveFirst();
            CString ID, doorID, passForwd, passBack, waring, data, time;
            m_dataList.SetRedraw(FALSE);
            
            //��������
            while(!g_pDBT->GetDBRecordSet()->IsEOF())
            {
                g_pDBT->GetDBRecordSet()->GetValueString(ID, cnt_Enannunciator[0]);
                g_pDBT->GetDBRecordSet()->GetValueString(doorID, cnt_Enannunciator[1]);
                g_pDBT->GetDBRecordSet()->GetValueString(passForwd, cnt_Enannunciator[2]);
                g_pDBT->GetDBRecordSet()->GetValueString(passBack, cnt_Enannunciator[3]);
                g_pDBT->GetDBRecordSet()->GetValueString(waring, cnt_Enannunciator[4]);
                g_pDBT->GetDBRecordSet()->GetValueString(data, cnt_Enannunciator[5]);
                g_pDBT->GetDBRecordSet()->GetValueString(time, cnt_Enannunciator[6]);
                int nRow = m_dataList.InsertItem(0, doorID);//������
                m_dataList.SetItemText(nRow, 1, passForwd);//������
                m_dataList.SetItemText(nRow, 2, passBack);
                m_dataList.SetItemText(nRow, 3, waring);//������
                m_dataList.SetItemText(nRow, 4, data);
                m_dataList.SetItemText(nRow, 5, time);//������
                g_pDBT->GetDBRecordSet()->MoveNext();
            }
        }
        
        m_dataList.SetRedraw(TRUE);
        m_dataList.UpdateWindow();
        g_pDBT->GetDBRecordSet()->Close();
    }
}

void CDataQueryDlg::OnCbnSelchangeCmbDoorid()
{
    CString strNew;
    //CString strPre, strNew;
    int nSel = m_idCombo.GetCurSel();
    m_idCombo.GetLBText(nSel, strNew);
    //m_comboBox.GetWindowText(strPre);
    //GetDlgItem(IDC_CMB_DOORID)->GetWindowText(strNew);
    GetDataFormDB(strNew);
}

void CDataQueryDlg::OnBnClickedBtnClr()
{
    m_dataList.DeleteAllItems();
}

//��̬�ؼ�����͸��������
HBRUSH CDataQueryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    switch(nCtlColor)
    {
        case CTLCOLOR_BTN://��ť��
            break;
            
        case CTLCOLOR_STATIC://��������Ҫ�Ļ��ľ�̬
            {
                pDC->SetBkMode(TRANSPARENT);//͸��
                return  HBRUSH(GetStockObject(HOLLOW_BRUSH));
            }
            break;
            
        default:
            break;
    }
    
    return(CDialog::OnCtlColor(pDC, pWnd, nCtlColor));
}