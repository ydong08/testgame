#ifndef CMD_BACCARAT_HEAD_FILE
#define CMD_BACCARAT_HEAD_FILE
#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID						249									//游戏 I D
#define GAME_PLAYER					150									//游戏人数
#define GAME_NAME					TEXT("欢乐至尊")						//游戏名字

//版本信息
#define VERSION_SERVER			    PROCESS_VERSION(6,0,3)				//程序版本
#define VERSION_CLIENT				PROCESS_VERSION(6,0,3)				//程序版本

//状态定义
#define	GS_PLACE_JETTON				GAME_STATUS_PLAY							//下注状态
#define	GS_GAME_END					GAME_STATUS_PLAY+1						//结束状态
#define	GS_MOVECARD_END				GAME_STATUS_PLAY+2						//结束状态

//区域索引
#define ID_SHUN_MEN					0									//顺门
#define ID_JIAO_L					1									//左边角
#define ID_HENG_L					2									//左横
#define ID_DUI_MEN					3									//对门
#define ID_DAO_MEN					4									//倒门
#define ID_JIAO_R					5									//右边角
#define ID_HENG_R					6									//右横

//玩家索引
#define BANKER_INDEX				0									//庄家索引
#define SHUN_MEN_INDEX				1									//顺门索引
#define DUI_MEN_INDEX				2									//对门索引
#define DAO_MEN_INDEX				3									//倒门索引
#define MAX_INDEX					3									//最大索引

#define CHIP_COUNT					8									//筹码种类
#define AREA_COUNT					7									//区域数目
#define CONTROL_AREA				3									//受控区域

//赔率定义
#define RATE_TWO_PAIR				12									//对子赔率
#define SERVER_LEN					32									//房间长度

#define MAX_CARD					2
#define MAX_CARDGROUP				4

//机器人信息
struct tagRobotInfo
{
	int nChip[CHIP_COUNT];												//筹码定义
	int nAreaChance[AREA_COUNT];										//区域几率
	TCHAR szCfgFileName[MAX_PATH];										//配置文件
	int	nMaxTime;														//最大赔率

	tagRobotInfo()
	{
		int nTmpChip[CHIP_COUNT] = {1000, 10000, 100000, 500000, 1000000, 5000000};
		int nTmpAreaChance[AREA_COUNT] = {2, 2, 3, 4, 4, 2,1};
		TCHAR szTmpCfgFileName[MAX_PATH] = _T("SupremeConfig.ini");

		nMaxTime = 1;
		memcpy(nChip, nTmpChip, sizeof(nChip));
		memcpy(nAreaChance, nTmpAreaChance, sizeof(nAreaChance));
		memcpy(szCfgFileName, szTmpCfgFileName, sizeof(szCfgFileName));
	}
};

//记录信息
struct tagServerGameRecord
{
	bool							bWinShunMen;						//顺门胜利
	bool							bWinDuiMen;							//对门胜利
	bool							bWinDaoMen;							//倒门胜利
};

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_FREE				44									//游戏空闲
#define SUB_S_GAME_START			22									//游戏开始
#define SUB_S_PLACE_JETTON			54									//用户下注
#define SUB_S_GAME_END				102									//游戏结束
#define SUB_S_APPLY_BANKER			103									//申请庄家
#define SUB_S_CHANGE_BANKER			433									//切换庄家
#define SUB_S_CHANGE_USER_SCORE		105									//更新积分
#define SUB_S_SEND_RECORD			23									//游戏记录
#define SUB_S_PLACE_JETTON_FAIL		107									//下注失败
#define SUB_S_CANCEL_BANKER			108									//取消申请
#define SUB_S_CHEAT					123									//作弊信息

#define SUB_S_AMDIN_COMMAND			9998									//管理员命令
#define SUB_S_ADMIN_STORAGE         113                                 //库存通知
#define SUB_S_ROBOT_BANKER          114                                 //机器人上装
#define SUB_S_QIANG_BANKER          115                                 //抢庄
#define SUB_S_APPLY_FAILURE         116                                 //抢庄失败
 
