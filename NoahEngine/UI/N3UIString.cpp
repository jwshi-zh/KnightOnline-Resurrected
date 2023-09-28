#include "pch.h"
#include "N3UIString.h"

CN3UIString::CN3UIString()
{
	m_eType = UI_TYPE_STRING;
	m_pDFont = nullptr;

	m_Color = 0xffffffff;
	ZeroMemory(&m_ptDrawPos, sizeof(m_ptDrawPos));
	m_iLineCount = 0;
	m_iStartLine = 0;
}

CN3UIString::~CN3UIString()
{
	delete m_pDFont;	m_pDFont = nullptr;
}

void CN3UIString::Release()
{
	CN3UIBase::Release();

	m_szString = "";
	delete m_pDFont;	m_pDFont = nullptr;
	m_Color = 0xffffffff;

	ZeroMemory(&m_ptDrawPos, sizeof(m_ptDrawPos));
	m_iLineCount = 0;
	m_iStartLine = 0;
	m_NewLineIndices.clear();
}

void CN3UIString::Render()
{
	if(!m_bVisible) return;
	
	__ASSERT(m_pDFont,"m_pDFont is NULL");
	m_pDFont->DrawText( m_ptDrawPos.x, m_ptDrawPos.y, m_Color, 0 );

	CN3UIBase::Render();
}

void CN3UIString::Init(CN3UIBase* pParent)
{
	CN3UIBase::Init(pParent);

	if(m_pDFont) delete m_pDFont; m_pDFont = nullptr;
	m_pDFont = new CDFont("Arial", 16);	// Roll 16 by default
	m_pDFont->InitDeviceObjects( CN3Base::s_lpD3DDev );
	m_pDFont->RestoreDeviceObjects();
}

void CN3UIString::SetString(const std::string& szString)
{
	m_szString = szString;

	if(m_szString.empty()) m_pDFont->SetText("");
	else WordWrap();
}

void CN3UIString::SetStringAsInt(int iVal)
{
	char szBuff[32] = "";
	sprintf(szBuff, "%d", iVal);
	this->SetString(szBuff);
}

void CN3UIString::SetString_NoWordWrap(const std::string& szString)
{
	// Align it to the left and draw it.
	m_ptDrawPos.x = m_rcRegion.left;
	m_ptDrawPos.y = m_rcRegion.top;

	m_pDFont->SetText(szString.c_str());
}

void CN3UIString::SetFont(const std::string& szFontName, DWORD dwHeight, BOOL bBold, BOOL bItalic)
{
	DWORD dwFlag = 0;
	if (bBold) dwFlag |= D3DFONT_BOLD;
	if (bItalic) dwFlag |= D3DFONT_ITALIC;
	if (m_pDFont)
	{
		m_pDFont->SetFont(szFontName, dwHeight, dwFlag);
		WordWrap();
	}
}

void CN3UIString::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	WordWrap();
}

void CN3UIString::SetStyle(DWORD dwStyle)
{
	CN3UIBase::SetStyle(dwStyle);
	WordWrap();
}

// The writing position also changes.
BOOL CN3UIString::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	m_ptDrawPos.x += iOffsetX;	m_ptDrawPos.y += iOffsetY;
	return TRUE;
}

