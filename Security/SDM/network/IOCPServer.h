#pragma once
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")


#define MAX_BUFFER_LEN        8192				// ���������� (1024*8) ���ȷʵ�ͻ��˷�����ÿ�����ݶ��Ƚ��٣���ô�����õ�СһЩ��ʡ�ڴ�
#define DEFAULT_PORT          12345				// Ĭ�϶˿�
#define DEFAULT_IP            _T("127.0.0.1")	// Ĭ��IP��ַ
#define PACK_SIZE            1024

// ����ɶ˿���Ͷ�ݵ�I/O����������
typedef enum OpType
{
    AcceptPosted,                     // ��־Ͷ�ݵ�Accept����
    SendPosted,                       // ��־Ͷ�ݵ��Ƿ��Ͳ���
    RecvPosted,                       // ��־Ͷ�ݵ��ǽ��ղ���
    NullPosted                        // ���ڳ�ʼ����������
} OperateType;

typedef struct PerIOContext
{
    OVERLAPPED     overLapped;						// ÿһ���ص�����������ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)
    SOCKET         sockAccept;						// ������������ʹ�õ�Socket
    WSABUF         wsaBuf;							// WSA���͵Ļ����������ڸ��ص�������������
    char           szBuffer[MAX_BUFFER_LEN];		// �����WSABUF�������ַ��Ļ�����
    OperateType	   opType;							// ��ʶ�������������(��Ӧ�����ö��)
    bool           busy;							// ��ʶ��IO�����Ƿ���æµ�У�trueΪæ���С���Ҫ���ڷ������ݵ��ж�
    
    // ��ʼ��
    PerIOContext()
    {
        ZeroMemory(&overLapped, sizeof(overLapped));
        ZeroMemory(szBuffer, MAX_BUFFER_LEN);
        sockAccept = INVALID_SOCKET;
        wsaBuf.buf = szBuffer;
        wsaBuf.len = MAX_BUFFER_LEN;
        opType     = NullPosted;
        busy       = false;
    }
    // �ͷŵ�Socket
    ~PerIOContext()
    {
        if(sockAccept != INVALID_SOCKET)
        {
            closesocket(sockAccept);
            sockAccept = INVALID_SOCKET;
        }
    }
    // ���û���������
    void ResetBuffer()
    {
        ZeroMemory(szBuffer, MAX_BUFFER_LEN);
    }
    
} PerIOContext, *PPerIOContext;

typedef struct PerSocketContext
{
    SOCKET      m_Socket;						// ÿһ���ͻ������ӵ�Socket
    SOCKADDR_IN m_clientAddr;					// �ͻ��˵ĵ�ַ
    CArray<PerIOContext*> m_arrayIoContext;		// �ͻ���������������������ݣ�
    // Ҳ����˵����ÿһ���ͻ���Socket���ǿ���������ͬʱͶ�ݶ��IO�����
    
    // ��ʼ��
    PerSocketContext()
    {
        m_Socket = INVALID_SOCKET;
        memset(&m_clientAddr, 0, sizeof(m_clientAddr));
    }
    
    // �ͷ���Դ
    ~PerSocketContext()
    {
        if(m_Socket != INVALID_SOCKET)
        {
            closesocket(m_Socket);
            m_Socket = INVALID_SOCKET;
        }
        
        // �ͷŵ����е�IO����������//�ص����е�IO����
        for(int i = 0; i < m_arrayIoContext.GetCount(); i++)
        {
            delete m_arrayIoContext.GetAt(i);
        }
        
        m_arrayIoContext.RemoveAll();
    }
    
    // ��ȡһ���µ�IoContext
    PerIOContext* GetNewIoContext()
    {
        PerIOContext* p = new PerIOContext;
        m_arrayIoContext.Add(p);
        return p;
    }
    
    // ���������Ƴ�һ��ָ����IoContext
    void RemoveContext(PerIOContext* pContext)
    {
        //����
        ASSERT(pContext != NULL);
        
        for(int i = 0; i < m_arrayIoContext.GetCount(); i++)
        {
            if(pContext == m_arrayIoContext.GetAt(i))
            {
                delete pContext;
                pContext = NULL;
                m_arrayIoContext.RemoveAt(i);
                break;
            }
        }
    }
    
} PerSocketContext, *PPerSocketContext;

