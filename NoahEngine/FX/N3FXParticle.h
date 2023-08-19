#pragma once

#include "N3Base.h"

class CN3FXPartParticles;

class CN3FXParticle : public CN3Base  
{
public:
	CN3FXPartParticles*	m_pRefParent;
	__VertexXyzColorT1*	m_pVB;

	int			m_iID;
	float		m_fLife;
	float		m_fCurrLife;
	
	int			m_iNumTex;
	int			m_iTexIdx;
	
	__Vector3	m_vCreatePoint;
	__Vector3	m_vAxis;
	__Vector3	m_vVelocity;
	__Vector3	m_vAccel;

	float		m_fDropVel;
	float		m_fDropY;
	float		m_fRot;
	__Vector3	m_vLcPos;
	__Vector3	m_vWdPos;
	DWORD		m_dwColor;
	float		m_fSize;
	//float		m_fCurrSize;

public:
	bool Tick();
	
	CN3FXParticle();	
	virtual ~CN3FXParticle();
};
