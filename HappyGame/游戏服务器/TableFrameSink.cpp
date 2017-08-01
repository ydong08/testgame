#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define SEND_COUNT					300									//发送次数

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//时间定义
#define IDI_FREE					1									//空闲时间
#define IDI_PLACE_JETTON			2									//下注时间
#define IDI_GAME_END				3									//结束时间
//定时清理库存
#define IDI_RESET_STORAGE           4                                   //清理库存

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;				//游戏人数

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//总下注数
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//控制变量
	m_cbWinSideControl=0;
	m_nSendCardCount=0;

	//玩家成绩	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//扑克信息
	ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));

	//状态变量
	m_dwJettonTime=0L;
	m_bExchangeBanker=true;
	m_wAddTime=0;

	//庄家信息
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

	//记录变量
	m_nRecordFirst=0;
	m_nRecordLast=0;
	m_dwRecordCount=0;
	ZeroMemory(m_GameRecordArrary,sizeof(m_GameRecordArrary));

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//服务控制
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

//析构函数
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

//接口查询
void *  CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
    QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	//控制接口
//	m_pITableFrameControl=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrameControl);
//	if (m_pITableFrameControl==NULL) return false;
	m_pITableFrame->SetStartMode(START_MODE_TIME_CONTROL);

	//设置文件名
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);
	_sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\SupremeConfig.ini"),szPath);

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	_sntprintf(m_szRoomName,sizeof(m_szRoomName),TEXT("%d"),m_pGameServiceOption->wServerID);

	//变量定义
	TCHAR OutBuf[255] = {};
	////库存设置
	//m_lStorageStart = GetPrivateProfileInt(m_szRoomName, TEXT("StorageStart"), 0, m_szConfigFileName);
	//m_nStorageDeduct = GetPrivateProfileInt(m_szRoomName, TEXT("StorageDeduct"), 0, m_szConfigFileName);

	//库存设置
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

//复位桌子
VOID  CTableFrameSink::RepositionSink()
{
	//总下注数
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));
	
	//个人下注
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//玩家成绩	
	ZeroMemory(m_lUserWinScore,sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore,sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue,sizeof(m_lUserRevenue));

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));


	m_bcFirstPostCard = MAKEWORD(1,1);
	return;
}



//游戏状态
bool  CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	return true;
}

//查询限额
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

//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;

		if(wChairID==m_wCurrentBanker) return true;

		if(wChairID==i)
		{
			//返回下注
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

//游戏开始
bool  CTableFrameSink::OnEventGameStart()
{
	m_StorageControl.ResetCondition();

	//变量定义
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart,sizeof(GameStart));

	//获取庄家
	IServerUserItem *pIBankerServerUserItem=NULL;
	if (INVALID_CHAIR!=m_wCurrentBanker) 
	{
		pIBankerServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
		m_lBankerScore=pIBankerServerUserItem->GetUserScore();
	}

	//设置变量
	GameStart.cbTimeLeave=m_nPlaceJettonTime;
	GameStart.wBankerUser=m_wCurrentBanker;
	if (pIBankerServerUserItem!=NULL) 
		GameStart.lBankerScore=m_lBankerScore;
	GameStart.bContiueCard=m_bContiueCard;

	//下注机器人数量
	int nChipRobotCount = 0;
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
			nChipRobotCount++;
	}

	nChipRobotCount = min(nChipRobotCount, m_nMaxChipRobot);

	//MyDebug(_T("机器人数量 %d 下注机器人上限 %d"), nChipRobotCount, m_nMaxChipRobot);

	//机器人控制
	m_nChipRobotCount = 0;
	ZeroMemory(m_lRobotAreaScore, sizeof(m_lRobotAreaScore));

	//旁观玩家
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));	

	//游戏玩家
	for (WORD wChairID=0; wChairID<GAME_PLAYER; ++wChairID)
	{
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//设置积分
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

//游戏结束
bool  CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束	
		{
			//游戏作弊
			if( m_pServerContro != NULL && m_pServerContro->NeedControl() )
			{
				//获取控制目标牌型
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



			//计算分数
			LONGLONG lBankerWinScore=CalculateScore();

			//递增次数
			m_wBankerTime++;

			//结束消息
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			m_lBankerWinScore+=m_lExitScore;
			lBankerWinScore+=m_lExitScore;

			//庄家信息
			GameEnd.nBankerTime = m_wBankerTime;
			GameEnd.lBankerTotallScore=m_lBankerWinScore;
			GameEnd.lBankerScore=lBankerWinScore;
			GameEnd.bcFirstCard = m_bcFirstPostCard;
			GameEnd.wCurrentBanker = m_wCurrentBanker;

			//扑克信息
			CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			GameEnd.cbLeftCardCount=m_cbLeftCardCount;

			//发送积分
			GameEnd.cbTimeLeave=m_nGameEndTime;	
			for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
				if ( pIServerUserItem == NULL ) continue;

				//设置成绩
				GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

				//返还积分
				GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

				//设置税收
				if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
				else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
				else GameEnd.lRevenue=0;

				if(m_wCurrentBanker != INVALID_CHAIR && m_lExitScore > 0L && wUserIndex== m_wCurrentBanker)
				{ 
				      GameEnd.lUserScore=m_lUserWinScore[wUserIndex]+m_lExitScore;
				}

				//发送消息					
				m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			return true;
		}
	case GER_USER_LEAVE:		//用户离开
	case GER_NETWORK_ERROR:
		{
			//闲家判断
			if (m_wCurrentBanker!=wChairID)
			{
				//变量定义
				LONGLONG lScore=0;
				LONGLONG lRevenue=0;
			

				//统计成绩
				for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lScore -= m_lUserJettonScore[nAreaIndex][wChairID];

				LONGLONG lExitScore=0L;

				//写入积分
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

							////游戏玩家
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
						str.Format(TEXT("闲家 %s强退！"),pIServerUserItem->GetNickName());
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

					
					//写入积分
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

			//状态判断
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//提示消息
				TCHAR szTipMsg[128];
				_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于庄家[ %s ]强退，系统提前发牌，按通赔结算！"),pIServerUserItem->GetNickName());

				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	

				//设置状态
				m_pITableFrame->SetGameStatus(GS_GAME_END);

				//设置时间
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);

				////游戏作弊
				//if( m_pServerContro != NULL && m_pServerContro->NeedControl() )
				//{
				//	//获取控制目标牌型
				//	m_pServerContro->GetSuitResult(m_cbTableCardArray, m_cbTableCard, m_lAllJettonScore);
				//	m_pServerContro->CompleteControl();
				//}

				//计算分数
				CalculateScore1();

				//结束消息
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));

				//庄家信息
				GameEnd.nBankerTime = m_wBankerTime;
				GameEnd.lBankerTotallScore=m_lBankerWinScore;
				GameEnd.lBankerScore=m_lUserWinScore[m_wCurrentBanker];
				GameEnd.wCurrentBanker = m_wCurrentBanker;

				//扑克信息
				CopyMemory(GameEnd.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
				GameEnd.cbLeftCardCount=m_cbLeftCardCount;

				//发送积分
				GameEnd.cbTimeLeave=m_nGameEndTime;	
				for ( WORD wUserIndex = 0; wUserIndex < GAME_PLAYER; ++wUserIndex )
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wUserIndex);
					if ( pIServerUserItem == NULL ) continue;

					//设置成绩
					GameEnd.lUserScore=m_lUserWinScore[wUserIndex];

					//返还积分
					GameEnd.lUserReturnScore=m_lUserReturnScore[wUserIndex];

					//设置税收
					if (m_lUserRevenue[wUserIndex]>0) GameEnd.lRevenue=m_lUserRevenue[wUserIndex];
					else if (m_wCurrentBanker!=INVALID_CHAIR) GameEnd.lRevenue=m_lUserRevenue[m_wCurrentBanker];
					else GameEnd.lRevenue=0;

					//发送消息					
					m_pITableFrame->SendTableData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					m_pITableFrame->SendLookonData(wUserIndex,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				}
			}

			//扣除分数
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

			//切换庄家
			ChangeBanker(true);

			return true;
		}
	}

	return false;
}

