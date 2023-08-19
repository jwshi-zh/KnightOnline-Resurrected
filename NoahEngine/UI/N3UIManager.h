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
		BOOL	bSet;	// 이미 값이 3D device의 값으로 세팅이 되었는지 판단.
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
