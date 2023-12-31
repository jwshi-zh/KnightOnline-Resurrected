#include "pch.h"
#include "N3FXShape.h"

CN3FXSPart::CN3FXSPart()
{
	m_vPivot.Set(0,0,0);
	m_WorldMtx.Identity();

	m_bOutOfCameraRange = TRUE;

	m_fTexFPS = 10.0f;
	m_fTexIndex = 0;
	m_TexRefs.clear();

	m_bTexLoop = true;
	
	m_Mtl.Init();

//	m_pPM = NULL;

	m_pRefShape = nullptr;
}

CN3FXSPart::~CN3FXSPart()
{
	const int iTC = m_TexRefs.size();
	for(int i = 0; i < iTC; i++) s_MngTex.Delete(&m_TexRefs[i]);

//	if(m_pPM) { m_pPM->Release(); delete m_pPM; m_pPM = NULL; }
}

void CN3FXSPart::Release()
{
	m_vPivot.Set(0,0,0);
	m_WorldMtx.Identity();	
	m_bOutOfCameraRange = TRUE;

	m_fTexFPS = 10.0f;
	m_fTexIndex = 0;

	const int iTC = m_TexRefs.size();
	for(int i = 0; i < iTC; i++) s_MngTex.Delete(&m_TexRefs[i]);
	m_TexRefs.clear();

//	if(m_pPM) { m_pPM->Release(); delete m_pPM; m_pPM = NULL; }
	m_FXPMInst.Release();

}

void CN3FXSPart::TexAlloc(int nCount)
{
	if(nCount <= 0) return;

	const int iTC = m_TexRefs.size();
	for(int i = 0; i < iTC; i++) s_MngTex.Delete(&m_TexRefs[i]);
	m_TexRefs.clear();

	m_TexRefs.assign(nCount, nullptr);
}

CN3Texture* CN3FXSPart::Tex(int iIndex) const
{
	if(iIndex < 0 || iIndex >= m_TexRefs.size()) return nullptr;
	return m_TexRefs[iIndex];
}

CN3Texture* CN3FXSPart::TexSet(int iIndex, const std::string& szFN)
{
	if(iIndex < 0 || iIndex >= m_TexRefs.size()) return nullptr;
	s_MngTex.Delete(&m_TexRefs[iIndex]);
	m_TexRefs[iIndex] = s_MngTex.Get(szFN);
	return m_TexRefs[iIndex];
}

void CN3FXSPart::TexSet(int iIndex, CN3Texture* pTex)
{
	if(iIndex < 0 || iIndex >= m_TexRefs.size()) return;
	s_MngTex.Delete(&m_TexRefs[iIndex]);
}

void CN3FXSPart::Tick(const __Matrix44& mtxParent) 
{
	const CN3FXPMesh* pFXPMesh = m_FXPMInst.GetMesh();
	if(nullptr == pFXPMesh) return;

	m_bOutOfCameraRange = FALSE;

	m_WorldMtx.Identity();
	m_WorldMtx.PosSet(m_vPivot);
	m_WorldMtx *= mtxParent;

	const float fDist = (m_WorldMtx.Pos() - s_CameraData.vEye).Magnitude();
	const float fRadius = Radius();
	if(s_CameraData.IsOutOfFrustum(this->m_WorldMtx.Pos(), fRadius * 3.0f))
	{
		m_bOutOfCameraRange = TRUE;
		return;
	}

	const float fLOD = fDist * s_CameraData.fFOV;
	m_FXPMInst.SetLOD(fLOD);


	const int iTC = m_TexRefs.size();
	if(iTC > 1) m_fTexIndex += CN3Base::s_fSecPerFrm * m_fTexFPS;
}

