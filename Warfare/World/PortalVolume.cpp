#include "pch.h"
#include "PortalVolume.h"
#include "GameBase.h"
#include "PlayerMySelf.h"

#include "PvsMgr.h"

#include "N3ShapeMgr.h"
#include "N3ShapeExtra.h"

CPortalVolume::CPortalVolume()	: m_fOffs(0.001f), m_fHeightOffs(0.01f), m_fVolOffs(0.001f), m_fPickIncline(0.6f), m_fCameraOffs(0.4f)
{
	m_pvVertex[0].Set(-fBaseVolumnSize, -fBaseVolumnSize,  fBaseVolumnSize);
	m_pvVertex[1].Set( fBaseVolumnSize, -fBaseVolumnSize,  fBaseVolumnSize);
	m_pvVertex[2].Set( fBaseVolumnSize, -fBaseVolumnSize, -fBaseVolumnSize);
	m_pvVertex[3].Set(-fBaseVolumnSize, -fBaseVolumnSize, -fBaseVolumnSize);
	m_pvVertex[4].Set(-fBaseVolumnSize,  fBaseVolumnSize,  fBaseVolumnSize);
	m_pvVertex[5].Set( fBaseVolumnSize,  fBaseVolumnSize,  fBaseVolumnSize);
	m_pvVertex[6].Set( fBaseVolumnSize,  fBaseVolumnSize, -fBaseVolumnSize);
	m_pvVertex[7].Set(-fBaseVolumnSize,  fBaseVolumnSize, -fBaseVolumnSize);

	unsigned short*		pIdx = m_pIndex;

	// bottom side.
	*pIdx++ = 0;  *pIdx++ = 1;  *pIdx++ = 3;
	*pIdx++ = 2;  *pIdx++ = 3;  *pIdx++ = 1;

	// obverse..
	*pIdx++ = 7;  *pIdx++ = 3;  *pIdx++ = 6;
	*pIdx++ = 2;  *pIdx++ = 6;  *pIdx++ = 3;

	// left..
	*pIdx++ = 4;  *pIdx++ = 0;  *pIdx++ = 7;
	*pIdx++ = 3;  *pIdx++ = 7;  *pIdx++ = 0;

	// right..
	*pIdx++ = 6;  *pIdx++ = 2;  *pIdx++ = 5;
	*pIdx++ = 1;  *pIdx++ = 5;  *pIdx++ = 2;

	// The back..
	*pIdx++ = 5;  *pIdx++ = 1;  *pIdx++ = 4;
	*pIdx++ = 0;  *pIdx++ = 4;  *pIdx++ = 1;

	// top side..
	*pIdx++ = 4;  *pIdx++ = 7;  *pIdx++ = 5;
	*pIdx++ = 6;  *pIdx++ = 5;  *pIdx++ = 7;

	m_iID = -1;
	m_pManager = nullptr;
	m_iPriority = 100;

	m_eRenderType = TYPE_UNKNOWN;
}

CPortalVolume::~CPortalVolume()
{
	DeleteAllPvsObj();
}

void CPortalVolume::DeleteAllPvsObj()
{
	ShapeInfo* pSI;
	auto siit = m_plShapeInfoList.begin();
	while(siit != m_plShapeInfoList.end())
	{
		pSI = *siit++;
		delete pSI;
	}
	m_plShapeInfoList.clear();	

	ShapePart* pSP;
	auto spit = m_lpShapePartList.begin();
	while(spit != m_lpShapePartList.end())
	{
		pSP = *spit++;
		pSP->Clear();
		delete pSP;
	}
	m_lpShapePartList.clear();

	__ColIndex* pCI;
	auto ciit = m_lpShapeColPartList.begin();
	while(ciit != m_lpShapeColPartList.end())
	{
		pCI = *ciit++;
		delete pCI;
	}
	m_lpShapeColPartList.clear();

	m_piVisibleIDList.clear();
	m_pVisiblePvsList.clear();
}

