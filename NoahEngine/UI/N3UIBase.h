#pragma once

#include "N3BaseFileAccess.h"
#include "N3UIDef.h"
#include <DInput.h>
#include <list>
#include <set>
#include <string>

class CN3UIBase;
using namespace std;
typedef list<CN3UIBase*>			UIList;
typedef UIList::iterator			UIListItor;
typedef UIList::const_iterator		UIListItorConst;
typedef UIList::reverse_iterator	UIListReverseItor;

class CN3UIImage;
class CN3UIString;
class CN3UIButton;
class CN3UIStatic;
class CN3UIProgress;
class CN3UITrackBar;
class CN3UIScrollBar;
class CN3UIEdit;
class CN3UITooltip;
class CN3UIArea;
class CN3SndObj;
class CN3UIList;

class CN3UIBase : public CN3BaseFileAccess
{
#ifdef _N3TOOL
friend class CN3UIBase;
friend class CHierarchyView;	// To access the child list in the tool.
friend class CPropertyView;	// To access each variable in the tool
friend class CUIEView;	// To access the child list in the tool.
#endif

public:
	std::string m_szID;			// User id
	std::string	m_szToolTip;	// tooltip text
	void		SetID(LPCTSTR pszID) {m_szID = pszID;}
	const std::string GetID() const {return m_szID;}
	void		SetTooltipText(LPCTSTR pszTT) {m_szToolTip = pszTT;}

	static CN3UITooltip*	s_pTooltipCtrl;		// tool tip
	
	CN3UIBase*	m_pChildUI;		// It is not part of the UI, but has other UI as children.
	CN3UIBase*	m_pParentUI;

protected:
	static std::string	s_szStringTmp;		// Temporary string.. to pass a pointer..

	int			m_iChildID;
	CN3UIBase*	m_pParent;		// parent pointer
	UIList		m_Children;		// children pointer list
	eUI_TYPE	m_eType;		// UI Type - button, image .....
	eUI_STATE	m_eState;		// UI states
	DWORD		m_dwStyle;		// style
	DWORD		m_dwReserved;	// You can enter any other information you want to put in temporarily.

	RECT		m_rcRegion;		// Of the UI - screen coordinates (screen: main window client area): Coordinates not relative to the parent.
	RECT		m_rcMovable;	// Area where UI can be moved by dragging - (screen : main window client area) ~~~~~~~

	bool		m_bVisible;		// Is it visible on the screen (if the parent is not visible, the children do not render)
	CN3SndObj*	m_pSnd_OpenUI;	// The sound that the UI makes when it is visible on the screen
	CN3SndObj*	m_pSnd_CloseUI;	// The sound made when the UI disappears from the screen

	static CN3UIEdit*		s_pFocusedEdit;		// Edit that currently has focus, if NULL no one has focus.
	
public:
	CN3UIBase();
	virtual ~CN3UIBase();

// Attributes
public:
	eUI_TYPE		UIType() const { return m_eType; }
	eUI_STATE 		GetState() const { return m_eState; }
	bool			IsVisible()	const { return m_bVisible; }
	RECT			GetRegion() const { return m_rcRegion; }
	void			SetMoveRect(const RECT& Rect) { m_rcMovable = Rect; }
	RECT			GetMoveRect() const { return m_rcMovable; }
	void			SetReserved(DWORD dwReserved) {m_dwReserved = dwReserved;}
	DWORD			GetReserved() const {return m_dwReserved;}
	CN3UIBase*		GetParent() const {return m_pParent;}
	static CN3UIEdit*	GetFocusedEdit() {return s_pFocusedEdit;}
	static CN3UITooltip*	GetTooltipCtrl() {return s_pTooltipCtrl;}
	DWORD			GetStyle()	{return m_dwStyle;}

