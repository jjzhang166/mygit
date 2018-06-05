#pragma once

#define    BUFFER_LITTLE_SIZE        512//����512
#define    BUFFER_SMALL_SIZE         1024//����1k
#define    BUFFER_BIG_SIZE           4096//����4k

class CShowSDData
{
    public:
        CShowSDData(void);
        virtual ~CShowSDData(void);
        CShowSDData(CListCtrl* pL, CImageList* pI);
        
        static CShowSDData* Instance(CListCtrl* pL = NULL, CImageList* pI = NULL);
        
        void ShowSDData(CString IP, CString Name, int TheState, CTime ConnTime, CTime ReTime, CString ReData);
        void DisplaySDData();
        
    private:
        //�洢������ʾ��Ϣ�Ķ���
        typedef struct ShowSdData
        {    
            char IP[30];//ip:Port  ip��ַ�Ͷ˿�
            char Name[BUFFER_LITTLE_SIZE];//��������
            int  TheState;//��ǰ״̬:0����/1�Ͽ�
            CTime ConnTime;//����ʱ��
            CTime ReTime;//�ϴν�������ʱ��
            char ReData[BUFFER_SMALL_SIZE];//�ϴν�������
        } ShowSdcData;

        typedef struct ShowSdDataQueue
        {
            ShowSdcData DataQueue[BUFFER_BIG_SIZE];
            DWORD Head;
            DWORD Tail;
            ShowSdDataQueue()
            {
                Head = 0;
                Tail = 0;
            }
        } ShowSdcDataQueue, *PShowSdcDataQueue;
        
        //��Ա����
        PShowSdcDataQueue m_pShowQueue;//���ݴ洢����
        CListCtrl* m_pListCtrl;//��ʾ�б�
        CImageList* m_pImageList;//ͼ���б�
        
        static CShowSDData* _instance;
        
        CRITICAL_SECTION m_csShowBuf;//��ʾ���ݵ���ʾ�������ٽ���
        //�������ģʽ�����Զ��ͷŵ�����
        class Cleaner
        {
            public:
                Cleaner() {}
                ~Cleaner()
                {
                    if(CShowSDData::Instance())
                    {
                        delete CShowSDData::Instance();
                    }
                }
        };
};

