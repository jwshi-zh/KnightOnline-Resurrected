#pragma once

#include <list>
#include "N3FXPartBase.h"

class CN3FXParticle;
class CN3FXShape;

class CN3FXPartParticles : public CN3FXPartBase  
{
public:
	//related whole particle system...
	__VertexXyzColorT1			m_vUnit[NUM_VERTEX_PARTICLE];
	//WORD						m_wUnitIB[6];

	__VertexXyzColorT1*			m_pVB;
	//WORD*						m_pIB;

	__Matrix44					m_mtxVI;				//	inverse view mtx..

	int							m_iNumParticle;			//	Total number of particles..(maximum)
	int							m_iNumLodParticle;		//	Number of Particles by Distance
	std::pair<float, float>		m_pair_fParticleSize;
	std::pair<float, float>		m_pair_fParticleLife;
	
	std::list<CN3FXParticle*>	m_pVBList_Alive;
	std::list<CN3FXParticle*>	m_pVBList_Dead;

	float						m_fCreateDelay;
	float						m_CurrCreateDelay;
	int							m_iNumCreate;

	__Vector3					m_MinCreateRange;
	__Vector3					m_MaxCreateRange;

	//emitter...
	DWORD						m_dwEmitType;			//	Launch type..(spread, gather)..
	PARTICLEEMITCONDITION		m_uEmitCon;				//	Required data according to launch type..
	__Vector3					m_vEmitterDir;
	__Vector3					m_vPrevShapePos;
	__Vector3					m_vShapePos;

	//particle..
	__Vector3	m_vPtEmitDir;
	float		m_fPtVelocity;
	float		m_fPtAccel;
	float		m_fPtRotVelocity;
	float		m_fPtGravity;

	DWORD		m_dwChangeColor[NUM_KEY_COLOR];
	bool		m_bChangeColor;

	CN3FXShape*	m_pShape;
	CN3FXShape*	m_pRefShape;
	float		m_fMeshFPS;
	bool		m_bAnimKey;

	float		m_fTexRotateVelocity;		//	Rotate the particle texture on its local Z-axis so it appears to be spinning... (velocity)
	float		m_fScaleVelX;
	float		m_fScaleVelY;

protected:
	void	Rotate() const;
	void	Scaling();
	void	Move();

	void	InitVB();
	void	CreateParticles();
	void	CreateParticles_Spread();
	void	CreateParticles_Gather();

	std::list<CN3FXParticle*>::iterator	DestroyParticle(std::list<CN3FXParticle*>::iterator it);

	bool	IsDead();

	bool RotateQuaternion(__Vector3 vSrcDir, __Vector3 vDestDir, __Quaternion* pQt);
	float CameraDist(__Vector3 v1, __Vector3 v2, __Vector3 v3);

	void	PSort();
	void	PMerge(std::list<CN3FXParticle*>& l1, std::list<CN3FXParticle*>& l2);
	bool	PComp(CN3FXParticle* pP1, CN3FXParticle* pP2);
	
public:
	void	Init();
	void	Start();
	void	Stop();
	bool	Tick();
	void	Render();
	bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);
	void	Duplicate(CN3FXPartParticles* pSrc);

	bool	GetColor(int key, DWORD& color);

	CN3FXPartParticles();	
	virtual ~CN3FXPartParticles();	

#ifdef _N3TOOL
	bool	ParseScript(char* szCommand, char* szBuff0, char* szBuff1, char* szBuff2, char* szBuff3);

	bool	m_bChangeColorKey[NUM_KEY_COLOR];
	bool	m_bChangeAlphaKey[NUM_KEY_COLOR];
#endif // end of _N3TOOL
};
