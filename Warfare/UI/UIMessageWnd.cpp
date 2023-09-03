#include "pch.h"
#include "GameProcMain.h"
#include "PacketDef.h"
#include "N3UIString.h"
#include "N3UIScrollBar.h"
#include "UIMessageWnd.h"
#include "UIManager.h"

CUIMessageWnd::CUIMessageWnd()
{
	m_pChatOut = nullptr;
	m_pScrollbar = nullptr;
	m_iChatLineCount = 0;
	m_ppUILines = nullptr;
	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));
}

CUIMessageWnd::~CUIMessageWnd()
{
	if (m_ppUILines) {delete [] m_ppUILines; m_ppUILines = nullptr;}	// The pointer of m_ppUILines[n] should not be deleted because it is automatically released when the parent is freed even if the memory is allocated.

	for(auto itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
	{
		const __ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_ChatBuffer.clear();

	for(auto itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
	{
		const __ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_LineBuffer.clear();
}

void CUIMessageWnd::Release()
{
	CN3UIBase::Release();

	m_pChatOut = nullptr;
	m_pScrollbar = nullptr;
	m_iChatLineCount = 0;
	if (m_ppUILines) {delete [] m_ppUILines; m_ppUILines = nullptr;}	// The pointer of m_ppUILines[n] should not be deleted because it is automatically released when the parent is freed even if the memory is allocated.
	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));

	for(auto itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
	{
		const __ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_ChatBuffer.clear();

	for(auto itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
	{
		const __ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_LineBuffer.clear();
}

BOOL CUIMessageWnd::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (0 == iOffsetX && 0 == iOffsetY) return FALSE;
	// ui area
	m_rcRegion.left += iOffsetX;		m_rcRegion.top += iOffsetY;
	m_rcRegion.right += iOffsetX;		m_rcRegion.bottom += iOffsetY;

	// movable area
	m_rcMovable.left += iOffsetX;		m_rcMovable.top += iOffsetY;
	m_rcMovable.right += iOffsetX;		m_rcMovable.bottom += iOffsetY;

	// update children coordinates
	// Child UI...
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pCUI = (*itor);
		__ASSERT(pCUI, "child UI pointer is NULL!");
		pCUI->MoveOffset(iOffsetX, iOffsetY);
	}

	// Also, move the chat window here...
	const RECT rt = CGameProcedure::s_pProcMain->m_pUIChatDlg->GetRegion();
	const POINT pt = this->GetPos();
	if( (pt.x != rt.right) || ( pt.y != rt.top) )
	{
		CGameProcedure::s_pProcMain->m_pUIChatDlg->SetPos(pt.x - (rt.right-rt.left), pt.y);
	}
	
	return TRUE;
}

bool CUIMessageWnd::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	m_pChatOut = (CN3UIString*)GetChildByID("text_message");	__ASSERT(m_pChatOut, "NULL UI Component!!");
	m_pScrollbar = (CN3UIScrollBar*)GetChildByID("scroll");		__ASSERT(m_pScrollbar, "NULL UI Component!!");

	m_rcChatOutRegion = m_pChatOut->GetRegion();
	CreateLines();

	__ASSERT(0<m_iChatLineCount,"Chat window is too small");
	
	return true;
}


// 
// 

bool CUIMessageWnd::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if(nullptr == pSender) return false;

	if (dwMsg == UIMSG_SCROLLBAR_POS)
	{
		// Set chat line for scroll bar
		const int iCurLinePos = m_pScrollbar->GetCurrentPos();
		SetTopLine(iCurLinePos);
	}
	return true;
}

void CUIMessageWnd::CreateLines()
{
	int i;
	if (m_ppUILines) {
		for (i=0; i<m_iChatLineCount; ++i)
		{
			if (m_ppUILines[i]) {delete m_ppUILines[i]; m_ppUILines[i] = nullptr;}
		}
		delete [] m_ppUILines; m_ppUILines = nullptr;
	}
	SIZE size;
	if (m_pChatOut && m_pChatOut->GetTextExtent(" ", lstrlen(" "), &size) && size.cy>0)
	{
		m_iChatLineCount = (m_rcChatOutRegion.bottom - m_rcChatOutRegion.top)/size.cy;
	}
	else return;
	const std::string& szFontName = m_pChatOut->GetFontName();
	const DWORD dwFontHeight = m_pChatOut->GetFontHeight();
	const DWORD dwFontFlag = m_pChatOut->GetFontFlags();

	if (m_iChatLineCount<=0 || szFontName.size() <= 0) return;

	m_ppUILines = new CN3UIString*[m_iChatLineCount];
	for (i=0; i<m_iChatLineCount; ++i)
	{
		RECT rc;
		SetRect(&rc, m_rcChatOutRegion.left, m_rcChatOutRegion.top+(i*size.cy),
									m_rcChatOutRegion.right, m_rcChatOutRegion.top+((i+1)*size.cy));
		m_ppUILines[i] = new CN3UIString();
		m_ppUILines[i]->Init(this);
		m_ppUILines[i]->SetRegion(rc);
		m_ppUILines[i]->SetStyle(UISTYLE_STRING_SINGLELINE|UISTYLE_STRING_ALIGNLEFT|UISTYLE_STRING_ALIGNTOP);
		m_ppUILines[i]->SetFont(szFontName, dwFontHeight, dwFontFlag & D3DFONT_BOLD, dwFontFlag & D3DFONT_ITALIC);
	}
}

void CUIMessageWnd::AddMsg(const std::string& szString, D3DCOLOR color)
{
	const int iStrLen = szString.size();
	__ASSERT(m_pScrollbar, "");
	if (0 >= iStrLen) return;

	// Put into ChatBuffer
	auto* pChatInfo = new __ChatInfo;
	pChatInfo->szChat = szString;
	pChatInfo->color = color;
	m_ChatBuffer.push_back(pChatInfo);
	
	if (m_ChatBuffer.size() > 255)	// If there are more than 255, erase from the front.
	{
		const __ChatInfo* pTemp = m_ChatBuffer.front();
		if (pTemp) delete pTemp;

		m_ChatBuffer.pop_front();
	}

	// put in line buffer
	AddLineBuffer(szString, color);
	
	// Adjusting the number of line buffers
	int iCurLinePos = m_pScrollbar->GetCurrentPos();	// The line currently pointed by the scroll bar
	const BOOL bAutoScroll = (m_pScrollbar->GetMaxPos() == iCurLinePos) ? TRUE : FALSE;

	while (m_LineBuffer.size() > MAX_CHAT_LINES && 0 < iCurLinePos)	// MAX_CHAT_LINES is the maximum number of lines (however, if the scroll bar is at 0, lines are not deleted, so 500 can be exceeded)
	{
		// delete one line
		const __ChatInfo* pTemp = m_LineBuffer.front();
		if (pTemp) delete pTemp;
		m_LineBuffer.pop_front();
		--iCurLinePos;	
	}

	const int iLineBufferSize = m_LineBuffer.size();
	int iMaxScrollPos = iLineBufferSize-m_iChatLineCount;
	if (iMaxScrollPos<0) iMaxScrollPos = 0;
	m_pScrollbar->SetRange(0, iMaxScrollPos);	// Set scroll bar range

	// If it scrolls automatically
	if ( bAutoScroll) iCurLinePos = iMaxScrollPos;
	if (iCurLinePos < 0) iCurLinePos = 0;

	// Reset scrollbar current position
	m_pScrollbar->SetCurrentPos(iCurLinePos);

	// Set chat line for scroll bar
	SetTopLine(iCurLinePos);
}

void CUIMessageWnd::AddLineBuffer(const std::string& szString, D3DCOLOR color)
{
	if(szString.empty()) return;

	__ASSERT(m_pChatOut, "");
	const int iStrLen = szString.size();

	// Add line buffer
	SIZE size;
	if (FALSE == m_pChatOut->GetTextExtent(szString, iStrLen, &size)) {__ASSERT(0,"no device context"); return;}

	const int iRegionWidth = m_rcChatOutRegion.right - m_rcChatOutRegion.left;

	// Code for cutting characters, characters that go out of range are cut and placed on the line below.
	int iCX=0;
	int iCount = 0;
	int iLineStart = 0;

	while(iCount<iStrLen)
	{
		if ('\n' == szString[iCount])		// \n
		{
			auto* pLineInfo = new __ChatInfo;
			m_LineBuffer.push_back(pLineInfo);

			pLineInfo->color = color;
			if ((iCount - iLineStart)>0)
			{
				const int iLineLength = iCount - iLineStart + 1;
				std::string szLine;
				pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
			}	// In case of continuous \n, pszLine = NULL.

			++iCount;
			iLineStart = iCount;
			iCX = 0;
		}
		else
		{
			int iCC=0;
			if (0x80 & szString[iCount])	iCC = 2;	// 2BYTE characters
			else							iCC = 1;	// 1 BYTE character

			const BOOL bFlag = m_pChatOut->GetTextExtent(&(szString[iCount]), iCC, &size);
			__ASSERT(bFlag, "cannot get size of dfont");
			if ((iCX+size.cx) > iRegionWidth)	// If the width exceeds
			{
				// Add one more line

				const int iLineLength = iCount - iLineStart;
				if (iLineLength>0)
				{
					auto* pLineInfo = new __ChatInfo;
					m_LineBuffer.push_back(pLineInfo);

					pLineInfo->color = color;
					pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
				}
				else
				{
					__ASSERT(iRegionWidth>15, "It's so narrow that I can't even take a picture of Korean characters.");
					break;
				}
				iLineStart = iCount;
				iCX = 0;
			}
			// add letters
			iCount += iCC;
			iCX += size.cx;
		}
	}

	// last exit processing
	const int iLineLength = iStrLen - iLineStart;
	if (iLineLength>0)
	{
		auto* pLineInfo = new __ChatInfo;
		m_LineBuffer.push_back(pLineInfo);

		pLineInfo->color = color;
		pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
	}
}

void CUIMessageWnd::SetTopLine(int iTopLine) const
{
	if (m_iChatLineCount<=0) return;

	const int iLineBufferSize = m_LineBuffer.size();
	if (iTopLine<0) iTopLine = 0;
	else if (iTopLine > iLineBufferSize) iTopLine = iLineBufferSize;
	
	int i;
	// Store in a temporary buffer in order from the previous row and measure the length of the string
	auto** ppLineInfos  = new __ChatInfo*[m_iChatLineCount];
	ZeroMemory(ppLineInfos, sizeof(__ChatInfo*)*m_iChatLineCount);

	for (i=0; i<m_iChatLineCount; ++i)
	{
		const int iCurLine = iTopLine + i;
		if (iLineBufferSize <= iCurLine) break;
		ppLineInfos[i] = m_LineBuffer[iCurLine];
	}

	__ASSERT(m_ppUILines, "null pointer");
	// Put each in the buffer in order, starting from the front.
	const int iRealLine = i;	// Number of lines actually printed
	int iRealLineCount = 0;
	for (i=0; i<iRealLine; ++i)
	{
		++iRealLineCount;
		if (nullptr == m_ppUILines[i]) continue;
		m_ppUILines[i]->SetColor(ppLineInfos[i]->color);
		m_ppUILines[i]->SetString(ppLineInfos[i]->szChat);
	}
	for (i=iRealLineCount; i<m_iChatLineCount; ++i)
	{
		if (nullptr == m_ppUILines[i]) continue;
		m_ppUILines[i]->SetString("");	// leave the rest blank
	}
	delete [] ppLineInfos;
}

void CUIMessageWnd::RecalcLineBuffer()	// If called when the size of the chat window changes, the line buffer is recalculated and inserted.
{
	// Initializing the line buffer
	for(auto itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
	{
		const __ChatInfo* pLineBuff = (*itor);
		if (pLineBuff) delete pLineBuff;
	}
	m_LineBuffer.clear();

	// Reload the line buffer
	for(auto itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
	{
		const __ChatInfo* pChatBuff = (*itor);
		if (pChatBuff) AddLineBuffer(pChatBuff->szChat, pChatBuff->color);
	}

	// Adjusting the number of line buffers
	while (m_LineBuffer.size() > MAX_CHAT_LINES)	// MAX_CHAT_LINES is the maximum number of lines
	{
		// delete one line
		const __ChatInfo* pLineBuff = m_LineBuffer.front();
		if (pLineBuff) delete pLineBuff;
		m_LineBuffer.pop_front();
	}

	const int iLineBufferSize = m_LineBuffer.size();
	int iMaxScrollPos = iLineBufferSize-m_iChatLineCount;
	if (iMaxScrollPos<0) iMaxScrollPos = 0;
	m_pScrollbar->SetRange(0, iMaxScrollPos);	// Set scroll bar range

	// Reset scrollbar current position
	m_pScrollbar->SetCurrentPos(iMaxScrollPos);

	// Set chat line for scroll bar
	SetTopLine(iMaxScrollPos);
}

void CUIMessageWnd::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	// Arrange your children appropriately.
	// m_rcChatOutRegion = ; // Specifies the chat output area again.
	// CreateLines(); // Calculate how many chat lines will fit and recreate them.
	// RecalcLineBuffer(); // Clear the line buffer, recreate it, and display the text.
}


bool CUIMessageWnd::OnKeyPress(int iKey)
{
	switch(iKey)
	{
	case DIK_ESCAPE:
		{	// When the hotkey is focused, other UI cannot be closed, so when DIK_ESCAPE comes in, focus again
			// Close other open children.
			CGameProcedure::s_pUIMgr->ReFocusUI();// this_ui
			CN3UIBase* pFocus = CGameProcedure::s_pUIMgr->GetFocusedUI();
			if(pFocus && pFocus != this) pFocus->OnKeyPress(iKey);
		}
		return true;
	}

	return CN3UIBase::OnKeyPress(iKey);
}
