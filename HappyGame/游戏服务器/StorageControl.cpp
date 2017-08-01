#include "stdafx.h"
#include "StorageControl.h"

#define MAX_RETRY					100									//最大重新随机次数

CStorageControl::CStorageControl()
{
	m_llStroageScore = 0;	
	m_llSafeScore = 10000000;	
	m_nAbsWinRate = 10;
	m_nRandNum = 0;
	memset(m_szConfigFileName,0,sizeof(m_szConfigFileName));
	memset(m_szRoomName,0,sizeof(m_szRoomName));
	memset(m_szMsg,0,sizeof(m_szMsg));
	m_bAutoUpdateStorage = true;
	m_bControled = false;
}

CStorageControl::~CStorageControl()
{

}

VOID CStorageControl::InitConfig(TCHAR * lpszConfigFileName,TCHAR * lpszRoomName,bool bAutoUpdateStorage)
{
	SetConfigName(lpszConfigFileName,lpszRoomName);
	ReadConfig();
	m_bAutoUpdateStorage=bAutoUpdateStorage;
}

VOID CStorageControl::SetConfigName(TCHAR * lpszConfigFileName,TCHAR * lpszRoomName)
{
	if (lpszConfigFileName)
	{
		lstrcpyn(m_szConfigFileName,lpszConfigFileName,CountArray(m_szConfigFileName));
	}
	if (lpszRoomName)
	{
		lstrcpyn(m_szRoomName,lpszRoomName,CountArray(m_szRoomName));
	}
}

VOID CStorageControl::ReadConfig()
{
	CWHDataLocker lock(m_cs);
	if (m_szConfigFileName[0]==0 || m_szRoomName[0]==0)
	{
		return;
	}

	m_nAbsWinRate = GetPrivateProfileInt(m_szRoomName, TEXT("AbsWinRate"), 10, m_szConfigFileName);

	m_llStroageScore = GetPrivateProfileInt64(m_szRoomName, TEXT("AndroidWinScore"),10000000 , m_szConfigFileName);

	m_llSafeScore = GetPrivateProfileInt64(m_szRoomName, TEXT("AndroidSafeScore"),10000000 , m_szConfigFileName);
}

bool CStorageControl::Judge(LONGLONG llSystemScore)
{
	CWHDataLocker lock(m_cs);

	if (llSystemScore<0)
	{
		bool bResult = IsMaxRetry();
		m_nRetry++;
		if (m_llStroageScore <= -m_llSafeScore)
		{
			CTraceService::TraceString(TEXT("触发库存安全线，100%赢"),TraceLevel_Info);
			m_bControled = true;
			return bResult;
		}
		else if (m_llStroageScore < -m_llSafeScore*0.75)
		{	
			if(m_nRandNum<75)
			{
				m_bControled = true;
				CTraceService::TraceString(TEXT("触发库存安全线，75%赢，成功"),TraceLevel_Info);
				return bResult;
			}
			else
			{
				CTraceService::TraceString(TEXT("触发库存安全线，75%赢，失败"),TraceLevel_Info);
			}
		}
		else if (m_llStroageScore < -m_llSafeScore*0.5)
		{	
			if(m_nRandNum<50)
			{
				m_bControled = true;
				CTraceService::TraceString(TEXT("触发库存安全线，50%赢，成功"),TraceLevel_Info);
				return bResult;
			}
			else
			{
				CTraceService::TraceString(TEXT("触发库存安全线，50%赢，失败"),TraceLevel_Info);
			}
		}
		else if (m_nRandNum<m_nAbsWinRate)
		{
			m_bControled = true;
			CTraceService::TraceString(TEXT("触发系统必赢"),TraceLevel_Info);
			return bResult;
		}
	}

	if (m_bAutoUpdateStorage)
		ModifyStroageScore(llSystemScore);
	return true;
}

VOID CStorageControl::ResetCondition()
{ 
	ReadConfig();
	m_nRandNum = m_rand.Random(100); 
	m_bControled = false;
	m_nRetry = 0;
}

bool CStorageControl::IsMaxRetry()
{
	return (m_nRetry>MAX_RETRY)?true:false;
}

VOID CStorageControl::ModifyStroageScore(LONGLONG llSystemScore)
{
	CWHDataLocker lock(m_cs);
	m_llStroageScore += llSystemScore;
	WritePrivateProfileInt64(m_szRoomName,TEXT("AndroidWinScore"),m_llStroageScore,m_szConfigFileName);
	memset(m_szMsg,0,sizeof(m_szMsg));
	_sntprintf(m_szMsg,CountArray(m_szMsg),TEXT("游戏结束,系统得分:%I64d,当前库存:%I64d,必胜率:%d％"),llSystemScore,m_llStroageScore,m_nAbsWinRate);
	CTraceService::TraceString(m_szMsg,TraceLevel_Info);
}

VOID CStorageControl::SetStroageScore(LONGLONG llValue)
{
	CWHDataLocker lock(m_cs);
	m_llStroageScore = llValue;
	WritePrivateProfileInt64(m_szRoomName,TEXT("AndroidWinScore"),m_llStroageScore,m_szConfigFileName);
	memset(m_szMsg,0,sizeof(m_szMsg));
	_sntprintf(m_szMsg,CountArray(m_szMsg),TEXT("修改库存成功,当前库存:%I64d"),m_llStroageScore);
	CTraceService::TraceString(m_szMsg,TraceLevel_Info);
}

//读取长整
LONGLONG CStorageControl::GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName)
{
	//变量定义
	TCHAR OutBuf[MAX_PATH] = { 0, };
	LONGLONG lNumber = 0;

	GetPrivateProfileString(lpAppName, lpKeyName, TEXT(""), OutBuf, CountArray(OutBuf), lpFileName);

	if (OutBuf[0] != 0)
		lNumber = _ttoi64(OutBuf);
	else
		lNumber = lDefault;

	return lNumber;
}

VOID CStorageControl::WritePrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lValue, LPCTSTR lpFileName)
{
	TCHAR szValue[MAX_PATH] = { 0, };
	_sntprintf(szValue,CountArray(szValue),TEXT("%I64d"),lValue);
	WritePrivateProfileString(lpAppName,lpKeyName,szValue,lpFileName);
}
