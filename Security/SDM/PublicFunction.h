#pragma once
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <vector>
#include "SDM.h"
#include "./common/DataDefine.h"
#include "Log.h"

using namespace std;

extern CString g_strCurrentLanguage;
extern CString GetIniStringValue(CString strSection, CString strSectionKey, CString strFileName);
class CPublicFunction
{
    public:
        CPublicFunction(void);
        ~CPublicFunction(void);
        
    public:
        //�жϸ������ļ��Ƿ����
        static bool IsFileExist(const CString& strName);
        //����ͼƬ�ļ�ȫ·��
        static CString GetImagePath(const CString &path, const CString &fileName, const CString &date);
        //��ʾͼƬ
        static void ShowImage(IPicture *pPicture, RECT rect, CDC *dc, long nPicWidth, long nPicHeight, int rgb, int zoomStep, int zoomLeft, int zoomTop);
        //�����ļ�����ȡͼƬ��
        static IPicture* GetPictureFromName(const char *path);
        //�������ƻ�ȡBMPͼ����Ϣ
        static BMPImageInfo GetBmpImageInfoByName(const char *dibFileName);
        
        static double GetShowRadio(int clickCount);
        static int GetClickCount(double radio);
        
        //static BOOL   ShowMessageBox(const CString &msg,MsgBoxType msgType=eWarning,const CString &title=GetIniStringValue("MainDialog","PromptTitle",g_strCurrentLanguage));
        static void DrawRectangeSide(CDC *pDc, CRect rect, int nWidth, COLORREF crColor); //�����α߿�
        static CString GetCurrentDateTime();
        static CString GetImageNameTimeSpan(CString &curDT);
        static CString GetDate();
        static CString CreateCurrentDTPath();//��������Ŀ¼
        static void WriteRunLogFile(CString Source, CString Message); //��¼������־
        
        static void DrawGradient(HDC pDC, const RECT &rect,
                                 COLORREF beginColor = RGB(160, 160, 160), COLORREF endColor = RGB(236, 236, 236),
                                 bool fromUpToDown = false, bool drawWhole = true, int drawHeight = 0);
                                 
        static int SetRegValue(LPCSTR KeyValueName, DWORD &dwVal);
        static void GetRegValue();
        static void WriteExitTimeReg();
        static void LostRegInfoPrompt();
        
        static BOOL WriteCodeToFile(const CString& code, const CString &path);
        static CString ReadCodeFromFile(const CString& path);
        
        static BOOL ContainSpecialChar(const CString &str);
        
        //ָ�����̣����磺C:���Ƿ����㹻�Ŀռ�ɹ�ʹ�ã�crisisValueΪ�ٽ�ֵ����λΪMB
        static BOOL DiskHasEnoughSpace(const CString &szPath, DWORD &crisisValue);
        
        static void DeleteDir(CString str);
        static BOOL DelFolder(const CString &szPath);
        //�����ļ����ܴ�С����λ��MB��
        static double GetFolderSizeEx(const CString &szPath);
        
        static BOOL CreateDir(const CString& dir);
        
        static vector<CString> SplitString(const CString& stringToSplit, const char split);
        
        static bool ThePlateNoIsExsit(const CString& plateNo);
        
        static CheckInfo GetCheckInfo();
        
        static BOOL ModifyCheckResult(const CString& id, int checkResult, const CString& field);
        
        static BOOL UpdateDB(CheckInfo &checkInfo, const CString& plateNo, int vpId, const CString& saveName, const CString& plateNoImageName, const CString &captureImageName, const CString& exist);
        
        //��С����ʱ����
        static void AdjustCroodForZoomIn(int& destX, int& destY, int& displayW, int& displayH, int& clientW, int& clientH);
        //�ƶ�ͼ��ʱ����ͼ���Ƴ��߽�
        static void AdjustCroodForDrag(int& destX, int& destY, int& displayW, int& displayH, int& clientW, int& clientH, int moveW, int moveH);
        
        //��ʾץ��ͼ��ʱ����
        static void AdjustShowSnapRect(CRect rect, int nImageWidth, int nImageHeight, int& destX, int& destY, int& destW, int& destH);
        
        static CheckInfo GetTcpBackupMsgCheckInfo();
        
        static void ShowImage(CDC *dc, HDC hdc, CString &fileName, int ctrIID, RECT rect, int rgb);
    private:
        static void WriteLog();
};
