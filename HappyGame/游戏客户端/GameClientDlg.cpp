 #include "Stdafx.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FREE					99									//����ʱ��
#define IDI_PLACE_JETTON			100									//��עʱ��
#define IDI_DISPATCH_CARD			301									//����ʱ��

#define IDI_ANDROID_BET				1000	

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientDlg, CGameFrameEngine)
	ON_MESSAGE(IDM_PLACE_JETTON,OnPlaceJetton)
	ON_MESSAGE(IDM_APPLY_BANKER, OnApplyBanker)
	ON_MESSAGE(IDM_CONTINUE_CARD, OnContinueCard)
	ON_MESSAGE(IDM_AUTO_OPEN_CARD, OnAutoOpenCard)	
	ON_MESSAGE(IDM_OPEN_CARD,OnOpenCard)
	ON_MESSAGE(IDM_ADMIN_COMMDN,OnAdminCommand)
	ON_MESSAGE(IDM_CHEAK_ACCOUNT, OnCheckAccount)
	ON_MESSAGE(IDM_GET_ACCOUNT,OnGetAccount)
	ON_MESSAGE(IDM_RESET_STORAGE,OnResetStorage)
	ON_MESSAGE(IDM_QIANG_BANKER,OnQiangBanker)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
//���캯��
CGameClientDlg::CGameClientDlg() 
{
	//������Ϣ
	m_lMeMaxScore=0L;			
	m_lAreaLimitScore=0L;		
	m_lApplyBankerCondition=0L;	

	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//ׯ����Ϣ
	m_lBankerScore=0L;
	m_wCurrentBanker=0L;
	m_cbLeftCardCount=0;

	//״̬����
	m_bMeApplyBanker=false;
	m_bMeQiangBanker=false;


	//��������
	m_ListAndroid.RemoveAll();
	m_nNotScoreCnt = 0;
	return;
}

//��������
CGameClientDlg::~CGameClientDlg()
{
}

//��ʼ����
bool CGameClientDlg::OnInitGameEngine()
{
	//���ñ���
	SetWindowText(TEXT("������Ϸ  --  Ver��6.6.1.0"));

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	if(0)
	{
		BYTE cbCardData[2]={1,2};
		for (int i=0; i<CountArray(m_GameClientView.m_CardControl); ++i) 
			m_GameClientView.m_CardControl[i].SetCardData(cbCardData,2);

		m_GameClientView.SetMoveCardTimer();
	}
	//��������
	//VERIFY(m_DTSDBackground.Create(TEXT("BACK_GROUND")));
	//VERIFY(m_DTSDCheer[0].Create(TEXT("CHEER1")));
	//VERIFY(m_DTSDCheer[1].Create(TEXT("CHEER2")));
	//VERIFY(m_DTSDCheer[2].Create(TEXT("CHEER3")));
	//BYTE cbCard1[2]={0x34,0x14};
	//BYTE cbCard2[2]={0x36,0x16};

	//int t1=m_GameLogic.GetCardType(&cbCard1[0],2);
 //   int t2=m_GameLogic.GetCardType(&cbCard2[0],2);

	//int n = m_GameLogic.CompareCard(&cbCard1[0],2,&cbCard2[0],2);

	//CString str;
	//str.Format(TEXT("%d %d %d"),t1,t2,n);
	//AfxMessageBox(str);
	return true;
}

//���ÿ��
	bool CGameClientDlg::OnResetGameEngine()
{
	//������Ϣ
	m_lMeMaxScore=0L;			

	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//ׯ����Ϣ
	m_lBankerScore=0L;
	m_wCurrentBanker=0L;
	m_cbLeftCardCount=0;

	//״̬����
	m_bMeApplyBanker=false;
	m_bMeQiangBanker=false;

	//��������
	m_ListAndroid.RemoveAll();

	return true;
}

//��Ϸ����
void CGameClientDlg::OnGameOptionSet()
{
	return;
}

//ʱ����Ϣ
bool CGameClientDlg::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD nTimerID)
{
	if ((nTimerID==IDI_PLACE_JETTON)&&(nElapse==0))
	{
		//���ù��
		m_GameClientView.SetCurrentJetton(0L);

		//��ֹ��ť
		m_GameClientView.m_btJetton100.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500.EnableWindow(FALSE);			
		m_GameClientView.m_btJetton1000.EnableWindow(FALSE);		
		m_GameClientView.m_btJetton10000.EnableWindow(FALSE);	
		m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);


		//ׯ�Ұ�ť
		m_GameClientView.m_btApplyBanker.EnableWindow( FALSE );
		m_GameClientView.m_btCancelBanker.EnableWindow( FALSE );
	}
	//if (IsEnableSound()) 
	//{
		if (nTimerID==IDI_PLACE_JETTON&&nElapse<=5) 
		{
			PlayGameSound(AfxGetInstanceHandle(),TEXT("TIME_WARIMG"));
		}
	//}

	if(nTimerID==IDI_FREE&&nElapse<=8)
	{
		if(nElapse<=0)
		{
			m_GameClientView.SetAnimFlag(false);
		}
		else if(nElapse==4)
		{
			m_GameClientView.SetAnimFlag(true);
		}
	}

	UpdateButtonContron();

	return true;
}

