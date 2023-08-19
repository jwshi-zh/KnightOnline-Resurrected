#pragma once

#include "N3TransformCollision.h"
#include "N3Texture.h"
#include "N3FXPMesh.h"
#include "N3FXPMeshInstance.h"

#include <vector>

class CN3FXSPart : public CN3BaseFileAccess
{
friend class CN3FXShape;

public:
	__Material	m_Mtl;					// Material
	__Vector3	m_vPivot;				// Local
	__Matrix44	m_WorldMtx;				// World Matrix.. It is good to calculate in advance when loading the shape.	
	BOOL		m_bOutOfCameraRange;	// Out of scope of Camera...

	float		m_fTexFPS;				// Texture Animation Interval;
	bool		m_bTexLoop;

	CN3FXShape*	m_pRefShape;

protected:
	std::vector<CN3Texture*>	m_TexRefs;		// Texture Reference Pointers
	CN3FXPMeshInstance			m_FXPMInst;		// Progressive Mesh Instance

	float						m_fTexIndex;	// Current Texture Index.. This is the index necessary for animation. Controls texture animation with a float.

public:
	bool Load(HANDLE hFile);
	bool Save(HANDLE hFile);
	void Duplicate(CN3FXSPart* pSrc);
		
	int			TexCount() const { return m_TexRefs.size(); }
	CN3Texture* Tex(int iIndex) const;	
	void		TexAlloc(int m_nCount);
	CN3Texture*	TexSet(int iIndex, const std::string& szFN);
	void		TexSet(int iIndex, CN3Texture* pTex);

	__Vector3 Min() { if(m_FXPMInst.GetMesh()) return m_FXPMInst.GetMesh()->Min() * m_WorldMtx; else return __Vector3(0,0,0); }
	__Vector3 Max() { if(m_FXPMInst.GetMesh()) return m_FXPMInst.GetMesh()->Max() * m_WorldMtx; else return __Vector3(0,0,0); }
	float	Radius() { if(m_FXPMInst.GetMesh()) return m_FXPMInst.GetMesh()->Radius(); else return 0.0f; }


	CN3FXPMesh*	Mesh() { return m_FXPMInst.GetMesh(); }
	__VertexXyzColorT1* GetColorVertices() { return m_FXPMInst.GetVertices(); }
	void	SetColor(DWORD dwColor = 0xffffffff) { m_FXPMInst.SetColor(dwColor); }
	bool	MeshSet(const std::string& szFN);
	void	Tick(const __Matrix44& mtxParent);
	void	Render();
	void	Release();
	
	CN3FXSPart();
	virtual ~CN3FXSPart();
};

class CN3FXShape : public CN3TransformCollision
{
public:
	std::vector<CN3FXSPart*>	m_Parts;

	__Matrix44		m_mtxParent;
	__Matrix44		m_mtxFinalTransform;

	DWORD			m_dwSrcBlend;
	DWORD			m_dwDestBlend;
	BOOL			m_bAlpha;

	DWORD			m_dwZEnable;
	DWORD			m_dwZWrite;
	DWORD			m_dwLight;
	DWORD			m_dwDoubleSide;
	
public:
	void			SetPartsMtl(BOOL bAlpha, DWORD dwSrcBlend, DWORD dwDestBlend, DWORD dwZEnable, DWORD dwZWrite, DWORD dwLight, DWORD dwDoubleSide);
	__Vector3		CenterPos();

	void			SetCurrFrm(float fFrm);
	float			GetCurrFrm();
	float			GetWholeFrm() { return m_fFrmWhole; }

	void			FindMinMax();

	CN3FXSPart*		Part(int iIndex) { if(iIndex < 0 || iIndex >= m_Parts.size()) return nullptr; return m_Parts[iIndex]; }
	CN3FXSPart*		PartAdd() {
		auto* pPart = new CN3FXSPart(); m_Parts.push_back(pPart); return pPart; }
	int				PartCount() { return m_Parts.size(); }
	void			PartDelete(int iIndex);
	
	bool	Load(HANDLE hFile);
	bool	Save(HANDLE hFile);
	void	Duplicate(CN3FXShape* pSrc);

	void	Tick(float fFrm = FRAME_SELFPLAY);
	void	Render();
	void	Release();		

public:
	void SetMaxLOD();
	CN3FXShape();
	virtual ~CN3FXShape();
};
