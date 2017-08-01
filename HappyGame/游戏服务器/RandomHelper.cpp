#include "stdafx.h"
#include "RandomHelper.h"

// 随机数种子的重设条件
#define RND_RESEED_MIN_PAN  1000 // 最小盘数
#define RND_RESEED_MAX_PAN  2000 // 最大盘数

CRandomHelper::CRandomHelper()
{
	InitRNG();
}

CRandomHelper::~CRandomHelper()
{
	if (!m_hCryptProv) {
		::CryptReleaseContext(m_hCryptProv, 0);
	}
}

INT	CRandomHelper::GeneRand(DWORD nMin, DWORD nMax)
{
	INT nRes;
	FLOAT fRand = ((FLOAT)rand()) / RAND_MAX;
	nRes = (INT)((nMax - nMin) * fRand + nMin + 0.5);
	return nRes;
}

// 生成新种子
ULONGLONG CRandomHelper::genNewSeed()
{
	//随机种子
	GUID Guid;
	CoCreateGuid(&Guid);
	WORD w1 = GetTickCount() & 0xFFFF;
	WORD w2 = Guid.Data1 & 0xFFFF;
	WORD w3 = Guid.Data2 & 0xFFFF;
	WORD w4 = Guid.Data3 & 0xFFFF;

	return (ULONGLONG)(((ULONGLONG)w1) << 48)
		| (ULONGLONG)(((ULONGLONG)w2) << 32)
		| (ULONGLONG)(((ULONGLONG)w3) << 16)
		| ((ULONGLONG)w4);
}

// 初始化随机数生成器
BOOL CRandomHelper::InitRNG()
{
	//随机种子
	GUID Guid;
	CoCreateGuid(&Guid);

	TCHAR szUserName[64] = { 0, };

	_sntprintf(szUserName, CountArray(szUserName),
		TEXT("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
		Guid.Data1, Guid.Data2, Guid.Data3,
		Guid.Data4[0], Guid.Data4[1], Guid.Data4[2],
		Guid.Data4[3], Guid.Data4[4], Guid.Data4[5],
		Guid.Data4[6], Guid.Data4[7]);

	if (!::CryptAcquireContext(&m_hCryptProv, szUserName, 0, PROV_RSA_FULL, 0))
	{
		if (NTE_BAD_KEYSET == GetLastError())
		{
			if (!::CryptAcquireContext(&m_hCryptProv, szUserName, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET))
			{
				//LOG_ERROR(L"CryptAcquireContext() Error! GetLastError()=%d\n", GetLastError());
				m_hCryptProv = NULL;
				return FALSE;
			}
		}
	}
	return TRUE;
}


ULONGLONG CRandomHelper::GetCryRandomSeed(const ULONGLONG &seed)
{
	const UINT nsize = 1;
	ULONGLONG arRes[nsize] = { seed };

	if (::CryptGenRandom(m_hCryptProv, nsize * sizeof(ULONGLONG), (BYTE*)arRes)) {
		return arRes[0];
	}
	else {
		return seed;
	}
}

INT CRandomHelper::GetCryRandom(CONST DWORD &dwMin,CONST DWORD &dwMax)
{
	LONGLONG nPos = -1;
	DWORD wCanDealNum = dwMax - dwMin + 1;

	CONST UINT nsize = 1;
	ULONGLONG arRes[nsize] = {};

	if (::CryptGenRandom(m_hCryptProv, nsize * sizeof(ULONGLONG), (BYTE*)arRes))
	{
		nPos = arRes[0] % wCanDealNum;
		nPos += dwMin;
	}
	else
	{
		nPos = GeneRand(dwMin, dwMax);
	}

	return (INT)nPos;
}

INT CRandomHelper::Random(CONST DWORD &dwMin, CONST DWORD &dwMax)
{
	if (m_nNumberNeedSeed == m_nCntNumberNeedSeed)
	{   // 需要重新设置种子
		m_nCntNumberNeedSeed = 0;
		m_nNumberNeedSeed = RND_RESEED_MIN_PAN + GetCryRandomSeed(genNewSeed()) % RND_RESEED_MAX_PAN;
	}
	m_nCntNumberNeedSeed++;

	return GetCryRandom(dwMin,dwMax);
}

INT CRandomHelper::Random(CONST DWORD &dwMaxIncr)
{
	return Random(0,dwMaxIncr-1);
}