//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}
//发送场景
bool  CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:			//空闲状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);

			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));			

			//控制信息
			StatusFree.lApplyBankerCondition = m_lApplyBankerCondition;
			StatusFree.lAreaLimitScore = m_lAreaLimitScore;
			StatusFree.lQiangConition=m_lQiangCondition;
			StatusFree.lQiangScore=2000000;

			//庄家信息
			StatusFree.bEnableSysBanker=m_bEnableSysBanker;
			StatusFree.wBankerUser=m_wCurrentBanker;	
			StatusFree.cbBankerTime=m_wBankerTime;
			StatusFree.lBankerWinScore=m_lBankerWinScore;
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);
				StatusFree.lBankerScore=pIServerUserItem->GetUserScore();
			}

			//玩家信息
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				StatusFree.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore*4); 
			}

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			StatusFree.cbTimeLeave=(BYTE)(m_nFreeTime-__min(dwPassTime,(DWORD)m_nFreeTime));

			StatusFree.dwServerID=m_pGameServiceOption->wServerID;

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//限制提示
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d  抢庄条件：%I64d,抢庄需要额外支付2000000欢乐豆"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore,m_lQiangCondition);
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
						
			//发送申请者
			SendApplyUser(pIServerUserItem);

			return bSuccess;
		}
	case GS_PLACE_JETTON:		//游戏状态
	case GS_GAME_END:			//结束状态
		{
			//发送记录
			SendGameRecord(pIServerUserItem);		

			//构造数据
			CMD_S_StatusPlay StatusPlay={0};

			//全局下注
			CopyMemory(StatusPlay.lAllJettonScore,m_lAllJettonScore,sizeof(StatusPlay.lAllJettonScore));

			//玩家下注
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
			{
				for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
					StatusPlay.lUserJettonScore[nAreaIndex] = m_lUserJettonScore[nAreaIndex][wChiarID];

				//最大下注
				StatusPlay.lUserMaxScore=min(pIServerUserItem->GetUserScore(),m_lUserLimitScore);
			}

			//控制信息
			StatusPlay.lApplyBankerCondition=m_lApplyBankerCondition;		
			StatusPlay.lAreaLimitScore=m_lAreaLimitScore;	

			StatusPlay.lQiangConition=m_lQiangCondition;
			StatusPlay.lQiangScore=2000000;

			//庄家信息
			StatusPlay.bEnableSysBanker=m_bEnableSysBanker;
			StatusPlay.wBankerUser=m_wCurrentBanker;			
			StatusPlay.cbBankerTime=m_wBankerTime;
			StatusPlay.lBankerWinScore=m_lBankerWinScore;	
			if (m_wCurrentBanker!=INVALID_CHAIR)
			{
				StatusPlay.lBankerScore=m_lBankerScore;
			}	

			//全局信息
			DWORD dwPassTime=(DWORD)time(NULL)-m_dwJettonTime;
			int	nTotalTime = (cbGameStatus==GS_PLACE_JETTON?m_nPlaceJettonTime:m_nGameEndTime);
			StatusPlay.cbTimeLeave=(BYTE)(nTotalTime-__min(dwPassTime,(DWORD)nTotalTime));
			StatusPlay.cbGameStatus=m_pITableFrame->GetGameStatus();						

			//结束判断
			if (cbGameStatus==GS_GAME_END)
			{
				//设置成绩
				StatusPlay.lEndUserScore=m_lUserWinScore[wChiarID];

				//返还积分
				StatusPlay.lEndUserReturnScore=m_lUserReturnScore[wChiarID];

				//设置税收
				if (m_lUserRevenue[wChiarID]>0) StatusPlay.lEndRevenue=m_lUserRevenue[wChiarID];
				else if (m_wCurrentBanker!=INVALID_CHAIR) StatusPlay.lEndRevenue=m_lUserRevenue[m_wCurrentBanker];
				else StatusPlay.lEndRevenue=0;

				//庄家成绩
				StatusPlay.lEndBankerScore=m_lBankerCurGameScore;

				//扑克信息
				CopyMemory(StatusPlay.cbTableCardArray,m_cbTableCardArray,sizeof(m_cbTableCardArray));
			}


			//房间名称
			StatusPlay.dwServerID=m_pGameServiceOption->wServerID;

			//发送场景
			bool bSuccess = m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
			TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d  抢庄条件：%I64d,抢庄需要额外支付2000000欢乐豆"),m_lApplyBankerCondition,
				m_lAreaLimitScore,m_lUserLimitScore,m_lQiangCondition);
			m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);
			
			//发送申请者
			SendApplyUser( pIServerUserItem );

			return bSuccess;
		}
	}

	return false;
}

