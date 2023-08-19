#pragma once

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"
#include "N3Texture.h"

class CN3FXBundle;

class CN3FXPartBase : public CN3BaseFileAccess  
{
public:
	int				m_iVersion;
	int				m_iBaseVersion;

	CN3FXBundle*	m_pRefBundle;		//	A pointer to the bundle to which I belong.
	CN3FXPartBase*	m_pRefPrevPart;		//	The part in front of me...

	int				m_iType;			//	format type (particle, mesh, board...)
	float			m_fLife;

	std::string		m_strName;

	__Vector3		m_vVelocity;
	__Vector3		m_vAcceleration;
	__Vector3		m_vRotVelocity;

	float			m_fCurrLife;			
	__Vector3		m_vCurrVelocity;
	__Vector3		m_vCurrPos;

	DWORD			m_dwState;
	__Vector3		m_vPos;

	bool			m_bOnGround;

	char			m_pTexName[MAX_PATH];
	CN3Texture**	m_ppRefTex;
	int				m_iNumTex;
	float			m_fTexFPS;

	float			m_fFadeOut;
	float			m_fFadeIn;

	DWORD			m_dwRenderFlag;

	DWORD			m_dwSrcBlend;
	DWORD			m_dwDestBlend;
	BOOL			m_bAlpha;

	DWORD			m_dwZEnable;
	DWORD			m_dwZWrite;
	DWORD			m_dwLight;
	DWORD			m_dwDoubleSide;
		
protected:
	virtual bool	IsDead();

public:
	virtual float	GetGroundHeight(float x, float z) { return 0.01f; }

	virtual void	Init();
	virtual void	Start();
	virtual void	Stop();
	virtual bool	Tick();
	virtual void	Render();
	virtual bool	Load(HANDLE hFile);
	virtual bool	Save(HANDLE hFile);
	void	Duplicate(CN3FXPartBase* pSrc);

#ifdef _N3TOOL
protected:
	virtual bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);
	
public:
	bool	DecodeScriptFile(const char* lpPathName);
#endif // end of _N3TOOL

public:
	CN3FXPartBase();
	virtual ~CN3FXPartBase();
};
