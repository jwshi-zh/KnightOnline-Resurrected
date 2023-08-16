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
	int		m_iCurNumStar;		// ���� ���ִ� ���� ��
	int		m_iNextNumStar;		// ������ ���� �� ����
	float	m_fAddTime;			// �� �ϳ��� �߰��ǰų� �������� �ð� ����
	BOOL	m_bIncrease;		// ���� ������ ���ΰ� ������ ���ΰ�
	__VertexXyzColor	m_Stars[MAX_STAR];
// Operations
public:
	void		Init();
	virtual void Release();
	virtual void Tick();
	virtual void Render();

	void		SetStar(int iNum, float fSec);
};
