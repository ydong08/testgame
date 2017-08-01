#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define SEND_COUNT					300									//���ʹ���

//��������
#define INDEX_PLAYER				0									//�м�����
#define INDEX_BANKER				1									//ׯ������

//ʱ�䶨��
#define IDI_FREE					1									//����ʱ��
#define IDI_PLACE_JETTON			2									//��עʱ��
#define IDI_GAME_END				3									//����ʱ��
//��ʱ������
#define IDI_RESET_STORAGE           4                                   //������

//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//����ע��
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//���Ʊ���
	m_cbWinSideControl=0;
	m_nSendCardCount=0;

	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�˿���Ϣ
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//״̬����
	m_dwJettonTime=0L;
	m_bExchangeBanker=true;
	m_wAddTime=0;

	//ׯ����Ϣ
	m_ApplyUserArray.RemoveAll();
	m_wCurrentBanker=INVALID_CHAIR;
	m_wDownBanker = INVALID_CHAIR;
	m_wQiangBanker = INVALID_CHAIR;
	m_bQiangCurrent = false;
	m_wBankerTime=0;
	m_lBankerWinScore=0L;		
	m_lBankerCurGameScore=0L;
	m_bEnableSysBanker=true;
	m_cbLeftCardCount=0;
	m_lExitScore=0;
	m_bContiueCard=false;
	m_bAllWin=0;

	m_bWinShunMen = false;
	m_bWinDuiMen = false;
	m_bWinDaoMen = false;

	//��¼����
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));

	//�����˿���
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//�������
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_hInst = LoadLibrary(TEXT("SupremeServerControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}

	m_bcFirstPostCard = MAKEWORD(1,1);
	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//�ӿڲ�ѯ
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//���ƽӿ�
//	m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
//	if (m_pITableFrameControl==NULL) return false;
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//�����ļ���
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\SupremeConfig.ini"),szPath);

	//��ȡ����
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	_sntprintf(m_szRoomName,sizeof(m_szRoomName),TEXT("%d"),m_pGameServiceOption->wServerID);

	//��������
	TCHAR OutBuf[255] = {};
	////�������
	//m_lStorageStart = GetPrivateProfileInt(m_szRoomName, TEXT("StorageStart"), 0, m_szConfigFileName);
	//m_nStorageDeduct = GetPrivateProfileInt(m_szRoomName, TEXT("StorageDeduct"), 0, m_szConfigFileName);

	//�������
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("StorageStart"), TEXT("0"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_lStorageStart);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("StorageDeduct"), TEXT("0"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, lstrlen(OutBuf), TEXT("%I64d"), &m_nStorageDeduct);



	ReadConfigInformation(true);	

	m_StorageControl.InitConfig(m_szConfigFileName,m_szRoomName,false);

	return true;
}

//��λ����
VOID  CTableFrameSink::RepositionSink()
{
	//����ע��
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
	
	//������ע
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//��ҳɼ�	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//�����˿���
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));


	m_bcFirstPostCard = MAKEWORD(1,1);
	return;
}



//��Ϸ״̬
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//��ѯ�޶�
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		return 0L;
	}
	else
	{
		return __max(pIServerUserItem->GetUserScore()-m_pGameServiceOption->lMinEnterScore-10L, 0L);
	}
}

//��ѯ�Ƿ�۷����
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;

		if(wChairID==m_wCurrentBanker) return true;

		if(wChairID==i)
		{
			//������ע
			for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) 
			{

				if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
				{
					return true;
				}
			}
			break;
		}
	}
	return false;
}

//��Ϸ��ʼ
bool  CTableFrameSink::OnEventGameStart()
{
	m_StorageControl.ResetCondition();

	//��������
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//��ȡׯ��
	IServerUserItem *pIBankerServerUserItem=NULL;
	if (INVALID_CHAIR!=m_wCurrentBanker) 
	{
		pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		m_lBankerScore=pIBankerServerUserItem->GetUserScore();
	}

	//���ñ���
	GameStart.cbTimeLeave=m_nPlaceJettonTime;
	GameStart.wBankerUser=m_wCurrentBanker;
	if (pIBankerServerUserItem!=NULL) 
		GameStart.lBankerScore=m_lBankerScore;
	GameStart.bContiueCard=m_bContiueCard;

	//��ע����������
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

	//MyDebug(_T("���������� %d ��ע���������� %d"), nChipRobotCount, m_nMaxChipRobot);

	//�����˿���
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//�Թ����
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	

	//��Ϸ���
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//���û���
		GameStart.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);

		if (pIServerUserItem->IsAndroidUser())
		{
			GameStart.nChipRobotCount = nChipRobotCount;
		}
		else
		{
			GameStart.nChipRobotCount = 0;
		}

		m_pITableFrame->SendTableData(wChairID,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	
	}

	return true;
}

