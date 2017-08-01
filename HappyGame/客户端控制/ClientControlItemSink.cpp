// ClientControlItemSink.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"


// CClientControlItemSinkDlg �Ի���

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
}

void CClientControlItemSinkDlg::ReSetAdminWnd()
{

	if(m_cbControlStyle!=CS_BANKER_WIN&&m_cbControlStyle!=CS_BANKER_LOSE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(0);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->SetCheck(1);
		((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(m_cbControlStyle==CS_BANKER_WIN?1:0);
		((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(m_cbControlStyle==CS_BANKER_LOSE?1:0);
	}

	if(m_cbControlStyle==CS_BET_AREA)
	{
	   ((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	}
	((CButton*)GetDlgItem(IDC_CHECK_SHUNMEN))->SetCheck(m_bWinArea[0]);
	((CButton*)GetDlgItem(IDC_CHECK_DUIMEN))->SetCheck(m_bWinArea[1]);
	((CButton*)GetDlgItem(IDC_CHECK_DAOMEN))->SetCheck(m_bWinArea[2]);
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes>0?m_cbExcuteTimes-1:-1);

	OnRadioClick();
}

//��ȡ�ǳ�
void __cdecl CClientControlItemSinkDlg::GetUserNickName(CString &strNickName)
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->GetWindowText(strNickName); 
}

void CClientControlItemSinkDlg::SetUserNickName(LPCTSTR szNickName)
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->AddString(szNickName);
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->SetCurSel(0);
}
void CClientControlItemSinkDlg::ResetUserNickName()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_GAMEID))->ResetContent();
}

//������
bool __cdecl CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{
	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	CString strTemp=TEXT("");
	switch(pResult->cbAckType)
	{
	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  				
				strTemp=TEXT("�������ݳɹ���");
				SetWindowText(strTemp);
				Sleep(200);
				if(m_cbControlStyle==CS_BANKER_WIN || m_cbControlStyle==CS_BANKER_LOSE)
				{
					if(m_cbControlStyle==CS_BANKER_WIN) str.Format(TEXT("ׯ��Ӯ��ִ�д���%d��"),m_cbExcuteTimes);
					else str.Format(TEXT("ׯ���䣬ִ�д���%d��"),m_cbExcuteTimes);
				}
				else if(m_cbControlStyle==CS_BET_AREA)
				{
					strTemp = TEXT("ʤ����");
					CString strShun=TEXT(""),strDui=TEXT(""),strDao=TEXT("");
					if(m_bWinArea[0]) strShun=TEXT(" ˳�� ");
					if(m_bWinArea[1]) strDui=TEXT(" ���� ");
					if(m_bWinArea[2]) strDao=TEXT(" ���� ");
					if(!m_bWinArea[0]&&!m_bWinArea[1]&&!m_bWinArea[2]) strShun=TEXT(" ׯͨ�� ");

					str.Format(TEXT("��ִ�д���%d��"),m_cbExcuteTimes);

					str=strTemp+strShun+strDui+strDao+str;
				}
				else str=TEXT("��ʱ�޿���");
			}
			else
			{
				str.Format(TEXT("����ʧ��!"));
				m_cbExcuteTimes=0;
				m_cbControlStyle=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				m_cbControlStyle=0;
				m_cbExcuteTimes=0;
				ZeroMemory(m_bWinArea,sizeof(m_bWinArea));
				ReSetAdminWnd();
				str=TEXT("���������ѽ���!");
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str=TEXT("������ͬ�������ѽ���!");
				
				tagAdminReq*pAdminReq=(tagAdminReq*)pResult->cbExtendData;
				m_cbExcuteTimes=pAdminReq->m_cbExcuteTimes;
				m_cbControlStyle=pAdminReq->m_cbControlStyle;
				memcpy(m_bWinArea,pAdminReq->m_bWinArea,sizeof(m_bWinArea));

				strTemp=TEXT("�������ݳɹ���");
				SetWindowText(strTemp);
				Sleep(200);

				if(m_cbControlStyle==CS_BANKER_WIN || m_cbControlStyle==CS_BANKER_LOSE)
				{
					if(m_cbControlStyle==CS_BANKER_WIN) str.Format(TEXT("ׯ��Ӯ��ִ�д���%d��"),m_cbExcuteTimes);
					else str.Format(TEXT("ׯ���䣬ִ�д���%d��"),m_cbExcuteTimes);
				}
				else if(m_cbControlStyle==CS_BET_AREA)
				{
					strTemp = TEXT("ʤ����");
					CString strShun=TEXT(""),strDui=TEXT(""),strDao=TEXT("");
					if(m_bWinArea[0]) strShun=TEXT(" ˳�� ");
					if(m_bWinArea[1]) strDui=TEXT(" ���� ");
					if(m_bWinArea[2]) strDao=TEXT(" ���� ");
					if(!m_bWinArea[0]&&!m_bWinArea[1]&&!m_bWinArea[2]) strShun=TEXT(" ׯͨ�� ");

					str.Format(TEXT("��ִ�д���%d��"),m_cbExcuteTimes);

					str=strTemp+strShun+strDui+strDao+str;
				}
				else str=TEXT("��ʱ�޿���");

				ReSetAdminWnd();
			}
			else
			{
				str.Format(TEXT("����ʧ��!"));
			}
			break;
		}

	default: break;
	}


	SetWindowText(str);
	//str=strTemp+str;
	//SetDlgItemText(IDC_STATIC_INFO,str);
	return true;
}


