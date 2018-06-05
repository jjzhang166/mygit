#ifndef _DATADEFINE_H_
#define _DATADEFINE_H_

enum UserAuthority
{
	uaSuperAdministrator=1,       //��������Ա
	uaCommonlyAdministrator=2,    //һ�����Ա
	uaSysMaintenancePersonnel=3,  //ϵͳά����Ա
	uaOrdinaryOperator=4,          //��ͨ����Ա
	uaNone=5
};

typedef struct _tagUser
{
	int id;                          //�û�ID
	CString username;                //�û���
	CString pwd;                     //����
	int     roleID;                  //Ȩ��ID
	UserAuthority userAuthority;     //Ȩ��
	CString       des;               //������Ϣ

	_tagUser()
	{
		id=0;
		username="";
		pwd="";
		roleID=4;
		userAuthority=uaNone;
		des="";
	}
}User;

enum AutoJudged
{
	ajNormal=0,
	ajContraband=1,
	ajDangerous=2,
	ajCustom=3,
	ajDifficultToPenetrate=4
};

typedef struct _tagCheckInfo
{
	CString   Id;                   //ID, by automatic generation system
	CString   SDoorID;			    //��ID
	CString   checkTime;            //���ʱ��
	CString   autoJudged;           //�����
	CString   userName;             //�˿�����
	CString   userID;               //�˿����֤ID
	CString   captureImageName;     //ץ��ͼ������
	CString   existHisRecord;       //�Ƿ������ʷ��¼
	CString   checkOperator;        //������Ա
	CString   describe;             //�˿�������Ϣ
	CString   BlackWhiteList ;      //�ڰ�����

} CheckInfo;

//��ʾ��Ϣ����
enum MsgBoxType
{
	eInformation = 0,     //��Ϣ
	eQuestion    = 1,     //����
	eWarning     = 2,     //����
	eError       =3       //����
};

//BMPͼ����Ϣ
typedef struct _tagBMPImageInfo
{
	BYTE  *pBmpData;   //BMPͼ�������
	DWORD dSize;       //��С
	int   nWidth;      //���
	int   mHeight;     //�߶�
} BMPImageInfo;

struct TModuleData
{
	WORD Code;//ģ����
	WORD StartTime;
	WORD Day;//ʹ������
	WORD Custom;//ģ���Զ�����Ϣ
};

#endif