#ifndef __ANDROID_CHAT_AND_PRESENT_TOOL_H__
#define __ANDROID_CHAT_AND_PRESENT_TOOL_H__

#include <deque>

enum AndroidMockActionType
{
	None,
	Chat,
	Expresstion,
	PresendTool,
	TirteenSys,
};
struct AndroidMockAction
{
	AndroidMockAction()
		: type(None)
		, dwTargetUserID(0)
		, wToolID(0)
		, wToolCount(0)
		, cbThirteenSysMsgID(0)
	{
		szChatMsg[0] = 0;
	}
	AndroidMockAction& operator = (const AndroidMockAction& rv)
	{
		type = rv.type;
		dwTargetUserID = rv.dwTargetUserID;
		wToolID = rv.wToolID;
		wToolCount = rv.wToolCount;
		cbThirteenSysMsgID = rv.cbThirteenSysMsgID;
		_tcscpy(szChatMsg, rv.szChatMsg);
		return *this;
	}
	AndroidMockActionType type;
	DWORD dwTargetUserID;
	WORD wToolID;
	WORD wToolCount;
	TCHAR szChatMsg[128];
	BYTE cbThirteenSysMsgID;
};

class AndroidChatAndPresent
{
public:
	AndroidChatAndPresent();

	void Reset(IAndroidUserItem *pAndroidUserItem);

	void WaitGameStart();
	void StartGame();
	void WaitGameEnd();
	void EndGame(LONGLONG lWin);
	void ResetStart();

	AndroidMockAction GetAction();

	virtual bool OnTimer(unsigned uTimerID);

private:
	static const unsigned s_uTimeIDBase = 5000;
	void SetTimer(WORD wTimerID, unsigned uElapse);
	void KillTimer(WORD wTimerID);
	LPCTSTR GetCustomMessage(BYTE MessageType);

private:
	IAndroidUserItem *m_pMeAndroidItem;
	bool m_bPlaying;
	bool m_bLastWin;
	std::deque<AndroidMockAction> m_deqMockAction;
};




#endif