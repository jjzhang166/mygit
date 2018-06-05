/*####################################################################
	Filename: 	ado.cpp
	----------------------------------------------------
	Remarks:	...
	----------------------------------------------------

  ####################################################################*/
#include "stdafx.h"
#include "resource.h"
#include <assert.h>

#include <math.h>

#include "Ado.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

#pragma warning (disable:4996)

/*####################################################################
			------------------------------------------------
						CAdoConnection class
			------------------------------------------------
  ####################################################################*/

CAdoConnection::CAdoConnection()
{
    ///���� Connection ����---------------------------
    m_pConnection.CreateInstance("ADODB.Connection");
    assert(m_pConnection != NULL);
}

CAdoConnection::~CAdoConnection()
{
    if(m_pConnection != NULL)
    {
        Release();
    }
}

/*===================================================================
	Name:		���ӵ�����Դ.
	-----------------------------------------------------
	Params:		strConnect: �����ַ���,����������Ϣ.
				lOptions:	��ѡ�������÷����������ӽ���֮��(�첽)��������
						����֮ǰ(ͬ��)���ء�����������ĳ��������
		����					˵��
		adConnectUnspecified	(Ĭ��)ͬ����ʽ�����ӡ�
		adAsyncConnect			�첽��ʽ�����ӡ�Ado�� ConnectComplete ��
							����֪ͨ��ʱ������ӡ�
=====================================================================*/
BOOL CAdoConnection::Connect(LPCTSTR strConnect, long lOptions)
{
    assert(m_pConnection != NULL);
    m_strConnect = strConnect;
    
    try
    {
        // �������ݿ�---------------------------------------------
        return SUCCEEDED(m_pConnection->Open(strConnect, "", "", lOptions));
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( �������ݿⷢ���쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Abnormal connection to the database:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    catch(...)
    {
        //TRACE(_T(":( :"));
        char pTempArray[100] = "An unknown error occurred while connecting to the database\n";
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
    }
    
    return FALSE;
}

/*===================================================================
	Name:	�ر�������Դ������.
	-----------------------------------------------------
	Remarks: ʹ�� Close �����ɹر� Connection �����Ա��ͷ����й�����ϵͳ��
		Դ���رն��󲢷ǽ������ڴ���ɾ�������Ը��������������ò����ڴ˺���
		�δ򿪡�Ҫ��������ڴ�����ȫɾ���������Release������
=====================================================================*/
BOOL CAdoConnection::Disconnect()
{
    try
    {
        if(m_pConnection != NULL && m_pConnection->State == adStateOpen)
        {
            if(SUCCEEDED(m_pConnection->Close()))
            {
                return TRUE;
            }
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( �ر����ݿⷢ���쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Close the database exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

/*===================================================================
	Name:	�ر����Ӳ��ͷŶ���.
	-----------------------------------------------------
	Remarks: �ر����Ӳ��ͷ�connection�������������ϴ���������ȫ���
		��connection����
=====================================================================*/
BOOL CAdoConnection::Release()
{
    try
    {
        Disconnect();
        m_pConnection.Release();
        return TRUE;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( Release���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Release method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return TRUE;
}

/*========================================================================
	Remarks:	�뿼�� CAdoRecordSet �� Cancel ����.
==========================================================================*/
BOOL CAdoConnection::Cancel()
{
    assert(m_pConnection != NULL);
    
    try
    {
        return m_pConnection->Cancel();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( Cancel�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Cancel method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

_ConnectionPtr CAdoConnection::GetConnection()
{
    return m_pConnection;
}

/*===================================================================
	Name:	ȡ��������Ĵ�����Ϣ.
	-----------------------------------------------------
	Remarks: �κ��漰 ADO ����Ĳ��������Բ���һ�������ṩ�ߴ��󡣲�����
		��ʱ,���Խ�һ������ Error �������� Connection ����� Errors ����
		�С����� ADO ������������ʱ������� Errors ���ϣ����ҽ��µ� Error
		�������� Errors �����С�
			ÿ�� Error ��������ض����ṩ�ߴ��󣬶����� ADO ����ADO ����
		������������ʱ���쳣��������С�
			û�в�������� ADO ������ Errors ����û��Ӱ�졣ʹ�� Clear ��ʽ
		���ֹ���� Errors ���ϡ�
			Errors �����е� Error ���󼯺�������Ӧ�������ʱ���������д���
		�о� Errors ������ָ�������ʹ���������̸���ȷ��ȷ�����������ԭ
		�򼰴�����Դ����ִ���ʵ���ԭ���衣
			ĳЩ���Ժͷ�����������Ϊ Errors �����е� Error ������ʾ�ľ��棬
		��������ֹ�����ִ�С��ڵ��� Recordset �����ϵ� Resync��UpdateBatch
		�� CancelBatch ������Connection �� Open ������������ Recordset ����
		�ϵ� Filter ����ǰ������� Errors �����ϵ� Clear �������������Ϳ���
		�Ķ� Errors ���ϵ� Count �����Բ��Է��صľ��档
=====================================================================*/
CString CAdoConnection::GetLastError()
{
    assert(m_pConnection != NULL);
    CString strErrors = "";
    
    try
    {
        if(m_pConnection != NULL)
        {
            ErrorsPtr pErrors = m_pConnection->Errors;
            CString strError;
            
            for(long n = 0; n < pErrors->Count; n++)
            {
                ErrorPtr pError = pErrors->GetItem(n);
                strError.Format(_T("Description: %s\r\n\r\nState: %s, Native: %d, Source: %s\r\n"),
                                (LPCTSTR)pError->Description,
                                (LPCTSTR)pError->SQLState,
                                pError->NativeError,
                                (LPCTSTR)pError->Source);
                strErrors += strError;
            }
        }
        
        return strErrors;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetLastError�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetLastError method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return strErrors;
    }
    
    return strErrors;
}

ErrorsPtr CAdoConnection::GetErrors()
{
    assert(m_pConnection != NULL);
    
    try
    {
        if(m_pConnection != NULL)
        {
            return m_pConnection->Errors;
        }
        
        return NULL;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetLastError�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetLastErrors method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
    
    return NULL;
}

BOOL CAdoConnection::IsOpen()
{
    try
    {
        long dwState = m_pConnection->State;
        return (m_pConnection != NULL && (dwState & adStateOpen));
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( IsOpen���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "IsOpen method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoConnection::SetConnectTimeOut(long lTime)
{
    assert(m_pConnection != NULL);
    
    try
    {
        return SUCCEEDED(m_pConnection->put_ConnectionTimeout(lTime));
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( SetConnectTimeOut���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "SetConnectTimeOut method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}


/*========================================================================
    Name:		������Դ��ȡ���ݿ���Ϣ.
	-----------------------------------------------------
	Params:		queryType  ��Ҫ���е�ģʽ��ѯ����,����Ϊ�������ⳣ��.
	adSchemaAsserts				CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME
	adSchemaCatalogs			CATALOG_NAME
	adSchemaCharacterSets		CHARACTER_SET_CATALOG
								CHARACTER_SET_SCHEMA
								CHARACTER_SET_NAME
	adSchemaCheckConstraints	CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME
	adSchemaCollations			 COLLATION_CATALOG
								COLLATION_SCHEMA
								COLLATION_NAME
	adSchemaColumnDomainUsage	DOMAIN_CATALOG
								DOMAIN_SCHEMA
								DOMAIN_NAME
								COLUMN_NAME
	adSchemaColumnPrivileges	TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME
								GRANTOR
								GRANTEE
	adSchemaColumns				TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME
	adSchemaConstraintColumnUsage TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME
	adSchemaConstraintTableUsage TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
	adSchemaForeignKeys			PK_TABLE_CATALOG
								PK_TABLE_SCHEMA
								PK_TABLE_NAME
								FK_TABLE_CATALOG
								FK_TABLE_SCHEMA
								FK_TABLE_NAME
	adSchemaIndexes				TABLE_CATALOG
								TABLE_SCHEMA
								INDEX_NAME
								TYPE
								TABLE_NAME
	adSchemaKeyColumnUsage		CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME
								TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								COLUMN_NAME
	adSchemaPrimaryKeys			PK_TABLE_CATALOG
								PK_TABLE_SCHEMA
								PK_TABLE_NAME
	adSchemaProcedureColumns	PROCEDURE_CATALOG
								PROCEDURE_SCHEMA
								PROCEDURE_NAME
								COLUMN_NAME
	adSchemaProcedureParameters PROCEDURE_CATALOG
								PROCEDURE_SCHEMA
								PROCEDURE_NAME
								PARAMTER_NAME
	adSchemaProcedures			PROCEDURE_CATALOG
								PROCEDURE_SCHEMA
								PROCEDURE_NAME
								PROCEDURE_TYPE
	adSchemaProviderSpecific	 �μ�˵��
	adSchemaProviderTypes		DATA_TYPE
								BEST_MATCH
	adSchemaReferentialConstraints CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME
	adSchemaSchemata			CATALOG_NAME
								SCHEMA_NAME
								SCHEMA_OWNER
	adSchemaSQLLanguages		<��>
	adSchemaStatistics			TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
	adSchemaTableConstraints	CONSTRAINT_CATALOG
								CONSTRAINT_SCHEMA
								CONSTRAINT_NAME
								TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								CONSTRAINT_TYPE
	adSchemaTablePrivileges		TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								GRANTOR
								GRANTEE
	adSchemaTables				TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
								TABLE_TYPE
	adSchemaTranslations		TRANSLATION_CATALOG
								TRANSLATION_SCHEMA
								TRANSLATION_NAME
	adSchemaUsagePrivileges		OBJECT_CATALOG
								OBJECT_SCHEMA
								OBJECT_NAME
								OBJECT_TYPE
								GRANTOR
								GRANTEE
	adSchemaViewColumnUsage		VIEW_CATALOG
								VIEW_SCHEMA
								VIEW_NAME
	adSchemaViewTableUsage		VIEW_CATALOG
								VIEW_SCHEMA
								VIEW_NAME
	adSchemaViews				TABLE_CATALOG
								TABLE_SCHEMA
								TABLE_NAME
	-----------------------------------------------------
	returns:	���ذ������ݿ���Ϣ�� Recordset ����. Recordset ����ֻ������̬
			�α��.
	-----------------------------------------------------
	Remarks:	OpenSchema��������������Դ�йص���Ϣ��������ڷ������ϵı�
			�Լ����е��е���Ϣ, �������ݽ����ο�, �Ӿ��������Դ���ܻ��в�ͬ��
==========================================================================*/
_RecordsetPtr CAdoConnection::OpenSchema(SchemaEnum queryType)
{
    assert(m_pConnection != NULL);
    
    try
    {
        return m_pConnection->OpenSchema(queryType, vtMissing, vtMissing);
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( ���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "OpenSchema method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
    
    return NULL;
}

/*########################################################################
			  ------------------------------------------------
									������
			  ------------------------------------------------
	BeginTrans		- ��ʼ������
	CommitTrans		- �����κθ��Ĳ�������ǰ������Ҳ��������������
	RollbackTrans	- ȡ����ǰ�������������κθ��Ĳ�����������Ҳ��������
					������

		һ�������� BeginTrans �������ڵ��� CommitTrans �� RollbackTrans ��
	������֮ǰ�����ݿ⽫���������ύ�������κθ��ġ�
		����֧��Ƕ����������ݿ���˵�����Ѵ򿪵������е��� BeginTrans ����
	����ʼ�µ�Ƕ�����񡣷���ֵ��ָʾǶ�ײ�Σ�����ֵΪ 1 ��ʾ�Ѵ򿪶�������
	(�����񲻱���һ��������Ƕ��)������ֵΪ 2 ��ʾ�Ѵ򿪵ڶ�������(Ƕ��
	�ڶ��������е�����)���������ơ����� CommitTrans �� RollbackTrans ֻӰ
	�����´򿪵������ڴ����κθ��߲�����֮ǰ����رջ�ؾ�ǰ����
		���� CommitTrans ���������������ϴ򿪵������������ĸ��Ĳ���������
	���� RollbackTrans������ԭ�������������ĸ��Ĳ�����������δ������
	ʱ���������κ�һ�ַ���������������
		ȡ���� Connection ����� Attributes ���ԣ����� CommitTrans ��
	RollbackTrans �����������Զ�������������� Attributes ��������Ϊ
	adXactCommitRetaining�����ݿ��� CommitTrans ���ú���Զ�����������
	��� Attributes ��������Ϊ adXactAbortRetaining�����ݿ��ڵ���
	RollbackTrans ֮���Զ�����������
  ########################################################################*/
long CAdoConnection::BeginTrans()
{
    assert(m_pConnection != NULL);
    
    try
    {
        return m_pConnection->BeginTrans();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( BeginTrans�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "BeginTrans method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return -1;
    }
    
    return -1;
}

BOOL CAdoConnection::CommitTrans()
{
    assert(m_pConnection != NULL);
    
    try
    {
        return SUCCEEDED(m_pConnection->CommitTrans());
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( CommitTrans�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "CommitTrans method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoConnection::RollbackTrans()
{
    assert(m_pConnection != NULL);
    
    try
    {
        return SUCCEEDED(m_pConnection->RollbackTrans());
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( �����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "RollbackTrans method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}
BOOL CAdoConnection::ClearTbl(char* TblName)
{
    assert(m_pConnection != NULL);
    
    try
    {
        char pSql[100] = "";
        sprintf(pSql, "TRUNCATE TABLE %s", TblName);
        return SUCCEEDED(m_pConnection->Execute(_bstr_t(pSql), NULL, adExecuteNoRecords));
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( �����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "ClearTbl method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}
/*########################################################################
			------------------------------------------------
					 CAdoRecordSet class ���캯��
			------------------------------------------------
  ########################################################################*/

CAdoRecordSet::CAdoRecordSet()
{
    m_pConnection = NULL;
    m_pRecordset.CreateInstance("ADODB.Recordset");
    assert(m_pRecordset != NULL);
}

CAdoRecordSet::CAdoRecordSet(CAdoConnection *pConnection)
{
    m_pConnection = pConnection;
    assert(m_pConnection != NULL);
    m_pRecordset.CreateInstance("ADODB.Recordset");
    assert(m_pRecordset != NULL);
}

CAdoRecordSet::~CAdoRecordSet()
{
    Release();
}

/*========================================================================
	Params:
		- strSQL:		���� Command ����ı�����,SQL ���,����,�洢���̵�
					�û�־� Recordset �ļ���.
		- CursorType:   ��ѡ.CursorTypeEnum ֵ, ȷ����RecordsetʱӦ
					��ʹ�õ��α�����.��Ϊ���г���֮һ.
		����				˵��
		-----------------------------------------------
		adOpenForwardOnly	�򿪽���ǰ�����α�.
		adOpenKeyset		�򿪼��������α�.
		adOpenDynamic		�򿪶�̬�����α�.
		adOpenStatic		�򿪾�̬�����α�.
		-----------------------------------------------
		- LockType:     ��ѡ,ȷ����RecordsetʱӦ��ʹ�õ���������(����)��
					LockTypeEnumֵ,��Ϊ���г���֮һ.
		����				˵��
		-----------------------------------------------
		adLockReadOnly		ֻ�� �� ���ܸı�����.
		adLockPessimistic	����ʽ���� - ͨ��ͨ���ڱ༭ʱ������������Դ�ļ�¼.
		adLockOptimistic	����ʽ���� - ֻ�ڵ��� Update ����ʱ��������¼.
		adLockBatchOptimistic ����ʽ������ - ����������ģʽ(����������ģʽ
						���).
		-----------------------------------------------
		- lOption		��ѡ. ������ֵ, ����ָʾ strSQL ���������Ϳ�Ϊ����
					����֮һ.
		����				˵��
		-------------------------------------------------
		adCmdText			ָʾstrSQLΪ�����ı�, ����ͨ��SQL���.
		adCmdTable			ָʾADO����SQL��ѯ�Ա����strSQL�������ı��з�
						��������.
		adCmdTableDirect	ָʾ�����ĸ�����strSQL�������ı��з���������.
		adCmdStoredProc		ָʾstrSQLΪ�洢����.
		adCmdUnknown		ָʾstrSQL�����е���������Ϊδ֪.
		adCmdFile			ָʾӦ����strSQL���������ļ��лָ�����(�����)
						Recordset.
		adAsyncExecute		ָʾӦ�첽ִ��strSQL.
		adAsyncFetch		ָʾ����ȡ Initial Fetch Size ������ָ���ĳ�ʼ
						������,Ӧ���첽��ȡ����ʣ�����.������������δ��
						ȡ,��Ҫ���߳̽�������ֱ�������¿���.
		adAsyncFetchNonBlocking ָʾ��Ҫ�߳�����ȡ�ڼ��δ����. ���������
						������δ��ȡ,��ǰ���Զ��Ƶ��ļ�ĩβ.
==========================================================================*/
HRESULT CAdoRecordSet::Open(LPCTSTR strSQL, long lOption, CursorTypeEnum CursorType, LockTypeEnum LockType)
{
    assert(m_pConnection != NULL);
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pConnection == NULL)
        {
            return -1;
        }
        
        Close();
        HRESULT  hresult = m_pRecordset->Open(_bstr_t(strSQL), _variant_t((IDispatch*)m_pConnection->GetConnection(), true),
                                              CursorType, LockType, lOption);
                                              
        if(FAILED(hresult))
        {
            return -1;
        }
        
        if(m_pRecordset == NULL)
        {
            return -1;
        }
        
        return (m_pRecordset->adoEOF) ? 0 : 1;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( : %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Open the record set abnormal:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return -1;
    }
}

/*===================================================================
	Name:		�رմ򿪵Ķ����κ���ض���.
	-----------------------------------------------------
	Remarks:	ʹ��Close�����ɹر�Recordset�����Ա��ͷ����й�����ϵͳ��Դ.
		�رն��󲢷ǽ������ڴ���ɾ��, ���Ը��������������ò����ڴ˺��ٴδ�
		��.Ҫ��������ڴ�����ȫɾ��, �ɽ������������Ϊ Nothing.
		    ʹ��Close�����ر�Recordset�����ͬʱ, ���ͷŹ��������ݺͿ�����
		��ͨ�����ض�Recordset��������ݽ��еĶ�������.���ɵ���Open������
		�´򿪾�����ͬ���Ի����޸����Ե�Recordset.��Recordset����رպ�,��
		���κ���Ҫ��α�ķ�������������.
			���������������ģʽ�½��б༭,����Close��������������, Ӧ����
		����Update��CancelUpdat ����.������������ڼ�ر�Recordset����, ��
		���ϴ�UpdateBatch���������������޸Ľ�ȫ����ʧ.
			���ʹ��Clone���������Ѵ򿪵�Recordset����ĸ���,�ر�ԭʼRecordset
		���丱������Ӱ���κ���������.
			Ҫ��������ڴ�����ȫɾ���������Release������
=====================================================================*/
BOOL CAdoRecordSet::Close()
{
    try
    {
        if(m_pRecordset != NULL && m_pRecordset->State != adStateClosed)
        {
            return SUCCEEDED(m_pRecordset->Close());
        }
        
        return TRUE;
    }
    catch(const _com_error& e)
    {
        //TRACE(_T(":( �رռ�¼�������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "An exception occurs off the record set:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return TRUE;
}

/*===================================================================
	Name:	�ر����Ӳ��ͷŶ���.
	-----------------------------------------------------
	Remarks: �ر����Ӳ��ͷ�CAdoRecordSet�������������ϴ���������ȫ���
		��CAdoRecordSet����
=====================================================================*/
BOOL CAdoRecordSet::Release()
{
    try
    {
        if(m_pRecordset != NULL)
        {
            Close();
            m_pRecordset.Release();
        }
        
        return TRUE;
    }
    catch(const _com_error& e)
    {
        //TRACE(_T(":( Release���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Abnormal record set:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return TRUE;
}

/*########################################################################
			  ------------------------------------------------
							   ��¼�����²���
			  ------------------------------------------------
  ########################################################################*/

/*===================================================================
	Remarks:	����µļ�¼.��AddNew()����, Ȼ����PutCollect()����ÿ
				�е�ֵ, �����Update()�������ݿ�.
=====================================================================*/
BOOL CAdoRecordSet::AddNew()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->AddNew());
        }
    }
    catch(_com_error e)
    {
        TRACE(_T(":( AddNew�����쳣: %s\n"), e.ErrorMessage());
        //char pTempArray[100] = "";
        //sprintf(pTempArray, "AddNew�����쳣:%s", e.ErrorMessage());
        //MessageBox(GetTopWindow(NULL), pTempArray, "���ݿ�", MB_OK);
        return FALSE;
    }
    
    return	FALSE;
}

/*===================================================================
	Remarks:	�ڵ���AddNew()�ȷ����󣬵��ô˷�����ɸ��»��޸�.
=====================================================================*/
BOOL CAdoRecordSet::Update()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->Update());
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( Update�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Update Abnormal:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return	FALSE;
}

/*========================================================================
	Name:		ȡ���ڵ��� Update ����ǰ�Ե�ǰ��¼���¼�¼�������κθ���.
	-----------------------------------------------------
	Remarks:	ʹ�� CancelUpdate ������ȡ���Ե�ǰ��¼�������κθ��Ļ�
	��������ӵļ�¼.�ڵ��� Update �������޷������Ե�ǰ��¼���¼�¼
	�����ĸ���,���Ǹ����ǿ����� RollbackTrans �����ؾ�������һ����,
	�����ǿ����� CancelBatch ����ȡ���������µ�һ����.
	����ڵ��� CancelUpdate ����ʱ����¼�¼,����� AddNew ֮ǰ�ĵ�ǰ
	��¼���ٴγ�Ϊ��ǰ��¼.
	�����δ���ĵ�ǰ��¼������¼�¼,���� CancelUpdate ��������������.
==========================================================================*/
BOOL CAdoRecordSet::CancelUpdate()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->CancelUpdate());
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( CancelUpdate�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "CancelUpdate Abnormal:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

/*========================================================================
	Name:		ȡ�������������.
	-----------------------------------------------------
	Params:		AffectRecords   ��ѡ�� AffectEnum ֵ,����CancelBatch
		������Ӱ���¼����Ŀ,��Ϊ���г���֮һ��
		����			˵��
		-------------------------------------------------
		AdAffectCurrent ��ȡ����ǰ��¼�Ĺ������.
		AdAffectGroup	�����㵱ǰ Filter �������õļ�¼ȡ���������.
						ʹ�ø�ѡ��ʱ,���뽫 Filter ��������Ϊ�Ϸ���Ԥ
						���峣��֮һ.
		AdAffectAll		Ĭ��ֵ.ȡ�� Recordset ���������м�¼�Ĺ����
						��,�����ɵ�ǰ Filter �������������ص��κμ�¼.
==========================================================================*/
BOOL CAdoRecordSet::CancelBatch(AffectEnum AffectRecords)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->CancelBatch(AffectRecords));
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( CancelBatch�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "CancelBatch Abnormal:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

/*========================================================================
	Params:		AffectRecords:  AffectEnumֵ, ȷ��Delete������Ӱ��ļ�¼��
		Ŀ,��ֵ���������г���֮һ.
		����				˵��
		-------------------------------------------------
		AdAffectCurrent		Ĭ��.��ɾ����ǰ��¼.
		AdAffectGroup		ɾ�����㵱ǰFilter�������õļ�¼.Ҫʹ�ø�ѡ��,
						���뽫 Filter ��������Ϊ��Ч��Ԥ���峣��֮һ.
		adAffectAll			ɾ�����м�¼.
		adAffectAllChapters ɾ�������Ӽ���¼.
==========================================================================*/
BOOL CAdoRecordSet::Delete(AffectEnum AffectRecords)
{
    assert(m_pRecordset != NULL);
    
    if(!IsOpen())
    {
        return FALSE;
    }
    
    try
    {
        if(SUCCEEDED(m_pRecordset->Delete(AffectRecords)))
        {
            return SUCCEEDED(m_pRecordset->Update());
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( Delete�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Delete method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return	FALSE;
}

/*########################################################################
			  ------------------------------------------------
								��¼����������
			  ------------------------------------------------
    Remarks:
		ʹ��MoveFirst��������ǰ��¼λ���ƶ���Recordse�еĵ�һ����¼.
		ʹ��MoveLast��������ǰ��¼λ���ƶ���Recordset�е����һ����¼.
	Recordset�������֧����ǩ��������ƶ�;������ø÷�������������.

		ʹ��MoveNext��������ǰ��¼��ǰ�ƶ�һ����¼(��Recordset�ĵײ�).���
	���һ����¼�ǵ�ǰ��¼���ҵ���MoveNext����,��ADO����ǰ��¼���õ�
	Recordset(EOFΪTrue)��β��¼֮��.��EOF�����Ѿ�ΪTrueʱ��ͼ��ǰ�ƶ�
	����������.
		ʹ��MovePrevious��������ǰ��¼λ������ƶ�һ����¼(���¼���Ķ���).
	Recordset �������֧����ǩ������α��ƶ�;���򷽷����ý���������.
	����׼�¼�ǵ�ǰ��¼���ҵ���MovePrevious����,��ADO����ǰ��¼������
	Recordset(BOFΪTrue)���׼�¼֮ǰ.��BOF����ΪTrueʱ����ƶ���������
	��. ���Recordset����֧����ǩ������α��ƶ�,��MovePrevious������
	��������.
		�����¼���ǽ���ǰ��,�����û�ϣ��֧����ǰ��������,�����ʹ��
	CacheSize ���Դ�����¼����,ͨ�� Move����֧������α��ƶ�.����
	�����¼�Ǽ��ص��ڴ��е�,����Ӧ���ⲻ��Ҫ�ػ���̫���¼.���Ե���
	����ǰ Recordset ����� MoveFirst ����;��������ʹ�ṩ������ִ����
	�� Recordset ���������.
  ########################################################################*/

BOOL CAdoRecordSet::MoveFirst()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->MoveFirst());
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( MoveFirst�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "MoveFirst method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return	FALSE;
}

BOOL CAdoRecordSet::MoveLast()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->MoveLast());
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( MoveLast�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "MoveLast method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return	FALSE;
}

BOOL CAdoRecordSet::MovePrevious()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->MovePrevious());
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( MovePrevious�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "MovePrevious method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::MoveNext()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->MoveNext());
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( MoveNext�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "MoveNext  method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

/*========================================================================
	Name:		�ƶ� Recordset �����е�ǰ��¼��λ�á�
    ----------------------------------------------------------
	Params:
		- lRecords    �����ų����ͱ��ʽ��ָ����ǰ��¼λ���ƶ��ļ�¼����
		- Start    ��ѡ���ַ���������ͣ����ڼ�����ǩ��Ҳ��Ϊ���� BookmarkEnum ֵ֮һ��
		����				˵��
		adBookmarkCurrent	Ĭ�ϡ��ӵ�ǰ��¼��ʼ��
		adBookmarkFirst		���׼�¼��ʼ��
		AdBookmarkLast		��β��¼��ʼ��
==========================================================================*/
BOOL CAdoRecordSet::Move(long lRecords, long Start)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            return SUCCEEDED(m_pRecordset->Move(lRecords, _variant_t(Start)));
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( Move�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Move  method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return	FALSE;
}

/*########################################################################
			  ------------------------------------------------
								  ��¼������
			  ------------------------------------------------
  ########################################################################*/

/*========================================================================
	Name:		�����п�Ӧ�ö���,˵�������״̬�Ǵ򿪻��ǹر�.��ִ��
	�첽������ Recordset ����,˵����ǰ�Ķ���״̬�����ӡ�ִ�л��ǻ�ȡ.
	-----------------------------------------------------
	returns:	�������г���֮һ�ĳ�����ֵ.
		����				˵��
		----------------------------------
		adStateClosed		ָʾ�����ǹرյ�.
		adStateOpen			ָʾ�����Ǵ򿪵�.
		adStateConnecting	ָʾ Recordset ������������.
		adStateExecuting	ָʾ Recordset ��������ִ������.
		adStateFetching		ָʾ Recordset ����������ڱ���ȡ.
	-----------------------------------------------------
	Remarks:		������ʱʹ�� State ����ȷ��ָ������ĵ�ǰ״̬.��
	������ֻ����.Recordset ����� State ���Կ��������ֵ.����,���
	����ִ�����,�����Խ��� adStateOpen �� adStateExecuting �����ֵ.
==========================================================================*/
long CAdoRecordSet::GetState()
{
    assert(m_pRecordset != NULL);
    long lState = 0;
    
    try
    {
        if(SUCCEEDED(m_pRecordset->get_State(&lState)))
        {
            return lState;
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetState�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetState method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
    
    return NULL;
}

/*========================================================================
	Name:		ָʾ�й������»��������������ĵ�ǰ��¼��״̬.
	-----------------------------------------------------
	returns:	��������һ������ RecordStatusEnum ֵ֮��.
		����						˵��
		-------------------------------------------------
		adRecOK						�ɹ��ظ��¼�¼.
		adRecNew					��¼���½���.
		adRecModified				��¼���޸�.
		adRecDeleted				��¼��ɾ��.
		adRecUnmodified				��¼û���޸�.
		adRecInvalid				������ǩ��Ч,��¼û�б���.
		adRecMultipleChanges		����Ӱ������¼,��˼�¼δ������.
		adRecPendingChanges			���ڼ�¼���ù���Ĳ���,���δ������.
		adRecCanceled				���ڲ�����ȡ��,δ�����¼.
		adRecCantRelease			�������м�¼����,û�б����¼�¼.
		adRecConcurrencyViolation	���ڿ���ʽ������ʹ����,��¼δ������.
		adRecIntegrityViolation		�����û�Υ��������Լ��,��¼δ������.
		adRecMaxChangesExceeded		���ڴ��ڹ���������,��¼δ������.
		adRecObjectOpen				������򿪵Ĵ�������ͻ,��¼δ������.
		adRecOutOfMemory			���ڼ�����ڴ治��,��¼δ������.
		adRecPermissionDenied		�����û�û���㹻��Ȩ��,��¼δ������.
		adRecSchemaViolation		���ڼ�¼Υ���������ݿ�Ľṹ,���δ������.
		adRecDBDeleted				��¼�Ѿ�������Դ��ɾ��.
	-----------------------------------------------------
	Remarks:	ʹ�� Status ���Բ鿴���������б��޸ĵļ�¼����Щ���ı�
	����.Ҳ��ʹ�� Status ���Բ鿴��������ʱʧ�ܼ�¼��״̬.����,����
	Recordset ����� Resync��UpdateBatch �� CancelBatch ����,��������
	Recordset ����� Filter ����Ϊ��ǩ����.ʹ�ø�����,�ɼ��ָ����¼
	Ϊ��ʧ�ܲ���������.
==========================================================================*/
long CAdoRecordSet::GetStatus()
{
    assert(m_pRecordset != NULL);
    long lStatus = 0;
    
    try
    {
        if(SUCCEEDED(m_pRecordset->get_Status(&lStatus)))
        {
            return lStatus;
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetStatus�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetStatus method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
    
    return NULL;
}

/*========================================================================
	Name:		��ȡ��ǰ��¼���м�¼��Ŀ
==========================================================================*/
long CAdoRecordSet::GetRecordCount()
{
    assert(m_pRecordset != NULL);
    long lcount;
    
    try
    {
        m_pRecordset->get_RecordCount(&lcount);
        return lcount;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetRecordCount�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetRecordCount method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
}

/*========================================================================
	Name:		��ȡ��ǰ��¼�����ֶ���Ŀ
==========================================================================*/
long CAdoRecordSet::GetFieldsCount()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        return GetFields()->Count;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetFieldsCount�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetFieldsCount method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return -1;
    }
}

/*========================================================================
	Name:		ָ���Ƿ����ڼ�¼��ͷ
==========================================================================*/
BOOL CAdoRecordSet::IsBOF()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        return m_pRecordset->adoBOF;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( IsBOF�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "IsBOF method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

/*========================================================================
	Name:		ָ���Ƿ����ڼ�¼��β
==========================================================================*/
BOOL CAdoRecordSet::IsEOF()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        return m_pRecordset->adoEOF;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( IsEOF�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "IsEOF method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
}

void CAdoRecordSet::SetAdoConnection(CAdoConnection *pConnection)
{
    m_pConnection = pConnection;
}

/*===================================================================
	Remarks:	Recordset ������� Field ������ɵ� Fields ����.ÿ��
	Field �����Ӧ Recordset ���е�һ��.
=====================================================================*/
Fields* CAdoRecordSet::GetFields()
{
    assert(m_pRecordset != NULL);
    FieldsPtr pFields;
    
    try
    {
        if(SUCCEEDED(m_pRecordset->get_Fields(&pFields)))
        {
            return pFields;
        }
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetFields�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetFields method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
    
    return NULL;
}

/*========================================================================
	Remarks:	ȡ��ָ�����ֶε��ֶ���.
==========================================================================*/
CString CAdoRecordSet::GetFieldName(long lIndex)
{
    assert(m_pRecordset != NULL);
    CString strFieldName;
    
    try
    {
        FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
        strFieldName = LPCTSTR(_bstr_t(field->GetName()));
        return strFieldName;
        field->GetType();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetFieldName�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetFieldName method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return strFieldName;
    }
}

/*========================================================================
	returns:	���� Field ����Attributes ����Ϊֻ������ֵ����Ϊ��������һ������ FieldAttributeEnum ֵ�ĺ͡�
	  ����				˵��
	  adFldMayDefer			ָʾ�ֶα��ӳ٣�������ӵ��������¼������Դ����
						�ֶ�ֵ��������ʽ������Щ�ֶ�ʱ�Ž��м�����
	  adFldUpdatable		ָʾ����д����ֶΡ�
	  adFldUnknownUpdatable ָʾ�ṩ���޷�ȷ���Ƿ����д����ֶΡ�
	  adFldFixed			ָʾ���ֶΰ����������ݡ�
	  adFldIsNullable		ָʾ���ֶν��� Null ֵ��
	  adFldMayBeNull		ָʾ���ԴӸ��ֶζ�ȡ Null ֵ��
	  adFldLong				ָʾ���ֶ�Ϊ���������ֶΡ���ָʾ����ʹ�� AppendChunk �� GetChunk ������
	  adFldRowID			ָʾ�ֶΰ����־õ��б�ʶ�����ñ�ʶ���޷���д��
						���ҳ��˶��н��б�ʶ(���¼�š�Ψһ��ʶ����)�ⲻ
						�����������ֵ��
	  adFldRowVersion		ָʾ���ֶΰ����������ٸ��µ�ĳ��ʱ������ڱ�ǡ�
	  adFldCacheDeferred	ָʾ�ṩ�߻������ֶ�ֵ������������Ի���Ķ�ȡ��
    ----------------------------------------------------------
	Remarks:	ȡ�� Field ����һ���������ԡ�
==========================================================================*/
long CAdoRecordSet::GetFieldAttributes(long lIndex)
{
    assert(m_pRecordset != NULL);
    long lAttributes;
    
    try
    {
        FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
        field->get_Attributes(&lAttributes);
        return lAttributes;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetFieldAttributes�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetFieldAttributes method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return -1;
    }
}
/*========================================================================
	Name:		ָʾ Field ����������ĳ��ȡ�
    ----------------------------------------------------------
	returns:	����ĳ���ֶζ���ĳ���(���ֽ���)�ĳ�����ֵ��
    ----------------------------------------------------------
	Remarks:	ʹ�� DefinedSize ���Կ�ȷ�� Field ���������������
==========================================================================*/

long CAdoRecordSet::GetFieldDefineSize(long lIndex)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
        return field->GetDefinedSize();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetDefineSize�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetDefineSize method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return -1;
    }
}

/*========================================================================
	Name:	ȡ���ֶε�ֵ��ʵ�ʳ��ȡ�
    ----------------------------------------------------------
	returns:	���س�����ֵ��ĳЩ�ṩ���������ø������Ա�Ϊ BLOB ����Ԥ��
			�ռ䣬�ڴ������Ĭ��ֵΪ 0��
    ----------------------------------------------------------
	Remarks:	ʹ�� ActualSize ���Կɷ��� Field ����ֵ��ʵ�ʳ���.��������
			�ֶ�,ActualSize ����Ϊֻ������� ADO �޷�ȷ�� Field ����ֵ��ʵ
			�ʳ��ȣ�ActualSize ���Խ����� adUnknown��
				�����·�����ʾ��ActualSize ��  DefinedSize ����������ͬ��
			adVarChar ������������󳤶�Ϊ 50 ���ַ��� Field ���󽫷���Ϊ
			50 �� DefinedSize ����ֵ�����Ƿ��ص� ActualSize ����ֵ�ǵ�ǰ��
			¼���ֶ��д洢�����ݵĳ��ȡ�
==========================================================================*/
long CAdoRecordSet::GetFieldActualSize(long lIndex)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
        return field->GetActualSize();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetFieldActualSize�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetFieldActualSize method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return -1;
    }
}

/*========================================================================
	returns:	��������ֵ֮һ����Ӧ�� OLE DB ���ͱ�ʶ�����±��˵�����������и�����
	  ����				˵��
	  ---------------------------------------------------------
	  adArray			����������һ������߼� OR ��ָʾ���������������͵�
					��ȫ���� (DBTYPE_ARRAY)��
	  adBigInt			8 �ֽڴ����ŵ����� (DBTYPE_I8)��
	  adBinary			������ֵ (DBTYPE_BYTES)��
	  adBoolean			������ֵ (DBTYPE_BOOL)��
	  adByRef			����������һ������߼� OR ��ָʾ������������������
					�ݵ�ָ�� (DBTYPE_BYREF)��
	  adBSTR			�Կս�β���ַ��� (Unicode) (DBTYPE_BSTR)��
	  adChar			�ַ���ֵ (DBTYPE_STR)��
	  adCurrency		����ֵ (DBTYPE_CY)���������ֵ�С����λ�ù̶���С��
					���Ҳ�����λ���֡���ֵ����Ϊ 8 �ֽڷ�ΧΪ10,000 �Ĵ���
					������ֵ��
	  adDate			����ֵ (DBTYPE_DATE)�����ڰ�˫��������ֵ�����棬��
					��ȫ����ʾ�� 1899 �� 12 �� 30 ��ʼ����������С��������
					һ�쵱�е�Ƭ��ʱ�䡣
	  adDBDate			����ֵ (yyyymmdd) (DBTYPE_DBDATE)��
	  adDBTime			ʱ��ֵ (hhmmss) (DBTYPE_DBTIME)��
	  adDBTimeStamp		ʱ��� (yyyymmddhhmmss �� 10 �ڷ�֮һ��С��)(DBTYPE_DBTIMESTAMP).
	  adDecimal			���й̶����Ⱥͷ�Χ�ľ�ȷ����ֵ (DBTYPE_DECIMAL)��
	  adDouble			˫���ȸ���ֵ (DBTYPE_R8)��
	  adEmpty			δָ��ֵ (DBTYPE_EMPTY)��
	  adError			32 - λ������� (DBTYPE_ERROR)��
	  adGUID			ȫ��Ψһ�ı�ʶ�� (GUID) (DBTYPE_GUID)��
	  adIDispatch		OLE ������ Idispatch �ӿڵ�ָ�� (DBTYPE_IDISPATCH)��
	  adInteger			4 �ֽڵĴ��������� (DBTYPE_I4)��
	  adIUnknown		OLE ������ IUnknown �ӿڵ�ָ�� (DBTYPE_IUNKNOWN)��
	  adLongVarBinary	��������ֵ��
	  adLongVarChar		���ַ���ֵ��
	  adLongVarWChar	�Կս�β�ĳ��ַ���ֵ��
	  adNumeric			���й̶����Ⱥͷ�Χ�ľ�ȷ����ֵ (DBTYPE_NUMERIC)��
	  adSingle			�����ȸ���ֵ (DBTYPE_R4)��
	  adSmallInt		2 �ֽڴ��������� (DBTYPE_I2)��
	  adTinyInt			1 �ֽڴ��������� (DBTYPE_I1)��
	  adUnsignedBigInt	8 �ֽڲ����������� (DBTYPE_UI8)��
	  adUnsignedInt		4 �ֽڲ����������� (DBTYPE_UI4)��
	  adUnsignedSmallInt 2 �ֽڲ����������� (DBTYPE_UI2)��
	  adUnsignedTinyInt 1 �ֽڲ����������� (DBTYPE_UI1)��
	  adUserDefined		�û�����ı��� (DBTYPE_UDT)��
	  adVarBinary		������ֵ��
	  adVarChar			�ַ���ֵ��
	  adVariant			�Զ������� (DBTYPE_VARIANT)��
	  adVector			����������һ������߼� OR �У�ָʾ������ DBVECTOR
					�ṹ(�� OLE DB ����)���ýṹ����Ԫ�صļ�������������
					(DBTYPE_VECTOR) ���ݵ�ָ�롣
	  adVarWChar		�Կս�β�� Unicode �ַ�����
	  adWChar			�Կս�β�� Unicode �ַ��� (DBTYPE_WSTR)��
    ----------------------------------------------------------
	Remarks:	����ָ���е��������͡�
==========================================================================*/
DWORD CAdoRecordSet::GetFieldType(long lIndex)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        FieldPtr field = GetFields()->GetItem(_variant_t(lIndex));
        return field->GetType();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetField�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetField method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
}

DWORD CAdoRecordSet::GetFieldType(LPCTSTR strfield)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        FieldPtr field = GetFields()->GetItem(strfield);
        return field->GetType();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetField�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetField method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
}
/*========================================================================
	Name:	ȡ��ָ���е��ֶζ����ָ��.
==========================================================================*/
FieldPtr CAdoRecordSet::GetField(long lIndex)
{
    try
    {
        return GetFields()->GetItem(_variant_t(lIndex));
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( GetField�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "GetField method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return NULL;
    }
}

const _RecordsetPtr& CAdoRecordSet::GetRecordset()
{
    return m_pRecordset;
}

CString CAdoRecordSet::GetLastError()
{
    assert(m_pConnection != NULL);
    return m_pConnection->GetLastError();
}

/*========================================================================
	Name:		ȡ��ִ�й�����첽 Execute �� Open �����ĵ���.
	-----------------------------------------------------
	Remarks:	ʹ�� Cancel ������ִֹ���첽 Execute �� Open ��������(��ͨ
		�� adAsyncConnect��adAsyncExecute �� adAsyncFetch �������õķ���).
		�������ͼ��ֹ�ķ�����û��ʹ�� adAsyncExecute,�� Cancel ����������
		ʱ����.
			�±���ʾ���ض����Ͷ�����ʹ�� Cancel ����ʱ����ֹ������.
		�������Ϊ		����ֹ�Ը÷�������һ���첽����
		-------------------------------------------------
		Command			Execute
		Connection		Execute �� Open
		Recordset		Open
==========================================================================*/
BOOL CAdoRecordSet::Cancel()
{
    assert(m_pRecordset != NULL);
    
    try
    {
        return m_pRecordset->Cancel();
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( Cancel�����쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Cancel method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

_RecordsetPtr CAdoRecordSet::operator =(_RecordsetPtr &pRecordSet)
{
    Release();
    m_pRecordset = pRecordSet;
    return m_pRecordset;
}

/*########################################################################
			  ------------------------------------------------
								   ��������
			  ------------------------------------------------
  ########################################################################*/

/*========================================================================
	Remarks:	ȡ�õ�ǰ��¼��ָ���ֶε�ֵ��ת�����ַ���
==========================================================================*/
BOOL CAdoRecordSet::GetValueString(CString& strValue, long lFieldIndex)
{
    assert(m_pRecordset != NULL);
    
    if(lFieldIndex < 0 || lFieldIndex >= GetFieldsCount())
    {
        return FALSE;
    }
    
    try
    {
        if(m_pRecordset == NULL || m_pRecordset->State == adStateClosed)
        {
            MessageBox(NULL, "The database may have been disconnected,\r\nPlease reconnect and try again.", "Prompt", MB_ICONINFORMATION);
            return FALSE;
        }
        
        if(m_pRecordset->adoEOF)
        {
            return FALSE;
        }
        
        _variant_t value = m_pRecordset->GetCollect(_variant_t(lFieldIndex));
        strValue = Format(value);
        return TRUE;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( �ֶη���ʧ��: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Field access failure:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetValueString(CString &strValue, CString strFieldName)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset == NULL || m_pRecordset->State == adStateClosed)
        {
            MessageBox(NULL, "The database may have been disconnected,\r\nPlease reconnect and try again.", "Prompt", MB_ICONINFORMATION);
            return FALSE;
        }
        
        if(m_pRecordset->adoEOF)
        {
            return FALSE;
        }
        
        _variant_t value = m_pRecordset->GetCollect(_variant_t(LPCTSTR(strFieldName)));
        strValue = Format(value);
        return TRUE;
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( �ֶη���ʧ��: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Field access failure:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE/**/;
}

BOOL CAdoRecordSet::GetValueInt(int &intValue, CString strFieldName)
{
	assert(m_pRecordset != NULL);

	try
	{
		if (m_pRecordset == NULL || m_pRecordset->State == adStateClosed)
		{
			MessageBox(NULL, "The database may have been disconnected,\r\nPlease reconnect and try again.", "Prompt", MB_ICONINFORMATION);
			return FALSE;
		}

		if (m_pRecordset->adoEOF)
		{
			return FALSE;
		}

		_variant_t value = m_pRecordset->GetCollect(_variant_t(LPCTSTR(strFieldName)));
		intValue = value.intVal;
		return TRUE;
	}
	catch (_com_error e)
	{
		//TRACE(_T(":( �ֶη���ʧ��: %s\n"), e.ErrorMessage());
		char pTempArray[100] = "";
		sprintf(pTempArray, "Field access failure:%s", e.ErrorMessage());
		MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
		return FALSE;
	}

	return FALSE/**/;
}

/*========================================================================
	Remarks:	��_variant_t�ͱ�����ʽ����CString���ַ���.
==========================================================================*/
CString CAdoRecordSet::Format(_variant_t var)
{
    DATE dt;
    COleDateTime da;
    CString strValue;
    
    switch(var.vt)
    {
        case VT_BSTR://�ַ���
        case VT_LPSTR://�ַ���
        case VT_LPWSTR://�ַ���
            strValue = (LPCTSTR)(_bstr_t)var;
            break;
            
        case VT_I2://������
            strValue.Format("%d", var.iVal);
            break;
            
        case VT_UI2://�޷��Ŷ�����
            strValue.Format("%d", var.uiVal);
            break;
            
        case VT_VOID://
        case VT_INT://����
        case VT_I4://����
            strValue.Format("%d", var.intVal);
            break;
            
        case VT_I8://������
            strValue.Format("%d", var.lVal);
            break;
            
        case VT_UINT://�޷�������
        case VT_UI4://�޷�������
            strValue.Format("%d", var.uintVal);
            break;
            
        case VT_UI8://�޷��ų�����
            strValue.Format("%d", var.ulVal);
            break;
            
        case VT_R4://������
            strValue.Format("%.4f", var.fltVal);
            break;
            
        case VT_R8://˫������
            strValue.Format("%.8f", var.dblVal);
            break;
            
        case VT_DECIMAL: //С��
            {
                double val = var.decVal.Lo32;
                val *= (var.decVal.sign == 128) ? -1 : 1;
                val /= pow(double(10), var.decVal.scale);
                strValue.Format("%.4f", val);
            }
            break;
            
        case VT_BOOL://������
            strValue = var.lVal ? "TRUE" : "FALSE";
            break;
            
        case VT_DATE: //������
            dt = var.date;
            da = COleDateTime(dt);
            strValue = da.Format("%Y-%m-%d %H:%M:%S");
            break;
            
        case VT_NULL://NULLֵ
            strValue = "NULL";
            break;
            
        case VT_EMPTY://��
            strValue = "EMPTY";
            break;
            
        case VT_UNKNOWN://δ֪����
        default:
            strValue = "UN_KNOW";
            break;
    }
    
    return strValue;
}


BOOL CAdoRecordSet::SetCacheSize(const long &lCacheSize)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL && !(GetState() & adStateExecuting))
        {
            return SUCCEEDED(m_pRecordset->put_CacheSize(lCacheSize));
        }
    }
    catch(const _com_error& e)
    {
        //TRACE(_T(":( SetCacheSize���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "SetCacheSize method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

/*========================================================================
	Name:	ȷ��ָ���� Recordset �����Ƿ�֧���ض����͵Ĺ��ܡ�
    ----------------------------------------------------------
	Params:	CursorOptions   �����ͣ�����һ���������� CursorOptionEnum ֵ��
	����				˵��
	adAddNew			��ʹ�� AddNew ��������¼�¼��
	adApproxPosition	�ɶ�ȡ������ AbsolutePosition �� AbsolutePage �����ԡ�
	adBookmark			��ʹ�� Bookmark ���Ի�ö��ض���¼�ķ��ʡ�
	adDelete			����ʹ�� Delete ����ɾ����¼��
	AdHoldRecords		���Լ��������¼���߸�����һ������λ�ö������ύ���й���ĸ��ġ�
	AdMovePrevious		��ʹ�� MoveFirst �� MovePrevious �������Լ� Move ��
					GetRows ��������ǰ��¼λ������ƶ�������ʹ����ǩ��
	AdResync			ͨ�� Resync ������ʹ���ڻ��������ݿ��пɼ������ݸ����αꡣ
	AdUpdate			��ʹ�� Update �����޸����е����ݡ�
	AdUpdateBatch		����ʹ��������(UpdateBatch �� CancelBatch ����)�������鴫����ṩ�ߡ�
	AdIndex				����ʹ�� Index ��������������
	AdSeek				����ʹ�� Seek ������λ Recordset �е��С�
    ----------------------------------------------------------
	returns:	���ز�����ֵ��ָʾ�Ƿ�֧�� CursorOptions ��������ʶ�����й��ܡ�
    ----------------------------------------------------------
	Remarks:	ʹ�� Supports ����ȷ�� Recordset ������֧�ֵĹ������͡����
			Recordset ����֧������Ӧ������ CursorOptions �еĹ��ܣ���ô Supports
			�������� True�����򷵻� False��
	ע��   ���� Supports �����ɶԸ����Ĺ��ܷ��� True���������ܱ�֤�ṩ�߿�
	��ʹ���������л����¾���Ч��Supports ����ֻ�����ṩ���Ƿ�֧��ָ���Ĺ���
	(�ٶ�����ĳЩ����)�����磬Supports ��������ָʾ Recordset ����֧�ָ���
	(��ʹ�α���ڶ����ĺϲ�)��������ĳЩ����Ȼ�޷����¡�
==========================================================================*/
BOOL CAdoRecordSet::Supports(CursorOptionEnum CursorOptions)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            BOOL bSupports = m_pRecordset->Supports(CursorOptions);
            return bSupports;
        }
    }
    catch(const _com_error& e)
    {
        //TRACE(_T(":( Supports���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "Supports method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

/*########################################################################
			  ------------------------------------------------
								  ���ݴ�ȡ
			  ------------------------------------------------
  ########################################################################*/

BOOL CAdoRecordSet::PutCollect(long index, const _variant_t &value)
{
    assert(m_pRecordset != NULL);
    assert(index < GetFieldsCount());
    
    try
    {
        if(m_pRecordset != NULL && index < GetFieldsCount())
        {
            m_pRecordset->PutCollect(_variant_t(index), value);
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return	FALSE;
}

BOOL CAdoRecordSet::PutCollect(LPCSTR strFieldName, _variant_t &value)
{
    assert(m_pRecordset != NULL);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            m_pRecordset->put_Collect(_variant_t(strFieldName), value);
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return	FALSE;
}

BOOL CAdoRecordSet::PutCollect(long index, const LPCTSTR &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(!(GetFieldType(index) == adVarChar
            || GetFieldType(index) == adChar
            || GetFieldType(index) == adLongVarChar))
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adVarChar
           || GetFieldType(index) == adChar
           || GetFieldType(index) == adLongVarChar);
    return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const BYTE &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adUnsignedTinyInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adUnsignedTinyInt);
    return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const short &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adSmallInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adSmallInt);
    return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const int &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adInteger)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adInteger);
    return PutCollect(index, _variant_t(long(value)));
}

BOOL CAdoRecordSet::PutCollect(long index, const long &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adBigInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adBigInt);
    return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const float &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adSingle)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adSingle);
    return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(long index, const double &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adDouble)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adDouble);
    return PutCollect(index, _variant_t(value));
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const LPCTSTR &value)
{
    assert(m_pRecordset != NULL);
    DWORD wdfiledty = GetFieldType(strFieldName);
    #ifdef _DEBUG
    
    if(!(wdfiledty == adVarChar
            || wdfiledty == adChar
            || wdfiledty == adLongVarChar
            || wdfiledty == adVarWChar
            || adWChar == wdfiledty))
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(wdfiledty == adVarChar
           || wdfiledty == adChar
           || wdfiledty == adLongVarChar
           || wdfiledty == adVarWChar
           || adWChar == wdfiledty);
//	return PutCollect(strFieldName, _variant_t(value));
    _variant_t var;
    var = value;
    return PutCollect(strFieldName, var);
}
BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, COleDateTime &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != VT_DATE)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == VT_DATE);
//	return PutCollect(strFieldName, _variant_t(value));
    _variant_t var;
    var.vt = VT_DATE;
    var.date = value;
    return PutCollect(strFieldName, var);
}
BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const BYTE &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adUnsignedTinyInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adUnsignedTinyInt);
    //return PutCollect(strFieldName, _variant_t(value));
    _variant_t var;
    var.vt = VT_UI1;
    var.bVal = value;
    return PutCollect(strFieldName, var);
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const short &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adSmallInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adSmallInt);
    //return PutCollect(strFieldName, _variant_t(value));
    _variant_t var;
    var.vt = VT_I2;
    var.iVal = value;
    return PutCollect(strFieldName, var);
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const int &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adInteger)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adInteger);
    //return PutCollect(strFieldName, _variant_t(long(value)));
    _variant_t var;
    var.vt = VT_I2;
    var.iVal = value;
    return PutCollect(strFieldName, var);
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const long &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adBigInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adBigInt);
    //return PutCollect(strFieldName, _variant_t(value));
    _variant_t var;
    var.vt = VT_I4;
    var.lVal = value;
    return PutCollect(strFieldName, var);
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const float &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adSingle)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adSingle);
//	return PutCollect(strFieldName, _variant_t(value));
    _variant_t var;
    var.vt = VT_R4;
    var.fltVal = value;
    return PutCollect(strFieldName, var);
}

BOOL CAdoRecordSet::PutCollect(LPCTSTR strFieldName, const double &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adDouble)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adDouble);
    //return PutCollect(strFieldName, _variant_t(value));
    _variant_t var;
    var.vt = VT_R4;
    var.fltVal = value;
    return PutCollect(strFieldName, var);
}

/*########################################################################
			  ------------------------------------------------
							       ���ݴ�ȡ
			  ------------------------------------------------
  ########################################################################*/

BOOL CAdoRecordSet::GetCollect(long index,  BYTE &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adUnsignedTinyInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adUnsignedTinyInt);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = result.bVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  short &value)
{
    assert(m_pRecordset != NULL);
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adSmallInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adSmallInt);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = result.iVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  int &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adInteger)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adInteger);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = result.intVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  long &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adBigInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adBigInt);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = result.lVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  float &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adSingle)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adSingle);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = result.fltVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  double &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(index) != adDouble)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adDouble);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = result.dblVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  bool &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adBoolean)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adBoolean);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = (bool)result.boolVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}


BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  BYTE &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adUnsignedTinyInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adUnsignedTinyInt);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = result.bVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  short &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adSmallInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adSmallInt);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = result.iVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  int &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adInteger)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adInteger);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = result.intVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  long &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adBigInt)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adBigInt);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = result.lVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  float &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adSingle)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adSingle);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = result.fltVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  double &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != adDouble)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == adDouble);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = result.dblVal;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}

BOOL CAdoRecordSet::GetCollect(long index,  CString &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(!(GetFieldType(index) == adVarChar
            || GetFieldType(index) == adChar
            || GetFieldType(index) == adLongVarChar))
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(index) == adVarChar
           || GetFieldType(index) == adChar
           || GetFieldType(index) == adLongVarChar);
           
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(index));
            value = (LPCTSTR)(_bstr_t)result;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}
BOOL CAdoRecordSet::GetCollect(LPCSTR strFieldName,  COleDateTime &value)
{
    assert(m_pRecordset != NULL);
    #ifdef _DEBUG
    
    if(GetFieldType(strFieldName) != VT_DATE)
    {
        MessageBox(GetTopWindow(NULL), "Does not match the type of data you want to store the fields and variables", "Database", MB_OK);
    }
    
    #endif
    assert(GetFieldType(strFieldName) == VT_DATE);
    
    try
    {
        if(m_pRecordset != NULL)
        {
            _variant_t result = m_pRecordset->GetCollect(_variant_t(strFieldName));
            value = (COleDateTime)result.date;
            return TRUE;
        }
    }
    catch(_com_error e)
    {
        return FALSE;
    }
    
    return FALSE;
}
BOOL CAdoRecordSet::IsOpen()
{
    try
    {
        return (m_pRecordset != NULL && (GetState() & adStateOpen));
    }
    catch(_com_error e)
    {
        //TRACE(_T(":( IsOpen���������쳣: %s\n"), e.ErrorMessage());
        char pTempArray[100] = "";
        sprintf(pTempArray, "IsOpen method exception occurs:%s", e.ErrorMessage());
        MessageBox(GetTopWindow(NULL), pTempArray, "Database", MB_OK);
        return FALSE;
    }
    
    return FALSE;
}