//定时器事件
bool  CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	switch (wTimerID)
	{
	case IDI_FREE:		//空闲时间
		{
			//开始游戏
			m_pITableFrame->StartGame();

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_PLACE_JETTON,m_nPlaceJettonTime*1000,1,0L);

			//派发扑克
			DispatchTableCard();

			//设置状态
			m_pITableFrame->SetGameStatus(GS_PLACE_JETTON);

			return true;
		}
	case IDI_PLACE_JETTON:		//下注时间
		{
			//状态判断(防止强退重复设置)
			if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
			{
				//设置状态
				m_pITableFrame->SetGameStatus(GS_GAME_END);			

				//结束游戏
				OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

				//设置时间
				m_dwJettonTime=(DWORD)time(NULL);
				m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);			
			}

			return true;
		}
	case IDI_GAME_END:			//结束游戏
		{
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			TCHAR szTipMsg[256] = {};
			TCHAR szScoreTip[2048] = {};

			LONGLONG lSystemScore = 0;
			//写入积分
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


				//_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("\n [ %s ]成绩：%s "),pIServerUserItem->GetNickName(),AddComma(ScoreInfo[wUserChairID].lScore));
				//lstrcat(szScoreTip,szTipMsg);
				//写入积分
			//	if (m_lUserWinScore[wUserChairID]!=0L) m_pITableFrame->WriteUserScore(wUserChairID,m_lUserWinScore[wUserChairID], m_lUserRevenue[wUserChairID], ScoreKind);
			}

			if(m_wCurrentBanker != INVALID_CHAIR && m_lExitScore > 0)
			{
				ScoreInfo[m_wCurrentBanker].lScore+=m_lExitScore;
			}
			//发送消息
			//SendGameMessage(INVALID_CHAIR,szScoreTip);
			m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));


			//提示消息	JJWillDo 删除库存提示
			/*TCHAR szTipMsg[128];
			_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("%I64d - %I64d*%d/1000 = %I64d"), m_lStorageStart, m_lStorageStart, m_nStorageDeduct, m_lStorageStart - m_lStorageStart*m_nStorageDeduct/1000);
			SendGameMessage(INVALID_CHAIR,szTipMsg);*/

			//库存衰减
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



			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//读取配置
			if (m_bRefreshCfg)
				ReadConfigInformation(false);

			ChangeBanker(false);
			m_wDownBanker = INVALID_CHAIR;
			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,0L);

			//发送消息
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

//游戏消息处理
bool  CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	
	switch (wSubCmdID)
	{
	case SUB_C_PLACE_JETTON:		//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_PlaceJetton));
			if (wDataSize!=sizeof(CMD_C_PlaceJetton)) return false;

			//用户效验
			
			if (pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//消息处理
			CMD_C_PlaceJetton * pPlaceJetton=(CMD_C_PlaceJetton *)pDataBuffer;
			return OnUserPlaceJetton(pIServerUserItem->GetChairID(),pPlaceJetton->cbJettonArea,pPlaceJetton->lJettonScore);
		}
	case SUB_C_APPLY_BANKER:		//申请做庄
		{
			//用户效验
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return true;

			return OnUserApplyBanker(pIServerUserItem);	
		}
	case SUB_C_QIANG_BANKER:
		{
			if(pIServerUserItem->GetUserStatus()==US_LOOKON) return true;
			return OnUserQiangBanker(pIServerUserItem);
		}
	case SUB_C_CANCEL_BANKER:		//取消做庄
		{
			//用户效验
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return true;

			return OnUserCancelBanker(pIServerUserItem);	
		}
	case SUB_C_CONTINUE_CARD:		//继续发牌
		{
			//用户效验
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return true;
			if (pIServerUserItem->GetChairID()!=m_wCurrentBanker) return true;
			if (m_cbLeftCardCount < 8) return true;

			//设置变量
			m_bContiueCard=true;

			//发送消息
			SendGameMessage(pIServerUserItem->GetChairID(),TEXT("设置成功，下一局将继续发牌！"));

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

			//消息处理
			CMD_C_UpdateStorage * pControlApplication=(CMD_C_UpdateStorage *)pDataBuffer;

			m_StorageControl.SetStroageScore(pControlApplication->lStorage);

			m_pITableFrame->SendGameMessage(pIServerUserItem,TEXT("库存更新成功！"),SMT_CHAT);

			return true;
		}
	}

	return false;
}

//框架消息处理
bool  CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户坐下
bool  CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//设置时间
	if ((bLookonUser==false)&&(m_dwJettonTime==0L))
	{
		m_dwJettonTime=(DWORD)time(NULL);
		m_pITableFrame->SetGameTimer(IDI_FREE,m_nFreeTime*1000,1,NULL);
		m_pITableFrame->SetGameStatus(GAME_STATUS_FREE);
		//m_pITableFrame->SetGameTimer(IDI_RESET_STORAGE,60*1000*30,1,NULL);
	}

	//限制提示
	TCHAR szTipMsg[128];
	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("本房间上庄条件为：%I64d,区域限制为：%I64d,玩家限制为：%I64d  抢庄条件：%I64d,抢庄需要额外支付2000000欢乐豆,抢庄可额外增加坐庄次数%d次"),m_lApplyBankerCondition,
		m_lAreaLimitScore,m_lUserLimitScore,m_lQiangCondition,m_nQiangTimeAdd);
	m_pITableFrame->SendGameMessage(pIServerUserItem,szTipMsg,SMT_CHAT);

	return true;
}

//用户起来
bool  CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//记录成绩
	if (bLookonUser==false)
	{
		//切换庄家
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

			//设置变量
			//lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			
		}

		//取消申请
		for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
		{
			if (wChairID!=m_ApplyUserArray[i]) continue;

			//删除玩家
			m_ApplyUserArray.RemoveAt(i);

			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			//设置变量
			CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

			break;
		}

		return true;
	}

	return true;
}

