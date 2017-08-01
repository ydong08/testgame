#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "RandCard.h"

//////////////////////////////////////////////////////////////////////////
//牌型
#define	CT_ERROR					0									//错误类型
#define	CT_POINT					20									//点数类型
#define	CT_SPECIAL_19				19									//地杠
#define	CT_SPECIAL_18				18									//天杠
#define	CT_SPECIAL_17				17									//天九王
#define	CT_SPECIAL_16				16									//杂五
#define	CT_SPECIAL_15				15									//杂七
#define	CT_SPECIAL_14				14									//杂八
#define	CT_SPECIAL_13				13									//杂九
#define	CT_SPECIAL_12				12									//双幺五
#define	CT_SPECIAL_11				11									//双铜锤
#define	CT_SPECIAL_10				10									//双红头
#define	CT_SPECIAL_9				9									//双斧头
#define	CT_SPECIAL_8				8									//双板凳
#define	CT_SPECIAL_7				7									//双长
#define	CT_SPECIAL_6				6									//双梅
#define	CT_SPECIAL_5				5									//双和
#define	CT_SPECIAL_4				4									//双人
#define	CT_SPECIAL_3				3									//双地
#define	CT_SPECIAL_2				2									//双天
#define	CT_SPECIAL_1				1									//至尊宝

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//排序类型
#define	ST_VALUE					1									//数值排序
#define	ST_LOGIC					2									//逻辑排序

//扑克数目
#define CARD_COUNT					32									//扑克数目
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////	
////调试函数
//static void MyMsgBox(LPCSTR lpszFormat, ...)
//{
//	va_list args;
//	int		nBuf;
//	TCHAR	szBuffer[512];
//
//	va_start(args, lpszFormat);
//	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer)-sizeof(TCHAR), lpszFormat, args);
//#ifdef _DEBUG
//	AfxMessageBox(szBuffer);
//#else
//	TCHAR	szFormat[512];
//	_stprintf(szFormat, _T("[JJ]: %s"), szBuffer);
//	OutputDebugString(szFormat);
//#endif
//
//	va_end(args);
//}
//#ifdef _DEBUG
//static bool MyDebug(LPCSTR lpszFormat, ...)
//{
//	va_list args;
//	int		nbuf;
//	TCHAR	szBuffer[512];
//	TCHAR	szFormat[512];
//
//	va_start(args, lpszFormat);
//	nbuf = _vsntprintf(szBuffer, sizeof(szBuffer)-sizeof(TCHAR), lpszFormat, args);
//	_stprintf(szFormat, _T("[jj]: %s"), szBuffer);
//	OutputDebugString(szFormat);
//
//	va_end(args);
//
//	return true;
//}
//#endif
//////////////////////////////////////////////////////////////////////////

//游戏逻辑
class CGameLogic
{
	//变量定义
private:
	static const BYTE				m_cbCardListData[CARD_COUNT];		//扑克定义

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//类型函数
public:
	//获取数值
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE cbCardData) { return (cbCardData&LOGIC_MASK_COLOR)>>4; }

	//控制函数
public:
	//混乱扑克
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//排列扑克
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);

	//逻辑函数
public:
	//获取牌点
	BYTE GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);
	//获取牌型
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//大小比较
	int CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount);
	//逻辑大小
	BYTE GetCardLogicValue(BYTE cbCardData);
	//获取四付牌大小排列
	void GetCardOrder(const BYTE cbCardArray[][2], BYTE cbArrayCount, BYTE cbOrder[], BYTE &cbMinIndex, BYTE &cbMaxIndex);

protected:
	CRandCard m_RandCard;
};

//////////////////////////////////////////////////////////////////////////

#endif
