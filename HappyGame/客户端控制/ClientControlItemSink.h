#pragma once
#include "../��Ϸ�ͻ���/ClientControl.h"


// CClientControlItemSinkDlg �Ի���

class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
	DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
	bool m_bWinArea[CONTROL_AREA];
	BYTE m_cbControlStyle;
	BYTE m_cbExcuteTimes;
protected:		
	LONGLONG  m_lAllUserBet[AREA_COUNT];				//�û���ע

public:
	CClientControlItemSinkDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CClientControlItemSinkDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADMIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	//���¿���
	virtual void __cdecl OnAllowControl(bool bEnable);
	//������
	virtual bool __cdecl ReqResult(const void * pBuffer);
	//������ע
	virtual void __cdecl ResetUserBet();
	//�����ע
	virtual void __cdecl SetUserBetScore(BYTE cbArea,LONGLONG lScore);
	//��ȡ�ǳ�
	virtual void __cdecl GetUserNickName(CString &strNickName);
	//���1
	virtual void __cdecl SetUserNickName(LPCTSTR szNickName);
	virtual void __cdecl ResetUserNickName();

	//�����ע
	virtual void __cdecl SetUserGameScore(BYTE cbArea,LONGLONG lScore);
	//���¿��
	virtual void __cdecl UpDateSystemStorage(CMD_S_SystemStorage * pStorage);

public:
	//���ý���
	void ReSetAdminWnd();

public:
	afx_msg void  OnReSet();
	afx_msg void  OnRefresh();
	afx_msg void  OnExcute();
	afx_msg void  OnRadioClick();
	afx_msg void OnBnClickedBtGet();
	afx_msg void OnBnClickedBtCheak();
	afx_msg void OnBnClickedResetStorage();
protected:
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedStArea8();
};
