#include "pch.h"
#include "N3ShapeMod.h"

CN3ShapeMod::CN3ShapeMod()
{
	m_pModParts = nullptr;
	m_pMatchPart2ModPart = nullptr;
	Release();
}

CN3ShapeMod::~CN3ShapeMod()
{
	Release();
}

void CN3ShapeMod::Release()
{
	if (m_pModParts) {delete [] m_pModParts; m_pModParts = nullptr;}
	m_iStateCount = 0;
	m_iCurState = 0;	m_iPrevState = 0;
	m_iModPartCount = 0;
	if (m_pMatchPart2ModPart) {delete [] m_pMatchPart2ModPart; m_pMatchPart2ModPart = nullptr;}
//	m_iModType = 0;
	m_fTimeChanged = 0.0f;
	m_fTimeChanging = 0.0f;
}

CN3SPart* CN3ShapeMod::GetPartByPMeshFileName(const std::string& szFN) const
{
	if(szFN.empty()) return nullptr;

	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		if (m_Parts[i]->Mesh() && m_Parts[i]->Mesh()->FileName() == szFN) return m_Parts[i];
	}
	return nullptr;
}

bool CN3ShapeMod::Load(HANDLE hFile)
{
	const bool ret = CN3Shape::Load(hFile);

	char szPathName[_MAX_PATH];
	char szDir[_MAX_DIR];
	char szFName[_MAX_FNAME];
	_splitpath(m_szFileName.c_str(), nullptr, szDir, szFName, nullptr);
	_makepath(szPathName, nullptr, szDir, szFName, "txt");
	FILE* stream = fopen(szPathName, "r");
	LoadStateInfo(stream);
	fclose(stream);

	return ret;
}

BOOL CN3ShapeMod::LoadStateInfo(FILE* stream)	// Read state information (from text)
{
	__ASSERT(m_Parts.size() > 0, "먼저 shape를 load해야 한다.");
	if (nullptr == stream) return FALSE;
//	Release();

	int result;
//	char szSrcName[_MAX_PATH];
//	result = fscanf(stream, "Shape_FName=%s\n", szSrcName);	__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");

	// shape load
//	if (!this->Load(szSrcName)) return FALSE;

	// The number of changing PMesh and the number of states
	result = fscanf(stream, "PMesh_Count=%d\n", &m_iModPartCount);	__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
	result = fscanf(stream, "State_Count=%d\n", &m_iStateCount);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");

	// Read Shape State Information
	int i, j;
	__Vector3 vPos, vScale;
	__Vector3 vAxis;	float fDegree;
	__Quaternion qRot;
//	m_ModShape.pShapeStateInfos = new __ModPosRotScale[m_iStateCount];
//	for (i=0; i<m_iStateCount; ++i)
//	{
//		result = fscanf(stream, "S_Pos(%f, %f, %f)\n", &vPos.x, &vPos.y, &vPos.z);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
//		result = fscanf(stream, "S_Rot(%f, %f, %f)\n", &vRot.x, &vRot.y, &vRot.z);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
//		result = fscanf(stream, "S_Scale(%f, %f, %f)\n", &vScale.x, &vScale.y, &vScale.z);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
//		m_ModShape.pShapeStateInfos[i].vPos = vPos;
//		m_ModShape.pShapeStateInfos[i].vRot = vRot;
//		m_ModShape.pShapeStateInfos[i].vScale = vScale;
//	}

	// Read Part Status Information
	__ASSERT(m_iModPartCount > 0, "m_iModPartCount must be greater than or equal to 1.");
	__ASSERT(m_iStateCount > 0, "m_iStateCount must be greater than or equal to 1.");
	m_pModParts = new __ModPart[m_iModPartCount];

	char szPMeshName[_MAX_PATH] = "";
	for (i=0; i<m_iModPartCount; ++i)
	{
		result = fscanf(stream, "PMesh_FName=%s\n", szPMeshName);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
		m_pModParts[i].pPart = GetPartByPMeshFileName(szPMeshName);
		m_pModParts[i].pStateInfos = new __ModPosRotScale[m_iStateCount];
		for (j=0; j<m_iStateCount; ++j)
		{
			result = fscanf(stream, "Pos(%f, %f, %f)\n", &vPos.x, &vPos.y, &vPos.z);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
			result = fscanf(stream, "Rot(%f, %f, %f, %f)\n", &vAxis.x, &vAxis.y, &vAxis.z, &fDegree);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
			if(vAxis.x ==0.0f && vAxis.y == 0.0f && vAxis.z == 0.0f) qRot.Identity();
			else qRot.RotationAxis(vAxis, D3DXToRadian(fDegree));
			result = fscanf(stream, "Scale(%f, %f, %f)\n", &vScale.x, &vScale.y, &vScale.z);		__ASSERT(result != EOF, "Invalid N3ShapeMod settings file");
			m_pModParts[i].pStateInfos[j].vPos = vPos;
			m_pModParts[i].pStateInfos[j].qRot = qRot;
			m_pModParts[i].pStateInfos[j].vScale = vScale;
			if (0==j)
			{
				m_pModParts[i].CurStateInfo.vPos = vPos;
				m_pModParts[i].CurStateInfo.qRot = qRot;
				m_pModParts[i].CurStateInfo.vScale = vScale;
			}
			if (vPos.x != 0.0f || vPos.y != 0.0f || vPos.z != 0.0f) m_pModParts[i].bPos = true;
			if (qRot.w != 0.0f) m_pModParts[i].bRot = true;
			if (vScale.x != 1.0f || vScale.y != 1.0f || vScale.z != 1.0f) m_pModParts[i].bScale = true;
		}
	}

	// m_pMatchPart2ModPart Creates and initializes as many as the number of CN3SParts (matching)
	__ASSERT(m_pMatchPart2ModPart == NULL, "Possible memory leak in the machine");
	const int iPartCount = PartCount();
	if (iPartCount>0) m_pMatchPart2ModPart = new __ModPart*[iPartCount];
	ZeroMemory(m_pMatchPart2ModPart, sizeof(m_pMatchPart2ModPart[0])*iPartCount);
	// (matching)
	for(auto i =0; i<m_iModPartCount; ++i)
	{
		auto it = m_Parts.begin();
		const int iPC = m_Parts.size();
		for(int j = 0; j < iPC; j++)
		{
			if (m_Parts[j] == m_pModParts[i].pPart)
			{
				m_pMatchPart2ModPart[j] = m_pModParts + i;
				break;
			}
		}
	}

	return TRUE;
}