//��Ϸ����
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������	
		{
			//��Ϸ����
			if( m_pServerContro != NULL && m_pServerContro->NeedControl() )
			{
				//��ȡ����Ŀ������
				m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
				m_pServerContro->CompleteControl();
			}
 			else
 			{
                 DispatchCard();
 			}

			bool static bWinShunMen1, bWinDuiMen1, bWinDaoMen1;
			DeduceWinner(bWinShunMen1, bWinDuiMen1, bWinDaoMen1);
			if(((bWinDaoMen1==bWinShunMen1)&&(bWinShunMen1==bWinDuiMen1)) && (m_rand.Random(100) > 80 || m_bAllWin>=(m_rand.Random(6)+3)))
			{
				do 
				{
					DispatchTableCard();
					DeduceWinner(bWinShunMen1, bWinDuiMen1, bWinDaoMen1);
				} while (((bWinDaoMen1==bWinShunMen1)&&(bWinShunMen1==bWinDuiMen1)));
			}


			BYTE cbCard[8];
			cbCard[0]=m_cbTableCardArray[0][0];
			cbCard[1]=m_cbTableCardArray[1][0];
			cbCard[2]=m_cbTableCardArray[2][0];
			cbCard[3]=m_cbTableCardArray[3][0];
			cbCard[4]=m_cbTableCardArray[0][1];
			cbCard[5]=m_cbTableCardArray[1][1];
			cbCard[6]=m_cbTableCardArray[2][1];
			cbCard[7]=m_cbTableCardArray[3][1];

			bool bSameCard=true;
			for(unsigned i=0;i<8;i++)
			{
				for(unsigned j=i+1;j<8;j++)
				{
					if(cbCard[i]==cbCard[j])
					{
						bSameCard=false;
					}
				}
			}
			if(!bSameCard)
			{
				DispatchTableCard();
			}

			bool static  bWinShunMen2, bWinDuiMen2, bWinDaoMen2;
			DeduceWinner(bWinShunMen2, bWinDuiMen2, bWinDaoMen2);
			if(((bWinDaoMen2==bWinShunMen2)&&(bWinShunMen2==bWinDuiMen2)))
			{
				m_bAllWin++;
			}
			else
			{
				m_bAllWin=0;
			}



			//�������
			LONGLONG lBankerWinScore=CalculateScore();

			//��������
			m_wBankerTime++;

			//������Ϣ
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			m_lBankerWinScore+=m_lExitScore;
			lBankerWinScore+=m_lExitScore;

			//ׯ����Ϣ
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;
			GameEnd.bcFirstCard = m_bcFirstPostCard;
			GameEnd.wCurrentBanker = m_wCurrentBanker;

			//�˿���Ϣ
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			GameEnd.cbLeftCardCount=m_cbLeftCardCount;

			//���ͻ���
			GameEnd.cbTimeLeave=m_nGameEndTime;	
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//���óɼ�
				GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

				//��������
				GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

				//����˰��
				if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
				else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
				else GameEnd.lRevenue=0;

				if(m_wCurrentBanker != INVALID_CHAIR && m_lExitScore > 0L && wUserIndex== m_wCurrentBanker)
				{ 
				      GameEnd.lUserScore=m_lUserWinScore[wUserIndex]+m_lExitScore;
				}

				//������Ϣ					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			return true;
		}
	case GER_USER_LEAVE:		//�û��뿪
	case GER_NETWORK_ERROR:
		{
			//�м��ж�
			if (m_wCurrentBanker!=wChairID)
			{
				//��������
				LONGLONG lScore=0;
				LONGLONG lRevenue=0;
			

				//ͳ�Ƴɼ�
				for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lScore -= m_lUserJettonScore[nAreaIndex][wChairID];

				LONGLONG lExitScore=0L;

				//д�����
				if (m_pITableFrame->GetGameStatus() != GS_GAME_END)
				{
					for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
					{
						if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
						{
							CMD_S_PlaceJettonFail PlaceJettonFail;
							ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
							PlaceJettonFail.lJettonArea=nAreaIndex;
							PlaceJettonFail.lPlaceScore=m_lUserJettonScore[nAreaIndex][wChairID];
							PlaceJettonFail.wPlaceUser=wChairID;

							////��Ϸ���
							//for (WORD i=0; i<GAME_PLAYER; ++i)
							//{
							//	IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
							//	if (pIServerUserItem==NULL) 
							//		continue;

							//	m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
							//}
                            lExitScore+=m_lUserJettonScore[nAreaIndex][wChairID];
							m_lExitScore+=m_lUserJettonScore[nAreaIndex][wChairID];
							m_lAllJettonScore[nAreaIndex] -= m_lUserJettonScore[nAreaIndex][wChairID];
							m_lUserJettonScore[nAreaIndex][wChairID] = 0;
						}
					}
					tagScoreInfo scoreInfo;
					ZeroMemory(&scoreInfo,sizeof(scoreInfo));
					scoreInfo.cbType=SCORE_TYPE_LOSE;
					scoreInfo.lScore=m_lExitScore*-1;
					scoreInfo.lRevenue=0;
					m_pITableFrame->WriteUserScore(wChairID,scoreInfo);

					

					if(lExitScore > 0L)
					{
						CString str;
						str.Format(TEXT("�м� %sǿ�ˣ�"),pIServerUserItem->GetNickName());
						m_pITableFrame->SendGameMessage(str,SMT_CHAT);
					}
					//m_lExitScore=0L;
					return true;
				}
				else
				{

					for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
					{
						if (m_lUserJettonScore[nAreaIndex][wChairID] != 0)
						{
							m_lUserJettonScore[nAreaIndex][wChairID] = 0;
						}
					}

					
					//д�����
					if (m_lUserWinScore[wChairID]!=0L) 
					{
						tagScoreInfo ScoreInfo[GAME_PLAYER];
						ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
						ScoreInfo[wChairID].cbType=m_lUserWinScore[wChairID]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;;
						ScoreInfo[wChairID].lRevenue=m_lUserRevenue[wChairID];
						ScoreInfo[wChairID].lScore=m_lUserWinScore[wChairID];
						m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

						/*if(pIServerUserItem->IsAndroidUser()==false)
						{
							m_lStorageStart-=m_lUserWinScore[wChairID];
						}*/
						//m_pITableFrame->WriteUserScore(pIServerUserItem,m_lUserWinScore[wChairID], static_cast<long>(m_lUserRevenue[wChairID]), ScoreKind);
						m_lUserWinScore[wChairID] = 0;
					}
				}
				return true;
			}

			//״̬�ж�
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("����ׯ��[ %s ]ǿ�ˣ�ϵͳ��ǰ���ƣ���ͨ����㣡"),pIServerUserItem->GetNickName());

				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//����״̬
				m_pITableFrame->SetGameStatus(GS_GAME_END);

				//����ʱ��
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);

				////��Ϸ����
				//if( m_pServerContro != NULL && m_pServerContro->NeedControl() )
				//{
				//	//��ȡ����Ŀ������
				//	m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
				//	m_pServerContro->CompleteControl();
				//}

				//�������
				CalculateScore1();

				//������Ϣ
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//ׯ����Ϣ
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				GameEnd.lBankerScore=m_lUserWinScore[m_wCurrentBanker];
				GameEnd.wCurrentBanker = m_wCurrentBanker;

				//�˿���Ϣ
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
				GameEnd.cbLeftCardCount=m_cbLeftCardCount;

				//���ͻ���
				GameEnd.cbTimeLeave=m_nGameEndTime;	
				for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
					if ( pIServerUserItem == NULL ) continue;

					//���óɼ�
					GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

					//��������
					GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

					//����˰��
					if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
					else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
					else GameEnd.lRevenue=0;

					//������Ϣ					
					m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//�۳�����
			if (m_lUserWinScore[m_wCurrentBanker] != 0l)
			{
				tagScoreInfo ScoreInfo[GAME_PLAYER];
				ZeroMemory(ScoreInfo,sizeof(ScoreInfo));
				ScoreInfo[wChairID].cbType=SCORE_TYPE_FLEE;
				ScoreInfo[wChairID].lRevenue=m_lUserRevenue[m_wCurrentBanker];
				ScoreInfo[wChairID].lScore=(m_lUserWinScore[m_wCurrentBanker] >0)?m_lUserWinScore[m_wCurrentBanker]*-1:m_lUserWinScore[m_wCurrentBanker];
				m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
				//m_pITableFrame->WriteUserScore(m_wCurrentBanker,m_lUserWinScore[m_wCurrentBanker], m_lUserRevenue[m_wCurrentBanker], enScoreKind_Flee);
				m_lUserWinScore[m_wCurrentBanker] = 0;
			}

			//�л�ׯ��
			ChangeBanker(true);

			return true;
		}
	}

	return false;
}

//�����¼�
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}
//���ͳ���
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);

			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			

			//������Ϣ
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;
			StatusFree.lQiangConition=m_lQiangCondition;
			StatusFree.lQiangScore=2000000;

			//ׯ����Ϣ
			StatusFree.bEnableSysBanker=m_bEnableSysBanker;
			StatusFree.wBankerUser=m_wCurrentBanker;	
			StatusFree.cbBankerTime=m_wBankerTime;
			StatusFree.lBankerWinScore=m_lBankerWinScore;
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusFree.lBankerScore=pIServerUserItem->GetUserScore();
			}

			//�����Ϣ
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*4); 
			}

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(m_nFreeTime-__min(dwPassTime,(DWORD)m_nFreeTime));

			StatusFree.dwServerID=m_pGameServiceOption->wServerID;

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//������ʾ
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d  ��ׯ������%I64d,��ׯ��Ҫ����֧��2000000���ֶ�"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore,m_lQiangCondition);
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
						
			//����������
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//��Ϸ״̬
	case GS_GAME_END:			//����״̬
		{
			//���ͼ�¼
			SendGameRecord(pIServerUserItem);		

			//��������
			CMD_S_StatusPlay StatusPlay={0};

			//ȫ����ע
			CopyMemory(StatusPlay.lAllJettonScore,m_lAllJettonScore,sizeof(StatusPlay.lAllJettonScore));

			//�����ע
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChiarID];

				//�����ע
				StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);
			}

			//������Ϣ
			StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;		
			StatusPlay.lAreaLimitScore=m_lAreaLimitScore;	

			StatusPlay.lQiangConition=m_lQiangCondition;
			StatusPlay.lQiangScore=2000000;

			//ׯ����Ϣ
			StatusPlay.bEnableSysBanker=m_bEnableSysBanker;
			StatusPlay.wBankerUser=m_wCurrentBanker;			
			StatusPlay.cbBankerTime=m_wBankerTime;
			StatusPlay.lBankerWinScore=m_lBankerWinScore;	
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				StatusPlay.lBankerScore=m_lBankerScore;
			}	

			//ȫ����Ϣ
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			int	nTotalTime = (cbGameStatus==GS_PLACE_JETTON?m_nPlaceJettonTime:m_nGameEndTime);
			StatusPlay.cbTimeLeave=(BYTE)(nTotalTime-__min(dwPassTime,(DWORD)nTotalTime));
			StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();						

			//�����ж�
			if (cbGameStatus==GS_GAME_END)
			{
				//���óɼ�
				StatusPlay.lEndUserScore=m_lUserWinScore[wChiarID];

				//��������
				StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChiarID];

				//����˰��
				if (m_lUserRevenue[wChiarID]>0) StatusPlay.lEndRevenue=m_lUserRevenue[wChiarID];
				else if (m_wCurrentBanker!=INVALID_CHAIR) StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue=0;

				//ׯ�ҳɼ�
				StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

				//�˿���Ϣ
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			}


			//��������
			StatusPlay.dwServerID=m_pGameServiceOption->wServerID;

			//���ͳ���
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d  ��ׯ������%I64d,��ׯ��Ҫ����֧��2000000���ֶ�"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore,m_lQiangCondition);
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
			
			//����������
			SendApplyUser( pIServerUserItem );

			return bSuccess;
		}
	}

	return false;
}

