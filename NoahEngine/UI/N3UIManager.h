#pragma once

#include "N3UIBase.h"

class CN3UIManager : public CN3UIBase  
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
	
	static __RenderStateForUI s_sRSFU; // RenderStateForUI

	DWORD	m_dwMouseFlagsCur;
public:
	CN3UIManager();
	virtual ~CN3UIManager();

// Operations
public:
	static void	RenderStateRestore();
	static void	RenderStateSet();

	void	Release();
	void	Tick();
	DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	DWORD	GetCurMouseFlags() const { return m_dwMouseFlagsCur; }
	void	Render();
	void	ReorderChildList();
};