//加注事件
bool CTableFrameSink::OnUserPlaceJetton(WORD wChairID, BYTE cbJettonArea, LONGLONG lJettonScore)
{
	//效验参数
	ASSERT((cbJettonArea<=ID_HENG_R && cbJettonArea>=ID_SHUN_MEN)&&(lJettonScore>0L));
	if ((cbJettonArea>ID_HENG_R)||(lJettonScore<=0L) || cbJettonArea<ID_SHUN_MEN) return false;

	//效验状态
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

			//发送消息
			m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
		}

		return true;
	}

	//庄家判断
	if (m_wCurrentBanker==wChairID) return true;
	if (m_bEnableSysBanker==false && m_wCurrentBanker==INVALID_CHAIR) return true;

	//变量定义
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	LONGLONG lJettonCount=0L;
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lJettonCount += m_lUserJettonScore[nAreaIndex][wChairID];

	//玩家积分
	LONGLONG lUserScore = pIServerUserItem->GetUserScore();

	//合法校验
	if (lUserScore < lJettonCount + lJettonScore) return true;
	if (m_lUserLimitScore < lJettonCount + lJettonScore) return true;

	//成功标识
	bool bPlaceJettonSuccess=true;

	//合法验证
	if (GetUserMaxJetton(wChairID,cbJettonArea) >= lJettonScore)
	{
		//机器人验证
		if(pIServerUserItem->IsAndroidUser())
		{
			//区域限制
			if (m_lRobotAreaScore[cbJettonArea] + lJettonScore > m_lRobotAreaLimit)
				return true;

			//数目限制
			bool bHaveChip = false;
			for (int i = 0; i < AREA_COUNT; i++)
			{
				if (m_lUserJettonScore[i][wChairID] != 0)
					bHaveChip = true;
			}

			if (!bHaveChip && bPlaceJettonSuccess)
			{
				//MyDebug(_T("下注机器人数目 [%d / %d]"), m_nChipRobotCount, m_nMaxChipRobot);
				if (m_nChipRobotCount+1 > m_nMaxChipRobot)
				{
					bPlaceJettonSuccess = false;
				}
				else
					m_nChipRobotCount++;
			}

			//统计分数
			if (bPlaceJettonSuccess)
				m_lRobotAreaScore[cbJettonArea] += lJettonScore;
		}

		if (bPlaceJettonSuccess)
		{
			//保存下注
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
		//变量定义
		CMD_S_PlaceJetton PlaceJetton;
		ZeroMemory(&PlaceJetton,sizeof(PlaceJetton));

		//构造变量
		PlaceJetton.wChairID=wChairID;
		PlaceJetton.cbJettonArea=cbJettonArea;
		PlaceJetton.lJettonScore=lJettonScore;
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		bool bIsAndroid=false;
		if (pIServerUserItem != NULL) 
			bIsAndroid=pIServerUserItem->IsAndroidUser()?true:false;

		//发送消息
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

		//发送消息
		m_pITableFrame->SendTableData(wChairID,SUB_S_PLACE_JETTON_FAIL,&PlaceJettonFail,sizeof(PlaceJettonFail));
	}

	if (IsEndGame())
	{
		m_pITableFrame->KillGameTimer(IDI_PLACE_JETTON);
		if (m_pITableFrame->GetGameStatus()!=GS_GAME_END)
		{
			//设置状态
			m_pITableFrame->SetGameStatus(GS_GAME_END);

			//结束游戏
			OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);

			//设置时间
			m_dwJettonTime=(DWORD)time(NULL);
			m_pITableFrame->SetGameTimer(IDI_GAME_END,m_nGameEndTime*1000,1,0L);
		}
	}

	return true;
}

//发送扑克
bool CTableFrameSink::DispatchTableCard()	
{
	//继续发牌
	int const static nDispatchCardCount=8;
	bool bContiueCard=m_bContiueCard;
	srand((DWORD)(GetTickCount()));

	if (m_cbLeftCardCount<nDispatchCardCount) bContiueCard=false;

	//重新洗牌
	m_GameLogic.RandCardList(m_cbTableCard,CountArray(m_cbTableCard));

	ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	//设置扑克
	CopyMemory(&m_cbTableCardArray[0][0], m_cbTableCard+(m_rand.Random(CARD_COUNT-nDispatchCardCount-1)), sizeof(m_cbTableCardArray));


	m_bcFirstPostCard = MAKEWORD(m_rand.Random(6)+1,m_rand.Random(6)+1);
	//设置数目
	for (int i=0; i<CountArray(m_cbCardCount); ++i) m_cbCardCount[i]=2;

	//发牌标志
	m_bContiueCard=false;

	return true;
}

