#include "pch.h"
#include "N3UIEdit.h"
#include "N3UIString.h"
#include "N3UIImage.h"
#include "DFont.h"
#include "N3UIStatic.h"

#include "N3SndMgr.h"
#include "N3SndObj.h"
#include <imm.h>

const float CARET_FLICKERING_TIME = 0.4f;

// HWND CN3UIEdit::s_hWndParent = NULL;
HWND CN3UIEdit::s_hWndEdit = nullptr;
HWND CN3UIEdit::s_hWndParent = nullptr;
WNDPROC	CN3UIEdit::s_lpfnEditProc = nullptr;
char CN3UIEdit::s_szBuffTmp[512] = "";

CN3UIEdit::CN3Caret::CN3Caret()
{
	m_pVB[0].Set(0,0,UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xffff0000);
	m_pVB[1].Set(0,10,UI_DEFAULT_Z, UI_DEFAULT_RHW, 0xffff0000);
	m_bVisible = FALSE;
	m_fFlickerTimePrev = CN3Base::TimeGet();	// Time to blink...
	m_bFlickerStatus = true;

}
CN3UIEdit::CN3Caret::~CN3Caret()
{
}
void CN3UIEdit::CN3Caret::SetPos(int x, int y)
{
	m_pVB[0].x = x; m_pVB[0].y = y;
	m_pVB[1].x = x; m_pVB[1].y = y+m_iSize;
	this->InitFlckering();
}
void CN3UIEdit::CN3Caret::MoveOffset(int iOffsetX, int iOffsetY)
{
	m_pVB[0].x += iOffsetX; m_pVB[0].y += iOffsetY; 
	m_pVB[1].x = m_pVB[0].x; m_pVB[1].y = m_pVB[0].y+m_iSize;
	this->InitFlckering();
}
void CN3UIEdit::CN3Caret::SetSize(int iSize)
{
	m_iSize = iSize;
	m_pVB[1].y = m_pVB[0].y+m_iSize;
}
void CN3UIEdit::CN3Caret::SetColor(D3DCOLOR color)
{
	m_pVB[0].color = color;
	m_pVB[1].color = color;
}
void CN3UIEdit::CN3Caret::Render(LPDIRECT3DDEVICE9	lpD3DDev)
{
	if (FALSE == m_bVisible) return;

	// flicker handling...
	const float fTime = CN3Base::TimeGet();
	if(fTime - m_fFlickerTimePrev > CARET_FLICKERING_TIME)
	{
		m_bFlickerStatus = !m_bFlickerStatus;
		m_fFlickerTimePrev = fTime;
	}
	if(!m_bFlickerStatus) return;

	__ASSERT(lpD3DDev, "DIRECT3DDEVICE8 is null");
	lpD3DDev->SetTexture(0, nullptr);
	// lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,    D3DTOP_SELECTARG1 );
	// lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,  D3DTA_DIFFUSE );
	lpD3DDev->SetFVF(FVF_TRANSFORMEDCOLOR);
	lpD3DDev->DrawPrimitiveUP(D3DPT_LINELIST, 1, m_pVB, sizeof(m_pVB[0]));
}
void CN3UIEdit::CN3Caret::InitFlckering()
{
	m_fFlickerTimePrev = CN3Base::TimeGet();	// Time to blink...
	m_bFlickerStatus = true;
}

// 
// CN3UIEdit
// 

