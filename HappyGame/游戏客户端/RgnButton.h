#pragma once
#include "afxwin.h"
#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////
//����¼��ص��ӿ�
interface IMouseEvent : public IUnknownEx
{
	//����뿪
	virtual HRESULT __cdecl OnEventMouseLeft(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//����ƶ�
	virtual HRESULT __cdecl OnEventMouseMove(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//��갴ť
	virtual HRESULT __cdecl OnEventLButtonUp(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//��갴ť
	virtual HRESULT __cdecl OnEventLButtonDown(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//��갴ť
	virtual HRESULT __cdecl OnEventRButtonUp(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
	//��갴ť
	virtual HRESULT __cdecl OnEventRButtonDown(UINT uControlID, WPARAM wParam, LPARAM lParam)=NULL;
};
//����ť��
class  CRgnButton : public CButton
{
	//��������
protected:
	bool								m_bHovering;					//������־
	CRgn								m_ButtonRgn;					//��������
	COLORREF							m_crTrans;						//͸����ɫ
	COLORREF							m_crTextColor;					//������ɫ
	CBitImage							m_ImageBack;					//����λͼ

	//�ӿ�ָ��
protected:
	IMouseEvent							* m_pIMouseEvent;				//����¼�

	//��������
public:
	//���캯��
	CRgnButton();
	//��������
	virtual ~CRgnButton();

	//���غ���
protected:
	//�ؼ����໯
	virtual void PreSubclassWindow();
	//����滭����
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//Ĭ�ϻص�����
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//���ܺ���
public:
	//����λͼ
	bool LoadRgnImage(LPCTSTR pszFileName, COLORREF crTrans);
	//����λͼ
	bool LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans);
	//��ȡ���ص��ӿ�
	IMouseEvent * GetMouseEventCallBack() { return m_pIMouseEvent; }
	//�������ص��ӿ�
	void SetMouseEventCallBack(IMouseEvent * pIMouseEvent) { m_pIMouseEvent=pIMouseEvent; };

	//�ڲ����� 
private:
	//��������
	bool CreateControlRgn(COLORREF crTrans);

	//��Ϣ����
protected:
	//������Ϣ
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//������Ϣ
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//����뿪
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CRgnButton)
};

//////////////////////////////////////////////////////////////////////////