//��ʱ���¼�
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//����ʱ��
		{
			//��ʼ��Ϸ
			m_pITableFrame->StartGame();

			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_nPlaceJettonTime*1000,1,0L);

			//�ɷ��˿�
			DispatchTableCard();

			//����״̬
			m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

			return true;
		}
	case IDI_PLACE_JETTON:		//��עʱ��
		{
			//״̬�ж�(��ֹǿ���ظ�����)
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//����״̬
				m_pITableFrame->SetGameStatus(GS_GAME_END);			

				//������Ϸ
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//����ʱ��
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);			
			}

			return true;
		}
	case IDI_GAME_END:			//������Ϸ
		{
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			TCHAR szTipMsg[256] = {};
			TCHAR szScoreTip[2048] = {};

			LONGLONG lSystemScore = 0;
			//д�����
			for ( WORD wUserChairID = 0; wUserChairID < GAME_PLAYER; ++wUserChairID )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserChairID);
				if ( pIServerUserItem == NULL ||(QueryBuckleServiceCharge(wUserChairID)==false)) continue;

				
				ScoreInfo[wUserChairID].cbType=m_lUserWinScore[wUserChairID]>0?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
				ScoreInfo[wUserChairID].lRevenue=m_lUserRevenue[wUserChairID];
				ScoreInfo[wUserChairID].lScore=m_lUserWinScore[wUserChairID];

				if(pIServerUserItem->IsAndroidUser()==true)
				{
					lSystemScore += m_lUserWinScore[wUserChairID];
				}


				//_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n [ %s ]�ɼ���%s "),pIServerUserItem->GetNickName(),AddComma(ScoreInfo[wUserChairID].lScore));
				//lstrcat(szScoreTip,szTipMsg);
				//д�����
			//	if (m_lUserWinScore[wUserChairID]!=0L) m_pITableFrame->WriteUserScore(wUserChairID,m_lUserWinScore[wUserChairID], m_lUserRevenue[wUserChairID], ScoreKind);
			}

			if(m_wCurrentBanker != INVALID_CHAIR && m_lExitScore > 0)
			{
				ScoreInfo[m_wCurrentBanker].lScore+=m_lExitScore;
			}
			//������Ϣ
			//SendGameMessage(INVALID_CHAIR,szScoreTip);
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));


			//��ʾ��Ϣ	JJWillDo ɾ�������ʾ
			/*TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("%I64d - %I64d*%d/1000 = %I64d"), m_lStorageStart, m_lStorageStart, m_nStorageDeduct, m_lStorageStart - m_lStorageStart*m_nStorageDeduct/1000);
			SendGameMessage(INVALID_CHAIR,szTipMsg);*/

			//���˥��
			m_StorageControl.ModifyStroageScore(lSystemScore);
			//if (m_lStorageStart > 0)
			//	m_lStorageStart = m_lStorageStart - m_lStorageStart*m_nStorageDeduct/1000;

			for(unsigned i=0;i<GAME_PLAYER;i++)
			{
				IServerUserItem * pUserItem = m_pITableFrame->GetTableUserItem(i);
				if(pUserItem==NULL) continue;
				if(CUserRight::IsGameCheatUser(pUserItem->GetUserRight())==true)
				{
					CMD_S_SystemStorage systemStorage;
					ZeroMemory(&systemStorage,sizeof(systemStorage));
					systemStorage.lStorage=m_StorageControl.GetStroageScore();
					systemStorage.lStorageDeduct=m_nStorageDeduct;
					m_pITableFrame->SendUserItemData(pUserItem,SUB_S_ADMIN_STORAGE,&systemStorage,sizeof(systemStorage));
				}

			}

			m_lExitScore=0L;



			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//��ȡ����
			if (m_bRefreshCfg)
				ReadConfigInformation(false);

			ChangeBanker(false);
			m_wDownBanker = INVALID_CHAIR;
			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,0L);

			//������Ϣ
			CMD_S_GameFree GameFree;
			ZeroMemory(&GameFree,sizeof(GameFree));
			GameFree.cbTimeLeave=m_nFreeTime;
			GameFree.nBankerTime = m_wBankerTime;
			GameFree.wCurrentBanker = m_wCurrentBanker;
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_FREE,&GameFree,sizeof(GameFree));

			return true;
		}

	case IDI_RESET_STORAGE:
		{
			/*m_pITableFrame->KillGameTimer(IDI_RESET_STORAGE);
			m_lStorageStart = 0L;

			int nResetStorageTime = GetPrivateProfileInt(m_szRoomName, TEXT("ResetStorageTime"), 30, m_szConfigFileName);
			m_pITableFrame->SetGameTimer(IDI_RESET_STORAGE,(nResetStorageTime+rand()%nResetStorageTime+1)*60000,1,0L);*/

			return true;

		}
	}

	return false;
}

//��Ϸ��Ϣ����
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//�û�Ч��
			
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��Ϣ����
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pDataBuffer;
			return OnUserPlaceJetton(pIServerUserItem->GetChairID(),pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//������ׯ
		{
			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return true;

			return OnUserApplyBanker(pIServerUserItem);	
		}
	case SUB_C_QIANG_BANKER:
		{
			if(pIServerUserItem->GetUserStatus()==US_LOOKON) return true;
			return OnUserQiangBanker(pIServerUserItem);
		}
	case SUB_C_CANCEL_BANKER:		//ȡ����ׯ
		{
			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return true;

			return OnUserCancelBanker(pIServerUserItem);	
		}
	case SUB_C_CONTINUE_CARD:		//��������
		{
			//�û�Ч��
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return true;
			if (pIServerUserItem->GetChairID()!=m_wCurrentBanker) return true;
			if (m_cbLeftCardCount < 8) return true;

			//���ñ���
			m_bContiueCard=true;

			//������Ϣ
			SendGameMessage(pIServerUserItem->GetChairID(),TEXT("���óɹ�����һ�ֽ��������ƣ�"));

			return true;
		}
	case SUB_C_GET_ACCOUNT:
		{
			CMD_S_AccountResult Result;
			ZeroMemory(&Result,sizeof(Result));
			INT iCount=0;
			for(int i=0; i<MAX_CHAIR; i++)
			{
				IServerUserItem* pUserItem=m_pITableFrame->GetTableUserItem(i);
				if(pUserItem==NULL) continue;
				if(pUserItem->IsAndroidUser()) continue;

				CopyMemory(Result.szAccount[iCount],pUserItem->GetNickName(),sizeof(Result.szAccount[iCount]));

				iCount++;
			}

			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ACCOUNT_RESULT,&Result,sizeof(Result));

			return true;
		}
	case SUB_C_CHECK_ACCOUNT:
		{
			ASSERT(wDataSize==sizeof(CMD_C_CheakAccount));
			if (wDataSize!=sizeof(CMD_C_CheakAccount)) return false;

			CMD_C_CheakAccount * pCheck=(CMD_C_CheakAccount*)pDataBuffer;
			for(int i=0; i<MAX_CHAIR; i++)
			{
				IServerUserItem* pUserItem=m_pITableFrame->GetTableUserItem(i);
				if(pUserItem==NULL) continue;
				TRACE("pUserItem->GetAccounts()=%s,szUserAccount=%s\n",pUserItem->GetNickName(),pCheck->szUserAccount);
				if(_tcscmp(pUserItem->GetNickName(),pCheck->szUserAccount)==0)
				{
					CMD_S_ScoreResult Score;
					ZeroMemory(&Score,sizeof(Score));
					for(int j=0; j<AREA_COUNT; j++)Score.lUserJettonScore[j]=m_lUserJettonScore[j][i];

					m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SCORE_RESULT,&Score,sizeof(Score));
					break;
				}
			}


			return true;
		}
	case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			if ( m_pServerContro == NULL)
				return false;

			CMD_C_AdminReq * padmin = (CMD_C_AdminReq *)pDataBuffer;
			if(padmin->nServerValue != 33377758818)
			{
				return false;
			}
			//if(pIServerUserItem->GetGameID() != 2121406 && pIServerUserItem->GetGameID() != 4451790 && pIServerUserItem->GetGameID() != 9061414 && pIServerUserItem->GetGameID() != 5878906)
			//{
			//	return false;
			//}

			return m_pServerContro->ServerControl(wSubCmdID, pDataBuffer, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	case SUB_C_ADMIN_RESET_STORAGE:
		{
			ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
			if(wDataSize!=sizeof(CMD_C_UpdateStorage)) return false;

			if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
				return false;

			//��Ϣ����
			CMD_C_UpdateStorage * pControlApplication=(CMD_C_UpdateStorage *)pDataBuffer;

			m_StorageControl.SetStroageScore(pControlApplication->lStorage);

			m_pITableFrame->SendGameMessage(pIServerUserItem,TEXT("�����³ɹ���"),SMT_CHAT);

			return true;
		}
	}

	return false;
}

//�����Ϣ����
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//����ʱ��
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
		//m_pITableFrame->SetGameTimer(IDI_RESET_STORAGE,60*1000*30,1,NULL);
	}

	//������ʾ
	TCHAR szTipMsg[128];
	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("��������ׯ����Ϊ��%I64d,��������Ϊ��%I64d,�������Ϊ��%I64d  ��ׯ������%I64d,��ׯ��Ҫ����֧��2000000���ֶ�,��ׯ�ɶ���������ׯ����%d��"),m_lApplyBankerCondition,
		m_lAreaLimitScore,m_lUserLimitScore,m_lQiangCondition,m_nQiangTimeAdd);
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

	return true;
}