bool CPortalVolume::IsInVolumn(__Vector3 vec) const
{
	__Vector3 vec2[8];
	for(auto i = 0; i < 8; i++)
	{
		vec2[i] = m_pvVertex[i];
		vec2[i] *= m_Matrix;
	}

	if (vec.x >= vec2[0].x && vec.x <= vec2[1].x && vec.y >= vec2[0].y && vec.y <= vec2[4].y && vec.z >=  vec2[2].z && vec.z <= vec2[0].z )
		return true;

	return false;
}

void CPortalVolume::Render()
{
	DWORD dwAlpha, dwFog, dwLight, dwPointSize;
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_FOGENABLE, &dwFog);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlpha);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_POINTSIZE , &dwPointSize);
	
	if(dwFog) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	if(dwAlpha) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	if(dwLight) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	static __Material smtl;
	static bool bInit = false;
	if(false == bInit)
	{
		smtl.Init();
		bInit = true;
	}

	__Matrix44 mtxWorld;
	mtxWorld.Identity();

	CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxWorld);
	CN3Base::s_lpD3DDev->SetTexture(0, nullptr);

	// Shape..
	RenderShape();	

#ifdef _DEBUG
	RenderCollision();	
#endif 

	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_POINTSIZE, dwPointSize);
	if(dwFog) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_FOGENABLE, dwFog);
	if(dwAlpha) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlpha);
	if(dwLight) CN3Base::s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
}

void CPortalVolume::RenderShape()
{
	ShapeInfo* pSI;
	auto siit = m_plShapeInfoList.begin();
	while(siit != m_plShapeInfoList.end())
	{
		pSI = *siit++;

		pSI->m_pShape->PosSet(pSI->Pos());
		pSI->m_pShape->RotSet(pSI->Rot());
		pSI->m_pShape->ScaleSet(pSI->Scale());
		pSI->m_pShape->Tick(-1000);
		pSI->m_pShape->m_bDontRender = false;
		pSI->m_pShape->Render();

		// Apply pre-calculated world matrix when loading..
		__Matrix44 mtxBackup;
		CN3Base::s_lpD3DDev->GetTransform(D3DTS_WORLD, &mtxBackup);
		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &pSI->m_pShape->m_Matrix);

#ifdef _DEBUG
		if (pSI->m_pShape->CollisionMesh())
			pSI->m_pShape->CollisionMesh()->Render(0xffffffff);
#endif 

		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxBackup);
	}

	ShapePart* pSP = nullptr;
	auto spit = m_lpShapePartList.begin();
	while( spit != m_lpShapePartList.end())
	{
		pSP = *spit++;

		auto vit =	pSP->m_viIndex.begin();
		__VPI vpi;

		while (vit != pSP->m_viIndex.end())
		{
			vpi = *vit++;
			int iSize = vpi.m_ivVector.size();
		
	#ifdef _USE_VERTEXBUFFER
			LPDIRECT3DINDEXBUFFER9	pIB;
			HRESULT hr = CN3Base::s_lpD3DDev->CreateIndexBuffer(iSize*sizeof(WORD),
											D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB);
			if (FAILED(hr)) return hr;

			BYTE* pByte;
			hr = pIB->Lock(0, 0, &pByte, 0);
			WORD* pIndices = (WORD*)pByte;
			
			Iit = vpi.m_ivVector.begin();
			while(Iit != vpi.m_ivVector.end())
			{
				int iOrder = *Iit++;
				*pIndices++ = iOrder;
			}

			pIB->Unlock();
			
			// Rendering..
			pSI = CPvsMgr::GetShapeInfoByManager(pSP->m_iID);
			pSI->m_pShape->PosSet(pSI->Pos());
			pSI->m_pShape->RotSet(pSI->Rot());
			pSI->m_pShape->ScaleSet(pSI->Scale());
			pSI->m_pShape->Part(vpi.m_iPartIndex)->m_bOutOfCameraRange = FALSE;
			pSI->m_pShape->PartialRender(vpi.m_iPartIndex, iSize, pIB);
			pIB->Release();
	#else
			LPWORD pIndices;
			pIndices = new WORD[iSize]; memset(pIndices, 0, sizeof(WORD)*iSize);
			const int iSizes = vpi.m_ivVector.size();
			for(auto k = 0; k < iSizes; k++)
			{
				pIndices[k] = vpi.m_ivVector[k];
			}
			pSI = CPvsMgr::GetShapeInfoByManager(pSP->m_iID);
			pSI->m_pShape->PosSet(pSI->Pos());
			pSI->m_pShape->RotSet(pSI->Rot());
			pSI->m_pShape->ScaleSet(pSI->Scale());
			pSI->m_pShape->Part(vpi.m_iPartIndex)->m_bOutOfCameraRange = FALSE;
			pSI->m_pShape->PartialRender(vpi.m_iPartIndex, iSize, pIndices);
			delete pIndices;
	#endif
		}
	}
}

