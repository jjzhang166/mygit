// SystemSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "../SDM.h"
#include "./SystemSetDlg.h"

#define IDC_SYSQUERY_QUERY WM_USER+0x80
#define IDC_SYSQUERY_QUERYALL WM_USER+0x81
#define IDC_SYSQUERY_DEL WM_USER+0x82
#define IDC_SYSQUERY_CLEAR WM_USER+0x83

extern BYTE g_curDoorId;
extern BYTE g_address[];
extern int g_currentDoorNum;
extern ClientInfo *g_CurClientInfo;
extern CIOCPServer *g_server;
// CSystemSetDlg �Ի���
IMPLEMENT_DYNAMIC(CSystemSetDlg, CBaseDialog)

CSystemSetDlg::CSystemSetDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(CSystemSetDlg::IDD, pParent), m_sensitivityArea1(3900),
      m_sensitivityArea2(3900), m_sensitivityArea3(3900), m_sensitivityArea4(3900),
      m_sensitivityArea5(3900), m_sensitivityArea6(3900), m_sensitivityAll(0)
    , m_radioAutoFrequencyVal(1)
    , m_radioLanguageVal(0)
    , m_radioRingModeVal(0)
    , m_radioAlarmModeVal(0)
    , m_radioAreaTypeVal(2)
    , m_radioCheckModeVal(0)
    , m_radioAlarmAreaVal(0)
    , m_radioInfrateModeVal(0)
    , m_radioFilterModeVal(3)
{
    m_strTitle = "SystemSet";
    m_pDlg = NULL;
}

CSystemSetDlg::~CSystemSetDlg()
{
    theApp.m_SDCommData.RecoverPreCtrlWnd();
}


void CSystemSetDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_sensitivityArea1);
    DDV_MinMaxLong(pDX, m_sensitivityArea1, 0, 3999);
    DDX_Text(pDX, IDC_EDIT2, m_sensitivityArea2);
    DDV_MinMaxLong(pDX, m_sensitivityArea2, 0, 3999);
    DDX_Text(pDX, IDC_EDIT3, m_sensitivityArea3);
    DDV_MinMaxLong(pDX, m_sensitivityArea3, 0, 3999);
    DDX_Text(pDX, IDC_EDIT4, m_sensitivityArea4);
    DDV_MinMaxLong(pDX, m_sensitivityArea4, 0, 3999);
    DDX_Text(pDX, IDC_EDIT5, m_sensitivityArea5);
    DDV_MinMaxLong(pDX, m_sensitivityArea5, 0, 3999);
    DDX_Text(pDX, IDC_EDIT6, m_sensitivityArea6);
    DDV_MinMaxLong(pDX, m_sensitivityArea6, 0, 3999);
    DDX_Text(pDX, IDC_EDIT7, m_sensitivityAll);
    DDV_MinMaxLong(pDX, m_sensitivityAll, 0, 3999);
    DDX_Control(pDX, IDC_COMBO1, m_totalSenseCombo);
    DDX_Control(pDX, IDC_COMBO2, m_frequencyCombo);
    DDX_Control(pDX, IDC_COMBO3, m_alarmRingCombo);
    DDX_Control(pDX, IDC_COMBO4, m_alarmVoiceCombo);
    DDX_Control(pDX, IDC_COMBO5, m_alarmTimeCombo);
    DDX_Radio(pDX, IDC_RADIO1, m_radioAutoFrequencyVal);
    DDX_Radio(pDX, IDC_RADIO3, m_radioLanguageVal);
    DDX_Radio(pDX, IDC_RADIO5, m_radioRingModeVal);
    DDX_Radio(pDX, IDC_RADIO7, m_radioAlarmModeVal);
    DDX_Radio(pDX, IDC_RADIO9, m_radioAreaTypeVal);
    DDX_Radio(pDX, IDC_RADIO12, m_radioCheckModeVal);
    DDX_Radio(pDX, IDC_RADIO15, m_radioAlarmAreaVal);
    DDX_Radio(pDX, IDC_RADIO18, m_radioInfrateModeVal);
    DDX_Radio(pDX, IDC_RADIO21, m_radioFilterModeVal);
}