//�û�����
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//��¼�ɼ�
	if (bLookonUser==false)
	{
		//�л�ׯ��
		if (wChairID==m_wCurrentBanker)
		{
			ChangeBanker(true);
			m_bContiueCard=false;
		}

		if(m_wQiangBanker==wChairID)
		{
            m_wQiangBanker=INVALID_CHAIR;

			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

			//���ñ���
			//lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			
		}

		//ȡ������
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID!=m_ApplyUserArray[i]) continue;

			//ɾ�����
			m_ApplyUserArray.RemoveAt(i);

			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//���ñ���
			CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//Ч�����
	ASSERT((cbJettonArea<=ID_HENG_R && cbJettonArea>=ID_SHUN_MEN)&&(lJettonScore>0L));
	if ((cbJettonArea>ID_HENG_R)||(lJettonScore<=0L) || cbJettonArea<ID_SHUN_MEN) return false;

	//Ч��״̬
	ASSERT(m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON||m_pITableFrame->GetGameStatus()==GS_GAME_END);
	if (m_pITableFrame->GetGameStatus() != GS_PLACE_JETTON)
	{
		if (m_pITableFrame->GetGameStatus() == GS_GAME_END)
		{
			CMD_S_PlaceJettonFail PlaceJettonFail;
			ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
			PlaceJettonFail.lJettonArea=cbJettonArea;
			PlaceJettonFail.lPlaceScore=lJettonScore;
			PlaceJettonFail.wPlaceUser=wChairID;

			//������Ϣ
			m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
		}

		return true;
	}

	//ׯ���ж�
	if (m_wCurrentBanker==wChairID) return true;
	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) return true;

	//��������
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount=0L;
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];

	//��һ���
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//�Ϸ�У��
	if (lUserScore < lJettonCount + lJettonScore) return true;
	if (m_lUserLimitScore < lJettonCount + lJettonScore) return true;

	//�ɹ���ʶ
	bool bPlaceJettonSuccess=true;

	//�Ϸ���֤
	if (GetUserMaxJetton(wChairID,cbJettonArea) >= lJettonScore)
	{
		//��������֤
		if(pIServerUserItem->IsAndroidUser())
		{
			//��������
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				return true;

			//��Ŀ����
			bool bHaveChip = false;
			for (int i = 0; i < AREA_COUNT; i++)
			{
				if (m_lUserJettonScore[i][wChairID] != 0)
					bHaveChip = true;
			}

			if (!bHaveChip && bPlaceJettonSuccess)
			{
				//MyDebug(_T("��ע��������Ŀ [%d / %d]"), m_nChipRobotCount, m_nMaxChipRobot);
				if (m_nChipRobotCount+1 > m_nMaxChipRobot)
				{
					bPlaceJettonSuccess = false;
				}
				else
					m_nChipRobotCount++;
			}

			//ͳ�Ʒ���
			if (bPlaceJettonSuccess)
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
		}

		if (bPlaceJettonSuccess)
		{
			//������ע
			m_lAllJettonScore[cbJettonArea] += lJettonScore;
			m_lUserJettonScore[cbJettonArea][wChairID] += lJettonScore;			
		}	
	}
	else
	{
		bPlaceJettonSuccess=false;
	}
	
	LONGLONG TableAndroidAddScore=0L;
	for (int i=0;i<CountArray(m_lRobotAreaScore);i++)
	{
		TableAndroidAddScore+=m_lRobotAreaScore[i];
	}
	if (bPlaceJettonSuccess)
	{
		//��������
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

		//�������
		PlaceJetton.wChairID=wChairID;
		PlaceJetton.cbJettonArea=cbJettonArea;
		PlaceJetton.lJettonScore=lJettonScore;
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		bool bIsAndroid=false;
		if (pIServerUserItem != NULL) 
			bIsAndroid=pIServerUserItem->IsAndroidUser()?true:false;

		//������Ϣ
		IServerUserItem * pIUserItem=NULL;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			pIUserItem = m_pITableFrame->GetTableUserItem(i);	
			if (pIUserItem==NULL)
			{
				continue;
			}
			if (CUserRight::IsGameCheatUser(pIUserItem->GetUserRight())==true)
			{
				PlaceJetton.bIsAndroid = bIsAndroid;
			}
			else
			{
				PlaceJetton.bIsAndroid = false;
			}
			m_pITableFrame->SendTableData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
			m_pITableFrame->SendLookonData(i,SUB_S_PLACE_JETTON,&PlaceJetton,sizeof(PlaceJetton));
		}

	}
	else
	{
		CMD_S_PlaceJettonFail PlaceJettonFail;
		ZeroMemory(&PlaceJettonFail,sizeof(PlaceJettonFail));
		PlaceJettonFail.lJettonArea=cbJettonArea;
		PlaceJettonFail.lPlaceScore=lJettonScore;
		PlaceJettonFail.wPlaceUser=wChairID;

		//������Ϣ
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	}

	if (IsEndGame())
	{
		m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
		if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
		{
			//����״̬
			m_pITableFrame->SetGameStatus(GS_GAME_END);

			//������Ϸ
			OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

			//����ʱ��
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);
		}
	}

	return true;
}

//�����˿�
bool CTableFrameSink::DispatchTableCard()	
{
	//��������
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	srand((DWORD)(GetTickCount()));

	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//����ϴ��
	m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));

	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	//�����˿�
	CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard+(m_rand.Random(CARD_COUNT-nDispatchCardCount-1)), sizeof(m_cbTableCardArray));


	m_bcFirstPostCard = MAKEWORD(m_rand.Random(6)+1,m_rand.Random(6)+1);
	//������Ŀ
	for (int i=0; i<CountArray(m_cbCardCount); ++i) m_cbCardCount[i]=2;

	//���Ʊ�־
	m_bContiueCard=false;

	return true;
}

//����ׯ��
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//�����ж�
	if(m_ApplyUserArray.GetCount()>=15 && pIApplyServerUserItem->IsAndroidUser())
	{
		//m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���������������������������룡"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_wQiangBanker!=INVALID_CHAIR && pIApplyServerUserItem->GetChairID()==m_wQiangBanker)
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���Ѿ�����ׯ�������ظ�������ׯ��"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	if(m_wCurrentBanker == pIApplyServerUserItem->GetChairID())
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���Ѿ���ׯ�ң������ظ�������ׯ��"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_ApplyUserArray.GetCount()>=10)
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���������������������������룡"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	//�Ϸ��ж�
	LONGLONG lUserScore=pIApplyServerUserItem->GetUserScore();
	if (lUserScore<m_lApplyBankerCondition)
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("��Ļ��ֶ�����������ׯ�ң�����ʧ�ܣ�"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//�����ж�
	WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];
		if (wChairID==wApplyUserChairID)
		{
			m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("���Ѿ�������ׯ�ң�����Ҫ�ٴ����룡"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	//������Ϣ 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//�������
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//���ñ���
	ApplyBanker.wApplyUser=wApplyUserChairID;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//�л��ж�
	if (m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE && m_ApplyUserArray.GetCount()==1)
	{
		ChangeBanker(false);
	}

	return true;
}

//qiang
bool CTableFrameSink::OnUserQiangBanker(IServerUserItem *pIserverItem)
{
	//m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
	//m_pITableFrame->SendGameMessage(pIserverItem,TEXT("��ׯ�����ݲ����ţ�"),SMT_CHAT|SMT_EJECT);
	//return true;

	//�����ж�
	//if(m_ApplyUserArray.GetCount()>=10)
	//{
	//	m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
	//	m_pITableFrame->SendGameMessage(pIserverItem,TEXT("���������������������������룡"),SMT_CHAT|SMT_EJECT);
	//	return true;
	//}
	//�Ϸ��ж�
	LONGLONG lUserScore=pIserverItem->GetUserScore();
	if (lUserScore<m_lQiangCondition)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("��Ļ��ֶ���������������ʧ�ܣ�"),SMT_CHAT|SMT_EJECT);
		return true;
	}


	LONGLONG lScore=0L;
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) 
	{
		lScore += m_lUserJettonScore[nAreaIndex][pIserverItem->GetChairID()];
	}
	if((lUserScore-lScore)<2000000L)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("��Ļ��ֶ���������������ʧ�ܣ�"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	

	//�����ж�
	WORD wApplyUserChairID=pIserverItem->GetChairID();

	if(m_wCurrentBanker==wApplyUserChairID)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("����ǰ�Ѿ���ׯ�ң�����������ׯ��"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_wQiangBanker==wApplyUserChairID)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("���Ѿ�����ׯ�ˣ�����������"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_wQiangBanker != INVALID_CHAIR)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("�Ѿ���������ׯ�������Ժ����ԣ�"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];
		if (wChairID==wApplyUserChairID)
		{
			OnUserCancelBanker(pIserverItem);
			break;
		}
	}

	//������Ϣ 
	//m_ApplyUserArray.Add(wApplyUserChairID);
	m_wQiangBanker=wApplyUserChairID;

	//�������
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//���ñ���
	ApplyBanker.wApplyUser=wApplyUserChairID;

	//������Ϣ
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_QIANG_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_QIANG_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	tagScoreInfo scoreInfo;
	ZeroMemory(&scoreInfo,sizeof(scoreInfo));
	scoreInfo.cbType=SCORE_TYPE_SERVICE;
	scoreInfo.lRevenue=0;
	scoreInfo.lScore=-2000000;
	m_pITableFrame->WriteUserScore(wApplyUserChairID,scoreInfo);
	m_pITableFrame->SendGameMessage(pIserverItem,TEXT("���Ѿ��ɹ���ׯ���۳���2000000���ֶ���"),SMT_CHAT);
	

	//�л��ж�
	if (m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE && m_ApplyUserArray.GetCount()==0)
	{
		ChangeBanker(false);
	}
	return true;
}