void CPortalVolume::RenderCollision()
{
	const __ColIndex * pCI = nullptr;
	const ShapeInfo* pSI = nullptr;

	auto ciit = m_lpShapeColPartList.begin();
	while(ciit != m_lpShapeColPartList.end())
	{
		pCI = *ciit++;

		// matrix calculations..
		__Matrix44 mtxWorld;
		mtxWorld.Identity();

		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxWorld);

		LPDWORD pIndices;
		const int iSize = pCI->m_ivVector.size();
		pIndices = new DWORD[iSize]; memset(pIndices, 0, sizeof(DWORD)*iSize);
		const int iSizes = pCI->m_ivVector.size();
		for(auto k = 0; k < iSizes; k++)
		{
			pIndices[k] = pCI->m_ivVector[k];
		}

		__Matrix44 mtxBackup;
		CN3Base::s_lpD3DDev->GetTransform(D3DTS_WORLD, &mtxBackup);
		pSI = CPvsMgr::GetShapeInfoByManager(pCI->m_iID);
		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &pSI->m_Matrix);
		pSI->m_pShape->PartialColRender(iSize, (int*)pIndices);
		delete pIndices;

		CN3Base::s_lpD3DDev->SetTransform(D3DTS_WORLD, &mtxBackup);
	}
}

// 

bool CPortalVolume::Load(HANDLE hFile)
{
	CN3Transform::Load(hFile);

	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];

	// Load your own data...
	DWORD dwNum;
	std::string strSrc, strDest;

	// Load the number of links.. Read it once and throw it away..
	int iLinkedCount = 0, iTID, iEWT;
	ReadFile(hFile, &iLinkedCount, sizeof(int), &dwNum, nullptr);
	for(auto i = 0; i < iLinkedCount; i++ )
	{
		ReadFile(hFile, &iTID, sizeof(int), &dwNum, nullptr);
		ReadFile(hFile, &iEWT, sizeof(int), &dwNum, nullptr);
	}

	// Load the number of linked Shapes..
	int iCount = 0;
	ReadFile(hFile, &iCount, sizeof(int), &dwNum, nullptr);
	for (auto i = 0; i < iCount; i++)
	{
		auto*	pSI = new ShapeInfo;
		ReadFile(hFile, &pSI->m_iID, sizeof(int), &dwNum, nullptr);

		// string length...
		strSrc = CPvsMgr::ReadDecryptString(hFile);
		_splitpath(strSrc.c_str(), szDrive, szDir, szFName, szExt);
		strDest = szFName;	strDest +=  szExt;
		pSI->m_strShapeFile = m_pManager->GetIndoorFolderPath() + strDest;
		ReadFile(hFile, &pSI->m_iBelong, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iEventID, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iEventType, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iNPC_ID, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iNPC_Status, sizeof(int), &dwNum, nullptr);	
		if (pSI->m_iEventID || pSI->m_iEventType || pSI->m_iNPC_ID || pSI->m_iNPC_Status ) // if there is an event
			pSI->m_pShape = CPvsMgr::s_MngShapeExt.Get(m_pManager->GetIndoorFolderPath() + strDest);
		else
			pSI->m_pShape = CPvsMgr::s_MngShape.Get(m_pManager->GetIndoorFolderPath() + strDest);
		__ASSERT(pSI->m_pShape, "Shape Not Found");
		pSI->Load(hFile);
		m_plShapeInfoList.push_back(pSI);		
	}

	// Visible..
	IDAndPriority IDAP;
	ReadFile(hFile, &iCount, sizeof(int), &dwNum, nullptr);

	for(auto i = 0; i < iCount; i++ )
	{
		ReadFile(hFile, &IDAP.m_iID, sizeof(int), &dwNum, nullptr);
		ReadFile(hFile, &IDAP.m_iPriority, sizeof(int), &dwNum, nullptr);
		__ASSERT(IDAP.m_iPriority != -1, "wrong file");
		m_piVisibleIDList.push_back(IDAP);
	}

	ReadFile(hFile, &iCount, sizeof(int), &dwNum, nullptr);

	int iSize_2 = 0, iSize_3 = 0;
	for(auto i = 0; i < iCount; i++ )
	{
		auto* pSP = new ShapePart;
		ReadFile(hFile, &pSP->m_iID, sizeof(int), &dwNum, nullptr);
		
		ReadFile(hFile, &iSize_2, sizeof(int), &dwNum, nullptr);
		for(auto j = 0; j <iSize_2; j++ )
		{
			__VPI vpi;
			ReadFile(hFile, &vpi.m_iPartIndex, sizeof(int), &dwNum, nullptr);

			ReadFile(hFile, &iSize_3, sizeof(int), &dwNum, nullptr);
			for(auto k = 0; k < iSize_3; k++ )
			{
				int iV = 0;
				ReadFile(hFile, &iV, sizeof(int), &dwNum, nullptr);
				vpi.m_ivVector.push_back(iV);
			}

			pSP->m_viIndex.push_back(vpi);	
		}

		m_lpShapePartList.push_back(pSP);
	}

	ReadFile(hFile, &iCount, sizeof(int), &dwNum, nullptr);

	for(auto i = 0; i < iCount; i++ )
	{
		auto* pCI = new __ColIndex;
		ReadFile(hFile, &pCI->m_iID, sizeof(int), &dwNum, nullptr);		

		ReadFile(hFile, &iSize_2, sizeof(int), &dwNum, nullptr);
		for(auto j = 0; j <iSize_2; j++ )
		{
			int iV = 0;			
			ReadFile(hFile, &iV, sizeof(int), &dwNum, nullptr);
			pCI->m_ivVector.push_back(iV);
		}

		m_lpShapeColPartList.push_back(pCI);
	}

	return true;
}


