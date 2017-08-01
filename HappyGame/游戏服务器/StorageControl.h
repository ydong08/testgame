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
	LONGLONG	m_llStroageScore;		//���ֵ
	LONGLONG	m_llSafeScore;			//��ȫ��
	INT			m_nAbsWinRate;			//��Ӯ��
	INT			m_nRandNum;				//���ֵ
	TCHAR		m_szConfigFileName[MAX_PATH];
	TCHAR		m_szRoomName[32];
	TCHAR		m_szMsg[MAX_PATH];
	CCriticalSection	m_cs;
	bool		m_bAutoUpdateStorage;
	bool		m_bControled;			//����Ƿ��ܿ�����
	int			m_nRetry;				//���Դ���

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
	//��ȡ����
	LONGLONG GetPrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lDefault, LPCTSTR lpFileName);
	//д����
	VOID WritePrivateProfileInt64(LPCTSTR lpAppName, LPCTSTR lpKeyName, LONGLONG lValue, LPCTSTR lpFileName);
};


#endif