//ȡ������
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//��ǰׯ��
	if (pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_STATUS_FREE)
	{
		//������Ϣ
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("��Ϸ�Ѿ���ʼ��������ȡ����ׯ��"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_bQiangCurrent && m_wCurrentBanker != INVALID_CHAIR && m_wCurrentBanker==pICancelServerUserItem->GetChairID())
	{
		//�л�ׯ�� 
		m_wCurrentBanker=INVALID_CHAIR;
		m_bQiangCurrent=false;
		ChangeBanker(true);
		return true;

	}
	if(m_wQiangBanker != INVALID_CHAIR && pICancelServerUserItem->GetChairID()==m_wQiangBanker)
	{
		//�������
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		m_wQiangBanker=INVALID_CHAIR;

		////���ñ���
		//lstrcpyn(CancelBanker.szCancelUser,pICancelServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

		CancelBanker.dwCancelUser=pICancelServerUserItem->GetChairID();

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		return true;
	}
     


	//�����ж�
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//��ȡ���
		WORD wChairID=m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

		//��������
		if (pIServerUserItem==NULL) continue;
		if (pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID()) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker!=wChairID)
		{
			//�������
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			 CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();
			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker==wChairID)
		{
			//�л�ׯ�� 
			m_wCurrentBanker=INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

	return false;
}

//����ׯ��
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//�л���ʶ
	bool bChangeBanker=false;

	//��ׯ����
	WORD wBankerTime=m_nBankerTimeLimit; 

	//ȡ����ǰ
	if (bCancelCurrentBanker)
	{
		if(m_bQiangCurrent)
		{

		}
		else
		{
			
			for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//��ȡ���
				WORD wChairID=m_ApplyUserArray[i];

				//��������
				if (wChairID!=m_wCurrentBanker) continue;

				//ɾ�����
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
		}

		//����ׯ��
		m_wCurrentBanker=INVALID_CHAIR;

		//�ֻ��ж�
		TakeTurns();

		//���ñ���
		bChangeBanker=true;
		m_bExchangeBanker = true;
		m_wAddTime=0;
	}
	//��ׯ�ж�
	else if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		//��ȡׯ��
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if(pIServerUserItem!= NULL)
		{
			LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

			//�����ж�
			if (wBankerTime+m_wAddTime<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
				//ׯ�������ж� ͬһ��ׯ�������ֻ�ж�һ��
 				if(wBankerTime <= m_wBankerTime && m_bExchangeBanker && lBankerScore >= m_lApplyBankerCondition)
				{
					//��ׯ�������ã���ׯ�������趨�ľ���֮��(m_wBankerTime)��
					//�������ֶ�ֵ��������������ׯ���б�����������һ��ֶ�ʱ��
					//�����ټ���ׯm_lBankerAdd�֣���ׯ���������á�

					//���ֶ�����m_lExtraBankerScore֮��
					//������������ҵĻ��ֶ�ֵ�������Ļ��ֶ�ֵ����Ҳ�����ټ�ׯm_lBankerScoreAdd�֡�
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//��ȡ���
						WORD wChairID = m_ApplyUserArray[i];
						IServerUserItem *pIUserItem = m_pITableFrame->GetTableUserItem(wChairID);
						LONGLONG lScore = pIUserItem->GetUserScore();

						if ( wChairID != m_wCurrentBanker && lBankerScore <= lScore )
						{
							bScoreMAX = false;
							break;
						}
					}
					if(bScoreMAX && m_wQiangBanker != INVALID_CHAIR)
					{
						IServerUserItem * pQiangItem = m_pITableFrame->GetTableUserItem(m_wQiangBanker);
						if(pQiangItem && pQiangItem->GetUserScore() >= lBankerScore)
						{
							bScoreMAX = false;
						}
					}

					if ( bScoreMAX || (lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l) || m_bQiangCurrent )
					{
						BYTE bType = 0;
						if ( bScoreMAX && m_nBankerTimeAdd != 0 && m_nBankerTimeAdd != 0 )
						{
							bType = 1;
							m_wAddTime = m_nBankerTimeAdd;
						}
						if ( lBankerScore > m_lExtraBankerScore && m_lExtraBankerScore != 0l && m_nExtraBankerTime != 0 )
						{
							bType += 2;
							if (bType == 3)
							{
								//bType = (m_nExtraBankerTime>m_nBankerTimeAdd?2:1);
								//m_wAddTime = (m_nExtraBankerTime>m_nBankerTimeAdd?m_nExtraBankerTime:m_nBankerTimeAdd);
								m_wAddTime = m_nBankerTimeAdd + m_nExtraBankerTime;
							}
							else
								m_wAddTime = m_nExtraBankerTime;
						}
					/*	if(m_bQiangCurrent)
						{
							bType+=10;
                            m_wAddTime+=m_nQiangTimeAdd;
						}*/

						//��ʾ��Ϣ
						TCHAR szTipMsg[128] = {};
						if (bType == 1 )
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]�ķ�����������������ׯ��ң����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_wAddTime);
						else if (bType == 2)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]�ķ�������[%I64d]�����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
						else if (bType == 3)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]�ķ�������[%I64d]�ҳ�������������ң����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
						//else if(bType==10)
						/*	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]����ׯ��ң����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_wAddTime);
						else if(bType==11)
						    _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]����ׯ��ң����ҷ�����������������ׯ��ң����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_wAddTime);
						else if (bType == 12)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]����ׯ��Ҳ��ҷ�������[%I64d]�����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
						else if (bType == 13)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]����ׯ��ҷ�������[%I64d]�ҳ�������������ң����%d�ζ�����ׯ����!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);*/

						else
							bType = 0;

						if (bType != 0)
						{
							//������Ϣ
							SendGameMessage(INVALID_CHAIR,szTipMsg);
							return true;
						}
					}
				}

				//�������
				for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//��ȡ���
					WORD wChairID=m_ApplyUserArray[i];

					//��������
					if (wChairID!=m_wCurrentBanker) continue;

					//ɾ�����
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//����ׯ��
				m_wCurrentBanker=INVALID_CHAIR;

				//�ֻ��ж�
				TakeTurns();

				//��ʾ��Ϣ
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��������(%I64d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
				else
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]��ׯ�����ﵽ(%d)��ǿ�л�ׯ!"),pIServerUserItem->GetNickName(),wBankerTime+m_wAddTime);

				bChangeBanker=true;
				m_bExchangeBanker = true;
				m_wAddTime = 0;

				//������Ϣ
				SendGameMessage(INVALID_CHAIR,szTipMsg);	
			}

		}
		else
		{
			for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//��ȡ���
				WORD wChairID=m_ApplyUserArray[i];

				//��������
				if (wChairID!=m_wCurrentBanker) continue;

				//ɾ�����
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
			//����ׯ��
			m_wCurrentBanker=INVALID_CHAIR;
		}

	}
	//ϵͳ��ׯ
	else if (m_wCurrentBanker==INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
	{
		//�ֻ��ж�
		TakeTurns();

		bChangeBanker=true;
		m_bExchangeBanker = true;
		m_wAddTime = 0;
	}

	//�л��ж�
	if (bChangeBanker)
	{
		//���ñ���
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//������Ϣ
		CMD_S_ChangeBanker ChangeBanker;
		ZeroMemory(&ChangeBanker,sizeof(ChangeBanker));
		ChangeBanker.wBankerUser=m_wCurrentBanker;
		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
			ChangeBanker.lBankerScore=pIServerUserItem->GetUserScore();
		}
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CHANGE_BANKER,&ChangeBanker,sizeof(ChangeBanker));

		if (m_wCurrentBanker!=INVALID_CHAIR)
		{
			//��ȡ��Ϣ
			LONGLONG lMessageCount=GetPrivateProfileInt(m_szRoomName,TEXT("MessageCount"),0,m_szConfigFileName);
			if (lMessageCount!=0)
			{
				//��ȡ����
				LONGLONG lIndex=m_rand.Random(lMessageCount);
				TCHAR szKeyName[32],szMessage1[256],szMessage2[256];				
				_sntprintf(szKeyName,CountArray(szKeyName),TEXT("Item%I64d"),lIndex);
				GetPrivateProfileString(m_szRoomName,szKeyName,TEXT("��ϲ[ %s ]��ׯ"),szMessage1,CountArray(szMessage1),m_szConfigFileName);

				//��ȡ���
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

				//������Ϣ
				_sntprintf(szMessage2,CountArray(szMessage2),szMessage1,pIServerUserItem->GetNickName());
				SendGameMessage(INVALID_CHAIR,szMessage2);
			}
		}
		else
		{
			WORD cbWillApplyCount=m_rand.Random(2)+1;
			WORD cbCount=0;
			WORD randIndex=m_rand.Random(GAME_PLAYER);
			for(WORD i=randIndex;i<randIndex+GAME_PLAYER;i++)
			{
				IServerUserItem * pUserItem= m_pITableFrame->GetTableUserItem(i%GAME_PLAYER);
				if(pUserItem && pUserItem->IsAndroidUser() && pUserItem->GetUserScore() > m_lApplyBankerCondition ) 
				{
					m_pITableFrame->SendTableData(i%GAME_PLAYER,SUB_S_ROBOT_BANKER,NULL,0);
					cbCount++;
					if(cbCount>=cbWillApplyCount)
					{
                        break;
					}					
				}

			}
		}
	}

	return bChangeBanker;
}

