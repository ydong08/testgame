#include "StdAfx.h"
#include "Math.h"
#include "Resource.h"
#include "GameClientView.h"
#include "GameClientDlg.h"
#include ".\gameclientview.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_FLASH_WINNER			100									//������ʶ
#define IDI_SHOW_CHANGE_BANKER		101									//�ֻ�ׯ��
#define IDI_DISPATCH_CARD			102									//���Ʊ�ʶ
#define IDI_SHOWDISPATCH_CARD_TIP	103									//������ʾ
#define IDI_OPENCARD				104									//������ʾ
#define IDI_MOVECARD_END			105									//�Ƅ��ƽY��
#define IDI_POSTCARD				106									//������ʾ
#define IDI_CARD_ANIM				107									//ϴ�ƶ���

//��ť��ʶ
#define IDC_JETTON_BUTTON_100		200									//��ť��ʶ
#define IDC_JETTON_BUTTON_500		201									//��ť��ʶ
#define IDC_JETTON_BUTTON_1000		202									//��ť��ʶ
#define IDC_JETTON_BUTTON_10000		203									//��ť��ʶ
#define IDC_JETTON_BUTTON_100000	204									//��ť��ʶ
#define IDC_JETTON_BUTTON_500000	205									//��ť��ʶ
#define IDC_JETTON_BUTTON_1000000	206									//��ť��ʶ
#define IDC_JETTON_BUTTON_5000000	207									//��ť��ʶ
#define IDC_APPY_BANKER				208									//��ť��ʶ
#define IDC_CANCEL_BANKER			209									//��ť��ʶ
#define IDC_SCORE_MOVE_L			210									//��ť��ʶ
#define IDC_SCORE_MOVE_R			211									//��ť��ʶ
#define IDC_VIEW_CHART				212									//��ť��ʶ
#define IDC_AUTO_OPEN_CARD			213									//��ť��ʶ
#define IDC_OPEN_CARD				214									//��ť��ʶ
#define IDC_BANK					215									//��ť��ʶ
#define IDC_CONTINUE_CARD			216									//��ť��ʶ
#define IDC_UP						223									//��ť��ʶ
#define IDC_DOWN					224									//��ť��ʶ
#define IDC_OVER_CHIP				225									//��ť��ʶ
#define IDC_QIANGZHUANG             226 

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientView, CGameFrameViewGDI)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_UP,OnUp)
	ON_BN_CLICKED(IDC_DOWN,OnDown)
	ON_BN_CLICKED(IDC_BT_ADMIN,OpenAdminWnd)
	ON_BN_CLICKED(IDC_BANK_DRAW, OnBankDraw)
	ON_BN_CLICKED(IDC_BANK_STORAGE, OnBankStorage)
	ON_BN_CLICKED(IDC_APPY_BANKER, OnApplyBanker)
	ON_BN_CLICKED(IDC_CANCEL_BANKER, OnCancelBanker)
	ON_BN_CLICKED(IDC_SCORE_MOVE_L, OnScoreMoveL)
	ON_BN_CLICKED(IDC_SCORE_MOVE_R, OnScoreMoveR)
	ON_BN_CLICKED(IDC_OPEN_CARD, OnOpenCard)
	ON_BN_CLICKED(IDC_AUTO_OPEN_CARD,OnAutoOpenCard)
	ON_MESSAGE(WM_VIEWLBTUP,OnViLBtUp)
	ON_BN_CLICKED(IDC_OVER_CHIP, OnOverChip)
	ON_BN_CLICKED(IDC_QIANGZHUANG,OnQiangBanker)
	ON_BN_CLICKED(IDC_BANK,OnBank)


END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameClientView::CGameClientView()
{
	//��ע��Ϣ
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//ȫ����ע
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//ׯ����Ϣ
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lTmpBankerWinScore=0;
	m_lMeMaxScore = 0L;
	AddWinFalgWidth = 0;

	m_Scene_Width = 0;
	m_Scene_Height = 0;
	m_Anim_Index = 0;

	//���ֳɼ�
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//״̬��Ϣ
	m_lCurrentJetton=0L;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bWinShunMen=false;
	m_bWinDuiMen=false;
	m_bWinDaoMen=false;
	m_bFlashResult=false;
	m_blMoveFinish = false;
	m_blAutoOpenCard = true;
	m_bWaitSkip=false;
	m_bPlayAnim = false;
	m_bCheckOverBtn = false;
	m_enDispatchCardTip=enDispatchCardTip_NULL;

	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		

	m_lAreaLimitScore=0L;	

	//���ױ���
	m_bShowCheatInfo = false;
	ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
	ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));
	ZeroMemory(m_bIsShowScore, sizeof(m_bIsShowScore));
	ZeroMemory(m_nStarPoint, sizeof(m_nStarPoint));
	

	//λ����Ϣ
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//��ʷ�ɼ�
	m_lMeStatisticScore=0;

	//�ؼ�����
	m_pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);

	//����λͼ
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageViewFill.LoadFromResource(hInstance,IDB_VIEW_FILL);
	//m_ImageViewBack.LoadFromResource(hInstance,IDB_VIEW_BACK);
	m_ImageWinFlags.LoadFromResource(hInstance,IDB_WIN_FLAGS);
	m_ImageLoseFlags.LoadFromResource(hInstance,IDB_LOSE_FLAGS);
	m_ImageJettonView.LoadFromResource(hInstance,IDB_JETTOM_VIEW);
	m_ImageMeJetton.LoadFromResource(hInstance,IDB_JETTOM_ME);
	m_ImageScoreNumber.LoadFromResource(hInstance,IDB_SCORE_NUMBER);
	m_ImageBKScoreNumber.LoadFromResource(hInstance,IDB_BANKER_SCORE_NUMBER);
	m_ImageTLScoreNumber.LoadFromResource(hInstance,IDB_TOTAL_SCORE_NUMBER);
	m_ImageBankerInfo.LoadFromResource(hInstance,IDC_BANKER_INFO);
	m_ImageBankerList.LoadFromResource(hInstance,IDC_BANKER_LIST);
	//--------------------------
	m_ImageNoteMain.LoadFromResource(hInstance,IDB_NOTE_MAIN);
	m_ImageNoteArr_L.LoadFromResource(hInstance,IDB_NOTE_AR_L);
	m_ImageNoteArr_C.LoadFromResource(hInstance,IDB_NOTE_AR_C);
	m_ImageNoteArr_R.LoadFromResource(hInstance,IDB_NOTE_AR_R);
	m_ImageNoteInfo.LoadFromResource(hInstance,IDB_NOTE_INFO);
	m_ImageMainTimer.LoadFromResource(hInstance,IDB_MAIN_TIMER);
	m_ImageTimerNumber.LoadFromResource(hInstance,IDB_MAIN_TIMER_NUM);
	m_ImageBankerOther.LoadFromResource(hInstance,IDB_BANKER_OTHER);
	m_ImageBankerMe.LoadFromResource(hInstance,IDB_BANKER_ME);

	m_ImageFrameShunMen.LoadFromResource(hInstance,IDB_FRAME_SHUN_MEN);
	m_ImageFrameDaoMen.LoadFromResource(hInstance,IDB_FRAME_DAO_EMN);
	m_ImageFrameDuiMen.LoadFromResource(hInstance,IDB_FRAME_DUI_MEN);
	m_ImageFrameJiaoL.LoadFromResource(hInstance,IDB_FRAME_JIAO_L);
	m_ImageFrameJiaoR.LoadFromResource(hInstance,IDB_FRAME_JIAO_R);
	m_ImageFrameHengL.LoadFromResource(hInstance,IDB_FRAME_HENG_L);
	m_ImageFrameHengR.LoadFromResource(hInstance,IDB_FRAME_HENG_R);
	for (int i=0; i<19; i++)
	{
		m_ImageCardFlag[i].LoadFromResource(hInstance,IDB_CARD_FLAG_1+i);
	}
	TCHAR sz_png[64];
	for (int i=0; i<64; i++)
	{
		_sntprintf(sz_png,CountArray(sz_png),TEXT("CARD_ANIMATION_%d"),i+1);
		m_PngAnim[i].LoadImage(hInstance,sz_png);
	}

	for (int i=0; i<6; i++)
	{
		_sntprintf(sz_png,CountArray(sz_png),TEXT("STAR_ANIMATION_%d"),i+1);
		m_PngStarAnim[0][i].LoadImage(hInstance,sz_png);
		m_PngStarAnim[1][i].LoadImage(hInstance,sz_png);
	}

	m_ImageSweepAll.LoadFromResource(hInstance,IDD_SWEEP_ALL);
	m_ImagePassAll.LoadFromResource(hInstance,IDD_PASS_ALL);
	//---------------------------

	//m_PngFrameCenter.LoadImage(AfxGetInstanceHandle(),TEXT("FARAME_CENTER"));
	//m_PngFrameAngleUp.LoadImage(AfxGetInstanceHandle(),TEXT("FARAME_ANGLE_UP"));
	//m_PngFrameAngleDown.LoadImage(AfxGetInstanceHandle(),TEXT("FARAME_ANGLE_DOWN"));

	//m_PngTimeNum.LoadImage(AfxGetInstanceHandle(),TEXT("TIME_NUMBER"));

	//m_ImageGameEnd.SetLoadInfo( IDB_GAME_END, hInstance );

	m_ImageMeBanker.LoadFromResource(  hInstance,IDB_ME_BANKER );
	m_ImageChangeBanker.LoadFromResource( hInstance,IDB_CHANGE_BANKER );
	m_ImageNoBanker.LoadFromResource( hInstance, IDB_NO_BANKER );	


	//m_cbTableCardArray[BANKER_INDEX][0]=0x1A;
	//m_cbTableCardArray[BANKER_INDEX][1]=0x37;
	//m_cbTableCardArray[SHUN_MEN_INDEX][0]=0x2A;
	//m_cbTableCardArray[SHUN_MEN_INDEX][1]=0x07;
	//bool bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;

	m_pClientControlDlg = NULL;
	m_hInst = NULL;


	m_ScoreFont.CreateFont(0,0,0,0,700,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));

	return;
}

//��������
CGameClientView::~CGameClientView(void)
{
	if( m_pClientControlDlg )
	{
		delete m_pClientControlDlg;
		m_pClientControlDlg = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}
void CGameClientView::SetMoveCardTimer()
{		
	KillTimer(IDI_POSTCARD);
	m_OpenCardIndex = 1;
	m_bcShowCount = 0;
	ZeroMemory(m_nStarPoint, sizeof(m_nStarPoint));
	//m_PostStartIndex =  m_GameLogic.GetCardValue(m_bcfirstShowCard);
	//if(m_PostStartIndex>1)
	//	m_PostStartIndex  = (m_PostStartIndex-1)%4;
	int hiint = HIBYTE(m_bcfirstShowCard);
	int loint = LOBYTE(m_bcfirstShowCard);
	m_nStarPoint[0]=hiint-1;
	m_nStarPoint[1]=loint-1;
	m_PostCardIndex=(hiint+loint-1)%4;
	//if(m_PostCardIndex==1 || m_PostCardIndex==3) m_PostCardIndex+=2;
	for (int i = 0;i<4;i++)
	{
		m_CardControl[i].m_blAdd = true;
		m_CardControl[i].m_blAuto = m_blAutoOpenCard;
		m_CardControl[i].m_blGameEnd = true;	
	}
	m_bWaitSkip=true;
	SetTimer(IDI_POSTCARD,50,NULL);
}

//������Ϣ
int CGameClientView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//�����ؼ�
	CRect rcCreate(0,0,0,0);
	//m_GameRecord.Create(IDD_DLG_GAME_RECORD,this);
	m_ApplyUser.Create( IDD_DLG_GAME_RECORD	, this );
	//m_GameRecord.ShowWindow(SW_HIDE);

	//��ע��ť
	m_btJetton100.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100);
	m_btJetton500.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500);
	m_btJetton1000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000);
	m_btJetton10000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_10000);
	m_btJetton100000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_100000);
	m_btJetton500000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_500000);
	m_btJetton1000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_1000000);
	m_btJetton5000000.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_JETTON_BUTTON_5000000);

	//���밴ť
	m_btApplyBanker.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_APPY_BANKER);
	m_btCancelBanker.Create(NULL,WS_CHILD|WS_DISABLED,rcCreate,this,IDC_CANCEL_BANKER);

	m_btScoreMoveL.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_L);
	m_btScoreMoveR.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_SCORE_MOVE_R);

	m_btOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_OPEN_CARD);
	m_btAutoOpenCard.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_AUTO_OPEN_CARD);

	m_btAutoOpenCard.ShowWindow(FALSE);
	

	m_btBank.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_BANK);
	//m_btContinueCard.Create(NULL,WS_CHILD,rcCreate,this,IDC_CONTINUE_CARD);

	m_btUp.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_UP);
	m_btDown.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_DOWN);

	m_BtnOverChip.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_OVER_CHIP);
	
	m_btQiangZhuang.Create(NULL,WS_CHILD|WS_VISIBLE|WS_DISABLED,rcCreate,this,IDC_QIANGZHUANG);

	//���ð�ť
	HINSTANCE hResInstance=AfxGetInstanceHandle();
	m_btJetton100.LoadRgnImage(hResInstance,IDB_BT_JETTON_100,RGB(255,0,255));
	m_btJetton500.LoadRgnImage(hResInstance,IDB_BT_JETTON_500,RGB(255,0,255));
	m_btJetton1000.LoadRgnImage(hResInstance,IDB_BT_JETTON_1000,RGB(255,0,255));
	m_btJetton10000.LoadRgnImage(hResInstance,IDB_BT_JETTON_10000,RGB(255,0,255));
	m_btJetton100000.LoadRgnImage(hResInstance,IDB_BT_JETTON_100000,RGB(255,0,255));
	m_btJetton500000.LoadRgnImage(hResInstance,IDB_BT_JETTON_500000,RGB(255,0,255));
	m_btJetton1000000.LoadRgnImage(hResInstance,IDB_BT_JETTON_1000000,RGB(255,0,255));
	m_btJetton5000000.LoadRgnImage(hResInstance,IDB_BT_JETTON_5000000,RGB(255,0,255));

	m_btApplyBanker.SetButtonImage(IDB_BT_APPLY_BANKER,hResInstance,false,true);
	m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_BANKER,hResInstance,false,true);//IDB_BT_CANCEL_APPLY
	m_btCancelBanker.ShowWindow(SW_HIDE);

	m_btScoreMoveL.SetButtonImage(IDB_BT_SCORE_MOVE_L,hResInstance,false,false);
	m_btScoreMoveR.SetButtonImage(IDB_BT_SCORE_MOVE_R,hResInstance,false,false);
	

	m_btAutoOpenCard.SetButtonImage(IDB_BT_AUTO_OPEN_CARD,hResInstance,false,true);
	m_btOpenCard.SetButtonImage(IDB_BT_OPEN_CARD,hResInstance,false,true);

	m_btBank.SetButtonImage(IDB_BT_BANK,hResInstance,false,true);
	//m_btContinueCard.SetButtonImage(IDB_BT_CONTINUE_CARD,hResInstance,false,false);

	m_btOpenAdmin.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(4,4,11,11),this,IDC_BT_ADMIN);
	m_btOpenAdmin.ShowWindow(SW_HIDE);

	m_btUp.SetButtonImage(IDB_BT_BT_S,hResInstance,false,false);
	m_btDown.SetButtonImage(IDB_BT_BT_X,hResInstance,false,false);

	m_BtnOverChip.SetButtonImage(IDB_CHIP_OVER,hResInstance,false,true);
	m_btQiangZhuang.SetButtonImage(IDB_BT_QIANGZHUANG,hResInstance,false,true);

	m_btBank.EnableWindow(TRUE);

	//�˿˿ؼ�
	for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].SetDisplayFlag(true);

	//����
	m_hInst = NULL;
	m_pClientControlDlg = NULL;
	m_hInst = LoadLibrary(TEXT("SupremeClientControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(CWnd* pParentWnd); 
		CREATE ClientControl = (CREATE)GetProcAddress(m_hInst,"CreateClientControl"); 
		if ( ClientControl )
		{
			m_pClientControlDlg = static_cast<IClientControlDlg*>(ClientControl(this));
			m_pClientControlDlg->ShowWindow( SW_HIDE );
		}
	}

	m_btUp.ShowWindow(SW_SHOW);
	m_btUp.EnableWindow(false);
	m_btDown.ShowWindow(SW_SHOW);
	m_btDown.EnableWindow(false);
	//����
#ifdef __SPECIAL___
	if(m_DlgBank.m_hWnd==NULL) m_DlgBank.Create(IDD_BANK_STORAGE,this);
#endif

	return 0;
}

