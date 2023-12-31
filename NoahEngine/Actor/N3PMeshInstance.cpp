#include "pch.h"
#include "N3PMeshInstance.h"

// An instance of a mesh. Each version of each mesh that is rendered at a 
// separate level of detail needs one of these.
// Each instance is tied to the original mesh that it represents.
CN3PMeshInstance::CN3PMeshInstance()
{
	m_pPMesh = nullptr;
#ifdef _USE_VERTEXBUFFER
	m_pIB = NULL;
#else
	m_pIndices = nullptr;
#endif
	m_iNumVertices = 0;
	m_iNumIndices  = 0;
	m_pCollapseUpTo = nullptr;
}

CN3PMeshInstance::CN3PMeshInstance(CN3PMesh *pN3PMesh)
{
	m_pPMesh = nullptr;

#ifdef _USE_VERTEXBUFFER
	m_pIB = NULL;
#else
	m_pIndices = nullptr;
#endif
	m_iNumVertices = 0;
	m_iNumIndices  = 0;
	m_pCollapseUpTo = nullptr;

	CN3PMeshInstance::Create(pN3PMesh);
}

CN3PMeshInstance::CN3PMeshInstance(const std::string& szFN)
{
	m_pPMesh = nullptr;

#ifdef _USE_VERTEXBUFFER
	m_pIB = NULL;
#else
	m_pIndices = nullptr;
#endif
	m_iNumVertices = 0;
	m_iNumIndices  = 0;
	m_pCollapseUpTo = nullptr;

	this->Create(szFN);
}

CN3PMeshInstance::~CN3PMeshInstance()
{
#ifdef _USE_VERTEXBUFFER
	if (m_pIB && m_pIB->Release() == 0) m_pIB = NULL;
#else
	delete [] m_pIndices; m_pIndices = nullptr;
#endif
	s_MngPMesh.Delete(&m_pPMesh);
}

void CN3PMeshInstance::Release()
{
#ifdef _USE_VERTEXBUFFER
	if (m_pIB) {m_pIB->Release(); m_pIB = NULL;}
#else
	if (m_pIndices)	{ delete[] m_pIndices;m_pIndices = nullptr;}
#endif

	s_MngPMesh.Delete(&m_pPMesh);
	m_pCollapseUpTo = nullptr;
	m_iNumVertices = 0;
	m_iNumIndices = 0;
}

bool CN3PMeshInstance::Create(CN3PMesh* pN3PMesh)
{
	if(pN3PMesh == nullptr)
	{
		CN3PMeshInstance::Release();
		return false;
	}

	m_pPMesh = pN3PMesh;
	m_szName = pN3PMesh->m_szName;

	// And setup my instance-specific data.
	// We start with the lowest level of detail.
	const int iMaxNumIndices = m_pPMesh->GetMaxNumIndices();
	if (iMaxNumIndices>0)
	{
#ifdef _USE_VERTEXBUFFER
		HRESULT hr = s_lpD3DDev->CreateIndexBuffer(m_pPMesh->m_iMaxNumIndices*sizeof(WORD),
									D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB);
		__ASSERT(SUCCEEDED(hr), "Failed to create index Buffer");

		BYTE* pDestByte, *pSrcByte;
		hr = m_pIB->Lock(0, 0, &pDestByte, 0);
		hr = m_pPMesh->m_pIB->Lock(0, 0, &pSrcByte, 0);

		CopyMemory(pDestByte, pSrcByte, m_pPMesh->m_iMaxNumIndices * sizeof(WORD));
		m_pPMesh->m_pIB->Unlock();
		m_pIB->Unlock();

#else
		if(m_pIndices) delete [] m_pIndices; m_pIndices = nullptr;
		m_pIndices = new WORD[m_pPMesh->m_iMaxNumIndices];
		__ASSERT(m_pIndices, "Failed to create index buffer");
		CopyMemory(m_pIndices, m_pPMesh->m_pIndices, m_pPMesh->m_iMaxNumIndices * sizeof(WORD));
#endif
	}

	// lowest level of detail right now.
	m_iNumVertices = m_pPMesh->GetMinNumVertices();
	m_iNumIndices  = m_pPMesh->GetMinNumIndices();

	// So far, we're at the last collapse
	m_pCollapseUpTo = m_pPMesh->m_pCollapses;

	return true;
}

