#pragma once

#include "N3FXPartBase.h"

class CN3FXPartBillBoard : public CN3FXPartBase  
{
public:
	int					m_iNum;
	float				m_fSizeX;
	float				m_fSizeY;
	bool				m_bTexLoop;
	
	int					m_iTexIdx;
	DWORD				m_dwCurrColor;

	float				m_fRadius;

	__VertexXyzColorT1*	m_pVB;

	bool				m_bRoateOnlyY;

	float				m_fScaleVelX;
	float				m_fScaleVelY;
	float				m_fScaleAccelX;
	float				m_fScaleAccelY;

	float				m_fCurrScaleVelX;
	float				m_fCurrScaleVelY;

	float				m_fCurrSizeX;
	float				m_fCurrSizeY;

	__Matrix44			m_mtxRot;
	float				m_fRotBillBoardX;
	float				m_fRotBillBoardY;
	float				m_fRotBillBoardZ;

protected:
	__Vector3			m_vUnit[4];

protected:
	void	CreateVB();
	bool	IsDead();
	__Vector3 Rotate2AbsolutePos(__Vector3 vRelativePos);
	float	CameraDist() const;

public:
	void	Init();
	void	Start();
	void	Stop();
	bool	Tick();
	void	Render();
	bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);
	void	Duplicate(CN3FXPartBillBoard* pSrc);

	void	SetScale(float size) { m_fSizeX = m_fCurrSizeX = size; m_fSizeY = m_fCurrSizeY = size; }
	void	SetRadius(float rad) { m_fRadius = rad; }
	
public:
	CN3FXPartBillBoard();
	virtual ~CN3FXPartBillBoard();

#ifdef _N3TOOL
	bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);
#endif // end of _N3TOOL
};
