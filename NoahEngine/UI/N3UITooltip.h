#pragma once

#include "N3UIStatic.h"

class CN3UITooltip : public CN3UIStatic
{
public:
	CN3UITooltip();
	virtual ~CN3UITooltip();

// Attributes
public:
protected:
	float			m_fHoverTime;	// 마우스가 한곳에서 정지하여 있는 시간(누적)
	bool			m_bSetText;		// 이미 text가 설정되었는가?
	POINT			m_ptCursor;		// 커서의 위치

// Operations
public:
	void			SetText(const std::string& szText);
	virtual void	Release();
	virtual void	Tick();
	virtual void	Render();
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
protected:

};
