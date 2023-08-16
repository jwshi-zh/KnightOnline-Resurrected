#pragma once

#include "N3Base.h"

class CN3ColorChange  : public CN3Base
{
public:
	CN3ColorChange();
	virtual ~CN3ColorChange();

public:
	D3DCOLOR	GetCurColor() const {return m_CurColor;}
	D3DCOLOR	GetNextColor() const {return m_NextColor;}
	void		ChangeColor(D3DCOLOR color, float fSec = 0.0f);
	void		Tick();
	void		SetPercentage(float fPercentage);
protected:
	D3DCOLOR	m_CurColor;	// ���� ��.
	D3DCOLOR	m_PrevColor;	// ��ȭ�ϱ� �� ��.
	D3DCOLOR	m_NextColor;	// ���������� ��ȭ�� ��.
	float		m_fPercentage;	// ���� ��ȭ�� ����(0.0 ~ 1.0)
	float		m_fRate;		// �ʴ� ��ȭ��
	float		m_fTempSec;		// �ӽ� �ð� ���� ����
};

class CN3DeltaChange  : public CN3Base
{
public:
	CN3DeltaChange();
	virtual ~CN3DeltaChange();

public:
	float		GetCurDelta() const {return m_fCurDelta;}
	float		GetNextDelta() const {return m_fNextDelta;}
	void		ChangeDelta(float fDelta, float fSec = 0.0f);
	void		Tick();
	void		SetPercentage(float fPercentage);
protected:
	float		m_fCurDelta;	// ���� ��.
	float		m_fPrevDelta;	// ��ȭ�ϱ� �� ��.
	float		m_fNextDelta;	// ���������� ��ȭ�� ��.

	float		m_fPercentage;	// ���� ��ȭ�� ����(0.0 ~ 1.0)
	float		m_fRate;		// �ʴ� ��ȭ��
	float		m_fTempSec;		// �ӽ� �ð� ���� ����
};