// �������̵߳��̲߳���
class CIOCPServer;
//�����̶߳��Ǿ�̬�������̺߳����Ǿ�̬������
typedef struct WorkerThreadParams
{
    CIOCPServer* pIOCPServer;                                   // ��ָ�룬���ڵ������еĺ���
    int         threadNo;                                    // �̱߳��
    
} WorkThreadParams, *PWorkThreadParams;

class CIOCPServer
{
    public:
        CIOCPServer(void);
        ~CIOCPServer(void);
        
    public:
        bool Start();	//�������������
        void Stop();	//ֹͣ��رշ�����
        bool LoadSocketLib();	//�����׽��ֿ���Ϣ
        void UnloadSocketLib() { WSACleanup(); }	//ж���׽��ֿ���Ϣ
        void SetPort(const int& nPort) { m_nPort = nPort; }
        void SetMainDlg(CDialog* p) { m_pMain = p; }
        bool SendData(char* dwAddr, int nPort, WSABUF wsaBuf, WORD buflen);
        
        CString GetLocalIP();
		void SetServerIpPort(CString ip, int port);
        
    protected:
        bool InitializeIOCP();	//��ʼ���˿�
        bool InitializeListenSocket();	//��ʼ�������׽���
        bool PostAccept(PerIOContext* pAcceptIoContext);   //�˿���������
        bool PostRecv(PerIOContext* pIoContext);
        bool PostSend(PerIOContext* pIoContext);
        bool DoAccpet(PerSocketContext* pSocketContext, PerIOContext* pIoContext);
        bool DoRecv(PerSocketContext* pSocketContext, PerIOContext* pIoContext);
        bool DoSend(PerSocketContext* pSocketContext, PerIOContext* pIoContext);
        bool HandleError(PerSocketContext *pContext, const DWORD& dwErr);
        bool AssociateWithIOCP(PerSocketContext *pContext);
        bool IsSocketAlive(SOCKET s);
        
        void AddToContextList(PerSocketContext *pSocketContext, WSABUF wsabuf);
        void RemoveContext(PerSocketContext *pSocketContext);
        void ShowMessage(const CString szFormat, ...) const;
        void ClearContextList();
        void DeInitialize();	//�������������ʱ�ģ��ͷ���Դ
        
        static DWORD WINAPI WorkerThread(LPVOID lpParam);
        int GetNoOfProcessors();
        
        PerIOContext* GetCliectIOContext(PerSocketContext * sendSock);
        PerSocketContext* GetClientSocktContext(char* dwAddr, int nPort);
        
    private:
    
        HANDLE                       m_hShutdownEvent;              // ����֪ͨ�߳�ϵͳ�˳����¼���Ϊ���ܹ����õ��˳��߳�
        HANDLE                       m_hIOCompletionPort;           // ��ɶ˿ڵľ��
        HANDLE*                      m_phWorkerThreads;             // �������̵߳ľ��ָ������
        CString                      m_strIP;                       // �������˵�IP��ַ
        
        CDialog*                     m_pMain;                       // ������Ľ���ָ�룬����������������ʾ��Ϣ
        CRITICAL_SECTION             m_csContextList;               // ����Worker�߳�ͬ���Ļ�����
        CArray<PerSocketContext*>	 m_arrayClientContext;          // �ͻ���Socket��Context��Ϣ
        PerSocketContext*            m_pListenContext;              // ���ڼ�����Socket��Context��Ϣ
        
        LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx �� GetAcceptExSockaddrs �ĺ���ָ�룬���ڵ�����������չ����
        LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs;
        int		                     m_nThreads;                    // ���ɵ��߳�����
        int                          m_nPort;                       // �������˵ļ����˿�
        bool                         m_start;
};