//�Թ���Ϣ
bool CGameClientDlg::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool CGameClientDlg::OnEventGameMessage(WORD wSubCmdID, VOID * pBuffer, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_FREE:		//��Ϸ����
		{
			return OnSubGameFree(pBuffer,wDataSize);
		}
	case SUB_S_GAME_START:		//��Ϸ��ʼ
		{
			return OnSubGameStart(pBuffer,wDataSize);
		}
	case SUB_S_PLACE_JETTON:	//�û���ע
		{
			return OnSubPlaceJetton(pBuffer,wDataSize,true);
		}
	case SUB_S_APPLY_BANKER:	//������ׯ
		{
			return OnSubUserApplyBanker(pBuffer, wDataSize);
		}
	case SUB_S_QIANG_BANKER:
		{
			return OnSubQiangBanker(pBuffer,wDataSize);
		}
	case SUB_S_CANCEL_BANKER:	//ȡ����ׯ
		{
			return OnSubUserCancelBanker(pBuffer, wDataSize);
		}
	case SUB_S_CHANGE_BANKER:	//�л�ׯ��
		{
			return OnSubChangeBanker(pBuffer, wDataSize);
		}
	case SUB_S_GAME_END:		//��Ϸ����
		{
			return OnSubGameEnd(pBuffer,wDataSize);
		}
	case SUB_S_SEND_RECORD:		//��Ϸ��¼
		{
			return OnSubGameRecord(pBuffer,wDataSize);
		}
	case SUB_S_PLACE_JETTON_FAIL:	//��עʧ��
		{
			return OnSubPlaceJettonFail(pBuffer,wDataSize);
		}
	case SUB_S_APPLY_FAILURE:
		{
             m_bMeApplyBanker=false;
			 m_bMeQiangBanker=false;
			 UpdateButtonContron();
			 return true;
		}
	case SUB_S_CHEAT:			//�������
		{
			return OnSubCheat(pBuffer,wDataSize);
		}
	case SUB_S_AMDIN_COMMAND:
		{
			return OnSubReqResult(pBuffer,wDataSize);
		}
	case SUB_S_SCORE_RESULT:
		{
			return OnSubScoreResult(pBuffer,wDataSize);
		}
	case SUB_S_ACCOUNT_RESULT:
		{
			return OnSubAccountResult(pBuffer,wDataSize);
		}
	case SUB_S_ADMIN_STORAGE:
		{
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight))
			{

				ASSERT(wDataSize==sizeof(CMD_S_SystemStorage));
				if(wDataSize!=sizeof(CMD_S_SystemStorage)) return false;

				if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd() )
				{
					CMD_S_SystemStorage* pResult = (CMD_S_SystemStorage*)pBuffer;
					m_GameClientView.m_pClientControlDlg->UpDateSystemStorage(pResult);
				}
				return true;
			}
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ����
bool CGameClientDlg::OnEventSceneMessage(BYTE cbGameStation, bool bLookonOther, VOID * pBuffer, WORD wDataSize)
{

	switch (cbGameStation)
	{
	case GAME_STATUS_FREE:			//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��Ϣ����
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pBuffer;

			//����ʱ��
			SetGameClock(GetMeChairID(),IDI_FREE,pStatusFree->cbTimeLeave);

            //�����Ϣ
			m_lMeMaxScore=pStatusFree->lUserMaxScore;
			m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
			 IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
			m_GameClientView.SetMeChairID(pMeUserData->GetUserID());

            //ׯ����Ϣ
			SetBankerInfo(pStatusFree->wBankerUser,pStatusFree->lBankerScore);
			m_GameClientView.SetBankerScore(pStatusFree->cbBankerTime,pStatusFree->lBankerWinScore);
			m_bEnableSysBanker=pStatusFree->bEnableSysBanker;
			m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

			//������Ϣ
			m_lApplyBankerCondition=pStatusFree->lApplyBankerCondition;
			m_lAreaLimitScore=pStatusFree->lAreaLimitScore;
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);
			m_lQiangCondition=pStatusFree->lQiangConition;
			m_lQiangScore=pStatusFree->lQiangScore;

			//��������
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_pClientControlDlg)
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

			//����״̬
			SetGameStatus(GAME_STATUS_FREE);
			m_GameClientView.AllowControl(GAME_STATUS_FREE);
			
			//���¿���
			UpdateButtonContron();
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	case GS_PLACE_JETTON:		//��Ϸ״̬
	case GS_GAME_END:		//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusPlay));
			if (wDataSize!=sizeof(CMD_S_StatusPlay)) return false;

			//��Ϣ����
			CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pBuffer;

			//��ע��Ϣ
			for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
			{
				m_GameClientView.PlaceUserJetton(nAreaIndex,pStatusPlay->lAllJettonScore[nAreaIndex]);
				SetMePlaceJetton(nAreaIndex,pStatusPlay->lUserJettonScore[nAreaIndex]);
			}

			//��һ���
			m_lMeMaxScore=pStatusPlay->lUserMaxScore;			
			m_GameClientView.SetMeMaxScore(m_lMeMaxScore);
			 IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
			m_GameClientView.SetMeChairID(pMeUserData->GetUserID());

			//������Ϣ
			m_lApplyBankerCondition=pStatusPlay->lApplyBankerCondition;
			m_lAreaLimitScore=pStatusPlay->lAreaLimitScore;
			m_GameClientView.SetAreaLimitScore(m_lAreaLimitScore);

			m_lQiangCondition=pStatusPlay->lQiangConition;
			m_lQiangScore=pStatusPlay->lQiangScore;

			if (pStatusPlay->cbGameStatus==GS_GAME_END)
			{
				//�˿���Ϣ
				m_GameClientView.SetCardInfo(pStatusPlay->cbTableCardArray);
				m_GameClientView.FinishDispatchCard(false);

				m_GameClientView.m_bShowGameResult = true;
				m_GameClientView.m_blMoveFinish = true;

				for (int i = 0;i<4;i++)
				{
					m_GameClientView.m_CardControl[i].SetCardData(pStatusPlay->cbTableCardArray[i],2);
					m_GameClientView.m_CardControl[i].OnCopyCard();
					m_GameClientView.m_CardControl[i].OnCopyCard();

					m_GameClientView.m_CardControl[i].SetPositively(true);
					m_GameClientView.m_CardControl[i].m_blGameEnd = true;
					m_GameClientView.m_CardControl[i].m_blhideOneCard = false;
					m_GameClientView.m_CardControl[i].SetDisplayFlag(true);
					m_GameClientView.m_CardControl[i].ShowCardControl(true);
				}

				//���óɼ�
				m_GameClientView.SetCurGameScore(pStatusPlay->lEndUserScore,pStatusPlay->lEndUserReturnScore,pStatusPlay->lEndBankerScore,pStatusPlay->lEndRevenue);
			}
			else
			{
				m_GameClientView.SetCardInfo(NULL);

				for (int i = 0;i<5;i++)
				{
					m_GameClientView.m_CardControl[i].m_CardItemArray.SetSize(0);
				}
			}

			//��������

			PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));


			//ׯ����Ϣ
			SetBankerInfo(pStatusPlay->wBankerUser,pStatusPlay->lBankerScore);
			m_GameClientView.SetBankerScore(pStatusPlay->cbBankerTime,pStatusPlay->lBankerWinScore);
			m_bEnableSysBanker=pStatusPlay->bEnableSysBanker;
			m_GameClientView.EnableSysBanker(m_bEnableSysBanker);

			//����
			if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_pClientControlDlg)
				m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

			//����״̬
			SetGameStatus(pStatusPlay->cbGameStatus);

			//����ʱ��
			SetGameClock(GetMeChairID(),pStatusPlay->cbGameStatus==GS_GAME_END?IDI_DISPATCH_CARD:IDI_PLACE_JETTON,pStatusPlay->cbTimeLeave);

			//MyDebug(_T("Client::OnGameSceneMessage [%d %d]"), pStatusPlay->cbGameStatus, pStatusPlay->cbTimeLeave);

			//���°�ť
			UpdateButtonContron();
			m_GameClientView.UpdateGameView(NULL);

			return true;
		}
	}

	return false;
}