// 

bool CPortalVolume::CheckCollisionCameraWithTerrain(__Vector3& vEyeResult, const __Vector3& vAt, float fNP)
{
	float fPlayerHeight = CGameBase::s_pPlayer->Position().y;

	bool bCollision = false;
	bool bColl = false;
	__Vector3 vDir = vEyeResult- vAt;	vDir.Normalize();
	const __ColIndex * pCI = nullptr;

	int iCount = m_lpShapeColPartList.size();
	auto ciit = m_lpShapeColPartList.begin();
	while(ciit != m_lpShapeColPartList.end())
	{
		pCI = *ciit++;

		const int iSize = pCI->m_ivVector.size();
		__Vector3 vA, vB, vC, vPick;
		float t,u,v;

		const ShapeInfo* pSI = CPvsMgr::GetShapeInfoByManager(pCI->m_iID);
		pSI->m_pShape->PosSet(pSI->Pos());
		pSI->m_pShape->RotSet(pSI->Rot());
		pSI->m_pShape->ScaleSet(pSI->Scale());
		pSI->m_pShape->Tick(-1000);
		CN3Shape *pShape = pSI->m_pShape;

		for(auto k = 0; k < iSize/3; k++)
		{
			pShape->PartialGetCollision(pCI->m_ivVector[k*3], vA);			vA *= pShape->m_Matrix;		vA.y += m_fCameraOffs;
			pShape->PartialGetCollision(pCI->m_ivVector[k*3+1], vB);		vB *= pShape->m_Matrix;		vB.y += m_fCameraOffs;
			pShape->PartialGetCollision(pCI->m_ivVector[k*3+2], vC);		vC *= pShape->m_Matrix;		vC.y += m_fCameraOffs;

			__Vector3 vEdge1 = vB - vA;
			__Vector3 vEdge2 = vC - vA;
			__Vector3 pVec;	
			pVec.Cross(vEdge1, vEdge2);	pVec.Normalize();	pVec *= m_fOffs;	vA += pVec;	vB += pVec; vC += pVec;

			bColl = ::_IntersectTriangle(vAt, vDir, vA, vB, vC, t, u, v, &vPick);
			if (bColl)
			{
				if ((vPick - vAt).Magnitude() < fNP)	
				{
					vEyeResult = vPick;
					bCollision = true;
				}
			}
		}
	}

	return bCollision;
}

