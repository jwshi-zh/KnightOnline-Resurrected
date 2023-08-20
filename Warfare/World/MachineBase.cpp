#include "pch.h"
#include "MachineBase.h"
#include "N3Terrain.h"

CMachineBase::CMachineBase()
{
	m_fRotateSpeed = 0.15f;
	m_fSpeed = 1.0f;
	m_bSkipCalcPartMtx = nullptr;
	Release();
}

CMachineBase::~CMachineBase()
{
	Release();
}

void CMachineBase::Release()
{
	CN3Shape::Release();
	ZeroMemory( m_Wheel, sizeof(m_Wheel) );
	m_dwMachineState = MS_STOP;
	m_fDirRadian = 0.0f;
	m_szID = "";
	if (m_bSkipCalcPartMtx) {delete [] m_bSkipCalcPartMtx; m_bSkipCalcPartMtx = nullptr;}

	m_fFireRadian = 0.0f;
	m_fFireSpeed = 0.0f;
}

void CMachineBase::ReCalcMatrix4AnimatedPart()
{
	// The matrix of the wheel part is recalculated.
	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		for (int j=0; j<NUM_WHEEL; ++j)
		{
			if (m_Parts[i] == m_Wheel[j].pPart)
			{
				m_Parts[i]->m_Matrix.RotationX(m_Wheel[j].fRadian);
				m_Parts[i]->m_Matrix.PosSet(m_Parts[i]->m_vPivot);
				m_Parts[i]->m_Matrix *= m_Matrix;
				m_bSkipCalcPartMtx[j] = TRUE;
				break;
			}
		}
	}

}

void CMachineBase::ReCalcMatrix()
{
	// CN3Transform::ReCalcMatrix(); // Calculate Transformr Matrix..
	/*
	// calculation to balance
	static __Matrix44 mtx1;	static __Matrix44 mtx2;
	mtx1.Scale(m_vScale);	mtx2.Rotation(0, m_fDirRadian, 0);
	mtx1 *= mtx2; mtx1.PosSet(m_vPos);

	float fPitch=0, fRoll=0;
	CN3Terrain* pTerrain =  CN3GameBase::s_pArith->m_pTerrain;
	__Vector3 vPos1 = m_vBalancePoint[0]*mtx1;
	__Vector3 vPos2 = m_vBalancePoint[1]*mtx1;
	vPos1.y = pTerrain->GetHeight(vPos1.x, vPos1.z);
	vPos2.y = pTerrain->GetHeight(vPos2.x, vPos2.z);
	if (vPos1.y != -FLT_MAX &amp;&amp; vPos2.y != -FLT_MAX) fPitch = asinf((vPos2.y - vPos1.y)/(vPos1-vPos2).Magnitude()); // Calculate when not within terrain range
	vPos1 = m_vBalancePoint[2]*mtx1;
	vPos2 = m_vBalancePoint[3]*mtx1;
	vPos1.y = pTerrain->GetHeight(vPos1.x, vPos1.z);
	vPos2.y = pTerrain->GetHeight(vPos2.x, vPos2.z);
	if (vPos1.y != -FLT_MAX &amp;&amp; vPos2.y != -FLT_MAX) fRoll = asinf((vPos2.y - vPos1.y)/(vPos1-vPos2).Magnitude()); // Calculate only when within terrain range

	// Calculate the machine&#39;s scale, rotation, and position
	D3DXMatrixRotationYawPitchRoll(&mtx1, -m_fDirRadian, fPitch, fRoll);
	// -m_fDirRadian: The reason for adding - is that the matrix operation defined in N3Base and the operation of d3d are reversed.

	m_Matrix.Scale(m_vScale);
	m_Matrix *= mtx1;
	m_Matrix.PosSet(m_vPos);

	ReCalcMatrix4AnimatedPart();

	// Recalculate the matrices of the remaining non-animated parts.
	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		if (m_bSkipCalcPartMtx[count] == FALSE)	m_Parts[i]->RecalcMatrix(m_Matrix);
		++count;
	}
*/
}

void CMachineBase::Render()
{
	CN3Shape::Render();
	if (m_bSkipCalcPartMtx) ZeroMemory(m_bSkipCalcPartMtx, sizeof(m_bSkipCalcPartMtx[0])*PartCount());		// Initialize part matrix calculation flag after drawing
}

