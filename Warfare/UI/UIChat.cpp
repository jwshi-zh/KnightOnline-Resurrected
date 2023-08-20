#include "pch.h"
#include "UIChat.h"
#include "PacketDef.h"
#include "GameProcMain.h"
#include "UIMessageWnd.h"
#include "UIManager.h"

#include "N3UIString.h"
#include "N3UIScrollBar.h"
#include "N3UIEdit.h"										// son, chat_in

CUIChat::CUIChat()													// Do you not call Release in the constructor and destructor??
{
	m_pChatOut = nullptr;
	m_pScrollbar = nullptr;
	m_iChatLineCount = 0;
	m_ppUILines = nullptr;
	m_iCurContinueMsg = 0;
	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));

	m_eChatMode = N3_CHAT_NORMAL;
	// m_eChatBuffer = CHAT_BUFFER_NORMAL;

	m_pBtn_Normal = nullptr;
	m_pBtn_Private = nullptr;
	m_pBtn_PartyOrForce = nullptr;
	m_pBtn_KnightsOrGuild = nullptr;
	m_pBtn_Shout = nullptr;
	m_pNoticeTitle = nullptr;

	m_pEdit = nullptr;													// son, chat_in

	m_bChatNormal	= true;
	m_bChatPrivate	= true;
	m_bChatClan		= true;
	m_bChatShout	= true;
	m_bChatParty	= true;

	m_bKillFocus	= false;
}

CUIChat::~CUIChat()
{
	if (m_ppUILines) {delete [] m_ppUILines; m_ppUILines = nullptr;}	// Even if the pointer of m_ppUILines[n] is allocated, it should not be deleted because it is automatically released when the parent is freed.

	// for(int i = 0; i < CHAT_BUFFER_COUNT; i++)
	// {
		// for(itor = m_ChatBuffers[i].begin(); m_ChatBuffers[i].end() != itor; ++itor)
		for(auto itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
		{
			const __ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
		// m_ChatBuffers[i].clear();
		m_ChatBuffer.clear();

		// for(itor = m_LineBuffers[i].begin(); m_LineBuffers[i].end() != itor; ++itor)
		for(auto itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
		{
			const __ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
		// m_LineBuffers[i].clear();
		m_LineBuffer.clear();
	// }

	DeleteContinueMsg();
}

void CUIChat::Release()
{
	CN3UIBase::Release();

	m_pEdit = nullptr;													// son, chat_in

	m_pChatOut = nullptr;
	m_pScrollbar = nullptr;
	m_iChatLineCount = 0;
	if (m_ppUILines) {delete [] m_ppUILines; m_ppUILines = nullptr;}	// Even if the pointer of m_ppUILines[n] is allocated, it should not be deleted because it is automatically released when the parent is freed.
	ZeroMemory(&m_rcChatOutRegion, sizeof(m_rcChatOutRegion));

	// for(int i = 0; i < CHAT_BUFFER_COUNT; i++)
	// {
	// for(itor = m_ChatBuffers[i].begin(); m_ChatBuffers[i].end() != itor; ++itor)
		for(auto itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
		{
			const __ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
		// m_ChatBuffers[i].clear();
		m_ChatBuffer.clear();

		// for(itor = m_LineBuffers[i].begin(); m_LineBuffers[i].end() != itor; ++itor)
		for(auto itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
		{
			const __ChatInfo* pChatInfo = (*itor);
			if (pChatInfo) delete pChatInfo;
		}
		// m_LineBuffers[i].clear();
		m_LineBuffer.clear();
	// }

	m_eChatMode = N3_CHAT_NORMAL;
	// m_eChatBuffer = CHAT_BUFFER_NORMAL;

	m_pBtn_Normal = nullptr;
	m_pBtn_Private = nullptr;
	m_pBtn_PartyOrForce = nullptr;
	m_pBtn_KnightsOrGuild = nullptr;
	m_pBtn_Shout = nullptr;
}

bool CUIChat::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if(nullptr == pSender) return false;

	if(dwMsg == UIMSG_BUTTON_CLICK)
	{
		e_ChatMode eCM = N3_CHAT_UNKNOWN;

		if(pSender == m_pBtn_Normal) eCM = N3_CHAT_NORMAL;
		else if(pSender == m_pBtn_Private) eCM = N3_CHAT_PRIVATE;
		else if(pSender == m_pBtn_PartyOrForce) eCM = N3_CHAT_PARTY;
		else if(pSender == m_pBtn_KnightsOrGuild) eCM = N3_CHAT_CLAN;
		else if(pSender == m_pBtn_Shout) eCM = N3_CHAT_SHOUT;

		if(pSender == m_pBtn_Check)
			ChatListenEnable();

		if(N3_CHAT_UNKNOWN != eCM)
			this->ChangeChattingMode(eCM); // When the chat mode changes...
	}

	else if (dwMsg == UIMSG_SCROLLBAR_POS)
	{
		// Set chat line for scroll bar
		const int iCurLinePos = m_pScrollbar->GetCurrentPos();
		SetTopLine(iCurLinePos);
	}
	
	// son, chat_in
	else if (dwMsg == UIMSG_EDIT_RETURN)
	{
		auto* pEdit = (CN3UIEdit*)pSender;
		// Chat m_pEdit-&gt;SetString(&quot;&quot;); After doing that, the pointer obtained by m_pEdit-&gt;GetString();
		// Be careful as this will result in an invalid pointer.

		// Copied to buffer.
		m_szString = m_pEdit->GetString();
		const int iStrLen = m_szString.size();
		if (iStrLen > 0)
		{
			if(iStrLen > 1 && '/' == m_szString[0])
			{
				CGameProcedure::s_pProcMain->ParseChattingCommand(m_szString);
			}
			else if(iStrLen > 1 && '@' == m_szString[0])
			{
				const int iTmp = m_szString.find(' ');
				if(iTmp > 0)
				{
					const std::string szID = m_szString.substr(1, iTmp-1);
					CGameProcedure::s_pProcMain->MsgSend_ChatSelectTarget(szID);
					CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_PRIVATE, &(m_szString[1]));
				}
			}
			else if(iStrLen > 1 && '#' == m_szString[0]) // party
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_PARTY, &(m_szString[1]));
			}
			else if(iStrLen > 1 && '$' == m_szString[0]) // clan
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_CLAN, &(m_szString[1]));
			}
			else if(iStrLen > 1 && '!' == m_szString[0]) // bawling
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(N3_CHAT_SHOUT, &(m_szString[1]));
			}
			else
			{
				CGameProcedure::s_pProcMain->MsgSend_Chat(m_eChatMode, m_szString);
				// this->SetFocus();
			}
		}

		// Erase the text displayed on the screen.
		// this->SetString("");
		// ::SetWindowText(s_hWndEdit, &quot;&quot;);

		SetEnableKillFocus(true);
	}
	// son, chat_in

	return true;
}

