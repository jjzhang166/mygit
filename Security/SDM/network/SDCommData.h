#pragma once

//�Զ�����Ϣ
#define WM_DEV_INFO_RECEIVED  (WM_USER + 0x040)		//�豸��Ϣˢ����Ϣ
#define WM_ALARM_DATA_RECEIVED  (WM_USER + 0x041)	//��������ˢ����Ϣ
#define WM_PARAM_DATA_RECEIVED  (WM_USER + 0x042)	//��������ˢ����Ϣ

class CSDCommData
{
    public:
        CSDCommData(void);
        ~CSDCommData(void);
        
        ClientInfo* GetCurClientInfo(int doorID);
        ClientInfo* GetCurClientInfo(BYTE doorID);	//���ݰ����ű�Ż�ȡ��Ӧ�Ŀͻ�����Ϣ
		ClientInfo* GetCurClientInfo(char *ip);
        bool AddClient(ClientInfo* c);	//��ӿͻ���
        bool DeleteClient(char* IpAdress, int nPort);
        bool UpdateUI();
        void InitUI(CDialog* p);
        void RecvData(ClientInfo* sender, char* message);
        void LoginOutInform();
        void SetCurCtrlWnd(HWND hCtrlWnd);	//���õ�ǰ����ڵľ��
        void RecoverPreCtrlWnd();			//�Ӵ��ڹرպ�ָ��������ھ��
        vector<ClientInfo*> GetAllClients();//��ȡ���еĿͻ���
        
        enum Command
        {
            M0 = 0x0,		//ͬ������
            M10 = 0x0A,		//�ޱ�������ͨ��
            M11 = 0x0B,		//ͨ���ұ���
            M12 = 0x0C,		//�����Ų���
            M16 = 0x10,		//�����Źػ�
        };
        
    private:
        bool AnalysisData(ClientInfo* sender, char* message);
        bool SendToAllClient(ClientInfo* sender, char* message);
        void SendToClient(ClientInfo* c);
        void ClearVec();
        
        vector<ClientInfo*> m_CliectVec;
        CRITICAL_SECTION  m_CliectCs;
        CRITICAL_SECTION  m_RecvCs;
        CDialog* m_pUI;
        
        HWND m_curCtrlWnd;
        HWND m_preCtrlWnd;
        
        int m_CountIndex;
};

