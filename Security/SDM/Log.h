// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__433D6B93_2870_4992_937D_5DD1DB5718CB__INCLUDED_)
#define AFX_LOG_H__433D6B93_2870_4992_937D_5DD1DB5718CB__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000
#include <Afxtempl.h>
#include <map>
#include "MyLock.h"
using namespace std;

enum LogLevel  //��־�ȼ�
{
    Log_Debug = 0, //������Ϣ�������������
    Log_Info = 1, //������Ϣ������������
    Log_Warn = 2, //������Ϣ
    Log_Err = 3   //������Ϣ��������������
};

typedef struct _tagLogMsg
{
    int level;
    CString msg;
} LogMsg;

class CLog  //��־��
{
    public:
        CLog();
        virtual ~CLog();
        
        // call this first!
        void		Init(const char *pOutputFilename, int iLogLevel);
        
        /////////////////////////////////////////////////////////////////////////////
        // turn it on or off
        void		Enable(BOOL bEnable);
        
        /////////////////////////////////////////////////////////////////////////////
        // timestamp each line?
        void     PrintTime(BOOL b) {m_bPrintTime = b;}
        
        /////////////////////////////////////////////////////////////////////////////
        // print the application name?
        void     PrintAppName(BOOL b) {m_bPrintAppName = b;}
        
        /////////////////////////////////////////////////////////////////////////////
        // override the default app name, which is the name the EXE (minus the ".exe")
        void     SetAppName(const char *pName) {m_csAppName = pName;}
        
        void     InsertMsg(const CString &logFileName, int iLogLevel, const CString &msg);
        
        void     Uninit();
        
        CMyLock m_critSect;
        
    protected:
        /////////////////////////////////////////////////////////////////////////////
        // work buffer
        enum {TBUF_SIZE = 3000};
        char	m_tBuf[TBUF_SIZE];
        
        CString  m_csAppName;
        CString	m_csFileName;
        CString  m_logDir;
        CWinThread  *m_pThread;
        multimap<CString, LogMsg> m_logMsgMap;
        BOOL      m_hasData;
        BOOL      m_insertData;
        
        
        /////////////////////////////////////////////////////////////////////////////
        // protection
        CRITICAL_SECTION  m_crit;
        
        /////////////////////////////////////////////////////////////////////////////
        // controlling stuff
        BOOL		m_bEnable;
        BOOL     m_bPrintTime;
        BOOL     m_bPrintAppName;
        int      m_iLogLevelFlag;     //0-���Եȼ���1-������Ϣ�ȼ���2����ȼ���3����ȼ�
        /////////////////////////////////////////////////////////////////////////////
        // string utils
        CString GetBaseDir(const CString & path);
        CString GetBaseName(const CString & path);
        CString ChangeLogName(const CString &filename);
        
        ////////////////////////���̺߳���
        static UINT WriteThread(LPVOID lParam);
};

#endif // !defined(AFX_LOG_H__433D6B93_2870_4992_937D_5DD1DB5718CB__INCLUDED_)