	void			SetUIType(eUI_TYPE eUIType) { m_eType = eUIType; }	// by ecli666 It's annoying to add functions to the tool.. ^^
// Operations
public:	
	bool			IsIn(int x, int y) const;
	void			AddChild(CN3UIBase* pChild) { m_Children.push_front(pChild); }
	void			RemoveChild(CN3UIBase* pChild); // In the child list, only the pointer disappears and is not actually deleted.
	void			SetParent(CN3UIBase* pParent);	// change parents
	int				GetWidth() { return m_rcRegion.right - m_rcRegion.left; }
	int				GetHeight() { return m_rcRegion.bottom - m_rcRegion.top; }
	POINT			GetPos() const;
	virtual void	SetPos(int x, int y);	// Position designation (the position of chilren is also changed.) Calls the MoveOffset function internally.
	void			SetPosCenter();	// Align it to the center of the screen. (The position of chilren is also changed.) Call the MoveOffset function internally.
	CN3UIBase*		GetChildByID(const std::string& szID);

	virtual void	SetRegion(const RECT& pRect) { m_rcRegion = pRect; }	// zoning
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);	// It moves as much as the offset. (Move region, children, move rect)
	virtual void	SetSize(int iWidth, int iHeight);	// sizing
	virtual void	SetState(eUI_STATE eState) { m_eState = eState; }
	virtual void	SetStyle(DWORD dwStyle) {m_dwStyle = dwStyle;}	// style designation
	virtual void	SetVisible(bool bVisible);

	virtual void	SetVisibleWithNoSound(bool bVisible, bool bWork = false, bool bReFocus = false);

	virtual void	CallBackProc(int iID, DWORD dwFlag);
	virtual void	ShowWindow(int iID = -1, CN3UIBase* pParent = nullptr);
	virtual bool	Load(HANDLE hFile);
	virtual bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg); // Receives a message. Sender, msg
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Tick();
	virtual void	Render();
	virtual void	Release(); // Delete the child pointer as well.
	virtual void	Init(CN3UIBase* pParent);
	virtual bool	OnKeyPress(int iKey) { return false; }
	virtual bool	OnKeyPressed(int iKey) { return false; }

	static	bool	EnableTooltip(const std::string& szFN);	// Initializes the tooltip UI.
	static	void	DestroyTooltip();	// Releases related to tooltip ui.

	int				GetChildrenCount() { return m_Children.size(); }
	CN3UIBase*		GetChildByIndex(int iIndex)
	{
		if(iIndex < 0 || iIndex >= m_Children.size()) return nullptr;
		auto it = m_Children.begin();
		for(int i = 0; i < iIndex; it++, i++);
		return *it;
	}
	virtual void	operator = (const CN3UIBase& other);

protected:

#ifdef _N3TOOL
public:
	virtual bool	Save(HANDLE hFile);
	virtual void	ChangeImagePath(const std::string& szPathOld, const std::string& szPathNew);
	virtual void	ChangeFont(const std::string& szFont);
	virtual void	GatherImageFileName(std::set<std::string>& setImgFile);
	
	void			ResizeAutomaticalyByChild();
	int				IsMyChild(CN3UIBase* pUI);
	bool			SwapChild(CN3UIBase* pChild1, CN3UIBase* pChild2);
	
	bool			MoveToLower(CN3UIBase* pChild);
	bool			MoveToUpper(CN3UIBase* pChild);
	bool			MoveToLowest(CN3UIBase* pChild);
	bool			MoveToHighest(CN3UIBase* pChild);
	
	void			ArrangeZOrder();
	RECT			GetMoveRect() const {return m_rcMovable;}
	void			SetMoveRect(RECT rc) {m_rcMovable = rc;}
	void			SetSndOpen(const std::string& strFileName);
	void			SetSndClose(const std::string& strFileName);
	std::string		GetSndFName_OpenUI() const;
	std::string		GetSndFName_CloseUI() const;

	virtual bool	ReplaceAllTextures(const std::string& strFind, const std::string& strReplace);
#endif
};
