#pragma once

#include "N3UIBase.h"
#include "GameDef.h"
#include "UIChat.h"

class CUIMessageWnd : public CN3UIBase  
{
protected:
	CN3UIString*	m_pChatOut;		// UIString reference pointer where chatting is output (actually managed as m_Child), only references font and initial area.
	CN3UIScrollBar* m_pScrollbar;	// scrollbar reference pointer (actually managed by m_Child)

	deque<__ChatInfo*>		m_ChatBuffer;		// Buffer based on chat packets
	deque<__ChatInfo*>		m_LineBuffer;		// Line-based buffer
	
	int				m_iChatLineCount;	// The number of lines displayed in the chat window (recalculate when the size of the chat window changes.)
	RECT			m_rcChatOutRegion;	// Area where chat is displayed
	CN3UIString**	m_ppUILines;		// UIString array pointer where the chat is displayed (as the size of the chat window changes, the array also changes.

protected:
	void			SetTopLine(int iTopLine) const;		// Specifies the top row.
	void			AddLineBuffer(const std::string& szString, D3DCOLOR color);	// Creates a line buffer. (If it is too long, it automatically creates two lines.)
	void			RecalcLineBuffer();		// If called when the size of the chat window changes, the line buffer is recalculated and inserted.
	void			CreateLines();

// Operations
public:
	bool OnKeyPress(int iKey);
	BOOL	MoveOffset(int iOffsetX, int iOffsetY);	// It moves as much as offsetY. (region, children, move rect)
	bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	void	Release();
	bool	Load(HANDLE hFile);
	void	SetRegion(const RECT& Rect);	// Area designation (called when the size changes. Simple movement is not called. (MoveOffset is called for simple movement))

	void	AddMsg(const std::string& szString, D3DCOLOR color = 0xffffffff);		// Saves the chat message and displays it on the screen in an appropriate format.

	CUIMessageWnd();
	virtual ~CUIMessageWnd();
};