//申请庄家
bool CTableFrameSink::OnUserApplyBanker(IServerUserItem *pIApplyServerUserItem)
{
	//数量判断
	if(m_ApplyUserArray.GetCount()>=15 && pIApplyServerUserItem->IsAndroidUser())
	{
		//m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("申请玩家数量已满，请过会再申请！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_wQiangBanker!=INVALID_CHAIR && pIApplyServerUserItem->GetChairID()==m_wQiangBanker)
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("您已经抢过庄，无需重复申请上庄！"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	if(m_wCurrentBanker == pIApplyServerUserItem->GetChairID())
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("您已经是庄家，无需重复申请上庄！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_ApplyUserArray.GetCount()>=10)
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("申请玩家数量已满，请过会再申请！"),SMT_CHAT|SMT_EJECT);
		return true;
	}
	//合法判断
	LONGLONG lUserScore=pIApplyServerUserItem->GetUserScore();
	if (lUserScore<m_lApplyBankerCondition)
	{
		m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("你的欢乐豆不足以申请庄家，申请失败！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	//存在判断
	WORD wApplyUserChairID=pIApplyServerUserItem->GetChairID();
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];
		if (wChairID==wApplyUserChairID)
		{
			m_pITableFrame->SendTableData(pIApplyServerUserItem->GetChairID(),SUB_S_APPLY_FAILURE);
			m_pITableFrame->SendGameMessage(pIApplyServerUserItem,TEXT("你已经申请了庄家，不需要再次申请！"),SMT_CHAT|SMT_EJECT);
			return true;
		}
	}

	//保存信息 
	m_ApplyUserArray.Add(wApplyUserChairID);

	//构造变量
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//设置变量
	ApplyBanker.wApplyUser=wApplyUserChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	//切换判断
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
	//m_pITableFrame->SendGameMessage(pIserverItem,TEXT("抢庄功能暂不开放！"),SMT_CHAT|SMT_EJECT);
	//return true;

	//数量判断
	//if(m_ApplyUserArray.GetCount()>=10)
	//{
	//	m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
	//	m_pITableFrame->SendGameMessage(pIserverItem,TEXT("申请玩家数量已满，请过会再申请！"),SMT_CHAT|SMT_EJECT);
	//	return true;
	//}
	//合法判断
	LONGLONG lUserScore=pIserverItem->GetUserScore();
	if (lUserScore<m_lQiangCondition)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("你的欢乐豆不足以抢，申请失败！"),SMT_CHAT|SMT_EJECT);
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
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("你的欢乐豆不足以抢，申请失败！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	

	//存在判断
	WORD wApplyUserChairID=pIserverItem->GetChairID();

	if(m_wCurrentBanker==wApplyUserChairID)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("您当前已经是庄家，不能申请上庄！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_wQiangBanker==wApplyUserChairID)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("您已经抢过庄了，无需再抢！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_wQiangBanker != INVALID_CHAIR)
	{
		m_pITableFrame->SendTableData(pIserverItem->GetChairID(),SUB_S_APPLY_FAILURE);
		m_pITableFrame->SendGameMessage(pIserverItem,TEXT("已经有人抢过庄，请你稍后再试！"),SMT_CHAT|SMT_EJECT);
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

	//保存信息 
	//m_ApplyUserArray.Add(wApplyUserChairID);
	m_wQiangBanker=wApplyUserChairID;

	//构造变量
	CMD_S_ApplyBanker ApplyBanker;
	ZeroMemory(&ApplyBanker,sizeof(ApplyBanker));

	//设置变量
	ApplyBanker.wApplyUser=wApplyUserChairID;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_QIANG_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_QIANG_BANKER, &ApplyBanker, sizeof(ApplyBanker));

	tagScoreInfo scoreInfo;
	ZeroMemory(&scoreInfo,sizeof(scoreInfo));
	scoreInfo.cbType=SCORE_TYPE_SERVICE;
	scoreInfo.lRevenue=0;
	scoreInfo.lScore=-2000000;
	m_pITableFrame->WriteUserScore(wApplyUserChairID,scoreInfo);
	m_pITableFrame->SendGameMessage(pIserverItem,TEXT("您已经成功抢庄，扣除您2000000欢乐豆！"),SMT_CHAT);
	

	//切换判断
	if (m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE && m_ApplyUserArray.GetCount()==0)
	{
		ChangeBanker(false);
	}
	return true;
}

//取消申请
bool CTableFrameSink::OnUserCancelBanker(IServerUserItem *pICancelServerUserItem)
{
	//当前庄家
	if (pICancelServerUserItem->GetChairID()==m_wCurrentBanker && m_pITableFrame->GetGameStatus()!=GAME_STATUS_FREE)
	{
		//发送消息
		m_pITableFrame->SendGameMessage(pICancelServerUserItem,TEXT("游戏已经开始，不可以取消当庄！"),SMT_CHAT|SMT_EJECT);
		return true;
	}

	if(m_bQiangCurrent && m_wCurrentBanker != INVALID_CHAIR && m_wCurrentBanker==pICancelServerUserItem->GetChairID())
	{
		//切换庄家 
		m_wCurrentBanker=INVALID_CHAIR;
		m_bQiangCurrent=false;
		ChangeBanker(true);
		return true;

	}
	if(m_wQiangBanker != INVALID_CHAIR && pICancelServerUserItem->GetChairID()==m_wQiangBanker)
	{
		//构造变量
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		m_wQiangBanker=INVALID_CHAIR;

		////设置变量
		//lstrcpyn(CancelBanker.szCancelUser,pICancelServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

		CancelBanker.dwCancelUser=pICancelServerUserItem->GetChairID();

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		return true;
	}
     


	//存在判断
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		//获取玩家
		WORD wChairID=m_ApplyUserArray[i];
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

		//条件过滤
		if (pIServerUserItem==NULL) continue;
		if (pIServerUserItem->GetUserID()!=pICancelServerUserItem->GetUserID()) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		if (m_wCurrentBanker!=wChairID)
		{
			//构造变量
			CMD_S_CancelBanker CancelBanker;
			ZeroMemory(&CancelBanker,sizeof(CancelBanker));

			 CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();
			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		}
		else if (m_wCurrentBanker==wChairID)
		{
			//切换庄家 
			m_wCurrentBanker=INVALID_CHAIR;
			ChangeBanker(true);
		}

		return true;
	}

	return false;
}

