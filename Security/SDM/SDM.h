#pragma once

#ifndef __AFXWIN_H__
    #error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������
#include "predefine.h"
#include "./database/DataBaseT.h"
#include "./network/SDCommData.h"

#define WM_DELETE_UPDATE (WM_USER + 0x100)

class CSDMApp : public CWinApp
{
    public:
        CSDMApp();
        
        virtual BOOL InitInstance();
        virtual int ExitInstance();
        
        void DeleteExpiredLog(const CString& strFilepath);
        void OnQueryValidPeriod(CString &strpath);	//��ѯ��Ч��
        void GetLastExitTime();
        void AutoRunApp();	//�����Զ�����
        static UINT DelExpiredImage(LPVOID lParam);	//ɾ������ͼƬ��Ϣ
        
        
        CSDCommData m_SDCommData;
        int m_nFullWidth;
        int m_nFullHeight;
        
        DECLARE_MESSAGE_MAP()
        
    private:
        HANDLE m_hMutex;//���������󣬷�ֹ������������
        GdiplusStartupInput m_gdiplusStartupInput;
        ULONG_PTR m_pGdiToken;
        
        BOOL OnlyInstance();                                //��֤һ̨����ֻ������һ��ʵ��
        BOOL ChangeDisMode();                               //�ı���Ļ�ֱ���
        void InitDBConnect(const CString &strReturnPath);   //��ʼ�����ݿ�����
        void FreeResource();
};

extern CSDMApp theApp;