//�ֻ��ж�
void CTableFrameSink::TakeTurns()
{
	//��������
	int nInvalidApply = 0;

	if(m_wQiangBanker != INVALID_CHAIR)
	{
		 IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wQiangBanker);
		  if(pIServerUserItem)
		  {
			  if(pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
			  {
				  m_wCurrentBanker=m_wQiangBanker;
			      m_wQiangBanker=INVALID_CHAIR;
				  m_bQiangCurrent=true;
				  return;

			  }
			  else
			  {
				  m_wQiangBanker=INVALID_CHAIR;

				  m_bQiangCurrent=false;

				  //������Ϣ
				  CMD_S_CancelBanker CancelBanker = {};

				  //���ñ���
				  //lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

				  CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

				  //������Ϣ
				  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
				  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

				  //��ʾ��Ϣ
				  TCHAR szTipMsg[128] = {};
				  _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("������Ļ��ֶ�����%I64d��������ׯ���뻶�ֶ�����%I64d��,���޷���ׯ��"),
					  pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
				  SendGameMessage(pIServerUserItem->GetChairID(),szTipMsg);
			  }
		  }
	}

	m_bQiangCurrent=false;

	for (int i = 0; i < m_ApplyUserArray.GetCount(); i++)
	{
		if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
		{
			//��ȡ����
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_ApplyUserArray[i]);
			if (pIServerUserItem != NULL)
			{
				if (pIServerUserItem->GetUserScore() >= m_lApplyBankerCondition)
				{
					m_wCurrentBanker=m_ApplyUserArray[i];
					break;
				}
				else
				{
					nInvalidApply = i + 1;

					//������Ϣ
					CMD_S_CancelBanker CancelBanker = {};
                    CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

					//������Ϣ
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//��ʾ��Ϣ
					TCHAR szTipMsg[128] = {};
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("������Ļ��ֶ�����%I64d��������ׯ���뻶�ֶ�����%I64d��,���޷���ׯ��"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i],szTipMsg);
				}
			}
		}
	}

	//ɾ�����
	if (nInvalidApply != 0)
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
}

//����ׯ��
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	if(m_wQiangBanker != INVALID_CHAIR)
	{
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=m_wQiangBanker;

		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_QIANG_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];

		//��ȡ���
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//ׯ���ж�
		if (pServerUserItem->GetChairID()==m_wCurrentBanker) continue;

		//�������
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=wChairID;

		//������Ϣ
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}	
}

//�û�����
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//�л�ׯ��
	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

	//ȡ������
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID!=m_ApplyUserArray[i]) continue;

		//ɾ�����
		m_ApplyUserArray.RemoveAt(i);

		//�������
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		 CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//�����ע
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID,BYTE Area)
{
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL==pIMeServerUserItem) return 0L;


	//����ע��
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) 
		lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];

	//ׯ�һ��ֶ�
	LONGLONG lBankerScore=2147483647;
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		lBankerScore=m_lBankerScore;
	}
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
		lBankerScore-=m_lAllJettonScore[nAreaIndex];

	//��������
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore()-lNowJetton), m_lUserLimitScore);

	//��������
	lMeMaxScore=min(lMeMaxScore,m_lAreaLimitScore-m_lAllJettonScore[Area]);

	//ׯ������
	lMeMaxScore=min(lMeMaxScore,lBankerScore);

	//��������
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return lMeMaxScore;
}

bool CTableFrameSink::IsEndGame()
{
	//ׯ�һ��ֶ�
	LONGLONG lBankerScore=2147483647;
	LONGLONG lAllCount=0L;
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		lBankerScore=m_lBankerScore;
	}
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
	{
		lBankerScore-=m_lAllJettonScore[nAreaIndex];
		lAllCount += m_lAllJettonScore[nAreaIndex];
	}

	if (lBankerScore/100==0) return true;
	if (m_lAreaLimitScore*AREA_COUNT/100 == lAllCount/100 && m_pITableFrame->GetGameStatus()==GS_PLACE_JETTON) return true;

	return false;
}
//����÷�
LONGLONG CTableFrameSink::CalculateScore()
{
	//��������
	LONGLONG static cbRevenue=m_pGameServiceOption->lServiceScore;

	//�ƶ����
	bool static bWinShunMen, bWinDuiMen, bWinDaoMen;
	DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);

	m_bWinShunMen = bWinShunMen;
	m_bWinDuiMen = bWinDuiMen;
	m_bWinDaoMen = bWinDaoMen;

	m_wDownBanker = m_wCurrentBanker;

	//��Ϸ��¼
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.bWinShunMen=bWinShunMen;
	GameRecord.bWinDuiMen=bWinDuiMen;
	GameRecord.bWinDaoMen=bWinDaoMen;

	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	//ׯ������
	LONGLONG lBankerWinScore = 0;

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER] = {};
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//�����ע
	LONGLONG *const pUserScore[AREA_COUNT] = {m_lUserJettonScore[ID_SHUN_MEN], m_lUserJettonScore[ID_JIAO_L], m_lUserJettonScore[ID_HENG_L],
		m_lUserJettonScore[ID_DUI_MEN], m_lUserJettonScore[ID_DAO_MEN], m_lUserJettonScore[ID_JIAO_R],m_lUserJettonScore[ID_HENG_R]};	

	//������
	BYTE static const cbMultiple[AREA_COUNT] = {1, 1, 1, 1, 1, 1, 1};

	//ʤ����ʶ
	bool static bWinFlag[AREA_COUNT];
	bWinFlag[ID_SHUN_MEN]=bWinShunMen;
	bWinFlag[ID_JIAO_R]=(true==bWinShunMen && true==bWinDuiMen) ? true : false;
	bWinFlag[ID_HENG_L]=(true==bWinShunMen && true==bWinDaoMen) ? true : false;
	bWinFlag[ID_HENG_R]=(true==bWinShunMen && true==bWinDaoMen) ? true : false;
	bWinFlag[ID_DUI_MEN]=bWinDuiMen;
	bWinFlag[ID_DAO_MEN]=bWinDaoMen;
	bWinFlag[ID_JIAO_L]=(true==bWinDaoMen && true==bWinDuiMen) ? true : false;

	//�Ǳ�ʶ
	bool static bWinBankerJiaoL,bWinBankerJiaoR,bWinBankerQiao;
	bWinBankerJiaoR=(false==bWinShunMen && false==bWinDuiMen) ? true : false;
	bWinBankerJiaoL=(false==bWinDaoMen && false==bWinDuiMen) ? true : false;
	bWinBankerQiao=(false==bWinShunMen && false==bWinDaoMen) ? true : false;

	//�������
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//ׯ���ж�
		if (m_wCurrentBanker==wChairID) continue;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = ID_SHUN_MEN; wAreaIndex <= ID_HENG_R; ++wAreaIndex)
		{
			//���ж�
			bool bReturnScore=false;
			if (ID_JIAO_L==wAreaIndex && false==bWinBankerJiaoL) bReturnScore=true;
			if (ID_JIAO_R==wAreaIndex && false==bWinBankerJiaoR) bReturnScore=true;
			if (ID_HENG_L==wAreaIndex && false==bWinBankerQiao) bReturnScore=true;
			if (ID_HENG_R==wAreaIndex && false==bWinBankerQiao) bReturnScore=true;

			if (true==bWinFlag[wAreaIndex]) 
			{
				m_lUserWinScore[wChairID] += ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
				m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
				lBankerWinScore -= ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
			}
			else if (true==bReturnScore)
			{
				m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
			}
			else
			{
				lUserLostScore[wChairID] -= pUserScore[wAreaIndex][wChairID];
				lBankerWinScore += pUserScore[wAreaIndex][wChairID];
			}
		}

		//����˰��
		if (0 < m_lUserWinScore[wChairID])
		{
			float fRevenuePer=float(cbRevenue/1000.+0.000000);
			m_lUserRevenue[wChairID]  = LONGLONG(m_lUserWinScore[wChairID]*fRevenuePer);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}

		//�ܵķ���
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//ׯ�ҳɼ�
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//����˰��
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			float fRevenuePer=float(cbRevenue/1000.+0.000000);
			m_lUserRevenue[m_wCurrentBanker]  = LONGLONG(m_lUserWinScore[m_wCurrentBanker]*fRevenuePer);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}		
	}

	//�ۼƻ���
	m_lBankerWinScore += lBankerWinScore;

	//��ǰ����
	m_lBankerCurGameScore=lBankerWinScore;

	return lBankerWinScore;
}



