#pragma once

#include "N3Base.h"

#define MAX_RIVER_TEX	32

class CN3River2 : public CN3Base  
{
public:
	CN3River2();
	virtual ~CN3River2();

	struct __VertexRiver
	{
	public:
		float x,y,z;
		float nx,ny,nz;
		D3DCOLOR	color;
		float u,v, u2,v2;
		void Set(float sx, float sy,float sz, float snx, float sny, float snz, D3DCOLOR scolor, float su, float sv, float su2, float sv2)
		{
			x = sx, y = sy, z = sz;
			nx = snx, y = sny, z = sny;
			color = scolor;
			u = su, v = sv;
			u2 = su2, v2 = sv2;
		}
	};

	struct _RIVER_DIFF
	{
		float	fDiff;
		float	fWeight;
	};

	struct _RIVER_INFO
	{
		int iVC;
		int iIC;
		__VertexRiver		*pVertices;
		WORD				*pwIndex;
		_RIVER_DIFF			*pDiff;

		BOOL				m_bTick2Rand;
		__Vector3			m_vCenterPo;	// midpoint of the river
		float				m_fRadius;		// diameter of river

		_RIVER_INFO() {
			pVertices = nullptr; pwIndex = nullptr; pDiff = nullptr;
		}
		~_RIVER_INFO() {
			if (pVertices)
				delete[] pVertices, pVertices = nullptr;
			if (pwIndex)
				delete[] pwIndex, pwIndex = nullptr;
			if (pDiff)
				delete[] pDiff, pDiff = nullptr;
		}
	};

protected:
	_RIVER_INFO			*m_pRiverInfo;
	int					m_iRiverCount;

	CN3Texture			*m_pTexRiver[MAX_RIVER_TEX];
	CN3Texture			*m_pTexWave;


	float				m_fTexIndex;
	void				UpdateWaterPositions();
public:
	virtual bool	Load(HANDLE hFile);
	void			Render() const;
	void			Tick();
	void			Init(const std::string& TexPath);
};
