#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"
#include "RandomHelper.h"
#include "StorageControl.h"

enum EnumControlType
{
	ectNone = 0,
	ectWin,
	ectLose,
	ectSuperWin,
	ectSuperLose,
	ectBankerLose
};

struct tagPunishUser
{
	DWORD		dwGameID;
	WORD		wPunishRate;
};



//////////////////////////////////////////////////////////////////////////
//��ʷ��¼
#define MAX_SCORE_HISTORY			65									//��ʷ����
//////////////////////////////////////////////////////////////////////////
#define  __BANKER___

//��Ϸ������
#ifdef __BANKER___
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
#else
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
#endif
{
	//����ע��
protected:
	LONGLONG						m_lAllJettonScore[AREA_COUNT];	//ȫ����ע
	
	//������ע
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT][GAME_PLAYER];//������ע
	LONGLONG                        m_lExitScore;

	//���Ʊ���
protected:
	bool							m_bRefreshCfg;							//ÿ��ˢ��
	TCHAR							m_szRoomName[32];						//���÷���
	TCHAR							m_szConfigFileName[MAX_PATH];			//�����ļ�
	LONGLONG						m_lAreaLimitScore;						//��������
	LONGLONG						m_lUserLimitScore;						//��������

	//��ׯ����
	LONGLONG						m_lApplyBankerCondition;				//��������
	int								m_nBankerTimeLimit;						//��������
	int								m_nBankerTimeAdd;						//���Ӵ��� (���ֶ������������ʱ)
	LONGLONG						m_lExtraBankerScore;					//�������� (���ڴ�ֵʱ������������������ׯ)
	int								m_nExtraBankerTime;						//�������
	int                             m_nQiangTimeAdd;                        //��ׯ�������ӵĴ���
	LONGLONG                        m_lQiangCondition;                      //��ׯ����
	WORD                            m_wQiangBanker;                           //��ׯ��
	bool                            m_bQiangCurrent;

	//������
	LONGLONG						m_lStorageStart;						//�����ֵ
	int								m_nStorageDeduct;						//��ȡ����

	//ʱ�����
	int								m_nFreeTime;							//����ʱ��
	int								m_nPlaceJettonTime;						//��עʱ��
	int								m_nGameEndTime;							//����ʱ��

	//�����˿���
	int								m_nMaxChipRobot;						//�����Ŀ (��ע������)
	int								m_nChipRobotCount;						//����ͳ�� (��ע������)
	LONGLONG						m_lRobotAreaLimit;						//����ͳ�� (������)
	LONGLONG						m_lRobotAreaScore[AREA_COUNT];			//����ͳ�� (������)

	BYTE                            m_bAllWin;

	tagPunishUser				m_PunishUser[128];						//�ͷ��û�
	int							m_nPunishUserCount;						//�ͷ��û�����
	CStorageControl				m_StorageControl;

	//��ҳɼ�
protected:
	LONGLONG						m_lUserWinScore[GAME_PLAYER];			//��ҳɼ�
	LONGLONG						m_lUserReturnScore[GAME_PLAYER];		//������ע
	LONGLONG						m_lUserRevenue[GAME_PLAYER];			//���˰��
	BYTE							m_cbLeftCardCount;						//�˿���Ŀ
	bool							m_bContiueCard;							//��������
	WORD							m_bcFirstPostCard;						//���°l���c��

	//�˿���Ϣ
protected:
	BYTE							m_cbCardCount[4];						//�˿���Ŀ
    BYTE							m_cbTableCardArray[4][2];				//�����˿�
	BYTE							m_cbTableCard[CARD_COUNT];				//�����˿�

	//״̬����
protected:
	DWORD							m_dwJettonTime;							//��ʼʱ��
	bool							m_bExchangeBanker;						//�任ׯ��
	WORD							m_wAddTime;								//������ׯ 

	//ׯ����Ϣ
