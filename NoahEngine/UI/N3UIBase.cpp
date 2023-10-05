#include "pch.h"
#include "N3UIBase.h"

#include <vector>
#include "N3UIButton.h"
#include "N3UIProgress.h"
#include "N3UIImage.h"
#include "N3UIScrollBar.h"
#include "N3UIString.h"
#include "N3UITrackBar.h"
#include "N3UIStatic.h"
#include "N3UIEdit.h"
#include "N3UITooltip.h"
#include "N3UIArea.h"
#include "N3UIList.h"
#ifdef _REPENT
#include "N3UIIconSlot.h"
#endif

#include "N3SndMgr.h"
#include "N3SndObj.h"

CN3UIEdit* CN3UIBase::s_pFocusedEdit = nullptr;
CN3UITooltip* CN3UIBase::s_pTooltipCtrl = nullptr;
std::string CN3UIStatic::s_szStringTmp; // temporary variable...

CN3UIBase::CN3UIBase()
{
	m_eType = UI_TYPE_BASE;
	m_pParent = nullptr;
	m_pChildUI	= nullptr;
	m_pParentUI = nullptr;

	m_iChildID	= -1;

	ZeroMemory(&m_rcRegion, sizeof(m_rcRegion));
	ZeroMemory(&m_rcMovable, sizeof(m_rcMovable));
	m_eState = UI_STATE_COMMON_NONE;
	m_dwStyle = UISTYLE_NONE;

	m_dwReserved = 0;
	m_bVisible = true;
	m_pSnd_OpenUI = nullptr;
	m_pSnd_CloseUI = nullptr;
}

CN3UIBase::~CN3UIBase()
{
	if(m_pParent) m_pParent->RemoveChild(this);	// delete me from parent's child

	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);

	while(!m_Children.empty())
	{
		const CN3UIBase* pChild = m_Children.front();
		if (pChild) delete pChild;	// When a child is deleted, it is automatically removed from the parent's list.
									// Therefore, there is no need to delete a part from the list.
	}
}

void CN3UIBase::Release()
{
	if(m_pParent) m_pParent->RemoveChild(this);

	ZeroMemory(&m_rcRegion, sizeof(m_rcRegion));
	ZeroMemory(&m_rcMovable, sizeof(m_rcMovable));
	
	m_szID = "";
	m_szToolTip = "";

	m_eState = UI_STATE_COMMON_NONE;
	m_dwStyle = UISTYLE_NONE;
	m_dwReserved = 0;
	m_bVisible = true;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);

	while(!m_Children.empty())
	{
		const CN3UIBase* pChild = m_Children.front();
		if (pChild) delete pChild;	// When a child is deleted, it is automatically removed from the parent's list.
									// Therefore, there is no need to delete a part from the list.
	}

	CN3BaseFileAccess::Release();
}

void CN3UIBase::Init(CN3UIBase* pParent)
{
	Release();
	SetParent(pParent);
}

void CN3UIBase::RemoveChild(CN3UIBase* pChild)
{
	if(nullptr == pChild) return;

	for(auto itor = m_Children.begin(); m_Children.end() != itor;)
	{
		if((*itor) == pChild)
		{
			itor = m_Children.erase(itor);
			break;
		}
		else
		{
			++itor;
		}
	}
}

void CN3UIBase::SetParent(CN3UIBase* pParent)
{
	if(m_pParent) m_pParent->RemoveChild(this);
	m_pParent = pParent;
	if (m_pParent) m_pParent->AddChild(this);
}

POINT CN3UIBase::GetPos() const
{
	POINT p;
	p.x = m_rcRegion.left;
	p.y = m_rcRegion.top;
	return p;
}

// change position
void CN3UIBase::SetPos(int x, int y)
{
	// Find the moving difference
	int dx, dy;
	dx = x - m_rcRegion.left;
	dy = y - m_rcRegion.top;

	MoveOffset(dx, dy);
}

