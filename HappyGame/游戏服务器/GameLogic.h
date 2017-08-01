#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "RandCard.h"

//////////////////////////////////////////////////////////////////////////
//����
#define	CT_ERROR					0									//��������
#define	CT_POINT					20									//��������
#define	CT_SPECIAL_19				19									//�ظ�
#define	CT_SPECIAL_18				18									//���
#define	CT_SPECIAL_17				17									//�����
#define	CT_SPECIAL_16				16									//����
#define	CT_SPECIAL_15				15									//����
#define	CT_SPECIAL_14				14									//�Ӱ�
#define	CT_SPECIAL_13				13									//�Ӿ�
#define	CT_SPECIAL_12				12									//˫����
#define	CT_SPECIAL_11				11									//˫ͭ��
#define	CT_SPECIAL_10				10									//˫��ͷ
#define	CT_SPECIAL_9				9									//˫��ͷ
#define	CT_SPECIAL_8				8									//˫���
#define	CT_SPECIAL_7				7									//˫��
#define	CT_SPECIAL_6				6									//˫÷
#define	CT_SPECIAL_5				5									//˫��
#define	CT_SPECIAL_4				4									//˫��
#define	CT_SPECIAL_3				3									//˫��
#define	CT_SPECIAL_2				2									//˫��
#define	CT_SPECIAL_1				1									//����

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//��������
#define	ST_VALUE					1									//��ֵ����
#define	ST_LOGIC					2									//�߼�����

//�˿���Ŀ
#define CARD_COUNT					32									//�˿���Ŀ
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////	
////���Ժ���
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

//��Ϸ�߼�
class CGameLogic
{
	//��������
private:
	static const BYTE				m_cbCardListData[CARD_COUNT];		//�˿˶���

	//��������
public:
	//���캯��
	CGameLogic();
	//��������
	virtual ~CGameLogic();

	//���ͺ���
public:
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return (cbCardData&LOGIC_MASK_COLOR)>>4; }

	//���ƺ���
public:
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);

	//�߼�����
public:
	//��ȡ�Ƶ�
	BYTE GetCardListPip(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��С�Ƚ�
	int CompareCard(const BYTE cbFirstCardData[], BYTE cbFirstCardCount,const BYTE cbNextCardData[], BYTE cbNextCardCount);
	//�߼���С
	BYTE GetCardLogicValue(BYTE cbCardData);
	//��ȡ�ĸ��ƴ�С����
	void GetCardOrder(const BYTE cbCardArray[][2], BYTE cbArrayCount, BYTE cbOrder[], BYTE &cbMinIndex, BYTE &cbMaxIndex);

protected:
	CRandCard m_RandCard;
};

//////////////////////////////////////////////////////////////////////////

#endif
