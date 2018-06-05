// IpSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SDM.h"
#include "IpSetDlg.h"
#include "afxdialogex.h"

#define IDC_IPSETDLG_OK  WM_USER+0x90
#define IDC_IPSETDLG_CANCEL WM_USER+0x91
// IpSetDlg �Ի���

IMPLEMENT_DYNAMIC(IpSetDlg, CBaseDialog)

IpSetDlg::IpSetDlg(CWnd* pParent /*=NULL*/)
    : CBaseDialog(IpSetDlg::IDD, pParent), m_ip(_T(""))
	, m_port(_T("")), m_tip(_T(""))
{
    m_strTitle = "IP Manual Set";
}

IpSetDlg::~IpSetDlg()
{
}

void IpSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipCtrl);
	DDX_Control(pDX, IDC_PORT_EDIT, m_portEdit);
	DDX_Text(pDX, IDC_PORT_EDIT, m_port);
}


BEGIN_MESSAGE_MAP(IpSetDlg, CBaseDialog)
    ON_WM_CTLCOLOR()
    ON_BN_CLICKED(IDC_IPSETDLG_OK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_IPSETDLG_CANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

BOOL IpSetDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();
    InitStyle();
    return TRUE;
}

void IpSetDlg::OnClientDraw(CDC*pDC, INT nWidth, INT nHeight)
{
    m_titleImage.DrawImage(pDC, 5, 4, 24, 24);
    pDC->SetTextColor(RGB(255, 255, 255));
    CRect rcText(35, 10, 250, 26);
    DrawText(pDC, rcText, m_strTitle, DT_LEFT | DT_END_ELLIPSIS);
}

void IpSetDlg::DrawText(CDC* pDC, CRect rcText, LPCTSTR lpszText, UINT uFormat, INT nHeight/*=18*/, bool bBold/*=true*/, LPCTSTR lpszName/*=TEXT("΢���ź�")*/)
{
    CFont font;
    font.CreateFont(nHeight, 0, 0, 0, bBold ? FW_BOLD : FW_NORMAL, 0, FALSE, 0, 0, 0, 0, 0, 0, lpszName);
    CFont*  pOldFont = pDC->SelectObject(&font);
    pDC->DrawText(lpszText, &rcText, uFormat);
    pDC->SelectObject(pOldFont);
}

void IpSetDlg::InitStyle(void)
{
    HINSTANCE hInstance = AfxGetInstanceHandle();
    LoadBackSkin(hInstance, TEXT("LOGIN_DLG_BG"));//LOGIN_DLG_BG
    SetWindowPos(NULL, 0, 0, 362, 228, SWP_NOMOVE);
    m_titleImage.LoadImage(hInstance, TEXT("LOGIN_DLG_TITLE"));//LOGIN_DLG_TITLE
    //CRect rect;
    //GetClientRect(rect);//��ô���Ĵ�С
    //int bottom = rect.Height() / 2;
    //int left = rect.Width() * 2 / 3;
    //int interval = 80;
    //m_btnQuery.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(left, bottom, 0, 0), this, IDC_SYSQUERY_QUERY);
    //m_btnQuery.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    //m_btnQuery.SetWindowText("��ȡ����");
    //m_btnQueryAll.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(left, bottom + interval, 0, 0), this, IDC_SYSQUERY_QUERYALL);
    //m_btnQueryAll.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    //m_btnQueryAll.SetWindowText("�´����豸");
    //m_btnDel.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(left, bottom + 2 * interval, 0, 0), this, IDC_SYSQUERY_DEL);
    //m_btnDel.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    //m_btnDel.SetWindowText("�´��������豸");
    m_btnLogin.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(55, 180, 0, 0), this, IDC_IPSETDLG_OK);
    m_btnLogin.SetButtonImage(hInstance, TEXT("BTN_REGISTER"));
    m_btnLogin.SetWindowText("ȷ��");
	
    m_btExit.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(200, 180, 0, 0), this, IDC_IPSETDLG_CANCEL);
    m_btExit.SetButtonImage(hInstance, TEXT("BTN_EXIT"));
    m_btExit.SetWindowText("ȡ��");

	SetDlgItemText(IDC_IPDESC_STATIC, m_tip);
}

void IpSetDlg::OnBnClickedOk()
{
    m_ipCtrl.GetWindowText(m_ip);
	m_portEdit.GetWindowText(m_port);

    if (IsIpAddressValid(m_ip))
    {
        OnOK();
    }
	else
	{
		//��ʾ������Ϸ���ip��ַ
		return;
	}
}

void IpSetDlg::OnBnClickedCancel()
{
    OnCancel();
}

void IpSetDlg::SetTipContent(CString tip)
{
	m_tip = tip;
	/*SetDlgItemText(IDC_IPDESC_STATIC, tip);*/
	//GetDlgItem(IDC_IPDESC_STATIC)->SetWindowText(tip);
}

HBRUSH IpSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
    
    return (CDialog::OnCtlColor(pDC, pWnd, nCtlColor));
}

bool IpSetDlg::IsOk(string str, int num)
{
    int len = str.length();
    int d = 0;
    
    if (str == "0")
    {
        return true;
    }
    
    if (len > 1 && str[0] == '0')
    {
        return false;
    } //��ǰ��0,���Ϸ�
    
    for (int i = 0; i < len; i++)
    {
        if (!isdigit(str[i]) && str[i] != ' ')
        {
            return false;
        }
        
        if (str[i] == ' ')
        {
            if (num == 2 || num == 3)
            {
                return false;
            }
            else
            {
                if (num == 1 && i != 0 && str[i] == ' ')
                {
                    return false;
                }
                
                if (num == 4 && i != len - 1 && str[i] == ' ')
                {
                    return false;
                }
            }
        }
        
        if (isdigit(str[i]))
        {
            d = d * 10 + str[i] - '0';
        }
    }
    
    if (d < 0 || d > 255)
    { return false; }
    
    return true;
}

bool IpSetDlg::IsIpAddressValid(const char* pszIPAddr)
{
    if (pszIPAddr == NULL)
    {
        return false;
    }
    
    string str = string(pszIPAddr);
    //cout<<str<<endl;
    int len = str.length();
    int coma_num;   //=numberofComa(str);  //������ţ������ŵĸ���������3����Ϊ�Ƿ�
    //��str������һ��С���㣬Ϊ��ͳ�Ʒ���
    str.append(".");
    //���濪ʼ�ָ��ַ������õ�Ž��зָ�õ�ÿ���ַ���������ַ��������Ϸ����򷵻�false�����Ϸ�����������ַ����м��пո��ַ�����ǰ��0���ַ��������ֲ���0-255֮�䣬�����ַ���Ϊ�գ�����Щ���ǲ��Ϸ�
    //string::iterator iter_pre=str.begin(),iter_post;
    int iter_pre = 0;
    int iter_post = str.find('.', iter_pre);
    
    //  cout<<iter_post<<endl;
    if (iter_post == str.npos)
    {
        return false;
    }
    
    coma_num = 0;
    
    while (iter_post != str.npos)
    {
        coma_num++;
        string substr = str.substr(iter_pre, iter_post - iter_pre);
        
        if (substr == "")
        {
            return false;
        }
        
        iter_pre = iter_post + 1;
        //if(iter_pre<=len-1)
        iter_post = str.find('.', iter_pre);
    }
    
    if (coma_num != 4)
    {
        return false;
    }
    
    return true;
}