bool CN3PMeshInstance::Create(const std::string& szFN)
{
	if (m_pPMesh && m_pPMesh->FileName() == szFN) return true;	// If the file name is the same, return without creating a new one
	this->Release();

	CN3PMesh* pN3PMesh = s_MngPMesh.Get(szFN);
	return this->Create(pN3PMesh);
}

void CN3PMeshInstance::SetLODByNumVertices(int iNumVertices)
{
	if(m_pCollapseUpTo == nullptr) return;

	int iDiff = iNumVertices - m_iNumVertices;

	if(iDiff == 0)
	{
		return;
	}
	else if (iDiff > 0)
	{
		while(iNumVertices > m_iNumVertices)
		{
			if (m_pCollapseUpTo->NumVerticesToLose + m_iNumVertices > iNumVertices) break;		// Anti-flicker code..
			if (SplitOne() == false) break;
		}
	}
	else if (iDiff < 0)
	{
		iDiff = -iDiff;
		while(iNumVertices < m_iNumVertices)
		{
			if (CollapseOne() == false) break;
		}
	}

	while(m_pCollapseUpTo->bShouldCollapse)
	{
		if (SplitOne() == false) break;
	}
}

void CN3PMeshInstance::SetLOD(float value)
{
#define _USE_LODCONTROL_VALUE
#ifdef _USE_LODCONTROL_VALUE
	// value is distance * FOV.
	if (m_pPMesh == nullptr) return;

	if (m_pPMesh->m_iLODCtrlValueCount == 0)
	{	// If there is no LODCtrlValue, all are drawn.
		SetLODByNumVertices(0x7fffffff);
		return;
	}
	
	__ASSERT(m_pPMesh->m_pLODCtrlValues, "LOD control value is NULL!");

	CN3PMesh::__LODCtrlValue* pTmpLODCV = m_pPMesh->m_pLODCtrlValues + m_pPMesh->m_iLODCtrlValueCount-1;

	if (value < m_pPMesh->m_pLODCtrlValues[0].fDist)
	{		// Since it is closer than the minimum standard value, it is drawn with the most faces.
		SetLODByNumVertices(m_pPMesh->m_pLODCtrlValues[0].iNumVertices);
	}
	else if ( pTmpLODCV->fDist < value)
	{		// Since it is farther than the maximum standard value, it is drawn with the smallest side.
		SetLODByNumVertices(pTmpLODCV->iNumVertices);
	}
	else
	{		// Draw with the number of sides adjusted for the median value.
		for (int i=1; i< m_pPMesh->m_iLODCtrlValueCount; ++i)
		{
			if (value < m_pPMesh->m_pLODCtrlValues[i].fDist)
			{
				CN3PMesh::__LODCtrlValue* pHiValue = m_pPMesh->m_pLODCtrlValues + i;
				const CN3PMesh::__LODCtrlValue* pLowValue = pHiValue - 1;
				const float fVertices = (pHiValue->iNumVertices - pLowValue->iNumVertices)*
									(value - pLowValue->fDist)/(pHiValue->fDist - pLowValue->fDist);
				SetLODByNumVertices(pLowValue->iNumVertices + (int)fVertices);
				break;
			}
		}
	}
#else
	// value is distance * FOV.
	if (m_pCollapseUpTo == NULL || m_pPMesh == NULL) return;

	const int iLODCtrlValueCount = 5;
	__PMLODCtrlValue LODCtrlValues[iLODCtrlValueCount];
	__PMLODCtrlValue* pTmpLODCV = &(LODCtrlValues[iLODCtrlValueCount-1]);

	int iMaxNumVertices = m_pPMesh->GetMaxNumVertices();
	int iMinNumVertices = m_pPMesh->GetMinNumVertices();
	int iDiff = iMaxNumVertices - iMinNumVertices;
	
	float fVolume = m_pPMesh->GetVolume();
	float fD = (sqrtf(fVolume)*3.0f) / (value * 1.0f);
	fD = 1.0f;
//	float fD = fVolume/(value*13.0f) * (400.0f/(float)iMaxNumVertices);
	if(fD > 1.0f) SetLODByNumVertices(iMaxNumVertices);
	else if(fD < 0.1f) SetLODByNumVertices(iMinNumVertices);
	else SetLODByNumVertices(iMinNumVertices + (int)(iDiff * fD));

#endif
}