//��Ϸ��ʼ
bool CGameClientDlg::OnSubGameStart( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��Ϣ����
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

	//ׯ����Ϣ
	SetBankerInfo(pGameStart->wBankerUser,pGameStart->lBankerScore);

	//�����Ϣ
	m_lMeMaxScore=pGameStart->lUserMaxScore;
	m_GameClientView.SetMeMaxScore(m_lMeMaxScore);

	//����ʱ��
	SetGameClock(GetMeChairID(),IDI_PLACE_JETTON,pGameStart->cbTimeLeave);
	SetTimer(IDI_ANDROID_BET, 100, NULL);

	//����״̬
	SetGameStatus(GS_PLACE_JETTON);
	m_GameClientView.AllowControl(GS_PLACE_JETTON);

	//��������
	m_ListAndroid.RemoveAll();

	//���¿���
	UpdateButtonContron();

	m_GameClientView.m_btBank.EnableWindow(FALSE);

	//������ʾ
	//m_GameClientView.SetDispatchCardTip(pGameStart->bContiueCard ? enDispatchCardTip_Continue : enDispatchCardTip_Dispatch);

	//��������
	//if (IsEnableSound()) 
	{
		PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));
		
	}
	

	if (m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
	{
		m_GameClientView.m_pClientControlDlg->ResetUserBet();
		//m_GameClientView.m_pClientControlDlg->ResetUserNickName();
	}


	return true;
}

//��Ϸ����
bool CGameClientDlg::OnSubGameFree( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameFree));
	if (wDataSize!=sizeof(CMD_S_GameFree)) return false;

	//��Ϣ����
	CMD_S_GameFree * pGameFree=(CMD_S_GameFree *)pBuffer;

	m_GameClientView.m_btBank.EnableWindow(TRUE);


	///*if (m_nNotScoreCnt>=3)
	//{
	//	PostQuitMessage(0);
	//}*/

	//����ʱ��
	SetGameClock(GetMeChairID(),IDI_FREE,pGameFree->cbTimeLeave);

	//����״̬
	SetGameStatus(GAME_STATUS_FREE);

	//����ʱ��
	KillTimer(IDI_ANDROID_BET);
	KillTimer(IDI_DISPATCH_CARD);

	//����״̬
	m_GameClientView.AllowControl(GAME_STATUS_FREE);
  
	//��������
	m_GameClientView.StopAnimal();
	m_GameClientView.SetWinnerSide(false, false, false, false);
	m_GameClientView.CleanUserJetton();
	for (int nAreaIndex=ID_SHUN_MEN; nAreaIndex<=ID_HENG_R; ++nAreaIndex) SetMePlaceJetton(nAreaIndex,0);

	//�����˿�
	for (int i=0; i<CountArray(m_GameClientView.m_CardControl); ++i) m_GameClientView.m_CardControl[i].SetCardData(NULL,0);

	//���¿ؼ�
	UpdateButtonContron();

	//��ɷ���
	m_GameClientView.FinishDispatchCard(true);

	//���³ɼ�
	for (WORD wUserIndex = 0; wUserIndex < MAX_CHAIR; ++wUserIndex)
	{
		IClientUserItem  *pUserData = GetTableUserItem(wUserIndex);

		if ( pUserData == NULL ) continue;
		tagApplyUser ApplyUser ;

		//������Ϣ
		ApplyUser.lUserScore = pUserData->GetUserScore();
		ApplyUser.strUserName = pUserData->GetNickName();
		m_GameClientView.m_ApplyUser.UpdateUser(ApplyUser);

	}

	return true;
}

//�û���ע
bool CGameClientDlg::OnSubPlaceJetton( void * pBuffer, WORD wDataSize,bool bGameMes)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJetton));
	if (wDataSize!=sizeof(CMD_S_PlaceJetton)) return false;

	srand(GetTickCount());

	//��Ϣ����
	CMD_S_PlaceJetton * pPlaceJetton=(CMD_S_PlaceJetton *)pBuffer;
	if (pPlaceJetton->bIsAndroid==FALSE && (m_GameClientView.m_pClientControlDlg->GetSafeHwnd())&&bGameMes)
	{
		m_GameClientView.m_pClientControlDlg->SetUserBetScore(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
	}

	if (GetMeChairID()!=pPlaceJetton->wChairID || IsLookonMode())
	{
		//�Ƿ������
		if (pPlaceJetton->bIsAndroid)
		{
			//����
			static WORD wStFluc = 1;	//�������
			tagAndroidBet androidBet = {};
			androidBet.cbJettonArea = pPlaceJetton->cbJettonArea;
			androidBet.lJettonScore = pPlaceJetton->lJettonScore;
			androidBet.wChairID = pPlaceJetton->wChairID;
			androidBet.nLeftTime = ((rand()+androidBet.wChairID+wStFluc*3)%10+1)*100;
			wStFluc = wStFluc%3 + 1;

			//MyDebug(_T("client::OnSubPlaceJetton ��� %d ���� %d ��ע %I64d ʱ�� %d]"), androidBet.wChairID,
			//	androidBet.cbJettonArea, androidBet.lJettonScore, androidBet.nLeftTime);

			m_ListAndroid.AddTail(androidBet);
		}
		else
		{
			//��ע����
			m_GameClientView.PlaceUserJetton(pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);

			//��������
		//	if (IsEnableSound()) 
			{
				if (pPlaceJetton->wChairID!=GetMeChairID() || IsLookonMode())
				{
					if (pPlaceJetton->lJettonScore==5000000) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
					else PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
					//m_DTSDCheer[rand()%3].Play();
				}
			}
		}
	}
    
	return true;
}

//��Ϸ����
bool CGameClientDlg::OnSubGameEnd( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
	if (wDataSize!=sizeof(CMD_S_GameEnd)) return false;

	//��Ϣ����
	CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

	m_GameClientView.m_blMoveFinish = false;

	//����ʱ��
	SetGameClock(GetMeChairID(),IDI_DISPATCH_CARD, pGameEnd->cbTimeLeave);

	m_GameClientView.m_btBank.EnableWindow(TRUE);

	//�˿���Ϣ
	m_GameClientView.SetCardInfo(pGameEnd->cbTableCardArray);

	m_GameClientView.SetFirstShowCard(pGameEnd->bcFirstCard);

	m_GameClientView.ClearAreaFlash();

	//�����˿�
	for (int i=0; i<4; ++i) m_GameClientView.m_CardControl[i].SetCardData(m_GameClientView.m_cbTableCardArray[i],2);
	m_GameClientView.SetCheatInfo(NULL, NULL);

	//�O�Ó���Ƅ�
	m_GameClientView.SetMoveCardTimer();

	//����״̬
	SetGameStatus(GS_GAME_END);
	m_GameClientView.AllowControl(GS_GAME_END);

	m_cbLeftCardCount=pGameEnd->cbLeftCardCount;

	//ׯ����Ϣ
	m_GameClientView.SetBankerScore(pGameEnd->nBankerTime, pGameEnd->lBankerTotallScore);

	//�ɼ���Ϣ
	m_GameClientView.SetCurGameScore(pGameEnd->lUserScore,pGameEnd->lUserReturnScore,pGameEnd->lBankerScore,pGameEnd->lRevenue);

	if (pGameEnd->lUserScore==0 && pGameEnd->lUserReturnScore==0 && m_wCurrentBanker!=GetMeChairID())
	{
		m_nNotScoreCnt+=1;
	}

	//TCHAR szBuffer[512]=TEXT("");
	//swprintf(szBuffer,CountArray(szBuffer),TEXT("���γɼ����:"));
	//m_pIStringMessage->InsertCustomString(szBuffer,RGB(255,255,255));
	//swprintf(szBuffer,CountArray(szBuffer),TEXT("������%I64d"),pGameEnd->lUserScore);
	//m_pIStringMessage->InsertCustomString(szBuffer,RGB(255,255,255));

	//swprintf(szBuffer,CountArray(szBuffer),TEXT("ׯ�ң�%I64d"),pGameEnd->lBankerScore);
	//m_pIStringMessage->InsertCustomString(szBuffer,RGB(255,255,255));

	for (int i = 0;i<4;i++)
	{
		m_GameClientView.m_CardControl[i].m_blGameEnd = false;
		m_GameClientView.m_CardControl[i].SetPositively(false);
	}
	
	//���¿ؼ�
	UpdateButtonContron();

	//ֹͣ����
	//for (int i=0; i<CountArray(m_DTSDCheer); ++i) m_DTSDCheer[i].Stop();

	return true;
}

