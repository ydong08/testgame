#ifndef STROAGE_CONTORL_SINK_H_
#define STROAGE_CONTORL_SINK_H_

#pragma once
#include "stdafx.h"
#include "RandomHelper.h"

class CStorageControl
{
public:
	CStorageControl();
	virtual ~CStorageControl();

protected:
	CRandomHelper	m_rand;
	LONGLONG	m_llStroageScore;		//库存值
	LONGLONG	m_llSafeScore;			//安全线
	INT			m_nAbsWinRate;			//必赢率
	INT			m_nRandNum;				//随机值
	TCHAR		m_szConfigFileName[MAX_PATH];
	TCHAR		m_szRoomName[32];
	TCHAR		m_szMsg[MAX_PATH];
	CCriticalSection	m_cs;
	bool		m_bAutoUpdateStorage;
	bool		m_bControled;			//结果是否受控制了
	int			m_nRetry;				//重试次数

public:
	VOID ResetCondition();
	VOID SetConfigName(TCHAR * lpszConfigFileName,TCHAR * lpszRoomName);
	VOID ReadConfig();
	VOID InitConfig(TCHAR * lpszConfigFileName,TCHAR * lpszRoomName,bool bAutoUpdateStorage=true);
	bool Judge(LONGLONG llSystemScore);
	VOID ModifyStroageScore(LONGLONG llSystemScore);
	LONGLONG GetStroageScore(){return m_llStroageScore;};
	LONGLONG GetSafeScore(){return m_llSafeScore;};
	VOID SetStroageScore(LONGLONG llValue);
	bool GetControled(){ return m_bControled;};
	bool IsMaxRetry();

protected:
	//读取长整
	LONGLONG GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName);
	//写长整
	VOID WritePrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lValue, LPCTSTR lpFileName);
};


#endif