// ɾ����¼
BOOL CAdoRecordSet::DeleteCurrentRecord(void)
{
    if(IsOpen())
    {
        if(!Supports(adDelete))
        {
            return FALSE;
        }
        
        Delete(adAffectCurrent);
        return TRUE;
    }
    
    return FALSE;
}
BOOL CAdoRecordSet::DeleteAllRecord(void)
{
    if(IsOpen())
    {
        if(!Supports(adDelete))
        {
            return FALSE;
        }
        
        //Delete(adAffectAll);
        if(GetRecordCount() < 1)
        {
            return FALSE;
        }
        
        MoveFirst();
        
        while(!IsEOF())
        {
            Delete(adAffectCurrent);
            MoveNext();
        }
        
        return TRUE;
    }
    
    return FALSE;
}
BOOL CAdoRecordSet::ExecuteSql(CString strsql)
{
    assert(m_pConnection != NULL);
    
    if(IsOpen())
    {
        Close();
    }
    
    _variant_t   RecordsAffected;
    m_pRecordset = m_pConnection->GetConnection()->Execute(_bstr_t(strsql), &RecordsAffected, adCmdText);
    
    if(m_pRecordset != NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
// �����ݿ����ѹ��
BOOL CAdoConnection::CompactDB(LPCTSTR strsource, LPCTSTR strdest)
{
    try
    {
        IJetEnginePtr jet(__uuidof(JetEngine));
        jet->CompactDatabase(strsource, strdest);
        //jet->CompactDatabase( "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=d:\\nwind2.mdb;Jet OLEDB:Database Password=test",
        //	"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=d:\\abbc.mdb;Jet OLEDB:Engine Type=4;Jet OLEDB:Database Password=test");
    }
    catch(_com_error &e)
    {
        ::MessageBox(NULL, (LPCTSTR)e.Description(), "", MB_OK) ;
    }
    
    return 0;
}