//���¿���
void CGameClientDlg::UpdateButtonContron()
{
	//�����ж�
	bool bEnablePlaceJetton=true;
	if (m_bEnableSysBanker==false&&m_wCurrentBanker==INVALID_CHAIR) bEnablePlaceJetton=false;
	if (GetGameStatus()!=GS_PLACE_JETTON) bEnablePlaceJetton=false;
	if (m_wCurrentBanker==GetMeChairID()) bEnablePlaceJetton=false;
	if (IsLookonMode()) bEnablePlaceJetton=false;
	//if (GetUserData(GetMeChairID())->cbUserStatus!=US_PLAY) bEnablePlaceJetton=false;

	if(GetGameStatus()==GS_GAME_END)
	{
		m_GameClientView.m_btOpenCard.EnableWindow(false);
		m_GameClientView.m_btAutoOpenCard.EnableWindow(false);

	}else
	{
		m_GameClientView.m_btOpenCard.EnableWindow(true);
		m_GameClientView.m_btAutoOpenCard.EnableWindow(true);

	}

	//��ע��ť
	if (bEnablePlaceJetton==true && !m_GameClientView.m_bCheckOverBtn)
	{
		//�������
		LONGLONG lCurrentJetton=m_GameClientView.GetCurrentJetton();
		LONGLONG lLeaveScore=m_lMeMaxScore;
		for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lLeaveScore -= m_lUserJettonScore[nAreaIndex];

		//�����ע
		LONGLONG lUserMaxJetton=m_GameClientView.GetUserMaxJetton();

		//���ù��
		if (lCurrentJetton>lUserMaxJetton)
		{
			if (lUserMaxJetton>=5000000L) m_GameClientView.SetCurrentJetton(5000000L);
			else if (lUserMaxJetton>=1000000L) m_GameClientView.SetCurrentJetton(1000000L);
			else if (lUserMaxJetton>=500000L) m_GameClientView.SetCurrentJetton(500000L);
			else if (lUserMaxJetton>=100000L) m_GameClientView.SetCurrentJetton(100000L);
			else if (lUserMaxJetton>=10000L) m_GameClientView.SetCurrentJetton(10000L);
			else if (lUserMaxJetton>=1000L) m_GameClientView.SetCurrentJetton(1000L);
			else if (lUserMaxJetton>=500L) m_GameClientView.SetCurrentJetton(500L);
			else if (lUserMaxJetton>=100L) m_GameClientView.SetCurrentJetton(100L);
			else m_GameClientView.SetCurrentJetton(0L);
		}


		//���ư�ť
		m_GameClientView.m_BtnOverChip.EnableWindow(TRUE);
		m_GameClientView.m_btJetton100.EnableWindow((lUserMaxJetton>10000000)?TRUE:FALSE);
		m_GameClientView.m_btJetton500.EnableWindow((lUserMaxJetton>50000000)?TRUE:FALSE);
		m_GameClientView.m_btJetton1000.EnableWindow((lUserMaxJetton>1000)?TRUE:FALSE);
		m_GameClientView.m_btJetton10000.EnableWindow((lUserMaxJetton>10000)?TRUE:FALSE);
		m_GameClientView.m_btJetton100000.EnableWindow((lUserMaxJetton>100000)?TRUE:FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow((lUserMaxJetton>500000)?TRUE:FALSE);		
		m_GameClientView.m_btJetton1000000.EnableWindow((lUserMaxJetton>1000000)?TRUE:FALSE);		
		m_GameClientView.m_btJetton5000000.EnableWindow((lUserMaxJetton>5000000 && m_lBankerScore>5000000)?TRUE:FALSE);  //Ҫ���޸�Ϊ5KW
	}
	else
	{
		//���ù��
		m_GameClientView.SetCurrentJetton(0L);

		//��ֹ��ť

		m_GameClientView.m_BtnOverChip.EnableWindow(FALSE);
		m_GameClientView.m_btJetton100.EnableWindow(FALSE);	
		m_GameClientView.m_btJetton500.EnableWindow(FALSE);	
		m_GameClientView.m_btJetton1000.EnableWindow(FALSE);		
		m_GameClientView.m_btJetton10000.EnableWindow(FALSE);	
		m_GameClientView.m_btJetton100000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton500000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton1000000.EnableWindow(FALSE);
		m_GameClientView.m_btJetton5000000.EnableWindow(FALSE);
	}

	//ׯ�Ұ�ť
	if (!IsLookonMode())
	{
		//��ȡ��Ϣ
		 IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());

		//���밴ť
		bool bEnableApply=true;
		if (m_wCurrentBanker==GetMeChairID()) bEnableApply=false;
		if (m_bMeApplyBanker) bEnableApply=false;
		if (pMeUserData->GetUserScore()<m_lApplyBankerCondition) bEnableApply=false;
		m_GameClientView.m_btApplyBanker.EnableWindow(bEnableApply?TRUE:FALSE);

		//ȡ����ť
		bool bEnableCancel=true;
		if (m_wCurrentBanker==GetMeChairID() && GetGameStatus()!=GAME_STATUS_FREE) bEnableCancel=false;
		if (m_bMeApplyBanker==false) bEnableCancel=false;
		m_GameClientView.m_btCancelBanker.EnableWindow(bEnableCancel?TRUE:FALSE);
		m_GameClientView.m_btCancelBanker.SetButtonImage(m_wCurrentBanker==GetMeChairID()?IDB_BT_CANCEL_BANKER:IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,false);


		if (m_bMeApplyBanker||m_bMeQiangBanker)
		{
			m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_HIDE);
		}
		else
		{
			m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
			m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
		}

		if(m_bMeQiangBanker)
		{ 
              m_GameClientView.m_btQiangZhuang.EnableWindow(FALSE);
		}
		else
		{
			//m_GameClientView.m_btApplyBanker.EnableWindow(TRUE);
			if(pMeUserData->GetUserScore()>=m_lQiangCondition)
			{
				m_GameClientView.m_btQiangZhuang.EnableWindow(TRUE);
			}
			else
			{
				m_GameClientView.m_btQiangZhuang.EnableWindow(FALSE);
			}
		}
		if(m_wCurrentBanker == GetMeChairID())
		{
			m_GameClientView.m_BtnOverChip.ShowWindow(SW_HIDE);
			//m_GameClientView.m_btCancelBanker.ShowWindow(SW_SHOW);
		}else
		{
			m_GameClientView.m_BtnOverChip.ShowWindow(SW_SHOW);
			//m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
		}
		//m_GameClientView.m_btApplyBanker.EnableWindow(bEnableApply?TRUE:FALSE);


		//���ư�ť
		if (GetMeChairID()==m_wCurrentBanker)
		{
			BOOL bEnableButton=TRUE;
			if (GetGameStatus()!=GAME_STATUS_FREE) bEnableButton=FALSE;
			if (m_cbLeftCardCount < 8) bEnableButton=FALSE;

			//m_GameClientView.m_btContinueCard.ShowWindow(SW_SHOW);			
			//m_GameClientView.m_btContinueCard.EnableWindow(bEnableButton);
		}
		else
		{
			//m_GameClientView.m_btContinueCard.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
		m_GameClientView.m_btQiangZhuang.EnableWindow(FALSE);

		//m_GameClientView.m_btApplyBanker.ShowWindow(SW_SHOW);
		//m_GameClientView.m_btCancelBanker.EnableWindow(FALSE);
		//m_GameClientView.m_btCancelBanker.ShowWindow(SW_HIDE);
		//m_GameClientView.m_btContinueCard.ShowWindow(SW_HIDE);
	}
	if (m_GameClientView.m_ApplyUser.GetItemCount()>=10) m_GameClientView.m_btApplyBanker.EnableWindow(FALSE);
	//m_GameClientView.m_btApplyBanker.EnableWindow( m_GameClientView.m_ApplyUser.GetItemCount()>=3 ? FALSE : TRUE);

	ReSetGameCtr();
	m_GameClientView.InvalidGameView(0,0,0,0);
	return;
}

