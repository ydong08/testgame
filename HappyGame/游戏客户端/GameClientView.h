#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "RgnButton.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "ApplyUserList.h"
#include "ClientControl.h"
#include "../��Ϸ������/GameLogic.h"
#ifdef __BANKER___
#include "DlgBank.h"
#endif
//////////////////////////////////////////////////////////////////////////

//���붨��
#define JETTON_RADII				68									//����뾶

//��Ϣ����
#define IDM_PLACE_JETTON			WM_USER+200							//��ס��Ϣ
#define IDM_APPLY_BANKER			WM_USER+201							//������Ϣ
#define IDM_CONTINUE_CARD			WM_USER+202							//��������
#define IDM_AUTO_OPEN_CARD			WM_USER+203							//��ť��ʶ
#define IDM_OPEN_CARD				WM_USER+204		
#define IDM_QIANG_BANKER            WM_USER+205

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��ʷ��¼
#define MAX_SCORE_HISTORY			65									//��ʷ����
#define MAX_FALG_COUNT				26									//��ʶ����

//������Ϣ
struct tagJettonInfo
{
	int								nXPos;								//����λ��
	int								nYPos;								//����λ��
	BYTE							cbJettonIndex;						//��������
};

//�������
enum enOperateResult
{
	enOperateResult_NULL,
	enOperateResult_Win,
	enOperateResult_Lost
};

//��¼��Ϣ
struct tagClientGameRecord
{
	enOperateResult					enOperateShunMen;					//������ʶ
	enOperateResult					enOperateDuiMen;					//������ʶ
	enOperateResult					enOperateDaoMen;					//������ʶ
	bool							bWinShunMen;						//˳��ʤ��
	bool							bWinDuiMen;							//����ʤ��
	bool							bWinDaoMen;							//����ʤ��
};

//������ʾ
enum enDispatchCardTip
{
	enDispatchCardTip_NULL,
	enDispatchCardTip_Continue,											//��������
	enDispatchCardTip_Dispatch											//����ϴ��
};

//�ͻ��˻������·�
struct tagAndroidBet
{
	BYTE							cbJettonArea;						//��������
	LONGLONG						lJettonScore;						//��ע��Ŀ
	WORD							wChairID;							//���λ��
	int								nLeftTime;							//ʣ��ʱ�� (100msΪ��λ)
};

//��������
typedef CWHArray<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//������
class CGameClientDlg;
//////////////////////////////////////////////////////////////////////////

//��Ϸ��ͼ
class CGameClientView : public CGameFrameViewGDI
{
	//������Ϣ
protected:
	LONGLONG						m_lMeMaxScore;						//�����ע
    LONGLONG						m_lAreaLimitScore;					//��������

	//��ע��Ϣ
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1];	//������ע
	LONGLONG						m_lAllJettonScore[AREA_COUNT];	//ȫ����ע

	//λ����Ϣ
protected:
	int								m_nWinFlagsExcursionX;				//ƫ��λ��
	int								m_nWinFlagsExcursionY;				//ƫ��λ��
	int								m_nScoreHead;						//�ɼ�λ��
	CRect							m_rcJiaoL;							//���
	CRect							m_rcJiaoR;							//�ҽ�
	CRect							m_rcHengL;							//���
	CRect							m_rcHengR;							//�Һ�
	CRect							m_rcShunMen;						//˳��
	CRect							m_rcDuiMen;							//����
	CRect							m_rcDaoMen;							//����
	CRect							m_rcBanker;							//ׯ
	int								m_OpenCardIndex;					//�_�����
	int								m_PostCardIndex;					//�l�����
	int								m_PostStartIndex;					//������ʼλ��
	int                             m_Scene_Width;						//��Ļ��
	int                             m_Scene_Height;						//��Ļ��
	int								m_Anim_Index;						//����ͼƬ����
	CFont							m_ScoreFont;
	int								AddWinFalgWidth;					//������Ļ��ȼ����¸���

	//�˿���Ϣ
public:	
    BYTE							m_cbTableCardArray[4][2];			//�����˿�
	bool							m_blMoveFinish;
	WORD							m_bcfirstShowCard;
	BYTE							m_bcShowCount;
	bool							m_blAutoOpenCard;					//���Զ�
	bool							m_bWaitSkip;

	//��ʷ��Ϣ
