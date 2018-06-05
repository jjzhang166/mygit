// SDServer.cpp : ����Ӧ�ó��������Ϊ��
#include "stdafx.h"
#include "SDM.h"
#include "SDMDlg.h"
#include "SDM.h"
#include "Log.h"
#include "./system/LoginDlg.h"
#include "./system/RegisterDlg.h"
#include <string>
#include <sstream>
#include "./Des/DES.h"
#include "./Des/CRC32.h"
#include "./common/DataDefine.h"
#include "./common/CreateDump.h"
#include "./Des/TaskKeyMgr.h"
#include "./network/IOCPServer.h"

#define  MAX_PATH 260

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSDMApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


CSDMApp::CSDMApp()	// CSDMApp ����
{
}

int g_dayRemain;
int g_currentDoorNum = 0;// ��ǰ�����ŵ�����

bool g_bIsTimeExpired;		//�ж�����ǲ��ǹ���
bool g_bIsRegistered;		//�ж�����ǲ����Ѿ�ע��
bool g_autoStartApp;
bool g_autoLogin;

bool g_setIpPort = false;
int g_port = 12345;
CString g_ip = _T("127.0.0.1");

CString g_strCurrentLanguage;	//��ǰ����������
CString g_strIniFilePath;		//ini�ļ���·��
CString g_strImageRootPath;	//ͼ�񱣴��Ŀ¼
CString g_exePath;

CLog g_log;
CSDMApp theApp;	// Ψһ��һ�� CSDMApp ����

BYTE g_curDoorId;
BYTE g_address[200] = { 0 };
ClientInfo* g_CurClientInfo;
CDataBaseT*	g_pDBT = NULL;			//���ݿ����
CIOCPServer* g_server = NULL;

CString GetIniStringValue(CString strSection, CString strSectionKey, CString strFileName);

