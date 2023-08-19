#pragma once

//#define _USE_VERTEXBUFFER

#include "N3BaseFileAccess.h"

class CN3PMesh : public CN3BaseFileAccess
{
friend class CN3FXPMeshInstance;
friend class CN3PMeshInstance;
friend class CN3PMeshCreate;
friend class CPMeshEditor;

public:
	struct  __LODCtrlValue
	{
		float	fDist;
		int		iNumVertices;
	};

	// A single edge collapse / vertex split operation.
	struct __EdgeCollapse
	{
	//	float Value;
		int NumIndicesToLose, NumIndicesToChange, NumVerticesToLose;
		//int *IndexChanges;
		int iIndexChanges;	// store the nth instead of a pointer (0, 1, 2,...)
		int CollapseTo;
		bool	bShouldCollapse;	// If you stop merging/dividing here, a hole will be drilled. You need to take the next step further.
	};

protected:

	//int m_iNumMaterials;
	int m_iNumCollapses;			// triangle merge list
	int m_iTotalIndexChanges;
	int *m_pAllIndexChanges; // All the index changes

	// The mesh is an array of materials, and an edge collapse list. The mesh
	// is saved in its lowest level of detail

	__EdgeCollapse	*m_pCollapses;

#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;
#else
	__VertexT1*		m_pVertices;
	__VertexT2*		m_pVertices2;
	WORD*			m_pIndices;
#endif
	int m_iMaxNumVertices, m_iMaxNumIndices;
	int m_iMinNumVertices, m_iMinNumIndices;

	__Vector3	m_vMin, m_vMax;
	float		m_fRadius;

	// LOD adjustment information.. (The number of vertices can be adjusted according to the distance.)
	int					m_iLODCtrlValueCount;
	__LODCtrlValue*		m_pLODCtrlValues;
protected:
#ifdef _N3TOOL
	static int SortByDistance(const void *pArg1, const void *pArg2);
#endif // end of _N3TOOL

public:
#ifdef _N3TOOL
	void ReGenerateSharpNormal();
	void ReGenerateSmoothNormal();
	int LODCtrlCount() { return m_iLODCtrlValueCount; }
	void LODCtrlSet(__LODCtrlValue* pLODCtrls, int nCount);
	__LODCtrlValue* LODCtrlGet(int index) { if(index < 0 || index >= m_iLODCtrlValueCount) return NULL; return &m_pLODCtrlValues[index]; }
	int CollapsesCount() { return m_iNumCollapses; }
#endif // end of _N3TOOL

//	int					GetLODCtrlValueCount() const {return m_iLODCtrlValueCount;}
//	__PMLODCtrlValue*	GetLODCtrlValue(int iIndex) const {if (iIndex>=0&&m_iLODCtrlValueCount>iIndex)return m_pLODCtrlValues+iIndex; return NULL;}
//	void				SetLODCtrlValues(__PMLODCtrlValue* pValues) {m_pLODCtrlValues = pValues;}

	__Vector3 Min() { return m_vMin; }
	__Vector3 Max() { return m_vMax; }

	void Release();
	HRESULT Create(int iNumVertices, int iNumIndices);
	HRESULT GenerateSecondUV();
	
	bool Load(HANDLE hFile);
#ifdef _N3TOOL
	bool Save(HANDLE hFile);
#endif // end of _N3TOOL
	
	void FindMinMax();

	float Radius() const { return m_fRadius; }
	int	GetMinNumVertices() const {return m_iMinNumVertices;};
	int	GetMaxNumVertices() const {return m_iMaxNumVertices;};
	int GetMaxNumIndices() const {return m_iMaxNumIndices;};
	int GetMinNumIndices() const {return m_iMinNumIndices;};

#ifdef _USE_VERTEXBUFFER
	LPDIRECT3DVERTEXBUFFER9	GetVertexBuffer() const {return m_pVB;}
	LPDIRECT3DINDEXBUFFER9	GetIndexBuffer() const {return m_pIB;}
#else
	__VertexT1*		GetVertices() const { return m_pVertices; };
	__VertexT2*		GetVertices2() const { return m_pVertices2; };
	WORD*			GetIndices() const { return m_pIndices;};
	// (To get a proper index, create an N3PMeshInstance and get the index value after adjusting the LOD.)
#endif

#ifdef _N3TOOL
	void CopyMesh(CN3PMesh* pSrcPMesh);
#endif // end of _N3TOOL

	CN3PMesh();
	virtual ~CN3PMesh();
};
