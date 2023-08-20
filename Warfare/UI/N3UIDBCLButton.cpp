#include "pch.h"
#include "N3UIDBCLButton.h"
#include "GameProcedure.h"
#include "UIImageTooltipDlg.h"

CN3UIDBCLButton::CN3UIDBCLButton()
{
	CN3UIImage::CN3UIImage();
}

CN3UIDBCLButton::~CN3UIDBCLButton()
{
}

DWORD CN3UIDBCLButton::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;

	const RECT rect = GetRegion();
	if(!::PtInRect(&rect, ptCur))		// out of range
	{
		dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
		return dwRet;
	}

	if (dwFlags & UI_MOUSE_LBDBLCLK)
	{
		m_pParent->ReceiveMessage(this, UIMSG_ICON_DBLCLK); // Notify parent on button click..
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}