#include "StdAfx.h"
#include "IOCPServer.h"
#include "../SDMDlg.h"

#define WORKER_THREADS_PER_PROCESSOR 2
#define MAX_POST_ACCEPT              10
#define EXIT_CODE                    NULL

#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
#define RELEASE_HANDLE(x)               {if(x != NULL && x!=INVALID_HANDLE_VALUE){ CloseHandle(x);x = NULL;}}
#define RELEASE_SOCKET(x)               {if(x !=INVALID_SOCKET) { closesocket(x);x=INVALID_SOCKET;}}

CIOCPServer::CIOCPServer(void):
    m_nThreads(0),
    m_hShutdownEvent(NULL),
    m_hIOCompletionPort(NULL),
    m_phWorkerThreads(NULL),
    m_strIP(DEFAULT_IP),
    m_nPort(DEFAULT_PORT),
    m_pMain(NULL),
    m_lpfnAcceptEx(NULL),
    m_pListenContext(NULL),
    m_start(FALSE)
{
}

CIOCPServer::~CIOCPServer(void)
{
    this->Stop();
}


// �������̣߳�ΪIOCP�������Ĺ������߳�
// Ҳ����ÿ����ɶ˿��ϳ�����������ݰ����ͽ�֮ȡ�������д�����߳�
DWORD WINAPI CIOCPServer::WorkerThread(LPVOID lpParam)
{
    WorkThreadParams* pParam = (WorkThreadParams*)lpParam;
    CIOCPServer* pIOCPServer = (CIOCPServer*)pParam->pIOCPServer;
    int threadNo = (int)pParam->threadNo;
    pIOCPServer->ShowMessage(_T("�������߳�������ID: %d."), threadNo);
    OVERLAPPED           *pOverlapped = NULL;
    PerSocketContext     *pSocketContext = NULL;
    DWORD                dwBytesTransfered = 0;
    
    while(WAIT_OBJECT_0 != WaitForSingleObject(pIOCPServer->m_hShutdownEvent, 0))
    {
        BOOL bReturn = GetQueuedCompletionStatus(
                           pIOCPServer->m_hIOCompletionPort,
                           &dwBytesTransfered,
                           (PULONG_PTR)&pSocketContext,
                           &pOverlapped,
                           INFINITE);
                           
        if(EXIT_CODE == (DWORD)pSocketContext)
        {
            break;
        }
        
        if(!bReturn)
        {
            DWORD dwErr = GetLastError();
            
            if(!pIOCPServer->HandleError(pSocketContext, dwErr))
            {
                break;
            }
            
            continue;
        }
        else
        {
            PerIOContext* pIoContext = CONTAINING_RECORD(pOverlapped, PerIOContext, overLapped);
            
			if (0 == dwBytesTransfered)
			{
				continue;
			}

            if((0 == dwBytesTransfered) && (RecvPosted == pIoContext->opType || SendPosted == pIoContext->opType))
            {
                pIOCPServer->ShowMessage(_T("�ͻ��� %s:%d �Ͽ�����."), inet_ntoa(pSocketContext->m_clientAddr.sin_addr), ntohs(pSocketContext->m_clientAddr.sin_port));
                pIOCPServer->RemoveContext(pSocketContext);
                continue;
            }
            else
            {
                switch(pIoContext->opType)
                {
                    // Accept
                    case AcceptPosted:
                        {
                            // Ϊ�����Ӵ���ɶ��ԣ�������ר�ŵ�_DoAccept�������д�����������
                            pIOCPServer->DoAccpet(pSocketContext, pIoContext);
                        }
                        break;
                        
                    // RECV
                    case RecvPosted:
                        {
                            // Ϊ�����Ӵ���ɶ��ԣ�������ר�ŵ�DoRecv�������д����������
                            pIOCPServer->DoRecv(pSocketContext, pIoContext);
                        }
                        break;
                        
                    // SEND
                    // ����ֻ��Ϊ�˼��һ�·����Ƿ�ɹ�����Ȼ�����������ж������Ƿ���Ҫ�ط�
                    case SendPosted:
                        {
                            pIOCPServer->DoSend(pSocketContext, pIoContext);
                        }
                        break;
                        
                    default:
                        // ��Ӧ��ִ�е�����
                        TRACE(_T("_WorkThread�е� pIoContext->opType �����쳣.\n"));
                        break;
                } //switch
            }//if
        }//if
    }//while
    
    TRACE(_T("�������߳� %d ���˳�.\n"), threadNo);
    // �ͷ��̲߳���
    RELEASE(lpParam);
    return 0;
}

bool CIOCPServer::LoadSocketLib()
{
    WSADATA wsaData;
    int nResult;
    nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    if(NO_ERROR != nResult)
    {
        this->ShowMessage(_T("��ʼ��WinSock 2.2ʧ�ܣ�\n"));
        return false;
    }
    
    return true;
}