//���ý���
void CGameClientView::ResetGameView()
{
	//��ע��Ϣ
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	//ȫ����ע
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//ׯ����Ϣ
	m_wBankerUser=INVALID_CHAIR;		
	m_wBankerTime=0;
	m_lBankerScore=0L;	
	m_lBankerWinScore=0L;
	m_lTmpBankerWinScore=0;
	m_lMeMaxScore = 0L;
	AddWinFalgWidth = 0;

	m_Scene_Width = 0;
	m_Scene_Height = 0;
	m_Anim_Index = 0;

	//���ֳɼ�
	m_lMeCurGameScore=0L;	
	m_lMeCurGameReturnScore=0L;
	m_lBankerCurGameScore=0L;
	m_lGameRevenue=0L;

	//״̬��Ϣ
	m_lCurrentJetton=0L;
	m_cbAreaFlash=0xFF;
	m_wMeChairID=INVALID_CHAIR;
	m_bShowChangeBanker=false;
	m_bNeedSetGameRecord=false;
	m_bWinShunMen=false;
	m_bWinDuiMen=false;
	m_bWinDaoMen=false;
	m_bFlashResult=false;
	m_bShowGameResult=false;
	m_bPlayAnim = false;
	m_bCheckOverBtn = false;
	m_enDispatchCardTip=enDispatchCardTip_NULL;

	m_lMeCurGameScore=0L;			
	m_lMeCurGameReturnScore=0L;	
	m_lBankerCurGameScore=0L;		

	m_lAreaLimitScore=0L;	

	m_bWaitSkip=false;

	//���ױ���
	m_bShowCheatInfo = false;
	ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
	ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));
	ZeroMemory(m_bIsShowScore, sizeof(m_bIsShowScore));
	ZeroMemory(m_nStarPoint, sizeof(m_nStarPoint));

	//λ����Ϣ
	m_nScoreHead = 0;
	m_nRecordFirst= 0;	
	m_nRecordLast= 0;	

	//��ʷ�ɼ�
	m_lMeStatisticScore=0;

	//����б�
	m_ApplyUser.ClearAll();

	//�������
	CleanUserJetton();

	//���ð�ť
	m_btApplyBanker.ShowWindow(SW_SHOW);
	m_btApplyBanker.EnableWindow(FALSE);
	//m_btCancelBanker.ShowWindow(SW_HIDE);
	//m_btCancelBanker.SetButtonImage(IDB_BT_CANCEL_APPLY,AfxGetInstanceHandle(),false,true);

	return;
}

//�����ؼ�
void CGameClientView::RectifyControl(int nWidth, int nHeight)
{
	//λ����Ϣ

	m_Scene_Width = nWidth;
	m_Scene_Height = nHeight;

	m_nWinFlagsExcursionX = m_ImageNoteArr_R.GetWidth()+m_ImageNoteMain.GetHeight()-6;	
	m_nWinFlagsExcursionY = nHeight-m_ImageNoteMain.GetHeight()+5;

	//����λ��
	int nCenterX=nWidth/2, nCenterY=nHeight/2, nBorderWidth=3;

	m_rcJiaoL.left=(nWidth-m_ImageBankerMe.GetWidth())/2+6;
	m_rcJiaoL.top=(nHeight-m_ImageBankerMe.GetHeight())/2+6-60;
	m_rcJiaoL.right=m_rcJiaoL.left+m_ImageFrameJiaoL.GetWidth();
	m_rcJiaoL.bottom=m_rcJiaoL.top+m_ImageFrameJiaoL.GetHeight();

	m_rcDuiMen.left=m_rcJiaoL.right+6;
	m_rcDuiMen.top=m_rcJiaoL.top;
	m_rcDuiMen.right=m_rcDuiMen.left+m_ImageFrameDuiMen.GetWidth();
	m_rcDuiMen.bottom=m_rcDuiMen.top+m_ImageFrameDuiMen.GetHeight();

	m_rcJiaoR.left=m_rcDuiMen.right+6;
	m_rcJiaoR.top=m_rcJiaoL.top;
	m_rcJiaoR.right=m_rcJiaoR.left+m_ImageFrameJiaoR.GetWidth();
	m_rcJiaoR.bottom=m_rcJiaoR.top+m_ImageFrameJiaoR.GetHeight();

	m_rcDaoMen.left=m_rcJiaoL.left;
	m_rcDaoMen.top=m_rcJiaoL.bottom+6;
	m_rcDaoMen.right=m_rcDaoMen.left+m_ImageFrameDaoMen.GetWidth();
	m_rcDaoMen.bottom=m_rcDaoMen.top+m_ImageFrameDaoMen.GetHeight();

	m_rcShunMen.left=m_rcJiaoR.left;
	m_rcShunMen.top=m_rcJiaoR.bottom+6;
	m_rcShunMen.right=m_rcShunMen.left+m_ImageFrameShunMen.GetWidth();
	m_rcShunMen.bottom=m_rcShunMen.top+m_ImageFrameShunMen.GetHeight();

	m_rcHengL.left=m_rcJiaoL.left;
	m_rcHengL.top=m_rcDaoMen.bottom+6;
	m_rcHengL.right=m_rcHengL.left+m_ImageFrameHengL.GetWidth();
	m_rcHengL.bottom=m_rcHengL.top+m_ImageFrameHengL.GetHeight();

	m_rcBanker.left=m_rcHengL.right+6;
	m_rcBanker.top=m_rcHengL.top;
	m_rcBanker.right=m_rcBanker.left+m_ImageFrameShunMen.GetWidth();
	m_rcBanker.bottom=m_rcBanker.top+m_ImageFrameShunMen.GetHeight();
	
	m_rcHengR.left=m_rcShunMen.left;
	m_rcHengR.top=m_rcShunMen.bottom+6;
	m_rcHengR.right=m_rcHengR.left+m_ImageFrameHengR.GetWidth();
	m_rcHengR.bottom=m_rcHengR.top+m_ImageFrameHengR.GetHeight();

	//��������
	int ExcursionY=10;
	m_PointJettonNumber[ID_SHUN_MEN].SetPoint((m_rcShunMen.right+m_rcShunMen.left)/2, (m_rcShunMen.bottom+m_rcShunMen.top)/2-ExcursionY);
	m_PointJettonNumber[ID_JIAO_L].SetPoint((m_rcJiaoL.right+m_rcJiaoL.left)/2, (m_rcJiaoL.bottom+m_rcJiaoL.top)/2-ExcursionY);
	m_PointJettonNumber[ID_HENG_L].SetPoint((m_rcHengL.right+m_rcHengL.left)/2, (m_rcHengL.bottom+m_rcHengL.top)/2-ExcursionY);
	m_PointJettonNumber[ID_HENG_R].SetPoint((m_rcHengR.right+m_rcHengR.left)/2, (m_rcHengR.bottom+m_rcHengR.top)/2-ExcursionY);
	m_PointJettonNumber[ID_DUI_MEN].SetPoint((m_rcDuiMen.right+m_rcDuiMen.left)/2, (m_rcDuiMen.bottom+m_rcDuiMen.top)/2-ExcursionY);
	m_PointJettonNumber[ID_DAO_MEN].SetPoint((m_rcDaoMen.right+m_rcDaoMen.left)/2, (m_rcDaoMen.bottom+m_rcDaoMen.top)/2-ExcursionY);
	m_PointJettonNumber[ID_JIAO_R].SetPoint((m_rcJiaoR.right+m_rcJiaoR.left)/2, (m_rcJiaoR.bottom+m_rcJiaoR.top)/2-ExcursionY);
	m_PointJettonNumber[AREA_COUNT].SetPoint((m_rcBanker.right+m_rcBanker.left)/2, (m_rcBanker.bottom+m_rcBanker.top)/2-ExcursionY);
	m_PointJettonNumber[AREA_COUNT+1].SetPoint((m_rcShunMen.left+m_rcDaoMen.right)/2, (m_rcBanker.top+m_rcDuiMen.bottom)/2-ExcursionY);

	////����λ��
	//m_PointJetton[ID_SHUN_MEN].SetPoint(m_rcShunMen.left, m_rcShunMen.top);
	//m_PointJetton[ID_JIAO_L].SetPoint(m_rcJiaoL.left, m_rcJiaoL.top);
	//m_PointJetton[ID_HENG_L].SetPoint(m_rcHengL.left, m_rcHengL.top);
	//m_PointJetton[ID_HENG_R].SetPoint(m_rcHengR.left, m_rcHengR.top);
	//m_PointJetton[ID_DUI_MEN].SetPoint(m_rcDuiMen.left, m_rcDuiMen.top);
	//m_PointJetton[ID_DAO_MEN].SetPoint(m_rcDaoMen.left, m_rcDaoMen.top);
	//m_PointJetton[ID_JIAO_R].SetPoint(m_rcJiaoR.left, m_rcJiaoR.top);
	//m_PointJetton[AREA_COUNT].SetPoint(m_rcBanker.left, m_rcBanker.top);

	//�˿˿ؼ�
	if (m_wMeChairID == m_wBankerUser &&  m_wMeChairID!=INVALID_CHAIR)
	{
		m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2,m_rcBanker.bottom-20),enXCenter,enYTop);
		m_CardControl[1].SetBenchmarkPos(CPoint(nWidth/2+220,nHeight/2-152),enXLeft,enYTop);
		m_CardControl[2].SetBenchmarkPos(CPoint(nWidth/2,m_rcJiaoL.top-170),enXCenter,enYTop);
		m_CardControl[3].SetBenchmarkPos(CPoint(nWidth/2-340,nHeight/2-152),enXLeft,enYTop);
	}
	else
	{
		m_CardControl[0].SetBenchmarkPos(CPoint(nWidth/2,m_rcJiaoL.top-170),enXCenter,enYTop);
		m_CardControl[1].SetBenchmarkPos(CPoint(nWidth/2-340,nHeight/2-152),enXLeft,enYTop);
		m_CardControl[2].SetBenchmarkPos(CPoint(nWidth/2,m_rcBanker.bottom-20),enXCenter,enYTop);
		m_CardControl[3].SetBenchmarkPos(CPoint(nWidth/2+220,nHeight/2-152),enXLeft,enYTop);
	}

	//m_CardControl[4].SetBenchmarkPos(CPoint(nWidth/2,m_rcJiaoL.top+100),enXCenter,enYTop);

	//�ƶ��ؼ�
	HDWP hDwp=BeginDeferWindowPos(32);
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	m_ApplyUser.m_viewHandle = m_hWnd;

	//�б�ؼ�
	DeferWindowPos(hDwp,m_ApplyUser,NULL,nWidth-200,30,170,60,uFlags);
	DeferWindowPos(hDwp,m_btUp,NULL,nWidth-20,28,75/5,16,uFlags);
	DeferWindowPos(hDwp,m_btDown,NULL,nWidth-20,70,75/5,16,uFlags);

	//���밴ť
	CRect rcJetton;
	m_btJetton100.GetWindowRect(&rcJetton);
	int nSpace = 11;
	int nXPos = nWidth - 4*(rcJetton.Width()+nSpace);
	int nYPos = nHeight - m_ImageNoteMain.GetWidth() - (rcJetton.Height()+(3*nSpace)) ;

	DeferWindowPos(hDwp,m_btJetton1000,NULL,nXPos,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton10000,NULL,nXPos + nSpace + rcJetton.Width(),nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton100000,NULL,nXPos + nSpace *2 + rcJetton.Width() * 2,nYPos,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton500000,NULL,nXPos + nSpace * 3 + rcJetton.Width() * 3,nYPos,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btJetton1000000,NULL,nXPos,nYPos + nSpace + rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton5000000,NULL,nXPos + nSpace + rcJetton.Width(),nYPos + nSpace + rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btJetton100,NULL,nXPos + nSpace * 2 + rcJetton.Width() * 2,nYPos + nSpace + rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btJetton500,NULL,nXPos + nSpace * 3 + rcJetton.Width() * 3,nYPos + nSpace + rcJetton.Height()-5,0,0,uFlags|SWP_NOSIZE);

	//��ׯ��ť
	DeferWindowPos(hDwp,m_btApplyBanker,NULL,nWidth-90,5,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btCancelBanker,NULL,nWidth-90,5,0,0,uFlags|SWP_NOSIZE);
	//DeferWindowPos(hDwp,m_btCancelBanker,NULL,nWidth-85,nHeight-m_ImageNoteMain.GetHeight()-m_ImageNoteInfo.GetHeight()-30,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btScoreMoveL,NULL,m_ImageNoteMain.GetWidth(),nHeight-m_ImageNoteMain.GetHeight(),0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btScoreMoveR,NULL,nWidth-m_ImageNoteArr_R.GetWidth(),nHeight-m_ImageNoteMain.GetHeight(),0,0,uFlags|SWP_NOSIZE);

	//���ư�ť
	DeferWindowPos(hDwp,m_btAutoOpenCard,NULL,nWidth-85,nHeight-m_ImageNoteMain.GetHeight()-m_ImageNoteInfo.GetHeight()-70,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_btOpenCard,NULL,nWidth-85,nHeight-m_ImageNoteMain.GetHeight()-m_ImageNoteInfo.GetHeight()-70,0,0,uFlags|SWP_NOSIZE);

	//������ť
	DeferWindowPos(hDwp,m_btBank,NULL,nWidth-85,100,0,0,uFlags|SWP_NOSIZE);
	//DeferWindowPos(hDwp,m_btContinueCard,NULL,nWidth/2-220,nHeight/2+270,0,0,uFlags|SWP_NOSIZE);


	DeferWindowPos(hDwp,m_BtnOverChip,NULL,nWidth-85,nHeight-m_ImageNoteMain.GetHeight()-m_ImageNoteInfo.GetHeight()-30,0,0,uFlags|SWP_NOSIZE);

	DeferWindowPos(hDwp,m_btQiangZhuang,NULL,nWidth-85*2,100,0,0,uFlags|SWP_NOSIZE);

	

	//�����ƶ�
	EndDeferWindowPos(hDwp);

	return;
}