void CN3UIBase::SetPosCenter()
{
	const POINT pt = this->GetPos();
	const RECT rc = this->GetRegion();
	const int iW = rc.right - rc.left;
	const int iH = rc.bottom - rc.top;
	const int iWVP = CN3Base::s_CameraData.vp.Width;
	const int iHVP = CN3Base::s_CameraData.vp.Height;

	// Find the moving difference
	MoveOffset(((iWVP - iW) / 2) - pt.x, ((iHVP - iH) / 2) - pt.y);
}

// Move by offset. (children also move)
BOOL CN3UIBase::MoveOffset(int iOffsetX, int iOffsetY)
{
	if (0 == iOffsetX && 0 == iOffsetY) return FALSE;
	// ui area
	m_rcRegion.left += iOffsetX;		m_rcRegion.top += iOffsetY;
	m_rcRegion.right += iOffsetX;		m_rcRegion.bottom += iOffsetY;

	// movable area
	if(	m_rcMovable.right - m_rcMovable.left != 0 &&
		m_rcMovable.bottom - m_rcMovable.top != 0 )
	{
		m_rcMovable.left += iOffsetX;		m_rcMovable.top += iOffsetY;
		m_rcMovable.right += iOffsetX;		m_rcMovable.bottom += iOffsetY;
	}

	// update children coordinates
	CN3UIBase* pCUI = nullptr; // Child UI...
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		pCUI = (*itor);
		__ASSERT(pCUI, "child UI pointer is NULL!");
		pCUI->MoveOffset(iOffsetX, iOffsetY);
	}
	return TRUE;
}

void CN3UIBase::CenterInScreen()
{
	RECT rc = GetRegion();
	SetPos((CN3Base::s_CameraData.vp.Width - (rc.right - rc.left)) / 2, (CN3Base::s_CameraData.vp.Height - (rc.bottom - rc.top)) / 2);
}

// true if the point (x,y) is in the domain..
bool CN3UIBase::IsIn(int x, int y) const
{
	if(x<m_rcRegion.left || x>m_rcRegion.right || y<m_rcRegion.top || y>m_rcRegion.bottom) return false;
	return true;
}

void CN3UIBase::SetSize(int iWidth, int iHeight)
{
	RECT rc;
	SetRect(&rc, m_rcRegion.left, m_rcRegion.top, m_rcRegion.left + iWidth, m_rcRegion.top + iHeight);
	SetRegion(rc);
}

bool CN3UIBase::ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg) // Receives a message. Sender, msg
{
	if(m_pParent && pSender) m_pParent->ReceiveMessage(pSender, dwMsg); // If there are parents, send them to them too.
	return true;
}

void CN3UIBase::CallBackProc(int iID, DWORD dwFlag)
{
}

void CN3UIBase::ShowWindow(int iID, CN3UIBase* pParent)
{
	if(pParent)
		pParent->m_pChildUI = this;

	m_pParentUI = pParent;
	m_iChildID = iID;

	SetVisible(true);
}