bool CIOCPServer::Start()
{
    InitializeCriticalSection(&m_csContextList);	//��ʼ���ٽ���
    // ����ϵͳ�˳����¼�֪ͨ
    m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    // ��ʼ��IOCP
    if(false == InitializeIOCP())
    {
        this->ShowMessage(_T("��ʼ��IOCPʧ�ܣ�\n"));
        return false;
    }
    else
    {
        this->ShowMessage(_T("\nIOCP��ʼ�����\n."));
    }
    
    // ��ʼ��Socket
    if(false == InitializeListenSocket())
    {
        this->ShowMessage(_T("Listen Socket��ʼ��ʧ�ܣ�\n"));
        this->DeInitialize();
        return false;
    }
    else
    {
        this->ShowMessage(_T("Listen Socket��ʼ�����."));
    }
    
    this->ShowMessage(_T("ϵͳ׼���������Ⱥ�����....\n"));
    //��־������������
    this->m_start = TRUE;
    return true;
}


//	��ʼ����ϵͳ�˳���Ϣ���˳���ɶ˿ں��߳���Դ
void CIOCPServer::Stop()
{
    if(m_pListenContext != NULL && m_pListenContext->m_Socket != INVALID_SOCKET)
    {
        // ����ر���Ϣ֪ͨ
        SetEvent(m_hShutdownEvent);
        
        for(int i = 0; i < m_nThreads; i++)
        {
            // ֪ͨ���е���ɶ˿ڲ����˳�
            PostQueuedCompletionStatus(m_hIOCompletionPort, 0, (DWORD)EXIT_CODE, NULL);
        }
        
        // �ȴ����еĿͻ�����Դ�˳�
        WaitForMultipleObjects(m_nThreads, m_phWorkerThreads, TRUE, INFINITE);
        // ����ͻ����б���Ϣ
        this->ClearContextList();
        // �ͷ�������Դ
        this->DeInitialize();
        this->ShowMessage(_T("ֹͣ����\n"));
    }
}

// ��ʼ����ɶ˿�
bool CIOCPServer::InitializeIOCP()
{
    // ������һ����ɶ˿�
    m_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    
    if(NULL == m_hIOCompletionPort)
    {
        this->ShowMessage(_T("������ɶ˿�ʧ�ܣ��������: %d!\n"), WSAGetLastError());
        return false;
    }
    
    // ���ݱ����еĴ�����������������Ӧ���߳�����CPU������*2 + 2.
    m_nThreads = WORKER_THREADS_PER_PROCESSOR * GetNoOfProcessors() + 2;
    // Ϊ�������̳߳�ʼ�����
    m_phWorkerThreads = new HANDLE[m_nThreads];
    // ���ݼ�����������������������߳�
    DWORD nThreadID;
    
    for(int i = 0; i < m_nThreads; i++)
    {
        WorkThreadParams* pThreadParams = new WorkThreadParams;
        pThreadParams->pIOCPServer = this;
        pThreadParams->threadNo  = i + 1;
        m_phWorkerThreads[i] = ::CreateThread(0, 0, WorkerThread, (void *)pThreadParams, 0, &nThreadID);
    }
    
    TRACE(" ���� WorkerThread %d ��.\n", m_nThreads);
    return true;
}