//��ע��Ϣ
LRESULT CGameClientDlg::OnPlaceJetton(WPARAM wParam, LPARAM lParam)
{
	//��������
	BYTE cbJettonArea=(BYTE)wParam;
	LONGLONG lJettonScore=m_GameClientView.GetCurrentJetton();

	//�Ϸ��ж�
	ASSERT(cbJettonArea>=ID_SHUN_MEN && cbJettonArea<=ID_HENG_R);
	if (!(cbJettonArea>=ID_SHUN_MEN && cbJettonArea<=ID_HENG_R)) return 0;

	//ׯ���ж�
	if ( GetMeChairID() == m_wCurrentBanker ) return true;

	//״̬�ж�
	if (GetGameStatus()!=GS_PLACE_JETTON)
	{
		UpdateButtonContron();
		return true;
	}
	
	//���ñ���
	m_lUserJettonScore[cbJettonArea] += lJettonScore;
	m_GameClientView.SetMePlaceJetton(cbJettonArea, m_lUserJettonScore[cbJettonArea]);

	//��������
	CMD_C_PlaceJetton PlaceJetton;
	ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

	//�������
	PlaceJetton.cbJettonArea=cbJettonArea;
	PlaceJetton.lJettonScore=lJettonScore;

	//������Ϣ
	SendSocketData(SUB_C_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));

	//Ԥ����ʾ
	m_GameClientView.PlaceUserJetton(cbJettonArea,lJettonScore);

	m_nNotScoreCnt = 0;

	//���°�ť
	UpdateButtonContron();

	//��������
	//if (IsEnableSound()) 
	{
		if (lJettonScore==5000000) PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD_EX"));
		else PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_GOLD"));
		//m_DTSDCheer[rand()%3].Play();
	}
	//CString str;
	//str.Format(TEXT("��ע����:%d"),cbJettonArea);
	//AfxMessageBox(str);

	return 0;
}
//�ֹ�����
LRESULT  CGameClientDlg::OnOpenCard(WPARAM wParam, LPARAM lParam)
{
	if (GetGameStatus()==GS_GAME_END)
	{
		m_pIStringMessage->InsertCustomString(TEXT("��ϵͳ��ʾ�ݿ���ʱ�䲻���л�����ģʽ��"),RGB(255,255,255));
		return 1;

	}
	m_GameClientView.m_blAutoOpenCard = false;
	m_pIStringMessage->InsertCustomString(TEXT("��ϵͳ��ʾ����ѡ�����ֹ�����ģʽ�����ƺ�������ʹ������϶��˿ˣ�"),RGB(255,255,255));
	m_GameClientView.m_btOpenCard.EnableWindow(false);
	m_GameClientView.m_btAutoOpenCard.EnableWindow(true);
	m_GameClientView.m_btAutoOpenCard.ShowWindow(true);
	m_GameClientView.m_btOpenCard.ShowWindow(false);
	return 1;
}
//�Զ�����
LRESULT  CGameClientDlg::OnAutoOpenCard(WPARAM wParam, LPARAM lParam)
{
	if (GetGameStatus()==GS_GAME_END)
	{
		m_pIStringMessage->InsertCustomString(TEXT("��ϵͳ��ʾ�ݿ���ʱ�䲻���л�����ģʽ��"),RGB(255,255,255));
		return 1;

	}
	m_GameClientView.m_blAutoOpenCard = true;
	m_pIStringMessage->InsertCustomString(TEXT("��ϵͳ��ʾ����ѡ�����Զ�����ģʽ�����ƺ�ϵͳ���Զ��������ҵ��ƣ�"),RGB(255,255,255));
	m_GameClientView.m_btOpenCard.EnableWindow(true);
	m_GameClientView.m_btAutoOpenCard.EnableWindow(false);
	m_GameClientView.m_btAutoOpenCard.ShowWindow(false);
	m_GameClientView.m_btOpenCard.ShowWindow(true);
	return 1;
}
//��������
LRESULT CGameClientDlg::OnContinueCard(WPARAM wParam, LPARAM lParam)
{
	//�Ϸ��ж�
	if (GetMeChairID()!=m_wCurrentBanker) return 0;
	if (GetGameStatus()!=GAME_STATUS_FREE) return 0;
	if (m_cbLeftCardCount < 8) return 0;
	if (IsLookonMode()) return 0;

	//������Ϣ
	//SendSocketData(SUB_C_CONTINUE_CARD);

	//���ð�ť
	//m_GameClientView.m_btContinueCard.EnableWindow(FALSE);

	return 0;
}