//更换庄家
bool CTableFrameSink::ChangeBanker(bool bCancelCurrentBanker)
{
	//切换标识
	bool bChangeBanker=false;

	//做庄次数
	WORD wBankerTime=m_nBankerTimeLimit; 

	//取消当前
	if (bCancelCurrentBanker)
	{
		if(m_bQiangCurrent)
		{

		}
		else
		{
			
			for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//获取玩家
				WORD wChairID=m_ApplyUserArray[i];

				//条件过滤
				if (wChairID!=m_wCurrentBanker) continue;

				//删除玩家
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
		}

		//设置庄家
		m_wCurrentBanker=INVALID_CHAIR;

		//轮换判断
		TakeTurns();

		//设置变量
		bChangeBanker=true;
		m_bExchangeBanker = true;
		m_wAddTime=0;
	}
	//轮庄判断
	else if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		//获取庄家
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

		if(pIServerUserItem!= NULL)
		{
			LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

			//次数判断
			if (wBankerTime+m_wAddTime<=m_wBankerTime || lBankerScore<m_lApplyBankerCondition)
			{
				//庄家增加判断 同一个庄家情况下只判断一次
 				if(wBankerTime <= m_wBankerTime && m_bExchangeBanker && lBankerScore >= m_lApplyBankerCondition)
				{
					//加庄局数设置：当庄家坐满设定的局数之后(m_wBankerTime)，
					//所带欢乐豆值还超过下面申请庄家列表里面所有玩家欢乐豆时，
					//可以再加坐庄m_lBankerAdd局，加庄局数可设置。

					//欢乐豆超过m_lExtraBankerScore之后，
					//就算是下面玩家的欢乐豆值大于他的欢乐豆值，他也可以再加庄m_lBankerScoreAdd局。
					bool bScoreMAX = true;
					m_bExchangeBanker = false;

					for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
					{
						//获取玩家
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

						//提示消息
						TCHAR szTipMsg[128] = {};
						if (bType == 1 )
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]的分数超过其他申请上庄玩家，获得%d次额外坐庄次数!"),pIServerUserItem->GetNickName(),m_wAddTime);
						else if (bType == 2)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]的分数超过[%I64d]，获得%d次额外坐庄次数!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
						else if (bType == 3)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]的分数超过[%I64d]且超过其他申请玩家，获得%d次额外坐庄次数!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
						//else if(bType==10)
						/*	_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]是抢庄玩家，获得%d次额外坐庄次数!"),pIServerUserItem->GetNickName(),m_wAddTime);
						else if(bType==11)
						    _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]是抢庄玩家，并且分数超过其他申请上庄玩家，获得%d次额外坐庄次数!"),pIServerUserItem->GetNickName(),m_wAddTime);
						else if (bType == 12)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]是抢庄玩家并且分数超过[%I64d]，获得%d次额外坐庄次数!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);
						else if (bType == 13)
							_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]是抢庄玩家分数超过[%I64d]且超过其他申请玩家，获得%d次额外坐庄次数!"),pIServerUserItem->GetNickName(),m_lExtraBankerScore,m_wAddTime);*/

						else
							bType = 0;

						if (bType != 0)
						{
							//发送消息
							SendGameMessage(INVALID_CHAIR,szTipMsg);
							return true;
						}
					}
				}

				//撤销玩家
				for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
				{
					//获取玩家
					WORD wChairID=m_ApplyUserArray[i];

					//条件过滤
					if (wChairID!=m_wCurrentBanker) continue;

					//删除玩家
					m_ApplyUserArray.RemoveAt(i);

					break;
				}

				//设置庄家
				m_wCurrentBanker=INVALID_CHAIR;

				//轮换判断
				TakeTurns();

				//提示消息
				TCHAR szTipMsg[128];
				if (lBankerScore<m_lApplyBankerCondition)
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]分数少于(%I64d)，强行换庄!"),pIServerUserItem->GetNickName(),m_lApplyBankerCondition);
				else
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("[ %s ]做庄次数达到(%d)，强行换庄!"),pIServerUserItem->GetNickName(),wBankerTime+m_wAddTime);

				bChangeBanker=true;
				m_bExchangeBanker = true;
				m_wAddTime = 0;

				//发送消息
				SendGameMessage(INVALID_CHAIR,szTipMsg);	
			}

		}
		else
		{
			for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
			{
				//获取玩家
				WORD wChairID=m_ApplyUserArray[i];

				//条件过滤
				if (wChairID!=m_wCurrentBanker) continue;

				//删除玩家
				m_ApplyUserArray.RemoveAt(i);

				break;
			}
			//设置庄家
			m_wCurrentBanker=INVALID_CHAIR;
		}

	}
	//系统做庄
	else if (m_wCurrentBanker==INVALID_CHAIR && m_ApplyUserArray.GetCount()!=0)
	{
		//轮换判断
		TakeTurns();

		bChangeBanker=true;
		m_bExchangeBanker = true;
		m_wAddTime = 0;
	}

	//切换判断
	if (bChangeBanker)
	{
		//设置变量
		m_wBankerTime = 0;
		m_lBankerWinScore=0;

		//发送消息
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
			//读取消息
			LONGLONG lMessageCount=GetPrivateProfileInt(m_szRoomName,TEXT("MessageCount"),0,m_szConfigFileName);
			if (lMessageCount!=0)
			{
				//读取配置
				LONGLONG lIndex=m_rand.Random(lMessageCount);
				TCHAR szKeyName[32],szMessage1[256],szMessage2[256];				
				_sntprintf(szKeyName,CountArray(szKeyName),TEXT("Item%I64d"),lIndex);
				GetPrivateProfileString(m_szRoomName,szKeyName,TEXT("恭喜[ %s ]上庄"),szMessage1,CountArray(szMessage1),m_szConfigFileName);

				//获取玩家
				IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wCurrentBanker);

				//发送消息
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

//轮换判断
void CTableFrameSink::TakeTurns()
{
	//变量定义
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

				  //发送消息
				  CMD_S_CancelBanker CancelBanker = {};

				  //设置变量
				  //lstrcpyn(CancelBanker.szCancelUser,pIServerUserItem->GetNickName(),CountArray(CancelBanker.szCancelUser));

				  CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

				  //发送消息
				  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
				  m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

				  //提示消息
				  TCHAR szTipMsg[128] = {};
				  _sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于你的欢乐豆数（%I64d）少于坐庄必须欢乐豆数（%I64d）,你无法上庄！"),
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
			//获取分数
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

					//发送消息
					CMD_S_CancelBanker CancelBanker = {};
                    CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

					//发送消息
					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

					//提示消息
					TCHAR szTipMsg[128] = {};
					_sntprintf(szTipMsg,CountArray(szTipMsg),TEXT("由于你的欢乐豆数（%I64d）少于坐庄必须欢乐豆数（%I64d）,你无法上庄！"),
						pIServerUserItem->GetUserScore(), m_lApplyBankerCondition);
					SendGameMessage(m_ApplyUserArray[i],szTipMsg);
				}
			}
		}
	}

	//删除玩家
	if (nInvalidApply != 0)
		m_ApplyUserArray.RemoveAt(0, nInvalidApply);
}

//发送庄家
void CTableFrameSink::SendApplyUser( IServerUserItem *pRcvServerUserItem )
{
	if(m_wQiangBanker != INVALID_CHAIR)
	{
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=m_wQiangBanker;

		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_QIANG_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}
	for (INT_PTR nUserIdx=0; nUserIdx<m_ApplyUserArray.GetCount(); ++nUserIdx)
	{
		WORD wChairID=m_ApplyUserArray[nUserIdx];

		//获取玩家
		IServerUserItem *pServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (!pServerUserItem) continue;

		//庄家判断
		if (pServerUserItem->GetChairID()==m_wCurrentBanker) continue;

		//构造变量
		CMD_S_ApplyBanker ApplyBanker;
		ApplyBanker.wApplyUser=wChairID;

		//发送消息
		m_pITableFrame->SendUserItemData(pRcvServerUserItem, SUB_S_APPLY_BANKER, &ApplyBanker, sizeof(ApplyBanker));
	}	
}

//用户断线
bool  CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem) 
{
	//切换庄家
	if (wChairID==m_wCurrentBanker) ChangeBanker(true);

	//取消申请
	for (WORD i=0; i<m_ApplyUserArray.GetCount(); ++i)
	{
		if (wChairID!=m_ApplyUserArray[i]) continue;

		//删除玩家
		m_ApplyUserArray.RemoveAt(i);

		//构造变量
		CMD_S_CancelBanker CancelBanker;
		ZeroMemory(&CancelBanker,sizeof(CancelBanker));

		 CancelBanker.dwCancelUser=pIServerUserItem->GetChairID();

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CANCEL_BANKER, &CancelBanker, sizeof(CancelBanker));

		break;
	}

	return true;
}