BOOL CSDMApp::InitInstance()
{
    // ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
    // ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
    //����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
    InitCommonControls();
    GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL);
    CWinApp::InitInstance();
    //skinppLoadSkin(_T("Longhorn.ssk"));
    AfxEnableControlContainer();
    // ��׼��ʼ��
    // ���δʹ����Щ���ܲ�ϣ����С
    // ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
    // ����Ҫ���ض���ʼ������
    // �������ڴ洢���õ�ע�����
    // TODO: Ӧ�ʵ��޸ĸ��ַ�����
    // �����޸�Ϊ��˾����֯��
    SetRegistryKey(_T("�����Ź���ƽ̨"));
    //system("rd /s/q D:\\SDServerLogFile");
    //��ʼ����־
    CTime cTime = CTime::GetCurrentTime();
    CString logFileName(_T(""));
    logFileName.Format("SDServer%d%.2d%.2d.log", cTime.GetYear(), cTime.GetMonth(), cTime.GetDay());
    g_log.Init(logFileName, 0);
    g_log.PrintTime(TRUE);
    g_log.PrintAppName(FALSE);
    #if(!_DEBUG)
    CTaskKeyMgr::Disable(CTaskKeyMgr::TASKKEYS, !CTaskKeyMgr::AreTaskKeysDisabled(), TRUE);
    #endif
    LOGMESSAGE("CSDMApp::InitInstance", "Start the application!");
    //��ȡ��ʼ������������
    CString strReturnPath = GetExePath();
    g_exePath = strReturnPath;
    g_strIniFilePath.Format("%s\\ini\\", strReturnPath);
    g_strCurrentLanguage = GetIniStringValue("Setting", "Language", "Setting");
    //��ȡͼ�񱣴��Ŀ¼
    g_strImageRootPath = GetIniStringValue("Image", "SavePath", "CCSystem");
    
    if("" == g_strImageRootPath)
    {
        //Ĭ��·��
        g_strImageRootPath = "D:\\SDServerImage\\";
    }
    
    CString strClosePC = GetIniStringValue("FunctionItem", "ClosePC", "InsideConfig");
    g_autoStartApp = (0 == strClosePC.CompareNoCase("0")) ? false : true;
    
    /*	//Ӣ��ϵͳ��������ʧ��
    if(g_autoStartApp)
    	AutoRunApp();
    */
    //��֤һ̨����ֻ������һ��ʵ��
    if(!OnlyInstance())
    {
        LOGMESSAGE("CSDMApp::InitInstance", "The application has started,you can't repeat start!");
        exit(0);
    }
    
    //����ϵͳ�ص���������ϵͳ����ʱ���ɲ���ϵͳ���ã�����Dump�ļ�
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashHandler);
    CString strAutoLogin = GetIniStringValue("FunctionItem", "AutoLogin", "InsideConfig");
    g_autoLogin = (0 == strAutoLogin.CompareNoCase("0")) ? false : true;
    LOGMESSAGE("CSDMApp::InitInstance", "Enter query valid period module!");
    //��ȡע����Ϣ
    //m_dayRemain = 100;
    CPublicFunction::GetRegValue();
    
    if(g_bIsRegistered)//�Ѿ�ע������֤��Ч��
    {
        //�ж��Ƿ��Ѿ�����
        g_bIsTimeExpired = true;
        //GetLastExitTime();
        CString strLicensePath = strReturnPath + "\\License.lic";
        OnQueryValidPeriod(_T(strLicensePath));
        
        if(g_bIsTimeExpired)
        {
            AfxMessageBox(GetIniStringValue("MainDialog", "SoftwareExpiredPrompt", g_strCurrentLanguage)); //�����Ч�ڽ����������»�ȡ��Ȩ��ע��
            LOGMESSAGE("CSDMApp::InitInstance", "Enter the register module!");
            CRegisterDlg dlg;
            
            //m_dayRemain = 100;
            if(IDCANCEL == dlg.DoModal())
            {
                LOGMESSAGE("CSDMApp::InitInstance", "Register failure!");
                exit(0);
            }
            
            LOGMESSAGE("CSDMApp::InitInstance", "Register success!");
        }
    }
    else//ע��
    {
        LOGMESSAGE("CSDMApp::InitInstance", "Enter the register module!");
        CRegisterDlg dlg;
        
        if(IDCANCEL == dlg.DoModal())
        {
            LOGMESSAGE("CSDMApp::InitInstance", "Register failure!");
            exit(0);
        }
        
        LOGMESSAGE("CSDMApp::InitInstance", "Register success!");
    }
    
    LOGMESSAGE("CSDMApp::InitInstance", "End of the query valid period module and the software is effective!");
    CString strlogpath = strReturnPath + "\\Logs";
    DeleteExpiredLog(strlogpath);
    
    //�ı���ʾ���ֱ���
    if(!ChangeDisMode())
    {
        return FALSE;
    }
    
    //��ʼ�����ݿ�����
    LOGMESSAGE("CSDMApp::InitInstance", "Initial connect the database!");
    InitDBConnect(strReturnPath);
    LOGMESSAGE("CSDMApp::InitInstance", "Connect the database OK");
    #if(!NEEDREGISTER)
    g_dayRemain = 3650;
    #endif
    
    if(!g_autoLogin)
    {
        LOGMESSAGE("CSDMApp::InitInstance", "Enter the login module!");
        CLoginDlg login;
        login.Init(g_dayRemain);
        
        if(IDOK != login.DoModal())
        {
            LOGMESSAGE("CSDMApp::InitInstance", "Login failure!");
            FreeResource();
            //CPublicFunction::WriteExitTimeReg();
            exit(1);
        }
        
        LOGMESSAGE("CSDMApp::InitInstance", "Login success!");
    }
    else
    {
        LOGMESSAGE("CSDMApp::InitInstance", "The ini file disabled the login module!");
    }
    
    LOGMESSAGE("CSDMApp::InitInstance", "Start DelExpiredImage thread");
    AfxBeginThread(DelExpiredImage, 0, THREAD_PRIORITY_ABOVE_NORMAL, 0, 0, NULL);
    LOGMESSAGE("CSDMApp::InitInstance", "Begin initial the main module controls!");
    CSDMDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    
    if(nResponse == IDOK)
    {
    }
    else if(nResponse == IDCANCEL)
    {
    }
    
    // ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
    // ����������Ӧ�ó������Ϣ�á�
    return FALSE;
}