void CUIChat::CreateLines()
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
	if (m_pChatOut && m_pChatOut->GetTextExtent("가", lstrlen("가"), &size) && size.cy>0)
	{
		m_iChatLineCount = (m_rcChatOutRegion.bottom - m_rcChatOutRegion.top)/size.cy;
	}
	else return;
	const std::string& szFontName = m_pChatOut->GetFontName();
	const DWORD dwFontHeight = m_pChatOut->GetFontHeight();
	const DWORD dwFontFlag = m_pChatOut->GetFontFlags();

	if (m_iChatLineCount<=0 || szFontName.empty()) return;

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

bool CUIChat::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	m_pChatOut		= (CN3UIString*)GetChildByID("text0");				__ASSERT(m_pChatOut, "NULL UI Component!!");
	m_pScrollbar	= (CN3UIScrollBar*)GetChildByID("scroll");			__ASSERT(m_pScrollbar, "NULL UI Component!!");
	m_pNoticeTitle	= (CN3UIString*)GetChildByID("text_notice_title");	__ASSERT(m_pNoticeTitle, "NULL UI Component!!");

	m_rcChatOutRegion = m_pChatOut->GetRegion();
	CreateLines();

	__ASSERT(0<m_iChatLineCount,"채팅창이 너무 작아요");

	// son, chat_in
	m_pEdit = (CN3UIEdit*)GetChildByID("edit0");				__ASSERT(m_pEdit, "NULL UI Component!!");
	m_pEdit->SetMaxString(256); // Chat string length limit..
	// son, chat_in

	m_pBtn_Normal			= GetChildByID("btn_normal");			__ASSERT(m_pBtn_Normal, "NULL UI Component!!");
	m_pBtn_Private			= GetChildByID("btn_private");			__ASSERT(m_pBtn_Private, "NULL UI Component!!");
	m_pBtn_PartyOrForce		= GetChildByID("btn_party_force");		__ASSERT(m_pBtn_PartyOrForce, "NULL UI Component!!");
	// m_pBtn_KnightsOrGuild = GetChildByID("btn_knights_guild");	__ASSERT(m_pBtn_KnightsOrGuild, "NULL UI Component!!");
	m_pBtn_KnightsOrGuild	= GetChildByID("btn_clan");				__ASSERT(m_pBtn_KnightsOrGuild, "NULL UI Component!!");
	m_pBtn_Shout			= GetChildByID("btn_shout");			__ASSERT(m_pBtn_Shout, "NULL UI Component!!");
	m_pBtn_Check			= GetChildByID("btn_check");			__ASSERT(m_pBtn_Check, "NULL UI Component!!");

	this->ChangeChattingMode(N3_CHAT_NORMAL); // This is normal chat mode.

	return true;
}

