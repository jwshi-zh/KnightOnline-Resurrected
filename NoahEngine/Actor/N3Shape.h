#pragma once

#include "N3TransformCollision.h"
#include "N3PMeshInstance.h"
#include "N3Texture.h"

#include <vector>

class CN3SPart : public CN3BaseFileAccess
{
	friend class CN3Shape;
public:
	__Vector3	m_vPivot; // Local axis
	__Matrix44	m_Matrix; // World Matrix.. It is good to calculate in advance when loading Shapes..	
	BOOL		m_bOutOfCameraRange; // Out of scope of Camera...

	__Material	m_Mtl; // Material
	float		m_fTexFPS; // Texture Animation Interval;

	float		m_fTimeToSetWind;		// Time to change the windy value..
	float		m_fWindFactorToReach;	// Current wind blowing value.. Rotate with this value..
	float		m_fWindFactorCur;		// windy price...

protected:
	std::vector<CN3Texture*>	m_TexRefs; // Texture Reference Pointers
	CN3PMeshInstance			m_PMInst; // Progressive Mesh Instance

	float	m_fTexIndex;

public:
	virtual bool Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool Save(HANDLE hFile);
#endif // end of _N3TOOL
	
	int			TexCount() const { return m_TexRefs.size(); }
	CN3Texture* Tex(int iIndex)
	{
		if(iIndex < 0 || iIndex >= m_TexRefs.size()) return nullptr;
		return m_TexRefs[iIndex];
	}
	void		TexAlloc(int m_nCount);
	CN3Texture*	TexSet(int iIndex, const std::string& szFN)
	{
		if(iIndex < 0 || iIndex >= m_TexRefs.size()) return nullptr;
		s_MngTex.Delete(&m_TexRefs[iIndex]);
		m_TexRefs[iIndex] = s_MngTex.Get(szFN, true, s_Options.iTexLOD_Shape);
		return m_TexRefs[iIndex];
	}
	void	TexSet(int iIndex, CN3Texture* pTex)
	{
		if(iIndex < 0 || iIndex >= m_TexRefs.size()) return;
		s_MngTex.Delete(&m_TexRefs[iIndex]);
		m_TexRefs[iIndex] = pTex;
	}


	CN3PMeshInstance*	MeshInstance() { return &m_PMInst; } 
	CN3PMesh*			Mesh() { return m_PMInst.GetMesh(); }
	void				MeshSet(const std::string& szFN);

	void ReCalcMatrix(const __Matrix44& mtxParent) { m_Matrix.Identity(); m_Matrix.PosSet(m_vPivot); m_Matrix *= mtxParent; }

	void Tick(const __Matrix44& mtxParent, const __Quaternion& qRot, float fScale);
	void Render() const;
#ifdef _N3TOOL
	void RenderSelected(bool bWireFrame);
	void RenderAxis();
#endif // end of _N3TOOL

	__Vector3 Min() { if(m_PMInst.GetMesh()) return m_PMInst.GetMesh()->Min() * m_Matrix; else return __Vector3(0,0,0); }
	__Vector3 Max() { if(m_PMInst.GetMesh()) return m_PMInst.GetMesh()->Max() * m_Matrix; else return __Vector3(0,0,0); }
	float	Radius() { if(m_PMInst.GetMesh()) return m_PMInst.GetMesh()->Radius(); else return 0.0f; }

	virtual void	Release();
	
	CN3SPart();
	virtual ~CN3SPart();

#ifdef _USE_VERTEXBUFFER
	void			PartialRender(int iCount, LPDIRECT3DINDEXBUFFER9 pIB);
#else
	void			PartialRender(int iCount, WORD* pIndices);
#endif
};

class CN3Shape : public CN3TransformCollision
{
public:
	int		m_iBelong;
	int		m_iEventID;
	int		m_iEventType;
	int		m_iNPC_ID;
	int		m_iNPC_Status;

	bool	m_bDontRender;
	bool	m_bVisible;

	std::vector<CN3SPart*>	m_Parts;

public:
#ifdef _N3TOOL
	bool			SaveToSameFolderAndMore(const std::string& szFullPath, const std::string& szRelativePath);
	bool			SaveToSameFolder(const std::string& szFullPath);
	void			RemoveRenderFlags(int nFlags = -1);
	void			MakeDefaultMaterial();
#endif // end of _N3TOOL

	// Precisely checked in polygon units - Check the box first and then check again precisely..
	int				CheckCollisionPrecisely(bool bIgnoreBoxCheck, int ixScreen, int iyScreen, __Vector3* pVCol = nullptr, __Vector3* pVNormal = nullptr);
	int				CheckCollisionPrecisely(bool bIgnoreBoxCheck, const __Vector3& vPos, const __Vector3& vDir, __Vector3* pVCol = nullptr, __Vector3* pVNormal = nullptr);
	bool			MakeCollisionMeshByParts();
	bool			MakeCollisionMeshByPartsDetail();

	void			FindMinMax();
	virtual void	ReCalcMatrix();
	void			ReCalcPartMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual void	Render();

	CN3SPart*		Part(int iIndex) { if(iIndex < 0 || iIndex >= m_Parts.size()) return nullptr; return m_Parts[iIndex]; }
	CN3SPart*		PartAdd() {
		auto* pPart = new CN3SPart(); m_Parts.push_back(pPart); return pPart; }
	int				PartCount() { return m_Parts.size(); }
	void			PartDelete(int iIndex);
	
	bool			Load(HANDLE hFile);
#ifdef _N3TOOL
	bool			Save(HANDLE hFile);
	void			RenderSelected(bool bWireFrame);
	void			RenderSelected(int iPart, bool bWireFrame);
	bool			IsPMeshProcessed();
#endif // end of _N3TOOL

	virtual void	Release();
	CN3Shape();
	virtual ~CN3Shape();

	void			SetMaxLOD();
	__Matrix44	GetPartMatrix(int iPartIndex);
#ifdef _USE_VERTEXBUFFER
	void			PartialRender(int iPartIndex, int iCount, LPDIRECT3DINDEXBUFFER9 pIB);
#else
	void			PartialRender(int iPartIndex, int iCount, WORD* pIndices);
#endif
	int				GetIndexbufferCount(int iPartIndex);
	int				GetIndexByiOrder(int iPartIndex, int iOrder);
__Vector3		GetVertexByIndex(int iPartIndex, int iIndex); 
	int				GetColIndexbufferCount();
	int				GetColIndexByiOrder(int iOrder);
__Vector3	  GetColVertexByIndex(int iIndex); 
	void			PartialColRender(int iCount, int* piIndices);
	void			PartialGetCollision(int iIndex, __Vector3& vec);
	bool			LoadTransformOnly(HANDLE hFile);
};
