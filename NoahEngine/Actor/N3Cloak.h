#pragma once

#include "N3Base.h"
#include "N3Texture.h"

#define CLOAK_MAX_WIDTH		7
#define CLOAK_MAX_HEIGHT	7
#define CLOAK_SKIP_LINE		2	// The number of lines in the cape that contain decorative vertices. This part does not move. ok?

class CN3PMesh;

enum e_Cloak_AnchorMovePattern {AMP_NONE=0, AMP_YAWCCW, AMP_YAWCW, AMP_MOVEX, AMP_MOVEY, AMP_MOVEZ, AMP_MOVEXZ, AMP_MOVEXZ2};

class CPlayerBase;
class CN3CPlug_Cloak;

class CN3Cloak : public CN3Base  
{
public:
	CN3Cloak();
	virtual ~CN3Cloak();

	class __Particle
	{
	public:
		__Particle()
		{
			memset(this, NULL, sizeof(__Particle));
		}
		~__Particle(){}

		float			x, y, z;
		float			vx, vy, vz;
		D3DXVECTOR3		LocalForce;
		float			mass;
		void Set(float mass1, float x1, float y1, float z1, float vx1, float vy1, float vz1)
		{
			mass = mass1;
			x = x1, y = y1, z=z1;
			vx = vx1, vy = vy1, vz = vz1;
		}
	};

	void				Init(CN3CPlug_Cloak *pPlugCloak);	
	void				SetPlayerBase(CPlayerBase *pBase){m_bpPlayerBase = pBase;}	
	void				SetLOD(int nLevel);
	void				ApplyOffset(D3DXVECTOR3	&vDif);
protected:
	//	Anchor
	e_Cloak_AnchorMovePattern	m_eAnchorPattern;
	float				m_fAnchorPreserveTime;
	D3DXVECTOR3			m_vOffset[CLOAK_MAX_WIDTH];

	void				RestoreAnchorLine();
	void				MoveAnchorLine(e_Cloak_AnchorMovePattern eType, float fPreserveTime);

	//
	CN3Texture			*m_pTex;
	__VertexT1*			m_pVertex;
	WORD				*m_pIndex;
	int					m_nVertexCount, m_nIndexCount;

	int					m_nGridW, m_nGridH;
	int					m_nLOD;

	CPlayerBase			*m_bpPlayerBase;
	CN3PMesh			*m_pPMesh;
	float				m_fOffsetRecoveryTime;
	float				m_fPrevYaw;
	
	
	__Particle			*m_pParticle;	
	D3DXVECTOR3			m_GravityForce;		// Gravity (.y) must always be present so that deformation does not occur.
	D3DXVECTOR3			m_Force;			// force applied from outside.
	

	void				UpdateLocalForce() const;	
	void				ApplyForce();
	void				TickYaw();
	void				TickByPlayerMotion();
public:
	virtual void Tick(int nLOD);
	virtual void Render(__Matrix44 &mtx);
	virtual void Release();
};