protected:
	CWHArray<WORD>					m_ApplyUserArray;						//�������
	WORD							m_wCurrentBanker;						//��ǰׯ��
	WORD							m_wDownBanker;							//�ȴ�ͨ����ׯ���
	WORD							m_wBankerTime;							//��ׯ����
	LONGLONG						m_lBankerScore;							//
	LONGLONG						m_lBankerWinScore;						//�ۼƳɼ�
	LONGLONG						m_lBankerCurGameScore;					//��ǰ�ɼ�
	bool							m_bEnableSysBanker;						//ϵͳ��ׯ

	//���Ʊ���
protected:
	BYTE							m_cbWinSideControl;						//������Ӯ
	int								m_nSendCardCount;						//���ʹ���

	//��¼����
protected:
	tagServerGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];	//��Ϸ��¼
	int								m_nRecordFirst;							//��ʼ��¼
	int								m_nRecordLast;							//����¼
	CFile							m_FileRecord;							//��¼���
	DWORD							m_dwRecordCount;						//��¼��Ŀ

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	ITableFrame						* m_pITableFrame;						//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���
	CRandomHelper					m_rand;

	//�������
protected:
	HINSTANCE						m_hInst;
	IServerControl*					m_pServerContro;

	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����
	bool							m_bWinShunMen;						//ʤ����ʶ
	bool							m_bWinDuiMen;						//ʤ����ʶ
	bool							m_bWinDaoMen;						//ʤ����ʶ
					//��ʼģʽ

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID  Release() { }
	//�Ƿ���Ч
	virtual bool  IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//�ӿڲ�ѯ
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool Initialization(IUnknownEx * pIUnknownEx);
	//��λ����
	virtual VOID RepositionSink();

	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore){};
	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem){return 0;}
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize){return false;}
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason){return false;}
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID);
	//��Ϣ�ӿ�
public:
	
	//��Ϸ״̬
	virtual bool  IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool  OnEventGameStart();
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����¼�
	virtual bool OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�û�����
	virtual bool  OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) ;
	//�û�����
	virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem) { return true; }
	//�û�����
	virtual bool  OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool  OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize){ return true; }
#ifdef __SPECIAL___
	//���в���
public:
	virtual bool  OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem);////
	bool  IsAllowBankAction(WORD wChairID, IServerUserItem * pIServerUserItem);
#endif


	//��Ϸ�¼�
protected:
	//��ע�¼�
	bool OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore);
	//����ׯ��
	bool OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem);
	//qiang
	bool OnUserQiangBanker(IServerUserItem *pIserverItem);
	//ȡ������
	bool OnUserCancelBanker(IServerUserItem *pICancelServerUserItem);

	//��������
private:
	//�����˿�
	bool DispatchTableCard();
	//����ׯ��
	void SendApplyUser( IServerUserItem *pServerUserItem );
	//����ׯ��
	bool ChangeBanker(bool bCancelCurrentBanker);
	//�ֻ��ж�
	void TakeTurns();
	//���ͼ�¼
	void SendGameRecord(IServerUserItem *pIServerUserItem);
	//������Ϣ
	void SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg);
	//����Ա����
	bool OnSubAmdinCommand(IServerUserItem*pIServerUserItem,const void*pDataBuffer);
	//��ȡ����
	void ReadConfigInformation(bool bReadFresh);

	//��ȡ���ֿ�������
	EnumControlType GetControlType();

	// ��Ӷ���
	CString AddComma( LONGLONG lScore , bool bPlus = false);
	// ɾ������
	LONGLONG DeleteComma( CString strScore );
	//��Ӯ����
protected:
	//����
	void DispatchCard();

	//��ע����
private:
	//�����ע
	LONGLONG GetUserMaxJetton(WORD wChairID, BYTE cbArea);

	bool IsEndGame();

	//��Ϸͳ��
private:
	//����÷�
    LONGLONG CalculateScore();
	//����÷�
	LONGLONG CalculateScore1();
	//�ƶ�Ӯ��
	void DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen);
	//��ȡ����
	__int64 GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, __int64 lDefault, LPCTSTR lpFileName);
	//��ȡϵͳ�÷�
	LONGLONG GetSystemScore(const BYTE cbOrder[], LONGLONG &lBankerScore);
};

//////////////////////////////////////////////////////////////////////////

#endif
