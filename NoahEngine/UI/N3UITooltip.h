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
	float			m_fHoverTime;	// ���콺�� �Ѱ����� �����Ͽ� �ִ� �ð�(����)
	bool			m_bSetText;		// �̹� text�� �����Ǿ��°�?
	POINT			m_ptCursor;		// Ŀ���� ��ġ

// Operations
public:
	void			SetText(const std::string& szText);
	virtual void	Release();
	virtual void	Tick();
	virtual void	Render();
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
protected:

};
