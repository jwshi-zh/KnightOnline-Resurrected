#include "pch.h"
#include "N3UIButton.h"
#include "N3UIImage.h"

#include "N3SndMgr.h"
#include "N3SndObj.h"

CN3UIButton::CN3UIButton()
{
	m_eType = UI_TYPE_BUTTON;

	m_dwStyle = UISTYLE_BTN_NORMAL;
	m_eState = UI_STATE_BUTTON_NORMAL;
	ZeroMemory(m_ImageRef, sizeof(CN3UIImage*)*NUM_BTN_STATE);
	ZeroMemory(&m_rcClick, sizeof(m_rcClick));
	m_pSnd_On = nullptr;
	m_pSnd_Click = nullptr;
}

CN3UIButton::~CN3UIButton()
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_On);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

void CN3UIButton::Release()
{
	CN3UIBase::Release();

	m_dwStyle = UISTYLE_BTN_NORMAL;
	m_eState = UI_STATE_BUTTON_NORMAL;
	ZeroMemory(m_ImageRef, sizeof(CN3UIImage*)*NUM_BTN_STATE);
	ZeroMemory(&m_rcClick, sizeof(m_rcClick));

	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_On);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

void CN3UIButton::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	SetClickRect(Rect);
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
}

BOOL CN3UIButton::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (FALSE == CN3UIBase::MoveOffset(iOffsetX, iOffsetY)) return FALSE;
	// click area
	m_rcClick.left += iOffsetX;		m_rcClick.top += iOffsetY;
	m_rcClick.right += iOffsetX;	m_rcClick.bottom += iOffsetY;	
	return TRUE;
}

void CN3UIButton::Render()
{
 	if(!m_bVisible) return;

	switch(m_eState)
	{
	case UI_STATE_BUTTON_NORMAL:
		{
			if (m_ImageRef[BS_NORMAL]) m_ImageRef[BS_NORMAL]->Render();
		}
		break;
	case UI_STATE_BUTTON_DOWN:
	case UI_STATE_BUTTON_DOWN_2CHECKDOWN:
	case UI_STATE_BUTTON_DOWN_2CHECKUP:
		{
			if (m_ImageRef[BS_DOWN]) m_ImageRef[BS_DOWN]->Render();
		}
		break;
	case UI_STATE_BUTTON_ON:
		{
			if (m_ImageRef[BS_ON]) m_ImageRef[BS_ON]->Render();
		}
		break;
	case UI_STATE_BUTTON_DISABLE:
		{
			if (m_ImageRef[BS_DISABLE]) m_ImageRef[BS_DISABLE]->Render();
		}
	}

    const int i = 0;
	for(auto itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		for(auto i = 0; i < NUM_BTN_STATE; i++) // See if it&#39;s not a component of a button..
			if(pChild == m_ImageRef[i]) break;
		if(i >= NUM_BTN_STATE) pChild->Render(); // Render if not a button child..
	}
}

DWORD CN3UIButton::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

#ifdef _N3GAME
	// TODO: Enable this later on
	// if ( CN3UIWndBase::m_sRecoveryJobInfo.m_bWaitFromServer ) return dwRet;
