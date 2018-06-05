#pragma once
#include "./database/DBTable.h"
#include "./database/Ado.h"
#include "./common/DataDefine.h"

class CDataBaseT
{
public:
	CDataBaseT(void);
	~CDataBaseT(void);
protected:
	// ���Ӷ���
	CAdoConnection* m_pAdoConnect;	
	//CAdoRecordSet m_pAdoRecordSet[5];
	CAdoRecordSet* m_pRecordset;
	const int m_nRdsNumCnt;

	CAdoRecordSet* m_pRecordsetFileDel;

	CString m_strSQL;
	CString m_strDBName;
public:
	// ��ʼ�����ݿ�
	BOOL InitDB(CString strConnect, CString strpath);
	void CompactDB(CString strsource);

	// �������ݿ�����
	CAdoConnection* GetDBConnection(void);
	CAdoRecordSet* GetDBRecordSet(void);

	BOOL ExecSql(LPCSTR SqlStr, int& nResult);
	BOOL OpenSql(LPCTSTR TblName, LPCTSTR FldList = "*", LPCTSTR Constr = "", LPCSTR OrderBy = "", int nItemLimited = 0);

	// �����ݿ�������µ�����
	BOOL AddNewData(CString DoorId, int forwardPass, int backPass, int Warning, CString Sdomain, COleDateTime  &pCurrentTime);
	// ��ָ���ı�
	void OpenRecordSet(DBTable dbTblSwitch, char* pTblInfo = NULL);
	BOOL AddDataToCheckInfo(const CheckInfo &checkinfo);
	BOOL ExecSql(LPCSTR SqlStr);
	BOOL AddNewBeiZhu(CString name, CString remark, CString beizhu);
	BOOL AddNewDev(CString name, CString remark, CString beizhu);
	BOOL DelDevFromDb(CString name);	//�����ݿ���ɾ���豸
	//ɾ�����ݿ��¼
	BOOL DeleteRecord(DBTable dbTblSwitch, CString strWhere);
	BOOL AddOrUpdateCountTb(int doorId, int totalPass, int totalAlarm, int todayPass, int todayAlarm, CString todayDate, int lastPass, int lastAlarm);
};