bool CPortalVolume::CheckCollisionCameraWithShape(__Vector3& vEyeResult, const __Vector3& vAt, float fNP)
{
	bool bCollision = false;
	bool bColl = false;
	__Vector3 vDir = vEyeResult- vAt;	vDir.Normalize();
	const ShapeInfo* pSI = nullptr;

	auto siit = m_plShapeInfoList.begin();
	while(siit != m_plShapeInfoList.end())
	{
		pSI = *siit++;

		const int iSize = pSI->m_pShape->GetColIndexbufferCount();
		__Vector3 vA, vB, vC, vPick;
		float t,u,v;

		for(auto k = 0; k < iSize/3; k++)
		{
			vA = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3));		 vA *= pSI->m_Matrix;
			vB = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3+1));		 vB *= pSI->m_Matrix;
			vC = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3+2));		 vC *= pSI->m_Matrix;

			__Vector3 vEdge1 = vB - vA;
			__Vector3 vEdge2 = vC - vA;
			__Vector3 pVec;	
			pVec.Cross(vEdge1, vEdge2);	pVec.Normalize();	pVec *= m_fOffs;	vA += pVec;	vB += pVec; vC += pVec;

			bColl = ::_IntersectTriangle(vAt, vDir, vA, vB, vC, t, u, v, &vPick);
			if (bColl)
			{
				if ((vPick - vAt).Magnitude() < fNP)	
				{
					vEyeResult = vPick;
					bCollision = true;
				}
			}
		}
	}

	return bCollision;
}

bool CPortalVolume::GetHeightWithTerrain(float fx, float fz, float& fy)
{
	CPortalVolume* pVol = nullptr;

	VisPortalPriority vPP;
	auto vppit = m_pVisiblePvsList.begin();
	while( vppit != m_pVisiblePvsList.end())
	{
		vPP = *vppit++;
		if (vPP.m_iPriority > 1)
			continue;	

		pVol = vPP.m_pVol;

		const __ColIndex * pCI = nullptr;
		int iCount = pVol->m_lpShapeColPartList.size();
		auto ciit = pVol->m_lpShapeColPartList.begin();
		while(ciit != pVol->m_lpShapeColPartList.end())
		{
			pCI = *ciit++;

			const int iSize = pCI->m_ivVector.size();
			__Vector3 vA, vB, vC;
			const ShapeInfo* pSI = CPvsMgr::GetShapeInfoByManager(pCI->m_iID);
			pSI->m_pShape->PosSet(pSI->Pos());
			pSI->m_pShape->RotSet(pSI->Rot());
			pSI->m_pShape->ScaleSet(pSI->Scale());
			pSI->m_pShape->Tick(-1000);
			CN3Shape *pShape = pSI->m_pShape;

			for(auto k = 0; k < iSize/3; k++)
			{
				pShape->PartialGetCollision(pCI->m_ivVector[k*3], vA);		 vA *= pShape->m_Matrix;
				pShape->PartialGetCollision(pCI->m_ivVector[k*3+1], vB);	vB *= pShape->m_Matrix;
				pShape->PartialGetCollision(pCI->m_ivVector[k*3+2], vC);	vC *= pShape->m_Matrix;

				float a, b, c;
				a = ((vB.x-vA.x)*(fz-vA.z)) - ((vB.z-vA.z)*(fx-vA.x));
				b = ((vC.x-vB.x)*(fz-vB.z)) - ((vC.z-vB.z)*(fx-vB.x));
				c = ((vA.x-vC.x)*(fz-vC.z)) - ((vA.z-vC.z)*(fx-vC.x));

				if((a<=0 && b<=0 && c<=0) || (a>=0 && b>=0 && c>=0))
				{
					__Vector3 vAB, vAC, vAxis;
					vAB.Set(vB.x - vA.x, vB.y - vA.y, vB.z - vA.z);
					vAC.Set(vC.x - vA.x, vC.y - vA.y, vC.z - vA.z);

					vAxis.Cross(vAB, vAC);
					if (vAxis.y <= 0.0f )
						continue;

					fy = (vAxis.x*(vA.x-fx)+vAxis.z*(vA.z-fz))/vAxis.y + vA.y;
					fy += m_fHeightOffs;
		
					return true;
				}
			}
		}
	}

	return false;
}