// ��ʼ��Socket
bool CIOCPServer::InitializeListenSocket()
{
    // AcceptEx �� GetAcceptExSockaddrs ��GUID�����ڵ�������ָ��
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
    // ��������ַ��Ϣ�����ڰ�Socket
    struct sockaddr_in ServerAddress;
    // �������ڼ�����Socket����Ϣ
    m_pListenContext = new PerSocketContext;
    // ��Ҫʹ���ص�IO�������ʹ��WSASocket������Socket���ſ���֧���ص�IO����
    m_pListenContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    
    if(INVALID_SOCKET == m_pListenContext->m_Socket)
    {
        this->ShowMessage(_T("��ʼ��Socketʧ�ܣ��������: %d.\n"), WSAGetLastError());
        return false;
    }
    else
    {
        TRACE("WSASocket() ���.\n");
    }
    
    // ��Listen Socket������ɶ˿���
    if(NULL == CreateIoCompletionPort((HANDLE)m_pListenContext->m_Socket, m_hIOCompletionPort, (DWORD)m_pListenContext, 0))
    {
        this->ShowMessage(_T("�� Listen Socket����ɶ˿�ʧ�ܣ��������: %d/n"), WSAGetLastError());
        RELEASE_SOCKET(m_pListenContext->m_Socket);
        return false;
    }
    else
    {
        TRACE("Listen Socket����ɶ˿� ���.\n");
    }
    
    ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    //ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerAddress.sin_addr.s_addr = inet_addr(m_strIP.GetString());
    ServerAddress.sin_port = htons(m_nPort);
    
    if(SOCKET_ERROR == bind(m_pListenContext->m_Socket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)))
    {
        this->ShowMessage(_T("bind()����ִ�д���.\n"));
        return false;
    }
    else
    {
        TRACE("bind() ���.\n");
    }
    
    // ��ʼ���м���
    if(SOCKET_ERROR == listen(m_pListenContext->m_Socket, SOMAXCONN))
    {
        this->ShowMessage(_T("Listen()����ִ�г��ִ���.\n"));
        return false;
    }
    else
    {
        TRACE("Listen() ���.\n");
    }
    
    // ʹ��AcceptEx��������Ϊ���������WinSock2�淶֮���΢�������ṩ����չ����
    // ������Ҫ�����ȡһ�º�����ָ�룬
    // ��ȡAcceptEx����ָ��
    DWORD dwBytes = 0;
    
    if(SOCKET_ERROR == WSAIoctl(
                m_pListenContext->m_Socket,
                SIO_GET_EXTENSION_FUNCTION_POINTER,
                &GuidAcceptEx,
                sizeof(GuidAcceptEx),
                &m_lpfnAcceptEx,
                sizeof(m_lpfnAcceptEx),
                &dwBytes,
                NULL,
                NULL))
    {
        this->ShowMessage(_T("WSAIoctl δ�ܻ�ȡAcceptEx����ָ�롣�������: %d\n"), WSAGetLastError());
        this->DeInitialize();
        return false;
    }
    
    // ��ȡGetAcceptExSockAddrs����ָ�룬Ҳ��ͬ��
    if(SOCKET_ERROR == WSAIoctl(
                m_pListenContext->m_Socket,
                SIO_GET_EXTENSION_FUNCTION_POINTER,
                &GuidGetAcceptExSockAddrs,
                sizeof(GuidGetAcceptExSockAddrs),
                &m_lpfnGetAcceptExSockAddrs,
                sizeof(m_lpfnGetAcceptExSockAddrs),
                &dwBytes,
                NULL,
                NULL))
    {
        this->ShowMessage(_T("WSAIoctl δ�ܻ�ȡGuidGetAcceptExSockAddrs����ָ�롣�������: %d\n"), WSAGetLastError());
        this->DeInitialize();
        return false;
    }
    
    // ΪAcceptEx ׼��������Ȼ��Ͷ��AcceptEx I/O����
    for(int i = 0; i < MAX_POST_ACCEPT; i++)
    {
        // �½�һ��IO_CONTEXT
        PerIOContext* pAcceptIoContext = m_pListenContext->GetNewIoContext();
        
        if(false == this->PostAccept(pAcceptIoContext))
        {
            m_pListenContext->RemoveContext(pAcceptIoContext);
            return false;
        }
    }
    
    this->ShowMessage(_T("Ͷ�� %d ��AcceptEx�������"), MAX_POST_ACCEPT);
    return true;
}

void CIOCPServer::DeInitialize()
{
    DeleteCriticalSection(&m_csContextList);
    RELEASE_HANDLE(m_hShutdownEvent);
    
    for(int i = 0; i < m_nThreads; i++)
    {
        RELEASE_HANDLE(m_phWorkerThreads[i]);
    }
    
    RELEASE(m_phWorkerThreads);
    RELEASE_HANDLE(m_hIOCompletionPort);
    RELEASE(m_pListenContext);
    this->ShowMessage(_T("�ͷ���Դ���.\n"));
}

bool CIOCPServer::PostAccept(PerIOContext* pAcceptIoContext)
{
    ASSERT(INVALID_SOCKET != m_pListenContext->m_Socket);
    // ׼������
    DWORD dwBytes = 0;
    pAcceptIoContext->opType = AcceptPosted;
    WSABUF *p_wbuf   = &pAcceptIoContext->wsaBuf;
    OVERLAPPED *p_ol = &pAcceptIoContext->overLapped;
    // Ϊ�Ժ�������Ŀͻ�����׼����Socket( ������봫ͳaccept�������� )
    pAcceptIoContext->sockAccept  = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    
    if(INVALID_SOCKET == pAcceptIoContext->sockAccept)
    {
        ShowMessage(_T("��������Accept��Socketʧ�ܣ��������: %d"), WSAGetLastError());
        return false;
    }
    
    // Ͷ��AcceptEx
    if(FALSE == m_lpfnAcceptEx(m_pListenContext->m_Socket, pAcceptIoContext->sockAccept, p_wbuf->buf, p_wbuf->len - ((sizeof(SOCKADDR_IN) + 16) * 2),
                               sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, p_ol))
    {
        int n = (sizeof(SOCKADDR_IN) + 16) * 2;
        
        if(WSA_IO_PENDING != WSAGetLastError())
        {
            ShowMessage(_T("Ͷ�� AcceptEx ����ʧ�ܣ��������: %d"), WSAGetLastError());
            return false;
        }
    }
    
    return true;
}