BOOL CN3UIEdit::CreateEditWindow(HWND hParent, RECT rect)
{
	if(nullptr == hParent) return FALSE;
	if(s_hWndEdit) return FALSE;

	s_hWndParent = hParent;
	s_hWndEdit = CreateWindow("EDIT", "EditWindow", WS_CHILD|WS_TABSTOP|ES_LEFT|ES_WANTRETURN, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, hParent, NULL, NULL, NULL);
	s_lpfnEditProc = (WNDPROC)SetWindowLong(s_hWndEdit, GWLP_WNDPROC, (DWORD)(CN3UIEdit::EditWndProc));

	// Set the edit control's text size to the maximum.
	::SendMessage(s_hWndEdit, EM_LIMITTEXT, 0, 0);

	// Set the edit control's font
	auto hFont = (HFONT)GetStockObject(ANSI_FIXED_FONT);
	::SendMessage(s_hWndEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
	::SendMessage(s_hWndEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

	// Erase the background...??
	const HDC hDC = GetDC(s_hWndEdit);
	SetBkMode(hDC, TRANSPARENT);
	SetROP2(hDC, R2_XORPEN);
	ReleaseDC(s_hWndEdit, hDC);

	return TRUE;
}

LRESULT APIENTRY CN3UIEdit::EditWndProc(HWND hWnd, WORD Message, WPARAM wParam, LPARAM lParam)
{
   //
   // When the focus is in an edit control inside a dialog box, the
   // default ENTER key action will not occur.
   //
    switch (Message)
	{
	case WM_KEYDOWN:
		if (wParam == VK_RETURN)
		{
			if(s_pFocusedEdit && s_pFocusedEdit->GetParent())
			{
				s_pFocusedEdit->GetParent()->ReceiveMessage(s_pFocusedEdit, UIMSG_EDIT_RETURN);
			}
			return 0;
		}
		(CallWindowProc(s_lpfnEditProc, hWnd, Message, wParam, lParam));
		if(s_pFocusedEdit) CN3UIEdit::UpdateCaretPosFromEditCtrl();
		return 0;
		// break;

    case WM_CHAR:
		if(s_pFocusedEdit) CN3UIEdit::UpdateCaretPosFromEditCtrl();
		if(wParam==VK_RETURN) return 0;
		if(wParam==VK_TAB) return 0;
		break;
	case WM_INPUTLANGCHANGE:
		{
			POINT ptPos;
			ptPos.x = 0;
			ptPos.y = 0;
			SetImeStatus(ptPos,true);
		}
		break;
	case WM_IME_ENDCOMPOSITION:
		{
			POINT ptPos;
			ptPos.x = -1000;
			ptPos.y = -1000;
			SetImeStatus(ptPos,false);
		}
		break;
	case WM_IME_STARTCOMPOSITION:
		{
			POINT ptPos;
			ptPos.x = 0;
			ptPos.y = 0;
			SetImeStatus(ptPos,true);
		}
		break;
    } // switch

	return (CallWindowProc(s_lpfnEditProc, hWnd, Message, wParam, lParam));
}

CN3UIEdit::CN3Caret CN3UIEdit::s_Caret;

CN3UIEdit::CN3UIEdit()
{
	m_eType = UI_TYPE_EDIT;
	m_nCaretPos = 0;
	m_iCompLength = 0;
	m_iMaxStrLen = 0x7fffffff;
	KillFocus();
	m_pSnd_Typing = nullptr;
}

CN3UIEdit::~CN3UIEdit()
{
	KillFocus();
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Typing);
}

void CN3UIEdit::Release()
{
	CN3UIBase::Release();
	m_nCaretPos = 0;
	m_iCompLength = 0;
	m_iMaxStrLen = 0x7fffffff;
	KillFocus();
	m_szPassword = "";
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Typing);
}

void CN3UIEdit::Render()
{
	if (!m_bVisible) return;

	CN3UIStatic::Render();
	if (HaveFocus())
	{
		s_Caret.Render(s_lpD3DDev);	// Draw caret when focused
	}
}

void CN3UIEdit::SetVisible(bool bVisible)
{
	CN3UIBase::SetVisible(bVisible);

	if (false == bVisible && true == m_bVisible)	// when invisible
	{
		KillFocus();
	}
}

void CN3UIEdit::KillFocus() const
{
	if (HaveFocus())
	{
		s_pFocusedEdit = nullptr;
		s_Caret.m_bVisible = FALSE;

		if(s_hWndEdit)
		{
			::SetWindowText(s_hWndEdit, "");
			::SetFocus(s_hWndParent);
		}
	}
}

bool CN3UIEdit::SetFocus()
{
	// if (HaveFocus()) return true; // If I already have focus return true;
	if (nullptr != s_pFocusedEdit) s_pFocusedEdit->KillFocus();	// Call killfocus if another edit has it
	s_pFocusedEdit = this;				// Set the edit that has focus to me

	SIZE size;
	if (m_pBuffOutRef && m_pBuffOutRef->GetTextExtent(" ",2,&size))
	{
		s_Caret.SetSize(size.cy);
		s_Caret.SetColor(m_pBuffOutRef->GetFontColor());
	}

	s_Caret.m_bVisible = TRUE;
	s_Caret.InitFlckering();
	CN3UIEdit::UpdateCaretPosFromEditCtrl(); // Set caret position

	if(s_hWndEdit)
	{
		::SetFocus(s_hWndEdit);

		const RECT rcEdit = GetRegion();
		const int iX		= rcEdit.left;
		const int iY		= rcEdit.top;
		const int iH		= rcEdit.bottom - rcEdit.top;
		const int iW		= rcEdit.right - rcEdit.left;
		::MoveWindow(s_hWndEdit, iX, iY, iW, iH, false);
		
		if(UISTYLE_EDIT_PASSWORD & m_dwStyle)
		{
			::SetWindowText(s_hWndEdit, m_szPassword.c_str());
		}
		else
		{
			if(m_pBuffOutRef) ::SetWindowText(s_hWndEdit, m_pBuffOutRef->GetString().c_str());
			else ::SetWindowText(s_hWndEdit, "");
		}
	}

	return true;
}