//�滭����
void CGameClientView::DrawGameView(CDC * pDC, int nWidth, int nHeight)
{
	//�滭����
	DrawViewImage(pDC,m_ImageViewFill,DRAW_MODE_SPREAD);
	//DrawViewImage(pDC,m_ImageViewBack,DRAW_MODE_CENTENT);
	
	//��ȡ״̬
	BYTE cbGameStatus=m_pGameClientDlg->GetGameStatus();

	//״̬��ʾ
	//CFont static InfoFont;
	//InfoFont.CreateFont(-16,0,0,0,400,0,0,0,134,3,2,ANTIALIASED_QUALITY,2,TEXT("����"));
	//CFont * pOldFont=pDC->SelectObject(&InfoFont);
	//pDC->SetTextColor(RGB(255,234,0));

	//pDC->SelectObject(pOldFont);
	//InfoFont.DeleteObject();

	//ʱ����ʾ
	//CImageHandle ImageHandleTimeFlag(&m_ImageTimeFlag);
	//int nTimeFlagWidth = m_ImageTimeFlag.GetWidth()/3;
	//int nFlagIndex=0;
	//if (cbGameStatus==GS_FREE) nFlagIndex=0;
	//else if (cbGameStatus==GS_PLACE_JETTON) nFlagIndex=1;
	//else if (cbGameStatus==GS_GAME_END) nFlagIndex=2;
	//m_ImageTimeFlag.BitBlt(pDC->GetSafeHdc(), nWidth/2-348, nHeight/2+225, nTimeFlagWidth, m_ImageTimeFlag.GetHeight(),
	//	nFlagIndex * nTimeFlagWidth, 0);
	//-------------------------------------------------------
	int begin_y = nHeight-m_ImageNoteMain.GetHeight();
	int w_arr = m_ImageNoteMain.GetWidth();

	m_ImageNoteMain.BitBlt(pDC->GetSafeHdc(),0,begin_y);
	m_ImageNoteArr_L.BitBlt(pDC->GetSafeHdc(),w_arr,begin_y);

	int begin_x = w_arr + m_ImageNoteArr_L.GetWidth();
	int max_widht = nWidth - begin_x - m_ImageNoteArr_R.GetWidth();
	int step = m_ImageNoteArr_C.GetWidth();
	for (int i=0; i<=max_widht/step; i++)
	{
		m_ImageNoteArr_C.BitBlt(pDC->GetSafeHdc(),begin_x+step*i,begin_y);
	}

	m_ImageNoteArr_R.BitBlt(pDC->GetSafeHdc(),nWidth-m_ImageNoteArr_R.GetWidth(),begin_y);

	int info_w = m_ImageNoteInfo.GetWidth();
	for (int i=0; i<=nWidth/info_w;i++)
	{
		m_ImageNoteInfo.BitBlt(pDC->GetSafeHdc(),info_w*i,begin_y-m_ImageNoteInfo.GetHeight());
	}
	int time_h = m_ImageMainTimer.GetHeight();
	m_ImageMainTimer.TransDrawImage(pDC,0,begin_y-time_h,m_ImageMainTimer.GetWidth(),time_h,0,0,RGB(255, 0, 255));

	if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
	{
		m_ImageBankerMe.TransDrawImage(pDC,(nWidth-m_ImageBankerMe.GetWidth())/2,(nHeight-m_ImageBankerMe.GetHeight())/2-60,
			m_ImageBankerMe.GetWidth(),m_ImageBankerMe.GetHeight(),0,0,RGB(255, 0, 255));
	}
	else
	{
		m_ImageBankerOther.TransDrawImage(pDC,(nWidth-m_ImageBankerOther.GetWidth())/2,(nHeight-m_ImageBankerOther.GetHeight())/2-60,
			m_ImageBankerOther.GetWidth(),m_ImageBankerOther.GetHeight(),0,0,RGB(255, 0, 255));
	}

	m_ImageBankerInfo.BitBlt(pDC->GetSafeHdc(),0,0);
	m_ImageBankerList.BitBlt(pDC->GetSafeHdc(),nWidth-m_ImageBankerList.GetWidth(),0);
	//-------------------------------------------------------
	//if(m_bWaitSkip)
	//{
	//	CPngImage	PngWait;
	//	PngWait.LoadImage(AfxGetInstanceHandle(),TEXT("WAITING"));
	//	PngWait.DrawImage(pDC,nWidth/2-PngWait.GetWidth()/2,nHeight/2-PngWait.GetHeight());
	//}
	//Dlg�����SetGameTimer(GetMeChairID()��������õ�SwitchViewChairID��������������8ʱʵ����ViewIDת��Ϊ0��
	WORD wUserTimer=GetUserClock(m_wMeChairID);
	if (wUserTimer>=0&&wUserTimer<100) /*DrawUserTimer(pDC,nWidth/2-323,nHeight/2+197,wUserTimer);*/
	{
		m_ImageTimerNumber.TransDrawImage(pDC,22,begin_y-m_ImageTimerNumber.GetHeight()-23,m_ImageTimerNumber.GetWidth()/10,m_ImageTimerNumber.GetHeight(),
			(wUserTimer/10)*m_ImageTimerNumber.GetWidth()/10,0,RGB(255, 0, 255));
		m_ImageTimerNumber.TransDrawImage(pDC,37,begin_y-m_ImageTimerNumber.GetHeight()-23,m_ImageTimerNumber.GetWidth()/10,m_ImageTimerNumber.GetHeight(),
			(wUserTimer%10)*m_ImageTimerNumber.GetWidth()/10,0,RGB(255, 0, 255));

		//m_PngTimeNum.DrawImage(pDC,nWidth/2-25,nHeight/2-320,m_PngTimeNum.GetWidth()/10,m_PngTimeNum.GetHeight(),(wUserTimer/10)*m_PngTimeNum.GetWidth()/10,0,
		//	m_PngTimeNum.GetWidth()/10,m_PngTimeNum.GetHeight());
		//m_PngTimeNum.DrawImage(pDC,nWidth/2,nHeight/2-320,m_PngTimeNum.GetWidth()/10,m_PngTimeNum.GetHeight(),(wUserTimer%10)*m_PngTimeNum.GetWidth()/10,0,
		//	m_PngTimeNum.GetWidth()/10,m_PngTimeNum.GetHeight());
	}

	//ʤ���߿�
	FlashJettonAreaFrame(nWidth,nHeight,pDC);

	//������Դ
	//CPngImage	PngJettonView;
	//PngJettonView.LoadImage(AfxGetInstanceHandle(),TEXT("JETTOM_VIEW_PNG"));
	//CSize static SizeJettonItem(PngJettonView.GetWidth()/8,PngJettonView.GetHeight());

	//�ҵ���ע
	DrawMeJettonNumber(pDC);

	//�滭����

	LONGLONG lScoreMax=0L;
	for (INT i=0;i<AREA_COUNT;i++)
	{
		//��������
		LONGLONG lScoreCount=0L;
		LONGLONG static lScoreJetton[CHIP_COUNT]={1000L,10000L,100000L,500000L,1000000L,5000000L,10000000L,50000000L};
		int static nJettonViewIndex=0;

		int index_ = i;
		if (m_wMeChairID != m_wBankerUser || m_wBankerUser==INVALID_CHAIR)
		{
			if(i==ID_SHUN_MEN)
				index_ = ID_DAO_MEN;
			if(i==ID_JIAO_L)
				index_ = ID_HENG_R;
			if(i==ID_HENG_L)
				index_ = ID_JIAO_R;
			if(i==ID_DUI_MEN)
				index_ = AREA_COUNT;
			if(i==ID_DAO_MEN)
				index_ = ID_SHUN_MEN;
			if(i==ID_JIAO_R)
				index_ = ID_HENG_L;
			if(i==ID_HENG_R)
				index_ = ID_JIAO_L;
		}

		//�滭����
		for (INT_PTR j=0;j<m_JettonInfoArray[i].GetCount();j++)
		{
			//��ȡ��Ϣ
			tagJettonInfo * pJettonInfo=&m_JettonInfoArray[i][j];

			//�ۼ�����
			ASSERT(pJettonInfo->cbJettonIndex<CHIP_COUNT);
			lScoreCount+=lScoreJetton[pJettonInfo->cbJettonIndex];

			//ͼƬ����
			//nJettonViewIndex=pJettonInfo->cbJettonIndex;
			//�滭����
			//PngJettonView.DrawImage(pDC,pJettonInfo->nXPos+m_PointJetton[index_].x,
			//	pJettonInfo->nYPos+m_PointJetton[index_].y,SizeJettonItem.cx,SizeJettonItem.cy,
			//	nJettonViewIndex*SizeJettonItem.cx,0,SizeJettonItem.cx,SizeJettonItem.cy);
	
		}

		lScoreMax += lScoreCount;
		//�滭����
		if (lScoreCount>0L)	DrawNumberString(pDC,lScoreCount,m_PointJettonNumber[index_].x,m_PointJettonNumber[index_].y+35,m_bIsShowScore[index_],1);
	}
	if (lScoreMax>0)
	{
		DrawNumberString(pDC,lScoreMax,m_PointJettonNumber[AREA_COUNT+1].x,m_PointJettonNumber[AREA_COUNT+1].y+35,false,3);
	}
	if(m_bPlayAnim)
	{
		if(m_Anim_Index<64)
		{
			CSize Size_anim(nWidth/2-170,nHeight/2-100);
			m_PngAnim[m_Anim_Index].DrawImage(pDC,m_Anim_Index<=33?Size_anim.cx:Size_anim.cx+80,Size_anim.cy);
		}
		else
		{
			m_bPlayAnim = false;
			m_Anim_Index = 0;
			KillTimer(IDI_CARD_ANIM);
		}
	}
	
	if (m_bcShowCount>0&&m_bcShowCount<=40)
	{
		int count_ = min(m_bcShowCount,27);
		CSize Size_anim(count_*((nWidth/2-50)/27),count_*((nHeight/2-50)/27));
		m_PngStarAnim[0][m_nStarPoint[0]].DrawImage(pDC,Size_anim.cx,Size_anim.cy,70,128,70*(count_-1),0);
		Size_anim.cy = count_*((nHeight/2-150)/27);
		m_PngStarAnim[1][m_nStarPoint[1]].DrawImage(pDC,Size_anim.cx,Size_anim.cy,70,128,70*(count_-1),0);
	}
	//�滭ׯ��
	DrawBankerInfo(pDC,nWidth,nHeight);

	//�滭�û�
	DrawMeInfo(pDC,nWidth,nHeight);
	

	//�л�ׯ��
	if ( m_bShowChangeBanker )
	{
		int	nXPos = nWidth / 2 - 130;
		int	nYPos = nHeight / 2 - 160;

		//������ׯ
		if ( m_wMeChairID == m_wBankerUser )
		{
			//CImageHandle ImageHandleBanker(&m_ImageMeBanker);
			m_ImageMeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
		else if ( m_wBankerUser != INVALID_CHAIR )
		{
			//CImageHandle ImageHandleBanker(&m_ImageChangeBanker);
			m_ImageChangeBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
		else
		{
			//CImageHandle ImageHandleBanker(&m_ImageNoBanker);
			m_ImageNoBanker.BitBlt(pDC->GetSafeHdc(), nXPos, nYPos);
		}
	}

	//������Ϣ
	DrawCheatCard(nWidth, nHeight, pDC);

	////������ʾ
	//if (enDispatchCardTip_NULL!=m_enDispatchCardTip)
	//{
	//	if (m_ImageDispatchCardTip.IsNull()==false) m_ImageDispatchCardTip.Destroy();
	//	if (enDispatchCardTip_Dispatch==m_enDispatchCardTip)
	//		m_ImageDispatchCardTip.LoadFromResource(AfxGetInstanceHandle(),IDB_DISPATCH_CARD);
	//	else
	//		m_ImageDispatchCardTip.LoadFromResource(AfxGetInstanceHandle(),IDB_CONTINUE_CARD);

	//	m_ImageDispatchCardTip.BitBlt(pDC->GetSafeHdc(), (nWidth-m_ImageDispatchCardTip.GetWidth())/2, nHeight/2);
	//}


	//����״̬
	//if (cbGameStatus==GS_GAME_END)	
	{
		//�滭�˿�

		BYTE cbCardData[2]={0,0};
		int card_flag = 0;
		int card_num = 0;
		CPoint nOff[4] = {CPoint(110,60),CPoint(0,140),CPoint(110,60),CPoint(0,140)};
		for (int i=0; i<CountArray(m_CardControl); ++i) 
		{
			m_CardControl[i].DrawCardControl(pDC);
			if (i<4 && m_CardControl[i].GetPositively())
			{
				if(m_CardControl[i].m_CardTempItemArray.GetCount() != 2) continue;
				cbCardData[0] = m_CardControl[i].m_CardTempItemArray[0].cbCardData;
				cbCardData[1] = m_CardControl[i].m_CardTempItemArray[1].cbCardData;

				card_flag =  m_GameLogic.GetCardType(cbCardData,2);


				CPoint OriginPoint;
				m_CardControl[i].GetOriginPoint(OriginPoint);
				if (card_flag<CT_POINT)
				{
					m_ImageCardFlag[card_flag-1].TransDrawImage(pDC,OriginPoint.x+nOff[i].x,OriginPoint.y+nOff[i].y,m_ImageCardFlag[card_flag-1].GetWidth(),
						m_ImageCardFlag[card_flag-1].GetHeight(),0,0,RGB(255,0,255));

				}
				//else
				//{
				//	card_num = m_GameLogic.GetCardListPip(cbCardData,2);
				//	CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());
				//	m_ImageScoreNumber.TransDrawImage(pDC,OriginPoint.x+nOff[i].x,OriginPoint.y+nOff[i].y,SizeScoreNumber.cx,SizeScoreNumber.cy,
				//		card_num*SizeScoreNumber.cx,0,RGB(255,0,255));
				//}
			}
		}
	}

	AddWinFalgWidth = 0;
	if (nWidth <= 760){
		AddWinFalgWidth = 0;
	}else{
		AddWinFalgWidth = (nWidth-760)/24-1;
	}
	//ʤ����־
	DrawWinFlags(pDC);

	if(m_blMoveFinish)
	{
		//��ʾ���
		ShowGameResult(pDC, nWidth, nHeight);	
	}

	return;
}

//������Ϣ
void CGameClientView::SetMeMaxScore(LONGLONG lMeMaxScore)
{
	if (m_lMeMaxScore!=lMeMaxScore)
	{
		//���ñ���
		m_lMeMaxScore=lMeMaxScore;
	}

	return;
}

//�����ע
void CGameClientView::SetAreaLimitScore(LONGLONG lAreaLimitScore)
{
	if ( m_lAreaLimitScore!= lAreaLimitScore )
	{
		//���ñ���
		m_lAreaLimitScore=lAreaLimitScore;
	}
}

//���ó���
void CGameClientView::SetCurrentJetton(LONGLONG lCurrentJetton)
{
	//���ñ���
	ASSERT(lCurrentJetton>=0L);
	m_lCurrentJetton=lCurrentJetton;

	return;
}

//��ʷ��¼
void CGameClientView::SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen)
{
	//��������
	tagClientGameRecord &GameRecord = m_GameRecordArrary[m_nRecordLast];
	GameRecord.bWinShunMen=bWinShunMen;
	GameRecord.bWinDuiMen=bWinDuiMen;
	GameRecord.bWinDaoMen=bWinDaoMen;

	//��������
	//��������
	if (0==m_lUserJettonScore[ID_SHUN_MEN]) GameRecord.enOperateShunMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_SHUN_MEN] > 0 && true==bWinShunMen) GameRecord.enOperateShunMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_SHUN_MEN] > 0 && false==bWinShunMen) GameRecord.enOperateShunMen=enOperateResult_Lost;

	if (0==m_lUserJettonScore[ID_DAO_MEN]) GameRecord.enOperateDaoMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_DAO_MEN] > 0 && true==bWinDaoMen) GameRecord.enOperateDaoMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_DAO_MEN] >0 && false==bWinDaoMen) GameRecord.enOperateDaoMen=enOperateResult_Lost;

	if (0==m_lUserJettonScore[ID_DUI_MEN]) GameRecord.enOperateDuiMen=enOperateResult_NULL;
	else if (m_lUserJettonScore[ID_DUI_MEN] > 0 && true==bWinDuiMen) GameRecord.enOperateDuiMen=enOperateResult_Win;
	else if (m_lUserJettonScore[ID_DUI_MEN]>0 && false==bWinDuiMen) GameRecord.enOperateDuiMen=enOperateResult_Lost;

	//�ƶ��±�
	m_nRecordLast = (m_nRecordLast+1) % MAX_SCORE_HISTORY;
	if ( m_nRecordLast == m_nRecordFirst )
	{
		m_nRecordFirst = (m_nRecordFirst+1) % MAX_SCORE_HISTORY;
		if ( m_nScoreHead < m_nRecordFirst ) m_nScoreHead = m_nRecordFirst;
	}

	int nHistoryCount = (m_nRecordLast - m_nRecordFirst + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( (MAX_FALG_COUNT+AddWinFalgWidth) > nHistoryCount ) m_btScoreMoveR.EnableWindow(TRUE);

	//�Ƶ����¼�¼
	if ( (MAX_FALG_COUNT+AddWinFalgWidth) < nHistoryCount )
	{
		m_nScoreHead = (m_nRecordLast - (MAX_FALG_COUNT+AddWinFalgWidth) + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
		m_btScoreMoveL.EnableWindow(TRUE);
		m_btScoreMoveR.EnableWindow(FALSE);
	}

	return;
}

//�������
void CGameClientView::CleanUserJetton()
{
	//��������
	for (BYTE i=0;i<CountArray(m_JettonInfoArray);i++)
	{
		m_JettonInfoArray[i].RemoveAll();
	}

	//��ע��Ϣ
	ZeroMemory(m_lUserJettonScore,sizeof(m_lUserJettonScore));

	ZeroMemory(m_bIsShowScore, sizeof(m_bIsShowScore));
	//ȫ����ע
	ZeroMemory(m_lAllJettonScore,sizeof(m_lAllJettonScore));

	//���½���
	UpdateGameView(NULL);

	m_bCheckOverBtn = false;
	return;
}

//������ע
void CGameClientView::SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount)
{
	//Ч�����
	ASSERT(cbViewIndex<=ID_HENG_R);
	if (cbViewIndex>ID_HENG_R) return;

	m_lUserJettonScore[cbViewIndex]=lJettonCount;

	//���½���
	UpdateGameView(NULL);
}

//�����˿�
void CGameClientView::SetCardInfo(BYTE cbTableCardArray[4][2])
{
	if (cbTableCardArray!=NULL)
	{
		CopyMemory(m_cbTableCardArray,cbTableCardArray,sizeof(m_cbTableCardArray));

		//��ʼ����
		DispatchCard();
	}
	else
	{
		ZeroMemory(m_cbTableCardArray,sizeof(m_cbTableCardArray));
	}
}

//���ó���
void CGameClientView::PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount)
{
	//Ч�����
	ASSERT(cbViewIndex<=ID_HENG_R);
	if (cbViewIndex>ID_HENG_R) return;

	//��������
	bool bPlaceJetton=false;
	LONGLONG lScoreIndex[CHIP_COUNT]={1000L,10000L,100000L,500000L,1000000L,5000000L,10000000L,50000000L};

	//�߿���
	int nFrameWidth=0, nFrameHeight=0;
	int nBorderWidth=3;

	switch (cbViewIndex)
	{
	case ID_SHUN_MEN:
		{ 
			m_lAllJettonScore[ID_SHUN_MEN] += lScoreCount;
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			{
				nFrameWidth = m_rcShunMen.right-m_rcShunMen.left;
				nFrameHeight = m_rcShunMen.bottom-m_rcShunMen.top;
			}
			else
			{
				nFrameWidth = m_rcDaoMen.right-m_rcDaoMen.left;
				nFrameHeight = m_rcDaoMen.bottom-m_rcDaoMen.top;
			}
			break;
		}
	case ID_JIAO_L:
		{
			m_lAllJettonScore[ID_JIAO_L] += lScoreCount;
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			{
				nFrameWidth = m_rcJiaoL.right-m_rcJiaoL.left;
				nFrameHeight = m_rcJiaoL.bottom-m_rcJiaoL.top;
			}
			else
			{
				nFrameWidth = m_rcHengR.right-m_rcHengR.left;
				nFrameHeight = m_rcHengR.bottom-m_rcHengR.top;
			}
			break;
		}
	case ID_HENG_L:
		{ 
			m_lAllJettonScore[ID_HENG_L] += lScoreCount;
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			{
				nFrameWidth = m_rcHengL.right-m_rcHengL.left;
				nFrameHeight = m_rcHengL.bottom-m_rcHengL.top;
			}
			else
			{
				nFrameWidth = m_rcJiaoR.right-m_rcJiaoR.left;
				nFrameHeight = m_rcJiaoR.bottom-m_rcJiaoR.top;
			}
			break;
		}
	case ID_HENG_R:
		{ 
			m_lAllJettonScore[ID_HENG_R] += lScoreCount;
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			{
				nFrameWidth = m_rcHengR.right-m_rcHengR.left;
				nFrameHeight = m_rcHengR.bottom-m_rcHengR.top;
			}
			else
			{
				nFrameWidth = m_rcJiaoL.right-m_rcJiaoL.left;
				nFrameHeight = m_rcJiaoL.bottom-m_rcJiaoL.top;
			}
			break;
		}
	case ID_DUI_MEN:
		{ 
			m_lAllJettonScore[ID_DUI_MEN] += lScoreCount;
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			{
				nFrameWidth = m_rcDuiMen.right-m_rcDuiMen.left;
				nFrameHeight = m_rcDuiMen.bottom-m_rcDuiMen.top;
			}
			else
			{
				nFrameWidth = m_rcBanker.right-m_rcBanker.left;
				nFrameHeight = m_rcBanker.bottom-m_rcBanker.top;
			}
			break;
		}
	case ID_DAO_MEN:
		{
			m_lAllJettonScore[ID_DAO_MEN] += lScoreCount;
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			{
				nFrameWidth = m_rcDaoMen.right-m_rcDaoMen.left;
				nFrameHeight = m_rcDaoMen.bottom-m_rcDaoMen.top;
			}
			else
			{
				nFrameWidth = m_rcShunMen.right-m_rcShunMen.left;
				nFrameHeight = m_rcShunMen.bottom-m_rcShunMen.top;
			}
			break;
		}
	case ID_JIAO_R:
		{ 
			m_lAllJettonScore[ID_JIAO_R] += lScoreCount;
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			{
				nFrameWidth = m_rcJiaoR.right-m_rcJiaoR.left;
				nFrameHeight = m_rcJiaoR.bottom-m_rcJiaoR.top;
			}
			else
			{
				nFrameWidth = m_rcHengL.right-m_rcHengL.left;
				nFrameHeight = m_rcHengL.bottom-m_rcHengL.top;
			}
			break;
		}	
	default:
		{
			ASSERT(FALSE);
			return;
		}
	}

	nFrameWidth += nBorderWidth;
	nFrameHeight += nBorderWidth;

	//�����ж�
	bool bAddJetton=lScoreCount>0?true:false;
	lScoreCount=(lScoreCount>0?lScoreCount:-lScoreCount);

	//���ӳ���
	for (BYTE i=0;i<CountArray(lScoreIndex);i++)
	{
		//������Ŀ
		BYTE cbScoreIndex=CHIP_COUNT-i-1;
		int nCellCount=int(lScoreCount/lScoreIndex[cbScoreIndex]);

		//�������
		if (nCellCount==0L) continue;

		//�������
		for (int j=0;j<nCellCount;j++)
		{
			if (true==bAddJetton)
			{
				//�������
				tagJettonInfo JettonInfo;
				int nJettonSize=68;
				JettonInfo.cbJettonIndex=cbScoreIndex;
				JettonInfo.nXPos=rand()%(nFrameWidth-nJettonSize);
				JettonInfo.nYPos=rand()%(nFrameHeight-nJettonSize);

				//��������
				bPlaceJetton=true;
				m_JettonInfoArray[cbViewIndex].Add(JettonInfo);
			}
			else
			{
				for (int nIndex=0; nIndex<m_JettonInfoArray[cbViewIndex].GetCount(); ++nIndex)
				{
					//�Ƴ��ж�
					tagJettonInfo &JettonInfo=m_JettonInfoArray[cbViewIndex][nIndex];
					if (JettonInfo.cbJettonIndex==cbScoreIndex)
					{
						m_JettonInfoArray[cbViewIndex].RemoveAt(nIndex);
						break;
					}
				}
			}
		}

		//������Ŀ
		lScoreCount-=nCellCount*lScoreIndex[cbScoreIndex];
	}

	//���½���
	if (bPlaceJetton==true) UpdateGameView(NULL);

	return;
}

//���ֳɼ�
void CGameClientView::SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue)
{
	m_lMeCurGameScore=lMeCurGameScore;			
	m_lMeCurGameReturnScore=lMeCurGameReturnScore;			
	m_lBankerCurGameScore=lBankerCurGameScore;			
	m_lGameRevenue=lGameRevenue;					
}

//����ʤ��
void CGameClientView::SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet)
{	
	//����ʱ��
	if (true==bSet)
	{
		//���ö�ʱ��
		SetTimer(IDI_FLASH_WINNER,500,NULL);

		//ȫʤ�ж�
		if (false==bWinShunMen && false==bWinDuiMen && false==bWinDaoMen)
		{
		}
	}
	else 
	{
		//�����ʱ��
		KillTimer(IDI_FLASH_WINNER);

		//ȫʤ�ж�
		if (false==m_bWinDaoMen && false==m_bWinShunMen && false==m_bWinDuiMen)
		{
		}
	}

	//���ñ���
	m_bWinShunMen=bWinShunMen;
	m_bWinDuiMen=bWinDuiMen;
	m_bWinDaoMen=bWinDaoMen;
	m_bFlashResult=bSet;
	m_bShowGameResult=bSet;
	m_cbAreaFlash=0xFF;

	//���½���
	UpdateGameView(NULL);

	return;
}

//��ȡ����
BYTE CGameClientView::GetJettonArea(CPoint MousePoint)
{
	if (m_rcShunMen.PtInRect(MousePoint))
	{
		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			return ID_SHUN_MEN;
		else
			return ID_DAO_MEN;
	}
	if (m_rcJiaoL.PtInRect(MousePoint))
	{
		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			return ID_JIAO_L;
		else
			return ID_HENG_R;
	}
	if (m_rcHengL.PtInRect(MousePoint))
	{
		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			return ID_HENG_L;
		else
			return ID_JIAO_R;
	}
	if (m_rcHengR.PtInRect(MousePoint))
	{
		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			return ID_HENG_R;
		else
			return ID_JIAO_L;
	}
	if (m_rcBanker.PtInRect(MousePoint))
	{
		if (m_wMeChairID != m_wBankerUser || m_wBankerUser==INVALID_CHAIR)
			return ID_DUI_MEN;	
	}
	if (m_rcDuiMen.PtInRect(MousePoint))
	{
		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			return ID_DUI_MEN;	
	}
	if (m_rcDaoMen.PtInRect(MousePoint))
	{
		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			return ID_DAO_MEN;	
		else
			return ID_SHUN_MEN;
	}
	if (m_rcJiaoR.PtInRect(MousePoint))
	{
		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
			return ID_JIAO_R;
		else
			return ID_HENG_L;
	}

	return 0xFF;
}

//�滭����
void CGameClientView::DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore,int type)
{
	//������Դ
	CSize SizeScoreNumber(m_ImageScoreNumber.GetWidth()/10,m_ImageScoreNumber.GetHeight());

	//������Ŀ
	int nNumberCount=0;
	LONGLONG lNumberTemp=lNumber;
	do
	{
		nNumberCount++;
		lNumberTemp/=10;
	} while (lNumberTemp>0);

	//λ�ö���
	INT nYDrawPos=nYPos-SizeScoreNumber.cy/2;
	INT nXDrawPos=nXPos+nNumberCount*SizeScoreNumber.cx/2-SizeScoreNumber.cx;

	if ( type == 1 && !bMeScore)
	{
		m_ImageJettonView.TransDrawImage(pDC,nXPos-50,nYPos-55,m_ImageJettonView.GetWidth(),m_ImageJettonView.GetHeight(),0,0,RGB(255,0,255));
	}

	//�滭����
	for (int i=0;i<nNumberCount;i++)
	{
		//�滭����
		int nCellNumber=(int)(lNumber%10);
		if ( type == 1)
		{
			m_ImageScoreNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				nCellNumber*SizeScoreNumber.cx,0,RGB(255,0,255));
		}
		if (type == 2)
		{
			m_ImageBKScoreNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				nCellNumber*SizeScoreNumber.cx,0,RGB(255,0,255));
		}
		if (type == 3)
		{
			m_ImageTLScoreNumber.TransDrawImage(pDC,nXDrawPos,nYDrawPos,SizeScoreNumber.cx,SizeScoreNumber.cy,
				nCellNumber*SizeScoreNumber.cx,0,RGB(255,0,255));
		}

		//���ñ���
		lNumber/=10;
		nXDrawPos-=SizeScoreNumber.cx;
	};

	return;
}

//�滭����
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos)
{
	CString strNumber, strTempNum;
	strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

	int nLength = strTempNum.GetLength();
	if (nLength <= 3)
		strNumber.Format(_T("%I64d"), lNumber);
	else
	{
		for (int i = 0; i < int((nLength-1)/3); i++)
			strTempNum.Insert(nLength - 3*(i+1), _T(","));
		strNumber.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
	}

	//�������
	pDC->TextOut(nXPos,nYPos,strNumber);
}

//�滭����
void CGameClientView::DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat)
{
	CString strNumber, strTempNum;
	strTempNum.Format(_T("%I64d"), (lNumber>0?lNumber:-lNumber));

	int nLength = strTempNum.GetLength();
	if (nLength <= 3)
		strNumber.Format(_T("%I64d"), lNumber);
	else
	{
		for (int i = 0; i < int((nLength-1)/3); i++)
			strTempNum.Insert(nLength - 3*(i+1), _T(","));
		strNumber.Format(_T("%s%s"), (lNumber<0?_T("-"):_T("")), strTempNum);
	}

	//�������
	if (nFormat==-1) 
	{
		pDC->DrawText(strNumber,rcRect,DT_END_ELLIPSIS|DT_LEFT|DT_TOP|DT_SINGLELINE);
	}
	else 
	{
		pDC->DrawText(strNumber,rcRect,nFormat);
	}
}

//��ʱ����Ϣ
void CGameClientView::OnTimer(UINT nIDEvent)
{
	//����ʤ��
	if (nIDEvent==IDI_FLASH_WINNER)
	{
		//���ñ���
		m_bFlashResult=!m_bFlashResult;

		//���½���
		UpdateGameView(NULL);

		return;
	}
	if (nIDEvent==IDI_CARD_ANIM)
	{
		UpdateGameView(NULL);
		m_Anim_Index++;
		//if (m_Anim_Index==5)
		//{
		//	m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("CARD_DICE_1"));
		//}
		if (m_Anim_Index==35)
		{
			m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("CARD_DICE_2"));
		}
		return;
	}
	if(IDI_POSTCARD ==nIDEvent)
	{

		if(m_bcShowCount<=40)
		{
			//if(m_CardControl[4].m_CardTempItemArray.GetCount()>0) 
			//{
			//	m_CardControl[4].m_blhideOneCard = false;
			//	m_CardControl[4].m_blGameEnd = true;
			//	m_CardControl[4].SetDisplayFlag(true);
			//	m_CardControl[4].ShowCardControl(true);
			//	m_CardControl[4].m_CardItemArray.SetSize(1);
			//	m_CardControl[4].m_CardItemArray[0]=m_CardControl[4].m_CardTempItemArray[0];
			//	m_CardControl[4].m_CardItemArray[0].cbCardData = m_bcfirstShowCard;
			//	m_CardControl[4].m_CardItemArray[0].bShoot = false;
			//}
			//m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));
			
			if(m_bcShowCount==10)
				m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("CARD_DICE_1"));

			UpdateGameView(NULL);
			if(m_bcShowCount==40)
			{
				KillTimer(IDI_POSTCARD);
				SetTimer(IDI_POSTCARD,500,NULL);
			}
		}else
		{
			//m_CardControl[4].m_CardItemArray.SetSize(0);

			bool blKillTimer = true;
			for (int i = 0;i<4;i++)
			{
				if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
				{
					blKillTimer = false;
					break;
				}
			}
			m_PostCardIndex = m_PostCardIndex%4;
			m_CardControl[m_PostCardIndex].OnCopyCard();
			m_PostCardIndex++;
			if(blKillTimer)
			{
				m_bWaitSkip=false;
				KillTimer(IDI_POSTCARD);
				SetTimer(IDI_OPENCARD,50,NULL);
			}
			else		
				m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("OUT_CARD"));

			UpdateGameView(NULL);
		}
		m_bcShowCount++;
	}
	if(IDI_OPENCARD==nIDEvent)
	{
		if(m_blAutoOpenCard == false)
		{
			while (1)
			{
				bool blCopy = false;
				int i = 0;
				for (i = 0;i<4;i++)
				{
					if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
					{
						blCopy = true;
						break;
					}
				}
				if(false == blCopy)
				{
					break;
				}else
				{
					m_CardControl[i].OnCopyCard();
				}
			}

			for (int i = 0;i<4;i++)
			{
				m_CardControl[i].m_blGameEnd = false;
				m_CardControl[i].m_blhideOneCard = false;
			}
			//���½���
			UpdateGameView(NULL);
			KillTimer(IDI_OPENCARD);

			SetTimer(IDI_DISPATCH_CARD,10000,NULL);

			return ;

		}
		else
		{
			if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x<30&&m_CardControl[m_OpenCardIndex%4].m_blAdd)
			{
				m_CardControl[m_OpenCardIndex%4].m_blGameEnd = false;
				m_CardControl[m_OpenCardIndex%4].m_MovePoint.x+=2;
				m_CardControl[m_OpenCardIndex%4].m_MovePoint.y+=2;
				m_CardControl[m_OpenCardIndex%4].m_blhideOneCard = false;
				//���½���
				UpdateGameView(NULL);


			}else
			{

				m_CardControl[m_OpenCardIndex%4].m_blAdd = false;
				m_CardControl[m_OpenCardIndex%4].m_blGameEnd = false;
				if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x>10)
				{
					m_CardControl[m_OpenCardIndex%4].m_MovePoint.x-=2;
					m_CardControl[m_OpenCardIndex%4].m_MovePoint.y-=2;
				}
				m_CardControl[m_OpenCardIndex%4].m_blhideOneCard = false;
				UpdateGameView(NULL);
				if(m_CardControl[m_OpenCardIndex%4].m_MovePoint.x<=10)
				{
					m_CardControl[m_OpenCardIndex%4].m_tMoveCard.m_blMoveCard = true;
					m_CardControl[m_OpenCardIndex%4].m_tMoveCard.m_blMoveFinish = true;
					m_CardControl[m_OpenCardIndex%4].m_MovePoint.x = 0;
					m_CardControl[m_OpenCardIndex%4].m_MovePoint.y = 0;
					m_CardControl[m_OpenCardIndex%4].m_blAdd = true;
					m_CardControl[m_OpenCardIndex%4].m_blGameEnd = true;
					m_CardControl[m_OpenCardIndex%4].SetPositively(true);
					m_OpenCardIndex++;
					if(m_OpenCardIndex==5)
					{
						for (int i = 0;i<4;i++)
						{
							m_CardControl[i].m_blGameEnd = true;
						}
						KillTimer(IDI_OPENCARD);
						UpdateGameView(NULL);
						//������ʾ�ƶ�ʱ��
						SetTimer(IDI_DISPATCH_CARD,30,NULL);
					}

				}

			}
		}
	}
	//�ֻ�ׯ��
	else if ( nIDEvent == IDI_SHOW_CHANGE_BANKER )
	{
		ShowChangeBanker( false );

		return;
	}
	else if (nIDEvent==IDI_DISPATCH_CARD)
	{
		//��ɷ���
		FinishDispatchCard(true);
		{
			for (int i = 0;i<4;i++)
			{
				m_CardControl[i].m_blGameEnd = true;
				m_CardControl[i].m_blhideOneCard = false;
			}
			KillTimer(IDI_OPENCARD);
			UpdateGameView(NULL);

		}
		while (1)
		{
			bool blCopy = false;
			int i = 0;
			for (i = 0;i<4;i++)
			{
				if(m_CardControl[i].m_CardItemArray.GetCount()!=m_CardControl[i].m_CardTempItemArray.GetCount())
				{
					blCopy = true;
					break;
				}
			}
			if(false == blCopy)
			{
				break;
			}else
			{
				m_CardControl[i].OnCopyCard();
			}
		}
		m_blMoveFinish = true;
		//���½���
		UpdateGameView(NULL);

		return;
	}
	else if (IDI_SHOWDISPATCH_CARD_TIP==nIDEvent)
	{
		SetDispatchCardTip(enDispatchCardTip_NULL);

	}else if(nIDEvent==IDI_MOVECARD_END)
	{
		KillTimer(IDI_MOVECARD_END);
	}

	__super::OnTimer(nIDEvent);
}

//�����Ϣ
void CGameClientView::OnLButtonDown(UINT nFlags, CPoint Point)
{
	for (int i = 0;i<4;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnLButtonDown(nFlags, Point);
		}
	}

	if (m_lCurrentJetton!=0L)
	{
		//��ע����
		BYTE cbJettonArea=GetJettonArea(Point);

		if (cbJettonArea >= ID_SHUN_MEN && cbJettonArea <= ID_HENG_R)
		{
			//�����ע
			LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

			//�Ϸ��ж�
			if (lMaxJettonScore < m_lCurrentJetton) return ;

			CGameFrameEngine::GetInstance()->SendMessage(IDM_PLACE_JETTON,cbJettonArea,0);
		}
	}

	UpdateGameView(NULL);

	__super::OnLButtonDown(nFlags,Point);
}

void CGameClientView::OnLButtonUp(UINT nFlags, CPoint Point)
{
	for (int i = 0;i<4;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnLButtonUp(nFlags, Point);
			UpdateGameView(NULL);
		}

	}
	__super::OnLButtonUp(nFlags,Point);
}

//���������ؼ���������Ϣ
LRESULT CGameClientView::OnViLBtUp(WPARAM wParam, LPARAM lParam)
{
	CPoint *pPoint = (CPoint*)lParam;
	ScreenToClient(pPoint);
	OnLButtonUp(1,*pPoint);
	return 1;

}
void CGameClientView::OnOverChip()
{
	m_btJetton100.EnableWindow(FALSE);
	m_btJetton500.EnableWindow(FALSE);			
	m_btJetton1000.EnableWindow(FALSE);		
	m_btJetton10000.EnableWindow(FALSE);	
	m_btJetton100000.EnableWindow(FALSE);
	m_btJetton500000.EnableWindow(FALSE);
	m_btJetton1000000.EnableWindow(FALSE);
	m_btJetton5000000.EnableWindow(FALSE);
	m_BtnOverChip.EnableWindow(FALSE);
	m_lCurrentJetton = 0L;
	m_bCheckOverBtn = true;
}

void CGameClientView::OnBank()
{
	CGameClientDlg *pGameClientEngine=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientEngine->m_pIClientKernel;

	if(NULL!=pIClientKernel)
	{
		CRect btRect;
		m_btBankerDraw.GetWindowRect(&btRect);
		ShowInsureGet(pIClientKernel,CPoint(btRect.left-200,btRect.top));
	}
}
//����ƶ���Ϣ
void CGameClientView::OnMouseMove(UINT nFlags, CPoint point)
{
	for (int i = 0;i<4;i++)
	{
		if(!m_CardControl[i].m_blAuto)
		{
			m_CardControl[i].OnMouseMove(nFlags, point);
			UpdateGameView(NULL);

		}

	}
	__super::OnMouseMove(nFlags,point);
}
//�����Ϣ
void CGameClientView::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//���ñ���
	m_lCurrentJetton=0L;

	if (m_pGameClientDlg->GetGameStatus()!=GS_GAME_END && m_cbAreaFlash!=0xFF)
	{
		m_cbAreaFlash=0xFF;
		UpdateGameView(NULL);
	}

	__super::OnLButtonDown(nFlags,Point);
}

//�����Ϣ
BOOL CGameClientView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	if (m_lCurrentJetton!=0L)
	{
		//��ȡ����
		CPoint MousePoint;
		GetCursorPos(&MousePoint);
		ScreenToClient(&MousePoint);
		BYTE cbJettonArea=GetJettonArea(MousePoint);

		//���ñ���
		if ( m_cbAreaFlash!= cbJettonArea )
		{
			m_cbAreaFlash = cbJettonArea;
			UpdateGameView(NULL);
		}

		//�����ж�
		if (cbJettonArea==0xFF)
		{
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			return TRUE;
		}

		//�����ע
		LONGLONG lMaxJettonScore=GetUserMaxJetton(cbJettonArea);

		//�Ϸ��ж�
		if ((m_lAllJettonScore[cbJettonArea]+m_lCurrentJetton)>m_lAreaLimitScore || lMaxJettonScore < m_lCurrentJetton)
		{
			SetCursor(LoadCursor(NULL,IDC_NO));
			return TRUE;	
		}

		////���ù��
		//switch (m_lCurrentJetton)
		//{
		//case 100:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)));
		//		return TRUE;
		//	}
		//case 500:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100)));
		//		return TRUE;
		//	}
		//case 1000:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000)));
		//		return TRUE;
		//	}
		//case 10000:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_10000)));
		//		return TRUE;
		//	}
		//case 100000:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_100000)));
		//		return TRUE;
		//	}
		//case 500000:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_500000)));
		//		return TRUE;
		//	}
		//case 1000000:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_1000000)));
		//		return TRUE;
		//	}
		//case 5000000:
		//	{
		//		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_SCORE_5000000)));
		//		return TRUE;
		//	}		
		//}
	}
	else
	{
		ClearAreaFlash();
	}

	return __super::OnSetCursor(pWnd, nHitTest, uMessage);
}

//�ֻ�ׯ��
void CGameClientView::ShowChangeBanker( bool bChangeBanker )
{
	//�ֻ�ׯ��
	if ( bChangeBanker )
	{
		SetTimer( IDI_SHOW_CHANGE_BANKER, 3000, NULL );
		m_bShowChangeBanker = true;
	}
	else
	{
		KillTimer( IDI_SHOW_CHANGE_BANKER );
		m_bShowChangeBanker = false ;
	}

	//���½���
	UpdateGameView(NULL);
}

//��ׯ��ť
void CGameClientView::OnApplyBanker()
{
	CGameFrameEngine::GetInstance()->SendMessage(IDM_APPLY_BANKER,1,0);

}

void CGameClientView::OnQiangBanker()
{
	CGameFrameEngine::GetInstance()->SendMessage(IDM_QIANG_BANKER,1,0);
}

//��ׯ��ť
void CGameClientView::OnCancelBanker()
{
	CGameFrameEngine::GetInstance()->SendMessage(IDM_APPLY_BANKER,0,0);
}

//��������
void CGameClientView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos)
{
	//��������
	int nStringLength=lstrlen(pszString);
	int nXExcursion[8]={1,1,1,0,-1,-1,-1,0};
	int nYExcursion[8]={-1,0,1,1,1,0,-1,-1};

	//�滭�߿�
	pDC->SetTextColor(crFrame);
	for (int i=0;i<CountArray(nXExcursion);i++)
	{
		pDC->TextOut(nXPos+nXExcursion[i],nYPos+nYExcursion[i],pszString,nStringLength);
	}

	//�滭����
	pDC->SetTextColor(crText);
	pDC->TextOut(nXPos,nYPos,pszString,nStringLength);

	return;
}
//ׯ����Ϣ
void CGameClientView::SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore) 
{
	//ׯ�����Ӻ�
	WORD wBankerUser=INVALID_CHAIR;

	//�������Ӻ�
	if (0!=dwBankerUserID)
	{
		for (WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
		{
			IClientUserItem  *pUserData=GetClientUserItem(wChairID);
			if (NULL!=pUserData && dwBankerUserID==pUserData->GetUserID())
			{
				wBankerUser=wChairID;
				break;
			}
		}
	}

	//�л��ж�
	if (m_wBankerUser!=wBankerUser)
	{
		m_wBankerUser=wBankerUser;
		m_wBankerTime=0L;
		m_lBankerWinScore=0L;	
		m_lTmpBankerWinScore=0L;

		if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
		{
			m_CardControl[0].SetBenchmarkPos(CPoint(m_Scene_Width/2,m_rcBanker.bottom-20),enXCenter,enYTop);
			m_CardControl[1].SetBenchmarkPos(CPoint(m_Scene_Width/2+220,m_Scene_Height/2-152),enXLeft,enYTop);
			m_CardControl[2].SetBenchmarkPos(CPoint(m_Scene_Width/2,m_rcJiaoL.top-170),enXCenter,enYTop);
			m_CardControl[3].SetBenchmarkPos(CPoint(m_Scene_Width/2-340,m_Scene_Height/2-152),enXLeft,enYTop);
		}
		else
		{
			m_CardControl[0].SetBenchmarkPos(CPoint(m_Scene_Width/2,m_rcJiaoL.top-170),enXCenter,enYTop);
			m_CardControl[1].SetBenchmarkPos(CPoint(m_Scene_Width/2-340,m_Scene_Height/2-152),enXLeft,enYTop);
			m_CardControl[2].SetBenchmarkPos(CPoint(m_Scene_Width/2,m_rcBanker.bottom-20),enXCenter,enYTop);
			m_CardControl[3].SetBenchmarkPos(CPoint(m_Scene_Width/2+220,m_Scene_Height/2-152),enXLeft,enYTop);
		}
	}
	m_lBankerScore=lBankerScore;
	m_lUserJettonScore[AREA_COUNT] = m_lBankerScore;
}

//�滭��ʶ
void CGameClientView::DrawWinFlags(CDC * pDC)
{
	//�ǿ��ж�
	if (m_nRecordLast==m_nRecordFirst) return;
	int nIndex = m_nScoreHead;
	COLORREF static clrOld ;

	clrOld = pDC->SetTextColor(RGB(255,234,0));
	int nDrawCount=0;
	while ( nIndex != m_nRecordLast && ( m_nRecordLast!=m_nRecordFirst ) && nDrawCount < (MAX_FALG_COUNT+AddWinFalgWidth) )
	{
		//ʤ����ʶ
		tagClientGameRecord &ClientGameRecord = m_GameRecordArrary[nIndex];
		bool static bWinMen[3];
		bWinMen[0]=ClientGameRecord.bWinShunMen;
		bWinMen[1]=ClientGameRecord.bWinDaoMen;
		bWinMen[2]=ClientGameRecord.bWinDuiMen;

		//�������
		enOperateResult static OperateResult[3];
		OperateResult[0]=ClientGameRecord.enOperateShunMen;
		OperateResult[1]=ClientGameRecord.enOperateDaoMen;
		OperateResult[2]=ClientGameRecord.enOperateDuiMen;

		for (int i=0; i<3; ++i)
		{
			//λ�ñ���
			int static nYPos=0,nXPos=0;
			nYPos=m_nWinFlagsExcursionY+i*m_ImageWinFlags.GetWidth();
			nXPos=m_nWinFlagsExcursionX + ((nIndex - m_nScoreHead + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY) * m_ImageWinFlags.GetWidth();

			//ʤ����ʶ
			int static nFlagsIndex=0;
			if (false==bWinMen[i])
			{
				m_ImageLoseFlags.TransDrawImage(pDC,nXPos,nYPos,m_ImageLoseFlags.GetWidth(),m_ImageLoseFlags.GetHeight(),0,0,RGB(255, 0, 255));
			}
			else
			{
				m_ImageWinFlags.TransDrawImage(pDC,nXPos,nYPos,m_ImageWinFlags.GetWidth(),m_ImageWinFlags.GetHeight(),0,0,RGB(255, 0, 255));
			}
			//if (OperateResult[i] != enOperateResult_NULL)
			//{			
			//	nFlagsIndex+=1;

			//	if(nFlagsIndex>=4)
			//	{
			//		nFlagsIndex=3;
			//	}
			//}
			//�滭��ʶ
			//m_ImageWinFlags.TransDrawImage( pDC, nXPos, nYPos, m_ImageWinFlags.GetWidth()/3 , 
			//	m_ImageWinFlags.GetHeight(),m_ImageWinFlags.GetWidth()/3 * nFlagsIndex, 0, RGB(255, 0, 255) );
		}

		//�ƶ��±�
		nIndex = (nIndex+1) % MAX_SCORE_HISTORY;
		nDrawCount++;

	}
	pDC->SetTextColor(clrOld);
}
//�ֹ�����
afx_msg void CGameClientView::OnOpenCard()
{
	CGameFrameEngine::GetInstance()->SendMessage(IDM_OPEN_CARD,0,0);

}

//�Զ�����
afx_msg void CGameClientView::OnAutoOpenCard()
{
	CGameFrameEngine::GetInstance()->SendMessage(IDM_AUTO_OPEN_CARD,0,0);
}

void CGameClientView::OnUp()
{
	m_ApplyUser.m_AppyUserList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
	double nPos = m_ApplyUser.m_AppyUserList.GetScrollPos(SB_VERT);
	double nMax = m_ApplyUser.m_AppyUserList.GetScrollLimit(SB_VERT);
}

void CGameClientView::OnDown()
{
	double nPos = m_ApplyUser.m_AppyUserList.GetScrollPos(SB_VERT);
	if(m_ApplyUser.m_AppyUserList.GetItemCount()>4)
	{
		if(nPos>m_ApplyUser.m_AppyUserList.GetItemCount()-5)
		{
			return ;
		}
	}
	m_ApplyUser.m_AppyUserList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	m_ApplyUser.m_AppyUserList.Invalidate(TRUE);
	double nMax = m_ApplyUser.m_AppyUserList.GetScrollLimit(SB_VERT);
};

//�ƶ���ť
void CGameClientView::OnScoreMoveL()
{
	if ( m_nRecordFirst == m_nScoreHead ) return;

	m_nScoreHead = (m_nScoreHead - 1 + MAX_SCORE_HISTORY) % MAX_SCORE_HISTORY;
	if ( m_nScoreHead == m_nRecordFirst ) m_btScoreMoveL.EnableWindow(FALSE);

	m_btScoreMoveR.EnableWindow(TRUE);

	//���½���
	UpdateGameView(NULL);
}

//�ƶ���ť
void CGameClientView::OnScoreMoveR()
{
	int nHistoryCount = ( m_nRecordLast - m_nScoreHead + MAX_SCORE_HISTORY ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount == (MAX_FALG_COUNT+AddWinFalgWidth) ) return;

	m_nScoreHead = ( m_nScoreHead + 1 ) % MAX_SCORE_HISTORY;
	if ( nHistoryCount-1 == (MAX_FALG_COUNT+AddWinFalgWidth) ) m_btScoreMoveR.EnableWindow(FALSE);

	m_btScoreMoveL.EnableWindow(TRUE);

	//���½���
	UpdateGameView(NULL);
}

//��ʾ���
void CGameClientView::ShowGameResult(CDC *pDC, int nWidth, int nHeight)
{
	//��ʾ�ж�
	//if (m_pGameClientDlg->GetGameStatus()!=GS_GAME_END) return;
	if (false==m_bShowGameResult) return;

	int	nXPos = nWidth / 2 - 129;
	int	nYPos = nHeight / 2 - 208;
	//CImageHandle ImageHandleGameEnd(&m_ImageGameEnd);
	CPngImage	m_ImageGameEnd;
	m_ImageGameEnd.LoadImage(AfxGetInstanceHandle(),TEXT("GAME_END"));

	//CRect rcAlpha(nXPos+2, nYPos+70, nXPos+2 + m_ImageGameEnd.GetWidth(), nYPos+70+m_ImageGameEnd.GetHeight());
	//DrawAlphaRect(pDC, &rcAlpha, RGB(74,70,73), 0.8f);
	m_ImageGameEnd.DrawImage(pDC, nXPos+2, nYPos+70);

	pDC->SetTextColor(RGB(255,234,0));
	CRect rcMeWinScore, rcMeReturnScore;
	rcMeWinScore.left = nXPos+2 + 60;
	rcMeWinScore.top = nYPos+70 + 30;
	rcMeWinScore.right = rcMeWinScore.left + 75;
	rcMeWinScore.bottom = rcMeWinScore.top + 26;

	rcMeReturnScore.left = nXPos+2 + 140;
	rcMeReturnScore.top = nYPos+70 + 30;
	rcMeReturnScore.right = rcMeReturnScore.left + 100;
	rcMeReturnScore.bottom = rcMeReturnScore.top + 26;

	CString strMeGameScore, strMeReturnScore;
	DrawNumberStringWithSpace(pDC,m_lMeCurGameScore,rcMeWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	DrawNumberStringWithSpace(pDC,m_lMeCurGameReturnScore,rcMeReturnScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	CRect rcBankerWinScore;
	rcBankerWinScore.left = nXPos+2 + 60;
	rcBankerWinScore.top = nYPos+70 + 60;
	rcBankerWinScore.right = rcBankerWinScore.left + 75;
	rcBankerWinScore.bottom = rcBankerWinScore.top + 23;

	CString strBankerCurGameScore;
	strBankerCurGameScore.Format(TEXT("%I64d"), m_lBankerCurGameScore);
	DrawNumberStringWithSpace(pDC,m_lBankerCurGameScore,rcBankerWinScore, DT_END_ELLIPSIS | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
}

//͸���滭
bool CGameClientView::DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha)
{
	ASSERT(pDC != 0 && lpRect != 0 && clr != CLR_NONE);
	if(pDC == 0 || lpRect == 0 || clr == CLR_NONE)
	{
		return false;
	}
	//ȫ͸��
	if( abs(fAlpha) <= 0.000001 )
	{
		return true;
	}

	for(int l=lpRect->top; l<lpRect->bottom; l++)
	{
		for(int k=lpRect->left; k<lpRect->right; k++)
		{
			COLORREF clrBk = pDC->GetPixel(k, l);
			COLORREF clrBlend = RGB(GetRValue(clrBk)*(1-fAlpha)+GetRValue(clr)*fAlpha, 
				GetGValue(clrBk)*(1-fAlpha)+GetGValue(clr)*fAlpha,
				GetBValue(clrBk)*(1-fAlpha)+GetBValue(clr)*fAlpha);
			pDC->SetPixel(k, l, clrBlend);
		}
	}

	return true;
}

//�����ע
LONGLONG CGameClientView::GetUserMaxJetton(BYTE cbArea/* = 0xFF*/)
{
	//��������
	LONGLONG lMeMaxScore = 0L;

	//����ע��
	LONGLONG lNowJetton = 0;
	ASSERT(AREA_COUNT<=CountArray(m_lUserJettonScore));
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lNowJetton += m_lUserJettonScore[nAreaIndex];

	//ׯ�ҽ��
	LONGLONG lBankerScore=2147483647;
	if (m_wBankerUser!=INVALID_CHAIR) lBankerScore=m_lBankerScore;
	for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lBankerScore-=m_lAllJettonScore[nAreaIndex];

	//��������
	LONGLONG lAreaLimitScore = 0;
	if (cbArea == 0xFF)
	{
		for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex)
		{
			LONGLONG lAreaLeaveScore = m_lAreaLimitScore-m_lAllJettonScore[nAreaIndex];
			if (lAreaLeaveScore > lAreaLimitScore)
				lAreaLimitScore = lAreaLeaveScore;
		}
	}
	else
	{
		lAreaLimitScore = m_lAreaLimitScore - m_lAllJettonScore[cbArea];
	}

	lMeMaxScore=min(m_lMeMaxScore-lNowJetton,lAreaLimitScore);

	//ׯ������
	lMeMaxScore=min(lMeMaxScore,lBankerScore);

	//��������
	ASSERT(lMeMaxScore >= 0);
	lMeMaxScore = max(lMeMaxScore, 0);

	return lMeMaxScore;
}

//�ɼ�����
void CGameClientView::SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore)
{
	m_lMeCurGameScore=lMeCurGameScore;
	m_lMeCurGameReturnScore=lMeCurGameReturnScore;
	m_lBankerCurGameScore=lBankerCurGameScore;	
}

//�滭����
void CGameClientView::DrawMeJettonNumber(CDC *pDC)
{
	//�滭����
	for (int i=0; i<AREA_COUNT; ++i)
	{
		int index_ = i;
		if (m_wMeChairID != m_wBankerUser || m_wBankerUser==INVALID_CHAIR)
		{
			if(i==ID_SHUN_MEN)
				index_ = ID_DAO_MEN;
			if(i==ID_JIAO_L)
				index_ = ID_HENG_R;
			if(i==ID_HENG_L)
				index_ = ID_JIAO_R;
			if(i==ID_DUI_MEN)
				index_ = AREA_COUNT;
			if(i==ID_DAO_MEN)
				index_ = ID_SHUN_MEN;
			if(i==ID_JIAO_R)
				index_ = ID_HENG_L;
			if(i==ID_HENG_R)
				index_ = ID_JIAO_L;
		}
		if (m_lUserJettonScore[i] > 0 )
		{
			INT nXPos = m_PointJettonNumber[index_].x; 
			INT nYPos = m_PointJettonNumber[index_].y;
			m_bIsShowScore[index_] = true;
			m_ImageMeJetton.TransDrawImage(pDC,nXPos-50,nYPos-20,m_ImageMeJetton.GetWidth(),m_ImageMeJetton.GetHeight(),0,0,RGB(255,0,255));
			pDC->SetTextColor(RGB(0,0,0));
			CRect rcDraw;
			rcDraw.left=nXPos-50;
			rcDraw.right=rcDraw.left+m_ImageMeJetton.GetWidth();
			rcDraw.top=nYPos-20;
			rcDraw.bottom=rcDraw.top+30;
			CString num_str;
			num_str.Format(_T("%d"),m_lUserJettonScore[i]);

			CFont * pOldFont=pDC->SelectObject(&m_ScoreFont);

			pDC->DrawText(num_str,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_CENTER|DT_END_ELLIPSIS);

			pDC->SelectObject(pOldFont);
		}
	}
}

//��ʼ����
void CGameClientView::DispatchCard()
{
	//���ý���
	for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].m_CardItemArray.SetSize(0);
	//���ñ�ʶ
	m_bNeedSetGameRecord=true;
}

//��������
void CGameClientView::FinishDispatchCard(bool bNotScene)
{
	//����ж�
	if (m_bNeedSetGameRecord==false) return;

	//���ñ�ʶ
	m_bNeedSetGameRecord=false;

	//ɾ����ʱ��
	KillTimer(IDI_DISPATCH_CARD);

	////�����˿�
	//for (int i=0; i<CountArray(m_CardControl); ++i) m_CardControl[i].SetCardData(m_cbTableCardArray[i],2);

	//�ƶ�Ӯ��
	bool bWinShunMen, bWinDuiMen, bWinDaoMen;
	DeduceWinner(bWinShunMen, bWinDuiMen, bWinDaoMen);	

	//�����¼
	if (bNotScene)
		SetGameHistory(bWinShunMen, bWinDaoMen, bWinDuiMen);

	//�ۼƻ���
	m_lMeStatisticScore+=m_lMeCurGameScore;
	m_lBankerWinScore=m_lTmpBankerWinScore;

	//����Ӯ��
	SetWinnerSide(bWinShunMen, bWinDuiMen, bWinDaoMen, true);

	//��������
	if (m_lMeCurGameScore>0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_WIN"));
	else if (m_lMeCurGameScore<0) m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_LOST"));
	else m_pGameClientDlg->PlayGameSound(AfxGetInstanceHandle(),TEXT("END_DRAW"));
}
void  CGameClientView::SetAnimFlag(bool flag)
{
	m_bPlayAnim = flag;
	if(flag)
		SetTimer(IDI_CARD_ANIM,30,NULL);
	else
		KillTimer(IDI_CARD_ANIM);
}
//��ֹ����
void CGameClientView::StopAnimal()
{
	//��ʱ��
	KillTimer(IDI_POSTCARD);
	KillTimer(IDI_DISPATCH_CARD);
	KillTimer(IDI_FLASH_WINNER);
	KillTimer(IDI_OPENCARD);

	FinishDispatchCard(true);

	for (int i = 0;i<4;i++)
	{
		m_CardControl[i].m_blGameEnd = true;
		m_CardControl[i].m_blhideOneCard = false;
	}
	m_blMoveFinish = true;
	m_bFlashResult = false;

	/*CGameFrameEngine::GetInstance()->SendMessage(IDM_CONTINUE_CARD,0,0);*/


	TCHAR szBuffer[512]=TEXT("");
	swprintf(szBuffer,CountArray(szBuffer),TEXT("���γɼ����:"));
	m_pGameClientDlg->SendUserMessage(szBuffer,RGB(255,255,255));
	swprintf(szBuffer,CountArray(szBuffer),TEXT("������%I64d"),m_lMeCurGameScore);
	m_pGameClientDlg->SendUserMessage(szBuffer,RGB(255,255,255));

	swprintf(szBuffer,CountArray(szBuffer),TEXT("ׯ�ң�%I64d"),m_lBankerCurGameScore);
	m_pGameClientDlg->SendUserMessage(szBuffer,RGB(255,255,255));


}

//ʤ���߿�
void CGameClientView::FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC)
{
	//�Ϸ��ж�
	//ׯȫ���ж�
	if (true==m_bShowGameResult)
	{
		if (true==m_bWinDaoMen && true==m_bWinShunMen && true==m_bWinDuiMen)
		{
			m_ImagePassAll.TransDrawImage(pDC,m_rcDuiMen.left-74, m_rcDuiMen.top-m_ImagePassAll.GetHeight(),m_ImagePassAll.GetWidth(),m_ImagePassAll.GetHeight(),0,0,RGB(255,0,255));
		}

		if (false==m_bWinDaoMen && false==m_bWinShunMen && false==m_bWinDuiMen)
		{
			m_ImageSweepAll.TransDrawImage(pDC,m_rcDuiMen.left-74, m_rcDuiMen.top-m_ImageSweepAll.GetHeight(),m_ImageSweepAll.GetWidth(),m_ImageSweepAll.GetHeight(),0,0,RGB(255,0,255));
		}
	}
	if (m_cbAreaFlash==0xFF && false==m_bFlashResult) return;

	CRect	rcDuiMen;
	//��ע�ж�
	if (false==m_bFlashResult)
	{

		switch (m_cbAreaFlash)
		{
		case ID_SHUN_MEN:	
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left,m_rcShunMen.top,m_ImageFrameShunMen.GetWidth(),m_ImageFrameShunMen.GetHeight(),0,0,RGB(255,0,255));
				else

					m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left,m_rcDaoMen.top,m_ImageFrameDaoMen.GetWidth(),m_ImageFrameDaoMen.GetHeight(),0,0,RGB(255,0,255));
				break;
			}
		case ID_JIAO_L:
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameJiaoL.TransDrawImage(pDC,m_rcJiaoL.left, m_rcJiaoL.top,m_ImageFrameJiaoL.GetWidth(),m_ImageFrameJiaoL.GetHeight(),0,0,RGB(255,0,255));
				else
					m_ImageFrameHengR.TransDrawImage(pDC,m_rcHengR.left, m_rcHengR.top,m_ImageFrameHengL.GetWidth(),m_ImageFrameHengL.GetHeight(),0,0,RGB(255,0,255));
				break;
			}
		case ID_HENG_L:
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
				{
					m_ImageFrameHengL.TransDrawImage(pDC,m_rcHengL.left,m_rcHengL.top,m_ImageFrameHengL.GetWidth(),m_ImageFrameHengL.GetHeight(),0,0, RGB(255,0,255));
				}
				else
				{
					m_ImageFrameJiaoR.TransDrawImage(pDC,m_rcJiaoR.left,m_rcJiaoR.top,m_ImageFrameJiaoL.GetWidth(),m_ImageFrameJiaoL.GetHeight(),0,0, RGB(255,0,255));
				}
				break;
			}
		case ID_HENG_R:
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
				{
					m_ImageFrameHengR.TransDrawImage(pDC,m_rcHengR.left, m_rcHengR.top,m_ImageFrameHengR.GetWidth(),m_ImageFrameHengR.GetHeight(),0,0, RGB(255,0,255));
				}
				else
				{
					m_ImageFrameJiaoL.TransDrawImage(pDC,m_rcJiaoL.left, m_rcJiaoL.top,m_ImageFrameJiaoR.GetWidth(),m_ImageFrameJiaoR.GetHeight(),0,0, RGB(255,0,255));
				}
				break;
			}
		case ID_DUI_MEN:
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					rcDuiMen = m_rcDuiMen;
				else
					rcDuiMen = m_rcBanker;
				m_ImageFrameDuiMen.TransDrawImage(pDC,rcDuiMen.left, rcDuiMen.top,m_ImageFrameDuiMen.GetWidth(),m_ImageFrameDuiMen.GetHeight(),0,0,RGB(255,0,255));
				break;
			}
		case ID_DAO_MEN:
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left,m_rcDaoMen.top,m_ImageFrameDaoMen.GetWidth(),m_ImageFrameDaoMen.GetHeight(),0,0, RGB(255,0,255));
				else
					m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left,m_rcShunMen.top,m_ImageFrameShunMen.GetWidth(),m_ImageFrameShunMen.GetHeight(),0,0, RGB(255,0,255));
				break;
			}
		case ID_JIAO_R:
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameJiaoR.TransDrawImage(pDC ,m_rcJiaoR.left, m_rcJiaoR.top,m_ImageFrameJiaoR.GetWidth(),m_ImageFrameJiaoR.GetHeight(),0,0,RGB(255,0,255));
				else
					m_ImageFrameHengL.TransDrawImage(pDC ,m_rcHengL.left, m_rcHengL.top,m_ImageFrameHengR.GetWidth(),m_ImageFrameHengR.GetHeight(),0,0,RGB(255,0,255));
				break;
			}
		}
	}
	else
	{
		//ׯȫʤ�ж�
		if (false==m_bWinDaoMen && false==m_bWinShunMen && false==m_bWinDuiMen)
		{
			if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
				rcDuiMen = m_rcBanker;
			else
				rcDuiMen = m_rcDuiMen;
			m_ImageFrameDuiMen.TransDrawImage(pDC,rcDuiMen.left, rcDuiMen.top,m_ImageFrameDuiMen.GetWidth(),m_ImageFrameDuiMen.GetHeight(),0,0,RGB(255,0,255));
			m_ImageSweepAll.TransDrawImage(pDC,m_rcDuiMen.left-74, m_rcDuiMen.top-m_ImageSweepAll.GetHeight(),m_ImageSweepAll.GetWidth(),m_ImageSweepAll.GetHeight(),0,0,RGB(255,0,255));
		}
		else
		{
			//���ж�
			if (true==m_bWinDaoMen)
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top,m_ImageFrameDaoMen.GetWidth(),m_ImageFrameDaoMen.GetHeight(),0,0, RGB(255,0,255));
				else
					m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top,m_ImageFrameShunMen.GetWidth(),m_ImageFrameShunMen.GetHeight(),0,0, RGB(255,0,255));
			}
			if (true==m_bWinShunMen)
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameShunMen.TransDrawImage(pDC,m_rcShunMen.left, m_rcShunMen.top,m_ImageFrameShunMen.GetWidth(),m_ImageFrameShunMen.GetHeight(),0,0,RGB(255,0,255));
				else
					m_ImageFrameDaoMen.TransDrawImage(pDC,m_rcDaoMen.left, m_rcDaoMen.top,m_ImageFrameDaoMen.GetWidth(),m_ImageFrameDaoMen.GetHeight(),0,0,RGB(255,0,255));
			}
			if (true==m_bWinDuiMen)
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					rcDuiMen = m_rcDuiMen;
				else
					rcDuiMen = m_rcBanker;
				m_ImageFrameDuiMen.TransDrawImage(pDC,rcDuiMen.left, rcDuiMen.top,m_ImageFrameDuiMen.GetWidth(),m_ImageFrameDuiMen.GetHeight(),0,0,RGB(255,0,255));
			}
			//���ж�
			if (true==m_bWinShunMen && true==m_bWinDuiMen)
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameJiaoL.TransDrawImage(pDC,m_rcJiaoR.left, m_rcJiaoR.top,m_ImageFrameJiaoL.GetWidth(),m_ImageFrameJiaoL.GetHeight(),0,0,RGB(255,0,255));
				else
					m_ImageFrameHengL.TransDrawImage(pDC,m_rcHengL.left, m_rcHengL.top,m_ImageFrameHengL.GetWidth(),m_ImageFrameHengL.GetHeight(),0,0,RGB(255,0,255));
			}
			if (true==m_bWinShunMen && true==m_bWinDaoMen)
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
				{
					m_ImageFrameHengL.TransDrawImage(pDC,m_rcHengL.left, m_rcHengL.top,m_ImageFrameHengL.GetWidth(),m_ImageFrameHengL.GetHeight(),0,0, RGB(255,0,255));
					m_ImageFrameHengR.TransDrawImage(pDC,m_rcHengR.left, m_rcHengR.top,m_ImageFrameHengR.GetWidth(),m_ImageFrameHengR.GetHeight(),0,0, RGB(255,0,255));
				}
				else
				{
					m_ImageFrameJiaoL.TransDrawImage(pDC,m_rcJiaoL.left, m_rcJiaoL.top,m_ImageFrameJiaoL.GetWidth(),m_ImageFrameJiaoL.GetHeight(),0,0, RGB(255,0,255));
					m_ImageFrameJiaoR.TransDrawImage(pDC,m_rcJiaoR.left, m_rcJiaoR.top,m_ImageFrameJiaoR.GetWidth(),m_ImageFrameJiaoR.GetHeight(),0,0, RGB(255,0,255));
				}
			}
			if (true==m_bWinDuiMen && true==m_bWinDaoMen)
			{
				if (m_wMeChairID == m_wBankerUser && m_wBankerUser!=INVALID_CHAIR)
					m_ImageFrameJiaoR.TransDrawImage(pDC,m_rcJiaoL.left, m_rcJiaoL.top,m_ImageFrameJiaoR.GetWidth(),m_ImageFrameJiaoR.GetHeight(),0,0,RGB(255,0,255));
				else
					m_ImageFrameHengR.TransDrawImage(pDC,m_rcHengR.left, m_rcHengR.top,m_ImageFrameHengR.GetWidth(),m_ImageFrameHengR.GetHeight(),0,0,RGB(255,0,255));
			}
		}
	}
}


//�ƶ�Ӯ��
void CGameClientView::DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen)
{
	//��С�Ƚ�
	bWinShunMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[SHUN_MEN_INDEX],2)==1?true:false;
	bWinDuiMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DUI_MEN_INDEX],2)==1?true:false;
	bWinDaoMen=m_GameLogic.CompareCard(m_cbTableCardArray[BANKER_INDEX],2,m_cbTableCardArray[DAO_MEN_INDEX],2)==1?true:false;

//	if(bWinShunMen)
//	{
//m_pGameClientDlg->m_pIStringMessage->InsertNormalString(TEXT("bWinShunMen==true"));
//	}
//	else
//	{
//		m_pGameClientDlg->m_pIStringMessage->InsertNormalString(TEXT("bWinShunMen==false"));
//
//	}
//	if(bWinDuiMen)
//	{
//		m_pGameClientDlg->m_pIStringMessage->InsertNormalString(TEXT("bWinDuiMen==true"));
//
//	}
//	else
//	{
//		m_pGameClientDlg->m_pIStringMessage->InsertNormalString(TEXT("bWinDuiMen==false"));
//
//	}
//	if(bWinDaoMen)
//	{
//		m_pGameClientDlg->m_pIStringMessage->InsertNormalString(TEXT("bWinDaoMen==true"));
//
//	}
//	else
//	{
//		m_pGameClientDlg->m_pIStringMessage->InsertNormalString(TEXT("bWinDaoMen==false"));
//
//	}
//
//	CString str;
//	str.Format(TEXT("ׯ���˿���:%d  %d"),m_cbTableCardArray[BANKER_INDEX][0],m_cbTableCardArray[BANKER_INDEX][1]);
//	m_pGameClientDlg->m_pIStringMessage->InsertNormalString(str);
//
//	//CString str;
//	str.Format(TEXT("˳���˿���:%d  %d"),m_cbTableCardArray[SHUN_MEN_INDEX][0],m_cbTableCardArray[SHUN_MEN_INDEX][1]);
//	m_pGameClientDlg->m_pIStringMessage->InsertNormalString(str);
//
//	//CString str;
//	str.Format(TEXT("�����˿���:%d  %d"),m_cbTableCardArray[DUI_MEN_INDEX][0],m_cbTableCardArray[DUI_MEN_INDEX][1]);
//	m_pGameClientDlg->m_pIStringMessage->InsertNormalString(str);
//
//	//CString str;
//	str.Format(TEXT("�����˿���:%d  %d"),m_cbTableCardArray[DAO_MEN_INDEX][0],m_cbTableCardArray[DAO_MEN_INDEX][1]);
//	m_pGameClientDlg->m_pIStringMessage->InsertNormalString(str);

}