// ���пͻ��������ʱ�򣬽��д���
// �����е㸴�ӣ���Ҫ�ǿ������Ļ����Ϳ����׵��ĵ���....
// ������������Ļ�����ɶ˿ڵĻ������������һ�����

// ��֮��Ҫ֪�����������ListenSocket��Context��������Ҫ����һ�ݳ������������Socket��
// ԭ����Context����Ҫ���������Ͷ����һ��Accept����
bool CIOCPServer::DoAccpet(PerSocketContext* pSocketContext, PerIOContext* pIoContext)
{
    SOCKADDR_IN* clientAddr = NULL;
    SOCKADDR_IN* LocalAddr = NULL;
    int remoteLen = sizeof(SOCKADDR_IN), localLen = sizeof(SOCKADDR_IN);
    // 1. ����ȡ������ͻ��˵ĵ�ַ��Ϣ
    // ��� m_lpfnGetAcceptExSockAddrs �����˰�~~~~~~
    // ��������ȡ�ÿͻ��˺ͱ��ض˵ĵ�ַ��Ϣ������˳��ȡ���ͻ��˷����ĵ�һ�����ݣ���ǿ����...
    this->m_lpfnGetAcceptExSockAddrs(pIoContext->wsaBuf.buf, pIoContext->wsaBuf.len - ((sizeof(SOCKADDR_IN) + 16) * 2),
                                     sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&clientAddr, &remoteLen);
    this->ShowMessage(_T("�ͻ��� %s:%d ����."), inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port));
    this->ShowMessage(_T("�ͻ��� %s:%d ��Ϣ��%s."), inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port), pIoContext->wsaBuf.buf);
    // 2. ������Ҫע�⣬���ﴫ��������ListenSocket�ϵ�Context�����Context���ǻ���Ҫ���ڼ�����һ������
    // �����һ���Ҫ��ListenSocket�ϵ�Context���Ƴ���һ��Ϊ�������Socket�½�һ��SocketContext
    PerSocketContext* pNewSocketContext = new PerSocketContext;
    pNewSocketContext->m_Socket = pIoContext->sockAccept;
    memcpy(&(pNewSocketContext->m_clientAddr), clientAddr, sizeof(SOCKADDR_IN));
    
    // ����������ϣ������Socket����ɶ˿ڰ�(��Ҳ��һ���ؼ�����)
    if(false == this->AssociateWithIOCP(pNewSocketContext))
    {
        RELEASE(pNewSocketContext);
        return false;
    }
    
    // 3. �������������µ�IoContext�����������Socket��Ͷ�ݵ�һ��Recv��������
    PerIOContext* pNewIoContext = pNewSocketContext->GetNewIoContext();
    pNewIoContext->opType       = RecvPosted;
    pNewIoContext->sockAccept   = pNewSocketContext->m_Socket;
    // ���Buffer��Ҫ���������Լ�����һ�ݳ���
    //memcpy( pNewIoContext->szBuffer,pIoContext->szBuffer,MAX_BUFFER_LEN );
    
    // �����֮�󣬾Ϳ��Կ�ʼ�����Socket��Ͷ�����������
    if(false == this->PostRecv(pNewIoContext))
    {
        pNewSocketContext->RemoveContext(pNewIoContext);
        return false;
    }
    
    // 4. ���Ͷ�ݳɹ�����ô�Ͱ������Ч�Ŀͻ�����Ϣ�����뵽ContextList��ȥ(��Ҫͳһ���������ͷ���Դ)
    this->AddToContextList(pNewSocketContext, pIoContext->wsaBuf);

    // 5. ʹ�����֮�󣬰�Listen Socket���Ǹ�IoContext���ã�Ȼ��׼��Ͷ���µ�AcceptEx
    pIoContext->ResetBuffer();
    return this->PostAccept(pIoContext);
}