void CClientControlItemSinkDlg::ResetUserBet()
{
	CString strPrint;
	memset(m_lAllUserBet,0,sizeof(LONGLONG)*AREA_COUNT);
	SetDlgItemText(IDC_ST_AREA1,TEXT("���:-----"));
	SetDlgItemText(IDC_ST_AREA2,TEXT("�Һ�:-----"));
	SetDlgItemText(IDC_ST_AREA3,TEXT("˳��:-----"));
	SetDlgItemText(IDC_ST_AREA4,TEXT("����:-----"));
	SetDlgItemText(IDC_ST_AREA5,TEXT("���:-----"));
	SetDlgItemText(IDC_ST_AREA6,TEXT("�ҽ�:-----"));
	SetDlgItemText(IDC_STATIC_Area111,TEXT("����:-----"));

	SetDlgItemText(IDC_ST_AREA7,TEXT("���:-----"));
	SetDlgItemText(IDC_ST_AREA8,TEXT("�Һ�:-----"));
	SetDlgItemText(IDC_ST_AREA9,TEXT("˳��:-----"));
	SetDlgItemText(IDC_ST_AREA10,TEXT("����:-----"));
	SetDlgItemText(IDC_ST_AREA11,TEXT("���:-----"));
	SetDlgItemText(IDC_ST_AREA12,TEXT("�ҽ�:-----"));
	SetDlgItemText(IDC_STATIC_AREE2222,TEXT("����:-----"));

}