protected:
	LONGLONG						m_lMeStatisticScore;				//��Ϸ�ɼ�
	tagClientGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];//��Ϸ��¼
	int								m_nRecordFirst;						//��ʼ��¼
	int								m_nRecordLast;						//����¼

	//״̬����
public:
	WORD							m_wMeChairID;						//�ҵ�λ��
	BYTE							m_cbAreaFlash;						//ʤ�����
	LONGLONG						m_lCurrentJetton;					//��ǰ����
	bool							m_bShowChangeBanker;				//�ֻ�ׯ��
	bool							m_bNeedSetGameRecord;				//�������
	bool							m_bWinShunMen;						//ʤ����ʶ
	bool							m_bWinDuiMen;						//ʤ����ʶ
	bool							m_bWinDaoMen;						//ʤ����ʶ
	bool							m_bFlashResult;						//��ʾ���
	bool							m_bShowGameResult;					//��ʾ���
	bool							m_bPlayAnim;						//���Ŷ���
	bool							m_bIsShowScore[AREA_COUNT+1];		//�Լ�������ʾ
	enDispatchCardTip				m_enDispatchCardTip;				//������ʾ

	//ׯ����Ϣ
protected:	
	WORD							m_wBankerUser;						//��ǰׯ��
	WORD							m_wBankerTime;						//��ׯ����
	LONGLONG						m_lBankerScore;						//ׯ�һ���
	LONGLONG						m_lBankerWinScore;					//ׯ�ҳɼ�	
	LONGLONG						m_lTmpBankerWinScore;				//ׯ�ҳɼ�	
	bool							m_bEnableSysBanker;					//ϵͳ��ׯ

	//���ֳɼ�
public:
	LONGLONG						m_lMeCurGameScore;					//�ҵĳɼ�
	LONGLONG						m_lMeCurGameReturnScore;			//�ҵĳɼ�
	LONGLONG						m_lBankerCurGameScore;				//ׯ�ҳɼ�
	LONGLONG						m_lGameRevenue;						//��Ϸ˰��

	//���ױ���
public:
	bool							m_bShowCheatInfo;					//��ʾ����
	BYTE							m_bCheatCard[4][2];					//�����˿�
	int								m_nWinInfo[3];						//��Ӯ���	
	bool							m_bCheckOverBtn;					//�Ƿ�����ע���

	//���ݱ���
protected:
	//CPoint							m_PointJetton[AREA_COUNT+1];		//����λ��
	CPoint							m_PointJettonNumber[AREA_COUNT+2];	//����λ��
	CJettonInfoArray				m_JettonInfoArray[AREA_COUNT];	//��������

	//�ؼ�����
public:
	CRgnButton						m_btJetton100;						//���밴ť
	CRgnButton						m_btJetton500;						//���밴ť
	CRgnButton						m_btJetton1000;						//���밴ť
	CRgnButton						m_btJetton10000;					//���밴ť
	CRgnButton						m_btJetton100000;					//���밴ť
	CRgnButton						m_btJetton500000;					//���밴ť
	CRgnButton						m_btJetton1000000;					//���밴ť
	CRgnButton						m_btJetton5000000;					//���밴ť

	CSkinButton						m_btApplyBanker;					//����ׯ��
	CSkinButton						m_btCancelBanker;					//ȡ��ׯ��
	CSkinButton						m_btScoreMoveL;						//�ƶ��ɼ�
	CSkinButton						m_btScoreMoveR;						//�ƶ��ɼ�
	CSkinButton						m_btAutoOpenCard;					//�Զ�����
	CSkinButton						m_btOpenCard;						//�ֶ�����
	CSkinButton						m_btBank;							//���а�ť
	CSkinButton						m_btContinueCard;					//��������
	CSkinButton						m_btUp;								//��ׯ�б���
	CSkinButton						m_btDown;							//��ׯ�б���
	CSkinButton						m_BtnOverChip;						//�����ע
    CSkinButton                     m_btQiangZhuang;
	CButton							m_btOpenAdmin;
	//�ؼ�����
public:
#ifdef __BANKER___
	CDlgBank						m_DlgBank;							//���пؼ�
	CSkinButton						m_btBankerStorage;					//��ť
	CSkinButton						m_btBankerDraw;						//ȡ�ť