BOOL CSystemSetDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();
    InitStyle();
    m_totalSenseCombo.SetCurSel(0);
    m_frequencyCombo.SetCurSel(1);
    m_alarmRingCombo.SetCurSel(0);
    m_alarmVoiceCombo.SetCurSel(8);
    m_alarmTimeCombo.SetCurSel(1);
    //CheckDlgButton(IDC_RADIO2, TRUE);
    //CheckDlgButton(IDC_RADIO3, TRUE);
    //CheckDlgButton(IDC_RADIO5, TRUE);
    //CheckDlgButton(IDC_RADIO7, TRUE);
    //CheckDlgButton(IDC_RADIO11, TRUE);
    //CheckDlgButton(IDC_RADIO12, TRUE);
    //CheckDlgButton(IDC_RADIO15, TRUE);
    //CheckDlgButton(IDC_RADIO18, TRUE);
    //CheckDlgButton(IDC_RADIO24, TRUE);
    ((CComboBox*)GetDlgItem(IDC_COMBO1))->SetWindowText("8");
    //GetDlgItem(IDC_COMBO2)->SetWindowText("8025");
    theApp.m_SDCommData.SetCurCtrlWnd(m_hWnd);
    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BEGIN_MESSAGE_MAP(CSystemSetDlg, CBaseDialog)
	ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_BTN_ALLDEV, &CSystemSetDlg::OnBnClickedBtnAlldev)
    ON_BN_CLICKED(IDC_BTN_SETDEV, &CSystemSetDlg::OnBnClickedBtnSetdev)
    ON_BN_CLICKED(IDC_BTN_GETPARA, &CSystemSetDlg::OnBnClickedBtnGetpara)
    ON_BN_CLICKED(IDC_RADIO1, &CSystemSetDlg::OnRadioAutoBnClicked)
    ON_BN_CLICKED(IDC_RADIO2, &CSystemSetDlg::OnRadioAutoBnClicked)
    ON_BN_CLICKED(IDC_RADIO3, &CSystemSetDlg::OnRadioLanguageBnClicked)
    ON_BN_CLICKED(IDC_RADIO4, &CSystemSetDlg::OnRadioLanguageBnClicked)
    ON_BN_CLICKED(IDC_RADIO5, &CSystemSetDlg::OnRadioRingModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO6, &CSystemSetDlg::OnRadioRingModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO7, &CSystemSetDlg::OnRadioAlarmModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO8, &CSystemSetDlg::OnRadioAlarmModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO9, &CSystemSetDlg::OnRadioAreaModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO10, &CSystemSetDlg::OnRadioAreaModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO11, &CSystemSetDlg::OnRadioAreaModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO12, &CSystemSetDlg::OnRadioCheckModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO13, &CSystemSetDlg::OnRadioCheckModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO14, &CSystemSetDlg::OnRadioCheckModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO15, &CSystemSetDlg::OnRadioAlarmAreaBnClicked)
    ON_BN_CLICKED(IDC_RADIO16, &CSystemSetDlg::OnRadioAlarmAreaBnClicked)
    ON_BN_CLICKED(IDC_RADIO17, &CSystemSetDlg::OnRadioAlarmAreaBnClicked)
    ON_BN_CLICKED(IDC_RADIO18, &CSystemSetDlg::OnRadioInfrateModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO19, &CSystemSetDlg::OnRadioInfrateModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO20, &CSystemSetDlg::OnRadioInfrateModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO21, &CSystemSetDlg::OnRadioFilterModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO22, &CSystemSetDlg::OnRadioFilterModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO23, &CSystemSetDlg::OnRadioFilterModeBnClicked)
    ON_BN_CLICKED(IDC_RADIO24, &CSystemSetDlg::OnRadioFilterModeBnClicked)
	ON_BN_CLICKED(IDC_SYSQUERY_QUERY, OnBnClickedBtnGetpara)
	ON_BN_CLICKED(IDC_SYSQUERY_QUERYALL, OnBnClickedBtnSetdev)
	ON_BN_CLICKED(IDC_SYSQUERY_DEL, OnBnClickedBtnAlldev)
    ON_MESSAGE(WM_PARAM_DATA_RECEIVED, OnUpdateParams)
END_MESSAGE_MAP()

