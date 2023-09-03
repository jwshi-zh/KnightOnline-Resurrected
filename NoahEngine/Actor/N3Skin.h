#pragma once

#include "N3IMesh.h"

struct __VertexSkinned
{
	__Vector3	vOrigin; // original point location.
	int			nAffect; // How many joints are affected?
	int*		pnJoints; // joint index
	float*		pfWeights; // weight

	__VertexSkinned() { memset(this, 0, sizeof(__VertexSkinned)); }
	~__VertexSkinned() { delete [] pnJoints; delete [] pfWeights; }
};

class CN3Skin : public CN3IMesh
{
protected:
#ifdef _N3TOOL
	struct __Weight
	{
		int nIndex;
		float fWeight;
	};
	static int SortWeightsProc(const void* pArg1, const void* pArg2);
#endif // end of _N3TOOL
	
	__VertexSkinned*	m_pSkinVertices;

public:
	__VertexSkinned*	SkinVertices() const { return m_pSkinVertices; }
	bool				Create(int nFC, int nVC, int nUVC);
	bool				Load(HANDLE hFile);

	bool				CheckCollisionPrecisely(const __Vector3 &vPos, const __Vector3 &vDir, __Vector3* pvPick);

#ifdef _N3TOOL
	bool				Save(HANDLE hFile);
	void				RecalcWeight();		// Recalculate weights
#endif // end of _N3TOOL
	
	void Release();
	CN3Skin();
	virtual ~CN3Skin();

};