bool CN3PMeshInstance::CollapseOne()
{
	if (m_pCollapseUpTo <= m_pPMesh->m_pCollapses) return false;

	m_pCollapseUpTo--;

	m_iNumIndices -= m_pCollapseUpTo->NumIndicesToLose;
	
#ifdef _USE_VERTEXBUFFER
	BYTE* pByte;
	WORD* pIndices;
	HRESULT hr = m_pIB->Lock(0, 0, &pByte, 0);
	pIndices = (WORD*)pByte;

	for (	int *i = m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges;
			i < m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges + m_pCollapseUpTo->NumIndicesToChange;
			i++)
	{
		pIndices[*i] = m_pCollapseUpTo->CollapseTo;
	}
	m_pIB->Unlock();
#else
	for (	int *i = m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges;
			i < m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges + m_pCollapseUpTo->NumIndicesToChange;
			i++)
	{
		m_pIndices[*i] = m_pCollapseUpTo->CollapseTo;
	}
#endif

	m_iNumVertices -= m_pCollapseUpTo->NumVerticesToLose;

	return true;
}

bool CN3PMeshInstance::SplitOne()
{
	if (m_pCollapseUpTo >= m_pPMesh->m_pCollapses + m_pPMesh->m_iNumCollapses) return false; // This will cause one pointer to stick out.
	// But the reason for doing this again is that with the code below, the last polygon is never drawn.
	// I've put some defense code in there so that it's okay to do this. When allocating m_pPMesh->m_pCollapses, I allocated one more and put the last data as the initial value.
//	if (m_pCollapseUpTo >= m_pPMesh->m_pCollapses + m_pPMesh->m_iNumCollapses - 1) return false; // this is normal...

	m_iNumIndices  += m_pCollapseUpTo->NumIndicesToLose;
	m_iNumVertices += m_pCollapseUpTo->NumVerticesToLose;

	if(m_pPMesh->m_pAllIndexChanges)
	{
#ifdef _USE_VERTEXBUFFER
		BYTE* pByte;
		WORD* pIndices;
		__ASSERT(m_pIB, "Index buffer pointer is NULL!");
		HRESULT hr = m_pIB->Lock(0, 0, &pByte, 0);
		pIndices = (WORD*)pByte;

		for (	int *i = m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges;
				i < m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges + m_pCollapseUpTo->NumIndicesToChange;
				i++)
		{
			pIndices[*i] = m_iNumVertices - 1;
		}
		m_pIB->Unlock();
#else
		for (	int *i = m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges;
				i < m_pPMesh->m_pAllIndexChanges + m_pCollapseUpTo->iIndexChanges + m_pCollapseUpTo->NumIndicesToChange;
				i++)
		{
			m_pIndices[*i] = m_iNumVertices - 1;
		}
#endif
	}

	m_pCollapseUpTo++;
	return true;
}

void CN3PMeshInstance::Render() const
{
	if (m_pPMesh == nullptr) return;
	s_lpD3DDev->SetFVF(FVF_VNT1);

	const int iPCToRender = 1000;	// primitive count to render
#ifdef _USE_VERTEXBUFFER
	__ASSERT(m_pPMesh->m_pVB && m_pIB, "Progressive mesh's vertex buffer or index buffer is NULL!");
	s_lpD3DDev->SetStreamSource(0, m_pPMesh->m_pVB, sizeof(__VertexT1));
	s_lpD3DDev->SetIndices(m_pIB, 0);

	if(m_iNumIndices > 3)
	{
		int iPC = m_iNumIndices / 3;

		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumVertices, i*iPCToRender*3, iPCToRender);
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumVertices, i*iPCToRender*3, iRPC);
	}
#else
	if(m_iNumIndices > 3)
	{
		int iPC = m_iNumIndices / 3;

		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iPCToRender, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices, sizeof(__VertexT1));
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iRPC, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices, sizeof(__VertexT1));
	}
#endif
}

void CN3PMeshInstance::RenderTwoUV()
{
	if(nullptr == m_pPMesh) return;
	if(nullptr == m_pPMesh->GetVertices2())
	{
		m_pPMesh->GenerateSecondUV(); // No second UV Create a new one.
	}
	if(nullptr == m_pPMesh->GetVertices2()) return;
	
	s_lpD3DDev->SetFVF(FVF_VNT2);

	const int iPCToRender = 1000;	// primitive count to render
#ifdef _USE_VERTEXBUFFER
	__ASSERT(m_pPMesh->m_pVB && m_pIB, "Progressive mesh's vertex buffer or index buffer is NULL!");
	s_lpD3DDev->SetStreamSource(0, m_pPMesh->m_pVB, sizeof(__VertexT2));
	s_lpD3DDev->SetIndices(m_pIB, 0);

	if(m_iNumIndices > 3)
	{
		int iPC = m_iNumIndices / 3;

		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumVertices, i*iPCToRender*3, iPCToRender);
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumVertices, i*iPCToRender*3, iRPC);
	}