void CUIChat::AddChatMsg(e_ChatMode eCM, const std::string& szString, D3DCOLOR color)
{
	if(szString.empty()) return;

	switch(eCM)
	{
	case N3_CHAT_NORMAL:
		if(!m_bChatNormal) return;
		break;
	case N3_CHAT_PRIVATE:
		if(!m_bChatPrivate) return;
		break;
	case N3_CHAT_PARTY:
	case N3_CHAT_FORCE:
		if(!m_bChatParty) return;
		break;
	case N3_CHAT_CLAN:
		if(!m_bChatClan) return;
		break;
	case N3_CHAT_SHOUT:
		if(!m_bChatShout) return;
		break;
	}

	// N3_CHAT_NORMAL = 1, N3_CHAT_PRIVATE, N3_CHAT_PARTY, N3_CHAT_FORCE, N3_CHAT_SHOUT, N3_CHAT_KNIGHTS, N3_CHAT_PUBLIC
	// e_ChatBuffer eCB = CHAT_BUFFER_NORMAL; // Chatting Mode Index 0 - Normal, shout, notice | 1 - private | 2 - party force | 3 - Knights
	// switch(eCM)
	// {
	// case N3_CHAT_NORMAL:
	// case N3_CHAT_SHOUT:
	// case N3_CHAT_PUBLIC:
		// eCB = CHAT_BUFFER_NORMAL;
		// break;
	// case N3_CHAT_PRIVATE:
		// eCB = CHAT_BUFFER_PRIVATE;
		// break;
	// case N3_CHAT_PARTY:
	// case N3_CHAT_FORCE:
		// eCB = CHAT_BUFFER_PARTY;
		// break;
	// case N3_CHAT_KNIGHTS:
		// eCB = CHAT_BUFFER_KNIGHTS;
		// break;
	// }

	// Put it in a regular ChatBuffer
	// if(CHAT_BUFFER_NORMAL != eCB)
	// {
	auto* pChatInfo = new __ChatInfo(szString, color);
	// m_ChatBuffers[CHAT_BUFFER_NORMAL].push_back(pChatInfo);
		m_ChatBuffer.push_back(pChatInfo);
		// if (m_ChatBuffers[CHAT_BUFFER_NORMAL].size() &gt; 255) // If there are more than 255, clear from the front
		if (m_ChatBuffer.size() > 255)	// If more than 255, erase from the front
		{
			// __ChatInfo* pTemp = m_ChatBuffers[CHAT_BUFFER_NORMAL].front();
			const __ChatInfo* pTemp = m_ChatBuffer.front();
			if (pTemp) delete pTemp;

			// m_ChatBuffers[CHAT_BUFFER_NORMAL].pop_front();
			m_ChatBuffer.pop_front();
		}
		// this->AddLineBuffer(CHAT_BUFFER_NORMAL, szString, color); // line buffer 에 넣기
		this->AddLineBuffer(szString, color); // put in line buffer
	// }

	// Put into ChatBuffer
	// __ChatInfo* pChatInfo = new __ChatInfo(szString, color);
	// m_ChatBuffers[eCB].push_back(pChatInfo);
	// if (m_ChatBuffers[eCB].size() &gt; 255) // If there are more than 255, clear from the front
	// {
		// __ChatInfo* pTemp = m_ChatBuffers[eCB].front();
		// if (pTemp) delete pTemp;
		//
		// m_ChatBuffers[eCB].pop_front();
	// }
	// this->AddLineBuffer(eCB, szString, color); // line buffer 에 넣기

	this->AdjustScroll(); // Adjust scroll bar and scroll position, etc.
}