// Ͷ�ݽ�����������
bool CIOCPServer::PostRecv(PerIOContext* pIoContext)
{
    // ��ʼ������
    DWORD dwFlags = 0;
    DWORD dwBytes = 0;
    WSABUF *p_wbuf   = &pIoContext->wsaBuf;
    OVERLAPPED *p_ol = &pIoContext->overLapped;
    pIoContext->ResetBuffer();
    pIoContext->opType = RecvPosted;
    // ��ʼ����ɺ󣬣�Ͷ��WSARecv����
    int nBytesRecv = WSARecv(pIoContext->sockAccept, p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL);
    
    // �������ֵ���󣬲��Ҵ���Ĵ��벢����Pending�Ļ����Ǿ�˵������ص�����ʧ����
    if((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
    {
        this->ShowMessage(_T("Ͷ�ݵ�һ��WSARecvʧ�ܣ�"));
        return false;
    }
    
    return true;
}

//Ͷ�ݷ�������
bool CIOCPServer::PostSend(PerIOContext* pIoContext)
{
    DWORD dwFlags = 0;
    DWORD dwBytes = 0;
    WSABUF *p_wbuf = &pIoContext->wsaBuf;
    OVERLAPPED *p_ol = &pIoContext->overLapped;
    //��IO�����ı�Ϊæ��״̬
    pIoContext->busy = true;
    //ʹ����ɶ˿ڷ���
    int nBytesSend = WSASend(pIoContext->sockAccept, p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL);
    int nError = WSAGetLastError();
    
    if((SOCKET_ERROR == nBytesSend) && (WSA_IO_PENDING != nError))
    {
        ShowMessage(TEXT("����ɶ˿����ύSEND����ʧ��, code: %d"), nError);
        //_RELEASE_IO_DATA(pIO);//�ͷ�PER_IO_DATA�ڴ�
        return false;
    }
    
    return true;
}

// ���н��յ����ݵ����ʱ�򣬽��д���
bool CIOCPServer::DoRecv(PerSocketContext* pSocketContext, PerIOContext* pIoContext)
{
    // �Ȱ���һ�ε�������ʾ���֣�Ȼ�������״̬��������һ��Recv����
    SOCKADDR_IN* clientAddr = &pSocketContext->m_clientAddr;
    this->ShowMessage(_T("�յ�  %s:%d ��Ϣ��%s"), inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port), pIoContext->wsaBuf.buf);
    ClientInfo *cliect = new ClientInfo;
    strcpy_s(cliect->ipAddress, inet_ntoa(clientAddr->sin_addr));
    cliect->nPort = ntohs(clientAddr->sin_port);
    char *str = pIoContext->wsaBuf.buf;
    theApp.m_SDCommData.RecvData(cliect, str);
    // Ȼ��ʼͶ����һ��WSARecv����
    return PostRecv(pIoContext);
}
//�ڷ������ݺ���м��
bool CIOCPServer::DoSend(PerSocketContext* pSocketContext, PerIOContext* pIoContext)
{
    DWORD nSendByte = 0;
    DWORD dwFlag = 0;
    //��÷��͵Ľ��
    BOOL result = WSAGetOverlappedResult(pSocketContext->m_Socket, &pIoContext->overLapped, &nSendByte, TRUE, &dwFlag);
    
    if(result) ////////////////////////////////dont check send bytes here
    {
        ShowMessage(_T("�������ݳɹ���"));
        //��IO���������ı�Ϊ����״̬
        pIoContext->busy = false;
    }
    else
    {
        ShowMessage(TEXT("��������ʧ��, ������Ϊ %d"), WSAGetLastError());
    }
    
    return (result != 0);
}


// �����(Socket)�󶨵���ɶ˿���
bool CIOCPServer::AssociateWithIOCP(PerSocketContext *pContext)
{
    // �����ںͿͻ���ͨ�ŵ�SOCKET�󶨵���ɶ˿���
    HANDLE hTemp = CreateIoCompletionPort((HANDLE)pContext->m_Socket, m_hIOCompletionPort, (DWORD)pContext, 0);
    
    if(NULL == hTemp)
    {
        this->ShowMessage(_T("ִ��CreateIoCompletionPort()���ִ���.������룺%d"), GetLastError());
        return false;
    }
    
    return true;
}

// ���ͻ��˵������Ϣ�洢��������
void CIOCPServer::AddToContextList(PerSocketContext *pHandleData, WSABUF wsabuf)
{
    //�ѿͻ�����Ϣ����
    EnterCriticalSection(&m_csContextList);
    m_arrayClientContext.Add(pHandleData);
    LeaveCriticalSection(&m_csContextList);
    //�ѿͻ�����Ϣ����ڽ����������
    ClientInfo *cliect = new ClientInfo;
    strcpy_s(cliect->ipAddress, inet_ntoa(pHandleData->m_clientAddr.sin_addr));
    int len = strlen(cliect->ipAddress);
    cliect->ipAddress[len] = 0;
    cliect->nPort = ntohs(pHandleData->m_clientAddr.sin_port);
    cliect->wsaBuf = wsabuf;
    cliect->wsaBuf.len = 4;
    cliect->doorID = ((unsigned int) wsabuf.buf[0]) & 0xff;
	cliect->cntOfOnlineJudge = 0;	//�Ƿ����ӵ��жϴ�����ʼΪ0
	cliect->isOfflineFlag = 0;
    theApp.m_SDCommData.AddClient(cliect);
	//��һ�����ݽ��մ���
	theApp.m_SDCommData.RecvData(cliect, wsabuf.buf);
}

//	�Ƴ�ĳ���ض���Context
void CIOCPServer::RemoveContext(PerSocketContext *pSocketContext)
{
    //ɾ������Ŀͻ�����Ϣ
    theApp.m_SDCommData.DeleteClient(inet_ntoa(pSocketContext->m_clientAddr.sin_addr), ntohs(pSocketContext->m_clientAddr.sin_port));
    EnterCriticalSection(&m_csContextList);
    
    for(int i = 0; i < m_arrayClientContext.GetCount(); i++)
    {
        if(pSocketContext == m_arrayClientContext.GetAt(i))
        {
            RELEASE(pSocketContext);
            m_arrayClientContext.RemoveAt(i);
            break;
        }
    }
    
    LeaveCriticalSection(&m_csContextList);
}

// ��տͻ�����Ϣ
void CIOCPServer::ClearContextList()
{
    EnterCriticalSection(&m_csContextList);
    
    for(int i = 0; i < m_arrayClientContext.GetCount(); i++)
    {
        delete m_arrayClientContext.GetAt(i);
    }
    
    m_arrayClientContext.RemoveAll();
    LeaveCriticalSection(&m_csContextList);
}

//���ͻ��˷�������
bool CIOCPServer::SendData(char* dwAddr, int nPort, WSABUF wsaBuf, WORD buflen)
{
    _ASSERTE(NULL != dwAddr);
    
    if(buflen > PACK_SIZE) //ȷ�ϰ��Ĵ�С��������󳤶�
    {
        _ASSERTE(!"Package is too large.");
        return false;
    }
    
    //�жϷ�������û������
    if(!m_start)
    {
        return false;
    }
    
    //�ҵ��Ƿ��д�IP��ַ�Ŀͻ���
    PerIOContext* pContext = NULL ;
    PerSocketContext *pSendSock = NULL ;
    //����IP��ȡ�ͻ���
    pSendSock = GetClientSocktContext(dwAddr, nPort);
    
    //���ÿͻ����Ƿ����
    if(!pSendSock)
    {
        return false;
    }
    
    pContext = GetCliectIOContext(pSendSock);
    pContext->wsaBuf = wsaBuf;
    PostSend(pContext);
    return true;
}

//���ҿͻ����еķ����������Ƿ���ڣ��������򴴽�һ��
PerIOContext* CIOCPServer::GetCliectIOContext(PerSocketContext * sendSock)
{
    PerIOContext* pSendContext = NULL;
    
    for(int i = 0; i < sendSock->m_arrayIoContext.GetCount(); i++)
    {
        if(sendSock->m_arrayIoContext.GetAt(i)->opType == SendPosted && (!sendSock->m_arrayIoContext.GetAt(i)->busy))
        {
            pSendContext = sendSock->m_arrayIoContext.GetAt(i);
            break;
        }
    }
    
    //����������ݵ������Ĳ����������ڣ������´���һ��
    if(!pSendContext)
    {
        pSendContext = sendSock->GetNewIoContext();
        pSendContext->sockAccept = sendSock->m_Socket;
        pSendContext->opType = SendPosted;
    }
    
    return pSendContext;
}

// ��ñ�����IP��ַ
CString CIOCPServer::GetLocalIP()
{
    // ��ñ���������
    char hostname[MAX_PATH] = {0};
    gethostname(hostname, MAX_PATH);
    struct hostent FAR* lpHostEnt = gethostbyname(hostname);
    
    if(lpHostEnt == NULL)
    {
        return DEFAULT_IP;
    }
    
    // ȡ��IP��ַ�б��еĵ�һ��Ϊ���ص�IP(��Ϊһ̨�������ܻ�󶨶��IP)
    LPSTR lpAddr = lpHostEnt->h_addr_list[0];
    // ��IP��ַת�����ַ�����ʽ
    struct in_addr inAddr;
    memmove(&inAddr, lpAddr, 4);
    m_strIP = CString(inet_ntoa(inAddr));
    return m_strIP;
}

void CIOCPServer::SetServerIpPort(CString ip, int port)
{
	m_strIP = ip;
	m_nPort = port;
}

// ��ñ����д�����������
int CIOCPServer::GetNoOfProcessors()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwNumberOfProcessors;
}

