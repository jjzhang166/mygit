/*########################################################################
	Filename: 	ado.h
	��ado��װ�������桿:
		��Ҫ����������ָ��û�м�ʱ�ͷŵ�bug, ɾ��oleinit()��������Ϊ���ò��ס�
		�����˼�¼����ȡ����.

	��ע�����:
		1. ����������ָ��msado15.dll��ȫ·��,һ����"C:\Program Files\Common
		   Files\System\ADO\"Ŀ¼��.
		2. �������Ҫ��ʼ��COM����,���Ե���CoInitialize(0)����ʼ��;
		   ��CoUninitialize()�ͷ�;
		3. ��ʹ�ü�¼������ǰ�����ȵ���CAdoConnection::Connect()�������ݿ�,
		   ���Ӻ�ɸ����CAdoRecordSet����ʹ��, �������ر����ݿ�.
		5. �򿪼�¼��ǰ, �ȵ���CAdoRecordSet::SetAdoConnection()�������Ӷ�
		   ��, �������رռ�¼��.
		6. �ڱ�������в����������ı��뾯��:
		   warning: unary minus operator applied to unsigned type, result
		   still unsigned
		   �������˾������,������StdAfx.h�ļ��м�������һ�д����Խ�ֹ��
		   ����: 	#pragma warning(disable:4146)
  ########################################################################*/
#ifndef _ADO_H_
#define _ADO_H_

#include <atlstr.h>
#include <ATLComTime.h>

#import "c:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF","adoEOF"),rename("BOF","adoBOF")
#import "../DLL/msjro.dll" no_namespace rename("ReplicaTypeEnum",   "_ReplicaTypeEnum")

class CAdoConnection
{
    public:
        CAdoConnection();
        virtual ~CAdoConnection();
        
    public:
        BOOL SetConnectTimeOut(long lTime);
        BOOL IsOpen();
        _ConnectionPtr GetConnection();
        
    public:
        ErrorsPtr GetErrors();
        BOOL Connect(LPCTSTR strConnect, long lOptions = adConnectUnspecified);
        BOOL Disconnect();
        
        BOOL Cancel();
        
        long BeginTrans();
        BOOL RollbackTrans();
        BOOL CommitTrans();
        
        _RecordsetPtr OpenSchema(SchemaEnum queryType);
        CString GetLastError();
        
        BOOL ClearTbl(char* TblName);
        
    protected:
        BOOL Release();
        CString			m_strConnect;
        _ConnectionPtr	m_pConnection;
    public:
        // �����ݿ����ѹ��
        BOOL CompactDB(LPCTSTR strsource, LPCTSTR strdest);
};

/*########################################################################
			------------------------------------------------
						CAdoRecordSet class
			------------------------------------------------
  ########################################################################*/

class CAdoRecordSet
{
    public:
        CAdoRecordSet();
        CAdoRecordSet(CAdoConnection *pConnection);
        virtual ~CAdoRecordSet();
        HRESULT Open(LPCTSTR strSQL, long lOption = adCmdText, CursorTypeEnum CursorType = adOpenStatic, LockTypeEnum LockType = adLockOptimistic);
        void SetAdoConnection(CAdoConnection *pConnection);
        BOOL Close();
        
    public:
        BOOL IsEOF();
        BOOL IsBOF();
        
        long GetState();
        long GetStatus();
        
        long GetRecordCount();
        long GetFieldsCount();
        
        CString GetLastError();
        
        CString GetFieldName(long lIndex);
        DWORD GetFieldType(long lIndex);
        DWORD GetFieldType(LPCTSTR strfield);
        
        long GetFieldAttributes(long lIndex);
        long GetFieldDefineSize(long lIndex);
        long GetFieldActualSize(long lIndex);
        
        Fields* GetFields();
        FieldPtr GetField(long lIndex);
        const _RecordsetPtr& GetRecordset();
        
    public:
        BOOL AddNew();
        BOOL Update();
        BOOL CancelUpdate();
        BOOL CancelBatch(AffectEnum AffectRecords = adAffectAll);
        BOOL Delete(AffectEnum AffectRecords = adAffectCurrent);
        
    public:
        BOOL MoveFirst();
        BOOL MovePrevious();
        BOOL MoveNext();
        BOOL MoveLast();
        BOOL Move(long lRecords, long Start = adBookmarkCurrent);
        
    public:
        BOOL IsOpen();
        BOOL PutCollect(long index, const _variant_t &value);
        BOOL PutCollect(long index, const LPCSTR &value);
        BOOL PutCollect(long index, const double &value);
        BOOL PutCollect(long index, const float &value);
        BOOL PutCollect(long index, const long &value);
        BOOL PutCollect(long index, const int &value);
        BOOL PutCollect(long index, const short &value);
        BOOL PutCollect(long index, const BYTE &value);
        
        BOOL PutCollect(LPCTSTR strFieldName, COleDateTime &value);
        BOOL PutCollect(LPCSTR strFieldName, _variant_t &value);
        BOOL PutCollect(LPCSTR strFieldName, const LPCSTR &value);
        BOOL PutCollect(LPCSTR strFieldName, const double &value);
        BOOL PutCollect(LPCSTR strFieldName, const float &value);
        BOOL PutCollect(LPCSTR strFieldName, const long &value);
        BOOL PutCollect(LPCSTR strFieldName, const int &value);
        BOOL PutCollect(LPCSTR strFieldName, const short &value);
        BOOL PutCollect(LPCSTR strFieldName, const BYTE &value);
        
        BOOL GetCollect(long index, CString &value);
        BOOL GetCollect(long index, double &value);
        BOOL GetCollect(long index, float &value);
        BOOL GetCollect(long index, long &value);
        BOOL GetCollect(long index, int& value);
        BOOL GetCollect(long index, short &value);
        
        BOOL GetCollect(LPCSTR strFieldName, bool &value);
        BOOL GetCollect(LPCSTR strFieldName, BYTE &value);
        
        BOOL GetCollect(LPCSTR strFieldName, double &value);
        BOOL GetCollect(LPCSTR strFieldName, float &value);
        BOOL GetCollect(LPCSTR strFieldName, long &value);
        BOOL GetCollect(LPCSTR strFieldName, int& value);
        BOOL GetCollect(LPCSTR strFieldName, short &value);
        BOOL GetCollect(long index, BYTE &value);
        BOOL GetCollect(LPCSTR strFieldName,  COleDateTime &value);
        
        BOOL Supports(CursorOptionEnum CursorOptions = adAddNew);
        BOOL SetCacheSize(const long& lCacheSize);
        BOOL Cancel();
        BOOL GetValueString(CString &strValue, CString strFieldName);
        BOOL GetValueString(CString &strValue, long lFieldIndex);
		BOOL GetValueInt(int &intValue, CString strFieldName);

        _RecordsetPtr operator =(_RecordsetPtr &pRecordSet);
        CString Format(_variant_t var);
        
    protected:
        BOOL Release();
        
        CAdoConnection     *m_pConnection;
        _RecordsetPtr		m_pRecordset;
        CString				m_strSQL;
    public:
        // ɾ����¼
        BOOL DeleteCurrentRecord(void);
        BOOL DeleteAllRecord(void);
        BOOL ExecuteSql(CString strsql);
};
#endif