void CSDMApp::AutoRunApp()
{
    HKEY hNewKey;
    UINT retVal;
    LPCSTR rgsKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    CString strexePath = GetExePath() + "\\SDServer.exe";
    retVal = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, rgsKey, 0, KEY_WRITE, &hNewKey);
    
    if(retVal)
    {
        return;
    }
    
    LPSTR exepathName = strexePath.GetBuffer();
    strexePath.ReleaseBuffer();
    retVal =::RegSetValueEx(hNewKey, "THSDServer", 0, REG_SZ, (const  unsigned  char *)exepathName, MAX_PATH);
    
    if(retVal)
    {
        RegCloseKey(hNewKey);
        return;
    }
}

CString GetIniStringValue(CString strSection, CString strSectionKey, CString strFileName)
{
    CString strFile;
    CString strValue;
    char m_Value[500] = "";
    memset(m_Value, 0, 500);
    strFile.Format("%s%s%s", g_strIniFilePath, strFileName, ".ini") ;
    GetPrivateProfileString(strSection, strSectionKey, NULL, m_Value, 500, strFile);
    strValue = m_Value;
    return strValue;
}

int CSDMApp::ExitInstance()
{
    // TODO: �ڴ����ר�ô����/����û���
    g_log.Uninit();
    FreeResource();
    GdiplusShutdown(m_pGdiToken);
    return CWinApp::ExitInstance();
}

void CSDMApp::FreeResource()
{
    try
    {
        if(NULL != g_pDBT)
        {
            delete g_pDBT;
            g_pDBT = NULL;
        }
        
        CPublicFunction::WriteExitTimeReg();
    }
    catch(...) {}
}

//Ψһappʵ��
BOOL CSDMApp::OnlyInstance()
{
    m_hMutex = CreateMutex(NULL, FALSE, "SDM.EXE");
    
    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
        //MessageBox(GetIniStringValue("MainDialog","PromptMsgOne",g_strCurrentLanguage));
        return FALSE;
    }
    
    return TRUE;
}

BOOL CSDMApp::ChangeDisMode()
{
    //�ı���ʾ���ֱ���֧��1280*1024��17��9��������1440*900��19��������1600*900(20����)
    //1680*1050��22�磩��1920*1080��21.5����������������⼸�������ķֱ���
    m_nFullWidth = GetSystemMetrics(SM_CXSCREEN);
    m_nFullHeight = GetSystemMetrics(SM_CYSCREEN);
    DEVMODE lpDevMode;
    lpDevMode.dmPelsWidth = m_nFullWidth;
    lpDevMode.dmPelsHeight = m_nFullHeight;
    
    if(lpDevMode.dmPelsWidth < 0x400)
    {
        lpDevMode.dmPelsWidth = 0x400;
    }
    
    if(lpDevMode.dmPelsHeight < 0x300)
    {
        lpDevMode.dmPelsHeight = 0x300;
    }
    
    lpDevMode.dmSize = sizeof(DEVMODE);
    lpDevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    
    if(ChangeDisplaySettings(&lpDevMode, NULL))
    {
        return FALSE;
    }
    
    return TRUE;
}