float CPortalVolume::GetHeightNearstPosWithShape(const __Vector3& vPos, float fDist, __Vector3* pvNormal)
{
	float fHeight = FLT_MIN;
	const ShapeInfo* pSI = nullptr;

	int iCount = m_plShapeInfoList.size();
	auto siit = m_plShapeInfoList.begin();
	while(siit != m_plShapeInfoList.end())
	{
		pSI = *siit++;

		const int iSize = pSI->m_pShape->GetColIndexbufferCount();
		__Vector3 vA, vB, vC;
		for(auto k = 0; k < iSize/3; k++)
		{
			vA = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3));		 vA *= pSI->m_Matrix;
			vB = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3+1));		 vB *= pSI->m_Matrix;
			vC = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3+2));		 vC *= pSI->m_Matrix;

			float a, b, c;
			a = ((vB.x-vA.x)*(vPos.z-vA.z)) - ((vB.z-vA.z)*(vPos.x-vA.x));
			b = ((vC.x-vB.x)*(vPos.z-vB.z)) - ((vC.z-vB.z)*(vPos.x-vB.x));
			c = ((vA.x-vC.x)*(vPos.z-vC.z)) - ((vA.z-vC.z)*(vPos.x-vC.x));

			if((a<=0 && b<=0 && c<=0) || (a>=0 && b>=0 && c>=0))
			{
				__Vector3 vAB, vAC, vAxis;
				vAB.Set(vB.x - vA.x, vB.y - vA.y, vB.z - vA.z);
				vAC.Set(vC.x - vA.x, vC.y - vA.y, vC.z - vA.z);

				vAxis.Cross(vAB, vAC);
				if (vAxis.y <= 0.0f )
					continue;
 
				fHeight = (vAxis.x*(vA.x-vPos.x)+vAxis.z*(vA.z-vPos.z))/vAxis.y + vA.y;
				fHeight += m_fHeightOffs;

				if (pvNormal)
				{
					const __Vector3 vEdge1 = vB - vA;
					const __Vector3 vEdge2 = vC - vA;
					pvNormal->Cross(vEdge1, vEdge2);	pvNormal->Normalize();
				}
	
				return fHeight;
			}
		}
	}

	return fHeight;
}

bool CPortalVolume::IsInTerrainWithTerrain(__Vector3& vec)
{
	__Vector3 vec2[3];
	for(auto i = 0; i < 3; i++)
	{
		vec2[i] = m_pvVertex[i];
		vec2[i] *= m_Matrix;
	}

	if (vec.x >= vec2[0].x && vec.x <= vec2[1].x && 
		vec.z >=  vec2[2].z && vec.z <= vec2[0].z )
		return GetHeightWithTerrain(vec.x, vec.z, vec.y);

	return false;
}