void CClientControlItemSinkDlg::SetUserBetScore(BYTE cbArea,LONGLONG lScore)
{
	m_lAllUserBet[cbArea]+=lScore;
	CString strPrint;
	switch(cbArea)
	{
	case 0: 
		{
			strPrint.Format(TEXT("˳��:%I64d"),m_lAllUserBet[cbArea]);//
			SetDlgItemText(IDC_ST_AREA3,strPrint);
			break;
		}
	case 1: 
		{
			strPrint.Format(TEXT("�ҽ�:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA6,strPrint);
			break;
		}
	case 2: 
		{
			strPrint.Format(TEXT("�Һ�:%I64d"),m_lAllUserBet[cbArea]);//
			SetDlgItemText(IDC_ST_AREA2,strPrint);
			break;
		}
	case 3:
		{
			strPrint.Format(TEXT("����:%I64d"),m_lAllUserBet[cbArea]);//
			SetDlgItemText(IDC_STATIC_Area111,strPrint);
			break;
		}
		
	case 4: 
		{
			strPrint.Format(TEXT("����:%I64d"),m_lAllUserBet[cbArea]);//
			SetDlgItemText(IDC_ST_AREA4,strPrint);
			break;
		}
	case 5: 
		{
			strPrint.Format(TEXT("���:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA5,strPrint);
			break;
		}	
	case 6: 
		{
			strPrint.Format(TEXT("���:%I64d"),m_lAllUserBet[cbArea]);
			SetDlgItemText(IDC_ST_AREA1,strPrint);
			break;
		}	


	default:false;
	}
}

void CClientControlItemSinkDlg::SetUserGameScore(BYTE cbArea,LONGLONG lScore)
{
	CString strPrint;
	switch(cbArea)
	{
	case 0: 
		{
			strPrint.Format(TEXT("˳��:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA9,strPrint);
			break;
		}	
	case 1: 
		{
			strPrint.Format(TEXT("�ҽ�:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA12,strPrint);
			break;
		}
	case 2: 
		{
			strPrint.Format(TEXT("�Һ�:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA8,strPrint);
			break;
		}
	case 3: 
		{
			strPrint.Format(TEXT("����:%I64d"),lScore);
			SetDlgItemText(IDC_STATIC_AREE2222,strPrint);
			break;
		}
	case 4:
		{
			strPrint.Format(TEXT("����:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA10,strPrint);
			break;
		}

	case 5: 
		{
			strPrint.Format(TEXT("���:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA11,strPrint);
			break;
		}
	case 6: 
		{
			strPrint.Format(TEXT("���:%I64d"),lScore);
			SetDlgItemText(IDC_ST_AREA7,strPrint);
			break;
		}	
	


	default:false;
	}
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BT_RESET,OnReSet)
	ON_BN_CLICKED(IDC_BT_CANCEL,OnCancel)
	ON_BN_CLICKED(IDC_BT_EXCUTE,OnExcute)
	ON_BN_CLICKED(IDC_BT_CURSET,OnRefresh)
	ON_BN_CLICKED(IDC_RADIO_CT_AREA,OnRadioClick)
	ON_BN_CLICKED(IDC_RADIO_CT_BANKER,OnRadioClick)
	ON_BN_CLICKED(IDC_BT_GET, OnBnClickedBtGet)
	ON_BN_CLICKED(IDC_BT_CHEAK, OnBnClickedBtCheak)
	ON_BN_CLICKED(IDC_BUTTON_RESET_STORAGE,OnBnClickedResetStorage)
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_ST_AREA8, OnStnClickedStArea8)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg ��Ϣ�������

void CClientControlItemSinkDlg::OnRadioClick()
{
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck()==1)
	{
		GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_WIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_DAOMEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SHUNMEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_DUIMEN)->EnableWindow(FALSE);
	}
	else
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			GetDlgItem(IDC_RADIO_LOSE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_WIN)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_DAOMEN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_SHUNMEN)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_DUIMEN)->EnableWindow(TRUE);
		}
	}
}

void CClientControlItemSinkDlg::OnBnClickedBtGet()
{
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_GET_ACCOUNT,0,0);
}

void CClientControlItemSinkDlg::OnBnClickedBtCheak()
{
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_CHEAK_ACCOUNT,0,0);
}

void CClientControlItemSinkDlg::OnReSet()
{

	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_RESET_CONTROL;
	adminReq.nServerValue=3377758818;
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnRefresh()
{
	
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_PRINT_SYN;
	adminReq.nServerValue=3377758818;
	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnExcute()
{
	BYTE cbIndex=0;
	bool bFlags=false;
	m_cbControlStyle=0;
	m_cbExcuteTimes=0;
	ZeroMemory(m_bWinArea,sizeof(m_bWinArea));

	//����ׯ��
	if(((CButton*)GetDlgItem(IDC_RADIO_CT_BANKER))->GetCheck())
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_WIN))->GetCheck())
		{
			m_cbControlStyle=CS_BANKER_WIN;
			bFlags=true;
		}
		else
		{
			if(((CButton*)GetDlgItem(IDC_RADIO_LOSE))->GetCheck())
			{
				m_cbControlStyle=CS_BANKER_LOSE;
				bFlags=true;
			}
		}
	}
	else //��������
	{
		if(((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->GetCheck())
		{
			m_cbControlStyle=CS_BET_AREA;
			for(cbIndex=0;cbIndex<CONTROL_AREA;cbIndex++)
				m_bWinArea[cbIndex]=((CButton*)GetDlgItem(IDC_CHECK_SHUNMEN+cbIndex))->GetCheck()?true:false;
			bFlags=true;
		}
	}

	m_cbExcuteTimes=(BYTE)((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel()+1;
	//��ȡִ�д���
	if(m_cbExcuteTimes<=0)
	{
		bFlags=false;
	}
	

	if(bFlags) //������Ч
	{
		CMD_C_AdminReq adminReq;
		adminReq.cbReqType=RQ_SET_WIN_AREA;
		adminReq.nServerValue=33377758818;
		tagAdminReq*pAdminReq=(tagAdminReq*)adminReq.cbExtendData;
		pAdminReq->m_cbExcuteTimes=m_cbExcuteTimes;
		pAdminReq->m_cbControlStyle=m_cbControlStyle;
		memcpy(pAdminReq->m_bWinArea,m_bWinArea,sizeof(m_bWinArea));
		CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
		if (pGameFrameEngine!=NULL) pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	}
	else
	{
		//AfxMessageBox(TEXT("��ѡ���ܿش����Լ��ܿط�ʽ!"));
		SetDlgItemText(IDC_STATIC_INFO,TEXT("��ѡ���ܿش����Լ��ܿط�ʽ!"));
		//OnRefresh();
	}
}

void CClientControlItemSinkDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialog::OnCancel();
}

//��ʼ��
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetDlgItemText(IDC_BT_RESET,TEXT("ȡ������"));	
	SetDlgItemText(IDC_BT_CURSET,TEXT("����"));
	SetDlgItemText(IDC_BT_EXCUTE,TEXT("ִ��"));	
	SetDlgItemText(IDC_BT_CANCEL,TEXT("ȡ��"));	
	SetDlgItemText(IDC_RADIO_WIN,TEXT("ׯ��Ӯ"));	
	SetDlgItemText(IDC_RADIO_LOSE,TEXT("ׯ����"));			 
	SetDlgItemText(IDC_RADIO_CT_BANKER,TEXT("ׯ�ҿ���"));	
	SetDlgItemText(IDC_RADIO_CT_AREA,TEXT("�������"));	
	SetDlgItemText(IDC_CHECK_SHUNMEN,TEXT("˳��"));	
	SetDlgItemText(IDC_CHECK_DUIMEN	,TEXT("����"));	
	SetDlgItemText(IDC_CHECK_DAOMEN	,TEXT("����"));		
	SetDlgItemText(IDC_STATIC_TIMES	,TEXT("���ƴ���"));	
	SetDlgItemText(IDC_STATIC_CHOICE,TEXT("����ѡ��"));	
	SetDlgItemText(IDC_STATIC_NOTICE,TEXT("˵��"));
	SetDlgItemText(IDC_STATIC_NOTICE1,TEXT("1.������Ӯ���Ʊ���Ϸ�����Ʋ������ȡ�"));	
	SetDlgItemText(IDC_STATIC_NOTICE2,TEXT("2.������ƣ�ѡ��Ϊʤ��δѡ��Ϊ�䡣"));	

	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));

	((CButton*)GetDlgItem(IDC_RADIO_CT_AREA))->SetCheck(1);
	OnRadioClick();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void __cdecl CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
	GetDlgItem(IDC_BT_EXCUTE)->EnableWindow(bEnable);
}

//���¿��
void __cdecl CClientControlItemSinkDlg::UpDateSystemStorage(CMD_S_SystemStorage * pStorage)
{
	CString str;
	str.Format(TEXT("��ǰϵͳ���:%I64d   ˥��ֵ:%I64d"),pStorage->lStorage,pStorage->lStorageDeduct);
	SetDlgItemText(IDC_STATIC_System_STORAGE,str);
}




HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID()==IDC_STATIC_INFO) 
	{  
		pDC->SetTextColor(RGB(255,10,10)); 
	} 
	if (pWnd->GetDlgCtrlID()==IDC_STATIC_System_STORAGE)
	{
        pDC->SetTextColor(RGB(255,10,10)); 
	}
	return hbr;
}

void CClientControlItemSinkDlg::OnBnClickedResetStorage()
{
	CString strStorage=TEXT(""),strDeduct=TEXT("");
	GetDlgItemText(IDC_EDIT1,strStorage);
	//ȥ���ո�
	strStorage.TrimLeft();
	strStorage.TrimRight();

	GetDlgItemText(IDC_EDIT2,strDeduct);
	//ȥ���ո�
	strDeduct.TrimLeft();
	strDeduct.TrimRight();
	LONGLONG lStorage=0L,lDeduct=0L;
	lStorage = StrToInt(strStorage);
	lDeduct = StrToInt(strDeduct);

	if(lDeduct < 0L || lDeduct > 99L)
	{
		SetDlgItemText(IDC_STATIC_INFO,TEXT("����ʧ�ܣ�˥��ֵ����0��99"));
		return;
	}

	CMD_C_UpdateStorage adminReq;
	ZeroMemory(&adminReq,sizeof(adminReq));
	adminReq.lStorage=lStorage;
	adminReq.lStorageDeduct=lDeduct;

	//g_pGameClientDlg->OnAdminRestStorage((WPARAM)&adminReq,0);

	CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
	pGameFrameEngine->SendSocketData(SUB_C_ADMIN_RESET_STORAGE,&adminReq,sizeof(adminReq));;
	//if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_RESET_STORAGE,(WPARAM)&adminReq,0);
}

void CClientControlItemSinkDlg::OnStnClickedStArea8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