void CN3FXSPart::Render()
{
	if(m_bOutOfCameraRange) return;
	if(m_bOutOfCameraRange || m_FXPMInst.GetNumVertices() <= 0) return;
	
#ifdef _DEBUG
	CN3Base::s_RenderInfo.nShape_Part++;
#endif
	
	LPDIRECT3DTEXTURE9 lpTex = nullptr;
	const int iTC = m_TexRefs.size();
	if(iTC > 0)
	{
		int iTexIndex = (int)m_fTexIndex;
		if(m_bTexLoop) iTexIndex = (int)m_fTexIndex % iTC;
		if(iTexIndex >= 0 && iTexIndex < iTC && m_TexRefs[iTexIndex]) lpTex = m_TexRefs[iTexIndex]->Get();
		else return;
	}

	if(m_Mtl.nRenderFlags & RF_ALPHABLENDING)
	{
		__AlphaPrimitive* pAP = s_AlphaMgr.Add();
		if(pAP)
		{
			pAP->bUseVB				= FALSE;
			pAP->dwBlendDest		= m_Mtl.dwDestBlend;
			pAP->dwBlendSrc			= m_Mtl.dwSrcBlend;
			pAP->dwFVF				= FVF_XYZCOLORT1;
			pAP->dwPrimitiveSize	= sizeof(__VertexXyzColorT1);
			pAP->fCameraDistance	= (s_CameraData.vEye - (this->Min() + (this->Max()-this->Min())*0.5f)).Magnitude();
			pAP->lpTex				= lpTex;
			pAP->ePrimitiveType		= D3DPT_TRIANGLELIST;
			pAP->nPrimitiveCount	= m_FXPMInst.GetNumIndices() / 3;
			//pAP->nRenderFlags		= RF_ALPHABLENDING | RF_NOTUSEFOG | RF_DIFFUSEALPHA | RF_NOTUSELIGHT | RF_DOUBLESIDED;
			pAP->nRenderFlags		= m_Mtl.nRenderFlags;
			pAP->nVertexCount		= m_FXPMInst.GetNumVertices();
			pAP->MtxWorld			= m_WorldMtx;
			pAP->pVertices			= m_FXPMInst.GetVertices();
			pAP->pwIndices			= m_FXPMInst.GetIndices();
		}
		return;
	}

	s_lpD3DDev->SetMaterial(&m_Mtl);
	s_lpD3DDev->SetTexture(0, lpTex);
	if(nullptr != lpTex)
	{
		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );		
		s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );		
	}
	else
	{
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	}

	__Matrix44 mtx;
	s_lpD3DDev->GetTransform(D3DTS_WORLD, &mtx);
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &m_WorldMtx);

	DWORD dwCullMode, dwZWriteEnable, dwZBufferEnable, dwLight;
	s_lpD3DDev->GetRenderState( D3DRS_ZWRITEENABLE, &dwZWriteEnable );
	s_lpD3DDev->GetRenderState( D3DRS_ZENABLE, &dwZBufferEnable );
	s_lpD3DDev->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
	s_lpD3DDev->GetRenderState( D3DRS_LIGHTING, &dwLight );

	if(m_pRefShape->m_dwZEnable != dwZBufferEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, m_pRefShape->m_dwZEnable);
	if(m_pRefShape->m_dwZWrite != dwZWriteEnable) s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, m_pRefShape->m_dwZWrite);
	if(m_pRefShape->m_dwLight != dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, m_pRefShape->m_dwLight);
	if(m_pRefShape->m_dwDoubleSide != dwCullMode) s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, m_pRefShape->m_dwDoubleSide);
	
	m_FXPMInst.Render();

	if(m_pRefShape->m_dwZEnable != dwZBufferEnable) s_lpD3DDev->SetRenderState(D3DRS_ZENABLE, dwZBufferEnable);
	if(m_pRefShape->m_dwZWrite != dwZWriteEnable) s_lpD3DDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWriteEnable);
	if(m_pRefShape->m_dwLight != dwLight) s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
	if(m_pRefShape->m_dwDoubleSide != dwCullMode) s_lpD3DDev->SetRenderState(D3DRS_CULLMODE, dwCullMode);

	s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtx);
}