bool CN3UIBase::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);
	DWORD dwRWC = NULL;

	// children information
	uint16_t sCC = 0;
	uint16_t sIdk = 0;
	ReadFile(hFile, &sCC, sizeof(sCC), &dwRWC, nullptr); // children count
	ReadFile(hFile, &sIdk, sizeof(sIdk), &dwRWC, nullptr); // children count
	eUI_TYPE eChildUIType;

	for(int i = 0; i < sCC; i++)
	{
		CN3UIBase* pChild = nullptr;
		ReadFile(hFile, &eChildUIType, sizeof(eChildUIType), &dwRWC, nullptr); // child의 ui type

		switch(eChildUIType)
		{
		case UI_TYPE_BASE:			pChild = new CN3UIBase();			break;
		case UI_TYPE_IMAGE:			pChild = new CN3UIImage();			break;
		case UI_TYPE_STRING:		pChild = new CN3UIString();			break;
		case UI_TYPE_BUTTON:		pChild = new CN3UIButton();			break;
		case UI_TYPE_STATIC:		pChild = new CN3UIStatic();			break;
		case UI_TYPE_PROGRESS:		pChild = new CN3UIProgress();		break;
		case UI_TYPE_SCROLLBAR:		pChild = new CN3UIScrollBar();		break;
		case UI_TYPE_TRACKBAR:		pChild = new CN3UITrackBar();		break;
		case UI_TYPE_EDIT:			pChild = new CN3UIEdit();			break;
		case UI_TYPE_AREA:			pChild = new CN3UIArea();			break;
#ifdef _REPENT
		case UI_TYPE_ICONSLOT:		pChild = new CN3UIIconSlot();		break;
#endif
		case UI_TYPE_LIST:			pChild = new CN3UIList();			break;
		}
		__ASSERT(pChild, "Unknown type UserInterface!!!");
		pChild->Init(this);
		pChild->Load(hFile);
	}

	// base information
	int iIDLen = 0;
	ReadFile(hFile, &iIDLen, sizeof(iIDLen), &dwRWC, nullptr);				// ui id length
	if (iIDLen>0)
	{
		std::vector<char> buffer(iIDLen+1, NULL);
		ReadFile(hFile, buffer.data(), iIDLen, &dwRWC, nullptr);			// ask id
		m_szID = buffer.data();
	}
	else
	{
		m_szID = "";
	}
	ReadFile(hFile, &m_rcRegion, sizeof(m_rcRegion), &dwRWC, nullptr);		// m_rcRegion
	ReadFile(hFile, &m_rcMovable, sizeof(m_rcMovable), &dwRWC, nullptr);	// m_rcMovable
	ReadFile(hFile, &m_dwStyle, sizeof(m_dwStyle), &dwRWC, nullptr);		// style
	ReadFile(hFile, &m_dwReserved, sizeof(m_dwReserved), &dwRWC, nullptr);	// m_dwReserved

	int iTooltipLen;
	ReadFile(hFile, &iTooltipLen, sizeof(iTooltipLen), &dwRWC, nullptr);		// tooltip string length
	if (iTooltipLen>0)
	{
		std::vector<char> buffer(iTooltipLen+1, NULL);
		ReadFile(hFile, buffer.data(), iTooltipLen, &dwRWC, nullptr);
		m_szToolTip = buffer.data();
	}

	// To convert the old uif file, block the sound loading part
	int iSndFNLen = 0;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, nullptr);		// sound file string length
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, buffer.data(), iSndFNLen, &dwRWC, nullptr);

		__ASSERT(NULL == m_pSnd_OpenUI, "memory leak");
		m_pSnd_OpenUI = s_SndMgr.CreateObj(buffer.data(), SNDTYPE_2D);
	}

	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, nullptr);		// sound file string length
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, buffer.data(), iSndFNLen, &dwRWC, nullptr);

		__ASSERT(NULL == m_pSnd_CloseUI, "memory leak");
		m_pSnd_CloseUI = s_SndMgr.CreateObj(buffer.data(), SNDTYPE_2D);
	}

	return true;
}


void CN3UIBase::Tick()
{
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->Tick();
	}
}

void CN3UIBase::Render()
{
	if (!m_bVisible) return;	// If not visible, don't render the children.

	for(auto itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->Render();

		// this_ui
		CN3UIBase* pCUI = nullptr;
		pCUI = pChild->m_pChildUI;
		while(pCUI)
		{
			pCUI->Render();
			pCUI = pCUI->m_pChildUI;
		}
	}
}