//请求回复
struct CMD_S_CommandResult
{
	BYTE cbAckType;					//回复类型
		#define ACK_SET_WIN_AREA  1
		#define ACK_PRINT_SYN     2
		#define ACK_RESET_CONTROL 3
	BYTE cbResult;
		#define CR_ACCEPT  2			//接受
		#define CR_REFUSAL 3			//拒绝
	BYTE cbExtendData[20];			//附加数据
};

struct CMD_Apply_Failure
{
	BYTE  cbCode;
};

//失败结构
struct CMD_S_PlaceJettonFail
{
	WORD							wPlaceUser;							//下注玩家
	BYTE							lJettonArea;						//下注区域
	LONGLONG						lPlaceScore;						//当前下注
};

//更新积分
struct CMD_S_ChangeUserScore
{
	WORD							wChairID;							//椅子号码
	LONGLONG							lScore;								//玩家积分

	//庄家信息
	WORD							wCurrentBankerChairID;				//当前庄家
	BYTE							cbBankerTime;						//庄家局数
	LONGLONG							lCurrentBankerScore;				//庄家分数
};

//申请庄家
struct CMD_S_ApplyBanker
{
	WORD							wApplyUser;							//申请玩家
};

//取消申请
struct CMD_S_CancelBanker
{
	WORD							dwCancelUser;					//取消玩家
};

//切换庄家
struct CMD_S_ChangeBanker
{
	WORD							wBankerUser;						//当庄玩家
	LONGLONG						lBankerScore;						//庄家欢乐豆
};

//游戏状态
struct CMD_S_StatusFree
{
	//全局信息
	BYTE							cbTimeLeave;						//剩余时间

	//玩家信息
	LONGLONG						lUserMaxScore;						//玩家欢乐豆

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家成绩
	LONGLONG						lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//控制信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制
	LONGLONG                        lQiangConition;
	LONGLONG                        lQiangScore;

		//房间信息
	LONGLONG                           dwServerID;
};

//游戏状态
struct CMD_S_StatusPlay
{
	//全局下注
	LONGLONG						lAllJettonScore[AREA_COUNT];		//全体总注

	//玩家下注
	LONGLONG						lUserJettonScore[AREA_COUNT];		//个人总注

	//玩家积分
	LONGLONG						lUserMaxScore;						//最大下注							

	//控制信息
	LONGLONG						lApplyBankerCondition;				//申请条件
	LONGLONG						lAreaLimitScore;					//区域限制

	LONGLONG                        lQiangConition;
	LONGLONG                        lQiangScore;

	//扑克信息
	BYTE							cbTableCardArray[4][2];				//桌面扑克

	//庄家信息
	WORD							wBankerUser;						//当前庄家
	WORD							cbBankerTime;						//庄家局数
	LONGLONG						lBankerWinScore;					//庄家赢分
	LONGLONG						lBankerScore;						//庄家分数
	bool							bEnableSysBanker;					//系统做庄

	//结束信息
	LONGLONG						lEndBankerScore;					//庄家成绩
	LONGLONG						lEndUserScore;						//玩家成绩
	LONGLONG						lEndUserReturnScore;				//返回积分
	LONGLONG						lEndRevenue;						//游戏税收

	//全局信息
	BYTE							cbTimeLeave;						//剩余时间
	BYTE							cbGameStatus;						//游戏状态
	
	//房间信息
	LONGLONG                           dwServerID;
};

//游戏空闲
struct CMD_S_GameFree
{
	BYTE							cbTimeLeave;						//剩余时间
	WORD							wCurrentBanker;						//当前庄家
	INT								nBankerTime;						//做庄次数
};