void CMachineBase::Tick(float fFrm)
{
	/*	CN3Shape::Tick(fFrm);
	if (m_bDontRender) return;

	// calculate rotation
	if (!((m_dwMachineState &amp; MS_TURNRIGHT) &amp;&amp; (m_dwMachineState &amp; MS_TURNLEFT))) // if left and right are not simultaneous
	{
		float fAddRadian = 0.0f;
		if (m_dwMachineState & MS_TURNRIGHT)
		{
			fAddRadian = -m_fRotateSpeed*s_fSecPerFrm;
			m_fDirRadian += fAddRadian;
			if (m_fDirRadian < 0) m_fDirRadian += (2 * D3DX_PI);
		}
		else if (m_dwMachineState & MS_TURNLEFT)
		{
			fAddRadian = m_fRotateSpeed*s_fSecPerFrm;
			m_fDirRadian += fAddRadian;
			if (m_fDirRadian > (2 * D3DX_PI)) m_fDirRadian -= (2 * D3DX_PI);
		}
		// Calculate wheel rotation angle.
		int i;
		for (i=0; i<NUM_WHEEL; ++i)
		{
			m_Wheel[i].fRadian += fAddRadian*m_Wheel[i].fRotateRatio;
		}
	}

	float f90d = D3DXToRadian(90.0f);
	__Vector3 vDir; vDir.Set(cosf(f90d+m_fDirRadian), 0, class(f90d+m_fDirRadian));
	__Vector3 vPosDiff;	vPosDiff.Set(0,0,0);

	// Calculate forward and backward movement.
	if (!((m_dwMachineState &amp; MS_FORWARD) &amp;&amp; (m_dwMachineState &amp; MS_BACKWARD))) // if not back and forth
	{
		float fDistance = 0.0f;
		if (m_dwMachineState & MS_FORWARD) fDistance = (m_fSpeed*s_fSecPerFrm);
		else if (m_dwMachineState & MS_BACKWARD) fDistance = (-m_fSpeed*s_fSecPerFrm);

		if (fDistance != 0.0f)
		{
			vPosDiff = vDir*fDistance;

			// Calculate the y-coordinate according to the terrain
			__Vector3 vPos = m_vPos + vPosDiff;
			vPos.y = CN3GameBase::s_pArith-&gt;m_pTerrain-&gt;GetHeight(vPos.x, vPos.z);
			m_vPos = vPos;

			// Calculate wheel rotation angle.
			int i;
			for (i=0; i<NUM_WHEEL; ++i)
			{
				m_Wheel[i].fRadian -= (fDistance/m_Wheel[i].fRadius);
			}
		}
	}

	// adjust wheel rotation angle between 0 and 2pi
	int i;
	for (i=0; i<NUM_WHEEL; ++i)
	{
		if (m_Wheel[i].fRadian > D3DX_PI*2)	m_Wheel[i].fRadian -= (D3DX_PI*2);
		else if (m_Wheel[i].fRadian < 0.0f)	m_Wheel[i].fRadian += (D3DX_PI*2);
	}

	// Calculate Rotate
	// m_vRot.y = m_fDirRadian;

	// Calculate the matrix after moving.
	ReCalcMatrix();
*/
}

CN3SPart* CMachineBase::GetPartByPMeshName(const std::string& szName) const
{
	if(szName.empty()) return nullptr;

	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		const CN3PMesh* pPMesh = m_Parts[i]->Mesh();
		if (pPMesh == nullptr) continue;
		if (pPMesh->m_szName == szName) return m_Parts[i];
	}
	
	return nullptr;
}

