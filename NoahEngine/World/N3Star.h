#pragma once

#include "N3Base.h"

const int	MAX_STAR = 200;

class CN3Star : public CN3Base  
{
public:
	CN3Star();
	virtual ~CN3Star();

// Attributes
public:
	int		GetNumStar() const {return m_iCurNumStar;}
protected:
	int		m_iCurNumStar;		// Number of stars currently floating
	int		m_iNextNumStar;		// number of stars to be seen
	float	m_fAddTime;			// Time interval at which one star is added or removed
	BOOL	m_bIncrease;		// Will the stars increase or decrease?
	__VertexXyzColor	m_Stars[MAX_STAR];
// Operations
public:
	void		Init();
	virtual void Release();
	virtual void Tick();
	virtual void Render();

	void		SetStar(int iNum, float fSec);
};
