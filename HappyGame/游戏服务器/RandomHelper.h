#ifndef RANDOM_HELPER_SINK_H_
#define RANDOM_HELPER_SINK_H_

#pragma once
#include "stdafx.h"
#include <wincrypt.h>

//数组维数
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

class CRandomHelper
{
public:
	CRandomHelper();
	virtual ~CRandomHelper();

protected:
	HCRYPTPROV m_hCryptProv;		// CryptoAPI句柄
	INT m_nNumberNeedSeed;			//需要重新设置种子的加密次数
	INT m_nCntNumberNeedSeed;		//用于nNumberNeedSeed的计数器（从0开始递增，达到nNumberNeedSeed后重置为0）

protected:
	ULONGLONG GetCryRandomSeed(const ULONGLONG &seed);
	ULONGLONG genNewSeed();
	BOOL InitRNG();
	INT GetCryRandom(CONST DWORD &dwMin, CONST DWORD &dwMax);
	INT	GeneRand(DWORD nMin, DWORD nMax);

public:
	INT Random(CONST DWORD &dwMin, CONST DWORD &dwMax);
	INT Random(CONST DWORD &dwMaxIncr);
};

#endif