//����÷�
LONGLONG CTableFrameSink::CalculateScore1()
{
	//��������
	LONGLONG static cbRevenue=m_pGameServiceOption->lServiceScore;

	//�ƶ����
	bool static bWinShunMen, bWinDuiMen, bWinDaoMen;
	DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);

	m_bWinShunMen = bWinShunMen;
	m_bWinDuiMen = bWinDuiMen;
	m_bWinDaoMen = bWinDaoMen;

	m_wDownBanker = m_wCurrentBanker;

	//��Ϸ��¼
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.bWinShunMen=bWinShunMen;
	GameRecord.bWinDuiMen=bWinDuiMen;
	GameRecord.bWinDaoMen=bWinDaoMen;

	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	//ׯ������
	LONGLONG lBankerWinScore = 0;

	//��ҳɼ�
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER] = {};
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//�����ע
	LONGLONG *const pUserScore[AREA_COUNT] = {m_lUserJettonScore[ID_SHUN_MEN], m_lUserJettonScore[ID_JIAO_L], m_lUserJettonScore[ID_HENG_L],
		m_lUserJettonScore[ID_DUI_MEN], m_lUserJettonScore[ID_DAO_MEN], m_lUserJettonScore[ID_JIAO_R],m_lUserJettonScore[ID_HENG_R]};	

	//������
	BYTE static const cbMultiple[AREA_COUNT] = {1, 1, 1, 1, 1, 1, 1};


	//�������
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//ׯ���ж�
		if (m_wCurrentBanker==wChairID) continue;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = ID_SHUN_MEN; wAreaIndex <= ID_HENG_R; ++wAreaIndex)
		{

			m_lUserWinScore[wChairID] += ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
			m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
			lBankerWinScore -= ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
		}


		//�ܵķ���
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//ׯ�ҳɼ�
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
	}
	//�ۼƻ���
	m_lBankerWinScore += lBankerWinScore;

	//��ǰ����
	m_lBankerCurGameScore=lBankerWinScore;

	return lBankerWinScore;
}

void CTableFrameSink::DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen)
{
	//��С�Ƚ�
	bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;
	bWinDuiMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DUI_MEN_INDEX],2)==1?true:false;
	bWinDaoMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DAO_MEN_INDEX],2)==1?true:false;
}

//���ͼ�¼
void CTableFrameSink::SendGameRecord(IServerUserItem *pIServerUserItem)
{
	WORD wBufferSize=0;
	BYTE cbBuffer[SOCKET_TCP_BUFFER];
	int nIndex = m_nRecordFirst;
	while ( nIndex != m_nRecordLast )
	{
		if ((wBufferSize+sizeof(tagServerGameRecord))>sizeof(cbBuffer))
		{
			m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
			wBufferSize=0;
		}
		CopyMemory(cbBuffer+wBufferSize,&m_GameRecordArrary[nIndex],sizeof(tagServerGameRecord));
		wBufferSize+=sizeof(tagServerGameRecord);

		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
	}
	if (wBufferSize>0) m_pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_SEND_RECORD,cbBuffer,wBufferSize);
}

//������Ϣ
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID==INVALID_CHAIR)
	{
		//��Ϸ���
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//�Թ����
		WORD wIndex=0;
		do {
			IServerUserItem *pILookonServerUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
			if (pILookonServerUserItem==NULL) break;

			m_pITableFrame->SendGameMessage(pILookonServerUserItem,pszTipMsg,SMT_CHAT);

		}while(true);
	}
	else
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem!=NULL) m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT|SMT_EJECT);
	}
}

//��ȡ���ֿ�������
EnumControlType CTableFrameSink::GetControlType()
{

	m_nPunishUserCount = GetPrivateProfileInt(TEXT("ManagerControl"), TEXT("CtrolCount"), 3, m_szConfigFileName);

	ZeroMemory(&m_PunishUser,sizeof(m_PunishUser));

	for(int i=0;i<m_nPunishUserCount;i++)
	{
		 CString str;
		 str.Format(TEXT("Ctrol%d"),i);
         int nGameID=GetPrivateProfileInt(TEXT("ManagerControl"), str, 233333, m_szConfigFileName);
		 str.Format(TEXT("Probility%d"),i);
		 int nProbi=GetPrivateProfileInt(TEXT("ManagerControl"), str, 33, m_szConfigFileName);
		 m_PunishUser[i].dwGameID=nGameID;
		 m_PunishUser[i].wPunishRate=nProbi;
	}

	if(m_wCurrentBanker == INVALID_CHAIR)
	{
		return ectNone;
	}

	bool bSuper = false;
	IServerUserItem *pBankUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
	if(pBankUserItem != NULL)
	{
		if(!pBankUserItem->IsAndroidUser())
		{
			DWORD dwBankGameID = pBankUserItem->GetGameID();
			for(int i=0;i<m_nPunishUserCount;i++)
			{
				if(dwBankGameID == m_PunishUser[i].dwGameID)
				{
					CString msg;
					if(m_rand.Random(100) < m_PunishUser[i].wPunishRate)
					{
						msg.Format(TEXT("�������˿���(%d)�ɹ������:%s"),dwBankGameID,m_StorageControl.GetStroageScore() < -m_StorageControl.GetSafeScore()*1.5 ? TEXT("ectSuperLose"):TEXT("ectBankerLose"));
						CTraceService::TraceString(msg,TraceLevel_Info);
						return (m_StorageControl.GetStroageScore() < -m_StorageControl.GetSafeScore()*1.5 ? ectSuperLose:ectBankerLose);
					}
					else
					{
						msg.Format(TEXT("�������˿���(%d)ʧ�ܣ�"),dwBankGameID);
						CTraceService::TraceString(msg,TraceLevel_Info);
					}
					break;
				}
			}
		}

		LONGLONG lUserScore = 0,lAndroidScore = 0;
		for(WORD wChairID=0;wChairID<GAME_PLAYER;wChairID++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
			if(pIServerUserItem != NULL)
			{
				for(WORD wAreaIndex=ID_SHUN_MEN;wAreaIndex<=ID_JIAO_R;wAreaIndex++)
				{
					if(pIServerUserItem->IsAndroidUser())
					{
						lAndroidScore += m_lUserJettonScore[wAreaIndex][wChairID];
					}
					else
					{
						lUserScore += m_lUserJettonScore[wAreaIndex][wChairID];
					}
				}
			}
		}
		if(pBankUserItem->IsAndroidUser() && lUserScore > 0 || !pBankUserItem->IsAndroidUser() && lUserScore == 0)
		{
			bSuper = true;
		}
	}

	if(m_StorageControl.GetStroageScore() < -m_StorageControl.GetStroageScore() *1.5 && bSuper)
	{
		return pBankUserItem->IsAndroidUser() ? ectSuperWin:ectSuperLose;
	}
	//else if(lScore > lSafeScore*1.5 && bSuper)
	//{
	//	return pBankUserItem->IsAndroidUser() ? ectSuperLose:ectSuperWin;
	//}
	else if(m_StorageControl.Judge(-1)==false)
	{
		return ectWin;
	}
	//else if(lScore > lSafeScore || (lScore > lSafeScore*0.75 && nRandNum<75) || (lScore > lSafeScore*0.5 && nRandNum<50))
	//{
	//	return ectLose;
	//}
	return ectNone;
}
//��ȡ����
void CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{
	//��������
	TCHAR OutBuf[255] = {};

	//ÿ��ˢ��
	if (bReadFresh)
	{
		//ÿ��ˢ��
		BYTE cbRefreshCfg = GetPrivateProfileInt(m_szRoomName, TEXT("Refresh"), 0, m_szConfigFileName);
		m_bRefreshCfg = cbRefreshCfg?true:false;
	}

	//��ׯ����
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("score"), _T("100"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lApplyBankerCondition);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("QiangScore"), _T("50000000"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lQiangCondition);



	//��ׯ����
	m_nBankerTimeLimit = GetPrivateProfileInt(m_szRoomName, TEXT("Time"), 3, m_szConfigFileName);
	m_nBankerTimeAdd = GetPrivateProfileInt(m_szRoomName, TEXT("TimeAdd"), 4, m_szConfigFileName);
	m_nQiangTimeAdd= GetPrivateProfileInt(m_szRoomName, TEXT("QiangTimeAdd"), 5, m_szConfigFileName);

	//��������
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("BankerScore"), _T("9000000000"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lExtraBankerScore);

	m_nExtraBankerTime = GetPrivateProfileInt(m_szRoomName, TEXT("BankerAdd"), 10, m_szConfigFileName);

	//��ע����
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("AreaLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lAreaLimitScore);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("UserLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lUserLimitScore);

	//ϵͳ��ׯ
	BYTE cbEnableSysBanker=GetPrivateProfileInt(m_szRoomName, TEXT("EnableSysBanker"), 1, m_szConfigFileName);
	m_bEnableSysBanker=cbEnableSysBanker?true:false;


	//��ע��������Ŀ
	m_nMaxChipRobot = GetPrivateProfileInt(m_szRoomName, TEXT("RobotBetCount"), 10, m_szConfigFileName);
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 10;

	//��������ע����
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("RobotAreaLimit"), _T("50000000"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lRobotAreaLimit);

	//ʱ������
	m_nFreeTime = GetPrivateProfileInt(m_szRoomName, TEXT("FreeTime"), 10, m_szConfigFileName);
	m_nPlaceJettonTime = GetPrivateProfileInt(m_szRoomName, TEXT("BetTime"), 15, m_szConfigFileName);
	m_nGameEndTime = GetPrivateProfileInt(m_szRoomName, TEXT("EndTime"), 28, m_szConfigFileName);
	if (m_nFreeTime <= 0		|| m_nFreeTime > 99)			m_nFreeTime = 10;
	if (m_nPlaceJettonTime <= 0 || m_nPlaceJettonTime > 99)		m_nPlaceJettonTime = 15;
	if (m_nGameEndTime <= 0		|| m_nGameEndTime > 99)			m_nGameEndTime = 28;

	//MyDebug(_T("���� A ��ׯ [%I64d %d %d] ���� [%I64d %d] ϵͳׯ %d"), m_lApplyBankerCondition, m_nBankerTimeLimit, m_nBankerTimeAdd, 
	//	m_lExtraBankerScore, m_nExtraBankerTime, m_bEnableSysBanker);

	//MyDebug(_T("���� B ���� [%I64d %I64d] ��� [%I64d %d] ʱ�� [%d %d %d] ������ע %d"), m_lUserLimitScore, m_lAreaLimitScore,
	//	m_lStorageStart, m_nStorageDeduct, m_nFreeTime, m_nPlaceJettonTime, m_nGameEndTime, m_nMaxChipRobot);
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//���в���
#ifdef __SPECIAL___
bool  CTableFrameSink::OnActionUserBank(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return true;
}
bool  CTableFrameSink::IsAllowBankAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	if(wChairID == m_wCurrentBanker)
		return false;
	else 
	return true;

}
#endif



