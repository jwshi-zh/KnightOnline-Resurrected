#pragma once

#include "N3UIBase.h"
#include "N3UITrackBar.h"

class CN3UIScrollBar : public CN3UIBase 
{
public:
	CN3UIScrollBar();
	virtual ~CN3UIScrollBar();
// Attributes
public:
	enum eBTN_TYPE	{BTN_LEFTUP=0, BTN_RIGHTDOWN, NUM_BTN_TYPE};
protected:
	CN3UITrackBar*	m_pTrackBarRef;
	CN3UIButton*	m_pBtnRef[NUM_BTN_TYPE];

	int				m_iLineSize;		// How much the trackbar moves when a button is pressed

// Operations
public:
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(DWORD dwStyle);
	virtual bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg); // Receives a message. Sender, msg

	void			SetRange(int iMin, int iMax) const {if(m_pTrackBarRef) m_pTrackBarRef->SetRange(iMin,iMax);}
	void			SetRangeMax(int iMax) {if(m_pTrackBarRef) m_pTrackBarRef->SetRangeMax(iMax);}
	void			SetRangeMin(int iMin) {if(m_pTrackBarRef) m_pTrackBarRef->SetRangeMin(iMin);}
	int				GetCurrentPos() const {if(m_pTrackBarRef) return m_pTrackBarRef->GetPos(); return 0;}
	void			SetCurrentPos(int iPos) {if(m_pTrackBarRef) m_pTrackBarRef->SetCurrentPos(iPos);}
	void			SetPageSize(int iSize) {if(m_pTrackBarRef) m_pTrackBarRef->SetPageSize(iSize);}
	int				GetPageSize() const {if(m_pTrackBarRef) return m_pTrackBarRef->GetPageSize(); return 0;}
	void			SetLineSize(int iSize) {m_iLineSize = iSize;}
	int				GetLineSize() const {return m_iLineSize;}
	int				GetMaxPos() const {if (m_pTrackBarRef) return m_pTrackBarRef->GetMaxPos(); return 0;}

#ifdef _N3TOOL
// Functions used by the tool
public:
	virtual void	operator = (const CN3UIScrollBar& other);
	void			CreateTrackBarAndBtns();			// Creating trackbars and buttons
	CN3UITrackBar*	GetTrackBarRef() const {return m_pTrackBarRef;}
	CN3UIButton*	GetBtnRef(eBTN_TYPE eBtnType) const {return m_pBtnRef[eBtnType];}
#endif

};
