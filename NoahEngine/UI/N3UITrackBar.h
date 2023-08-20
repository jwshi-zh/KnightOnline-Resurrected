#pragma once

#include "N3UIBase.h"

class CN3UITrackBar : public CN3UIBase  
{
public:
	CN3UITrackBar();
	virtual ~CN3UITrackBar();

// Attributes
public:
	enum eIMAGE_TYPE{IMAGETYPE_BKGND=0, IMAGETYPE_THUMB, NUM_IMAGETYPE};
protected:
	CN3UIImage*		m_pBkGndImageRef;		// Background image referance (memory allocation managed by children list)
	CN3UIImage*		m_pThumbImageRef;		// Image referance that can be moved by dragging in the middle

	int				m_iMaxPos;									// maximum
	int				m_iMinPos;									// Ieast
	int				m_iCurPos;									// current value
	int				m_iPageSize;								// Movement value when moving by page unit
// Operations
public:
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual void	SetRegion(const RECT& Rect);
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);

	void			SetRange(int iMin, int iMax);
	void			SetRangeMax(int iMax) {SetRange(m_iMinPos, iMax);}
	void			SetRangeMin(int iMin) {SetRange(iMin, m_iMaxPos);}
	void			SetCurrentPos(int iPos);
	int				GetPos() const {return m_iCurPos;}
	void			SetPageSize(int iSize) {m_iPageSize = iSize;}
	int				GetPageSize() const {return m_iPageSize;}
	int				GetMaxPos() const {return m_iMaxPos;}
	int				GetMinPos() const {return m_iMinPos;}
protected:
	void			UpdateThumbPos() const;							// Calculate m_iCurPos to recompute and replace Thumb position
	void			UpDownThumbPos(int iDiff);					// Update m_iCurPos after moving the Thumb position up and down by iDiff pixels

#ifdef _N3TOOL
// Functions used by tools
public:
	virtual void	operator = (const CN3UITrackBar& other);
	void			CreateImages();			// create image
	void			DeleteBkImage();		// Delete background image
	CN3UIImage*		GetBkGndImgRef() const {return m_pBkGndImageRef;}
	CN3UIImage*		GetThumbImgRef() const {return m_pThumbImageRef;}
#endif
};
