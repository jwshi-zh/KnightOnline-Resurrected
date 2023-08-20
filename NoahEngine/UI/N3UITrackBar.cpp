#include "pch.h"
#include "N3UITrackBar.h"
#include "N3UIImage.h"

CN3UITrackBar::CN3UITrackBar()
{
	m_eType = UI_TYPE_TRACKBAR;

	m_pBkGndImageRef = nullptr;
	m_pThumbImageRef = nullptr;
	m_iMaxPos = 10;
	m_iMinPos = 0;
	m_iCurPos = 0;
	m_iPageSize = 2;
}

CN3UITrackBar::~CN3UITrackBar()
{
}

void CN3UITrackBar::Release()
{
	CN3UIBase::Release();
	m_pBkGndImageRef = nullptr;
	m_pThumbImageRef = nullptr;
	m_iMaxPos = 10;
	m_iMinPos = 0;
	m_iCurPos = 0;
	m_iPageSize = 2;
}

bool CN3UITrackBar::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	// Setting ImageRef
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// Pick out only images
		const int iImageType = (int)(pChild->GetReserved());
		if (IMAGETYPE_BKGND == iImageType)
		{
			m_pBkGndImageRef = (CN3UIImage*)pChild;
		}
		else if (IMAGETYPE_THUMB == iImageType)
		{
			m_pThumbImageRef = (CN3UIImage*)pChild;
		}
	}
	return true;
}

void CN3UITrackBar::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	if(m_pBkGndImageRef) m_pBkGndImageRef->SetRegion(m_rcRegion);	// The background image is in the same area
	const RECT rcThumb = m_pThumbImageRef->GetRegion();

	const int iThumbWidth = rcThumb.right - rcThumb.left;
	const int iThumbHeight = rcThumb.bottom - rcThumb.top;
	const int iBkWidth = Rect.right - Rect.left;
	const int iBkHeight = Rect.bottom - Rect.top;
	if (iBkWidth<=0 || iBkHeight<=0) return;
	
	if ( iThumbWidth<=0 && iThumbHeight<=0 )
	{	// If the thumb image is not set, set it appropriately
		RECT rc;
		if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
		{	// length
			rc.left = Rect.left;		rc.top = Rect.top + iBkHeight*0.3f;
			rc.right = Rect.right;	rc.bottom = rc.top + iBkHeight*0.3f;
		}
		else
		{	// width
			rc.left = Rect.left + iBkWidth*0.3f;		rc.top = Rect.top;
			rc.right = rc.left + iBkWidth*0.3f;			rc.bottom = Rect.bottom;
		}
		m_pThumbImageRef->SetRegion(rc);
	}
	UpdateThumbPos();	
}