bool CN3FXSPart::Load(HANDLE hFile)
{
	DWORD dwRWC;
	int nL = 0;
	char szFN[256];

	ReadFile(hFile, &m_vPivot, sizeof(__Vector3), &dwRWC, nullptr);

	ReadFile(hFile, &nL, 4, &dwRWC, nullptr);
	ReadFile(hFile, szFN, nL, &dwRWC, nullptr); szFN[nL] = NULL;

	//Combine the path of m_pRefShape with the name of the file read...
	char szPath[_MAX_PATH];
	char szFName[_MAX_FNAME], szExt[_MAX_EXT];
	char szDir[_MAX_DIR];
	_splitpath(m_pRefShape->FileName().c_str(), nullptr, szDir, nullptr, nullptr);
	_splitpath(szFN, nullptr, nullptr, szFName, szExt);
	_makepath(szPath, nullptr, szDir, szFName, szExt);

	if(!this->MeshSet(szPath)) return false;

	ReadFile(hFile, &m_Mtl, sizeof(__Material), &dwRWC, nullptr);

	int iTC = 0;
	ReadFile(hFile, &iTC, 4, &dwRWC, nullptr);
	ReadFile(hFile, &m_fTexFPS, 4, &dwRWC, nullptr);
	m_TexRefs.clear();
	this->TexAlloc(iTC);
	for(int j = 0; j < iTC; j++)
	{
		ReadFile(hFile, &nL, 4, &dwRWC, nullptr);
		if(nL > 0)
		{
			ReadFile(hFile, szFN, nL, &dwRWC, nullptr); szFN[nL] = NULL;
			
			_splitpath(szFN, nullptr, nullptr, szFName, szExt);
			_makepath(szPath, nullptr, szDir, szFName, szExt);
			m_TexRefs[j] = s_MngTex.Get(szPath);
		}
	}

	return true;
}

void CN3FXSPart::Duplicate(CN3FXSPart* pSrc)
{
	m_vPivot = pSrc->m_vPivot;
	if(pSrc->Mesh()) MeshSet(pSrc->Mesh()->FileName());
	
	m_Mtl = pSrc->m_Mtl;

	const int iTC = pSrc->TexCount();
	m_fTexFPS = m_fTexFPS;

	m_TexRefs.clear();	
	this->TexAlloc(iTC);
	for(int j = 0; j < iTC; j++)
	{
		if(pSrc->Tex(j))
			m_TexRefs[j] = s_MngTex.Get(pSrc->Tex(j)->FileName());
	}
	return;
}

bool CN3FXSPart::Save(HANDLE hFile)
{	
	return true;
}

bool CN3FXSPart::MeshSet(const std::string& szFN)
{
	m_FXPMInst.Create(szFN);
	return true;
}

CN3FXShape::CN3FXShape()
{
	m_dwType |= OBJ_SHAPE;
	m_mtxParent.Identity();
	m_mtxFinalTransform.Identity();

	m_dwSrcBlend = D3DBLEND_ONE;
	m_dwDestBlend = D3DBLEND_ONE;
	m_bAlpha = TRUE;

	m_dwZEnable = D3DZB_TRUE;
	m_dwZWrite = TRUE;
	m_dwLight = FALSE;
	m_dwDoubleSide = D3DCULL_NONE;
}

CN3FXShape::~CN3FXShape()
{
	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		m_Parts[i]->Release();
		delete m_Parts[i];		
	}
	m_Parts.clear();
}

void CN3FXShape::Release()
{
	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		m_Parts[i]->Release();
		delete m_Parts[i];
	}
	m_Parts.clear();
	
	CN3TransformCollision::Release();
}

void CN3FXShape::Tick(float fFrm)
{
	CN3TransformCollision::Tick(m_fFrmCur);
	m_mtxFinalTransform = CN3Transform::m_Matrix * m_mtxParent;

	CN3FXSPart* pPD = nullptr;
	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		m_Parts[i]->Tick(m_mtxFinalTransform);
	}
}

void CN3FXShape::Render()
{
	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		m_Parts[i]->Render();
	}
}