void CN3ShapeMod::ReCalcMatrix()
{
	CN3Transform::ReCalcMatrix();

	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		if (m_pMatchPart2ModPart[i])
		{
			const __ModPart* pModPart = m_pMatchPart2ModPart[i];
			
			m_Parts[i]->m_Matrix.Scale(pModPart->CurStateInfo.vScale);
			
			static __Matrix44 mtxRot;
			D3DXMatrixRotationQuaternion(&mtxRot, &(pModPart->CurStateInfo.qRot));
			m_Parts[i]->m_Matrix *= mtxRot;

			m_Parts[i]->m_Matrix.PosSet(m_Parts[i]->m_vPivot + pModPart->CurStateInfo.vPos);
			m_Parts[i]->m_Matrix *= m_Matrix;
		}
		else
		{
			m_Parts[i]->ReCalcMatrix(m_Matrix);
		}
	}
}

void CN3ShapeMod::Tick(float fFrm)
{
	CN3Shape::Tick(fFrm);
	if (m_bDontRender) return;

	int i;
	if (m_fTimeChanged > 0.0f)
	{
		m_fTimeChanging += s_fSecPerFrm;
		if (m_fTimeChanging > m_fTimeChanged)
		{
			m_fTimeChanged = 0.0f;
			m_fTimeChanging = 0.0f;
			m_iPrevState = m_iCurState;

			for (i=0; i<m_iModPartCount; ++i)
			{
				m_pModParts[i].CurStateInfo = m_pModParts[i].pStateInfos[m_iCurState];
			}
		}
		else
		{
			__ModPart* pMP = nullptr;
			for (i=0; i<m_iModPartCount; ++i)
			{
				pMP = m_pModParts + i;
				if (pMP->bPos)
				{
					pMP->CurStateInfo.vPos = pMP->pStateInfos[m_iPrevState].vPos + 
											(pMP->pStateInfos[m_iCurState].vPos - pMP->pStateInfos[m_iPrevState].vPos)*m_fTimeChanging/m_fTimeChanged;
				}
				if (pMP->bRot)
				{
					pMP->CurStateInfo.qRot.Slerp(pMP->pStateInfos[m_iPrevState].qRot, pMP->pStateInfos[m_iCurState].qRot, m_fTimeChanging/m_fTimeChanged);
				}
				if (pMP->bScale)
				{
					pMP->CurStateInfo.vScale = pMP->pStateInfos[m_iPrevState].vScale + 
											(pMP->pStateInfos[m_iCurState].vScale - pMP->pStateInfos[m_iPrevState].vScale)*m_fTimeChanging/m_fTimeChanged;
				}
			}
		}
	}

	ReCalcMatrix();
}

BOOL CN3ShapeMod::SetState(int iState, float fSec)
{
	if (iState<0 || iState>= m_iStateCount) return FALSE;
	if (m_iCurState == iState) return FALSE;
	m_fTimeChanged = fSec;
	m_fTimeChanging = 0.0f;

	m_iPrevState = m_iCurState;
	m_iCurState = iState;
	return TRUE;
}
