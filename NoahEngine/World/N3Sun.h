#pragma once

#include "N3Base.h"
#include "N3ColorChange.h"

enum	eSUNPART {SUNPART_SUN=0, SUNPART_GLOW, SUNPART_FLARE, NUM_SUNPART};

class CN3Texture;
class CN3Sun : public CN3Base  
{
	friend class CN3SkyMng;
public:
	CN3Sun();
	virtual ~CN3Sun();
protected:
	struct __SunPart
	{
		CN3Texture*				pTex;			// texture
		__VertexTransformed		pVertices[4];	// vertex

		CN3ColorChange			Color;			// ���� ����ϴ� Ŭ����
		CN3DeltaChange			Delta;
	};

	__SunPart					m_Parts[NUM_SUNPART];
	float						m_fCurRadian;		// ���� ���� ȸ����ġ
// Operations
public:
	void	SetCurAngle(float fAngle)	{m_fCurRadian = D3DXToRadian(fAngle);}	// ���� ��������
	void	Init(const std::string* pszFNs);
//	By : Ecli666 ( On 2002-04-04 ���� 10:55:52 )
//
	float	GetCurAngle()	{	return D3DXToDegree(m_fCurRadian);	}
//	~(By Ecli666 On 2002-04-04 ���� 10:55:52 )

	void Release();
	void Render(__Matrix44& matView, __Matrix44& matProj);
	void Tick();
protected:

};
