#pragma once

#include "N3UIBase.h"
#include <list>

class CN3UIString;

class CN3UIList : public CN3UIBase  
{
protected:
	int						m_iCurSel;		// Current selection...
	std::list<CN3UIString*>	m_ListString;	// String List
	class CN3UIScrollBar*	m_pScrollBarRef;

	std::string				m_szFontName;
	DWORD					m_dwFontHeight;
	BOOL					m_bFontBold;
	BOOL					m_bFontItalic;
	D3DCOLOR				m_crFont;
	
public:
	const std::string&	FontName() { return m_szFontName; }
	DWORD				FontHeight() const { return m_dwFontHeight; }
	D3DCOLOR			FontColor() { return m_crFont; }
	BOOL				FontIsBold() { return m_bFontBold; }
	BOOL				FontIsItalic() { return m_bFontItalic; }

	void	SetFont(const std::string& szFontName, DWORD dwHeight, BOOL bBold, BOOL bItalic);
	void	SetFontColor(D3DCOLOR color);
	void	SetFontColor(int iIndex, D3DCOLOR color);

	void	ResetContent();
	void	UpdateChildRegions();
	int		AddStrings(const std::string* pszStrings, int iStringCount);
	int		AddString(const std::string& szString);
	bool	InsertString(int iIndex, const std::string& szString);
	bool	DeleteString(int iIndex);
	bool	GetString(int iIndex, std::string& szString);
	bool	SetString(int iIndex, const std::string& szString);
	int		GetCurSel() { return m_iCurSel; }
	bool	SetCurSel(int iIndex) { if(iIndex < 0 || iIndex >= m_ListString.size()) m_iCurSel = -1; else m_iCurSel = iIndex; return true; }
	int		GetCount() { return m_ListString.size(); }

	int		GetScrollPos() const;
	bool	SetScrollPos(int iScrollPos);
	
	virtual void	Render();
	virtual bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg);
	virtual bool	Load(HANDLE hFile);
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);

#ifdef _N3TOOL
	virtual bool	Save(HANDLE hFile);
	virtual void	operator = (const CN3UIList& other);
#endif

	virtual void	Release();
	CN3UIList();
	virtual ~CN3UIList();
};