void CSDMApp::InitDBConnect(const CString &strReturnPath)
{
    //windows�Դ����ݿ�
    CString pCurrentPath;
    pCurrentPath.Format("%s\\", strReturnPath);
    CString m_strDBPath;
    CString strConnect = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=";
    strConnect.AppendFormat("%s%s", pCurrentPath, "SDM.mdb");
    strConnect.Append(";Jet OLEDB:Database Password=car2012");
    m_strDBPath.Format("%s%s", pCurrentPath, "SDServer.mdb");
    //SQL���ݿ�
    //CString strConnect = "Provider=SQLOLEDB.1;Password=launch*0112;Persist Security Info=False;User ID=sa;Initial Catalog=SDM;Data Source=GOS-01708081054";
    //CString m_strDBPath = "D:\\DataNew\\SDM.mdf";
    g_pDBT = new CDataBaseT;
    
    //g_pDBT->CompactDB(m_strDBPath);          //���ݿ�ѹ��
    if(!g_pDBT->InitDB(strConnect, m_strDBPath))
    {
        AfxMessageBox(GetIniStringValue("MainDialog", "ConnDatabaseFailPrompt", g_strCurrentLanguage)); //�������ݿ�ʧ��,�޷�ʹ�ó���,�������ݿ�����!
        exit(0);
    }
}

void CSDMApp::DeleteExpiredLog(const CString& strFilepath)
{
    CString inipath = GetExePath() + "\\ini\\CCSystem.ini";
    char days[10] = "";
    memset(days, 0, 10);
    GetPrivateProfileString("Image", "SaveLogDays", "1", days, 80, inipath);
    int nday = atoi(days);
    WIN32_FIND_DATA findData;
    HANDLE hError = INVALID_HANDLE_VALUE;
    char filePathName[1024] = "";
    char fullLogName[1024] = "";
    strcpy(filePathName, strFilepath);
    strcat(filePathName, "\\*.*");
    hError = FindFirstFile(filePathName, &findData);
    CTime   curTime;
    
    if(hError == INVALID_HANDLE_VALUE)
    {
        return;
    }
    
    while(::FindNextFile(hError, &findData))
    {
        if(strcmp(findData.cFileName, ".") == 0
                || strcmp(findData.cFileName, "..") == 0
                /*|| (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)*/)
        {
            continue;
        }
        
        wsprintf(fullLogName, "%s\\%s", strFilepath, findData.cFileName);
        
        if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            DeleteExpiredLog(fullLogName);
        }
        
        CString strFileName(fullLogName);
        CFileStatus rStatus;
        
        if(CFile::GetStatus(strFileName, rStatus))
        {
            CTime now = CTime::GetCurrentTime();
            CTimeSpan ts = now - rStatus.m_ctime;
            
            if(ts.GetDays() > nday)
            {
                DeleteFile(strFileName);
            }
        }
    }
    
    if(hError != INVALID_HANDLE_VALUE)
    {
        FindClose(hError);
        hError = INVALID_HANDLE_VALUE;
    }
}

UINT CSDMApp::DelExpiredImage(LPVOID lParam)
{
    CString days = GetIniStringValue("Image", "ImageSaveDays", "CCSystem");
    int iDays = (days.IsEmpty()) ? 30 : atoi(days);
    WIN32_FIND_DATA findData;
    HANDLE hError = INVALID_HANDLE_VALUE;
    char filePathName[1024] = "";
    char fullLogName[1024] = "";
    strcpy(filePathName, g_strImageRootPath);
    strcat(filePathName, "\\*.*");
    hError = FindFirstFile(filePathName, &findData);
    CTime   curTime;
    
    if(hError == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    
    while(::FindNextFile(hError, &findData))
    {
        if(strcmp(findData.cFileName, ".") == 0
                || strcmp(findData.cFileName, "..") == 0
                /*|| (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)*/)
        {
            continue;
        }
        
        wsprintf(fullLogName, "%s\\%s", g_strImageRootPath, findData.cFileName);
        
        if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            CTime now = CTime::GetCurrentTime();
            CTimeSpan ts = now - findData.ftCreationTime;
            
            if(ts.GetDays() > iDays)
            {
                CPublicFunction::DeleteDir(fullLogName);
            }
        }
    }
    
    if(hError != INVALID_HANDLE_VALUE)
    {
        FindClose(hError);
        hError = INVALID_HANDLE_VALUE;
    }
    
    return 1;
}