DWORD CN3UIEdit::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;
	if(dwFlags & UI_MOUSE_LBCLICK &&IsIn(ptCur.x, ptCur.y))	// If the left button is pressed inside the area
	{
		SetFocus();	// give me focus
		dwRet |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_INREGION);
		return dwRet;
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CN3UIEdit::SetCaretPos(UINT nPos)
{
	if (nPos > m_iMaxStrLen) nPos = m_iMaxStrLen;	// If it is longer than the maximum length, set it to a smaller size
	m_nCaretPos = nPos;

	const std::string& szBuff = m_pBuffOutRef->GetString();
	__ASSERT(szBuff.empty() || -1 == szBuff.find('\n'), "multiline edit");	// Currently multiline is not supported.
	SIZE size = {0,0};
	if (!szBuff.empty() && m_pBuffOutRef ) m_pBuffOutRef->GetTextExtent(szBuff, m_nCaretPos, &size) ;

	const int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
	if (size.cx > iRegionWidth) size.cx = iRegionWidth;
	s_Caret.SetPos(m_pBuffOutRef->m_ptDrawPos.x + size.cx, m_pBuffOutRef->m_ptDrawPos.y);
}

void CN3UIEdit::SetMaxString(int iMax)		// set the maximum number of characters
{
	if (iMax <= 0) {__ASSERT(0, "Set the maximum number of characters to greater than 0"); return;}
	m_iMaxStrLen = iMax;

	if (nullptr == m_pBuffOutRef) return;

	const std::string szBuff = GetString();
	if ( m_iMaxStrLen >= szBuff.size()) return;

	// If you come this far, the current text length is larger than iMax, so set it again so that it is cut according to the limit characters.
	SetString(szBuff);
}

// 
//
// This is the part that checks whether a specific position is the second byte among 2 bytes of Korean characters.
// There is a function called IsDBCSLeadByte, but in case of combination type
// Since the start byte and end byte ranges are the same, it cannot be checked with this function.
// Therefore, there is no other way than to do the inspection from the beginning.
//
// I haven't tested how it works in Unicode on NT, but
// I think you can use it without any problems.
//
// 
BOOL CN3UIEdit::IsHangulMiddleByte( const char* lpszStr, int iPos )
{
    if( !lpszStr ) return FALSE;
    if (iPos <= 0) return FALSE;
    const int nLength = lstrlen(lpszStr);
    if (iPos >= nLength) return FALSE;
    if (!(lpszStr[iPos] & 0x80))
        return FALSE;
    register BOOL bMiddle = FALSE;
    register int i;

    for (i=0; i<iPos && i <nLength; i++)
    {
        if (lpszStr[i] & 0x80)
            bMiddle = !bMiddle;
    }
    return bMiddle;
}

const std::string& CN3UIEdit::GetString()
{
	if (UISTYLE_EDIT_PASSWORD & m_dwStyle) return m_szPassword;
	return CN3UIStatic::GetString();
}

void CN3UIEdit::SetString(const std::string& szString)
{
	if (nullptr == m_pBuffOutRef) return;
	if (szString.size() > m_iMaxStrLen)
	{
		std::string szNewBuff(m_iMaxStrLen, '?');

		if (IsHangulMiddleByte(szString.c_str(), m_iMaxStrLen))
		{
			szNewBuff = szString.substr(0, m_iMaxStrLen-1);	// -1 is Korean, so to copy one less +1 to insert NULL at the end
			if (UISTYLE_EDIT_PASSWORD & m_dwStyle)
			{
				int iNewBuffLen = szNewBuff.size();
				m_szPassword = szNewBuff;

				szNewBuff.assign(m_iMaxStrLen-1, '*');
				__ASSERT(NULL == szNewBuff[m_iMaxStrLen-1],"different number of characters.");
			}
			m_pBuffOutRef->SetString(szNewBuff);
		}
		else
		{
			szNewBuff = szString.substr(0, m_iMaxStrLen);	// +1 for putting NULL at the end
			if (UISTYLE_EDIT_PASSWORD & m_dwStyle)
			{
				int iNewBuffLen = szNewBuff.size();
				m_szPassword = szNewBuff;

				szNewBuff.assign(m_iMaxStrLen, '*');
				__ASSERT(NULL == szNewBuff[m_iMaxStrLen],"different number of characters.");
			}
			m_pBuffOutRef->SetString(szNewBuff);
		}
	}
	else
	{
		if (UISTYLE_EDIT_PASSWORD & m_dwStyle)
		{
			m_szPassword = szString;
			if (!szString.empty())
			{
				const std::string szNewBuff(szString.size(), '*');
				m_pBuffOutRef->SetString(szNewBuff);
			}
			else m_pBuffOutRef->SetString("");
		}
		else
		{
			m_pBuffOutRef->SetString(szString);
		}
	}

	const std::string& szTempStr = m_pBuffOutRef->GetString();
	const int iStrLen = szTempStr.size();
	if (m_nCaretPos > iStrLen) SetCaretPos(iStrLen);
}