//������Ϣ
LRESULT CGameClientDlg::OnApplyBanker(WPARAM wParam, LPARAM lParam)
{
	//�Ϸ��ж�
	IClientUserItem  *pMeUserData = GetTableUserItem( GetMeChairID() );
	if (pMeUserData->GetUserScore() < m_lApplyBankerCondition) return true;



	//�Թ��ж�
	if (IsLookonMode()) return true;

	//ת������
	bool bApplyBanker = wParam ? true:false;

	//��ǰ�ж�
	if (m_wCurrentBanker == GetMeChairID() && bApplyBanker) return true;

	if (bApplyBanker)
	{
		//������Ϣ
		SendSocketData(SUB_C_APPLY_BANKER, NULL, 0);

		m_bMeApplyBanker=true;
	}
	else
	{
		//������Ϣ
		SendSocketData(SUB_C_CANCEL_BANKER, NULL, 0);

		m_bMeApplyBanker=false;
	}

	//���ð�ť
	UpdateButtonContron();

	return true;
}

LRESULT CGameClientDlg::OnQiangBanker(WPARAM wParam,LPARAM lParam)
{
	IClientUserItem  *pMeUserData = GetTableUserItem( GetMeChairID() );
	if (pMeUserData->GetUserScore() < m_lQiangCondition) 
	{
		m_pIStringMessage->InsertCustomString(TEXT("���Ļ��ֶ�������ׯ��������ׯʧ�ܣ�"),RGB(255,255,0));
		return true;
	}

	if(IsLookonMode()) return true;
	//ת������
	bool bApplyBanker = wParam ? true:false;
	//��ǰ�ж�
	//��ǰ�ж�
	if (m_wCurrentBanker == GetMeChairID() && bApplyBanker) 
	{
		m_pIStringMessage->InsertCustomString(TEXT("���Ѿ���ׯ�ң�����ʧ�ܣ�"),RGB(255,255,0));
		return true;
	}

	if (bApplyBanker)
	{
		CInformation info(this);
		CString szMessage;
		szMessage.Format(TEXT("ȷ��Ҫ֧��%I64d ���ֶ���ׯ��"),m_lQiangScore);
		if(IDYES==info.ShowMessageBox(TEXT("ָ����"),szMessage,MB_YESNO))
		{
			//������Ϣ
			SendSocketData(SUB_C_QIANG_BANKER, NULL, 0);

			m_bMeQiangBanker=true;
		}
		else
		{
			return true;
		}
		
	}
	else
	{
		//������Ϣ
		SendSocketData(SUB_C_CANCEL_BANKER, NULL, 0);

		m_bMeApplyBanker=false;
	}

	//���ð�ť
	UpdateButtonContron();


	return true;
}

//������ׯ
bool CGameClientDlg::OnSubUserApplyBanker( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//��Ϣ����
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

	//��ȡ���
	IClientUserItem  *pUserData=GetTableUserItem(pApplyBanker->wApplyUser);

	//�������
	if (m_wCurrentBanker!=pApplyBanker->wApplyUser)
	{
		tagApplyUser ApplyUser;
		ApplyUser.bQiang=false;
		ApplyUser.strUserName=pUserData->GetNickName();
		ApplyUser.lUserScore=pUserData->GetUserScore();
		m_GameClientView.m_ApplyUser.InserUser(ApplyUser);

		if(m_GameClientView.m_ApplyUser.GetItemCount()>4)
		{
			m_GameClientView.m_btUp.EnableWindow(true);
			m_GameClientView.m_btDown.EnableWindow(true);  
		}
		else
		{
			m_GameClientView.m_btUp.EnableWindow(false);
			m_GameClientView.m_btDown.EnableWindow(false); 
		}
	}

	m_GameClientView.m_btApplyBanker.EnableWindow( m_GameClientView.m_ApplyUser.GetItemCount()>=10 ? FALSE : TRUE);
	//�Լ��ж�
	if (IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser) m_bMeApplyBanker=true;

	//���¿ؼ�
	UpdateButtonContron();

	return true;
}
bool CGameClientDlg::OnSubQiangBanker(void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ApplyBanker));
	if (wDataSize!=sizeof(CMD_S_ApplyBanker)) return false;

	//��Ϣ����
	CMD_S_ApplyBanker * pApplyBanker=(CMD_S_ApplyBanker *)pBuffer;

	//��ȡ���
	IClientUserItem  *pUserData=GetTableUserItem(pApplyBanker->wApplyUser);

	//�������
	if (m_wCurrentBanker!=pApplyBanker->wApplyUser)
	{
		tagApplyUser ApplyUser;
		ApplyUser.bQiang=true;
			ApplyUser.strUserName=pUserData->GetNickName();
		ApplyUser.lUserScore=pUserData->GetUserScore();
		m_GameClientView.m_ApplyUser.InserUser(ApplyUser);

		if(m_GameClientView.m_ApplyUser.GetItemCount()>4)
		{
			m_GameClientView.m_btUp.EnableWindow(true);
			m_GameClientView.m_btDown.EnableWindow(true);  
		}
		else
		{
			m_GameClientView.m_btUp.EnableWindow(false);
			m_GameClientView.m_btDown.EnableWindow(false); 
		}
	}

	m_GameClientView.m_btApplyBanker.EnableWindow( m_GameClientView.m_ApplyUser.GetItemCount()>=10 ? FALSE : TRUE);
	//�Լ��ж�
	if (IsLookonMode()==false && GetMeChairID()==pApplyBanker->wApplyUser) m_bMeApplyBanker=true;

	//���¿ؼ�
	UpdateButtonContron();

	return true;
}


//ȡ����ׯ
bool CGameClientDlg::OnSubUserCancelBanker( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_CancelBanker));
	if (wDataSize!=sizeof(CMD_S_CancelBanker)) return false;

	//��Ϣ����
	CMD_S_CancelBanker * pCancelBanker=(CMD_S_CancelBanker *)pBuffer;


	TCHAR szNickName[32]=TEXT("#");

	if(GetTableUserItem(pCancelBanker->dwCancelUser)!=NULL)
	{
		_sntprintf(szNickName,sizeof(szNickName),TEXT("%s"),GetTableUserItem(pCancelBanker->dwCancelUser)->GetNickName());
	}


	//ɾ�����
	tagApplyUser ApplyUser;
	ApplyUser.strUserName=szNickName;
	ApplyUser.lUserScore=0;
	m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);

	if(m_GameClientView.m_ApplyUser.GetItemCount()>4)
	{
		m_GameClientView.m_btUp.EnableWindow(true);
		m_GameClientView.m_btDown.EnableWindow(true);  
	}
	else
	{
		m_GameClientView.m_btUp.EnableWindow(false);
		m_GameClientView.m_btDown.EnableWindow(false); 
	}

	m_GameClientView.m_btApplyBanker.EnableWindow( m_GameClientView.m_ApplyUser.GetItemCount()>=10 ? FALSE : TRUE);

	//�Լ��ж�
	// IClientUserItem *pMeUserData=GetTableUserItem(GetMeChairID());
	if (IsLookonMode()==false && pCancelBanker->dwCancelUser==GetMeChairID()) 
	{
		m_bMeApplyBanker=false;
		m_bMeQiangBanker=false;
	}

	//���¿ؼ�
	UpdateButtonContron();

	return true;
}