void CSDMApp::OnQueryValidPeriod(CString &strpath)
{
    LOGMESSAGE("CSDMApp::OnQueryValidPeriod", "Enter the check the license file module!");
    //��ȡ�ļ��еĻ�����
    CFile fFile;
    
    if(!CPublicFunction::IsFileExist(strpath))
    {
        LOGMESSAGE("CSDMApp::OnQueryValidPeriod", "The license file is missing!");
        CPublicFunction::LostRegInfoPrompt();
        return;
    }
    
    //�жϴ��ļ��Ƿ�ɹ�
    /*if(!fFile.Open(strpath,CFile::modeRead))
    {
    	CPublicFunction::LostRegInfoPrompt();
    }
    CString strLicense = "";
    fFile.Read(strLicense.GetBufferSetLength(fFile.GetLength()),fFile.GetLength());
    fFile.Close();
    strLicense.ReleaseBuffer();*/
    CString strLicense = CPublicFunction::ReadCodeFromFile(strpath);
    
    if(strLicense.IsEmpty())
    {
        LOGMESSAGE("CSDMApp::OnQueryValidPeriod", "The license file is invalid!");
        CPublicFunction::LostRegInfoPrompt();
        return;
    }
    
    LOGMESSAGE("CSDMApp::OnQueryValidPeriod", "The license file is valid and enter the check regcode module!");
    //CString strLicense = "9FEA569AC6DC9A554DB1C88366E288B1ADDF61C65AFBCFAD";
    unsigned long number = GetHardwareNumber();
    std::ostringstream stm;
    std::string strResult;
    stm << number;
    strResult = stm.str();
    TDES des;
    des.MakeKey(strResult);
    strLicense.Trim();
    int len = strLicense.GetLength();
    TMemoryStream stream;
    stream.SetSize(len);
    stream.SetPosition(0);
    stream.Write(strLicense.GetString(), len);
    TMemoryStream dest;
    
    if(des.DecryptNumber(stream, dest) == true)
    {
        std::string decry(dest.Memory());
        int last = decry.find_first_of(':');
        
        if(last < 4)
        {
            g_bIsTimeExpired = true;
            LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is invalid!");
            return;
        }
        
        int posHead = last + 1;
        
        if(last == std::string::npos)
        {
            last = 0;
        }
        else
        {
            last++;
        }
        
        decry.resize(last - 1);
        last = decry.find_last_not_of('\0');
        
        if(last == std::string::npos)
        {
            last = 0;
        }
        else
        {
            last++;
        }
        
        decry.resize(last);
        
        if(decry != strResult || posHead < 2)
        {
            g_bIsTimeExpired = true;
            LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is invalid!");
            return;
        }
        
        char* pData = (char*)dest.Memory();
        pData = pData + posHead;
        TModuleData* pModuleData = (TModuleData*)pData;
        TDateTime minDate = TDateTime::OriginDateTime();
        TDateTime nowDateTime = TDateTime::GetCurrentTime();
        TDateTime startTime = minDate + (int)pModuleData->StartTime;
        pModuleData->Day += 1;
        TDateTime endTime = startTime + (int)pModuleData->Day;
        int CheckDay = (int)pModuleData->Day;
        
        if(CheckDay > 3600)
        {
            g_bIsTimeExpired = false;
            g_dayRemain = 50;
            LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is Permanent invalid!");
            return;
        }
        
        GetLastExitTime();
        g_dayRemain = static_cast<int>(endTime) - static_cast<int>(nowDateTime);
        int year = endTime.GetYear() - minDate.GetYear();
        
        if(year > 30)
        {
            g_bIsTimeExpired = true;
            LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is invalid!");
            return;
        }
        
        if(nowDateTime > endTime)
        {
            g_bIsTimeExpired = true;
            LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is invalid!");
            return;
        }
        
        if(nowDateTime < startTime)
        {
            g_bIsTimeExpired = true;
            LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is invalid!");
            return;
        }
        
        if(CheckDay <= 0 || g_dayRemain <= 0)
        {
            g_bIsTimeExpired = true;
            LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is invalid!");
            return;
        }
        
        g_bIsTimeExpired = false;
        LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is valid!");
    }
    else
    {
        g_bIsTimeExpired = true;
        LOGMESSAGE("CSDMApp::OnQueryValidPeriod", " The RegCode is invalid!");
        return;
    }
}