//最大下注
LONGLONG CTableFrameSink::GetUserMaxJetton(WORD wChairID,BYTE Area)
{
	IServerUserItem *pIMeServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (NULL==pIMeServerUserItem) return 0L;


	//已下注额
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) 
		lNowJetton += m_lUserJettonScore[nAreaIndex][wChairID];

	//庄家欢乐豆
	LONGLONG lBankerScore=2147483647;
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		lBankerScore=m_lBankerScore;
	}
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
		lBankerScore-=m_lAllJettonScore[nAreaIndex];

	//个人限制
	LONGLONG lMeMaxScore = min((pIMeServerUserItem->GetUserScore()-lNowJetton), m_lUserLimitScore);

	//区域限制
	lMeMaxScore=min(lMeMaxScore,m_lAreaLimitScore-m_lAllJettonScore[Area]);

	//庄家限制
	lMeMaxScore=min(lMeMaxScore,lBankerScore);

	//非零限制
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return lMeMaxScore;
}

bool CTableFrameSink::IsEndGame()
{
	//庄家欢乐豆
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
//计算得分
LONGLONG CTableFrameSink::CalculateScore()
{
	//变量定义
	LONGLONG static cbRevenue=m_pGameServiceOption->lServiceScore;

	//推断玩家
	bool static bWinShunMen, bWinDuiMen, bWinDaoMen;
	DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);

	m_bWinShunMen = bWinShunMen;
	m_bWinDuiMen = bWinDuiMen;
	m_bWinDaoMen = bWinDaoMen;

	m_wDownBanker = m_wCurrentBanker;

	//游戏记录
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.bWinShunMen=bWinShunMen;
	GameRecord.bWinDuiMen=bWinDuiMen;
	GameRecord.bWinDaoMen=bWinDaoMen;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	//庄家总量
	LONGLONG lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER] = {};
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//玩家下注
	LONGLONG *const pUserScore[AREA_COUNT] = {m_lUserJettonScore[ID_SHUN_MEN], m_lUserJettonScore[ID_JIAO_L], m_lUserJettonScore[ID_HENG_L],
		m_lUserJettonScore[ID_DUI_MEN], m_lUserJettonScore[ID_DAO_MEN], m_lUserJettonScore[ID_JIAO_R],m_lUserJettonScore[ID_HENG_R]};	

	//区域倍率
	BYTE static const cbMultiple[AREA_COUNT] = {1, 1, 1, 1, 1, 1, 1};

	//胜利标识
	bool static bWinFlag[AREA_COUNT];
	bWinFlag[ID_SHUN_MEN]=bWinShunMen;
	bWinFlag[ID_JIAO_R]=(true==bWinShunMen && true==bWinDuiMen) ? true : false;
	bWinFlag[ID_HENG_L]=(true==bWinShunMen && true==bWinDaoMen) ? true : false;
	bWinFlag[ID_HENG_R]=(true==bWinShunMen && true==bWinDaoMen) ? true : false;
	bWinFlag[ID_DUI_MEN]=bWinDuiMen;
	bWinFlag[ID_DAO_MEN]=bWinDaoMen;
	bWinFlag[ID_JIAO_L]=(true==bWinDaoMen && true==bWinDuiMen) ? true : false;

	//角标识
	bool static bWinBankerJiaoL,bWinBankerJiaoR,bWinBankerQiao;
	bWinBankerJiaoR=(false==bWinShunMen && false==bWinDuiMen) ? true : false;
	bWinBankerJiaoL=(false==bWinDaoMen && false==bWinDuiMen) ? true : false;
	bWinBankerQiao=(false==bWinShunMen && false==bWinDaoMen) ? true : false;

	//计算积分
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//庄家判断
		if (m_wCurrentBanker==wChairID) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = ID_SHUN_MEN; wAreaIndex <= ID_HENG_R; ++wAreaIndex)
		{
			//角判断
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

		//计算税收
		if (0 < m_lUserWinScore[wChairID])
		{
			float fRevenuePer=float(cbRevenue/1000.+0.000000);
			m_lUserRevenue[wChairID]  = LONGLONG(m_lUserWinScore[wChairID]*fRevenuePer);
			m_lUserWinScore[wChairID] -= m_lUserRevenue[wChairID];
		}

		//总的分数
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//庄家成绩
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;

		//计算税收
		if (0 < m_lUserWinScore[m_wCurrentBanker])
		{
			float fRevenuePer=float(cbRevenue/1000.+0.000000);
			m_lUserRevenue[m_wCurrentBanker]  = LONGLONG(m_lUserWinScore[m_wCurrentBanker]*fRevenuePer);
			m_lUserWinScore[m_wCurrentBanker] -= m_lUserRevenue[m_wCurrentBanker];
			lBankerWinScore = m_lUserWinScore[m_wCurrentBanker];
		}		
	}

	//累计积分
	m_lBankerWinScore += lBankerWinScore;

	//当前积分
	m_lBankerCurGameScore=lBankerWinScore;

	return lBankerWinScore;
}



//计算得分
LONGLONG CTableFrameSink::CalculateScore1()
{
	//变量定义
	LONGLONG static cbRevenue=m_pGameServiceOption->lServiceScore;

	//推断玩家
	bool static bWinShunMen, bWinDuiMen, bWinDaoMen;
	DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);

	m_bWinShunMen = bWinShunMen;
	m_bWinDuiMen = bWinDuiMen;
	m_bWinDaoMen = bWinDaoMen;

	m_wDownBanker = m_wCurrentBanker;

	//游戏记录
	tagServerGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.bWinShunMen=bWinShunMen;
	GameRecord.bWinDuiMen=bWinDuiMen;
	GameRecord.bWinDaoMen=bWinDaoMen;

	//移动下标
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst ) m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;

	//庄家总量
	LONGLONG lBankerWinScore = 0;

	//玩家成绩
	ZeroMemory(m_lUserWinScore, sizeof(m_lUserWinScore));
	ZeroMemory(m_lUserReturnScore, sizeof(m_lUserReturnScore));
	ZeroMemory(m_lUserRevenue, sizeof(m_lUserRevenue));
	LONGLONG lUserLostScore[GAME_PLAYER] = {};
	ZeroMemory(lUserLostScore, sizeof(lUserLostScore));

	//玩家下注
	LONGLONG *const pUserScore[AREA_COUNT] = {m_lUserJettonScore[ID_SHUN_MEN], m_lUserJettonScore[ID_JIAO_L], m_lUserJettonScore[ID_HENG_L],
		m_lUserJettonScore[ID_DUI_MEN], m_lUserJettonScore[ID_DAO_MEN], m_lUserJettonScore[ID_JIAO_R],m_lUserJettonScore[ID_HENG_R]};	

	//区域倍率
	BYTE static const cbMultiple[AREA_COUNT] = {1, 1, 1, 1, 1, 1, 1};


	//计算积分
	for (WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
	{
		//庄家判断
		if (m_wCurrentBanker==wChairID) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		for (WORD wAreaIndex = ID_SHUN_MEN; wAreaIndex <= ID_HENG_R; ++wAreaIndex)
		{

			m_lUserWinScore[wChairID] += ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
			m_lUserReturnScore[wChairID] += pUserScore[wAreaIndex][wChairID] ;
			lBankerWinScore -= ( pUserScore[wAreaIndex][wChairID] * cbMultiple[wAreaIndex] ) ;
		}


		//总的分数
		m_lUserWinScore[wChairID] += lUserLostScore[wChairID];
	}

	//庄家成绩
	if (m_wCurrentBanker!=INVALID_CHAIR)
	{
		m_lUserWinScore[m_wCurrentBanker] = lBankerWinScore;
	}
	//累计积分
	m_lBankerWinScore += lBankerWinScore;

	//当前积分
	m_lBankerCurGameScore=lBankerWinScore;

	return lBankerWinScore;
}

