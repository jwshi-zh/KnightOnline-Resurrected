#include "pch.h"
#include "N3FXPMesh.h"
#include "N3PMesh.h"

CN3FXPMesh::CN3FXPMesh()
{
	m_fRadius = 0.0f;
	m_pColorVertices = nullptr;
	m_pVertices = nullptr;
	m_pVertices2 = nullptr;
	m_pIndices = nullptr;
}

CN3FXPMesh::~CN3FXPMesh()
{
	Release();
}

HRESULT CN3FXPMesh::Create(int iNumVertices, int iNumIndices)
{
	m_iMaxNumVertices = iNumVertices;
	m_iMaxNumIndices = iNumIndices;

	if (m_pVertices) {delete m_pVertices; m_pVertices = nullptr;}
	if (m_pIndices) {delete m_pIndices; m_pIndices = nullptr;}
	if (m_pColorVertices) {delete m_pColorVertices; m_pColorVertices = nullptr;}

	if (m_iMaxNumVertices>0)
	{
		m_pVertices = new __VertexT1[m_iMaxNumVertices];
	}
	if (m_iMaxNumIndices>0)
	{
		m_pIndices = new WORD[m_iMaxNumIndices];
	}
	if (m_iMaxNumVertices>0)
	{
		m_pColorVertices = new __VertexXyzColorT1[m_iMaxNumVertices];
	}

	return S_OK;
}

void CN3FXPMesh::operator=(const CN3FXPMesh& fxPMesh)
{
	if (this == &fxPMesh)
		return;

	Release();

	FileNameSet(fxPMesh.FileName());

	m_iMaxNumVertices	= fxPMesh.m_iMaxNumVertices;
	m_iMaxNumIndices	= fxPMesh.m_iMaxNumIndices;
	m_iMinNumVertices	= fxPMesh.m_iMinNumVertices;
	m_iMinNumIndices	= fxPMesh.m_iMinNumIndices;
	m_iNumCollapses		= fxPMesh.m_iNumCollapses;

	m_fRadius			= fxPMesh.m_fRadius;
	m_vMax				= fxPMesh.m_vMax;
	m_vMin				= fxPMesh.m_vMin;

	m_iLODCtrlValueCount = fxPMesh.m_iLODCtrlValueCount;
	m_iTotalIndexChanges = fxPMesh.m_iTotalIndexChanges;

	Create(m_iMaxNumVertices ,m_iMaxNumIndices);

	memcpy(m_pColorVertices, fxPMesh.m_pColorVertices, sizeof(__VertexXyzColorT1)*m_iMaxNumVertices);
	memcpy(m_pIndices, fxPMesh.m_pIndices, sizeof(WORD)*m_iMaxNumIndices);

	if (m_iNumCollapses>0)
	{
		m_pCollapses = new __EdgeCollapse[m_iNumCollapses+1];	// Reason for +1: There was a case where the PMeshInstance::SplitOne() function inevitably required the pointer to point to the boundary line.
		memcpy(m_pCollapses, fxPMesh.m_pCollapses, sizeof(__EdgeCollapse)*(m_iMaxNumIndices+1));
	}

	m_pLODCtrlValues = new __LODCtrlValue[m_iLODCtrlValueCount];
	memcpy(m_pLODCtrlValues, fxPMesh.m_pLODCtrlValues, sizeof(__LODCtrlValue)*m_iLODCtrlValueCount);

	m_pAllIndexChanges = new int[m_iTotalIndexChanges];
	memcpy(m_pAllIndexChanges, fxPMesh.m_pAllIndexChanges, sizeof(int)*m_iTotalIndexChanges);
}

