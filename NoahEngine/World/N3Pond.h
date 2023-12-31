#pragma once

#define MAX_PONDMESH_LINE		200
#define MAX_PONDMESH_VERTEX		200*4
#define MAX_POND_TEX	32

#include "N3BaseFileAccess.h"

class CN3Pond : public CN3BaseFileAccess
{
public:
	CN3Pond();
	virtual		~CN3Pond();

	struct __VertexPond
	{
	public:
		float x,y,z;
		float nx,ny,nz;
		D3DCOLOR	color;
		float u,v, u2,v2;
		void Set(float sx, float sy,float sz, float snx, float sny, float snz, D3DCOLOR scolor, float su, float sv, float su2, float sv2)
		{
			x = sx, y = sy, z = sz;
			nx = snx, ny = sny, nz = snz;
			color = scolor;
			u = su, v = sv;
			u2 = su2, v2 = sv2;
		}
	};

	class CPongMesh
	{
	public:
		CN3Texture*		m_pTexWave;
		BOOL			m_bTick2Rand;	// Comes into view and decides to execute tick and rend
		__VertexPond*	m_pVertices;	// Vertices
		float*			m_pfVelocityArray;	// save calculation
		float			m_pfMaxHeight;	// Prevents waves from rising above a certain level
		WORD*			m_wpIndex;		// order of drawing
		int				m_iIC;			// Index Buffer Count.
		int				m_iVC;			// Vertex Count.

		int				m_iWidthVtx,m_iHeightVtx;		// required for calculation
		float			m_fmin,m_fmax,m_fmaxcal,m_fmincal;

		__Vector3		m_vCenterPo;	// midpoint of the pond
		float			m_fRadius;		// diameter of pond

		CPongMesh() {m_bTick2Rand=FALSE;m_pVertices= nullptr,m_wpIndex= nullptr;m_pfVelocityArray= nullptr;m_pTexWave= nullptr;};
		virtual ~CPongMesh() 
		{
			if(m_pVertices) delete []m_pVertices;m_pVertices= nullptr;
			if(m_wpIndex) delete []m_wpIndex;m_wpIndex= nullptr;
			if(m_pfVelocityArray) delete []m_pfVelocityArray;m_pfVelocityArray= nullptr;
			if(m_pTexWave)	{CN3Base::s_MngTex.Delete(&m_pTexWave); m_pTexWave = nullptr;}
		};
	};

public:
	int				m_iPondMeshNum;	// total number of ponds
	CPongMesh*		m_pCPondMesh;	// information of the pond

	CN3Texture*		m_pTexPond[MAX_POND_TEX];	
	float			m_fTexIndex;

	int				m_iMaxVtxNum;	// highest number of vertices
	float*			m_pfMaxVtx;		// Temporary for wave height calculation

public:
	void		Release();
	bool		Load(HANDLE hFile);
	void		Render() const;
	void		Tick();

private:
	void		CheckHeight(float& ChkHeight)
	{
		if ( ChkHeight < -0.01f )	ChkHeight +=  0.01f;
		else if ( ChkHeight >  0.01f )	ChkHeight += -0.01f;
	};

	void		UpdateWaterPositions();
};
