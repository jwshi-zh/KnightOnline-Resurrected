#pragma once

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"
#include "N3FXPartBase.h"

class CN3FXPartBottomBoard : public CN3FXPartBase  
{
public:
	float				m_fSizeX;
	float				m_fSizeZ;

	bool				m_bTexLoop;
	int					m_iTexIdx;

	DWORD				m_dwCurrColor;

	__VertexXyzColorT1*	m_pVB;

	float				m_fScaleVelX;
	float				m_fScaleVelZ;
	float				m_fScaleAccelX;
	float				m_fScaleAccelZ;

	float				m_fCurrScaleVelX;
	float				m_fCurrScaleVelZ;

	float				m_fCurrSizeX;
	float				m_fCurrSizeZ;

	float				m_fGap;
	
protected:
	__Vector3*			m_vUnit;

protected:
	void	CreateVB();
	bool	IsDead();

public:
	void	Init();
	void	Start();
	void	Stop();
	bool	Tick();
	void	Render();
	bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);
	void	Duplicate(CN3FXPartBottomBoard* pSrc);

public:
	CN3FXPartBottomBoard();
	virtual ~CN3FXPartBottomBoard();

#ifdef _N3TOOL
	bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);
#endif // end of _N3TOOL
};