//�л�ׯ��
bool CGameClientDlg::OnSubChangeBanker( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_ChangeBanker));
	if (wDataSize!=sizeof(CMD_S_ChangeBanker)) return false;

	//��Ϣ����
	CMD_S_ChangeBanker * pChangeBanker=(CMD_S_ChangeBanker *)pBuffer;

	//��ʾͼƬ
	m_GameClientView.ShowChangeBanker(m_wCurrentBanker!=pChangeBanker->wBankerUser);

	//�Լ��ж�
	if (m_wCurrentBanker==GetMeChairID() && IsLookonMode() == false && pChangeBanker->wBankerUser!=GetMeChairID()) 
	{
		m_bMeApplyBanker=false;
		m_bMeQiangBanker=false;
	}
	else if (IsLookonMode() == false && pChangeBanker->wBankerUser==GetMeChairID())
	{
		m_bMeApplyBanker=true;
	}

	//ׯ����Ϣ
	SetBankerInfo(pChangeBanker->wBankerUser,pChangeBanker->lBankerScore);
	m_GameClientView.SetBankerScore(0,0);

	//ɾ�����
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		IClientUserItem  *pBankerUserData=GetTableUserItem(m_wCurrentBanker);
		if (pBankerUserData != NULL)
		{
			tagApplyUser ApplyUser;
			ApplyUser.strUserName = pBankerUserData->GetNickName();
			m_GameClientView.m_ApplyUser.DeleteUser(ApplyUser);
		}
	}

	if(m_GameClientView.m_ApplyUser.GetItemCount()>4)
	{
		m_GameClientView.m_btUp.EnableWindow(true);
		m_GameClientView.m_btDown.EnableWindow(true);  
	}
	else
	{
		m_GameClientView.m_btUp.EnableWindow(false);
		m_GameClientView.m_btDown.EnableWindow(false); 
	}

	//���½���
	UpdateButtonContron();
	m_GameClientView.UpdateGameView(NULL);

	return true;
}

//��Ϸ��¼
bool CGameClientDlg::OnSubGameRecord( void * pBuffer, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagServerGameRecord)==0);
	if (wDataSize%sizeof(tagServerGameRecord)!=0) return false;

	//�������
	tagGameRecord GameRecord;
	ZeroMemory(&GameRecord,sizeof(GameRecord));

	//���ü�¼
	WORD wRecordCount=wDataSize/sizeof(tagServerGameRecord);
	for (WORD wIndex=0;wIndex<wRecordCount;wIndex++) 
	{
		tagServerGameRecord * pServerGameRecord=(((tagServerGameRecord *)pBuffer)+wIndex);

		m_GameClientView.SetGameHistory(pServerGameRecord->bWinShunMen, pServerGameRecord->bWinDaoMen, pServerGameRecord->bWinDuiMen);
	}

	return true;
}

//��עʧ��
bool CGameClientDlg::OnSubPlaceJettonFail( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_PlaceJettonFail));
	if (wDataSize!=sizeof(CMD_S_PlaceJettonFail)) return false;

	//��Ϣ����
	CMD_S_PlaceJettonFail * pPlaceJettonFail=(CMD_S_PlaceJettonFail *)pBuffer;

	//Ч�����
	BYTE cbViewIndex=pPlaceJettonFail->lJettonArea;
	ASSERT(cbViewIndex<=ID_HENG_R && cbViewIndex>=ID_SHUN_MEN);
	if (!(cbViewIndex<=ID_HENG_R && cbViewIndex>=ID_SHUN_MEN)) return false;

	//��ע����
	m_GameClientView.PlaceUserJetton(pPlaceJettonFail->lJettonArea,-pPlaceJettonFail->lPlaceScore);

	//�Լ��ж�
	if (GetMeChairID()==pPlaceJettonFail->wPlaceUser && false==IsLookonMode())
	{
		LONGLONG lJettonCount=pPlaceJettonFail->lPlaceScore;
		//�Ϸ�У��
		ASSERT(m_lUserJettonScore[cbViewIndex]>=lJettonCount);
		if (lJettonCount<=m_lUserJettonScore[cbViewIndex])
		{
			//������ע
			m_lUserJettonScore[cbViewIndex]-=lJettonCount;
			m_GameClientView.SetMePlaceJetton(cbViewIndex,m_lUserJettonScore[cbViewIndex]);
		}

	}

	//���°�ť
	UpdateButtonContron();

	return true;
}

//�������
bool CGameClientDlg::OnSubCheat( void * pBuffer, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_Cheat));
	if (wDataSize!=sizeof(CMD_S_Cheat)) return false;

	//��Ϣ����
	CMD_S_Cheat * pCheat=(CMD_S_Cheat *)pBuffer;

	//Ȩ�޸���
	if (CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)==false)	return true;

	//ʤ����Ϣ
	int nWinInfo[3] = {};
	for (int i = 0; i < 3; i++)
		nWinInfo[i] = m_GameLogic.CompareCard(pCheat->cbTableCardArray[i+1], 2, pCheat->cbTableCardArray[0], 2);

	m_GameClientView.SetCheatInfo(&pCheat->cbTableCardArray[0][0], nWinInfo);

	return true;
}

//����ׯ��
void CGameClientDlg::SetBankerInfo(WORD wBanker,LONGLONG lScore)
{
	m_wCurrentBanker=wBanker;
	m_lBankerScore=lScore;
	IClientUserItem  *pUserData=m_wCurrentBanker==INVALID_CHAIR ? NULL : GetTableUserItem(m_wCurrentBanker);
	DWORD dwBankerUserID = (NULL==pUserData) ? 0 : pUserData->GetUserID();
	m_GameClientView.SetBankerInfo(dwBankerUserID,m_lBankerScore);
}

//������ע
void CGameClientDlg::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
	//�Ϸ��ж�
	ASSERT(cbViewIndex>=ID_SHUN_MEN && cbViewIndex<=ID_HENG_R);
	if (!(cbViewIndex>=ID_SHUN_MEN && cbViewIndex<=ID_HENG_R)) return;

	//���ñ���
	m_lUserJettonScore[cbViewIndex]=lJettonCount;

	//���ý���
	m_GameClientView.SetMePlaceJetton(cbViewIndex,lJettonCount);
}

void CGameClientDlg::SendUserMessage(LPCTSTR str,COLORREF color)
{
	m_pIStringMessage->InsertCustomString(str,color);
}
//////////////////////////////////////////////////////////////////////////

