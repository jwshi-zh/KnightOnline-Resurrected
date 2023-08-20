#pragma once

#include "N3Transform.h"
const DWORD GETYPE_RAIN = 0x00000001;
const DWORD GETYPE_SNOW = 0x00000002;

class CN3GlobalEffect : public CN3Transform  
{
public:
	CN3GlobalEffect();
	virtual ~CN3GlobalEffect();

// Attributes
public:
	void						SetActive(BOOL bActive) {m_bActive = bActive;}
protected:
	int							m_iVC;
	int							m_iIC;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	LPDIRECT3DINDEXBUFFER9		m_pIB;
	DWORD						m_dwEffectType;
	BOOL						m_bActive;
	float						m_fFadeTime; // During this time, the number of particles gradually increases by the target amount.
	float						m_fFadeTimeCur; // last hour...
	int							m_iFadeMode; // 1 - FadeIn 0... -1 FadeOut

// Operations
public:
	virtual bool	NeedDelete() { if(m_iFadeMode < 0 && m_fFadeTimeCur >= m_fFadeTime) return true; return false; }
	virtual void	FadeSet(float fTimeToFade, bool bFadeIn);
	virtual void	Release();
	virtual void	Tick();
	virtual void	Render(__Vector3& vPos);
protected:

};
