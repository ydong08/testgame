#include "Stdafx.h"
#include "RandCard.h"

// 随机数种子的重设条件
#define RND_RESEED_MIN_PAN  1000 // 最小盘数
#define RND_RESEED_MAX_PAN  2000 // 最大盘数

#define MAKEULONGLONG(low, high)      ((ULONGLONG)(((DWORD)(((DWORD_PTR)(low)) & 0xffffffff)) | ((ULONGLONG)((ULONGLONG)(((DWORD_PTR)(high)) & 0xffffffff))) << 32))

CRandCard::CRandCard()
{
	ZeroMemory(m_cbCardListData,sizeof(m_cbCardListData));
	m_wCardCount = 0;

	m_hCryptProv = NULL;
	m_nNumberNeedSeed = 0;
	m_nCntNumberNeedSeed = 0;
}

CRandCard::~CRandCard()
{
	if (!m_hCryptProv) {
		::CryptReleaseContext(m_hCryptProv, 0);
	}
}

VOID CRandCard::GetRandomCard(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	WashCard();
	CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);
}

BOOL CRandCard::WashCard()
{
	BYTE* pcbTempCardListData = new BYTE[m_wCardCount];
    CopyMemory(pcbTempCardListData,m_cbCardListData,m_wCardCount);

	if (m_nNumberNeedSeed == m_nCntNumberNeedSeed)
	{   // 需要重新设置种子
		m_nCntNumberNeedSeed = 0;
		m_nNumberNeedSeed = RND_RESEED_MIN_PAN + GetCryRandomCardPos2(genNewSeed()) % RND_RESEED_MAX_PAN;
	}
	m_nCntNumberNeedSeed++;

	for ( INT i=0;i<m_wCardCount;i++ )
	{
		INT nDesPos = m_wCardCount - 1 - i;
		INT nWashPos = GetCryRandomCardPos(i);

		CopyMemory(m_cbCardListData + nDesPos,pcbTempCardListData + nWashPos,sizeof(BYTE));

		for ( INT j=nWashPos;j<m_wCardCount-1;j++ )
		{
			pcbTempCardListData[j] = pcbTempCardListData[j+1];
		}
	}

	delete[] pcbTempCardListData;

	return TRUE;
}

INT CRandCard::GetCryRandomCardPos(CONST WORD &wDealedCardCount)
{
	LONGLONG nPos = -1;
	WORD wCanDealNum = m_wCardCount - wDealedCardCount;

	CONST UINT nsize = 1;
	ULONGLONG arRes[nsize] = {};

	if (::CryptGenRandom(m_hCryptProv, nsize * sizeof(ULONGLONG), (BYTE*)arRes)) 
	{
		nPos = arRes[0] % wCanDealNum;
	}
	else 
	{
		//LOG_ERROR(L"CryptGenRandom() failed: GetLastError()=%d\n", GetLastError());
		nPos = GeneRand(0, wCanDealNum - 1);
	}

	return (INT)nPos;
}

INT	CRandCard::GeneRand(DWORD nMin,DWORD nMax)
{
	INT nRes;
	FLOAT fRand = ((FLOAT)rand()) / RAND_MAX;
	nRes = (INT)((nMax - nMin) * fRand + nMin + 0.5);
	return nRes;
}

ULONGLONG CRandCard::GetCryRandomCardPos2(const ULONGLONG &seed)
{
	const UINT nsize = 1;
	ULONGLONG arRes[nsize] = {seed};

	if (::CryptGenRandom(m_hCryptProv, nsize * sizeof(ULONGLONG), (BYTE*)arRes)) {
		return arRes[0];
	} else {
		//LOG_ERROR(L"CryptGenRandom() failed: GetLastError()=%d\n", GetLastError());
		return seed;
	}
}

// 生成新种子
ULONGLONG CRandCard::genNewSeed() 
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

	//return MAKEULONGLONG(GetTickCount(),time(NULL));
}

// 初始化随机数生成器
BOOL CRandCard::InitRNG() 
{
	//随机种子
	GUID Guid;
	CoCreateGuid(&Guid);

	TCHAR szUserName[64] = {0,};

	_sntprintf(szUserName, CountArray(szUserName),
		TEXT("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
		Guid.Data1, Guid.Data2, Guid.Data3,
		Guid.Data4[0], Guid.Data4[1], Guid.Data4[2],
		Guid.Data4[3], Guid.Data4[4], Guid.Data4[5],
		Guid.Data4[6], Guid.Data4[7]);

	if (!::CryptAcquireContext(&m_hCryptProv, szUserName, 0, PROV_RSA_FULL, 0))
	{
		if ( NTE_BAD_KEYSET == GetLastError() )
		{
			if ( !::CryptAcquireContext(&m_hCryptProv, szUserName, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET) )
			{
				//LOG_ERROR(L"CryptAcquireContext() Error! GetLastError()=%d\n", GetLastError());
				m_hCryptProv = NULL;
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CRandCard::SetCardInfo(BYTE cbCardListData[],WORD wCardCount)
{
	if (wCardCount>0)
	{
		ZeroMemory(m_cbCardListData,sizeof(m_cbCardListData));
		CopyMemory(m_cbCardListData,cbCardListData,wCardCount);
		m_wCardCount = wCardCount;

		return TRUE;
	}
	return FALSE;
}

BOOL CRandCard::SetCardInfo(CONST BYTE cbCardListData[],WORD wCardCount)
{
	if (wCardCount>0)
	{
		ZeroMemory(m_cbCardListData,sizeof(m_cbCardListData));
		CopyMemory(m_cbCardListData,cbCardListData,wCardCount);
		m_wCardCount = wCardCount;

		return TRUE;
	}
	return FALSE;
}