void CTableFrameSink::DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen)
{
	//大小比较
	bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;
	bWinDuiMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DUI_MEN_INDEX],2)==1?true:false;
	bWinDaoMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DAO_MEN_INDEX],2)==1?true:false;
}

//发送记录
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

//发送消息
void CTableFrameSink::SendGameMessage(WORD wChairID, LPCTSTR pszTipMsg)
{
	if (wChairID==INVALID_CHAIR)
	{
		//游戏玩家
		for (WORD i=0; i<GAME_PLAYER; ++i)
		{
			IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			m_pITableFrame->SendGameMessage(pIServerUserItem,pszTipMsg,SMT_CHAT);
		}

		//旁观玩家
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

//获取本局控制类型
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
						msg.Format(TEXT("触发个人控制(%d)成功，结果:%s"),dwBankGameID,m_StorageControl.GetStroageScore() < -m_StorageControl.GetSafeScore()*1.5 ? TEXT("ectSuperLose"):TEXT("ectBankerLose"));
						CTraceService::TraceString(msg,TraceLevel_Info);
						return (m_StorageControl.GetStroageScore() < -m_StorageControl.GetSafeScore()*1.5 ? ectSuperLose:ectBankerLose);
					}
					else
					{
						msg.Format(TEXT("触发个人控制(%d)失败！"),dwBankGameID);
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
//读取配置
void CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{
	//变量定义
	TCHAR OutBuf[255] = {};

	//每盘刷新
	if (bReadFresh)
	{
		//每盘刷新
		BYTE cbRefreshCfg = GetPrivateProfileInt(m_szRoomName, TEXT("Refresh"), 0, m_szConfigFileName);
		m_bRefreshCfg = cbRefreshCfg?true:false;
	}

	//上庄条件
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("score"), _T("100"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lApplyBankerCondition);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("QiangScore"), _T("50000000"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lQiangCondition);



	//做庄次数
	m_nBankerTimeLimit = GetPrivateProfileInt(m_szRoomName, TEXT("Time"), 3, m_szConfigFileName);
	m_nBankerTimeAdd = GetPrivateProfileInt(m_szRoomName, TEXT("TimeAdd"), 4, m_szConfigFileName);
	m_nQiangTimeAdd= GetPrivateProfileInt(m_szRoomName, TEXT("QiangTimeAdd"), 5, m_szConfigFileName);

	//额外条件
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("BankerScore"), _T("9000000000"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lExtraBankerScore);

	m_nExtraBankerTime = GetPrivateProfileInt(m_szRoomName, TEXT("BankerAdd"), 10, m_szConfigFileName);

	//下注限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("AreaLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lAreaLimitScore);

	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("UserLimitScore"), _T("2147483647"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lUserLimitScore);

	//系统坐庄
	BYTE cbEnableSysBanker=GetPrivateProfileInt(m_szRoomName, TEXT("EnableSysBanker"), 1, m_szConfigFileName);
	m_bEnableSysBanker=cbEnableSysBanker?true:false;


	//下注机器人数目
	m_nMaxChipRobot = GetPrivateProfileInt(m_szRoomName, TEXT("RobotBetCount"), 10, m_szConfigFileName);
	if (m_nMaxChipRobot < 0)	m_nMaxChipRobot = 10;

	//机器人下注限制
	ZeroMemory(OutBuf, sizeof(OutBuf));
	GetPrivateProfileString(m_szRoomName, TEXT("RobotAreaLimit"), _T("50000000"), OutBuf, 255, m_szConfigFileName);
	_sntscanf(OutBuf, _tcslen(OutBuf), _T("%I64d"), &m_lRobotAreaLimit);

	//时间配置
	m_nFreeTime = GetPrivateProfileInt(m_szRoomName, TEXT("FreeTime"), 10, m_szConfigFileName);
	m_nPlaceJettonTime = GetPrivateProfileInt(m_szRoomName, TEXT("BetTime"), 15, m_szConfigFileName);
	m_nGameEndTime = GetPrivateProfileInt(m_szRoomName, TEXT("EndTime"), 28, m_szConfigFileName);
	if (m_nFreeTime <= 0		|| m_nFreeTime > 99)			m_nFreeTime = 10;
	if (m_nPlaceJettonTime <= 0 || m_nPlaceJettonTime > 99)		m_nPlaceJettonTime = 15;
	if (m_nGameEndTime <= 0		|| m_nGameEndTime > 99)			m_nGameEndTime = 28;

	//MyDebug(_T("配置 A 坐庄 [%I64d %d %d] 额外 [%I64d %d] 系统庄 %d"), m_lApplyBankerCondition, m_nBankerTimeLimit, m_nBankerTimeAdd, 
	//	m_lExtraBankerScore, m_nExtraBankerTime, m_bEnableSysBanker);

	//MyDebug(_T("配置 B 限制 [%I64d %I64d] 库存 [%I64d %d] 时间 [%d %d %d] 机人下注 %d"), m_lUserLimitScore, m_lAreaLimitScore,
	//	m_lStorageStart, m_nStorageDeduct, m_nFreeTime, m_nPlaceJettonTime, m_nGameEndTime, m_nMaxChipRobot);
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//银行操作
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



// 添加逗号
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

// 删除逗号
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
	//变量定义
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

//发牌
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

	//通杀或者通赔
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

//获取系统得分
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