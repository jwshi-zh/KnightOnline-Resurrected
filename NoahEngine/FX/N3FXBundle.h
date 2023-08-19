#pragma once

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"
#include "N3FXPartBase.h"

class CN3FXBundle : public CN3BaseFileAccess  
{
public:
	int					m_iVersion;
	std::string			m_strName;
	int					m_iMoveType;
	float				m_fVelocity;
	bool				m_bStatic;

	float				m_fLife0;

	FXPARTWITHSTARTTIME*	m_pPart[MAX_FX_PART];
	
	float				m_fLife;
	DWORD				m_dwState;

	__Vector3			m_vPos;
	__Vector3			m_vPrePos;
	__Vector3			m_vDestPos;
	__Vector3			m_vDir;
	
	int					m_iSourceID;
	int					m_iTargetID;
	int					m_iTargetJoint;
	int					m_iSourceJoint;

	bool				m_bDependScale;
	//__Vector3			m_vTargetScale;
	float				m_fTargetScale;

//	int					m_iNumStep;
//	int					m_iCurrStep;
	static float		m_fEffectSndDist;

	class CN3SndObj*	m_pSndObj;

protected:
	bool	CheckAllPartsDead();

public:
	virtual void	Trigger(int iSourceID=0, int iTargetID = 0, int iTargetJoint = 0, int iSndID = -1);
	virtual bool	Tick();
	virtual void	Duplicate(CN3FXBundle* pDestBundle);

	void	Init();
	void	Stop(bool immediately=false);
	
	void	Render() const;
	DWORD	GetState() const { return m_dwState; }

	virtual bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);

//	int				GetPartCount() { return m_pPartList.size(); }
	CN3FXPartBase*	GetPart(int i);
	float			GetPartSTime(int i);
	void			SetPartSTime(int i, float time);
	static void		SetEffectSndDistance(float fDist) { m_fEffectSndDist = fDist; }
	static float	GetEffectSndDistance() { return m_fEffectSndDist; }

	CN3FXBundle();	
	virtual ~CN3FXBundle();

#ifdef _N3TOOL
public:
	CN3FXPartBase*	SetPart(const char* pFileName);
	bool	DecodeScriptFile(const char* lpPathName);
#endif // end of _N3TOOL
};