BOOL CN3UIEdit::MoveOffset(int iOffsetX, int iOffsetY)		// Designation of position (the position of chilren is also changed. The position of caret is also changed.)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	/*
		RECT rcEdit = GetRegion();
		int iX		= rcEdit.left;
		int iY		= rcEdit.top;
		int iH		= rcEdit.bottom - rcEdit.top;
		int iW		= rcEdit.right - rcEdit.left;

	::MoveWindow(s_hWndEdit, iX, iY, iW, iH, false);
	*/
	if (HaveFocus()) s_Caret.MoveOffset(iOffsetX, iOffsetY);
	return TRUE;
}

bool CN3UIEdit::Load(HANDLE hFile)
{
	if (false == CN3UIStatic::Load(hFile)) return false;

	// To convert the old uif file, block the sound loading part
	int iSndFNLen = 0;
	DWORD dwNum;

	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, nullptr);		// sound file string length
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, buffer.data(), iSndFNLen, &dwNum, nullptr);

		__ASSERT(NULL == m_pSnd_Typing, "memory leak");
		m_pSnd_Typing = s_SndMgr.CreateObj(buffer.data(), SNDTYPE_2D);
	}

	return true;
}

#ifdef _N3TOOL
void CN3UIEdit::operator = (const CN3UIEdit& other)
{
	CN3UIStatic::operator = (other);
	SetSndTyping(other.GetSndFName_Typing());
}

bool CN3UIEdit::Save(HANDLE hFile)
{
	if (false == CN3UIStatic::Save(hFile)) return false;

	DWORD dwNum;

	int iSndFNLen = 0;
	if (m_pSnd_Typing) iSndFNLen = m_pSnd_Typing->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		// sound file string length
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_Typing->m_szFileName.c_str(), iSndFNLen, &dwNum, NULL);

	return true;
}

void CN3UIEdit::SetSndTyping(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Typing);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Passes a relative path to the base path.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_Typing = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

std::string CN3UIEdit::GetSndFName_Typing() const
{
	if (m_pSnd_Typing) return m_pSnd_Typing->m_szFileName;
	else return std::string("");
}
#endif

void CN3UIEdit::UpdateTextFromEditCtrl()
{
	if(nullptr == s_pFocusedEdit || nullptr == s_hWndEdit) return;

	::GetWindowText(s_hWndEdit, s_szBuffTmp, 512);
	s_pFocusedEdit->SetString(s_szBuffTmp);
}

void CN3UIEdit::UpdateCaretPosFromEditCtrl()
{
	if(nullptr == s_pFocusedEdit || nullptr == s_hWndEdit) return;

	/* int iCare = 0;
		int iLen = GetWindowTextLength(s_hWndEdit);
		POINT ptCaret;
		GetCaretPos(&ptCaret);
		if(ptCaret.x > 0)
		{
			HDC hDC = GetDC(s_hWndEdit);
			SIZE size;
			GetTextExtentPoint32(hDC, "1", 1, &size);
			iCaret = ptCaret.x / size.cx;
			ReleaseDC(s_hWndEdit, hDC);
		}
	*/
	const int iTmp = ::SendMessage(s_hWndEdit, EM_GETSEL, 0, 0);
	const int iCaret = LOWORD(iTmp);
	int iCTmp2 = HIWORD(iTmp);
	s_pFocusedEdit->SetCaretPos(iCaret);
}

void CN3UIEdit::SetImeStatus(POINT ptPos, bool bOpen)
{
#ifndef _N3TOOL
	const HKL hHKL = GetKeyboardLayout(0);
	if(ImmIsIME(hHKL))
	{
		const HIMC hImc = ImmGetContext(s_hWndEdit);
		if(bOpen)
		{
			SendMessage(s_hWndEdit, WM_IME_NOTIFY, IMN_OPENSTATUSWINDOW, 0);
			ImmSetStatusWindowPos(hImc,&ptPos);
		}
		else
		{
			SendMessage(s_hWndEdit, WM_IME_NOTIFY, IMN_CLOSESTATUSWINDOW, 0);
		}
		ImmReleaseContext( s_hWndEdit, hImc );
	}
#endif
}
