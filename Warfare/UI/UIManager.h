#pragma once

#include "N3UIBase.h"

struct __IconItemSkill;

class CUIManager : public CN3UIBase  
{
protected:
	struct __RenderStateForUI
	{
		DWORD dwZEnable, dwAlphaBlend, dwSrcBlend, dwDestBlend, dwFog;
		DWORD dwMagFilter, dwMinFilter, dwMipFilter;
#ifdef _DEBUG
		BOOL	bSet;	// Determine whether the value has already been set as the value of the 3D device.
#endif
	};
	
	static __RenderStateForUI s_sRSFU;	// RenderStateForUI
	DWORD		m_dwMouseFlagsCur;
	bool		m_bEnableOperation;		// Is UI manipulation possible?
	CN3UIBase*	m_pUIFocused;

public:
	bool		m_bDoneSomething;		// I did the manipulation in the UI...

public:
	void UserMoveHideUIs();
	CN3UIBase* GetEnableFocusTopUI(bool bVisible);
	static void	RenderStateRestore();
	static void	RenderStateSet();

	void	EnableOperationSet(bool bEnable) { m_bEnableOperation = bEnable; } // UI controls available/disabled..
	bool	EnableOperation() const { return m_bEnableOperation; } // Is UI manipulation possible?

	void		Release();
	void		Tick();
	DWORD		MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	DWORD		GetCurMouseFlags() { return m_dwMouseFlagsCur; }
	CN3UIBase*	GetFocusedUI() { return m_pUIFocused; } // Import the taken UI..
	void		SetFocusedUI(CN3UIBase* pUI); // Import the taken UI..
	CN3UIBase*	GetTopUI(bool bVisible); // Get the UI on top.
	void		SetVisibleFocusedUI(CN3UIBase* pUI);
	CN3UIBase*	ReFocusUI();
	void		Render();
	void		ReorderChildList();

	bool		BroadcastIconDropMsg(__IconItemSkill* spItem);

	CUIManager();
	virtual ~CUIManager();
};
