#pragma once

#include "N3FXPartBase.h"

class CN3FXShape;

class CN3FXPartMesh : public CN3FXPartBase
{
public:
	CN3FXShape*	m_pShape;
	CN3FXShape*	m_pRefShape;

	DWORD		m_dwCurrColor;
	
	char		m_cTextureMoveDir;	//Texture movement direction..1:up 2:down, 3:left, 4:right
	float		m_fu;
	float		m_fv;

	__Vector3	m_vUnitScale;
	__Vector3	m_vScaleVel;
	__Vector3	m_vCurrScaleVel;
	__Vector3	m_vScaleAccel;
	__Vector3	m_vDir;

	bool		m_bTexLoop;
	float		m_fMeshFPS;

protected:
	bool	IsDead();

public:
	void	Init();
	void	Start();
	void	Stop();
	bool	Tick();
	void	Render();
	bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);
	void	Duplicate(CN3FXPartMesh* pSrc);
		
public:
	void	Rotate() const;
	void	Move();
	void	Scaling();
	void	MoveTexUV();

	int		NumPart();
	int		NumVertices(int Part);
	LPDIRECT3DVERTEXBUFFER9 GetVB(int Part);

	CN3FXPartMesh();
	virtual ~CN3FXPartMesh();

#ifdef _N3TOOL
	bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);
#endif // end of _N3TOOL

};