#endif
	CGameLogic						m_GameLogic;	
	CApplyUser						m_ApplyUser;						//�����б�
	CGameRecord						m_GameRecord;						//��¼�б�
	CCardControl					m_CardControl[5];					//�˿˿ؼ�	
	CGameClientDlg					*m_pGameClientDlg;					//����ָ��
					//��Ϸ�߼�

	//����
public:
	HINSTANCE						m_hInst;
	IClientControlDlg*				m_pClientControlDlg;				

	//�������
protected:
	CBitImage						m_ImageViewFill;					//����λͼ
	//CBitImage						m_ImageViewBack;					//����λͼ
	CBitImage						m_ImageWinFlags;					//ʤ����־
	CBitImage						m_ImageLoseFlags;					//ʧ�ܱ�־
	CBitImage						m_ImageJettonView;					//���˳���
	CBitImage						m_ImageMeJetton;					//�Լ�����
	CBitImage						m_ImageScoreNumber;					//����
	CBitImage						m_ImageBKScoreNumber;				//ׯ����
	CBitImage						m_ImageTLScoreNumber;				//������
	CBitImage						m_ImageDispatchCardTip;				//������ʾ
	CBitImage						m_ImageBankerInfo;					//������ʾ
	CBitImage						m_ImageBankerList;					//������ʾ
	//-------------------------------
	CBitImage						m_ImageNoteMain;				    //��¼����ͷ
	CBitImage						m_ImageNoteArr_L;				    //��¼���ͷ
	CBitImage						m_ImageNoteArr_C;				    //��¼�ж�
	CBitImage						m_ImageNoteArr_R;				    //��¼�Ҽ�ͷ
	CBitImage						m_ImageNoteInfo;				    //��¼���
	CBitImage						m_ImageMainTimer;				    //ʱ�����
	CBitImage						m_ImageTimerNumber;				    //ʱ������
	CBitImage						m_ImageBankerOther;				    //����ׯ��ʾ
	CBitImage						m_ImageBankerMe;				    //�Լ�ׯ��ʾ

	CBitImage						m_ImageFrameShunMen;				//˳��
	CBitImage						m_ImageFrameDaoMen;					//����
	CBitImage						m_ImageFrameDuiMen;					//����
	CBitImage						m_ImageFrameJiaoL;					//���
	CBitImage						m_ImageFrameJiaoR;					//�ҽ�
	CBitImage						m_ImageFrameHengL;					//���
	CBitImage						m_ImageFrameHengR;					//�Һ�
	CBitImage						m_ImageCardFlag[19];				//���ͱ�־


	CBitImage						m_ImageSweepAll;					//ͨ��
	CBitImage						m_ImagePassAll;						//ͨ��

	CPngImage						m_PngAnim[64];						//ϴ�ƶ���ͼƬ
	CPngImage						m_PngStarAnim[2][6];				//���Ӷ���ͼƬ
	int								m_nStarPoint[2];					//���ӵ���
	//-------------------------------

	//�߿���Դ
protected:

	CBitImage						m_ImageMeBanker;					//�л�ׯ��
	CBitImage						m_ImageChangeBanker;				//�л�ׯ��
	CBitImage						m_ImageNoBanker;					//�л�ׯ��

	//CPngImage						m_PngFrameCenter;					//�߿�ͼƬ				
	//CPngImage						m_PngFrameAngleUp;					//�߿�ͼƬ
	//CPngImage						m_PngFrameAngleDown;				//�߿�ͼƬ

	//CPngImage						m_PngTimeNum;						//ʱ������

	//������Դ
protected:
	//CSkinImage						m_ImageGameEnd;						//�ɼ�ͼƬ

	//��������
public:
	//���캯��
	CGameClientView();
	//��������
	virtual ~CGameClientView();

	//�̳к���
private:
	//���ý���
	virtual VOID ResetGameView();
	//�����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//�滭����
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);

	//virtual bool RealizeWIN7() { return true; }

	//���ú���
