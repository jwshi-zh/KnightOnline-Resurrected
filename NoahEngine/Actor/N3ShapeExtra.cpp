#include "pch.h"
#include "N3ShapeExtra.h"

CN3ShapeExtra::CN3ShapeExtra()
{
	m_dwType |= OBJ_SHAPE_EXTRA;
}

CN3ShapeExtra::~CN3ShapeExtra()
{
	m_Rotations.clear();
}

void CN3ShapeExtra::Release()
{
	CN3Shape::Release();
	
	m_Rotations.clear();
}

bool CN3ShapeExtra::Load(HANDLE hFile)
{
	const bool bSuccess = CN3Shape::Load(hFile);

	const int iPC = m_Parts.size();
	m_Rotations.clear();
	if(iPC <= 0) return bSuccess;

	m_Rotations.reserve(iPC);

	return bSuccess;
}

void CN3ShapeExtra::Tick(float fFrm)
{
	if(false == m_bVisible) 
	{
		m_bDontRender = true;
		return;
	}

	CN3Shape::Tick();
	if(m_bDontRender) return;
	if(m_Parts.empty()) return;

	bool bNeedRemakeCollisionMeshes = false;
	const int iPC = m_Parts.size();
	float fDir = 0;
	__Rotation* pRot = nullptr;
	__Quaternion qRot;
	for(int i = 0; i < iPC; i++)
	{
		CN3SPart* pPart = m_Parts[i];
		if(pPart->m_bOutOfCameraRange) continue;

		__Rotation* pRot = &(m_Rotations[i]);
		if(	pRot->fRadianPerSec == 0 || 
			pRot->fRadianCur == pRot->fRadianToReach) continue;
		(pRot->fRadianCur < pRot->fRadianToReach) ? fDir = 1.0f : fDir = -1.0f;

		const float fRotDelta = pRot->fRadianPerSec * fDir * CN3Base::s_fSecPerFrm;
		pRot->fRadianCur += fRotDelta;
		if(T_Abs(pRot->fRadianToReach - pRot->fRadianCur) <= fRotDelta)
		{
			bNeedRemakeCollisionMeshes = true;
			pRot->fRadianPerSec = 0;
			pRot->fRadianCur = pRot->fRadianToReach;
		}

		qRot.RotationAxis(pRot->vAxis, pRot->fRadianCur);
		pPart->m_Matrix = qRot;
		pPart->m_Matrix.PosSet(pPart->m_vPivot);
		pPart->m_Matrix *= m_Matrix;
	}

	if(bNeedRemakeCollisionMeshes) 
		this->MakeCollisionMeshByParts(); // Recreate the collision mesh.
}

void CN3ShapeExtra::RotateTo(int iPart, const __Vector3& vAxis, float fRadianToReach, float fRadianPerSec, bool bImmediately)
{
	if(iPart < 0 || iPart >= m_Rotations.size()) return;

	__Rotation* pRot = &(m_Rotations[iPart]);
	
	pRot->fRadianToReach = fRadianToReach;
	pRot->fRadianPerSec = fRadianPerSec;
	pRot->vAxis = vAxis;
	if(bImmediately) pRot->fRadianCur = pRot->fRadianToReach - 0.01f; // In this way, it opens directly. The reason for slightly subtracting the value is to make the tick run once.
}