DWORD CN3UITrackBar::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;
	if (false == IsIn(ptCur.x, ptCur.y))	// out of range
	{
		SetState(UI_STATE_COMMON_NONE);
		return dwRet;
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// This coordinate is inside the area.

	if (UI_STATE_TRACKBAR_THUMBDRAG == m_eState)
	{
		if(dwFlags & UI_MOUSE_LBCLICKED)  // When the left button is released
		{
			SetState(UI_STATE_COMMON_NONE);		// release drag
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if (dwFlags & UI_MOUSE_LBDOWN)
		{
			// move the thumb
			if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)	UpDownThumbPos(ptCur.y - ptOld.y);
			else UpDownThumbPos(ptCur.x - ptOld.x);
			// send a message to parents
			if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_TRACKBAR_POS);
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
	}
	else
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  // When the left button is pressed
		{
			if (m_pThumbImageRef->IsIn(ptCur.x, ptCur.y))	// thumb pressed
			{
				SetState(UI_STATE_TRACKBAR_THUMBDRAG);
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else	// The upper or lower part of the thumb (left or right margin) is pressed.
			{
				const RECT rcThumb = m_pThumbImageRef->GetRegion();
				if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
				{
					if (ptCur.y <= rcThumb.top) SetCurrentPos(m_iCurPos-m_iPageSize);// click on top
					else SetCurrentPos(m_iCurPos+m_iPageSize);// click below
				}
				else
				{
					if (ptCur.x <= rcThumb.left) SetCurrentPos(m_iCurPos-m_iPageSize);// left click
					else SetCurrentPos(m_iCurPos+m_iPageSize);// right click
				}
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_TRACKBAR_POS);
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

void CN3UITrackBar::SetRange(int iMin, int iMax)
{
	if (m_iMaxPos == iMax && m_iMinPos == iMin) return;
	m_iMaxPos = iMax;		m_iMinPos = iMin;
	if (m_iCurPos>m_iMaxPos)	m_iCurPos = m_iMaxPos;
	if (m_iCurPos<m_iMinPos)	m_iCurPos = m_iMinPos;
	UpdateThumbPos();
}

void CN3UITrackBar::SetCurrentPos(int iPos)
{
	if (iPos == m_iCurPos) return;
	m_iCurPos = iPos;
	if (m_iCurPos>m_iMaxPos)	m_iCurPos = m_iMaxPos;
	if (m_iCurPos<m_iMinPos)	m_iCurPos = m_iMinPos;
	UpdateThumbPos();
}

// Adjust Thumb position with Pos value
void CN3UITrackBar::UpdateThumbPos() const
{
	if (nullptr == m_pThumbImageRef) return;
	const float fDiff = m_iMaxPos - m_iMinPos;
	if (0.0f == fDiff) return;
	const float fPercentage = (float)m_iCurPos/fDiff;
	const RECT rcThumb = m_pThumbImageRef->GetRegion();

	if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)
	{
		const int iDY = fPercentage*((m_rcRegion.bottom - m_rcRegion.top) - (rcThumb.bottom - rcThumb.top));
		m_pThumbImageRef->SetPos(	m_rcRegion.left, m_rcRegion.top + iDY);
	}
	else
	{
		const int iDX = fPercentage*((m_rcRegion.right - m_rcRegion.left) - (rcThumb.right - rcThumb.left));
		m_pThumbImageRef->SetPos( m_rcRegion.left + iDX, m_rcRegion.top );
	}
}

// Adjust the position of the thumb in pixel units and calculate and insert the pos value based on the position of the thumb
void CN3UITrackBar::UpDownThumbPos(int iDiff)
{
	if (nullptr == m_pThumbImageRef) return;
	const RECT rcThumb = m_pThumbImageRef->GetRegion();

	if (UISTYLE_TRACKBAR_VERTICAL == m_dwStyle)		// when moving down
	{
		const int iRegionHeight = m_rcRegion.bottom - m_rcRegion.top;
		const int iThumbHeight = rcThumb.bottom - rcThumb.top;
		if (0==iRegionHeight || 0==iThumbHeight) return;

		// Finding the percentage of the position of the thumb after moving
		const float fPercentage = ((rcThumb.top-m_rcRegion.top)+iDiff) / (((float)(iRegionHeight)) - iThumbHeight);

		if (fPercentage>1.0f)	// Dropped too low.
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.bottom-iThumbHeight);
			m_iCurPos = m_iMaxPos;		// When calling the SetCurrentPos function, the thumb position is recalculated, so it is changed directly.
		}
		else if (fPercentage<0.0f)	// Raised too high.
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top);
			m_iCurPos = m_iMinPos;// When calling the SetCurrentPos function, the thumb position is recalculated, so it is changed directly.
		}
		else
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top+iDiff);
			m_iCurPos = m_iMinPos + (m_iMaxPos-m_iMinPos)*fPercentage;// When calling the SetCurrentPos function, the thumb position is recalculated, so it is changed directly.
		}
	}
	else											// when moving left and right
	{
		const int iRegionWidth = m_rcRegion.right - m_rcRegion.left;
		const int iThumbWidth = rcThumb.right - rcThumb.left;
		if (0==iRegionWidth || 0==iThumbWidth) return;

		// Finding the percentage of the position of the thumb after moving
		const float fPercentage = ((rcThumb.left-m_rcRegion.left)+iDiff) / (((float)(iRegionWidth)) - iThumbWidth);

		if (fPercentage>1.0f)	// pushed too far to the right.
		{
			m_pThumbImageRef->SetPos(rcThumb.right-iThumbWidth, rcThumb.top);
			m_iCurPos = m_iMaxPos;// When calling the SetCurrentPos function, the thumb position is recalculated, so it is changed directly.
		}
		else if (fPercentage<0.0f)	// pushed too far left
		{
			m_pThumbImageRef->SetPos(rcThumb.left, rcThumb.top);
			m_iCurPos = m_iMinPos;// When calling the SetCurrentPos function, the thumb position is recalculated, so it is changed directly.
		}
		else
		{
			m_pThumbImageRef->SetPos(rcThumb.left+iDiff, rcThumb.top);
			m_iCurPos = m_iMinPos + (m_iMaxPos-m_iMinPos)*fPercentage;// When calling the SetCurrentPos function, the thumb position is recalculated, so it is changed directly.
		}
	}
}

#ifdef _N3TOOL
void CN3UITrackBar::operator = (const CN3UITrackBar& other)
{
	CN3UIBase::operator = (other);

	m_iMaxPos = other.m_iMaxPos;									// maximum
	m_iMinPos = other.m_iMinPos;									// Ieast
	m_iCurPos = other.m_iCurPos;									// current value
	m_iPageSize = other.m_iPageSize;								// Movement value when moving by page unit

	// Setting ImageRef
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// Pick out only images
		int iImageType = (int)(pChild->GetReserved());
		if (IMAGETYPE_BKGND == iImageType)
		{
			m_pBkGndImageRef = (CN3UIImage*)pChild;
		}
		else if (IMAGETYPE_THUMB == iImageType)
		{
			m_pThumbImageRef = (CN3UIImage*)pChild;
		}
	}
}

void CN3UITrackBar::CreateImages()
{
	__ASSERT(NULL == m_pBkGndImageRef && NULL == m_pThumbImageRef, "이미지가 이미 할당되어 있어여");
	m_pBkGndImageRef = new CN3UIImage();
	m_pBkGndImageRef->Init(this);
	m_pBkGndImageRef->SetReserved(IMAGETYPE_BKGND);

	m_pThumbImageRef = new CN3UIImage();
	m_pThumbImageRef->Init(this);
	m_pThumbImageRef->SetReserved(IMAGETYPE_THUMB);

	SetRegion(m_rcRegion);	// realm reset
}

void CN3UITrackBar::DeleteBkImage()
{
	delete m_pBkGndImageRef;
	m_pBkGndImageRef = NULL;
}

#endif