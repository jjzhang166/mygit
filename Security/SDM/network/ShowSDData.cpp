#include "StdAfx.h"
#include "ShowSDData.h"

CShowSDData* CShowSDData::_instance = 0;
CShowSDData::CShowSDData(void)
{
}


CShowSDData::~CShowSDData(void)
{
    HeapFree(GetProcessHeap(), 0, m_pShowQueue);
    DeleteCriticalSection(&m_csShowBuf);
}

CShowSDData::CShowSDData(CListCtrl* pL, CImageList* pI)
{
    m_pShowQueue = (CShowSDData::PShowSdcDataQueue)HeapAlloc(
                       GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CShowSDData::ShowSdDataQueue));
    m_pListCtrl = pL;
    m_pImageList = pI;
    InitializeCriticalSection(&m_csShowBuf);
    //��ʼ���б���
    m_pListCtrl->InsertColumn(0, "IP��ַ:�˿�", LVCFMT_LEFT, 160); //������
    m_pListCtrl->InsertColumn(1, "����", LVCFMT_LEFT, 170);
    m_pListCtrl->InsertColumn(2, "����ʱ��(�Ͽ�ʱ��)", LVCFMT_LEFT, 140);
    m_pListCtrl->InsertColumn(3, "�ϴ��շ�����ʱ��", LVCFMT_LEFT, 140);
    m_pListCtrl->InsertColumn(4, "��ע", LVCFMT_LEFT, 230);
    m_pListCtrl->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES);
    m_pListCtrl->SetImageList(m_pImageList, LVSIL_SMALL);
}

//����ע�ắ��
CShowSDData* CShowSDData::Instance(CListCtrl* pL, CImageList* pI)
{
    if(0 == _instance)
    {
        _instance = new CShowSDData(pL, pI);
        static Cleaner cl;//����ʹ˱�����Ϊ�˵����������Զ��ͷ�
    }
    
    return _instance;
}

//׼����ʾ����
void CShowSDData::ShowSDData(CString IP, CString Name, int TheState, CTime ConnTime, CTime ReTime, CString ReData)
{
    EnterCriticalSection(&m_csShowBuf);
    _tcsncpy(m_pShowQueue->DataQueue[m_pShowQueue->Head].IP, IP, 30);
    _tcsncpy(m_pShowQueue->DataQueue[m_pShowQueue->Head].Name, Name, BUFFER_LITTLE_SIZE);
    m_pShowQueue->DataQueue[m_pShowQueue->Head].TheState = TheState;
    m_pShowQueue->DataQueue[m_pShowQueue->Head].ConnTime = ConnTime;
    m_pShowQueue->DataQueue[m_pShowQueue->Head].ReTime = ReTime;
    _tcsncpy(m_pShowQueue->DataQueue[m_pShowQueue->Head].ReData, ReData, BUFFER_SMALL_SIZE);
    m_pShowQueue->Head = (m_pShowQueue->Head + 1) % BUFFER_BIG_SIZE;
    LeaveCriticalSection(&m_csShowBuf);
}

void CShowSDData::DisplaySDData()
{
    CString IP;
    CString Name;
    int TheState;
    CTime ConnTime;
    CTime ReTime;
    CString ReData;
    LVITEM lvi;
    CString strText;
    
    while(m_pShowQueue->Tail != m_pShowQueue->Head)
    {
        IP = m_pShowQueue->DataQueue[m_pShowQueue->Tail].IP;
        Name = m_pShowQueue->DataQueue[m_pShowQueue->Tail].Name;
        TheState = m_pShowQueue->DataQueue[m_pShowQueue->Tail].TheState;
        ConnTime = m_pShowQueue->DataQueue[m_pShowQueue->Tail].ConnTime;
        ReTime = m_pShowQueue->DataQueue[m_pShowQueue->Tail].ReTime;
        ReData = m_pShowQueue->DataQueue[m_pShowQueue->Tail].ReData;
        int nItem;
        LV_FINDINFO FindInfo;
        ZeroMemory(&FindInfo, sizeof(LV_FINDINFO));
        FindInfo.flags  = LVFI_STRING;
        FindInfo.psz = IP;
        nItem = m_pListCtrl->FindItem(&FindInfo, -1);
        
        if(nItem == -1) //�����
        {
            //Set subitem 0 ip
            ZeroMemory(&lvi, sizeof(LVITEM));
            lvi.mask =  LVIF_PARAM | LVIF_TEXT | LVIF_IMAGE;
            lvi.pszText  = const_cast<char*>(IP.GetBuffer(0));//���ü������IP+Port��ָ��Ϊ��item��Ψһ��ʶ
            lvi.iItem    = m_pListCtrl->GetItemCount();
            lvi.iSubItem = 0;
            lvi.iImage = TheState;
            m_pListCtrl->InsertItem(&lvi);
            lvi.mask =  LVIF_TEXT;
            //Set subitem 1 ����
            lvi.iSubItem = 1;
            lvi.pszText = const_cast<char*>(Name.GetBuffer(0));
            m_pListCtrl->SetItem(&lvi);
            // Set subitem 2 ����ʱ��
            lvi.iSubItem = 2;
            strText = ConnTime.Format("%Y-%m-%d %H:%M:%S ");
            lvi.pszText = const_cast<char*>(strText.GetBuffer(0));
            m_pListCtrl->SetItem(&lvi);
            // Set subitem 3 �շ�����ʱ��
            lvi.iSubItem = 3;
            
            if(ReTime == 0)
            {
                strText = "";
            }
            else
            {
                strText = ReTime.Format("%Y-%m-%d %H:%M:%S ");
            }
            
            lvi.pszText = const_cast<char*>(strText.GetBuffer(0));
            m_pListCtrl->SetItem(&lvi);
            // Set subitem 4 ��ע
            lvi.iSubItem = 4;
            lvi.pszText = const_cast<char*>(ReData.GetBuffer(0));
            m_pListCtrl->SetItem(&lvi);
        }
        else//�޸���
        {
            m_pListCtrl->SetItem(nItem, 0, LVIF_IMAGE, NULL, TheState, 0, 0, 0);
            m_pListCtrl->SetItem(nItem, 1, LVIF_TEXT, const_cast<char*>(Name.GetBuffer(0)), TheState, 0, 0, 0);
            strText = ConnTime.Format("%Y-%m-%d %H:%M:%S ");
            m_pListCtrl->SetItem(nItem, 2, LVIF_TEXT, const_cast<char*>(strText.GetBuffer(0)), TheState, 0, 0, 0);
            
            if(ReTime == 0)
            {
                strText = "";
            }
            else
            {
                strText = ReTime.Format("%Y-%m-%d %H:%M:%S ");
            }
            
            m_pListCtrl->SetItem(nItem, 3, LVIF_TEXT, const_cast<char*>(strText.GetBuffer(0)), TheState, 0, 0, 0);
            m_pListCtrl->SetItem(nItem, 4, LVIF_TEXT, const_cast<char*>(ReData.GetBuffer(0)), TheState, 0, 0, 0);
        }//if end
        
        m_pShowQueue->Tail = (m_pShowQueue->Tail + 1) % BUFFER_BIG_SIZE;
    }//while end
}
