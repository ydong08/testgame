#pragma once

//��Ϸ���ƻ���
class IClientControlDlg : public CDialog 
{
public:
	IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent){}
	virtual ~IClientControlDlg(void){}

public:
	//���¿���
	virtual void __cdecl OnAllowControl(bool bEnable) = NULL;
	//������
	virtual bool __cdecl ReqResult(const void * pBuffer) = NULL;
	//������ע
	virtual void __cdecl ResetUserBet()=NULL;
	//�����ע
	virtual void __cdecl SetUserBetScore(BYTE cbArea,LONGLONG lScore)=NULL;
	//��ȡ�ǳ�
	virtual void __cdecl GetUserNickName(CString &strNickName)=NULL;
	//���1
	virtual void __cdecl SetUserNickName(LPCTSTR szNickName)=NULL;
	virtual void __cdecl ResetUserNickName()=NULL;

	//�����ע
	virtual void __cdecl SetUserGameScore(BYTE cbArea,LONGLONG lScore)=NULL;
	//���¿��
	virtual void __cdecl UpDateSystemStorage(CMD_S_SystemStorage * pStorage)=NULL;
};