// ������������ʾ��ʾ��Ϣ
void CIOCPServer::ShowMessage(const CString szFormat, ...) const
{
    // ���ݴ���Ĳ�����ʽ���ַ���
    CString   strMessage;
    va_list   arglist;
    // ����䳤����
    va_start(arglist, szFormat);
    strMessage.FormatV(szFormat, arglist);
    va_end(arglist);
    // ������������ʾ
    //CSDMDlg* pMain = (CSDMDlg*)m_pMain;
    //
    //if(m_pMain != NULL)
    //{
    //    //pMain->AddInformation(strMessage);
    //    pMain->m_LogListBox.AddString(strMessage);
    //    TRACE(strMessage + _T("\n"));
    //}
}

// �жϿͻ���Socket�Ƿ��Ѿ��Ͽ���������һ����Ч��Socket��Ͷ��WSARecv����������쳣
// ʹ�õķ����ǳ��������socket�������ݣ��ж����socket���õķ���ֵ
// ��Ϊ����ͻ��������쳣�Ͽ�(����ͻ��˱������߰ε����ߵ�)��ʱ�򣬷����������޷��յ��ͻ��˶Ͽ���֪ͨ��
bool CIOCPServer::IsSocketAlive(SOCKET s)
{
    int nByteSent = send(s, "", 0, 0);
    
    if(-1 == nByteSent)
    {
        return false;
    }
    
    return true;
}

