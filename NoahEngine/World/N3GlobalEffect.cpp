#include "pch.h"
#include "N3GlobalEffect.h"

CN3GlobalEffect::CN3GlobalEffect()
{
	m_pVB = nullptr;
	m_pIB = nullptr;
	m_bActive = FALSE;
	m_iVC = 0;
	m_iIC = 0;
	m_fFadeTime = 0.0f; // During this time, the number of particles gradually increases by the target amount.
	m_fFadeTimeCur = 0.0f; // last hour...
	m_iFadeMode = 0;
}

CN3GlobalEffect::~CN3GlobalEffect()
{
	if (m_pVB) { m_pVB->Release(); m_pVB = nullptr;}
	if (m_pIB) { m_pIB->Release(); m_pIB = nullptr;}
}

void CN3GlobalEffect::Release()
{
	if (m_pVB) { m_pVB->Release(); m_pVB = nullptr;}
	if (m_pIB) { m_pIB->Release(); m_pIB = nullptr;}
	m_bActive = FALSE;
	m_iVC = 0;
	m_iIC = 0;
	m_fFadeTime = 0.0f; // During this time, the number of particles gradually increases by the target amount.
	m_fFadeTimeCur = 0.0f; // last hour...
	m_iFadeMode = 0;

	CN3Transform::Release();
}

void CN3GlobalEffect::Tick()
{
	CN3Transform::Tick(-1);

	if(m_iFadeMode && m_fFadeTime > 0) // time passes..
	{
		m_fFadeTimeCur += s_fSecPerFrm;
		if(m_fFadeTimeCur > m_fFadeTime) m_fFadeTimeCur = m_fFadeTime;
	}
}

void CN3GlobalEffect::Render(__Vector3& vPos)
{
	PosSet(vPos);
}


void CN3GlobalEffect::FadeSet(float fTimeToFade, bool bFadeIn)
{
	m_fFadeTime = fTimeToFade;
	m_fFadeTimeCur = 0;
	if(bFadeIn) m_iFadeMode = 1;
	else m_iFadeMode = -1;
}
