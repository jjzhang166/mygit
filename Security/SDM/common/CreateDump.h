#ifndef CREATEDUMP_TH_H_
#define CREATEDUMP_TH_H_

#include "PublicFunction.h"

/************************************************************
 ����Dump�ļ� ��ȡ�������е�·��
 ************************************************************/
CString GetExePath()
{
    CString strFullPath;
    char m_Buff[MAX_PATH];
    memset(m_Buff, 0, MAX_PATH);
    GetModuleFileName(NULL, m_Buff, MAX_PATH);
    strFullPath = m_Buff;
    int nReturnLen = strFullPath.ReverseFind('\\');
    CString strReturnPath = strFullPath.Left(nReturnLen);
    return strReturnPath;
}

//�õ���ǰʱ��
CString GetPresentTime()
{
    CTime cTime = CTime::GetCurrentTime();
    CString strCurrentTime(_T(""));
    strCurrentTime.Format("%d%.2d%.2d_%.2d%.2d%.2d",
                          cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(),
                          cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());
    return strCurrentTime;
}

//����Dump�ļ�
void CreateDumpFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
    // ����Dump�ļ�
    HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    // Dump��Ϣ
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;
    // д��Dump�ļ�����
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    CloseHandle(hDumpFile);
}

// �����쳣�Ļص�����
LONG CrashHandler(EXCEPTION_POINTERS *pException)
{
    // �Ե�ǰʱ��Ϊ�ļ���
    CString path = GetExePath();
    path += "\\Dump\\";
    
    if(!PathFileExists(path))
    {
        CreateDirectory(path, NULL);
    }
    
    CString strDumpFileName = path;
    strDumpFileName += GetPresentTime() + _T(".dmp");
    // ����Dump�ļ�
    CreateDumpFile(strDumpFileName, pException);
    static  bool showed = false;
    
    if(!showed)
    {
        showed = true;
        AfxMessageBox(GetIniStringValue("MainDialog", "AppErrorMsg", g_strCurrentLanguage));
    }
    
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif