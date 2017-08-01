#include "Stdafx.h"
#include "AndroidChatAndPresent.h"
#include <vector>

#define TOOLSID_FLOWER							5
#define TOOLSID_REDPACKET						4

static LPCTSTR s_pszQuickMsg[] = {
    	_T("快点出，我等到花儿也谢了！"), 
		_T("初次见面，各位大侠可得手下留情哦！"),
		_T("你是MM，还是GG？"),
		_T("交个朋友吧，可以告诉我联系方式吗？"),
		_T("你的牌打得太好了，高手中的高手！"),
		_T("手气太差了，郁闷！赶紧让我时来运转吧~"),
		_T("像我这样的高手，那是相当……相当的寂寞。"),
		_T("开心第一，游戏第二，大家互相认识下吧！"),
		_T("手气太好就是没办法~~"),
		_T("跟你一起玩的很开心，但愿下次能再一起玩！"),
		_T("别急着走，我们再战三百回合！")
};

static BYTE s_cbExpresstionIndex[4][6] =
{
	{71,119,55,38,11,34},//等待
	{11,9,54,55,18,79},//输
	{13,0,51,16,52,81},//赢
	{0,1,30,22,29,81}//平常状态
};

static BYTE s_cbProperty[4][3]=
{
	{2,8,9},
	{2,3,13},
	{3,4,5},
	{7,7,7}
};



#define IDT_WAITGAMESTART 1  // 等待其他玩家动作时催牌
#define IDT_WAITGAMEEND 2
#define IDT_CHATWHENWIN 3 // 赢的时候说话
#define IDT_CHATWHENLOST 4 // 输的时候说话
#define IDT_CHATCOMMON  5//普通闲聊


AndroidChatAndPresent::AndroidChatAndPresent()
: m_pMeAndroidItem(NULL)
, m_bPlaying(false)
, m_bLastWin(true)
{
}

void AndroidChatAndPresent::Reset(IAndroidUserItem *pAndroidUserItem)
{
	m_pMeAndroidItem = pAndroidUserItem;
	m_bPlaying = false;
	m_bLastWin = true;
	m_deqMockAction.clear();
	KillTimer(IDT_WAITGAMESTART);
	KillTimer(IDT_WAITGAMEEND);
	KillTimer(IDT_CHATWHENWIN);
	KillTimer(IDT_CHATWHENLOST);
}


void AndroidChatAndPresent::WaitGameStart()
{
	SetTimer(IDT_WAITGAMESTART, 5 + rand() % 5);
}

void AndroidChatAndPresent::StartGame()
{
	KillTimer(IDT_WAITGAMESTART);
	KillTimer(IDT_CHATWHENWIN);
	KillTimer(IDT_CHATWHENLOST);
	SetTimer(IDT_CHATCOMMON,5);
	m_bPlaying = true;
}

void AndroidChatAndPresent::ResetStart()
{
	KillTimer(IDT_WAITGAMESTART);
}

void AndroidChatAndPresent::WaitGameEnd()
{
	SetTimer(IDT_WAITGAMEEND, 5 + rand() % 5);
}

void AndroidChatAndPresent::EndGame(LONGLONG lWin)
{
	KillTimer(IDT_WAITGAMEEND);

	bool bWin = lWin > 0;
	if (bWin)
	{
	   if(lWin > 10000)
	   {
		   SetTimer(IDT_CHATWHENWIN, 2 + rand() % 3);
	   }
	}
	else
	{
		SetTimer(IDT_CHATWHENLOST, 2 + rand() % 3);
	}
	KillTimer(IDT_CHATCOMMON);
	m_bPlaying = false;
	m_bLastWin = bWin;
}

AndroidMockAction AndroidChatAndPresent::GetAction()
{
	AndroidMockAction action;
	if (!m_deqMockAction.empty())
	{
		action = m_deqMockAction.front();
		m_deqMockAction.pop_front();
	}
	return action;
}

bool AndroidChatAndPresent::OnTimer(unsigned uTimerID)
{
	WORD wTimerID = uTimerID - s_uTimeIDBase;
	int nRand = rand() % 100;
	if (wTimerID == IDT_WAITGAMESTART || wTimerID == IDT_WAITGAMEEND)
	{
		if (nRand < 10)
		{		
			int nRandType = rand()%100;
			if(nRandType < 20)
			{
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, s_pszQuickMsg[0]);
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 75)
			{
				AndroidMockAction action;
				action.type = Expresstion;
				action.wToolID= s_cbExpresstionIndex[0][rand()%6];
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 100)
			{
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, GetCustomMessage(0));
				m_deqMockAction.push_back(action);
				return true;
			}
		}
	}
	else if (wTimerID == IDT_CHATWHENWIN)
	{
		if (nRand < 8)
		{
			int nRandType = rand()%100;
			if(nRandType < 20)
			{
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, s_pszQuickMsg[8]);
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 75)
			{
				AndroidMockAction action;
				action.type = Expresstion;
				action.wToolID= s_cbExpresstionIndex[1][rand()%6];
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 100)
			{
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, GetCustomMessage(1));
				m_deqMockAction.push_back(action);
				return true;
			}
		}
	}
	else if (wTimerID == IDT_CHATWHENLOST)
	{
		if (nRand < 8)
		{
			int nRandType = rand()%100;
			if(nRandType < 20)
			{
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, s_pszQuickMsg[5]);
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 75)
			{
				AndroidMockAction action;
				action.type = Expresstion;
				action.wToolID= s_cbExpresstionIndex[2][rand()%6];
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 100)
			{
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, GetCustomMessage(2));
				m_deqMockAction.push_back(action);
				return true;
			}
		}
	}
	else if (wTimerID == IDT_CHATCOMMON)
	{
		if (nRand < 2)
		{
			int nRandType = rand()%100;
			if(nRandType < 20)
			{
				BYTE cbCommonMsg[7] = {1,2,3,5,6,9,10};
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, s_pszQuickMsg[cbCommonMsg[rand()%7]]);
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 70)
			{
				AndroidMockAction action;
				action.type = Expresstion;
				action.wToolID= s_cbExpresstionIndex[3][rand()%6];
				m_deqMockAction.push_back(action);
				return true;
			}
			else if(nRandType < 100)
			{
				AndroidMockAction action;
				action.type = Chat;
				_tcscpy(action.szChatMsg, GetCustomMessage(3));
				m_deqMockAction.push_back(action);
				return true;
			}
			return true;
			
		}
		else
		{
			SetTimer(IDT_CHATCOMMON,5);
		}
	}
	return false;
}

void AndroidChatAndPresent::SetTimer(WORD wTimerID, unsigned uElapse)
{
	unsigned uTimerID = s_uTimeIDBase + wTimerID;
	m_pMeAndroidItem->SetGameTimer(uTimerID, uElapse);
}

void AndroidChatAndPresent::KillTimer(WORD wTimerID)
{
	unsigned uTimerID = s_uTimeIDBase + wTimerID;
	m_pMeAndroidItem->KillGameTimer(uTimerID);
}

LPCTSTR AndroidChatAndPresent::GetCustomMessage(BYTE MessageType)
{
	ASSERT(MessageType < 4);
	if(MessageType >=4) return TEXT("妈的个巴子的的");
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH];
	_stprintf(szFileName,sizeof(szFileName),TEXT("%s\\BaccaratNewConfig.ini"),szPath);

	TCHAR OutBuf[128];
	ZeroMemory(OutBuf,sizeof(OutBuf));
	int nRandIndex=rand()%200;
	switch(MessageType)
	{
	case 0:
		{
			CString str;
			str.Format(TEXT("WaitUser%d"),nRandIndex);
			GetPrivateProfileString(TEXT("AndroidChat"),str,TEXT("速度"),OutBuf,128,szFileName);
			return OutBuf;
		}
	case 1:
		{
			CString str;
			str.Format(TEXT("Win%d"),nRandIndex);
			GetPrivateProfileString(TEXT("AndroidChat"),str,TEXT("哈哈"),OutBuf,128,szFileName);
			return OutBuf;
		}
	case 2:
		{
			CString str;
			str.Format(TEXT("Lost%d"),nRandIndex);
			GetPrivateProfileString(TEXT("AndroidChat"),str,TEXT("悲剧"),OutBuf,128,szFileName);
			return OutBuf;
		}
	case 3:
		{
			CString str;
			str.Format(TEXT("Common%d"),nRandIndex);
			GetPrivateProfileString(TEXT("AndroidChat"),str,TEXT("你哪里人啊"),OutBuf,128,szFileName);
			return OutBuf;
		}
	}
    return TEXT("亲娘，咋又出错了");

}