void CSDMApp::GetLastExitTime()
{
    //��ע����л�ȡ�ϴε�¼ʱ��,��֤ϵͳû�зǷ��ı�ʱ��
    HKEY hKey;
    DWORD dwSize = 20, dwType = REG_SZ;
    char owner_Get[20];
    memset(owner_Get, 0, 20);
    LOGMESSAGE("CSDMApp::GetLastExitTime", "Enter open the regedit module!");
    
    if(ERROR_SUCCESS ==::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\CSecureCD"), NULL, KEY_READ, &hKey))
    {
        LOGMESSAGE("CSDMApp::GetLastExitTime", "Open the regedit success!");
        
        if(ERROR_SUCCESS ==::RegQueryValueEx(hKey, _T("ExitTime"), NULL, &dwType, (BYTE *)owner_Get, &dwSize))
        {
            //��ȡע����м�¼�ĵ�¼ʱ��
            char *pPos = strstr(owner_Get, "-");
            int iYear = atoi(owner_Get);
            int iMonth = atoi(pPos + 1);
            pPos = strstr(pPos + 1, "-");
            int iDay = atoi(pPos + 1);
            pPos = strstr(pPos + 1, "-");
            int iHour = atoi(pPos + 1);
            pPos = strstr(pPos + 1, "-");
            int iMin = atoi(pPos + 1);
            pPos = strstr(pPos + 1, "-");
            int iSec = atoi(pPos + 1);
            struct tm sourcedate;
            memset((void*)&sourcedate, 0, sizeof(sourcedate));
            sourcedate.tm_sec = iSec;
            sourcedate.tm_min = iMin;
            sourcedate.tm_hour = iHour;
            sourcedate.tm_mday = iDay;
            sourcedate.tm_mon = iMonth - 1;
            sourcedate.tm_year = iYear - 1900;
            time_t nLastLoginTime = mktime(&sourcedate);
            //����ǰʱ����бȽ�
            time_t nRemainTime = 0;
            time_t nlongCurTime;
            time(&nlongCurTime);
            nRemainTime = nlongCurTime - nLastLoginTime;
            
            if(nRemainTime > 0)
            {
                CPublicFunction::WriteExitTimeReg();
            }
            else
            {
                if(abs((double)nRemainTime) > 60)
                {
                    AfxMessageBox(GetIniStringValue("MainDialog", "SystemTimeisModPrompt", g_strCurrentLanguage)); //ϵͳʱ���뵱ǰʱ�䲻һ�£��޷��������
                    LOGMESSAGE("CSDMApp::GetLastExitTime", "The system time is inconsistent with the current time,so the software can't start!");
                    ::RegCloseKey(hKey);
                    exit(0);
                }
            }
            
            LOGMESSAGE("CSDMApp::GetLastExitTime", "Get valid time success!");
        }
        else
        {
            LOGMESSAGE("CSDMApp::GetLastExitTime", "Get valid time failure!");
            ::RegCloseKey(hKey);
            CPublicFunction::LostRegInfoPrompt();
        }
    }
    else//��ע���ʧ��
    {
        LOGMESSAGE("CSDMApp::GetLastExitTime", "Open the regedit failure!");
        ::RegCloseKey(hKey);
        CPublicFunction::LostRegInfoPrompt();
    }
    
    ::RegCloseKey(hKey);
}