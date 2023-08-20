#pragma once

#include <string>
#include "N3UIBase.h"
#include "DFont.h"

class CDFont;
class CN3UIString : public CN3UIBase  
{
	friend class CN3UIEdit;
#ifdef _N3TOOL
friend class CPropertyView;	// To access each variable in the tool
#endif

protected:
	CDFont*			m_pDFont;			// DFont (class that prints actual text on the screen)
	POINT			m_ptDrawPos;		// The coordinates of the top left of the text to be displayed on the actual screen
	std::string 	m_szString;			// string buffer
	D3DCOLOR		m_Color;			// text color
	int				m_iLineCount;		// The number of lines of currently set characters
	std::vector<int>	m_NewLineIndices;	// In case of multiline, indices of new lines
	int				m_iStartLine;		// In the case of multiline, the starting line number where text is printed
	uint32_t		m_iPadding;

public:
	CN3UIString();
	virtual ~CN3UIString();

// Attributes
public:
	void				SetColor(D3DCOLOR color) { m_Color = color; }
	D3DCOLOR			GetColor() const { return m_Color; }
	const std::string&	GetString() { return m_szString; }
	int					GetLineCount() const {return m_iLineCount;}
	int					GetStartLine() const {return m_iStartLine;}
	int					GetStringRealWidth(int iNum) const;

	virtual	DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Render();
	virtual void	Release();	
	virtual void	Init(CN3UIBase* pParent);
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);// The location of the writing also changes.
	virtual bool	Load(HANDLE hFile);
	void			ClearOnlyStringBuffer() { m_szString = ""; }	// Clear only the string buffer.
	void			SetStartLine(int iLine);	// Change the starting line for multiline

	virtual void	operator = (const CN3UIString& other);

#ifdef _N3TOOL
	virtual bool	Save(HANDLE hFile);
	virtual void	ChangeFont(const std::string& szFont);
#endif
	
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(DWORD dwStyle);
	virtual void	SetStyle(DWORD dwType, DWORD dwStyleEx);

	virtual void	SetString(const std::string& szString);
	virtual void	SetStringAsInt(int iVal);
	void			SetString_NoWordWrap(const std::string& szString);	// Do not align letters.
	virtual void	SetFont(const std::string& szFontName, DWORD dwHeight, BOOL bBold, BOOL bItalic); // dwHeight is the point size.
	BOOL			GetTextExtent(const const std::string& szString, int iStrLen, SIZE* pSize ) const
	{
		if (m_pDFont) return m_pDFont->GetTextExtent(szString, iStrLen, pSize);
		return FALSE;
	}
	DWORD				GetFontColor() const {if (m_pDFont) return m_pDFont->GetFontColor(); return 0xffffffff;}
	const std::string&	GetFontName() const {if (m_pDFont) return m_pDFont->GetFontName(); return s_szStringTmp;} 
	DWORD				GetFontHeight() const {if (m_pDFont) return m_pDFont->GetFontHeight(); return 0;} 
	DWORD				GetFontFlags() const {if (m_pDFont) return m_pDFont->GetFontFlags(); return 0;}
protected:
	void				WordWrap();		// wordwrap
};
