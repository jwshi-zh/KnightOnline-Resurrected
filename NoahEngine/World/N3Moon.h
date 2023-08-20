#pragma once

#include "N3Base.h"
#include <string>

class CN3Texture;
class CN3Moon : public CN3Base  
{
#ifdef _N3TOOL
	friend class CN3SkyMng;
#endif

public:
	CN3Moon();
	virtual ~CN3Moon();
// Attributes
public:
	int						GetMoonPhaseIndex() const {return m_iMoonPhaseIndex;}
protected:	
	__VertexTransformed		m_pVertices[4];
	CN3Texture				*m_pTexture;
	float					m_fCurRadian;		// Rotational position of the current moon
	float					m_fMoonRatio;		// moon pixel size
	int						m_iMoonPhaseIndex;	// Moon Shape Index 0~23;

// Operations
public:
	void	Init(const std::string& szTexFN);
	void	SetMoonPhase(int iIndex);	// set the shape of the moon
	void	SetCurAngle(float fAngle)	{m_fCurRadian = D3DXToRadian(fAngle);}	// current angle setting

	void	Render(__Matrix44& matView, __Matrix44& matProj);
	void	Release();
};