// Aligns text according to dwStyle.
void CN3UIString::WordWrap()
{
	m_iLineCount = 0;

	if (nullptr == m_pDFont) return;
	int iStrLen = m_szString.size();

	if (0 == iStrLen)
	{
		m_pDFont->SetText("");

		// It does not take text, but in the case of edit, caret is taken, so set the coordinates for drawing text.
		if (m_dwStyle & UISTYLE_STRING_SINGLELINE)
		{
			if (m_dwStyle & UISTYLE_STRING_ALIGNCENTER)	m_ptDrawPos.x = m_rcRegion.left + (m_rcRegion.right - m_rcRegion.left)/2;
			else m_ptDrawPos.x = m_rcRegion.left;

			if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER)
			{
				// Measure the pixel length of a string
				SIZE sizeTmp = {0, 0};
				m_pDFont->GetTextExtent(" ", 2, &sizeTmp);
				m_ptDrawPos.y = m_rcRegion.top + ((m_rcRegion.bottom - m_rcRegion.top-sizeTmp.cy)/2);
			}
			else m_ptDrawPos.y = m_rcRegion.top;
		}
		else
		{
			m_ptDrawPos.x = m_rcRegion.left;
			m_ptDrawPos.y = m_rcRegion.top;
		}

		return;
	}

	// Measure the pixel length of a string
	SIZE size;
	const std::string szString = GetString();
	iStrLen = szString.size();
	if (FALSE == m_pDFont->GetTextExtent(szString.c_str(), iStrLen, &size))
	{	// If the length cannot be measured (if m_hDC is not created)
		m_pDFont->SetText(szString);
		return;
	}

	const int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
	const int iRegionHeight = m_rcRegion.bottom - m_rcRegion.top;

	if (m_dwStyle & UISTYLE_STRING_SINGLELINE)
	{
		// catch temporary variable
		std::string szNewBuff;

		if (false && size.cy>iRegionHeight)	// If the text height is greater than the area
		{	
			m_ptDrawPos.y = m_rcRegion.top;
			// m_pDFont->SetText("The text height is greater than the STRING control.");
			// return;
		}
		else if (size.cx <= iRegionWidth)	// If the width of the text is less than the length of the area
		{
			szNewBuff = m_szString;

			// horizontal alignment
			if (m_dwStyle & UISTYLE_STRING_ALIGNRIGHT) m_ptDrawPos.x = m_rcRegion.right - size.cx;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNCENTER) m_ptDrawPos.x = m_rcRegion.left + ((iRegionWidth-size.cx)/2);
			else m_ptDrawPos.x = m_rcRegion.left;
			// vertical alignment
			if (m_dwStyle & UISTYLE_STRING_ALIGNBOTTOM) m_ptDrawPos.y = m_rcRegion.bottom-size.cy;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER) m_ptDrawPos.y = m_rcRegion.top + ((iRegionHeight-size.cy)/2);
			else m_ptDrawPos.y = m_rcRegion.top;
			m_iLineCount = 1;
		}
		else	// If the text width is greater than the area length, display only the characters that fit within the area.
		{
			// Horizontal alignment is left alignment.
			m_ptDrawPos.x = m_rcRegion.left;
			// vertical alignment
			if (m_dwStyle & UISTYLE_STRING_ALIGNBOTTOM) m_ptDrawPos.y = m_rcRegion.bottom-size.cy;
			else if (m_dwStyle & UISTYLE_STRING_ALIGNVCENTER) m_ptDrawPos.y = m_rcRegion.top + ((iRegionHeight-size.cy)/2);
			else m_ptDrawPos.y = m_rcRegion.top;

			// text cutting code
			int iCount = 0;
			while(iCount<iStrLen)
			{
				if ('\n' == szString[iCount])		// \n
				{
					break;
				}
				else if (0x80 & szString[iCount])	// 2BYTE characters
				{
					const BOOL bFlag = m_pDFont->GetTextExtent(szString.c_str(), iCount+2, &size);
					__ASSERT(bFlag, "cannot get size of dfont");
					if (size.cx>iRegionWidth) break;
					iCount += 2;
				}
				else								// 1 BYTE character
				{
					const BOOL bFlag = m_pDFont->GetTextExtent(szString.c_str(), iCount+1, &size);
					__ASSERT(bFlag, "cannot get size of dfont");
					if (size.cx>iRegionWidth) break;
					++iCount;
				}
			}
			// Copy only iCount to strDisplayString
			szNewBuff = m_szString.substr(0, iCount);	// +1 for putting NULL at the end
			m_iLineCount = 1;
		}
		m_pDFont->SetText(szNewBuff);

	}
	else
	{	// When mutiline, always draw left-aligned.
		m_ptDrawPos.x = m_rcRegion.left;
		m_ptDrawPos.y = m_rcRegion.top;

		m_NewLineIndices.clear();	// clear new line index

		// Text cutting code, texts outside the area are not printed.
		int iCX=0;// , iCY=0;
		int iCount = 0;

		// First of all, check if the first line can fit
		const BOOL bFlag = m_pDFont->GetTextExtent(" ", 2, &size);
		__ASSERT(bFlag, "cannot get size of dfont");
		// iCY += size.cy;
		// if (iCY > iRegionHeight)
		if (size.cy > iRegionHeight)
		{
			// m_pDFont->SetText("The text height is greater than the STRING control.");
			// return;
		}

		m_iLineCount = 1;	// If you come this far, one line will be taken.
		m_NewLineIndices.push_back(0);	// The first start is 0

		while(iCount<iStrLen)
		{
			if ('\n' == szString[iCount])		// \n
			{
				// szNewBuff += '\n';
				// iCY += size.cy;
				// if (iCY > iRegionHeight) break; // If the vertical range is exceeded, no more text is printed.
				++iCount;
				iCX = 0;
				if (iCount<iStrLen-1)
				{
					++m_iLineCount;	// Add one line if not the last character
					m_NewLineIndices.push_back(iCount);
				}
			}
			else
			{
				int iCC=0;
				if (0x80 & szString[iCount])	iCC = 2;	// 2BYTE characters
				else iCC = 1;	// 1 BYTE character

				const BOOL bFlag = m_pDFont->GetTextExtent(&(szString[iCount]), iCC, &size);
				__ASSERT(bFlag, "cannot get size of dfont");
				if ((iCX+size.cx) > iRegionWidth)	// If the width exceeds
				{
					// szNewBuff += '\n'; // down to the next line
					iCX = 0;
					// iCY += size.cy;
					// if (iCY > iRegionHeight) break; // If the vertical range is exceeded, no more text is printed.
					if (iCount<iStrLen-1)
					{
						++m_iLineCount;	// Add one line if not the last character
						m_NewLineIndices.push_back(iCount);
					}
				}
				// letter copy
				// szNewBuff += szString.substr(iCount, iCC);
				
				iCount += iCC;
				iCX += size.cx;
			}
		}
		SetStartLine(0);
	}
	// m_pDFont->SetText(szNewBuff);
}