void CUIChat::AdjustScroll()
{
	// Adjusting the number of line buffers
	int iCurLinePos = m_pScrollbar->GetCurrentPos();	// The line currently pointed by the scroll bar
	const BOOL bAutoScroll = (m_pScrollbar->GetMaxPos() == iCurLinePos) ? TRUE : FALSE;

	// while (m_LineBuffers[m_eChatBuffer].size() &gt; MAX_CHAT_LINES &amp;&amp; 0 &lt; iCurLinePos) // MAX_CHAT_LINES is the maximum number of lines.
	while (m_LineBuffer.size() > MAX_CHAT_LINES && 0 < iCurLinePos)	// MAX_CHAT_LINES is the maximum number of lines (however, if the scroll bar is at 0, lines are not deleted, so 500 can be exceeded)
	{
		// delete one line
		// __ChatInfo* pTemp = m_LineBuffers[m_eChatBuffer].front();
		const __ChatInfo* pTemp = m_LineBuffer.front();
		if (pTemp) delete pTemp;
		// m_LineBuffers[m_eChatBuffer].pop_front();
		m_LineBuffer.pop_front();
		--iCurLinePos;	
	}

	// int iLineBufferSize = m_LineBuffers[m_eChatBuffer].size();
	const int iLineBufferSize = m_LineBuffer.size();
	int iMaxScrollPos = iLineBufferSize-m_iChatLineCount;
	if (iMaxScrollPos<0) iMaxScrollPos = 0;
	m_pScrollbar->SetRange(0, iMaxScrollPos);	// Set scroll bar range

	// if it scrolls automatically
	if ( bAutoScroll) iCurLinePos = iMaxScrollPos;
	if (iCurLinePos < 0) iCurLinePos = 0;

	// Reset scrollbar current position
	m_pScrollbar->SetCurrentPos(iCurLinePos);

	// Set chat line for scroll bar
	SetTopLine(iCurLinePos);
}

// void CUIChat::AddLineBuffer(e_ChatBuffer eCB, const std::string& szString, D3DCOLOR color)
void CUIChat::AddLineBuffer(const std::string& szString, D3DCOLOR color)
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
			// m_LineBuffers[eCB].push_back(pLineInfo);
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
					// m_LineBuffers[eCB].push_back(pLineInfo);
					m_LineBuffer.push_back(pLineInfo);

					pLineInfo->color = color;
					pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
				}
				else
				{
					__ASSERT(iRegionWidth>15, "너무 좁아서 한글자도 찍을 수가 없다");
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

	// Last exit processing
	const int iLineLength = iStrLen - iLineStart;
	if (iLineLength>0)
	{
		auto* pLineInfo = new __ChatInfo;
		// m_LineBuffers[eCB].push_back(pLineInfo);
		m_LineBuffer.push_back(pLineInfo);

		pLineInfo->color = color;
		pLineInfo->szChat = szString.substr(iLineStart, iLineLength);
	}
}

