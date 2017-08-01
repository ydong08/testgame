#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

#include "DirectSound.h"
#include "afxtempl.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ�Ի���
class CGameClientDlg : public CGameFrameEngine
{
	//��Ԫ����
	friend class CGameClientView;

	//������Ϣ
protected:
	LONGLONG						m_lMeMaxScore;						//�����ע
	LONGLONG						m_lAreaLimitScore;					//��������
	LONGLONG						m_lApplyBankerCondition;			//��������
	LONGLONG                        m_lQiangCondition;
	LONGLONG                        m_lQiangScore;

	//������ע
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT];	//������ע

	//ׯ����Ϣ
protected:
	LONGLONG						m_lBankerScore;						//ׯ�һ���
	WORD							m_wCurrentBanker;					//��ǰׯ��
	BYTE							m_cbLeftCardCount;					//�˿���Ŀ
	bool							m_bEnableSysBanker;					//ϵͳ��ׯ
	int								m_nNotScoreCnt;						//û��ע����

	//״̬����
protected:
	bool							m_bMeApplyBanker;					//�����ʶ
	bool                            m_bMeQiangBanker;

	//��������
protected:
	CList<tagAndroidBet,tagAndroidBet>		m_ListAndroid;				//��������ע����

	//�ؼ�����
protected:
	CGameLogic						m_GameLogic;						//��Ϸ�߼�
	CGameClientView					m_GameClientView;					//��Ϸ��ͼ

	//������Դ
protected:
	CDirectSound					m_DTSDBackground;					//��������
	CDirectSound					m_DTSDCheer[3];						//��������

	//��������
public:
	//���캯��
	CGameClientDlg();
	//��������
	virtual ~CGameClientDlg();

	//����̳�
private:
	//��ʼ����
	virtual bool OnInitGameEngine();
	//���ÿ��
	virtual bool OnResetGameEngine();
	//��Ϸ����
	virtual void OnGameOptionSet();
	//ʱ��ɾ��
	virtual bool OnEventGameClockKill(WORD wChairID);
	//ʱ����Ϣ
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);
	//�Թ�״̬
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//������Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);
	virtual bool AllowBackGroundSound(bool bAllowSound);

	bool OnFrameMessage(WORD wSubCmdID,  void * pBuffer, WORD wDataSize);
	//��Ϣ����
protected:
	//��Ϸ��ʼ
	bool OnSubGameStart( void * pBuffer, WORD wDataSize);
	//��Ϸ����
	bool OnSubGameFree( void * pBuffer, WORD wDataSize);
	//�û���ע
	bool OnSubPlaceJetton( void * pBuffer, WORD wDataSize,bool bGameMes);
	//��Ϸ����
	bool OnSubGameEnd( void * pBuffer, WORD wDataSize);
	//������ׯ
	bool OnSubUserApplyBanker( void * pBuffer, WORD wDataSize);
	//��ׯ
	bool OnSubQiangBanker( void * pBuffer, WORD wDataSize);
	//ȡ����ׯ
	bool OnSubUserCancelBanker( void * pBuffer, WORD wDataSize);
	//�л�ׯ��
	bool OnSubChangeBanker( void * pBuffer, WORD wDataSize);
	//��Ϸ��¼
	bool OnSubGameRecord( void * pBuffer, WORD wDataSize);
	//��עʧ��
	bool OnSubPlaceJettonFail( void * pBuffer, WORD wDataSize);
	//�������
	bool OnSubCheat( void * pBuffer, WORD wDataSize);
	//������
	bool OnSubReqResult(const void * pBuffer, WORD wDataSize);
	bool OnSubScoreResult(void * pBuffer, WORD wDataSize);
	bool OnSubAccountResult(void * pBuffer, WORD wDataSize);
	//��ֵ����
protected:
	//����ׯ��
	void SetBankerInfo(WORD wBanker,LONGLONG lScore);
	//������ע
	void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);
public:
	void SendUserMessage(LPCTSTR str,COLORREF color);
	//���ܺ���
protected:
	//���¿���
	void UpdateButtonContron();
	//�������пؼ�
	void ReSetGameCtr();

	//��Ϣӳ��
protected:
	//��ע��Ϣ
	LRESULT OnPlaceJetton(WPARAM wParam, LPARAM lParam);
	//������Ϣ
	LRESULT OnApplyBanker(WPARAM wParam, LPARAM lParam);
	//��������
	LRESULT OnContinueCard(WPARAM wParam, LPARAM lParam);
	//�ֹ�����
	LRESULT  OnOpenCard(WPARAM wParam, LPARAM lParam);
	//�Զ�����
	LRESULT  OnAutoOpenCard(WPARAM wParam, LPARAM lParam);
	//��ׯ
	LRESULT  OnQiangBanker(WPARAM wParam,LPARAM lParam);
	//����Ա����
	LRESULT OnAdminCommand(WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetAccount(WPARAM wParam, LPARAM lParam);
	LRESULT	 OnCheckAccount(WPARAM wParam, LPARAM lParam);
	LRESULT  OnResetStorage(WPARAM wParm,LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTimer(UINT nIDEvent);
};

//////////////////////////////////////////////////////////////////////////