void CN3UIString::SetStartLine(int iLine)
{
	if ((m_dwStyle & UISTYLE_STRING_SINGLELINE) || iLine >= m_iLineCount) return;
	m_iStartLine = iLine;

	SIZE size = {0,0};
	const BOOL bFlag = m_pDFont->GetTextExtent(" ", 2, &size);
	__ASSERT(bFlag, "cannot get size of dfont");
	if (0 == size.cy) return;

	int iEndLine = m_iStartLine + ((m_rcRegion.bottom - m_rcRegion.top)/size.cy);
	bool bMoreLine = true;
	if (iEndLine >= m_iLineCount)
	{
		iEndLine = m_iLineCount;
		bMoreLine = false;
	}
	int i, iCC, iSize;
	std::string strNew;
	for (i=m_iStartLine; i<iEndLine-1; ++i)
	{
		iCC = m_NewLineIndices[i+1] - m_NewLineIndices[i];
		if (iCC>0)
		{
			strNew += m_szString.substr(m_NewLineIndices[i], iCC);
			iSize = strNew.size();
			if ((iSize>0) && ('\n' != strNew[iSize-1])) strNew += "\n";
		}
	}
	// last line processing
	if (bMoreLine)
	{
		iCC = m_NewLineIndices[iEndLine] - m_NewLineIndices[iEndLine-1];
		if (iCC>0) strNew += m_szString.substr(m_NewLineIndices[i], iCC);
	}
	else
	{
		iCC = m_szString.size() - m_NewLineIndices[iEndLine-1];
		if (iCC>0) strNew += m_szString.substr(m_NewLineIndices[i], iCC);
	}
	m_pDFont->SetText(strNew);
}