bool CN3FXPMesh::Load(HANDLE hFile)
{
	CN3BaseFileAccess::Load(hFile);

	DWORD dwNum;
	ReadFile(hFile, &m_iNumCollapses, sizeof(m_iNumCollapses), &dwNum, nullptr);
	ReadFile(hFile, &m_iTotalIndexChanges, sizeof(m_iTotalIndexChanges), &dwNum, nullptr);

	ReadFile(hFile, &m_iMaxNumVertices, sizeof(int), &dwNum, nullptr);
	ReadFile(hFile, &m_iMaxNumIndices , sizeof(int), &dwNum, nullptr);
	ReadFile(hFile, &m_iMinNumVertices, sizeof(int), &dwNum, nullptr);
	ReadFile(hFile, &m_iMinNumIndices , sizeof(int), &dwNum, nullptr);

	const HRESULT hr = Create(m_iMaxNumVertices, m_iMaxNumIndices);
	__ASSERT(SUCCEEDED(hr), "Failed to create progressive mesh");

	if (m_iMaxNumVertices>0)
	{
		ReadFile(hFile, m_pVertices, m_iMaxNumVertices*sizeof(__VertexT1), &dwNum, nullptr);
		for(int i=0;i<m_iMaxNumVertices;i++)
		{
			m_pColorVertices[i].x = m_pVertices[i].x;
			m_pColorVertices[i].y = m_pVertices[i].y;
			m_pColorVertices[i].z = m_pVertices[i].z;
			m_pColorVertices[i].color = 0xffffffff;
			m_pColorVertices[i].tu = m_pVertices[i].tu;
			m_pColorVertices[i].tv = m_pVertices[i].tv;			
		}

		delete[] m_pVertices;
		m_pVertices = nullptr;
	}

	if (m_iMaxNumIndices>0)
		ReadFile(hFile, m_pIndices, m_iMaxNumIndices*sizeof(WORD), &dwNum, nullptr);

	if (m_iNumCollapses>0)
	{
		m_pCollapses = new __EdgeCollapse[m_iNumCollapses+1];	// Reason for +1: There was a case where the PMeshInstance::SplitOne() function inevitably required the pointer to point to the boundary line.
		ReadFile(hFile, m_pCollapses, m_iNumCollapses*sizeof(__EdgeCollapse), &dwNum, nullptr);
		ZeroMemory(m_pCollapses + m_iNumCollapses, sizeof(__EdgeCollapse));

		bool bFixed = false;
		for(int i = 0; i < m_iNumCollapses; i++)
		{
			if(m_pCollapses[i].iIndexChanges < 0)
			{
				m_pCollapses[i].iIndexChanges = 0;
				bFixed = true;
			}
		}
#ifdef _DEBUG
		if(bFixed)
			::MessageBox(s_hWndBase, "잘못된 Progressive Mesh 수정", m_szName.c_str(), MB_OK);
#endif
	}
	if (m_iTotalIndexChanges>0)
	{
		m_pAllIndexChanges = new int[m_iTotalIndexChanges];
		ReadFile(hFile, m_pAllIndexChanges, m_iTotalIndexChanges*sizeof(int), &dwNum, nullptr);
	}

	__ASSERT(m_pLODCtrlValues == NULL && m_iLODCtrlValueCount == 0, "Invalid Level of detail control value");
	ReadFile(hFile, &m_iLODCtrlValueCount, sizeof(m_iLODCtrlValueCount), &dwNum, nullptr);
	if (m_iLODCtrlValueCount>0)
	{
		m_pLODCtrlValues = new __LODCtrlValue[m_iLODCtrlValueCount];
		ReadFile(hFile, m_pLODCtrlValues, m_iLODCtrlValueCount*sizeof(__LODCtrlValue), &dwNum, nullptr);
	}

	FindMinMax();

	return true;
}

void CN3FXPMesh::Release()
{
	CN3PMesh::Release();

	if (m_pVertices) {delete m_pVertices; m_pVertices = nullptr;}
	if (m_pIndices) {delete m_pIndices; m_pIndices = nullptr;}
	if (m_pColorVertices) {delete m_pColorVertices; m_pColorVertices = nullptr;}
}


void CN3FXPMesh::Render() const
{
	s_lpD3DDev->SetFVF(FVF_XYZCOLORT1);

	const int iPCToRender = 1000;
	if(m_iMaxNumIndices > 3)
	{
		const int iPC = m_iMaxNumIndices / 3;

		const int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iMaxNumVertices, iPCToRender, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pColorVertices, sizeof(__VertexXyzColorT1));
		}

		const int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iMaxNumVertices, iRPC, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pColorVertices, sizeof(__VertexXyzColorT1));
	}
}

void CN3FXPMesh::FindMinMax()
{
	if(m_iMaxNumVertices <= 0)
	{
		m_vMin.Zero();
		m_vMax.Zero();
		m_fRadius = 0;
		return;
	}

	m_vMin.Set(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vMax.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(int i = 0; i < m_iMaxNumVertices; i++)
	{
		if(m_pColorVertices[i].x < m_vMin.x) m_vMin.x = m_pColorVertices[i].x;
		if(m_pColorVertices[i].y < m_vMin.y) m_vMin.y = m_pColorVertices[i].y;
		if(m_pColorVertices[i].z < m_vMin.z) m_vMin.z = m_pColorVertices[i].z;

		if(m_pColorVertices[i].x > m_vMax.x) m_vMax.x = m_pColorVertices[i].x;
		if(m_pColorVertices[i].y > m_vMax.y) m_vMax.y = m_pColorVertices[i].y;
		if(m_pColorVertices[i].z > m_vMax.z) m_vMax.z = m_pColorVertices[i].z;
	}

	m_fRadius  = (m_vMax - m_vMin).Magnitude() * 0.5f;
}

void CN3FXPMesh::SetColor(DWORD dwColor)
{
	if (m_pColorVertices == nullptr)	return;
	if (m_iMaxNumVertices <= 0)		return;

	for(int i=0;i<m_iMaxNumVertices;i++)
	{
		m_pColorVertices[i].color = dwColor;
	}
}