//游戏开始
struct CMD_S_GameStart
{
	WORD							wBankerUser;						//庄家位置
	LONGLONG						lBankerScore;						//庄家欢乐豆
	LONGLONG						lUserMaxScore;						//我的欢乐豆
	BYTE							cbTimeLeave;						//剩余时间	
	bool							bContiueCard;						//继续发牌
	int								nChipRobotCount;					//人数上限 (下注机器人)
};

//用户下注
struct CMD_S_PlaceJetton
{
	WORD							wChairID;							//用户位置
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
	bool							bIsAndroid;							//是否机器人
};

//游戏结束
struct CMD_S_GameEnd
{
	//下局信息
	BYTE							cbTimeLeave;						//剩余时间

	//扑克信息
	BYTE							cbTableCardArray[4][2];				//桌面扑克
	BYTE							cbLeftCardCount;					//扑克数目

	WORD							bcFirstCard;
 
	//庄家信息
	WORD							wCurrentBanker;						//当前庄家
	LONGLONG						lBankerScore;						//庄家成绩
	LONGLONG						lBankerTotallScore;					//庄家成绩
	INT								nBankerTime;						//做庄次数

	//玩家成绩
	LONGLONG						lUserScore;							//玩家成绩
	LONGLONG						lUserReturnScore;					//返回积分

	//全局信息
	LONGLONG						lRevenue;							//游戏税收
};

//游戏作弊
struct CMD_S_Cheat
{
	BYTE							cbTableCardArray[4][2];				//桌面扑克
};

//更新库存
struct CMD_S_SystemStorage
{
	LONGLONG						lStorage;						//新库存值
	LONGLONG						lStorageDeduct;					//库存衰减
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_PLACE_JETTON			1									//用户下注
#define SUB_C_APPLY_BANKER			2									//申请庄家
#define SUB_C_CANCEL_BANKER			3									//取消申请
#define SUB_C_CONTINUE_CARD			4									//继续发牌
#define SUB_C_AMDIN_COMMAND			5									//管理员命令
#define SUB_C_GET_ACCOUNT			7									//获取帐号
#define SUB_C_CHECK_ACCOUNT			8									//获取帐号
#define SUB_S_SCORE_RESULT			9									//积分结果
#define SUB_S_ACCOUNT_RESULT		10									//帐号结果
//客户端消息
#define IDM_ADMIN_COMMDN			WM_USER+1000
#define IDM_GET_ACCOUNT				WM_USER+1001
#define IDM_CHEAK_ACCOUNT			WM_USER+1002
#define IDM_RESET_STORAGE           WM_USER+1003
#define SUB_C_ADMIN_RESET_STORAGE   11                                  //清除库存

#define SUB_C_QIANG_BANKER          12                                  //抢庄申请

//控制区域信息
struct tagControlInfo
{
	BYTE cbControlArea[MAX_INDEX];			//控制区域
};

struct tagAdminReq
{
	BYTE							m_cbExcuteTimes;					//执行次数
	BYTE							m_cbControlStyle;					//控制方式
									#define		CS_BANKER_LOSE    1
									#define		CS_BANKER_WIN	  2
									#define		CS_BET_AREA		  3
	bool							m_bWinArea[3];						//赢家区域
};


struct CMD_C_AdminReq
{
	BYTE cbReqType;
		 #define RQ_SET_WIN_AREA	1
		 #define RQ_RESET_CONTROL	2
		 #define RQ_PRINT_SYN		3
	BYTE cbExtendData[20];			//附加数据V
	LONGLONG nServerValue;
};

//用户下注
struct CMD_C_PlaceJetton
{
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
};

struct CMD_C_CheakAccount
{
	TCHAR szUserAccount[32];
};

struct CMD_S_ScoreResult
{
	LONGLONG lUserJettonScore[AREA_COUNT];	    //个人总注
};

struct CMD_S_AccountResult
{
	TCHAR szAccount[100][32];					//帐号昵称
};
//更新库存
struct CMD_C_UpdateStorage
{
	LONGLONG						lStorage;						//新库存值
	LONGLONG						lStorageDeduct;					//库存衰减
};


//////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif
