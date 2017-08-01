#ifndef RANDCARD_HEAD_FILE
#define RANDCARD_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <wincrypt.h>

#define MAX_CARD_NUM  512

class CRandCard
{
public:
	CRandCard();
	virtual ~CRandCard();

protected:
	HCRYPTPROV m_hCryptProv;		// CryptoAPI句柄
	INT m_nNumberNeedSeed;			//需要重新设置种子的加密次数
	INT m_nCntNumberNeedSeed;		//用于nNumberNeedSeed的计数器（从0开始递增，达到nNumberNeedSeed后重置为0）
	BYTE m_cbCardListData[MAX_CARD_NUM];
	WORD m_wCardCount;

protected:
	INT GetCryRandomCardPos(CONST WORD &wDealedCardCount);
	INT	GeneRand(DWORD nMin,DWORD nMax);
	ULONGLONG GetCryRandomCardPos2(const ULONGLONG &seed);
	ULONGLONG genNewSeed();
	BOOL WashCard();

public:
	BOOL InitRNG();
	BOOL SetCardInfo(BYTE cbCardListData[],WORD wCardCount);
	BOOL SetCardInfo(CONST BYTE cbCardListData[],WORD wCardCount);
	VOID GetRandomCard(BYTE cbCardBuffer[], BYTE cbBufferCount);
};

#endif