bool CN3UIString::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	DWORD dwNum;
	// font information
	int iStrLen = 0;
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, nullptr);			// font name length
	if (iStrLen>0)
	{
		std::string szFontName(iStrLen, '?');
		ReadFile(hFile, &(szFontName[0]), iStrLen, &dwNum, nullptr);				// string

		DWORD dwFontFlags = 0, dwFontHeight = 0;
		ReadFile(hFile, &dwFontHeight, sizeof(dwFontHeight), &dwNum, nullptr);	// font height
		ReadFile(hFile, &dwFontFlags, sizeof(dwFontFlags), &dwNum, nullptr);	// font flag (bold, italic)

		SetFont(szFontName, dwFontHeight, dwFontFlags & D3DFONT_BOLD, dwFontFlags & D3DFONT_ITALIC);	// font designation
	}
#ifdef _N3TOOL
	else
	{
		SetFont("Arial", 10, FALSE, FALSE);	// Temporarily set font
		MessageBox(GetActiveWindow(), "I set it to Arial(10) because there was a UIString with no font specified..", "No font", MB_OK);
	}
#else
	__ASSERT(iStrLen>0, "No font name");
#endif

	// string
	ReadFile(hFile, &m_Color, sizeof(m_Color), &dwNum, nullptr);			// text color
	ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, nullptr);			// string length
	if (iStrLen>0)
	{
		std::string szString(iStrLen, '?');
		ReadFile(hFile, &(szString[0]), iStrLen, &dwNum, nullptr);				// string
		SetString(szString);
	}
	ReadFile(hFile, &m_iPadding, sizeof(m_iPadding), &dwNum, nullptr);
	return true;
}

void CN3UIString::operator = (const CN3UIString& other)
{
	CN3UIBase::operator = (other);

	m_ptDrawPos = other.m_ptDrawPos;		// The coordinates of the top left of the text to be displayed on the actual screen
	m_Color = other.m_Color;			// text color

	// font settings
	const DWORD dwFontFlags = other.GetFontFlags();
	SetFont(other.GetFontName(), other.GetFontHeight(), dwFontFlags & D3DFONT_BOLD, dwFontFlags & D3DFONT_ITALIC);

	// font settings
	this->SetString(other.m_szString); // m_szString = other.m_szString;			// string buffer
}

#ifdef _N3TOOL
bool CN3UIString::Save(HANDLE hFile)
{
	if (false == CN3UIBase::Save(hFile)) return false;
	DWORD dwNum;
	// font information
	char* pszFontName = NULL;
	__ASSERT(m_pDFont, "no font");
	const std::string strFontName(m_pDFont->GetFontName());
	int iStrLen = strFontName.size();
	__ASSERT(iStrLen>0, "No font name");
	WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			// font name length
	if (iStrLen>0)
	{
		WriteFile(hFile, strFontName.c_str(), iStrLen, &dwNum, NULL);				// string
		DWORD dwFontFlags = 0, dwFontHeight = 0;
		if (m_pDFont)
		{
			dwFontHeight = m_pDFont->GetFontHeight();
			dwFontFlags = m_pDFont->GetFontFlags();
		}
		WriteFile(hFile, &dwFontHeight, sizeof(dwFontHeight), &dwNum, NULL);	// font height
		WriteFile(hFile, &dwFontFlags, sizeof(dwFontFlags), &dwNum, NULL);	// font flag (bold, italic)
	}

	// string
	WriteFile(hFile, &m_Color, sizeof(m_Color), &dwNum, NULL);			// text color
	iStrLen = 0;
	iStrLen = m_szString.size();
	WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);			// string length
	if (iStrLen>0)
	{
		WriteFile(hFile, m_szString.c_str(), iStrLen, &dwNum, NULL);				// string
	}
	return true;
}