//�ؼ�����
BOOL CGameClientView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//��ȡID
	WORD wControlID=LOWORD(wParam);

	//�ؼ��ж�
	switch (wControlID)
	{
	case IDC_JETTON_BUTTON_100:
		{
			//���ñ���
			m_lCurrentJetton=10000000L;
			break;
		}
	case IDC_JETTON_BUTTON_500:
		{
			//���ñ���
			m_lCurrentJetton=50000000L;
			break;
		}
	case IDC_JETTON_BUTTON_1000:
		{
			//���ñ���
			m_lCurrentJetton=1000L;
			break;
		}
	case IDC_JETTON_BUTTON_10000:
		{
			//���ñ���
			m_lCurrentJetton=10000L;
			break;
		}
	case IDC_JETTON_BUTTON_100000:
		{
			//���ñ���
			m_lCurrentJetton=100000L;
			break;
		}
	case IDC_JETTON_BUTTON_500000:
		{
			//���ñ���
			m_lCurrentJetton=500000L;
			break;
		}
	case IDC_JETTON_BUTTON_1000000:
		{
			//���ñ���
			m_lCurrentJetton=1000000L;
			break;
		}
	case IDC_JETTON_BUTTON_5000000:
		{
			//���ñ���
			m_lCurrentJetton=5000000L;
			break;
		}
	case IDC_AUTO_OPEN_CARD:
		{
			break;
		}
	case IDC_OPEN_CARD:
		{
			break;
		}
	case IDC_BANK:
		{
			break;
		}
	case IDC_CONTINUE_CARD:
		{
			CGameFrameEngine::GetInstance()->SendMessage(IDM_CONTINUE_CARD,0,0);
			break;
		}
	}

	return CGameFrameView::OnCommand(wParam, lParam);
}

//�ҵ�λ��
void CGameClientView::SetMeChairID(DWORD dwMeUserID)
{
	//�������Ӻ�
	for (WORD wChairID=0; wChairID<MAX_CHAIR; ++wChairID)
	{
		IClientUserItem  *pUserData=GetClientUserItem(wChairID);
		if (NULL!=pUserData && dwMeUserID==pUserData->GetUserID())
		{
			m_wMeChairID=wChairID;
			break;
		}
	}
}

//������ʾ
void CGameClientView::SetDispatchCardTip(enDispatchCardTip DispatchCardTip)
{
	//���ñ���
	m_enDispatchCardTip=DispatchCardTip;

	//���ö�ʱ��
	if (enDispatchCardTip_NULL!=DispatchCardTip) SetTimer(IDI_SHOWDISPATCH_CARD_TIP,2*1000,NULL);		
	else KillTimer(IDI_SHOWDISPATCH_CARD_TIP);

	//���½���
	UpdateGameView(NULL);
}

//�滭ׯ��
void CGameClientView::DrawBankerInfo(CDC *pDC,int nWidth,int nHeight)
{
	//ׯ����Ϣ																											
	pDC->SetTextColor(RGB(255,234,0));

	//��ȡ���
	IClientUserItem  *pUserData = m_wBankerUser==INVALID_CHAIR ? NULL : GetClientUserItem(m_wBankerUser);

	//λ����Ϣ
	CRect StrRect;
	StrRect.top = 8;
	StrRect.bottom = StrRect.top + 15;
	StrRect.left = 5;
	StrRect.right = StrRect.left + 100;
	pDC->DrawText(TEXT("ׯ  �ң�"),StrRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
	StrRect.top = 29;
	StrRect.bottom=StrRect.top+15;
	pDC->DrawText(TEXT("���ֶ���"),StrRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
	StrRect.top = 50;
	StrRect.bottom=StrRect.top+15;
	pDC->DrawText(TEXT("��  ����"),StrRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
	StrRect.top = 71;
	StrRect.bottom=StrRect.top+15;
	pDC->DrawText(TEXT("��  ����"),StrRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);


	pDC->SetTextColor(RGB(255,255,255));
	//ׯ������
	StrRect.top = 8;
	StrRect.bottom = StrRect.top + 15;
	StrRect.left = 55;
	StrRect.right = StrRect.left + 200;
	pDC->DrawText(pUserData==NULL?(m_bEnableSysBanker?TEXT("ϵͳ��ׯ"):TEXT("������ׯ")):pUserData->GetNickName(), StrRect, DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS );

	//ׯ���ܷ�
	StrRect.top = 29;
	StrRect.bottom = StrRect.top + 15;
	StrRect.left = 55;
	StrRect.right = StrRect.left + 200;
	DrawNumberStringWithSpace(pDC,pUserData==NULL?0:pUserData->GetUserScore(), StrRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

	//ׯ�ҳɼ�
	StrRect.top = 50;
	StrRect.bottom = StrRect.top + 15;
	StrRect.left = 55;
	StrRect.right = StrRect.left + 200;
	DrawNumberStringWithSpace(pDC,m_lBankerWinScore,StrRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

	//ׯ�Ҿ���
	StrRect.top = 71;
	StrRect.bottom = StrRect.top + 15;
	StrRect.left = 55;
	StrRect.right = StrRect.left + 200;
	DrawNumberStringWithSpace(pDC,m_wBankerTime,StrRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);


	if (pUserData)
	{
		int index_ = AREA_COUNT;
		if (m_wMeChairID != m_wBankerUser || m_wBankerUser==INVALID_CHAIR)
		{
			index_ = ID_DUI_MEN;
		}
		INT nXPos = m_PointJettonNumber[index_].x; 
		INT nYPos = m_PointJettonNumber[index_].y+35;
		DrawNumberString(pDC,pUserData->GetUserScore(),nXPos,nYPos,true,2);
	}
}

void CGameClientView::SetFirstShowCard(WORD bcCard)
{
	m_bcfirstShowCard =  bcCard;
	//m_CardControl[4].SetCardData(NULL,0);
	//m_CardControl[4].SetCardData(&bcCard,1);
	//m_CardControl[4].m_blGameEnd = true;
	//m_CardControl[4].SetDisplayFlag(true);
	//m_CardControl[4].ShowCardControl(true);
}

//������������
void CGameClientView::SetCheatInfo(BYTE bCardData[], int nWinInfo[])
{
	//��������
	if (bCardData == NULL || nWinInfo == NULL)
	{
		m_bShowCheatInfo = false;
		ZeroMemory(m_bCheatCard, sizeof(m_bCheatCard));
		ZeroMemory(m_nWinInfo, sizeof(m_nWinInfo));
	}
	else
	{
		m_bShowCheatInfo = true;
		memcpy(m_bCheatCard, bCardData, sizeof(m_bCheatCard));
		memcpy(m_nWinInfo, nWinInfo, sizeof(m_nWinInfo));
	}

	UpdateGameView(NULL);
}

//�滭���
void CGameClientView::DrawMeInfo(CDC *pDC,int nWidth,int nHeight)
{
	//�Ϸ��ж�

	if (INVALID_CHAIR==m_wMeChairID) return;

	IClientUserItem  *pMeUserData =  GetClientUserItem(m_wMeChairID);
//#ifdef DEBUG
//	tagUserData *pMeUserData=new tagUserData;
//
//	_sntprintf(pMeUserData->szName,sizeof(pMeUserData->szName),TEXT("�����û�"));
//	pMeUserData->lScore=2000000L;
//
//#endif
	
	if ( pMeUserData != NULL )
	{
		//������ɫ
		pDC->SetTextColor(RGB(255,234,0));


		CRect static rcRect;
		rcRect.left=nWidth-170;
		rcRect.top=10;
		rcRect.bottom=rcRect.top+15;
		rcRect.right=rcRect.left+100;
		pDC->DrawText(TEXT("�ȴ���ׯ���"),rcRect,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		//�ܹ���ע
		LONGLONG lMeJetton=0L;
		for (int nAreaIndex=0; nAreaIndex<AREA_COUNT; ++nAreaIndex) lMeJetton += m_lUserJettonScore[nAreaIndex];

		//����λ��
		CRect static rcDraw;
		int main_top = nHeight-m_ImageNoteMain.GetHeight()-m_ImageMainTimer.GetHeight();
		rcDraw.left=5;
		rcDraw.top=main_top-20;
		rcDraw.bottom=rcDraw.top+15;
		rcDraw.right=rcDraw.left+300;
		pDC->DrawText(TEXT("������ע��"),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
		rcDraw.left=65;
		DrawNumberStringWithSpace(pDC,lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		rcDraw.left=m_ImageMainTimer.GetWidth()+5;
		rcDraw.top=main_top+3;
		rcDraw.bottom=rcDraw.top+15;
		rcDraw.right=rcDraw.left+300;
		pDC->DrawText(TEXT("��  �ң�"),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
		rcDraw.top=main_top+28;
		rcDraw.bottom=rcDraw.top+15;
		pDC->DrawText(TEXT("���ֶ���"),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
		rcDraw.top=main_top+53;
		rcDraw.bottom=rcDraw.top+15;
		pDC->DrawText(TEXT("��  ����"),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		rcDraw.left=nWidth-170;
		rcDraw.top=main_top-20;
		rcDraw.bottom=rcDraw.top+15;
		rcDraw.right=rcDraw.left+300;
		if(m_wBankerUser!=m_wMeChairID)
		{
			pDC->DrawText(TEXT("��ѡ����ֵ"),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
		}


		pDC->SetTextColor(RGB(255,255,255));

		//�滭�ʺ�
		rcDraw.left=m_ImageMainTimer.GetWidth()+50;		
		rcDraw.top=main_top+3;
		rcDraw.bottom=rcDraw.top+15;
		pDC->DrawText(pMeUserData->GetNickName(),lstrlen(pMeUserData->GetNickName()),rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);

		//�滭����
		rcDraw.left=m_ImageMainTimer.GetWidth()+50;
		rcDraw.top=main_top+28;
		rcDraw.bottom=rcDraw.top+15;
		DrawNumberStringWithSpace(pDC,pMeUserData->GetUserScore()-lMeJetton,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
		//if(m_wBankerTime!=0&&m_wBankerUser==m_wMeChairID)
		//rcDraw.top=main_top+40;
		//rcDraw.bottom=rcDraw.top+15;
		//DrawNumberStringWithSpace(pDC,m_wBankerTime,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
		//�滭�ɼ�
		rcDraw.left=m_ImageMainTimer.GetWidth()+50;
		rcDraw.top=main_top+53;
		rcDraw.bottom=rcDraw.top+15;
		DrawNumberStringWithSpace(pDC,m_lMeStatisticScore,rcDraw,DT_VCENTER|DT_SINGLELINE|DT_LEFT|DT_END_ELLIPSIS);
	}
////#ifdef DEBUG
////	delete pMeUserData;
////#endif
}

//�����˿�
void CGameClientView::DrawCheatCard(int nWidth, int nHeight, CDC *pDC)
{
	if (!m_bShowCheatInfo)	return;

	//����ͼƬ
	//CImageHandle HandleCard(&CCardControl::m_ImageCard);

	//��������
	int nXPos[4] = {nWidth/2-48, nWidth/2-360, nWidth/2-48, nWidth/2+265};
	int nYPos[4] = {nHeight/2-340, nHeight/2-152, nHeight/2+45, nHeight/2-152};
	int nXImagePos = 0, nYImagePos = 0;
	CSize cardSize(CCardControl::m_ImageCard.GetWidth()/13, CCardControl::m_ImageCard.GetHeight()/5);

	//�滭�˿�
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			BYTE cbCardData = m_bCheatCard[i][j];
			if (cbCardData == 0)	continue;

			nXImagePos = ((cbCardData&CARD_MASK_VALUE)-1)*cardSize.cx;
			nYImagePos = ((cbCardData&CARD_MASK_COLOR)>>4)*cardSize.cy;

			CCardControl::m_ImageCard.TransDrawImage(pDC,nXPos[i]+j*DEF_X_DISTANCE,nYPos[i],
				cardSize.cx,cardSize.cy,nXImagePos,nYImagePos,RGB(255,0,255));
		}
	}

	//�滭�߿�
	for (int i = 0; i < 3; i++)
	{
		if (m_nWinInfo[i] < 0)
		{
			pDC->Draw3dRect(nXPos[i+1]-1, nYPos[i+1]-1, cardSize.cx+DEF_X_DISTANCE+2, cardSize.cy+2, RGB(255,255,0), RGB(255,255,0));		
			pDC->Draw3dRect(nXPos[i+1]-2, nYPos[i+1]-2, cardSize.cx+DEF_X_DISTANCE+4, cardSize.cy+4, RGB(255,255,0), RGB(255,255,0));		
		}
		else if (m_nWinInfo[i] > 0)
		{
			pDC->Draw3dRect(nXPos[i+1]-1, nYPos[i+1]-1, cardSize.cx+DEF_X_DISTANCE+2, cardSize.cy+2, RGB(255,0,0), RGB(255,0,0));
			pDC->Draw3dRect(nXPos[i+1]-2, nYPos[i+1]-2, cardSize.cx+DEF_X_DISTANCE+4, cardSize.cy+4, RGB(255,0,0), RGB(255,0,0));
		}
		else
		{
			pDC->Draw3dRect(nXPos[i+1]-1, nYPos[i+1]-1, cardSize.cx+DEF_X_DISTANCE+2, cardSize.cy+2, RGB(0,255,255), RGB(0,255,255));
			pDC->Draw3dRect(nXPos[i+1]-2, nYPos[i+1]-2, cardSize.cx+DEF_X_DISTANCE+4, cardSize.cy+4, RGB(0,255,255), RGB(0,255,255));
		}
	}
}	
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//���д��
void CGameClientView::OnBankStorage()
{
}

//����ȡ��
void CGameClientView::OnBankDraw()
{
}

void CGameClientView::OpenAdminWnd()
{
	//����д�Ȩ��
	CGameClientDlg *pGameClientDlg=CONTAINING_RECORD(this,CGameClientDlg,m_GameClientView);
	IClientKernel *pIClientKernel=(IClientKernel *)pGameClientDlg->m_pIClientKernel;
	//if(m_pClientControlDlg != NULL && ((pIClientKernel->GetUserAttribute()->dwUserRight)&UR_GAME_CONTROL)!=0)
	if(m_pClientControlDlg!=NULL && (CUserRight::IsGameCheatUser(pIClientKernel->GetUserAttribute()->dwUserRight)))
	{
		if(!m_pClientControlDlg->IsWindowVisible()) 
			m_pClientControlDlg->ShowWindow(SW_SHOW);
		else 
			m_pClientControlDlg->ShowWindow(SW_HIDE);
	}
}

//�������
void CGameClientView::AllowControl(BYTE cbStatus)
{
	if(m_pClientControlDlg != NULL && m_pClientControlDlg->m_hWnd!=NULL && m_hInst)
	{
		bool bEnable=false;
		switch(cbStatus)
		{
		case GAME_STATUS_FREE: bEnable=true; break;
		case GS_GAME_END:bEnable=false;break;
		case GS_PLACE_JETTON:bEnable=true;break;
		default:bEnable=false;break;
		}
		m_pClientControlDlg->OnAllowControl(bEnable);
	}
}

void CGameClientView::UpdateGameView( CRect *pRect )
{
	if(pRect == NULL){
		CRect rect;
		GetClientRect(&rect);
		InvalidGameView(rect.left,rect.top,rect.Width(),rect.Height());
		return;
	}
	InvalidGameView(pRect->left,pRect->top,pRect->Width(),pRect->Height());
}