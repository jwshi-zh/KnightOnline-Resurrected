#pragma once

#include "N3UIImage.h"

class CN3UIIcon  : public CN3UIImage
{
	D3DCOLOR	m_dc;	// Backup Color

public:
	CN3UIIcon();
	virtual ~CN3UIIcon();

	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Render();	
};