// ��ʾ��������ɶ˿��ϵĴ���
bool CIOCPServer::HandleError(PerSocketContext *pContext, const DWORD& dwErr)
{
    // ����ǳ�ʱ�ˣ����ټ����Ȱ�
    if(WAIT_TIMEOUT == dwErr)
    {
        // ȷ�Ͽͻ����Ƿ񻹻���...
        if(!IsSocketAlive(pContext->m_Socket))
        {
            this->ShowMessage(_T("��⵽�ͻ����쳣�˳���"));
            this->RemoveContext(pContext);
            return true;
        }
        else
        {
            this->ShowMessage(_T("���������ʱ��������..."));
            return true;
        }
    }
    // �����ǿͻ����쳣�˳���
    else if(ERROR_NETNAME_DELETED == dwErr)
    {
        this->ShowMessage(_T("��⵽�ͻ����쳣�˳���"));
        this->RemoveContext(pContext);
        return true;
    }
    else
    {
        this->ShowMessage(_T("��ɶ˿ڲ������ִ����߳��˳���������룺%d"), dwErr);
        return false;
    }
}

PerSocketContext* CIOCPServer::GetClientSocktContext(char* dwAddr, int nPort)
{
    PerSocketContext *pSendSocketContext = NULL;
    EnterCriticalSection(&m_csContextList);
    
    for(int i = 0; i < m_arrayClientContext.GetCount(); i++)
    {
        if(strcmp(inet_ntoa(m_arrayClientContext.GetAt(i)->m_clientAddr.sin_addr), dwAddr) == 0 && ntohs(m_arrayClientContext.GetAt(i)->m_clientAddr.sin_port) == nPort)
        {
            pSendSocketContext = m_arrayClientContext.GetAt(i);
            break;
        }
    }
    
    LeaveCriticalSection(&m_csContextList);
    return pSendSocketContext;
}

