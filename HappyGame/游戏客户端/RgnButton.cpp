#include "Stdafx.h"
#include ".\rgnbutton.h"
IMPLEMENT_DYNAMIC(CRgnButton, CButton)

BEGIN_MESSAGE_MAP(CRgnButton, CButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()




//////////////////////////////////////////////////////////////////////////

//���캯��
CRgnButton::CRgnButton()
{
	m_bHovering=false;
	m_pIMouseEvent=NULL;
	m_crTrans=RGB(255,0,255);
	m_crTextColor=RGB(0,0,0);
}

//��������
CRgnButton::~CRgnButton()
{
}

//���󸽼ӵ����д���
void CRgnButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	SendMessage(WM_SETFONT,(WPARAM)&CSkinResourceManager::GetInstance()->GetDefaultFont(),TRUE);
	return;
}

//������Ϣ
int CRgnButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);
	SendMessage(WM_SETFONT,(WPARAM)&CSkinResourceManager::GetInstance()->GetDefaultFont(),TRUE);
	return 0;
}

//����λͼ
bool CRgnButton::LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans)
{
	m_ImageBack.LoadFromResource(hInstance,uBitmapID);
	return CreateControlRgn(crTrans);
}

//����λͼ
bool CRgnButton::LoadRgnImage(LPCTSTR pszFileName, COLORREF crTrans)
{
	m_ImageBack.Load(pszFileName);
	return CreateControlRgn(crTrans);
}

//��������
bool CRgnButton::CreateControlRgn(COLORREF crTrans)
{
	////CImageHandle ImageHandle(&m_ImageBack);
	//if (ImageHandle.IsResourceValid())
	{
		//������ʱͼ
		CBitImage BufferImage;
		int nButtonWidth=m_ImageBack.GetWidth()/5;
		int nButtomHeight=m_ImageBack.GetHeight();
		BufferImage.Create(nButtonWidth,nButtomHeight,32);
		m_ImageBack.BitBlt(BufferImage.GetDC(),0,0,nButtonWidth,nButtomHeight,0,0);
		BufferImage.ReleaseDC();

		//��������
		CRgn RgnControl;
		BufferImage.CreateImageRegion(RgnControl,crTrans);

		//��������
		if (RgnControl.GetSafeHandle()!=NULL)
		{
			//���ñ���
			m_crTrans=crTrans;

			//���ð�ť
			SetWindowRgn(RgnControl,TRUE);
			SetWindowPos(NULL,0,0,nButtonWidth,nButtomHeight,SWP_NOMOVE|SWP_NOACTIVATE);

			//��������
			m_ButtonRgn.DeleteObject();
			m_ButtonRgn.Attach(RgnControl.Detach());

			return true;
		}
	}

	return false;
}

//����ƶ���Ϣ
void CRgnButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bHovering==false)
	{
		//ע����Ϣ
		m_bHovering=true;
		Invalidate(FALSE);
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);
	}

	__super::OnMouseMove(nFlags, point);
}

//����뿪��Ϣ
LRESULT CRgnButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bHovering=false;
	Invalidate(FALSE);
	return 0;
}

//����滭����
void CRgnButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//�������
	CRect ClientRect;
	GetClientRect(&ClientRect);
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

	//���ر���ͼ
	////CImageHandle ImageHandle(&m_ImageBack);
	//if (m_ImageBack.m_hBitmap != NULL)
	{
		//����λͼλ��
		int iPartWidth=m_ImageBack.GetWidth()/5,iDrawPos=0;
		if (bDisable) iDrawPos=iPartWidth*4;
		else if (bButtonDown) iDrawPos=iPartWidth;
		else if (m_bHovering) iDrawPos=iPartWidth*3;
		else if (lpDrawItemStruct->itemState&ODS_FOCUS) iDrawPos=iPartWidth*2;

		//�滭����ͼ
		CDC * pDesDC=CDC::FromHandle(lpDrawItemStruct->hDC);
		m_ImageBack.TransDrawImage(pDesDC,0,0,iPartWidth,m_ImageBack.GetHeight(),iDrawPos,0,m_crTrans);
	}

	//��ȡ����
	CString strText;
	GetWindowText(strText);

	//�滭����
	ClientRect.top+=1;
	::SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);
	if (bDisable) ::SetTextColor(lpDrawItemStruct->hDC,GetSysColor(COLOR_GRAYTEXT));
	else ::SetTextColor(lpDrawItemStruct->hDC,m_crTextColor);
	DrawText(lpDrawItemStruct->hDC,strText,strText.GetLength(),ClientRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	return;
}

//Ĭ�ϻص�����
LRESULT CRgnButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pIMouseEvent)
	{
		switch (message)
		{
		case WM_MOUSELEAVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseLeft(uButtonID,wParam,lParam);
				break;
			}
		case WM_MOUSEMOVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseMove(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonUp(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonUp(uButtonID,wParam,lParam);
				break;
			}
		}
	}
	return __super::DefWindowProc(message, wParam, lParam);
}

//��������
BOOL CRgnButton::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

