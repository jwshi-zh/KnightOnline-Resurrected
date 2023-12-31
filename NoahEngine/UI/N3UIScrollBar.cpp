#include "pch.h"
#include "N3UIScrollBar.h"
#include "N3UIButton.h"

CN3UIScrollBar::CN3UIScrollBar()
{
	m_eType = UI_TYPE_SCROLLBAR;
	m_pTrackBarRef = nullptr;
	ZeroMemory(m_pBtnRef, sizeof(CN3UIButton*)*NUM_BTN_TYPE);
	m_iLineSize = 1;
}

CN3UIScrollBar::~CN3UIScrollBar()
{
}

void CN3UIScrollBar::Release()
{
	CN3UIBase::Release();
	m_pTrackBarRef = nullptr;
	ZeroMemory(m_pBtnRef, sizeof(CN3UIButton*)*NUM_BTN_TYPE);
	m_iLineSize = 1;
}

bool CN3UIScrollBar::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;
	__ASSERT(NULL == m_pTrackBarRef, "scrollbar is not initialized");

	// Setting m_pTrackBarRef, m_pBtnRef
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_TRACKBAR == pChild->UIType())
		{
			m_pTrackBarRef = (CN3UITrackBar*)pChild;
		}
		else if (UI_TYPE_BUTTON == pChild->UIType())
		{
			const int iBtnType = pChild->GetReserved();
			if (iBtnType<0 || iBtnType >= NUM_BTN_TYPE) continue;
			m_pBtnRef[iBtnType] = (CN3UIButton*)pChild;
		}
	}
	return true;
}

void CN3UIScrollBar::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	// First of all, temporarily arrange the same as the size of the scroll area.
	// for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	// {
	// (*itor)->SetRegion(Rect);
	// }
}

bool CN3UIScrollBar::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg)
{
	if (UIMSG_TRACKBAR_POS == dwMsg)
	{
		if (m_pParent) return m_pParent->ReceiveMessage(this, UIMSG_SCROLLBAR_POS);
	}
	else if (UIMSG_BUTTON_CLICK == dwMsg)
	{
		if (m_pTrackBarRef)
		{
			if (BTN_LEFTUP == pSender->GetReserved()) m_pTrackBarRef->SetCurrentPos(m_pTrackBarRef->GetPos()-m_iLineSize);
			else if (BTN_RIGHTDOWN == pSender->GetReserved()) m_pTrackBarRef->SetCurrentPos(m_pTrackBarRef->GetPos()+m_iLineSize);

			if (m_pParent) return m_pParent->ReceiveMessage(this, UIMSG_SCROLLBAR_POS);
		}
	}
	return true;
}

void CN3UIScrollBar::SetStyle(DWORD dwStyle)
{
	CN3UIBase::SetStyle(dwStyle);
	if (UISTYLE_SCROLLBAR_HORIZONTAL == dwStyle)
	{
		if (m_pTrackBarRef) m_pTrackBarRef->SetStyle(UISTYLE_TRACKBAR_HORIZONTAL);
	}
	else
	{
		if (m_pTrackBarRef) m_pTrackBarRef->SetStyle(UISTYLE_TRACKBAR_VERTICAL);
	}
}

#ifdef _N3TOOL
void CN3UIScrollBar::operator = (const CN3UIScrollBar& other)
{
	CN3UIBase::operator = (other);
	m_iLineSize = other.m_iLineSize;		// How much the trackbar moves when a button is pressed

	// Setting m_pTrackBarRef, m_pBtnRef
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_TRACKBAR == pChild->UIType())
		{
			m_pTrackBarRef = (CN3UITrackBar*)pChild;
		}
		else if (UI_TYPE_BUTTON == pChild->UIType())
		{
			int iBtnType = pChild->GetReserved();
			if (iBtnType<0 || iBtnType >= NUM_BTN_TYPE) continue;
			m_pBtnRef[iBtnType] = (CN3UIButton*)pChild;
		}
	}
}

void CN3UIScrollBar::CreateTrackBarAndBtns()
{
	__ASSERT(NULL == m_pTrackBarRef, "component is already assigned");
	int i;
	for (i=0; i<NUM_BTN_TYPE; ++i)
	{
		m_pBtnRef[i] = new CN3UIButton();
		m_pBtnRef[i]->Init(this);
		m_pBtnRef[i]->SetReserved(i);		// State number (eBTN_TYPE) assignment.
		m_pBtnRef[i]->CreateImages();
	}
	
	m_pTrackBarRef = new CN3UITrackBar();
	m_pTrackBarRef->Init(this);
	m_pTrackBarRef->CreateImages();			// create image of trackbar
}
#endif