/*
BOOL CIOCPServer::Startselfserver(UINT uListenPort)
{
	if (!WinSockInit())
	{
		return FALSE;
	}
	m_ListenSocket = WSASocket(AF_INET , SOCK_STREAM , IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if ( m_ListenSocket == INVALID_SOCKET )
	{
		goto __Error_End;
	}
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = INADDR_ANY;
	service.sin_port = htons(uListenPort);
	if ( bind(m_ListenSocket, (sockaddr*)&service, sizeof(sockaddr_in)) == SOCKET_ERROR )
	{
		goto __Error_End;
	}
	if( listen(m_ListenSocket, SOMAXCONN) == SOCKET_ERROR )
	{
		SetLastErrorMsg(_T("����ʧ�ܣ�"));
		goto __Error_End;
	}
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if ( m_hCompletionPort == NULL )
	{
		SetLastErrorMsg(_T("��ɶ˿ڴ���ʧ�ܣ�"));
		goto __Error_End;
	}
	CreateIoCompletionPort((HANDLE)m_ListenSocket, m_hCompletionPort,(ULONG_PTR)m_ListenSocket, 0);
	m_wsaapi.LoadAllFun(m_ListenSocket);
	::InitializeCriticalSection(&m_getsktpoll);
	SYSTEM_INFO  systeminfo;
	GetSystemInfo(&systeminfo);
	pThreadpool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(pThreadpool, 2);
	SetThreadpoolThreadMaximum(pThreadpool,(int)systeminfo.dwNumberOfProcessors *2);
	InitializeThreadpoolEnvironment(&tcbe);
	SetThreadpoolCallbackPool(&tcbe, pThreadpool);
	pTpcg= CreateThreadpoolCleanupGroup();
	SetThreadpoolCallbackCleanupGroup(&tcbe, pTpcg, NULL);
	pTpWork= CreateThreadpoolWork(allMyWorkCallback,this,&tcbe);
	SubmitThreadpoolWork(pTpWork);
	///��Ͷ��100�ĵ�����
	PostCreateSocket(m_ListenSocket);//����һ��SOCKET��
	return TRUE;
}

BOOL CIOCPServer::PostCreateSocket(SOCKET cListSKT)
{
    SOCKET cClientSKT;
    CSoketpool *cspl=NULL;
    int lierr=0;
    int li=0;
    for (li=0;li<m_ConnFreeMax;li++)
    {
cClientSKT=WSASocket(AF_INET,SOCK_STREAM,IPPROTO_TCP,NULL,0,WSA_FLAG_OVERLAPPED);
        if (INVALID_SOCKET == cClientSKT)
        {
            lierr=WSAGetLastError();
            break;
        }
        CreateIoCompletionPort((HANDLE)cClientSKT,m_hCompletionPort,(ULONG_PTR)cListSKT,0);
        COverLappedEx *m_pOverLap = new COverLappedEx(IO_TYPE_ACCEPT,cClientSKT);
        BOOL bRet = AcceptEx(cListSKT,cClientSKT, m_pOverLap->m_szBuf, 0,
            sizeof(sockaddr_in)+16, sizeof(sockaddr_in)+16, &m_pOverLap->dwBytesRecv, &m_pOverLap->m_OLap);
        if (!bRet)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                AfxMessageBox(_T("111"));
                break;
            }
        }
        InterlockedIncrement(&m_ConnSKTCount);
        InterlockedIncrement(&m_ConnNowCount);
    }
    BOOL lspw=SetProcessWorkingSetSize(GetCurrentProcess(),1024*1024*1024,2*1024*1024*1024);
    return TRUE;
}

VOID CALLBACK allMyWorkCallback(PTP_CALLBACK_INSTANCE Instance,PVOID Parameter,PTP_WORK Work)
{
    //����DLL�����߳�
    CIOCPServer *pThis = (CIOCPServer *)Parameter;
    COverLappedEx *pOverLaps = NULL;
    SOCKET cListenSKT;
    BOOL bIORet=FALSE;
    BOOL bSendt=FALSE;
    DWORD dwTrans = 0;
    char lctype=NULL;
    char revc[2048]={0};
    CString csket;
    char szx[10] = {'\0'};
    PTP_WORK pTpWorks;
    DWORD dwFlags=0;

    InterlockedIncrement(&pThis->m_ThreadNums);
    while (TRUE)
    {
        bIORet = GetQueuedCompletionStatus(pThis->m_hCompletionPort, &dwTrans, (PULONG_PTR)&cListenSKT, (LPOVERLAPPED *)&pOverLaps, INFINITE);
        if (!bIORet) continue;
        if(bIORet && pOverLaps)
        {
            switch (pOverLaps->m_IOType)
            {
                case IO_TYPE_READ:
                    {
                        memset(revc,0,2048);
                        WSABUF wsaBuf = {0};
                        wsaBuf.buf = revc;
                        wsaBuf.len = 2048;

                        WSARecv(pOverLaps->m_ClientSKT, &wsaBuf, 1, &(pOverLaps->dwBytesRecv), &(pOverLaps->dwFlags), NULL, NULL);
                        if ((strcmp(revc,"")==0) || (revc[0]==NULL))
                        {//���յ������ݣ��Ͽ�����
                            pThis->PostCloseSocket(pOverLaps->m_ClientSKT,pOverLaps);
                            pThis->Postaccept(pOverLaps);//�Ͽ����Ӻ�����Ͷ��Ϊ��������
                            continue;
                        }
                        memset(revc,0,2048);
                        pThis->PostRecv(pOverLaps);
                        continue;
                    }
                case IO_TYPE_SEND:
                    {
                        continue;
                    }
                case IO_TYPE_ACCEPT:
                    {    InterlockedDecrement(&pThis->m_ConnSKTCount);

                        pThis->PostRecv(pOverLaps);
                        pTpWorks= CreateThreadpoolWork(allMyWorkCallback,pThis,&pThis->tcbe);// ����һ��������
                        SubmitThreadpoolWork(pTpWorks);
                        if (pThis->m_ConnSKTCount<pThis->m_ConnFreeMax)
                        {
                            pThis->PostCreateSocket(cListenSKT);
                        }
                        continue;
                    }
                case IO_TYPE_CLOSE:
                    {
                        continue;
                    }
                default:
                    {
                        continue;
                    }
            }
        }
        else if (!pOverLaps )
        {
            break;
        }
    }
    InterlockedDecrement(&pThis->m_ThreadNums);
    return ;
}

BOOL CIOCPServer::PostRecv(COverLappedEx *m_pOverLap)
{
    ZeroMemory(&(m_pOverLap->m_OLap), sizeof(OVERLAPPED));
    m_pOverLap->m_IOType=IO_TYPE_READ;
    m_pOverLap->dwBytesRecv=0;
    WSABUF wsaBuf={NULL,0};
    int iRet = WSARecv(m_pOverLap->m_ClientSKT,&wsaBuf, 1, &(m_pOverLap->dwBytesRecv), &(m_pOverLap->dwFlags), &(m_pOverLap->m_OLap), NULL);
    if (iRet != NO_ERROR)
    {
        int lierr=WSAGetLastError() ;
        if (lierr != WSA_IO_PENDING)
        {
            delete m_pOverLap;
            return FALSE;
        }
    }
    return TRUE;
}
*/