#endif

	if(false == IsIn(ptCur.x, ptCur.y))	// out of range
	{
		if (false == IsIn(ptOld.x, ptOld.y))	return dwRet; // If the previous pointer was also out of range, just return it.
		dwRet |= UI_MOUSEPROC_PREVINREGION;	// The previous mouse coordinates were inside the region.

		if (UI_STATE_BUTTON_DISABLE == m_eState) return dwRet;	// If disabled, just return

		if(UISTYLE_BTN_NORMAL & m_dwStyle) // If it is a normal button
		{
			SetState(UI_STATE_BUTTON_NORMAL);	// in normal state
		}
		else if (UISTYLE_BTN_CHECK & m_dwStyle) // If the check button
		{
			if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState)	// If you are trying to up
				SetState(UI_STATE_BUTTON_DOWN);	// down
			else if (UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState ||	// If you are trying to down or
				UI_STATE_BUTTON_ON == m_eState)	// When on
				SetState(UI_STATE_BUTTON_NORMAL);	// in normal state
		}
		return dwRet; // Since it is out of scope, it is not further processed.
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// This time the mouse coordinates are inside the area

	if (UI_STATE_BUTTON_DISABLE == m_eState) return dwRet;	// If disabled, just return

	// outside the click area
	if (FALSE == PtInRect(&m_rcClick, ptCur))
	{
		if(UISTYLE_BTN_NORMAL & m_dwStyle) // If it is a normal button
		{
			SetState(UI_STATE_BUTTON_NORMAL);	// in normal state
		}
		else if (UISTYLE_BTN_CHECK & m_dwStyle) // If the check button
		{
			if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState)	// If you are trying to up
				SetState(UI_STATE_BUTTON_DOWN);	// down
			else if (UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState ||	// If you are trying to down or
				UI_STATE_BUTTON_ON == m_eState)	// When on
				SetState(UI_STATE_BUTTON_NORMAL);	// in normal state
		}
		return dwRet;
	}

	// Below is when it is not in the click area.
	// If it is a normal button
	if(UISTYLE_BTN_NORMAL & m_dwStyle)
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  // When the left button is pressed
		{
			SetState(UI_STATE_BUTTON_DOWN); // Make it pressed...
			if (m_pSnd_Click) m_pSnd_Click->Play();	// Play if there is sound
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if(dwFlags & UI_MOUSE_LBCLICKED) // When the left button is released
		{
			if(m_pParent && UI_STATE_BUTTON_DOWN == m_eState) // If the previous state is the Down state of the button
			{
				SetState(UI_STATE_BUTTON_ON); // Make the button On.
				m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // Notify parent on button click..
			}
			dwRet |= UI_MOUSEPROC_DONESOMETHING;
			return dwRet;
		}
		else if (UI_STATE_BUTTON_NORMAL == m_eState)	// In the normal state, it is in the on state.
		{
			SetState(UI_STATE_BUTTON_ON); // Make it on...
			if (m_pSnd_On) m_pSnd_On->Play();	// Play if there is sound
			dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
			return dwRet;
			// Do not put UI_MOUSEPROC_DONESOMETHING (when the mouse pointer quickly moves from one button to another)
			// to prevent the state of the previous button from getting weird)
		}
	}

	// If the check button
	else if(UISTYLE_BTN_CHECK & m_dwStyle) 
	{
		if(dwFlags & UI_MOUSE_LBCLICK)  // When the left button is pressed
		{
			if (UI_STATE_BUTTON_NORMAL == m_eState || UI_STATE_BUTTON_ON == m_eState)
			{
				SetState(UI_STATE_BUTTON_DOWN_2CHECKDOWN); // Make it temporarily pressed (DOWN_2CHECKDOWN).
				if (m_pSnd_Click) m_pSnd_Click->Play();	// Play if there is sound
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else if(UI_STATE_BUTTON_DOWN == m_eState)
			{
				SetState(UI_STATE_BUTTON_DOWN_2CHECKUP); // Make it temporarily pressed (DOWN_2CHECKUP).
				if (m_pSnd_Click) m_pSnd_Click->Play();	// Play if there is sound
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
		else if(dwFlags & UI_MOUSE_LBCLICKED)  // When the left button is released
		{
			if(UI_STATE_BUTTON_DOWN_2CHECKDOWN == m_eState) // If the previous state is 2CHECKDOWN
			{
				SetState(UI_STATE_BUTTON_DOWN); // put down
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // Notify parent on button click..
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
			else if (UI_STATE_BUTTON_DOWN_2CHECKUP == m_eState) // If the previous state is 2CHECKUP state
			{
				SetState(UI_STATE_BUTTON_ON); // make it on
				if (m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // Notify parent on button click..
				dwRet |= UI_MOUSEPROC_DONESOMETHING;
				return dwRet;
			}
		}
		else if (UI_STATE_BUTTON_NORMAL == m_eState)	// In the normal state, it is in the on state.
		{
			SetState(UI_STATE_BUTTON_ON); // Make it on...
			if (m_pSnd_On) m_pSnd_On->Play();	// Play if there is sound
			dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
			return dwRet;
			// Do not put UI_MOUSEPROC_DONESOMETHING (when the mouse pointer quickly moves from one button to another)
			// to prevent the state of the previous button from getting weird)
		}

	}
	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

bool CN3UIButton::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	DWORD dwNum;
	ReadFile(hFile, &m_rcClick, sizeof(m_rcClick), &dwNum, nullptr);	// click area

	// Setting m_ImageRef
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// Pick out only images
		const int iBtnState = (int)(pChild->GetReserved());
		if (iBtnState<NUM_BTN_STATE)
		{
			m_ImageRef[iBtnState] = (CN3UIImage*)pChild;
		}
	}

	// To convert the old uif file, block the sound loading part
	int iSndFNLen = 0;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, nullptr);		// sound file string length
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, buffer.data(), iSndFNLen, &dwNum, nullptr);

		__ASSERT(NULL == m_pSnd_On, "memory leak");
		m_pSnd_On = s_SndMgr.CreateObj(buffer.data(), SNDTYPE_2D);
	}

	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, nullptr);		// sound file string length
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, buffer.data(), iSndFNLen, &dwNum, nullptr);

		__ASSERT(NULL == m_pSnd_Click, "memory leak");
		m_pSnd_Click = s_SndMgr.CreateObj(buffer.data(), SNDTYPE_2D);
	}

	return true;
}