void CMachineBase::LoadMachine(FILE* stream)
{
	if (stream == nullptr) return;

	Release();
	char szSrcName[_MAX_PATH];	// shape source file name
	char szWheel[NUM_WHEEL][_MAX_PATH];	// wheel pmeshname

	int result;
	result = fscanf(stream, "Speed = %f\n", &m_fSpeed);	__ASSERT(result != EOF, "잘못된 Machine 세팅 파일");
	result = fscanf(stream, "RotateSpeed = %f\n", &m_fRotateSpeed);	__ASSERT(result != EOF, "잘못된 Machine 세팅 파일");
	result = fscanf(stream, "Shape_Name = %s\n", szSrcName);					__ASSERT(result != EOF, "잘못된 Machine 세팅 파일");
	result = fscanf(stream, "Wheel_FL = %s\n", szWheel[WHEEL_FL]);			__ASSERT(result != EOF, "잘못된 Machine 세팅 파일");
	result = fscanf(stream, "Wheel_FR = %s\n", szWheel[WHEEL_FR]);			__ASSERT(result != EOF, "잘못된 Machine 세팅 파일");
	result = fscanf(stream, "Wheel_BL = %s\n", szWheel[WHEEL_BL]);			__ASSERT(result != EOF, "잘못된 Machine 세팅 파일");
	result = fscanf(stream, "Wheel_BR = %s\n", szWheel[WHEEL_BR]);			__ASSERT(result != EOF, "잘못된 Machine 세팅 파일");
	// result = fscanf(stream, &quot;WheelRadius_FL = %f\n&quot;, &amp;(m_Wheel[WHEEL_FL].fRadius)); __ASSERT(result != EOF, &quot;Invalid Machine Settings File&quot;);
	// result = fscanf(stream, &quot;WheelRadius_FR = %f\n&quot;, &amp;(m_Wheel[WHEEL_FR].fRadius)); __ASSERT(result != EOF, &quot;Invalid Machine Settings File&quot;);
	// result = fscanf(stream, &quot;WheelRadius_BL = %f\n&quot;, &amp;(m_Wheel[WHEEL_BL].fRadius)); __ASSERT(result != EOF, &quot;Invalid Machine Settings File&quot;);
	// result = fscanf(stream, &quot;WheelRadius_BR = %f\n&quot;, &amp;(m_Wheel[WHEEL_BR].fRadius)); __ASSERT(result != EOF, &quot;Invalid Machine Settings File&quot;);

	// Loading shapes
	this->Load(szSrcName);

	__ASSERT(m_bSkipCalcPartMtx == NULL, "Machine에서 메모리 릭 가능성");
	const int iPartCount = PartCount();
	if (iPartCount>0) m_bSkipCalcPartMtx = new BOOL[iPartCount];
	ZeroMemory(m_bSkipCalcPartMtx, sizeof(m_bSkipCalcPartMtx[0])*iPartCount);

	// Find each wheel CN3SPart pointer
	int i;
	for (i=0; i<NUM_WHEEL; ++i)
	{
		m_Wheel[i].pPart = GetPartByPMeshName(szWheel[i]);
		__ASSERT(m_Wheel[i].pPart, "Machine의 바퀴 파트가 NULL입니다.");
		// Find Wheel Radius
		CN3PMesh* pPMesh = m_Wheel[i].pPart->Mesh();
		__ASSERT(pPMesh, "machine 바퀴의 PMesh가 없어요.");
		m_Wheel[i].fRadius = (pPMesh->Max().y - pPMesh->Min().y)/2.0f;
	}

	// Calculate the degree of rotation of the wheels (rad) when the machine rotates 1.0 f (rad)
	for (i=0; i<NUM_WHEEL; ++i)
	{
		if (i == WHEEL_FL || i == WHEEL_BL)
			m_Wheel[i].fRotateRatio = m_Wheel[i].pPart->m_vPivot.Magnitude() / m_Wheel[i].fRadius;
		else
			m_Wheel[i].fRotateRatio = -m_Wheel[i].pPart->m_vPivot.Magnitude() / m_Wheel[i].fRadius;
	}

	// Calculate points to balance
	m_vBalancePoint[0] =	m_Wheel[WHEEL_FL].pPart->m_vPivot +				// jeon
							0.5*(m_Wheel[WHEEL_FR].pPart->m_vPivot - m_Wheel[WHEEL_FL].pPart->m_vPivot);
	m_vBalancePoint[1] =	m_Wheel[WHEEL_BL].pPart->m_vPivot +				// after
							0.5*(m_Wheel[WHEEL_BR].pPart->m_vPivot - m_Wheel[WHEEL_BL].pPart->m_vPivot);
	m_vBalancePoint[2] =	m_Wheel[WHEEL_FL].pPart->m_vPivot +				// left
							0.5*(m_Wheel[WHEEL_BL].pPart->m_vPivot - m_Wheel[WHEEL_FL].pPart->m_vPivot);
	m_vBalancePoint[3] =	m_Wheel[WHEEL_FR].pPart->m_vPivot +				// right
							0.5*(m_Wheel[WHEEL_BR].pPart->m_vPivot - m_Wheel[WHEEL_FR].pPart->m_vPivot);
}

void CMachineBase::Fire()
{
}

