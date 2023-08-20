#pragma once

#include "N3UIBase.h"
#include <deque>

// enum e_ChatBuffer { CHAT_BUFFER_NORMAL = 0, CHAT_BUFFER_PRIVATE, CHAT_BUFFER_PARTY, CHAT_BUFFER_KNIGHTS, CHAT_BUFFER_COUNT };

struct __ChatInfo
{
	std::string szChat; // string
	D3DCOLOR	color; // string color

	__ChatInfo() {};
	__ChatInfo(const std::string& szChat_Arg, D3DCOLOR color_Arg) { szChat = szChat_Arg; color = color_Arg; }
};

const int MAX_CHAT_LINES = 100;

class CUIChat : public CN3UIBase  
{
protected:
	// ChatList m_ChatBuffers[CHAT_BUFFER_COUNT]; // buffer based on chat packet
	// ChatList m_LineBuffers[CHAT_BUFFER_COUNT]; // buffer based on line
	deque<__ChatInfo*>		m_ChatBuffer;		// Buffer based on chat packets
	deque<__ChatInfo*>		m_LineBuffer;		// Line based buffer
	deque<__ChatInfo*>		m_ContinueMsg;		// A message buffer that continuously notifies
	int				m_iCurContinueMsg;

	CN3UIString*	m_pNoticeTitle;		// A notice to be displayed on the top line of the chat window...
	CN3UIString*	m_pChatOut;			// UIString reference pointer where chatting is output (actually managed as m_Child), only references font and initial area.
	CN3UIScrollBar* m_pScrollbar;		// scrollbar reference pointer (actually managed by m_Child)
	int				m_iChatLineCount;	// The number of lines displayed in the chat window (recalculate when the size of the chat window changes.)
	RECT			m_rcChatOutRegion;	// Area where chat is displayed
	CN3UIString**	m_ppUILines;		// UIString array pointer where the chat is displayed (as the size of the chat window changes, the array also changes.

	CN3UIEdit*		m_pEdit;			// son, chat_in
	std::string		m_szString;			// son, chat_in

	CN3UIBase*		m_pBtn_Normal;
	CN3UIBase*		m_pBtn_Private;
	CN3UIBase*		m_pBtn_PartyOrForce;
	CN3UIBase*		m_pBtn_KnightsOrGuild;
	CN3UIBase*		m_pBtn_Shout;
	CN3UIBase*		m_pBtn_Check;

	enum e_ChatMode	m_eChatMode;

	bool			m_bChatNormal;
	bool			m_bChatPrivate;
	bool			m_bChatClan;
	bool			m_bChatShout;
	bool			m_bChatParty;

	bool			m_bKillFocus;
	// e_ChatBuffer m_eChatBuffer; // chat display mode .. the buffer is divided..
		
	/*
	ChatList m_MsgBuffer; // buffer based on chat packet
	ChatList m_MsgLineBuffer; // buffer based on line

	CN3UIString* m_pMsgOut; // UIString reference pointer where the chat is displayed (actually managed as m_Child)
	CN3UIScrollBar* m_pMsgScrollbar; // scrollbar reference pointer (actually managed as m_Child)
	int m_iMsgLineCount; // The number of lines displayed in the chat window (recalculate when the chat window size changes.)
	*/

protected:
	void			SetTopLine(int iTopLine) const;		// Specifies the top row.
	// void AddLineBuffer(e_ChatBuffer eCB, const std::string&amp; szString, D3DCOLOR color); // Creates a line buffer. (If it is too long, it automatically makes it into two lines.)
	void			AddLineBuffer(const std::string& szString, D3DCOLOR color);	// Creates a line buffer. (If it is too long, it automatically creates two lines.)
	void			RecalcLineBuffers();		// If called when the size of the chat window changes, the line buffer is recalculated and inserted.
	void			CreateLines();

// Operations
public:
	void			SetNoticeTitle(const std::string& szString, D3DCOLOR color);
	void			ShowContinueMsg();
	void			DeleteContinueMsg();
	bool			OnKeyPress(int iKey);
	bool			GetEnableKillFocus() const { return m_bKillFocus; }
	void			SetEnableKillFocus(bool bKillFocus) { m_bKillFocus = bKillFocus; }
	void			ChatListenEnable();
	void			ChangeChattingMode(e_ChatMode eCM);
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);	// It moves as much as the offset. (region, children, move rect)
	virtual bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual void	SetRegion(const RECT& Rect);	// Area designation (called when the size changes. Simple movement is not called. (MoveOffset is called for simple movement))

	void				SetString(const std::string& szChat);
	void				SetCaretPos(int iPos);
	const std::string&	GetString() const {return m_szString;}				// son, chat_in
	void				AddChatMsg(e_ChatMode eCM, const std::string& szString, D3DCOLOR color = 0xffffffff);		// Saves the chat message and displays it on the screen in an appropriate format.
	void				AddContinueMsg(e_ChatMode eCM, const std::string& szString, D3DCOLOR color = 0xffffffff);
	void				AdjustScroll(); // Adjust scroll position, etc.

	BOOL			IsChatMode();
	void			SetFocus();
	void			KillFocus();

	CUIChat();
	virtual ~CUIChat();
};
