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
	HCRYPTPROV m_hCryptProv;		// CryptoAPI���
	INT m_nNumberNeedSeed;			//��Ҫ�����������ӵļ��ܴ���
	INT m_nCntNumberNeedSeed;		//����nNumberNeedSeed�ļ���������0��ʼ�������ﵽnNumberNeedSeed������Ϊ0��
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