#ifdef _N3TOOL
void CN3UIButton::operator = (const CN3UIButton& other)
{
	CN3UIBase::operator = (other);

	m_rcClick = other.m_rcClick;			// click area
	SetSndOn(other.GetSndFName_On());		// sound
	SetSndClick(other.GetSndFName_Click());	// sound

	// Setting m_ImageRef
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// Pick out only images
		int iBtnState = (int)(pChild->GetReserved());
		if (iBtnState<NUM_BTN_STATE)
		{
			m_ImageRef[iBtnState] = (CN3UIImage*)pChild;
		}
	}
}

bool CN3UIButton::Save(HANDLE hFile)
{
	if (false == CN3UIBase::Save(hFile)) return false;
	DWORD dwNum;
	WriteFile(hFile, &m_rcClick, sizeof(m_rcClick), &dwNum, NULL);	// click area

	int iSndFNLen = 0;
	if (m_pSnd_On) iSndFNLen = m_pSnd_On->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		// sound file string length
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_On->m_szFileName.c_str(), iSndFNLen, &dwNum, NULL);

	iSndFNLen = 0;
	if (m_pSnd_Click) iSndFNLen = m_pSnd_Click->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		// sound file string length
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_Click->m_szFileName.c_str(), iSndFNLen, &dwNum, NULL);

	return true;
}

// Functions for use in tools: Create n3uiImage.
void CN3UIButton::CreateImages()
{
	int i;
	for (i=0; i<NUM_BTN_STATE; ++i)
	{
		__ASSERT(NULL == m_ImageRef[i],"이미지가 이미 할당되어 있어여");
		m_ImageRef[i] = new CN3UIImage();
		m_ImageRef[i]->Init(this);
		m_ImageRef[i]->SetRegion(m_rcRegion);

		m_ImageRef[i]->SetReserved(i);		// State number (eBTN_STATE) assignment.
	}
}

void CN3UIButton::SetSndOn(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_On);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Passes a relative path to the base path.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_On = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

void CN3UIButton::SetSndClick(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Passes a relative path to the base path.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_Click = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

std::string CN3UIButton::GetSndFName_On() const
{
	if (m_pSnd_On) return m_pSnd_On->m_szFileName;
	else return std::string("");
}

std::string CN3UIButton::GetSndFName_Click() const
{
	if (m_pSnd_Click) return m_pSnd_Click->m_szFileName;
	else return std::string("");
}

#endif
