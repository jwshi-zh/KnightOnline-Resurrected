#include "pch.h"
#include "N3RiverPatch.h"

CN3RiverPatch::CN3RiverPatch()
{
	m_pRivers = nullptr;
	Release();
}

CN3RiverPatch::~CN3RiverPatch()
{
	Release();
}

void CN3RiverPatch::Release()
{
	m_iRiverCount = 0;
	if (m_pRivers) {delete [] m_pRivers; m_pRivers = nullptr;}
}

bool CN3RiverPatch::Load(HANDLE hFile)
{
	Release();
	DWORD dwNum;
	ReadFile(hFile, &m_iRiverCount, sizeof(m_iRiverCount), &dwNum, nullptr);
	__River* pRivers = CreateRiver(m_iRiverCount);
	int i;
	for (i=0; i<m_iRiverCount; ++i)
	{
		__River* pR = pRivers+i;
		ReadFile(hFile, &(pR->iRiverID), sizeof(pR->iRiverID), &dwNum, nullptr);
		ReadFile(hFile, &(pR->iVertexCount), sizeof(pR->iVertexCount), &dwNum, nullptr);
		if (pR->iVertexCount>0)
		{
			pR->pVertices = new __VertexRiver[pR->iVertexCount];
			ReadFile(hFile, pR->pVertices, sizeof(__VertexRiver)*pR->iVertexCount, &dwNum, nullptr);
		}
	}
	return true;
}

bool CN3RiverPatch::Save(HANDLE hFile)
{
	DWORD dwNum;
	WriteFile(hFile, &m_iRiverCount, sizeof(m_iRiverCount), &dwNum, nullptr);
	int i;
	for (i=0; i<m_iRiverCount; ++i)
	{
		__River* pR = m_pRivers+i;
		WriteFile(hFile, &(pR->iRiverID), sizeof(pR->iRiverID), &dwNum, nullptr);
		WriteFile(hFile, &(pR->iVertexCount), sizeof(pR->iVertexCount), &dwNum, nullptr);
		if (pR->iVertexCount>0)
		{
			WriteFile(hFile, pR->pVertices, sizeof(__VertexRiver)*pR->iVertexCount, &dwNum, nullptr);
		}
	}
	return true;
}

__River* CN3RiverPatch::CreateRiver(int iRiverCount)
{
	if (iRiverCount <= 0) return nullptr;
	Release();
	m_iRiverCount = iRiverCount;
	m_pRivers = new __River[m_iRiverCount];

	return m_pRivers;
}

__River* CN3RiverPatch::GetRiverByID(int iRiverID) const
{
	int i;
	for (i=0; i<m_iRiverCount; ++i)
	{
		if (m_pRivers[i].iRiverID == iRiverID)
		{
			return (m_pRivers + i);
		}
	}
	return nullptr;
}