BOOL CPortalVolume::PickWideWithTerrain(int x, int y, __Vector3& vPick)
{
	BOOL bColl = FALSE;

	// Compute the vector of the pick ray in screen space
	__Vector3 vTmp;
	vTmp.x =  ( ( ( 2.0f * x ) / (CN3Base::s_CameraData.vp.Width) ) - 1 ) / CN3Base::s_CameraData.mtxProjection._11;
	vTmp.y = -( ( ( 2.0f * y ) / (CN3Base::s_CameraData.vp.Height) ) - 1 ) / CN3Base::s_CameraData.mtxProjection._22;
	vTmp.z =  1.0f;

	// Transform the screen space pick ray into 3D space
	const __Matrix44* pMtxVI = &CN3Base::s_CameraData.mtxViewInverse;
	__Vector3 vDir;
	vDir.x  = vTmp.x * pMtxVI->_11 + vTmp.y * pMtxVI->_21 + vTmp.z * pMtxVI->_31;
	vDir.y  = vTmp.x * pMtxVI->_12 + vTmp.y * pMtxVI->_22 + vTmp.z * pMtxVI->_32;
	vDir.z  = vTmp.x * pMtxVI->_13 + vTmp.y * pMtxVI->_23 + vTmp.z * pMtxVI->_33;
	const __Vector3 vPos = pMtxVI->Pos();
	__Vector3 vPosCur = vPos;

	vDir.Normalize();

	__Vector3 A, B, C;
	float t, u, v;
	const __ColIndex * pCI = nullptr;

	int iCount = m_lpShapeColPartList.size();
	auto ciit = m_lpShapeColPartList.begin();
	while(ciit != m_lpShapeColPartList.end())
	{
		pCI = *ciit++;

		const ShapeInfo* pSI = CPvsMgr::GetShapeInfoByManager(pCI->m_iID);
		pSI->m_pShape->PosSet(pSI->Pos());
		pSI->m_pShape->RotSet(pSI->Rot());
		pSI->m_pShape->ScaleSet(pSI->Scale());
		pSI->m_pShape->Tick(-1000);
		CN3Shape *pShape = pSI->m_pShape;

		const int iSize = pCI->m_ivVector.size();
		for(auto k = 0; k < iSize/3; k++)
		{
			pShape->PartialGetCollision(pCI->m_ivVector[k*3], A);		 A *= pShape->m_Matrix;
			pShape->PartialGetCollision(pCI->m_ivVector[k*3+1], B);		B *= pShape->m_Matrix;
			pShape->PartialGetCollision(pCI->m_ivVector[k*3+2], C);		C *= pShape->m_Matrix;

			if (::_IntersectTriangle(vPos, vDir, A, B, C, t, u, v, &vPick))
			{
				bool bNom = false;
				__Vector3 vEdge1 = B - A;
				__Vector3 vEdge2 = C - A;
				__Vector3 pVec;	
				pVec.Cross(vEdge1, vEdge2);	pVec.Normalize();	pVec.y = 0.0f;	
				if (pVec.Magnitude() < m_fPickIncline)	// inclination..
					return TRUE;
				else
				{
					int kk = 0;
				}
			}
		}
	}

	return FALSE;
}

CN3Shape* CPortalVolume::PickWithShape(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick)
{
	__Vector3 vPos, vDir;
	::_Convert2D_To_3DCoordinate(iXScreen, iYScreen, s_CameraData.mtxView, s_CameraData.mtxProjection, s_CameraData.vp, vPos, vDir);

	// Sort by distance.
	std::vector<ShapeInfo*> Shapes;
	ShapeInfo* pSI = nullptr;
	CPortalVolume* pVol = nullptr;
	VisPortalPriority vPP;
	auto vppit = m_pVisiblePvsList.begin();
	while( vppit != m_pVisiblePvsList.end())
	{
		vPP = *vppit++;
		pVol = vPP.m_pVol;

		auto siit = pVol->m_plShapeInfoList.begin();
		while(siit != pVol->m_plShapeInfoList.end())
		{
			pSI = *siit++;
			Shapes.push_back(pSI);
		}
	}

	const int iSC = Shapes.size();
	qsort(&(Shapes[0]), iSC, 4, CN3ShapeMgr::SortByCameraDistance);
	for(int i = 0; i < iSC; i++)
	{
		ShapeInfo* pShr = Shapes[i];
		if(bMustHaveEvent && Shapes[i]->m_iEventID <= 0) continue; // If there must be an event...
		Shapes[i]->m_pShape->PosSet(Shapes[i]->Pos());
		Shapes[i]->m_pShape->RotSet(Shapes[i]->Rot());
		Shapes[i]->m_pShape->ScaleSet(Shapes[i]->Scale());
		if(Shapes[i]->m_pShape->CheckCollisionPrecisely(false, vPos, vDir, pvPick) >= 0)
		{
			Shapes[i]->m_pShape->m_iEventID			= Shapes[i]->m_iEventID;
			Shapes[i]->m_pShape->m_iEventType	 = Shapes[i]->m_iEventType;
			Shapes[i]->m_pShape->m_iNPC_ID		   = Shapes[i]->m_iNPC_ID;
			Shapes[i]->m_pShape->m_iNPC_Status  = Shapes[i]->m_iNPC_Status;
			return Shapes[i]->m_pShape;
		}
	}

	return nullptr;
}