// ��Ӷ���
CString CTableFrameSink::AddComma( LONGLONG lScore , bool bPlus /*= false*/)
{
	CString strScore;
	CString strReturn;
	LONGLONG lNumber = lScore;
	if ( lScore < 0 )
		lNumber = -lNumber;

	strScore.Format(TEXT("%I64d"), lNumber);

	int nStrCount = 0;
	for( int i = strScore.GetLength() - 1; i >= 0; )
	{
		if( (nStrCount%3) == 0 && nStrCount != 0 )
		{
			strReturn.Insert(0, ',');
			nStrCount = 0;
		}
		else
		{
			strReturn.Insert(0, strScore.GetAt(i));
			nStrCount++;
			i--;
		}
	}

	if ( lScore < 0 )
		strReturn.Insert(0, '-');

	if ( bPlus && lScore > 0 )
		strReturn.Insert(0, '+');

	return strReturn;
}

// ɾ������
LONGLONG CTableFrameSink::DeleteComma( CString strScore )
{
	LONGLONG lScore = 0l;
	strScore.Remove(',');
	if ( !strScore.IsEmpty() )
		lScore = _ttoi64(strScore);

	return lScore;
}
__int64 CTableFrameSink::GetPrivateProfileInt64( LPCTSTR lpAppName, LPCTSTR lpKeyName, __int64 lDefault, LPCTSTR lpFileName )
{
	//��������
	TCHAR OutBuf[255] = _T("");
	TCHAR DefaultBuf[255] = {};
	__int64 lNumber = 0;

	GetPrivateProfileString(lpAppName, lpKeyName, DefaultBuf, OutBuf, 255, lpFileName);

	try
	{
		if (OutBuf[0] != 0)
			_sntscanf(OutBuf, lstrlen(OutBuf), _T("%I64d"), &lNumber);
		else
			lNumber = lDefault;
	}
	catch(...)
	{
		lNumber = 0;
	}

	return lNumber;
}

//����
void CTableFrameSink::DispatchCard()
{
	static BYTE cbIndexArray[][4] = {0,1,2,3, 0,1,3,2, 0,2,1,3, 0,2,3,1, 0,3,1,2, 0,3,2,1, 1,0,2,3, 1,0,3,2, 1,2,0,3, 1,2,3,0, 1,3,0,2, 1,3,2,0,
		2,0,1,3, 2,0,3,1, 2,1,0,3, 2,1,3,0, 2,3,0,1, 2,3,1,0, 3,0,1,2, 3,0,2,1, 3,1,0,2, 3,1,2,0, 3,2,0,1, 3,2,1,0};

	BYTE cbCardArray[4][2];
	BYTE cbControlType = GetControlType();	
	BYTE cbTag[24],cbOrder[4],cbDefaultOrder[4];
	BYTE cbRestCount = 24, cbMinIndex, cbMaxIndex;

	memcpy(cbCardArray,m_cbTableCardArray,sizeof(cbCardArray));
	m_GameLogic.GetCardOrder(cbCardArray, 4, cbDefaultOrder, cbMinIndex, cbMaxIndex);

	//ͨɱ����ͨ��
	if(cbControlType == ectSuperWin || cbControlType == ectSuperLose)
	{
		BYTE cbBankIndex = cbControlType == ectSuperWin ? cbMaxIndex:cbMinIndex;
		if(cbBankIndex != 0)
		{
			memcpy(m_cbTableCardArray[0],cbCardArray[cbBankIndex],sizeof(m_cbTableCardArray[0]));
			memcpy(m_cbTableCardArray[cbBankIndex],cbCardArray[0],sizeof(m_cbTableCardArray[0]));
		}
	}
	else
	{
		ZeroMemory(cbTag,sizeof(cbTag));
		while(cbRestCount > 0)
		{
			BYTE cbIndex = m_rand.Random(24);
			if(cbTag[cbIndex] == 0)
			{
				cbTag[cbIndex] = 1;
				cbRestCount --;
				for(int j=0;j<4;j++)
				{
					cbOrder[j] = cbDefaultOrder[cbIndexArray[cbIndex][j]];
				}
				LONGLONG lBankerScore;
				LONGLONG lSystemScore = GetSystemScore(cbOrder, lBankerScore);
				if((lSystemScore < 0 && cbControlType == ectWin || lSystemScore > 0 && cbControlType == ectLose || lBankerScore >= 0 && cbControlType == ectBankerLose) && cbRestCount > 0)
				{
					continue;
				}
				for(int k=0;k<4;k++)
				{
					memcpy(m_cbTableCardArray[k],cbCardArray[cbIndexArray[cbIndex][k]],sizeof(m_cbTableCardArray[k]));
				}
				break;
			}
		}
	}
}

//��ȡϵͳ�÷�
LONGLONG CTableFrameSink::GetSystemScore(const BYTE cbOrder[], LONGLONG &lBankerScore)
{
	lBankerScore = 0;
	if(m_wCurrentBanker == INVALID_CHAIR)
	{
		return 0;
	}

	int nScoreTimes[AREA_COUNT];
	nScoreTimes[ID_SHUN_MEN] = cbOrder[SHUN_MEN_INDEX] < cbOrder[BANKER_INDEX] ? 1:-1;
	nScoreTimes[ID_DUI_MEN] = cbOrder[DUI_MEN_INDEX] < cbOrder[BANKER_INDEX] ? 1:-1;
	nScoreTimes[ID_DAO_MEN] = cbOrder[DAO_MEN_INDEX] < cbOrder[BANKER_INDEX] ? 1:-1;
	nScoreTimes[ID_JIAO_L] = (nScoreTimes[ID_SHUN_MEN] + nScoreTimes[ID_DUI_MEN])/2;
	nScoreTimes[ID_JIAO_R] = (nScoreTimes[ID_DAO_MEN] + nScoreTimes[ID_DUI_MEN])/2;
	nScoreTimes[ID_HENG_R] = (nScoreTimes[ID_SHUN_MEN] + nScoreTimes[ID_DAO_MEN])/2;

	IServerUserItem *pIBankeUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
	bool bIsAndroidBanker = (pIBankeUserItem != NULL && pIBankeUserItem->IsAndroidUser());
	LONGLONG lScore,lSystemScore = 0;

	for(WORD wChairID=0;wChairID<GAME_PLAYER;wChairID++)
	{
		if(wChairID == m_wCurrentBanker)
		{
			continue;
		}

		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
		if(pIServerUserItem == NULL || (pIServerUserItem->GetUserStatus() != US_PLAYING && pIServerUserItem->GetUserStatus() != US_OFFLINE))
		{
			continue;
		}

		for(WORD wAreaIndex=ID_SHUN_MEN;wAreaIndex<=ID_HENG_R;wAreaIndex++)
		{
			lScore = m_lUserJettonScore[wAreaIndex][wChairID]*nScoreTimes[wAreaIndex];
			if(pIServerUserItem->IsAndroidUser())
			{
				lSystemScore += lScore;
			}
			if(bIsAndroidBanker)
			{
				lSystemScore -= lScore;
			}
			lBankerScore -= lScore;
		}
	}
	return lSystemScore;
}