// CSystemSetDlg ��Ϣ�������
LRESULT CSystemSetDlg::OnUpdateParams(WPARAM wParam, LPARAM lParam)
{
    LPBYTE data = (BYTE*)wParam;
    //���������ȵĻ�ȡ
    m_sensitivityArea1 = (data[3] << 7) + data[4];
    m_sensitivityArea2 = (data[5] << 7) + data[6];
    m_sensitivityArea3 = (data[7] << 7) + data[8];
    m_sensitivityArea4 = (data[9] << 7) + data[10];
    m_sensitivityArea5 = (data[11] << 7) + data[12];
    m_sensitivityArea6 = (data[13] << 7) + data[14];
    m_sensitivityArea7 = (data[15] << 7) + data[16];
    m_sensitivityArea8 = (data[17] << 7) + data[18];
    m_sensitivityAll = 0;
    int totalSense = data[19] >> 5;
    ((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(totalSense);
    //��������Ļ�ȡ
    int zoneMode = (data[19] >> 3) & 0x03;
    m_radioAreaTypeVal = zoneMode;

    // Ƶ�ʵĻ�ȡ
    int frequency = (data[20] << 7) + data[21];
    CString str;
    str.Format("%d", frequency);
    GetDlgItem(IDC_COMBO2)->SetWindowText(str);

    // ������������ģʽ�Ļ�ȡ
    int alarmRingMode = (data[22] >> 5) & 0x01;
    m_radioRingModeVal = alarmRingMode;

    // �����Զ���Ƶ�Ļ�ȡ
    int autoSetFrequency = (data[22] >> 4) & 0x01;
    m_radioAutoFrequencyVal = autoSetFrequency;

    // �����������õĻ�ȡ
    int ringtones = data[22] & 0x0F;
    str.Format("%d", ringtones);
    ((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(ringtones - 1);

    // �����������õĻ�ȡ
    int volume = data[23] & 0x0F;
    str.Format("%d", volume);
    ((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(volume);

    // �������õĻ�ȡ
    int switchChsOrEng = data[23] >> 4;
	m_radioLanguageVal = switchChsOrEng;
    
    int alarmDelay = (data[24] >> 3) & 0x0f;		//�����ӳ�/**5*/
    int alarmMode = (data[24] >> 2) & 0x01;		//����ģʽ
    int alarmArea = data[24] & 0x03;			//������λ

    //�����ӳ�
    ((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(alarmDelay - 1);

    
    //����ģʽ
	m_radioAlarmModeVal = alarmMode;  
    //������λ
	m_radioAlarmAreaVal = alarmArea;

    int filterMode = data[25] & 0x03;			// �˲�ģʽ�Ļ�ȡ
    int checkMode = (data[25] >> 2) & 0x03;		//̽��ģʽ
    int infraredMode = (data[25] >> 4) & 0x03;	//�������
	m_radioCheckModeVal = checkMode;
	m_radioInfrateModeVal = infraredMode;
	m_radioFilterModeVal = filterMode;
    
    UpdateData(FALSE);	//���ؼ��ı���ֵ���µ��ؼ���ʾ
    LOGMESSAGE("CSystemSetDlg::OnUpdateParams", "Update Params Success!");
    return 0L;
}

//LRESULT CSystemSetDlg::OnUpdateParams(WPARAM wParam, LPARAM lParam)
//{
//    LPBYTE data = (BYTE*)wParam;
//    //���������ȵĻ�ȡ
//    m_sensitivityArea1 = (data[3] << 7) + data[4];
//    m_sensitivityArea2 = (data[5] << 7) + data[6];
//    m_sensitivityArea3 = (data[7] << 7) + data[8];
//    m_sensitivityArea4 = (data[9] << 7) + data[10];
//    m_sensitivityArea5 = (data[11] << 7) + data[12];
//    m_sensitivityArea6 = (data[13] << 7) + data[14];
//    m_sensitivityArea7 = (data[15] << 7) + data[16];
//    m_sensitivityArea8 = (data[17] << 7) + data[18];
//    m_sensitivityAll = 0;
//	UpdateData(FALSE);	//��ֵ���µ���Ӧ�Ŀؼ���ȥ
//    //����������--�����Ļ�ȡ
//    int totalSense = data[19] >> 5;
//	((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(totalSense);
//    //if(SensMenu7 == 0)
//    //{
//    //    m_totalSenseCombo.SetCurSel(1);
//    //}
//    //else
//    //{
//    //    m_totalSenseCombo.SetCurSel(0);
//    //}
//
//    //��������Ļ�ȡ
//    int zoneMode = (data[19] >> 3) & 0x03;
//
//    if(zoneMode == 0)
//    {
//        CheckDlgButton(IDC_RADIO9, TRUE);
//        CheckDlgButton(IDC_RADIO10, FALSE);
//        CheckDlgButton(IDC_RADIO11, FALSE);
//    }
//    else if(zoneMode == 1)
//    {
//        CheckDlgButton(IDC_RADIO9, FALSE);
//        CheckDlgButton(IDC_RADIO10, TRUE);
//        CheckDlgButton(IDC_RADIO11, FALSE);
//    }
//    else if(zoneMode == 2)
//    {
//        CheckDlgButton(IDC_RADIO9, FALSE);
//        CheckDlgButton(IDC_RADIO10, FALSE);
//        CheckDlgButton(IDC_RADIO11, TRUE);
//    }
//
//    // Ƶ�ʵĻ�ȡ
//    int frequency = (data[20] << 7) + data[21];
//    CString str;
//    str.Format("%d", frequency);
//    GetDlgItem(IDC_COMBO2)->SetWindowText(str);
//    // ���������Ļ�ȡ
//    // ������������ģʽ�Ļ�ȡ
//    int alarmRingMode = (data[22] >> 5) & 0x01;
//	//m_radioRingModeVal = alarmRingMode;
//    if(alarmRingMode == 0)
//    {
//        CheckDlgButton(IDC_RADIO5, TRUE);
//        CheckDlgButton(IDC_RADIO6, FALSE);
//    }
//    else
//    {
//        CheckDlgButton(IDC_RADIO5, FALSE);
//        CheckDlgButton(IDC_RADIO6, TRUE);
//    }
//
//    // �����Զ���Ƶ�Ļ�ȡ
//    int autoSetFrequency = (data[22] >> 4) & 0x01;
//	//m_radioAutoFrequencyVal = autoSetFrequency;
//    if(autoSetFrequency == 0)
//    {
//        CheckDlgButton(IDC_RADIO1, FALSE);
//        CheckDlgButton(IDC_RADIO2, TRUE);
//    }
//    else
//    {
//        CheckDlgButton(IDC_RADIO1, TRUE);
//        CheckDlgButton(IDC_RADIO2, FALSE);
//    }
//
//    // �����������õĻ�ȡ
//    int ringtones = data[22] & 0x0F;
//    str.Format("%d", ringtones);
//	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(ringtones-1);
//    //m_alarmRingCombo.SetWindowText(str);
//    // �����������õĻ�ȡ
//    int volume = data[23] & 0x0F;
//    str.Format("%d", volume);
//	((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(volume);
//    //m_alarmVoiceCombo.SetWindowText(str);
//    // �������õĻ�ȡ
//    int switchChsOrEng = data[23] >> 4;
//
//    if(switchChsOrEng == 0)
//    {
//        CheckDlgButton(IDC_RADIO3, TRUE);
//        CheckDlgButton(IDC_RADIO4, FALSE);
//    }
//    else
//    {
//        CheckDlgButton(IDC_RADIO3, FALSE);
//        CheckDlgButton(IDC_RADIO4, TRUE);
//    }
//
//    int alarmDelay = (data[24] >> 3) & 0x0f;		//�����ӳ�/**5*/
//    int alarmMode = (data[24] >> 2) & 0x01;		//����ģʽ
//    int alarmArea = data[24] & 0x03;			//������λ
//	//float alarmDelay = (data[24] >> 3) / 2.0;
//    //str.Format("%0.1fs", alarmDelay);
//    //�����ӳ�
//	((CComboBox*)GetDlgItem(IDC_COMBO5))->SetCurSel(alarmDelay-1);
//    //m_alarmTimeCombo.SetWindowText(str);
//
//    //����ģʽ
//    if(alarmMode == 0)
//    {
//        CheckDlgButton(IDC_RADIO7, TRUE);
//        CheckDlgButton(IDC_RADIO8, FALSE);
//    }
//    else if(alarmMode == 1)
//    {
//        CheckDlgButton(IDC_RADIO7, FALSE);
//        CheckDlgButton(IDC_RADIO8, TRUE);
//    }
//
//    //������λ
//    if(alarmArea == 0)
//    {
//        CheckDlgButton(IDC_RADIO15, TRUE);
//        CheckDlgButton(IDC_RADIO16, FALSE);
//        CheckDlgButton(IDC_RADIO17, FALSE);
//    }
//    else if(alarmArea == 1)
//    {
//        CheckDlgButton(IDC_RADIO15, FALSE);
//        CheckDlgButton(IDC_RADIO16, TRUE);
//        CheckDlgButton(IDC_RADIO17, FALSE);
//    }
//    else if(alarmArea == 2)
//    {
//        CheckDlgButton(IDC_RADIO15, FALSE);
//        CheckDlgButton(IDC_RADIO16, FALSE);
//        CheckDlgButton(IDC_RADIO17, TRUE);
//    }
//
//    int filterMode = data[25] & 0x03;			// �˲�ģʽ�Ļ�ȡ
//    int checkMode = (data[25] >> 2) & 0x03;		//̽��ģʽ
//    int infraredMode = (data[25] >> 4) & 0x03;	//�������
//
//    if(checkMode == 0)
//    {
//        CheckDlgButton(IDC_RADIO12, TRUE);
//        CheckDlgButton(IDC_RADIO13, FALSE);
//        CheckDlgButton(IDC_RADIO14, FALSE);
//    }
//    else if(checkMode == 1)
//    {
//        CheckDlgButton(IDC_RADIO12, FALSE);
//        CheckDlgButton(IDC_RADIO13, TRUE);
//        CheckDlgButton(IDC_RADIO14, FALSE);
//    }
//    else if(checkMode == 2)
//    {
//        CheckDlgButton(IDC_RADIO12, FALSE);
//        CheckDlgButton(IDC_RADIO13, FALSE);
//        CheckDlgButton(IDC_RADIO14, TRUE);
//    }
//
//    if(infraredMode == 0)
//    {
//        CheckDlgButton(IDC_RADIO18, TRUE);
//        CheckDlgButton(IDC_RADIO19, FALSE);
//        CheckDlgButton(IDC_RADIO20, FALSE);
//    }
//    else if(infraredMode == 1)
//    {
//        CheckDlgButton(IDC_RADIO18, FALSE);
//        CheckDlgButton(IDC_RADIO19, TRUE);
//        CheckDlgButton(IDC_RADIO20, FALSE);
//    }
//    else if(infraredMode == 2)
//    {
//        CheckDlgButton(IDC_RADIO18, FALSE);
//        CheckDlgButton(IDC_RADIO19, FALSE);
//        CheckDlgButton(IDC_RADIO20, TRUE);
//    }
//
//    if(filterMode == 0)
//    {
//        CheckDlgButton(IDC_RADIO21, FALSE);
//        CheckDlgButton(IDC_RADIO22, FALSE);
//        CheckDlgButton(IDC_RADIO23, TRUE);
//        CheckDlgButton(IDC_RADIO24, FALSE);
//    }
//    else if(filterMode == 1)
//    {
//        CheckDlgButton(IDC_RADIO21, TRUE);
//        CheckDlgButton(IDC_RADIO22, FALSE);
//        CheckDlgButton(IDC_RADIO23, FALSE);
//        CheckDlgButton(IDC_RADIO24, FALSE);
//    }
//    else if(filterMode == 2)
//    {
//        CheckDlgButton(IDC_RADIO21, FALSE);
//        CheckDlgButton(IDC_RADIO22, TRUE);
//        CheckDlgButton(IDC_RADIO23, FALSE);
//        CheckDlgButton(IDC_RADIO24, FALSE);
//    }
//    else if(filterMode == 3)
//    {
//        CheckDlgButton(IDC_RADIO21, FALSE);
//        CheckDlgButton(IDC_RADIO22, FALSE);
//        CheckDlgButton(IDC_RADIO23, FALSE);
//        CheckDlgButton(IDC_RADIO24, TRUE);
//    }
//
//    UpdateData(TRUE);
//    LOGMESSAGE("CSystemSetDlg::OnUpdateParams", "Update Params Success!");
//    return 0L;
//}

//BOOL CSystemSetDlg::OnInitDialog()
//{
//    CBaseDialog::OnInitDialog();
//
//    InitStyle();
//    theApp.m_SDCommData.SetCurCtrlWnd(m_hWnd);
//    //theApp.m_SDCommData.InitUI(this);
//    return TRUE;  // return TRUE unless you set the focus to a control
//
//}

void CSystemSetDlg::InitStyle(void)
{
    HINSTANCE hInstance = AfxGetInstanceHandle();
    LoadBackSkin(hInstance, TEXT("SYSTEMSET_DLG_BG"));
    SetWindowPos(NULL, 0, 0, 900, 700, SWP_NOMOVE);
    m_titleImage.LoadImage(hInstance, TEXT("SYSTEMSET_DLG_TITLE"));

	CRect rect;
	GetClientRect(rect);//��ô���Ĵ�С
	int bottom = rect.Height() / 2;
	int left = rect.Width() * 2 / 3;
	int interval = 80;
	m_btnQuery.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(left, bottom, 0, 0), this, IDC_SYSQUERY_QUERY);
	m_btnQuery.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
	m_btnQuery.SetWindowText("��ȡ����");
	m_btnQueryAll.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(left, bottom + interval, 0, 0), this, IDC_SYSQUERY_QUERYALL);
	m_btnQueryAll.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
	m_btnQueryAll.SetWindowText("�´����豸");
	m_btnDel.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(left, bottom + 2 * interval, 0, 0), this, IDC_SYSQUERY_DEL);
	m_btnDel.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
	m_btnDel.SetWindowText("�´��������豸");
}

void CSystemSetDlg::OnClientDraw(CDC*pDC, INT nWidth, INT nHeight)
{
    m_titleImage.DrawImage(pDC, 5, 4, 24, 24);
    pDC->SetTextColor(RGB(255, 255, 255));
    CRect rcText(35, 10, 250, 26);
    DrawText(pDC, rcText, m_strTitle, DT_LEFT | DT_END_ELLIPSIS);
}

void CSystemSetDlg::DrawText(CDC* pDC, CRect rcText, LPCTSTR lpszText, UINT uFormat, INT nHeight/*=18*/, bool bBold/*=true*/, LPCTSTR lpszName/*=TEXT("΢���ź�")*/)
{
    CFont font;
    font.CreateFont(nHeight, 0, 0, 0, bBold ? FW_BOLD : FW_NORMAL, 0, FALSE, 0, 0, 0, 0, 0, 0, lpszName);
    CFont*  pOldFont = pDC->SelectObject(&font);
    pDC->DrawText(lpszText, &rcText, uFormat);
    pDC->SelectObject(pOldFont);
}


void CSystemSetDlg::OnBnClickedBtnAlldev()
{
    GetDlgItem(IDC_BTN_ALLDEV)->EnableWindow(FALSE);
    vector<ClientInfo*> allCliect = theApp.m_SDCommData.GetAllClients();
    vector<ClientInfo*>::iterator iter = allCliect.begin();
    
    while(iter != allCliect.end())
    {
        ClientInfo *client = *iter;
        Setallparameter(client);
		iter++;
    }
    
    Sleep(1000);
    GetDlgItem(IDC_BTN_ALLDEV)->EnableWindow(TRUE);
}


void CSystemSetDlg::OnBnClickedBtnSetdev()
{
    UpdateData(TRUE);
    GetDlgItem(IDC_BTN_SETDEV)->EnableWindow(FALSE);
    Setallparameter(g_CurClientInfo);
    Sleep(1000);
    GetDlgItem(IDC_BTN_SETDEV)->EnableWindow(TRUE);
}

void CSystemSetDlg::Setallparameter(ClientInfo *client)
{
	if (client == NULL)
		return;

    UpdateData(TRUE);
    BYTE byte[27] = { 0 };
	byte[0] = client->doorID & 0xff;
    byte[1] = 0x1B;
    byte[2] = 0x03;
    
    if(m_sensitivityAll > 0 && (m_sensitivityAll < 3999))
    {
        m_sensitivityArea1 = m_sensitivityAll;
        m_sensitivityArea2 = m_sensitivityAll;
        m_sensitivityArea3 = m_sensitivityAll;
        m_sensitivityArea4 = m_sensitivityAll;
        m_sensitivityArea5 = m_sensitivityAll;
        m_sensitivityArea6 = m_sensitivityAll;
    }
    
    if(m_sensitivityArea1 < 0 || m_sensitivityArea1 > 3999)
    {
        return;
    }
    
    if(m_sensitivityArea2 < 0 || m_sensitivityArea2 > 3999)
    {
        return;
    }
    
    if(m_sensitivityArea3 < 0 || m_sensitivityArea3 > 3999)
    {
        return;
    }
    
    if(m_sensitivityArea4 < 0 || m_sensitivityArea4 > 3999)
    {
        return;
    }
    
    if(m_sensitivityArea5 < 0 || m_sensitivityArea5 > 3999)
    {
        return;
    }
    
    if(m_sensitivityArea6 < 0 || m_sensitivityArea6 > 3999)
    {
        return;
    }
    
    byte[3] = m_sensitivityArea1 / 128;
    byte[4] = m_sensitivityArea1 % 128;
    byte[5] = m_sensitivityArea2 / 128;
    byte[6] = m_sensitivityArea2 % 128;
    byte[7] = m_sensitivityArea3 / 128;
    byte[8] = m_sensitivityArea3 % 128;
    byte[9] = m_sensitivityArea4 / 128;
    byte[10] = m_sensitivityArea4 % 128;
    byte[11] = m_sensitivityArea5 / 128;
    byte[12] = m_sensitivityArea5 % 128;
    byte[13] = m_sensitivityArea6 / 128;
    byte[14] = m_sensitivityArea6 % 128;
    //m_sensitivityAll = 0;
    CString str;
    //GetDlgItem(IDC_COMBO1)->GetWindowText(str);
    //UINT totalSense = atoi(str);
    //
    //if(totalSense == 50)
    //{
    //    totalSense = 0;
    //}
	int totalSense = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	if (totalSense < 0)
		totalSense = 0;
    //������λ
    int zoneMode = m_radioAreaTypeVal;
    
    byte[19] = (totalSense << 5) | (zoneMode << 3) | 6;
    //��ȡƵ��
    GetDlgItem(IDC_COMBO2)->GetWindowText(str);
    int frequency = atoi(str);
    
    if(frequency > 8999 || frequency < 7000)
    {
        MessageBox("Ƶ�����ó�����Χ�����������ã�", "��ʾ", MB_OK);
        return;
    }
    
    byte[20] = frequency / 128;
    byte[21] = frequency % 128;
    //�����Զ�����Ƶ��
    int autoset = m_radioAutoFrequencyVal;
    
    int ringtonesMode = m_radioRingModeVal;
    
    //��ȡ����
    //GetDlgItem(IDC_COMBO3)->GetWindowText(str);
    int ringtones = ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel()+1;
    byte[22] = (ringtonesMode << 5) | (autoset << 4) | ringtones;
    //��ȡ����
    //GetDlgItem(IDC_COMBO4)->GetWindowText(str);
    //int volume = atoi(str);
	int volume = ((CComboBox*)GetDlgItem(IDC_COMBO4))->GetCurSel();
    //������Ӣ��
    int switchChsOrEng = m_radioLanguageVal;
    
    byte[23] = switchChsOrEng << 4 | volume;
    //����ģʽ
    int alarmMode = m_radioAlarmModeVal;
    
    //������λ
    int areaMode = m_radioAlarmAreaVal;
    
    //����ʱ��
    //GetDlgItem(IDC_COMBO5)->GetWindowText(str);
    //int alarmDelay = atof(str) * 10;
	int alarmDelay = ((CComboBox*)GetDlgItem(IDC_COMBO5))->GetCurSel()+1;
	byte[24] = (alarmDelay << 3) | (alarmMode << 2) | areaMode;
    /*byte[24] = ((alarmDelay / 5) << 3) | (alarmMode << 2) | areaMode;*/
    //̽��ģʽ
    int checkMode = m_radioCheckModeVal;
    
    //�������
    int infraredMode = m_radioInfrateModeVal;
    
    //�����˲�
    int filterMode = m_radioFilterModeVal;
    
    byte[25] = (infraredMode << 4) + (checkMode << 2) + filterMode;
    byte[26] = 0x7F;
    WSABUF wsaBuf;
    wsaBuf.len = 27;
    wsaBuf.buf = (char*)byte;
    
    try
    {
        g_server->SendData(client->ipAddress, client->nPort, wsaBuf, 27);
    }
    catch(...)
    {
    }

}

/*
void CSystemSetDlg::Setallparameter(ClientInfo *client)
{
UpdateData(TRUE);
BYTE byte[27] = { 0 };
byte[0] = client->doorID & 0xff;
byte[1] = 0x1B;
byte[2] = 0x03;
CString strAllset;
GetDlgItem(IDC_EDIT7)->GetWindowText(strAllset);
int allSet = atoi(strAllset);

if(m_sensitivityAll > 0 && (m_sensitivityAll < 3999))
{
m_sensitivityArea1 = m_sensitivityAll;
m_sensitivityArea2 = m_sensitivityAll;
m_sensitivityArea3 = m_sensitivityAll;
m_sensitivityArea4 = m_sensitivityAll;
m_sensitivityArea5 = m_sensitivityAll;
m_sensitivityArea6 = m_sensitivityAll;
}

if(m_sensitivityArea1 < 0 || m_sensitivityArea1 > 3999)
{
return;
}

if(m_sensitivityArea2 < 0 || m_sensitivityArea2 > 3999)
{
return;
}

if(m_sensitivityArea3 < 0 || m_sensitivityArea3 > 3999)
{
return;
}

if(m_sensitivityArea4 < 0 || m_sensitivityArea4 > 3999)
{
return;
}

if(m_sensitivityArea5 < 0 || m_sensitivityArea5 > 3999)
{
return;
}

if(m_sensitivityArea6 < 0 || m_sensitivityArea6 > 3999)
{
return;
}

byte[3] = m_sensitivityArea1 / 128;
byte[4] = m_sensitivityArea1 % 128;
byte[5] = m_sensitivityArea2 / 128;
byte[6] = m_sensitivityArea2 % 128;
byte[7] = m_sensitivityArea3 / 128;
byte[8] = m_sensitivityArea3 % 128;
byte[9] = m_sensitivityArea4 / 128;
byte[10] = m_sensitivityArea4 % 128;
byte[11] = m_sensitivityArea5 / 128;
byte[12] = m_sensitivityArea5 % 128;
byte[13] = m_sensitivityArea6 / 128;
byte[14] = m_sensitivityArea6 % 128;
m_sensitivityAll = 0;
CString str;
GetDlgItem(IDC_COMBO1)->GetWindowText(str);
UINT totalSense = atoi(str);

if(totalSense == 50)
{
totalSense = 0;
}

int zoneMode;

//������λ
if(IsDlgButtonChecked(IDC_RADIO9))
{
zoneMode = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO10))
{
zoneMode = 1;
}
else if(IsDlgButtonChecked(IDC_RADIO11))
{
zoneMode = 2;
}

byte[19] = (totalSense << 5) | (zoneMode << 3) | 6;
//��ȡƵ��
GetDlgItem(IDC_COMBO2)->GetWindowText(str);
int frequency = atoi(str);

if(frequency > 8999 || frequency < 7000)
{
MessageBox("Ƶ�����ó�����Χ�����������ã�", "��ʾ", MB_OK);
return;
}

byte[20] = frequency / 128;
byte[21] = frequency % 128;
int autoset;

//�����Զ�����Ƶ��
if(IsDlgButtonChecked(IDC_RADIO1))
{
autoset = 1;
}
else if(IsDlgButtonChecked(IDC_RADIO2))
{
autoset = 0;
}

int ringtonesMode;

if(IsDlgButtonChecked(IDC_RADIO5))
{
ringtonesMode = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO6))
{
ringtonesMode = 1;
}

//��ȡ����
GetDlgItem(IDC_COMBO3)->GetWindowText(str);
int ringtones = atoi(str);
byte[22] = (ringtonesMode << 5) | (autoset << 4) | ringtones;
//��ȡ����
GetDlgItem(IDC_COMBO4)->GetWindowText(str);
int volume = atoi(str);
//������Ӣ��
int switchChsOrEng;

if(IsDlgButtonChecked(IDC_RADIO3))
{
switchChsOrEng = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO4))
{
switchChsOrEng = 1;
}

byte[23] = switchChsOrEng << 4 | volume;
//����ģʽ
int alarmMode;

if(IsDlgButtonChecked(IDC_RADIO7))
{
alarmMode = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO8))
{
alarmMode = 1;
}

//������λ
int areaMode;

if(IsDlgButtonChecked(IDC_RADIO15))
{
areaMode = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO16))
{
areaMode = 1;
}
else if(IsDlgButtonChecked(IDC_RADIO17))
{
areaMode = 2;
}

//����ʱ��
GetDlgItem(IDC_COMBO5)->GetWindowText(str);
int alarmDelay = atof(str) * 10;
byte[24] = ((alarmDelay / 5) << 3) | (alarmMode << 2) | areaMode;
//̽��ģʽ
int checkMode;

if(IsDlgButtonChecked(IDC_RADIO12))
{
checkMode = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO13))
{
checkMode = 1;
}
else if(IsDlgButtonChecked(IDC_RADIO14))
{
checkMode = 2;
}

//�������
int infraredMode;

if(IsDlgButtonChecked(IDC_RADIO18))
{
infraredMode = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO19))
{
infraredMode = 1;
}
else if(IsDlgButtonChecked(IDC_RADIO20))
{
infraredMode = 2;
}

//�����˲�
int filterMode;

if(IsDlgButtonChecked(IDC_RADIO23))
{
filterMode = 0;
}
else if(IsDlgButtonChecked(IDC_RADIO21))
{
filterMode = 1;
}
else if(IsDlgButtonChecked(IDC_RADIO22))
{
filterMode = 2;
}
else if(IsDlgButtonChecked(IDC_RADIO24))
{
filterMode = 3;
}

byte[25] = (infraredMode << 4) + (checkMode << 2) + filterMode;
byte[26] = 0x7F;
WSABUF wsaBuf;
wsaBuf.len = 27;
wsaBuf.buf = (char*)byte;

try
{
m_pDlg->m_IOCPServer.SendData(client->ipAddress, client->nPort, wsaBuf, 27);
}
catch(...)
{
}

}
*/

//void CSystemSetDlg::Setallparameter(int nGetSel, BOOL isSendAll)
//{
//    if(g_CurClientInfo == NULL)
//    {
//        return;
//    }
//    
//    UpdateData(TRUE);
//    BYTE byte[27] = { 0 };
//    byte[0] = nGetSel | 0x80;  //  �����ã���д��
//    byte[1] = 0x1B;
//    byte[2] = 0x03;
//    CString strAllset;
//    GetDlgItem(IDC_EDIT7)->GetWindowText(strAllset);
//    int allSet = atoi(strAllset);
//    
//    if(m_sensitivityAll > 0 && (m_sensitivityAll < 3999))
//    {
//        m_sensitivityArea1 = m_sensitivityAll;
//        m_sensitivityArea2 = m_sensitivityAll;
//        m_sensitivityArea3 = m_sensitivityAll;
//        m_sensitivityArea4 = m_sensitivityAll;
//        m_sensitivityArea5 = m_sensitivityAll;
//        m_sensitivityArea6 = m_sensitivityAll;
//    }
//    
//    if(m_sensitivityArea1 < 0 || m_sensitivityArea1 > 3999)
//    {
//        return;
//    }
//    
//    if(m_sensitivityArea2 < 0 || m_sensitivityArea2 > 3999)
//    {
//        return;
//    }
//    
//    if(m_sensitivityArea3 < 0 || m_sensitivityArea3 > 3999)
//    {
//        return;
//    }
//    
//    if(m_sensitivityArea4 < 0 || m_sensitivityArea4 > 3999)
//    {
//        return;
//    }
//    
//    if(m_sensitivityArea5 < 0 || m_sensitivityArea5 > 3999)
//    {
//        return;
//    }
//    
//    if(m_sensitivityArea6 < 0 || m_sensitivityArea6 > 3999)
//    {
//        return;
//    }
//    
//    byte[3] = m_sensitivityArea1 / 128;
//    byte[4] = m_sensitivityArea1 % 128;
//    byte[5] = m_sensitivityArea2 / 128;
//    byte[6] = m_sensitivityArea2 % 128;
//    byte[7] = m_sensitivityArea3 / 128;
//    byte[8] = m_sensitivityArea3 % 128;
//    byte[9] = m_sensitivityArea4 / 128;
//    byte[10] = m_sensitivityArea4 % 128;
//    byte[11] = m_sensitivityArea5 / 128;
//    byte[12] = m_sensitivityArea5 % 128;
//    byte[13] = m_sensitivityArea6 / 128;
//    byte[14] = m_sensitivityArea6 % 128;
//    m_sensitivityAll = 0;
//    CString str;
//    GetDlgItem(IDC_COMBO1)->GetWindowText(str);
//    UINT totalSense = atoi(str);
//    
//    if(totalSense == 50)
//    {
//        totalSense = 0;
//    }
//    
//    int zoneMode;
//    
//    //������λ
//    if(IsDlgButtonChecked(IDC_RADIO9))
//    {
//        zoneMode = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO10))
//    {
//        zoneMode = 1;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO11))
//    {
//        zoneMode = 2;
//    }
//    
//    byte[19] = (totalSense << 5) | (zoneMode << 3) | 6;
//    //��ȡƵ��
//    GetDlgItem(IDC_COMBO2)->GetWindowText(str);
//    int frequency = atoi(str);
//    
//    if(frequency > 8999 || frequency < 7000)
//    {
//        MessageBox("Ƶ�����ó�����Χ�����������ã�", "��ʾ", MB_OK);
//        return;
//    }
//    
//    byte[20] = frequency / 128;
//    byte[21] = frequency % 128;
//    int autoset;
//    
//    //�����Զ�����Ƶ��
//    if(IsDlgButtonChecked(IDC_RADIO1))
//    {
//        autoset = 1;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO2))
//    {
//        autoset = 0;
//    }
//    
//    int ringtonesMode;
//    
//    if(IsDlgButtonChecked(IDC_RADIO5))
//    {
//        ringtonesMode = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO6))
//    {
//        ringtonesMode = 1;
//    }
//    
//    //��ȡ����
//    GetDlgItem(IDC_COMBO3)->GetWindowText(str);
//    int ringtones = atoi(str);
//    byte[22] = (ringtonesMode << 5) | (autoset << 4) | ringtones;
//    //��ȡ����
//    GetDlgItem(IDC_COMBO4)->GetWindowText(str);
//    int volume = atoi(str);
//    //������Ӣ��
//    int switchChsOrEng;
//    
//    if(IsDlgButtonChecked(IDC_RADIO3))
//    {
//        switchChsOrEng = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO4))
//    {
//        switchChsOrEng = 1;
//    }
//    
//    byte[23] = switchChsOrEng << 4 | volume;
//    //����ģʽ
//    int alarmMode;
//    
//    if(IsDlgButtonChecked(IDC_RADIO7))
//    {
//        alarmMode = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO8))
//    {
//        alarmMode = 1;
//    }
//    
//    //������λ
//    int areaMode;
//    
//    if(IsDlgButtonChecked(IDC_RADIO15))
//    {
//        areaMode = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO16))
//    {
//        areaMode = 1;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO17))
//    {
//        areaMode = 2;
//    }
//    
//    //����ʱ��
//    GetDlgItem(IDC_COMBO5)->GetWindowText(str);
//    int alarmDelay = atof(str) * 10;
//    byte[24] = ((alarmDelay / 5) << 3) | (alarmMode << 2) | areaMode;
//    //̽��ģʽ
//    int checkMode;
//    
//    if(IsDlgButtonChecked(IDC_RADIO12))
//    {
//        checkMode = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO13))
//    {
//        checkMode = 1;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO14))
//    {
//        checkMode = 2;
//    }
//    
//    //�������
//    int infraredMode;
//    
//    if(IsDlgButtonChecked(IDC_RADIO18))
//    {
//        infraredMode = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO19))
//    {
//        infraredMode = 1;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO20))
//    {
//        infraredMode = 2;
//    }
//    
//    //�����˲�
//    int filterMode;
//    
//    if(IsDlgButtonChecked(IDC_RADIO23))
//    {
//        filterMode = 0;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO21))
//    {
//        filterMode = 1;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO22))
//    {
//        filterMode = 2;
//    }
//    else if(IsDlgButtonChecked(IDC_RADIO24))
//    {
//        filterMode = 3;
//    }
//    
//    byte[25] = (infraredMode << 4) + (checkMode << 2) + filterMode;
//    byte[26] = 0x7F;
//    WSABUF wsaBuf;
//    wsaBuf.len = 27;
//    wsaBuf.buf = (char*)byte;
//    
//    try
//    {
//        g_server->SendData(g_CurClientInfo->ipAddress, g_CurClientInfo->nPort, wsaBuf, 27);
//    }
//    catch(...)
//    {
//    }
//    
//    UpdateData(FALSE);
//}

void CSystemSetDlg::OnBnClickedBtnGetpara()
{
    if(g_CurClientInfo == NULL)
    {
        return;
    }
    
    BYTE byte[4];
    byte[0] = g_CurClientInfo->doorID & 0xff;
    byte[1] = 0x04;
    byte[2] = 0x02;
    byte[3] = 0x7F;
    //m_pDlg->m_IOCPServer.SendData();
    WSABUF wsaBuf;
    wsaBuf.len = 4;
    wsaBuf.buf = (char*)byte;
    
    try
    {
        g_server->SendData(g_CurClientInfo->ipAddress, g_CurClientInfo->nPort, wsaBuf, 4);
    }
    catch(...)
    {
    }
    
    Sleep(100);
}

//��ѡ��ť�ؼ��ĵ����Ӧ�¼�����
void CSystemSetDlg::OnRadioAutoBnClicked()
{
    UpdateData(TRUE);
}

void CSystemSetDlg::OnRadioLanguageBnClicked()
{
    UpdateData(TRUE);
}
void CSystemSetDlg::OnRadioRingModeBnClicked()
{
    UpdateData(TRUE);
}
void CSystemSetDlg::OnRadioAlarmModeBnClicked()
{
    UpdateData(TRUE);
}
void CSystemSetDlg::OnRadioAreaModeBnClicked()
{
    UpdateData(TRUE);
}
void CSystemSetDlg::OnRadioCheckModeBnClicked()
{
    UpdateData(TRUE);
}
void CSystemSetDlg::OnRadioAlarmAreaBnClicked()
{
    UpdateData(TRUE);
}
void CSystemSetDlg::OnRadioInfrateModeBnClicked()
{
    UpdateData(TRUE);
}
void CSystemSetDlg::OnRadioFilterModeBnClicked()
{
    UpdateData(TRUE);
}

HBRUSH CSystemSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
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