void CUIChat::SetTopLine(int iTopLine) const
{
	if (m_iChatLineCount<=0) return;

	// const int iLineBufferSize = m_LineBuffers[m_eChatBuffer].size();
	const int iLineBufferSize = m_LineBuffer.size();
	if (iTopLine<0) iTopLine = 0;
	else if (iTopLine > iLineBufferSize) iTopLine = iLineBufferSize;
	
	int i;
	// Store in a temporary buffer in order from the previous row and measure the length of the string
	auto** ppLineInfos  = new __ChatInfo*[m_iChatLineCount];
	ZeroMemory(ppLineInfos, sizeof(__ChatInfo*)*m_iChatLineCount);

	int iCurLine = 0;
	for (i=0; i<m_iChatLineCount; ++i)
	{
		iCurLine = iTopLine + i;
		if (iLineBufferSize <= iCurLine) break;
		// ppLineInfos[i] = m_LineBuffers[m_eChatBuffer][iCurLine];
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

void CUIChat::RecalcLineBuffers()	// If called when the size of the chat window changes, the line buffer is recalculated and inserted.
{
	int iMaxScrollPos = 0;
	// for(int i = 0; i < CHAT_BUFFER_COUNT; i++)
	// {
		// Initializing the line buffer
		// for(itor = m_LineBuffers[i].begin(); m_LineBuffers[i].end() != itor; ++itor)
		for(auto itor = m_LineBuffer.begin(); m_LineBuffer.end() != itor; ++itor)
		{
			const __ChatInfo* pLineBuff = (*itor);
			if (pLineBuff) delete pLineBuff;
		}
		// m_LineBuffers[i].clear();
		m_LineBuffer.clear();

		// Reload the line buffer
		// for(itor = m_ChatBuffers[i].begin(); m_ChatBuffers[i].end() != itor; ++itor)
		for(auto itor = m_ChatBuffer.begin(); m_ChatBuffer.end() != itor; ++itor)
		{
			const __ChatInfo* pChatBuff = (*itor);
			// if (pChatBuff) AddLineBuffer((e_ChatBuffer)i, pChatBuff->szChat, pChatBuff->color);
			if (pChatBuff) AddLineBuffer(pChatBuff->szChat, pChatBuff->color);
		}

		// Adjusting the number of line buffers
		// while (m_LineBuffers[i].size() &gt; MAX_CHAT_LINES) // MAX_CHAT_LINES is the maximum number of lines
		while (m_LineBuffer.size() > MAX_CHAT_LINES)	// MAX_CHAT_LINES is the maximum number of lines
		{
			// delete one line
			// __ChatInfo* pLineBuff = m_LineBuffers[i].front();
			const __ChatInfo* pLineBuff = m_LineBuffer.front();
			if (pLineBuff) delete pLineBuff;
			// m_LineBuffers[i].pop_front();
			m_LineBuffer.pop_front();
		}

	// if(i == m_eChatBuffer)
	// {
	// int iLineBufferSize = m_LineBuffers[i].size();
	const int iLineBufferSize = m_LineBuffer.size();
			iMaxScrollPos = iLineBufferSize-m_iChatLineCount;
	// }
	// }

	// Reset scrollbar current position
	if (iMaxScrollPos<0) iMaxScrollPos = 0;
	m_pScrollbar->SetRange(0, iMaxScrollPos);	// Set scroll bar range
	m_pScrollbar->SetCurrentPos(iMaxScrollPos);
	
	// Set chat line for scroll bar
	SetTopLine(iMaxScrollPos);
}

// son, chat_in
void CUIChat::SetFocus()
{
	SetEnableKillFocus(false);
	this->SetString("");
	if (m_pEdit) m_pEdit->SetFocus();
}

void CUIChat::KillFocus()
{
	this->SetString("");
	if (m_pEdit) m_pEdit->KillFocus();
}

BOOL CUIChat::IsChatMode()
{
	return ((m_pEdit && GetFocusedEdit() == m_pEdit) ? TRUE : FALSE);			// TRUE --&gt; When not in chatting mode
}
// son, chat_in

void CUIChat::SetString(const std::string& szChat)
{
	m_szString = szChat;
	if(m_pEdit)
	{
		m_pEdit->SetString(m_szString);
		m_pEdit->SetCaretPos(m_szString.size());
	}
}

void CUIChat::SetCaretPos(int iPos)
{
	if (IsChatMode() && m_pEdit)
	{
		m_pEdit->SetCaretPos(iPos);
	}
}

BOOL CUIChat::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (0 == iOffsetX && 0 == iOffsetY) return FALSE;
	// ui area
	m_rcRegion.left += iOffsetX;		m_rcRegion.top += iOffsetY;
	m_rcRegion.right += iOffsetX;		m_rcRegion.bottom += iOffsetY;

	// movable area
	m_rcMovable.left += iOffsetX;		m_rcMovable.top += iOffsetY;
	m_rcMovable.right += iOffsetX;		m_rcMovable.bottom += iOffsetY;

	// update children coordinates
	CN3UIBase* pCUI = nullptr; // Child UI...
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		pCUI = (*itor);
		__ASSERT(pCUI, "child UI pointer is NULL!");
		pCUI->MoveOffset(iOffsetX, iOffsetY);
	}

	if(CGameProcedure::s_pProcMain->m_pUIMsgDlg)
	{
		const POINT pt = CGameProcedure::s_pProcMain->m_pUIMsgDlg->GetPos();
		const RECT rt = this->GetRegion();
		if( (rt.right != pt.x) || (rt.top != pt.y) )
		{
			CGameProcedure::s_pProcMain->m_pUIMsgDlg->SetPos(rt.right, rt.top);
		}
	}

	return TRUE;
}

void CUIChat::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	// Arrange your children appropriately.
	// m_rcChatOutRegion = ; // Specifies the chat output area again.
	// CreateLines(); // Calculate how many chat lines will fit and recreate them.
	// RecalcLineBuffers(); // Clear the line buffer, recreate it, and display the text.
}