bool CN3FXShape::Load(HANDLE hFile)
{
	CN3TransformCollision::Load(hFile);

	DWORD dwRWC = 0;
	
	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++) delete m_Parts[i];
	m_Parts.clear();

	ReadFile(hFile, &iPC, 4, &dwRWC, nullptr);
	if(iPC > 0)
	{
		m_Parts.assign(iPC, nullptr);
		for(int i = 0; i < iPC; i++)
		{
			m_Parts[i] = new CN3FXSPart();
			m_Parts[i]->m_pRefShape = this;
			if(!m_Parts[i]->Load(hFile)) return false;
			//m_Parts[i]->ReCalcMatrix(m_Matrix);
		}
	}

	DWORD dwTmp;		
	ReadFile(hFile, &dwTmp, 4, &dwRWC, nullptr);
	ReadFile(hFile, &dwTmp, 4, &dwRWC, nullptr);
	ReadFile(hFile, &dwTmp, 4, &dwRWC, nullptr);
	ReadFile(hFile, &dwTmp, 4, &dwRWC, nullptr);
	ReadFile(hFile, &dwTmp, 4, &dwRWC, nullptr);

	this->FindMinMax();

	return true;
}

bool CN3FXShape::Save(HANDLE hFile)
{
	/*
	CN3TransformCollision::Save(hFile);
	
	DWORD dwRWC = 0;
	
	int nL = 0;
	
	CN3SPart* pPD = NULL;
	int iPC = m_Parts.size();
	WriteFile(hFile, &iPC, 4, &dwRWC, NULL); // Mesh FileName
	for(int i = 0; i < iPC; i++)
	{
		m_Parts[i]->Save(hFile);
	}

	WriteFile(hFile, &m_iBelong, 4, &dwRWC, NULL);
	WriteFile(hFile, &m_iAttr0, 4, &dwRWC, NULL);
	WriteFile(hFile, &m_iAttr1, 4, &dwRWC, NULL);
	WriteFile(hFile, &m_iAttr2, 4, &dwRWC, NULL);
	WriteFile(hFile, &m_iAttr3, 4, &dwRWC, NULL);
	*/
	return true;
}

void CN3FXShape::PartDelete(int iIndex)
{
	if(iIndex < 0 || iIndex >= m_Parts.size()) return;
	auto it = m_Parts.begin();

	for(int i = 0; i < iIndex; i++, it++);
	delete *it;
	m_Parts.erase(it);
}

__Vector3 CN3FXShape::CenterPos()
{
	FindMinMax();
	return (m_vMin + (m_vMax - m_vMin) * 0.5f);
}

void CN3FXShape::FindMinMax()
{
	__Vector3 vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	__Vector3 vMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	static __Matrix44 mtxWI;
	D3DXMatrixInverse(&mtxWI, nullptr, &m_mtxFinalTransform);
	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		const __Vector3 vMinTmp = m_Parts[i]->Min() * mtxWI;
		const __Vector3 vMaxTmp = m_Parts[i]->Max() * mtxWI;

		if(vMinTmp.x < vMin.x) vMin.x = vMinTmp.x;
		if(vMinTmp.y < vMin.y) vMin.y = vMinTmp.y;
		if(vMinTmp.z < vMin.z) vMin.z = vMinTmp.z;
		if(vMaxTmp.x > vMax.x) vMax.x = vMaxTmp.x;
		if(vMaxTmp.y > vMax.y) vMax.y = vMaxTmp.y;
		if(vMaxTmp.z > vMax.z) vMax.z = vMaxTmp.z;
	}

	m_vMin = vMin * m_mtxFinalTransform;
	m_vMax = vMax * m_mtxFinalTransform;

	m_fRadius  = (m_vMax - m_vMin).Magnitude() * 0.5f;
}