void CN3UIString::ChangeFont(const std::string& szFont)
{
	if(m_pDFont)
	{
		DWORD dwFlag = m_pDFont->GetFontFlags();
		bool bBold = (dwFlag & D3DFONT_BOLD) ? true : false;
		bool bItalic = (dwFlag & D3DFONT_ITALIC) ? true : false;
		this->SetFont(szFont, m_pDFont->GetFontHeight(), bBold, bItalic);
	}

	CN3UIBase::ChangeFont(szFont);
}
#endif

int CN3UIString::GetStringRealWidth(int iNum) const
{
	SIZE size;
	const BOOL bFlag = m_pDFont->GetTextExtent(" ", lstrlen(" "), &size);
	__ASSERT(bFlag, "cannot get size of dfont");
	const int iLength = iNum/2;
	if (iLength == 0) return 0;
	return (size.cx*iLength); 
}

void CN3UIString::SetStyle(DWORD dwType, DWORD dwStyleEx)
{
	if (dwType == UI_STR_TYPE_LINE)
	{
		DWORD dwStyle = m_dwStyle;
		if (UISTYLE_STRING_SINGLELINE == dwStyleEx) dwStyle |= UISTYLE_STRING_SINGLELINE;
		else if (UISTYLE_STRING_MULTILINE == dwStyleEx) dwStyle &= (~UISTYLE_STRING_SINGLELINE);
		SetStyle(dwStyle);
	}
	else if (dwType == UI_STR_TYPE_HALIGN)
	{
		DWORD dwStyle = m_dwStyle;
		dwStyle &= (~UISTYLE_STRING_ALIGNLEFT);
		dwStyle &= (~UISTYLE_STRING_ALIGNCENTER);
		dwStyle &= (~UISTYLE_STRING_ALIGNRIGHT);
		if (UISTYLE_STRING_ALIGNLEFT == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNLEFT;
		else if (UISTYLE_STRING_ALIGNCENTER == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNCENTER;
		else if (UISTYLE_STRING_ALIGNRIGHT == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNRIGHT;
		SetStyle(dwStyle);
	}
	else if (dwType == UI_STR_TYPE_VALIGN)
	{
		DWORD dwStyle = m_dwStyle;
		dwStyle &= (~UISTYLE_STRING_ALIGNTOP);
		dwStyle &= (~UISTYLE_STRING_ALIGNVCENTER);
		dwStyle &= (~UISTYLE_STRING_ALIGNBOTTOM);
		if (UISTYLE_STRING_ALIGNTOP == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNTOP;
		else if (UISTYLE_STRING_ALIGNVCENTER == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNVCENTER;
		else if (UISTYLE_STRING_ALIGNBOTTOM == dwStyleEx) dwStyle |= UISTYLE_STRING_ALIGNBOTTOM;
		SetStyle(dwStyle);
	}
}

DWORD CN3UIString::MouseProc(DWORD dwFlags, const POINT &ptCur, const POINT &ptOld)
{
	const DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	// TODO: Enable this code
	/*
	#ifdef _N3GAME
	if ( CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer ) return dwRet;
	#endif
	*/

	// Send messages for specific events..
	if(IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBCLICKED) )	
	{
		m_pParent->ReceiveMessage(this, UIMSG_STRING_LCLICK); // Notify parent on button click..
		// dwRet |= UI_MOUSEPROC_DONESOMETHING;
	}

	if(IsIn(ptCur.x, ptCur.y) && (dwFlags & UI_MOUSE_LBDBLCLK) )	
	{
		m_pParent->ReceiveMessage(this, UIMSG_STRING_LDCLICK); // Notify parent on button click..
		// dwRet |= UI_MOUSEPROC_DONESOMETHING;
	}

	// dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}