void CUIChat::ChangeChattingMode(e_ChatMode eCM)
{
	m_eChatMode = eCM;
	// e_ChatBuffer eCBPrev = m_eChatBuffer;

	bool bNBDs[5] = { false, false, false, false, false };
	const bool bICLs[5] = { m_bChatNormal, m_bChatPrivate, m_bChatParty, m_bChatClan, m_bChatShout };
	CN3UIBase* pBtns[5] = { m_pBtn_Normal, m_pBtn_Private, m_pBtn_PartyOrForce, m_pBtn_KnightsOrGuild, m_pBtn_Shout };

	switch(m_eChatMode)
	{
	case N3_CHAT_NORMAL:
	case N3_CHAT_PUBLIC:
	case N3_CHAT_CONTINUE:
		// m_eChatBuffer = CHAT_BUFFER_NORMAL;
		bNBDs[0] = true;
		break;
	case N3_CHAT_PRIVATE:
		// m_eChatBuffer = CHAT_BUFFER_PRIVATE;
		bNBDs[1] = true;
		break;
	case N3_CHAT_PARTY:
	case N3_CHAT_FORCE:
		// m_eChatBuffer = CHAT_BUFFER_PARTY;
		bNBDs[2] = true;
		break;
	case N3_CHAT_CLAN:
		// m_eChatBuffer = CHAT_BUFFER_KNIGHTS;
		bNBDs[3] = true;
		break;
	case N3_CHAT_SHOUT:
		// m_eChatBuffer = CHAT_BUFFER_NORMAL;
		bNBDs[4] = true;
		break;
	}

	for(int i = 0; i < 5; i++)
	{
		if(nullptr == pBtns[i]) continue;
		
		if(bNBDs[i])
		{
			if(m_pBtn_Check)
			{
				if(bICLs[i])	m_pBtn_Check->SetState(UI_STATE_BUTTON_DOWN);
				else			m_pBtn_Check->SetState(UI_STATE_BUTTON_NORMAL);
			}
			pBtns[i]->SetState(UI_STATE_BUTTON_DOWN);
		}
		else pBtns[i]->SetState(UI_STATE_BUTTON_NORMAL);
	}

	// if(eCBPrev != m_eChatBuffer) this-&gt;AdjustScroll(); // If the chat mode changes...
}

void CUIChat::ChatListenEnable()
{
	switch(m_eChatMode)
	{
	case N3_CHAT_NORMAL:
		m_bChatNormal = !m_bChatNormal;
		break;
	case N3_CHAT_PRIVATE:
		m_bChatPrivate = !m_bChatPrivate;
		break;
	case N3_CHAT_PARTY:
	case N3_CHAT_FORCE:
		m_bChatParty = !m_bChatParty;
		break;
	case N3_CHAT_CLAN:
		m_bChatClan = !m_bChatClan;
		break;
	case N3_CHAT_SHOUT:
		m_bChatShout = !m_bChatShout;
		break;
	}
}

bool CUIChat::OnKeyPress(int iKey)
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

void CUIChat::AddContinueMsg(e_ChatMode eCM, const std::string& szString, D3DCOLOR color)
{
	auto* pChatInfo = new __ChatInfo(szString, color);
	m_ContinueMsg.push_back(pChatInfo);

	AddChatMsg(eCM, szString, color);	
}

void CUIChat::DeleteContinueMsg()
{
	m_iCurContinueMsg = 0;

	for(auto itor = m_ContinueMsg.begin(); m_ContinueMsg.end() != itor; ++itor)
	{
		const __ChatInfo* pChatInfo = (*itor);
		if (pChatInfo) delete pChatInfo;
	}
	m_ContinueMsg.clear();
}

void CUIChat::ShowContinueMsg()
{
	const int iSize = m_ContinueMsg.size();
	if( m_iCurContinueMsg > iSize )
		m_iCurContinueMsg = 0;

	const int iCnt = 0;
	for(auto itor = m_ContinueMsg.begin(); m_ContinueMsg.end() != itor; ++itor)
	{
		if( iCnt == m_iCurContinueMsg )
		{
			m_iCurContinueMsg++;
			const __ChatInfo* pChatInfo = (*itor);
			if(pChatInfo) AddChatMsg(N3_CHAT_CONTINUE, pChatInfo->szChat, pChatInfo->color);	
		}
	}
}

void CUIChat::SetNoticeTitle(const std::string& szString, D3DCOLOR color)
{
	if(m_pNoticeTitle)
	{
		m_pNoticeTitle->SetString(szString);
		m_pNoticeTitle->SetColor(color);
	}
}