public:
	//������Ϣ
	void SetMeMaxScore(LONGLONG lMeMaxScore);
	//�ҵ�λ��
	void SetMeChairID(DWORD dwMeUserID);
	//������ע
	void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);
	//ׯ����Ϣ
	void SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore);
	//ׯ�ҳɼ�
	void SetBankerScore(WORD wBankerTime, LONGLONG lWinScore) {m_wBankerTime=wBankerTime; m_lTmpBankerWinScore=lWinScore;}
	//���ֳɼ�
	void SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue);
	//���ó���
	void PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount);
	//��������
	void SetAreaLimitScore(LONGLONG lAreaLimitScore);	
	//�����˿�
	void SetCardInfo(BYTE cbTableCardArray[4][2]);
	//���ó���
	void SetCurrentJetton(LONGLONG lCurrentJetton);
	//��ʷ��¼
	void SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen);
	//�ֻ�ׯ��
	void ShowChangeBanker( bool bChangeBanker );
	//�ɼ�����
	void SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore);
	//����ϵͳ��ׯ
	void EnableSysBanker(bool bEnableSysBanker) {m_bEnableSysBanker=bEnableSysBanker;}
	//������ʾ
	void SetDispatchCardTip(enDispatchCardTip DispatchCardTip);

	void SetFirstShowCard(WORD bcCard);
	//������������
	void SetCheatInfo(BYTE bCardData[], int nWinInfo[]);
	//ȡ������
	void ClearAreaFlash() { m_cbAreaFlash = 0xFF; }

	void SetAnimFlag(bool flag);

	//��������
public:	
	//��ʼ����
	void DispatchCard();
	//��������
	void FinishDispatchCard(bool bNotScene);
	//���ñ�ը
	bool SetBombEffect(bool bBombEffect, WORD wAreaIndex);
	//��ֹ����
	void StopAnimal();

	void UpdateGameView( CRect *pRect );

	//��������
public:
	//�����ע
	LONGLONG GetUserMaxJetton(BYTE cbArea = 0xFF);

	//�滭����
protected:
	//��˸�߿�
	void FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC);

	//���溯��
public:
	//�������
	void CleanUserJetton();
	//����ʤ��
	void SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet);
	//��������
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//�滭����
	void DrawMeJettonNumber(CDC *pDC);
	//�滭ׯ��
	void DrawBankerInfo(CDC *pDC,int nWidth,int nHeight);
	//�滭���
	void DrawMeInfo(CDC *pDC,int nWidth,int nHeight);
	//�����˿�
	void DrawCheatCard(int nWidth, int nHeight, CDC *pDC);
	//�������
	void AllowControl(BYTE cbStatus);

	//��������
public:
	//��ǰ����
	inline LONGLONG GetCurrentJetton() { return m_lCurrentJetton; }	
	void SetMoveCardTimer();

	//�ڲ�����
private:
	//��ȡ����
	BYTE GetJettonArea(CPoint MousePoint);
	//�滭����
	void DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore = false,int type = 0);
	//�滭����
	void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);
	//�滭����
	void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat=-1);
	//�滭��ʶ
	void DrawWinFlags(CDC * pDC);
	//��ʾ���
	void ShowGameResult(CDC *pDC, int nWidth, int nHeight);
	//͸���滭
	bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);
	//�ƶ�Ӯ��
	void DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen);

	//��ť��Ϣ
protected:
	//��ׯ��ť
	afx_msg void OnApplyBanker();
	//��ׯ��ť
	afx_msg void OnCancelBanker();
	//�ƶ���ť
	afx_msg void OnScoreMoveL();
	//�ƶ���ť
	afx_msg void OnScoreMoveR();
	//�ֹ�����
	afx_msg void OnOpenCard();
	//�Զ�����
	afx_msg void OnAutoOpenCard();
	//��ҳ��ť
	afx_msg void OnUp();
	//��ҳ��ť
	afx_msg void OnDown();
	//�����ע
	afx_msg void OnOverChip();
	//��ׯ
	afx_msg void OnQiangBanker();
	//bank
	afx_msg void OnBank();

	//��Ϣӳ��
protected:
	//��ʱ����Ϣ
	afx_msg void OnTimer(UINT nIDEvent);
	//��������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//�����Ϣ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);	
	//�����Ϣ
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);	
    //�����Ϣ
	afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);
	//�����Ϣ
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//�ؼ�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//�����ؼ����UP��Ϣ
	LRESULT OnViLBtUp(WPARAM wParam, LPARAM lParam);
	//����ȡ��
	afx_msg void OnBankDraw();
	//���д��
	afx_msg void OnBankStorage();
	//����Ա����
	afx_msg void OpenAdminWnd();

	DECLARE_MESSAGE_MAP()	

};

//////////////////////////////////////////////////////////////////////////

#endif