#else
	if(m_iNumIndices > 3)
	{
		int iPC = m_iNumIndices / 3;

		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iPCToRender, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices2, sizeof(__VertexT2));
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iRPC, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices2, sizeof(__VertexT2));
	}
#endif
}

#ifdef _USE_VERTEXBUFFER
LPDIRECT3DVERTEXBUFFER9	CN3PMeshInstance::GetVertexBuffer() const
{
	if (m_pPMesh == NULL) return NULL;
	return m_pPMesh->GetVertexBuffer();
}

#else
__VertexT1*	CN3PMeshInstance::GetVertices() const
{
	if (m_pPMesh == nullptr) return nullptr;
	return m_pPMesh->m_pVertices;
}
#endif

#ifdef _USE_VERTEXBUFFER
void CN3PMeshInstance::PartialRender(int iCount, LPDIRECT3DINDEXBUFFER9 pIB)
{
	if (m_pPMesh == NULL) return;
	s_lpD3DDev->SetFVF(FVF_VNT1);
	const int iPCToRender = 1000;	// primitive count to render

	__ASSERT(m_pPMesh->m_pVB && pIB, "Progressive mesh's vertex buffer or index buffer is NULL!");
	s_lpD3DDev->SetStreamSource(0, m_pPMesh->m_pVB, sizeof(__VertexT1));
	s_lpD3DDev->SetIndices(pIB, 0);

	if(iCount > 3)
	{
		int iPC = iCount / 3;
		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumVertices, i*iPCToRender*3, iPCToRender);
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_iNumVertices, i*iPCToRender*3, iRPC);
}
#else
void CN3PMeshInstance::PartialRender(int iCount, WORD* pIndices)
{
	if (m_pPMesh == nullptr) return;
	s_lpD3DDev->SetFVF(FVF_VNT1);
	const int iPCToRender = 1000;	// primitive count to render

/*	if(m_iNumIndices > 3)
	{
		int iPC = m_iNumIndices / 3;

		int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iPCToRender, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices, sizeof(__VertexT1));
		}

		int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iRPC, m_pIndices + i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices, sizeof(__VertexT1));
	}
*/

	if(iCount > 3)
	{
		const int iPC = iCount / 3;
		const int iLC = iPC / iPCToRender;
		int i;
		for (i=0; i<iLC; ++i)
		{
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iPCToRender, pIndices+i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices, sizeof(__VertexT1));
		}

		const int iRPC = iPC%iPCToRender;
		if(iRPC > 0) s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertices, iRPC, pIndices+i*iPCToRender*3, D3DFMT_INDEX16, m_pPMesh->m_pVertices, sizeof(__VertexT1));
	}
}
#endif

int CN3PMeshInstance::GetIndexByiOrder(int iOrder)
{
	if (iOrder >= GetNumIndices()) 
		return 0;

#ifdef _USE_VERTEXBUFFER
	int	iIndex = 0;	
	BYTE* pByte;
	hr = m_pIB->Lock(0, 0, &pByte, 0);
	LPWORD pIndices = (LPWORD)pByte;
	iIndex = pIndices[iOrder];
	m_pIB->Unlock();
	return iIndex;
#else
	return m_pIndices[iOrder];
#endif
}

__Vector3 CN3PMeshInstance::GetVertexByIndex(int iIndex)
{
	__Vector3 vec; vec.Zero();
	if (iIndex > GetNumVertices())
		return vec;

#ifdef _USE_VERTEXBUFFER
	HRESULT hr;
	__VertexT1* pVB;
	hr = GetMesh()->m_pVB->Lock(0, 0, (void**)&pVB, 0);
	if (FAILED(hr)) 
		return vec;
	vec = (__Vector3)pVB[iIndex];
	GetMesh()->m_pVB->Unlock();
	return vec;
#else
	return GetMesh()->m_pVertices[iIndex];
#endif
}
