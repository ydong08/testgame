#ifndef RANDOM_HELPER_SINK_H_
#define RANDOM_HELPER_SINK_H_

#pragma once
#include "stdafx.h"
#include <wincrypt.h>

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

class CRandomHelper
{
public:
	CRandomHelper();
	virtual ~CRandomHelper();

protected:
	HCRYPTPROV m_hCryptProv;		// CryptoAPI���
	INT m_nNumberNeedSeed;			//��Ҫ�����������ӵļ��ܴ���
	INT m_nCntNumberNeedSeed;		//����nNumberNeedSeed�ļ���������0��ʼ�������ﵽnNumberNeedSeed������Ϊ0��

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