void CGameClientDlg::ReSetGameCtr()
{
#ifdef __SPECIAL___
	if(!IsLookonMode())
	{
		switch (GetGameStatus())
		{
		case GAME_STATUS_FREE:
			{
				if(GetMeChairID()!=m_wCurrentBanker)
				{
					if(m_GameClientView.m_DlgBank.m_hWnd!=NULL)
					{
						m_GameClientView.m_DlgBank.AllowStorage(TRUE);
						m_GameClientView.m_DlgBank.AllowDraw(TRUE);
					}

				}
				else
				{
					if(m_GameClientView.m_DlgBank.m_hWnd!=NULL)
					{
						m_GameClientView.m_DlgBank.AllowStorage(FALSE);
						m_GameClientView.m_DlgBank.AllowDraw(TRUE);
					}

				}
				break;
			}
		case GS_GAME_END:
		case GS_PLACE_JETTON:
			{
				if(GetMeChairID()!=m_wCurrentBanker)
				{
					if(m_GameClientView.m_DlgBank.m_hWnd)
					{
						m_GameClientView.m_DlgBank.AllowStorage(FALSE);
						m_GameClientView.m_DlgBank.AllowDraw(TRUE);
					}
				}
				else
				{
					if(m_GameClientView.m_DlgBank.m_hWnd)
					{
						m_GameClientView.m_DlgBank.AllowStorage(FALSE);
						m_GameClientView.m_DlgBank.AllowDraw(TRUE);
					}
				}
				break;
			}
		default:break;
		}
	}
	else
	{
	}
#endif
}

bool CGameClientDlg::OnSubScoreResult(void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize==sizeof(CMD_S_ScoreResult));
	if(wDataSize!=sizeof(CMD_S_ScoreResult)) return false;

	CMD_S_ScoreResult * pResult=(CMD_S_ScoreResult*)pBuffer;

	if (m_GameClientView.m_pClientControlDlg!=NULL && m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
	{
		for(int i=0; i<AREA_COUNT; i++) m_GameClientView.m_pClientControlDlg->SetUserGameScore(i,pResult->lUserJettonScore[i]);
	}

	return true;
}

bool CGameClientDlg::OnSubAccountResult( void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize==sizeof(CMD_S_AccountResult));
	if(wDataSize!=sizeof(CMD_S_AccountResult)) return false;

	CMD_S_AccountResult * pResult=(CMD_S_AccountResult*)pBuffer;

	if (m_GameClientView.m_pClientControlDlg!=NULL && m_GameClientView.m_pClientControlDlg->GetSafeHwnd())
	{
		m_GameClientView.m_pClientControlDlg->ResetUserNickName();
		//m_GameClientView.m_pClientControlDlg->ResetUserBet();

		for(int i=0; i<MAX_CHAIR; i++) 
		{
			if(pResult->szAccount[i][0]!=TEXT('\0')) m_GameClientView.m_pClientControlDlg->SetUserNickName(pResult->szAccount[i]);
		}
	}

	return true;
}

bool CGameClientDlg::OnSubReqResult(const void * pBuffer, WORD wDataSize)
{
	ASSERT(wDataSize==sizeof(CMD_S_CommandResult));
	if(wDataSize!=sizeof(CMD_S_CommandResult)) return false;

	if( NULL != m_GameClientView.m_pClientControlDlg && NULL != m_GameClientView.m_pClientControlDlg->GetSafeHwnd() )
	{
		m_GameClientView.m_pClientControlDlg->ReqResult(pBuffer);
	}

	return true;
}

LRESULT CGameClientDlg::OnAdminCommand(WPARAM wParam,LPARAM lParam)
{
	SendSocketData(SUB_C_AMDIN_COMMAND,(CMD_C_AdminReq*)wParam,sizeof(CMD_C_AdminReq));
	return true;
}

LRESULT CGameClientDlg::OnGetAccount(WPARAM wParam, LPARAM lParam)
{
	SendSocketData(SUB_C_GET_ACCOUNT);
	return true;
}

LRESULT CGameClientDlg::OnCheckAccount(WPARAM wParam, LPARAM lParam)
{
	CString strNickName;
	m_GameClientView.m_pClientControlDlg->GetUserNickName(strNickName);
	if(strNickName.IsEmpty()) return 0;

	CMD_C_CheakAccount CheakAccount;
	TRACE("szNickName=%s\n",strNickName);
	CopyMemory(CheakAccount.szUserAccount,strNickName.GetBuffer(),CountArray(CheakAccount.szUserAccount));
	strNickName.ReleaseBuffer();
	SendSocketData(SUB_C_CHECK_ACCOUNT,&CheakAccount,sizeof(CMD_C_CheakAccount));
	return true;
}
LRESULT CGameClientDlg::OnResetStorage(WPARAM wParm,LPARAM lParam)
{
	CMD_C_UpdateStorage * pResult=(CMD_C_UpdateStorage*)wParm;

	CMD_C_UpdateStorage  pData;
	pData.lStorage = pResult->lStorage;
	pData.lStorageDeduct=pResult->lStorageDeduct;

	SendSocketData(SUB_C_ADMIN_RESET_STORAGE,&pData,sizeof(pData));
	return true;
}
void CGameClientDlg::OnTimer(UINT nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == IDI_ANDROID_BET)
	{
		if (m_ListAndroid.GetCount() != 0)
		{
			POSITION pos = m_ListAndroid.GetTailPosition();
			POSITION posTmp = pos;

			//������ע����
			while (true)
			{
				if (pos == 0)
					break;

				posTmp = pos;
				tagAndroidBet & androidBet = m_ListAndroid.GetPrev(pos);
				
				androidBet.nLeftTime -= 100;
				if (androidBet.nLeftTime <= 0)
				{		
					//ģ����Ϣ
					CMD_S_PlaceJetton placeJetton = {};
					placeJetton.bIsAndroid = false;
					placeJetton.cbJettonArea = androidBet.cbJettonArea;
					placeJetton.lJettonScore = androidBet.lJettonScore;
					placeJetton.wChairID = androidBet.wChairID;

					//MyDebug(_T("client::OnTimer ��� %d ���� %d ��ע %I64d"), androidBet.wChairID, androidBet.cbJettonArea, androidBet.lJettonScore);

					OnSubPlaceJetton((void*)&placeJetton, sizeof(placeJetton),false);

					//ɾ��Ԫ��
					m_ListAndroid.RemoveAt(posTmp);
				}
			}
		}
	}

	__super::OnTimer(nIDEvent);
}
bool CGameClientDlg::OnEventGameClockKill( WORD wChairID )
{
	return true;
}

bool CGameClientDlg::AllowBackGroundSound( bool bAllowSound )
{
	if(bAllowSound)
		PlayBackGroundSound(AfxGetInstanceHandle(),TEXT("BACK_GROUND"));
	else
		StopSound();

	return true;
}
