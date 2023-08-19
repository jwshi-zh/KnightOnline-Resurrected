#include "pch.h"
#include "UINationSelectDlg.h"
#include "GameProcNationSelect.h"

CUINationSelectDlg::CUINationSelectDlg()
{
	m_pProcNationSelectRef = nullptr;
}

CUINationSelectDlg::~CUINationSelectDlg()
{
	m_pBtnKarus = nullptr;
	m_pBtnElmorad = nullptr;
}

bool CUINationSelectDlg::Load(HANDLE hFile)
{
	bool bSuccess = CN3UIBase::Load(hFile);

	m_pBtnKarus = this->GetChildByID("bt_karus");	__ASSERT(m_pBtnKarus, "NULL UI Component!!");
	m_pBtnElmorad = this->GetChildByID("bt_elmo");	__ASSERT(m_pBtnElmorad, "NULL UI Component!!");
	
	RECT rc = this->GetRegion();
	int iX = ((int)s_CameraData.vp.Width - (rc.right - rc.left))/2;
	int iY = ((int)s_CameraData.vp.Height - (rc.bottom - rc.top))/2;
	this->SetPos(iX, iY);

	return bSuccess;
}

bool CUINationSelectDlg::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if(nullptr == pSender) return false;

	if( dwMsg == UIMSG_BUTTON_CLICK )
	{
		if ( pSender == m_pBtnKarus )	// Karus
		{
			if(m_pProcNationSelectRef) m_pProcNationSelectRef->MsgSendNationSelect(NATION_KARUS);
		}
		else
		if ( pSender == m_pBtnElmorad )	// Elmorad
		{
			if(m_pProcNationSelectRef) m_pProcNationSelectRef->MsgSendNationSelect(NATION_ELMORAD);
		}
	}

	return true;
}