CN3Shape* CPortalVolume::ShapeGetByIDWithShape(int iID)
{
	const ShapeInfo* pSI = nullptr;
	CPortalVolume* pVol = nullptr;
	VisPortalPriority vPP;
	auto vppit = m_pVisiblePvsList.begin();
	while( vppit != m_pVisiblePvsList.end())
	{
		vPP = *vppit++;
		pVol = vPP.m_pVol;

		auto siit = pVol->m_plShapeInfoList.begin();
		while(siit != pVol->m_plShapeInfoList.end())
		{
			pSI = *siit++;
			if (pSI->m_iEventID == iID)
			{
				pSI->m_pShape->m_iEventID			= pSI->m_iEventID;
				pSI->m_pShape->m_iEventType		 = pSI->m_iEventType;
				pSI->m_pShape->m_iNPC_ID		   = pSI->m_iNPC_ID;
				pSI->m_pShape->m_iNPC_Status	 = pSI->m_iNPC_Status;
				return pSI->m_pShape;
			}
		}
	}

	return nullptr;
}

bool CPortalVolume::CheckCollisionWithShape(	const __Vector3& vPos,				 // crash location
																				const __Vector3& vDir,				   // direction vector
																				float fSpeedPerSec,					    // moving speed per second
																				__Vector3* pvCol,						 // crash point
																				__Vector3* pvNormal,				  // The normal vector of the colliding face
																				__Vector3* pVec)						// polygon of the collided face __Vector3[3]
{
	const __Vector3 vPosNext = vPos + (vDir * fSpeedPerSec); // next location
	const float fMcs = (vPosNext - vPos).Magnitude();

	bool bCollision = false;
	bool bColl = false;
	CPortalVolume* pVol = nullptr;
	const ShapeInfo* pSI = nullptr;

	VisPortalPriority vPP;
	auto vppit = m_pVisiblePvsList.begin();
	while( vppit != m_pVisiblePvsList.end())
	{
		vPP = *vppit++;
		if (vPP.m_iPriority > 1)
			continue;	

		pVol = vPP.m_pVol;

		auto siit = pVol->m_plShapeInfoList.begin();
		while(siit != pVol->m_plShapeInfoList.end())
		{
			pSI = *siit++;

			const int iSize = pSI->m_pShape->GetColIndexbufferCount();
			__Vector3 vA, vB, vC, vPick;
			float t,u,v;

			for(auto k = 0; k < iSize/3; k++)
			{
				vA = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3));		 vA *= pSI->m_Matrix;
				vB = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3+1));		 vB *= pSI->m_Matrix;
				vC = pSI->m_pShape->GetColVertexByIndex(pSI->m_pShape->GetColIndexByiOrder(k*3+2));		 vC *= pSI->m_Matrix;

				__Vector3 vEdge1 = vB - vA;
				__Vector3 vEdge2 = vC - vA;
				__Vector3 pVecto;	
				pVecto.Cross(vEdge1, vEdge2);	pVecto.Normalize();	pVecto *= m_fOffs;	vA += pVecto;	vB += pVecto; vC += pVecto;

				bColl = ::_IntersectTriangle(vPos, vDir, vA, vB, vC, t, u, v, &vPick);
				if (bColl)
				{
					if ((vPick - vPos).Magnitude() < fMcs)	
					{
						if(pvCol) *pvCol = vPick;
						if(pvNormal)
						{
							(*pvNormal).Cross(vB - vA, vC - vA);
							(*pvNormal).Normalize();
						}
						if (pVec)
						{
							pVec[0] = vA;
							pVec[1] = vB;
							pVec[2] = vC;
						}
						bCollision = true;
					}
				}
			}
		}
	}

	return bCollision;
}