DWORD CN3UIBase::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld )
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	// UI moving code
	if (UI_STATE_COMMON_MOVE == m_eState)
	{
		if (dwFlags&UI_MOUSE_LBCLICKED)
		{
			SetState(UI_STATE_COMMON_NONE);
		}
		else
		{
			MoveOffset(ptCur.x - ptOld.x, ptCur.y - ptOld.y);
		}
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	if(false == IsIn(ptCur.x, ptCur.y))	// out of range
	{
		if(false == IsIn(ptOld.x, ptOld.y))
		{
			return dwRet; // If the previous coordinates are also outside the domain
		}
		dwRet |= UI_MOUSEPROC_PREVINREGION;	// The previous coordinates were inside the area.
	}
	else
	{
		// tool tip related
		if (s_pTooltipCtrl) s_pTooltipCtrl->SetText(m_szToolTip);
	}
	dwRet |= UI_MOUSEPROC_INREGION;	// This coordinate is inside the area.


	// this_ui
	if(m_pChildUI && m_pChildUI->IsVisible())
		return dwRet;

	// Send message to child
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		const DWORD dwChildRet = pChild->MouseProc(dwFlags, ptCur, ptOld);
		if (UI_MOUSEPROC_DONESOMETHING & dwChildRet)
		{	// In this case, it is a case where the distance is focused.
			// (The code below must be done in the place where the dialog is managed. Therefore, it is blocked)
			// m_Children.erase(itor); // first remove from list
			// m_Children. push_front(pChild); // put in front I want to draw the drawing order last

			dwRet |= (UI_MOUSEPROC_CHILDDONESOMETHING|UI_MOUSEPROC_DONESOMETHING);
			return dwRet;
		}
	}

	// UI moving code
	if (UI_STATE_COMMON_MOVE != m_eState && 
			PtInRect(&m_rcMovable, ptCur) && (dwFlags&UI_MOUSE_LBCLICK) )
	{
		SetState(UI_STATE_COMMON_MOVE);
		dwRet |= UI_MOUSEPROC_DONESOMETHING;
		return dwRet;
	}

	return dwRet;
}

bool CN3UIBase::EnableTooltip(const std::string& szFN)
{
	delete s_pTooltipCtrl;
	s_pTooltipCtrl = nullptr;
	if (szFN.empty()) return false;
	
	s_pTooltipCtrl = new CN3UITooltip();
	s_pTooltipCtrl->Init(nullptr);
	s_pTooltipCtrl->LoadFromFile(szFN);
	return true;
}

void CN3UIBase::DestroyTooltip()
{
	if (s_pTooltipCtrl) {delete s_pTooltipCtrl; s_pTooltipCtrl = nullptr;}
}

CN3UIBase* CN3UIBase::GetChildByID(const std::string& szID)
{
	if(szID.empty()) return nullptr;

	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		// if(pChild->m_szID == szID) return pChild;
		if(lstrcmpi(pChild->m_szID.c_str(), szID.c_str()) == 0) return pChild; // Search case insensitive.
	}
	return nullptr;
}

void CN3UIBase::SetVisible(bool bVisible)
{
	if (bVisible != m_bVisible)
	{
		if (bVisible) { if(m_pSnd_OpenUI) m_pSnd_OpenUI->Play(); }	// opening sound
		else { if(m_pSnd_CloseUI) m_pSnd_CloseUI->Play(); }	// closing sound
	}
	m_bVisible = bVisible;
	if(!m_bVisible)
	{
		if(m_pChildUI)
		{
			m_pChildUI->SetVisible(false);
		}
		m_pChildUI	= nullptr;
		if(m_pParentUI)
		{
			if(m_pParentUI->m_pChildUI == this)
				m_pParentUI->m_pChildUI = nullptr;
		}
		m_pParentUI = nullptr;
		m_iChildID	= -1;
	}
}

void CN3UIBase::SetVisibleWithNoSound(bool bVisible, bool bWork, bool bReFocus)
{ 
	m_bVisible = bVisible; 
	if(!m_bVisible)
	{
		if(m_pChildUI)
		{
			m_pChildUI->SetVisible(false);
		}
		m_pChildUI	= nullptr;
		if(m_pParentUI)
		{
			if(m_pParentUI->m_pChildUI == this)
				m_pParentUI->m_pChildUI = nullptr;
		}
		m_pParentUI = nullptr;
		m_iChildID	= -1;
	}
}

#ifndef _N3TOOL
void CN3UIBase::operator = (const CN3UIBase& other)
{
	Init(nullptr);	// Initialize with no parent

	auto it = other.m_Children.begin();
	const auto itEnd = other.m_Children.end();
	CN3UIBase* pOtherChild = nullptr;
	CN3UIBase* pChild = nullptr;
	for(; it != itEnd; it++)
	{
		pOtherChild = *it;

		if(nullptr == pOtherChild) continue;

		pChild = nullptr;
		switch(pOtherChild->UIType())
		{
		case UI_TYPE_BASE:
			{ 
				pChild = new CN3UIBase();
				*pChild = *pOtherChild;
			}
			break;
		case UI_TYPE_BUTTON:
			{
				auto*pUINew = new CN3UIButton();
				*pUINew = *((CN3UIButton*)pOtherChild);
				pChild = pUINew;
			}
			break;	// button
		case UI_TYPE_STATIC:	
			{
				auto* pUINew = new CN3UIStatic();		
				*pUINew = *((CN3UIStatic*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// static (class with background picture and text)
		case UI_TYPE_PROGRESS:	
			{
				auto* pUINew = new CN3UIProgress();	
				*pUINew = *((CN3UIProgress*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// progress
		case UI_TYPE_IMAGE:		
			{
				auto* pUINew = new CN3UIImage();		
				*pUINew = *((CN3UIImage*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// image
		case UI_TYPE_SCROLLBAR:	
			{
				auto* pUINew = new CN3UIScrollBar();	
				*pUINew = *((CN3UIScrollBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// scroll bar
		case UI_TYPE_STRING:	
			{
				auto* pUINew = new CN3UIString();		
				*pUINew = *((CN3UIString*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// string
		case UI_TYPE_TRACKBAR:	
			{
				auto* pUINew = new CN3UITrackBar();	
				*pUINew = *((CN3UITrackBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// track bar
		case UI_TYPE_EDIT:		
			{
				auto* pUINew = new CN3UIEdit();		
				*pUINew = *((CN3UIEdit*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// edit
		case UI_TYPE_AREA:		
			{
				auto* pUINew = new CN3UIArea();		
				*pUINew = *((CN3UIArea*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// area
		case UI_TYPE_TOOLTIP:
			{
				auto* pUINew = new CN3UITooltip();
				*pUINew = *((CN3UITooltip*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		case UI_TYPE_LIST:
			{
				auto* pUINew = new CN3UIList();
				*pUINew = *((CN3UIList*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		// case UI_TYPE_ICON: pUIDest = new CN3UIIcon(); *pUIDest = *((CN3UIBase*)pUISrc); break; // icon
		// case UI_TYPE_ICON_MANAGER:	pUIDest = new CN3UIIconManager();	*pUIDest = *((CN3UIBase*)pUISrc); break;	// icon manager..
#ifdef _REPENT
		case UI_TYPE_ICONSLOT:
			{
				CN3UIIconSlot* pUINew = new CN3UIIconSlot();
				*pUINew = *((CN3UIIconSlot*)pOtherChild);
				pChild = pUINew;
			}
			break;	// icon slot
#endif
		}
		if(pChild) pChild->SetParent(this);	// parent designation
	}

	m_bVisible = other.m_bVisible;
	m_dwReserved = other.m_dwReserved;
	m_dwStyle = other.m_dwStyle;
	m_eState = other.m_eState;
	m_eType = other.m_eType;

	if(other.m_pSnd_OpenUI)
	{
		CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
		m_pSnd_OpenUI = s_SndMgr.CreateObj(other.m_pSnd_OpenUI->m_szFileName, SNDTYPE_2D);
	}

	if(other.m_pSnd_CloseUI)
	{
		CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);
		m_pSnd_CloseUI = s_SndMgr.CreateObj(other.m_pSnd_CloseUI->m_szFileName, SNDTYPE_2D);
	}

	m_rcMovable = other.m_rcMovable;
	m_rcRegion = other.m_rcRegion;
	m_szID = other.m_szID;
	m_szToolTip = other.m_szToolTip;
}
#endif

#ifdef _N3TOOL
bool CN3UIBase::Save(HANDLE hFile)
{
	CN3BaseFileAccess::Save(hFile);
	DWORD dwRWC = NULL;

	// child information
	int iCC = m_Children.size();
	WriteFile(hFile, &iCC, sizeof(iCC), &dwRWC, NULL); // The number of children is high..go..
	for(UIListReverseItor itor = m_Children.rbegin(); m_Children.rend() != itor; ++itor)
	// Since it is push_front when childadd, it must be saved backwards when saving.
	{
		CN3UIBase* pChild = (*itor);
		eUI_TYPE eUIType = pChild->UIType();

		WriteFile(hFile, &eUIType, sizeof(eUIType), &dwRWC, NULL); // Write UI Type..
		pChild->Save(hFile);
	}

	// base information
	int iIDLen = 0;
	iIDLen = m_szID.size();
	WriteFile(hFile, &iIDLen, sizeof(iIDLen), &dwRWC, NULL);				// id length
	if (iIDLen>0) WriteFile(hFile, m_szID.c_str(), iIDLen, &dwRWC, NULL);			// ask id
	WriteFile(hFile, &m_rcRegion, sizeof(m_rcRegion), &dwRWC, NULL);		// m_rcRegion
	WriteFile(hFile, &m_rcMovable, sizeof(m_rcMovable), &dwRWC, NULL);		// m_rcMovable
	WriteFile(hFile, &m_dwStyle, sizeof(m_dwStyle), &dwRWC, NULL);			// style
	WriteFile(hFile, &m_dwReserved, sizeof(m_dwReserved), &dwRWC, NULL);	// m_dwReserved

	int iTooltipLen = m_szToolTip.size();
	WriteFile(hFile, &iTooltipLen, sizeof(iTooltipLen), &dwRWC, NULL);		// tooltip string length
	if (iTooltipLen>0) WriteFile(hFile, m_szToolTip.c_str(), iTooltipLen, &dwRWC, NULL);

	int iSndFNLen = 0;
	if (m_pSnd_OpenUI) iSndFNLen = m_pSnd_OpenUI->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, NULL);		// sound file string length
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_OpenUI->m_szFileName.c_str(), iSndFNLen, &dwRWC, NULL);

	iSndFNLen = 0;
	if (m_pSnd_CloseUI) iSndFNLen = m_pSnd_CloseUI->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwRWC, NULL);		// sound file string length
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_CloseUI->m_szFileName.c_str(), iSndFNLen, &dwRWC, NULL);
	
	return true;
}

void CN3UIBase::ChangeImagePath(const std::string& szPathOld, const std::string& szPathNew)
{
	// child information
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->ChangeImagePath(szPathOld, szPathNew);
	}
}

void CN3UIBase::ChangeFont(const std::string& szFont)
{
	// child information
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->ChangeFont(szFont);
	}
}

void CN3UIBase::GatherImageFileName(std::set<std::string>& setImgFile)
{
	// child information
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		pChild->GatherImageFileName(setImgFile);
	}
}


void CN3UIBase::ResizeAutomaticalyByChild()
{
	if(m_Children.empty()) return;

	RECT rcMax = { 100000000, 100000000, -100000000, -100000000 };
	int iIndex = 0;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; itor++, iIndex++)
	{
		CN3UIBase* pChild = (*itor);
		RECT rcTmp = pChild->GetRegion();
		if(rcTmp.left < rcMax.left) rcMax.left = rcTmp.left;
		if(rcTmp.top < rcMax.top) rcMax.top = rcTmp.top;
		if(rcTmp.right > rcMax.right) rcMax.right = rcTmp.right;
		if(rcTmp.bottom > rcMax.bottom) rcMax.bottom = rcTmp.bottom;
	}

	RECT rcCur = this->GetRegion();
	if(rcCur.left > rcMax.left) rcCur.left = rcMax.left;
	if(rcCur.top > rcMax.top) rcCur.top = rcMax.top;
	if(rcCur.right < rcMax.right) rcCur.right = rcMax.right;
	if(rcCur.bottom < rcMax.bottom) rcCur.bottom = rcMax.bottom;
	// this->SetRegion(rcCur);
	m_rcRegion = rcCur;	// If SetRegion is done, the child's area may be changed, so I put it directly to change only my area.
}

int CN3UIBase::IsMyChild(CN3UIBase* pUI)
{
	int iIndex = 0;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; itor++, iIndex++)
	{
		CN3UIBase* pChild = (*itor);
		if(pChild == pUI) return iIndex;
	}

	return -1;
}

bool CN3UIBase::SwapChild(CN3UIBase* pChild1, CN3UIBase* pChild2)
{
	if(this->IsMyChild(pChild1) < 0 || IsMyChild(pChild2) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); m_Children.end() != itor1; itor1++)
		if(*itor1 == pChild1) break;
	if(itor1 == m_Children.end()) return false;

	for(UIListItor itor2 = m_Children.begin(); m_Children.end() != itor2; itor2++)
		if(*itor2 == pChild2) break;
	if(itor2 == m_Children.end()) return false;

	swap(*itor1, *itor2);

	return true;
}

bool CN3UIBase::MoveToHighest(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); m_Children.end() != itor1; itor1++)
	{
		if(*itor1 == pChild) 
		{
			m_Children.erase(itor1);
			m_Children.push_front(pChild);
			return true;
		}
	}

	return false;
}

bool CN3UIBase::MoveToLowest(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); m_Children.end() != itor1; itor1++)
	{
		if(*itor1 == pChild) 
		{
			m_Children.erase(itor1);
			m_Children.push_back(pChild);
			return true;
		}
	}

	return false;
}

bool CN3UIBase::MoveToLower(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); itor1 != m_Children.end(); itor1++)
	{
		if(*itor1 == pChild)
		{
			UIListItor itNext = itor1; itNext++;
			if(itNext != m_Children.end())
			{
				swap(*itNext, *itor1);
				return true;
			}
			break;
		}
	}

	return false;
}

bool CN3UIBase::MoveToUpper(CN3UIBase* pChild)
{
	if(this->IsMyChild(pChild) < 0) return false;
	
	for(UIListItor itor1 = m_Children.begin(); itor1 != m_Children.end(); itor1++)
	{
		if(*itor1 == pChild) 
		{
			if(itor1 != m_Children.begin())
			{
				UIListItor itPrev = itor1; itPrev--;
				swap(*itPrev, *itor1);
				return true;
			}
			break;
		}
	}

	return false;
}

void CN3UIBase::ArrangeZOrder()
{
	// Usually, the image becomes the background picture, so it is sent to the back in the child list.
	// because the last one is drawn first
	UIList tempList;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor;)
	{
		CN3UIBase* pChild = (*itor);
		if(UI_TYPE_IMAGE == pChild->UIType())
		{
			itor = m_Children.erase(itor);	// erase from current location
			tempList.push_back(pChild);		// store in temporary buffer
		}
		else ++itor;
	}

	for(itor = tempList.begin(); tempList.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		m_Children.push_back(pChild);		// put it at the end of the child list
	}
	tempList.clear();
}

void CN3UIBase::operator = (const CN3UIBase& other)
{
	Init(NULL);	// Initialize with no parent

	UIListItorConst it = other.m_Children.begin();
	UIListItorConst itEnd = other.m_Children.end();
	CN3UIBase* pOtherChild = NULL;
	CN3UIBase* pChild = NULL;
	for(; it != itEnd; it++)
	{
		pOtherChild = *it;

		if(NULL == pOtherChild) continue;

		pChild = NULL;
		switch(pOtherChild->UIType())
		{
		case UI_TYPE_BASE:
			{ 
				pChild = new CN3UIBase();
				*pChild = *pOtherChild;
			}
			break;
		case UI_TYPE_BUTTON:
			{
				CN3UIButton *pUINew = new CN3UIButton();
				*pUINew = *((CN3UIButton*)pOtherChild);
				pChild = pUINew;
			}
			break;	// button
		case UI_TYPE_STATIC:	
			{ 
				CN3UIStatic* pUINew = new CN3UIStatic();		
				*pUINew = *((CN3UIStatic*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// static (class with background picture and text)
		case UI_TYPE_PROGRESS:	
			{ 
				CN3UIProgress* pUINew = new CN3UIProgress();	
				*pUINew = *((CN3UIProgress*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// progress
		case UI_TYPE_IMAGE:		
			{ 
				CN3UIImage* pUINew = new CN3UIImage();		
				*pUINew = *((CN3UIImage*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// image
		case UI_TYPE_SCROLLBAR:	
			{ 
				CN3UIScrollBar* pUINew = new CN3UIScrollBar();	
				*pUINew = *((CN3UIScrollBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// scroll bar
		case UI_TYPE_STRING:	
			{ 
				CN3UIString* pUINew = new CN3UIString();		
				*pUINew = *((CN3UIString*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// string
		case UI_TYPE_TRACKBAR:	
			{ 
				CN3UITrackBar* pUINew = new CN3UITrackBar();	
				*pUINew = *((CN3UITrackBar*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// track bar
		case UI_TYPE_EDIT:		
			{ 
				CN3UIEdit* pUINew = new CN3UIEdit();		
				*pUINew = *((CN3UIEdit*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// edit
		case UI_TYPE_AREA:		
			{ 
				CN3UIArea* pUINew = new CN3UIArea();		
				*pUINew = *((CN3UIArea*)pOtherChild); 
				pChild = pUINew;
			} 
			break;	// area
		case UI_TYPE_TOOLTIP:
			{ 
				CN3UITooltip* pUINew = new CN3UITooltip();
				*pUINew = *((CN3UITooltip*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		case UI_TYPE_LIST:
			{ 
				CN3UIList* pUINew = new CN3UIList();
				*pUINew = *((CN3UIList*)pOtherChild);
				pChild = pUINew;
			} 
			break;	// tooltip
		// case UI_TYPE_ICON: pUIDest = new CN3UIIcon(); *pUIDest = *((CN3UIBase*)pUISrc); break; // icon
		// case UI_TYPE_ICON_MANAGER:	pUIDest = new CN3UIIconManager();	*pUIDest = *((CN3UIBase*)pUISrc); break;	// icon manager..
#ifdef _REPENT
		case UI_TYPE_ICONSLOT:
			{
				CN3UIIconSlot* pUINew = new CN3UIIconSlot();
				*pUINew = *((CN3UIIconSlot*)pOtherChild);
				pChild = pUINew;
			}
			break;	// icon slot
#endif
		}
		if(pChild) pChild->SetParent(this);	// parent designation
	}

	m_bVisible = other.m_bVisible;
	m_dwReserved = other.m_dwReserved;
	m_dwStyle = other.m_dwStyle;
	m_eState = other.m_eState;
	m_eType = other.m_eType;

	SetSndOpen(other.GetSndFName_OpenUI());
	SetSndClose(other.GetSndFName_CloseUI());

	m_rcMovable = other.m_rcMovable;
	m_rcRegion = other.m_rcRegion;
	m_szID = other.m_szID;
	m_szToolTip = other.m_szToolTip;
}

void CN3UIBase::SetSndOpen(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_OpenUI);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Passes a relative path to the base path.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_OpenUI = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

void CN3UIBase::SetSndClose(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_CloseUI);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Passes a relative path to the base path.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_CloseUI = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

std::string CN3UIBase::GetSndFName_OpenUI() const
{
	if (m_pSnd_OpenUI) return m_pSnd_OpenUI->m_szFileName;
	else return std::string("");
}

std::string CN3UIBase::GetSndFName_CloseUI() const
{
	if (m_pSnd_CloseUI) return m_pSnd_CloseUI->m_szFileName;
	else return std::string("");
}

bool CN3UIBase::ReplaceAllTextures(const std::string& strFind, const std::string& strReplace)
{
	if (strFind.size() <= 0 || strReplace.size() <= 0) return false;
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		if (false == (*itor)->ReplaceAllTextures(strFind, strReplace)) return false;
	}
	return true;
}

#endif	// _N3TOOL