void CN3FXShape::Duplicate(CN3FXShape* pSrc)
{
	if(!pSrc) return;

	m_dwSrcBlend = pSrc->m_dwSrcBlend;
	m_dwDestBlend = pSrc->m_dwDestBlend;
	m_bAlpha = pSrc->m_bAlpha;

	m_dwZEnable = pSrc->m_dwZEnable;
	m_dwZWrite = pSrc->m_dwZWrite;
	m_dwLight = pSrc->m_dwLight;
	m_dwDoubleSide = pSrc->m_dwDoubleSide;

	SetRadius(pSrc->Radius());
	SetMin(pSrc->Min());
	SetMax(pSrc->Max());

	if(pSrc->CollisionMesh()) SetMeshCollision(pSrc->CollisionMesh()->FileName());
	if(pSrc->ClimbMesh()) SetMeshClimb(pSrc->ClimbMesh()->FileName());

	ScaleSet(pSrc->Scale());
	PosSet(pSrc->m_vPos);
	RotSet(pSrc->Rot());
	
	FileNameSet(pSrc->FileName());

	m_Matrix = pSrc->m_Matrix;

	m_KeyPos.Duplicate(&(pSrc->m_KeyPos));
	m_KeyRot.Duplicate(&(pSrc->m_KeyRot));
	m_KeyScale.Duplicate(&(pSrc->m_KeyScale));

	m_fFrmWhole = pSrc->m_fFrmWhole;
	m_fFrmCur = 0.0f;

	int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++) delete m_Parts[i];
	m_Parts.clear();

	iPC = pSrc->m_Parts.size();
	if(iPC > 0)
	{
		m_Parts.assign(iPC, nullptr);
		for(int i = 0; i < iPC; i++)
		{
			m_Parts[i] = new CN3FXSPart();
			m_Parts[i]->m_pRefShape = this;
			m_Parts[i]->Duplicate(pSrc->m_Parts[i]);
		}
	}

	return;
}

void CN3FXShape::SetCurrFrm(float fFrm)
{
	if(FRAME_SELFPLAY == fFrm)
	{
		m_fFrmCur += s_fSecPerFrm;
		if(m_fFrmCur < 0) m_fFrmCur = 0.0f;
		if(m_fFrmCur >= m_fFrmWhole) m_fFrmCur = 0.0f;		
	}
	else m_fFrmCur = fFrm;
}

float CN3FXShape::GetCurrFrm()
{
	return m_fFrmCur;
}

void CN3FXShape::SetPartsMtl(BOOL bAlpha, DWORD dwSrcBlend, DWORD dwDestBlend, DWORD dwZEnable, DWORD dwZWrite, DWORD dwLight, DWORD dwDoubleSide)
{
	m_dwSrcBlend = dwSrcBlend;
	m_dwDestBlend = dwDestBlend;
	m_bAlpha = bAlpha;

	m_dwZEnable = dwZEnable;
	m_dwZWrite = dwZWrite;
	m_dwLight = dwLight;
	m_dwDoubleSide = dwDoubleSide;

	DWORD dwRenderFlag = RF_ALPHABLENDING | RF_NOTUSEFOG | RF_DIFFUSEALPHA | RF_NOTUSELIGHT | RF_DOUBLESIDED | RF_NOTZWRITE | RF_NOTZBUFFER; 
	if(m_dwZEnable == D3DZB_TRUE) dwRenderFlag ^= RF_NOTZBUFFER;
	if(m_dwZWrite == TRUE) dwRenderFlag ^= RF_NOTZWRITE;
	if(m_dwDoubleSide != D3DCULL_NONE) dwRenderFlag ^= RF_DOUBLESIDED;
	if(m_dwLight == TRUE) dwRenderFlag ^= RF_NOTUSELIGHT;
	if(m_bAlpha != TRUE) dwRenderFlag ^= RF_ALPHABLENDING;

	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++) m_Parts[i]->m_Mtl.nRenderFlags = dwRenderFlag;
}

void CN3FXShape::SetMaxLOD()
{
	const int iPC = m_Parts.size();
	for(int i = 0; i < iPC; i++)
	{
		m_Parts[i]->m_bOutOfCameraRange = FALSE;
		m_Parts[i]->m_FXPMInst.SetLOD(0);
	}
}
