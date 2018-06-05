#pragma once

#include "afxcmn.h"
#include "afxwin.h"

class IniHelper
{
public:
	static IniHelper* Instance();
	virtual ~IniHelper(void);

	void SetFileName(LPCTSTR szFileName);

	// Operations     
	BOOL SetProfileInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, int nKeyValue);
	BOOL SetProfileString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszKeyValue);

	DWORD GetProfileSectionNames(CStringArray& strArray); // ����section����     

	int GetProfileInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName);
	DWORD GetProfileString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, CString& szKeyValue);

	BOOL DeleteSection(LPCTSTR lpszSectionName);
	BOOL DeleteKey(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName);

	CString GetIniStringValue(CString strSection, CString strSectionKey);
	CString GetIniStringValue(CString strSection, CString strSectionKey, CString strFileName);
	void SetIniStringValue(CString strSection, CString strSectionKey, CString strSectionKeyVal);
	void SetIniStringValue(CString strSection, CString strSectionKey, CString strSectionKeyVal, CString strFileName);
private:
	IniHelper(void);
	IniHelper(LPCTSTR szFileName);


	void Init();
	CString GetAppPath();
	
	CString  m_szFileName; // .//Config.ini, ������ļ������ڣ���exe��һ����ͼWriteʱ���������ļ�     

	UINT m_unMaxSection; // ���֧�ֵ�section��(256)     
	UINT m_unSectionNameMaxSize; // section���Ƴ��ȣ�������Ϊ32(Null-terminated)     

	static IniHelper* m_instance;
};
