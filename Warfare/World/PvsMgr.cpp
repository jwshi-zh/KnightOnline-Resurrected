#include "pch.h"
#include "PvsMgr.h"
#include "GameBase.h"
#include "PlayerMySelf.h"
#include "GameProcedure.h"
#include "GameEng.h"

#include "N3Camera.h"
#include "N3ShapeMgr.h"

#define INDOOR_FOLDER "N3Indoor\\"

CN3Mng <class CN3Shape>		CPvsMgr::s_MngShape; 
CN3Mng <class CN3ShapeExtra>	CPvsMgr::s_MngShapeExt; 
std::list<ShapeInfo* > CPvsMgr::s_plShapeInfoList;

CPvsMgr::CPvsMgr()	: m_IndoorFolder("N3Indoor\\"), m_fVolumeOffs(0.6f)	// ..
{
	s_plShapeInfoList.clear();
	m_pCurVol = nullptr;
}

CPvsMgr::~CPvsMgr()
{
	DeleteAllPvsObj();
}

void CPvsMgr::DeleteAllPvsObj()
{
	auto it = m_pPvsList.begin();

	while (it != m_pPvsList.end())	
	{
		const CPortalVolume* pvs = *it;
		if (pvs) delete pvs;
		it++;
	}	
	m_pPvsList.clear();

	ShapeInfo* pSI;
	auto siit = s_plShapeInfoList.begin();
	while(siit != s_plShapeInfoList.end())
	{
		pSI = *siit++;
		delete pSI;
	}
	s_plShapeInfoList.clear();
	s_MngShape.Release();
	s_MngShapeExt.Release();
}

ShapeInfo* CPvsMgr::GetShapeInfoByManager(int iID)
{
	ShapeInfo* pSI;
	auto siit = s_plShapeInfoList.begin();
	while(siit != s_plShapeInfoList.end())
	{
		pSI = *siit++;
		if (pSI->m_iID == iID)
			return pSI;
	}

	return nullptr;
}

void CPvsMgr::Tick(bool bWarp, __Vector3 vPos)
{
	CPortalVolume* pVol = nullptr;
	__Vector3 vec = CGameBase::s_pPlayer->Position();	
	if(bWarp)
		vec = vPos;

	vec.y += m_fVolumeOffs;
	m_pCurVol = nullptr;

	auto it = m_pPvsList.begin();
	while(it != m_pPvsList.end())
	{
		pVol = *it++;	
		if (pVol->IsInVolumn(vec))
		{
			m_pCurVol = pVol;
			break;
		}
	}

	it = m_pPvsList.begin();
	while(it != m_pPvsList.end())
	{
		pVol = *it++;
		pVol->m_eRenderType = TYPE_UNKNOWN;
	}

	if (!m_pCurVol) return;

	VisPortalPriority vPP;
	auto vppit = m_pCurVol->m_pVisiblePvsList.begin();
	while(vppit != m_pCurVol->m_pVisiblePvsList.end())
	{
		vPP = *vppit++;
		vPP.m_pVol->m_eRenderType = TYPE_TRUE;
	}		
}

void CPvsMgr::Render()
{
	CPortalVolume* pVol = nullptr;
	auto it = m_pPvsList.begin();

	while(it != m_pPvsList.end())
	{
		pVol = *it++;	
		if ( pVol->m_eRenderType == TYPE_TRUE )
		{
			pVol->Render();
		}
	}
}

bool CPvsMgr::LoadOldVersion(HANDLE hFile, int iVersionFromData)
{
	// ..

	return true;
}

bool CPvsMgr::Load(HANDLE hFile)
{
	DWORD dwNum;
	int iT;

	ReadFile(hFile, &iT, sizeof(int), &dwNum, nullptr);
	if (iT != ciVersion)
		return LoadOldVersion(hFile, iT);

	// N3Scene file.. not used.. -.-;
	std::string strSrc = ReadDecryptString(hFile), strDest;

	// Total movement value.. does not work.. -.-;
	ReadFile(hFile, &iT, sizeof(int), &dwNum, nullptr);
	ReadFile(hFile, &iT, sizeof(int), &dwNum, nullptr);	
	ReadFile(hFile, &iT, sizeof(int), &dwNum, nullptr);	

	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFName[_MAX_FNAME], szExt[_MAX_EXT];
	int iCount;

	ReadFile(hFile, &iCount, sizeof(int), &dwNum, nullptr);	

	for (int i = 0; i < iCount; i++ )
	{
		auto*	pSI = new ShapeInfo;
		ReadFile(hFile, &pSI->m_iID, sizeof(int), &dwNum, nullptr);
		
		// string length...
		strSrc = ReadDecryptString(hFile);
		_splitpath(strSrc.c_str(), szDrive, szDir, szFName, szExt);
		strDest = szFName;	strDest +=  szExt;
		pSI->m_strShapeFile = m_IndoorFolder + strDest;
		pSI->m_pShape = s_MngShape.Get(m_IndoorFolder + strDest);
		__ASSERT(pSI->m_pShape, "Shape Not Found");
	
		ReadFile(hFile, &pSI->m_iBelong, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iEventID, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iEventType, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iNPC_ID, sizeof(int), &dwNum, nullptr);	
		ReadFile(hFile, &pSI->m_iNPC_Status, sizeof(int), &dwNum, nullptr);	
		pSI->Load(hFile);
		s_plShapeInfoList.push_back(pSI);
	}

	// Total Count..
	ReadFile(hFile, &iCount, sizeof(int), &dwNum, nullptr);

	CPortalVolume* pVol = nullptr, *pVolTo = nullptr;
	int iID;

	for(auto i = 0; i < iCount; i++ )
	{
		ReadFile(hFile, &iID, sizeof(int), &dwNum, nullptr);
		pVol = new CPortalVolume;
		pVol->m_pManager = this;
		pVol->m_iID	= iID;	 
		pVol->Load(hFile);
		m_pPvsList.push_back(pVol);
	}

	auto it = m_pPvsList.begin();
	IDAndPriority IDAP;
	VisPortalPriority vPP;

	while(it != m_pPvsList.end())
	{
		pVol = *it++;

		auto idapit = pVol->m_piVisibleIDList.begin();
		while (idapit != pVol->m_piVisibleIDList.end())
		{
			 IDAP = *idapit++;
			 pVolTo = GetPortalVolPointerByID(IDAP.m_iID);
			 vPP.m_pVol = pVolTo;
			 vPP.m_iPriority = IDAP.m_iPriority;
			 pVol->m_pVisiblePvsList.push_back(vPP);
		}

		pVol->m_piVisibleIDList.clear();
	}

	return true;
}

CPortalVolume* CPvsMgr::GetPortalVolPointerByID(int iID)
{
	CPortalVolume* pVol = nullptr;

	auto it = m_pPvsList.begin();

	while(it != m_pPvsList.end())
	{
		// Save your data...
		pVol = *it++;
		if (pVol->m_iID == iID)
			return pVol;
	}

	return nullptr;	
}

#define CRY_KEY 0x0816

std::string CPvsMgr::ReadDecryptString(HANDLE hFile)
{
	DWORD dwNum;
	int iCount;

	ReadFile(hFile, &iCount, sizeof(int), &dwNum, nullptr);
	std::vector<char> buffer(iCount);

	ReadFile(hFile, &(buffer[0]), iCount, &dwNum, nullptr);				// string
	for(auto i = 0; i < iCount; i++)
		buffer[i] ^= CRY_KEY;
	buffer.push_back((char)0x00);

	std::string strDest;
	strDest = buffer.data();
	
	return strDest;
}


// 

bool CPvsMgr::CheckCollisionCameraWithTerrain(__Vector3& vEyeResult, const __Vector3& vAt, float fNP)
{
	fNP = (vAt - vEyeResult).Magnitude();
	bool bCollision = false;
	float fNPMin = fNP, fTemp = 0.0f;

	CPortalVolume* pVol = nullptr;
	auto it = m_pPvsList.begin();

	while(it != m_pPvsList.end())
	{
		pVol = *it++;	
		if ( (pVol->m_eRenderType == TYPE_TRUE ) && pVol->CheckCollisionCameraWithTerrain(vEyeResult, vAt, fNP))
		{
			bCollision = true;
			fTemp = (vEyeResult- vAt).Magnitude(); 
			if (fTemp < fNPMin)
				fNPMin = fTemp;
		}
	}

	if (bCollision && (fNPMin < fNP))
	{
		__Vector3 vT; vT.Zero();
		vT = vEyeResult - vAt; vT.Normalize();	vT *= fNPMin; vEyeResult = vT + vAt;
	}
	
	return bCollision;
}

bool CPvsMgr::CheckCollisionCameraWithShape(__Vector3& vEyeResult, const __Vector3& vAt, float fNP)
{
	fNP = (vAt - vEyeResult).Magnitude();
	bool bCollision = false;
	float fNPMin = fNP, fTemp = 0.0f;

	CPortalVolume* pVol = nullptr;
	auto it = m_pPvsList.begin();

	while(it != m_pPvsList.end())
	{
		pVol = *it++;	
		if ( (pVol->m_eRenderType == TYPE_TRUE ) && pVol->CheckCollisionCameraWithShape(vEyeResult, vAt, fNP))
		{
			bCollision = true;
			fTemp = (vEyeResult- vAt).Magnitude(); 
			if (fTemp < fNPMin)
				fNPMin = fTemp;
		}
	}

	if (bCollision && (fNPMin < fNP))
	{
		__Vector3 vT; vT.Zero();
		vT = vEyeResult - vAt; vT.Normalize();	vT *= fNPMin; vEyeResult = vT + vAt;	
	}
	
	return bCollision;
}

float CPvsMgr::GetHeightWithTerrain(float x, float z, bool bWarp)
{
	if (!m_pCurVol)
		Tick();

	float fHeight = FLT_MIN;
	if (!m_pCurVol)
		return fHeight;

	m_pCurVol->GetHeightWithTerrain(x, z, fHeight);
	return fHeight;
}

float CPvsMgr::GetHeightNearstPosWithShape(const __Vector3& vPos, float fDist, __Vector3* pvNormal)
{
	float fHeightMax = FLT_MIN, fHeight;

	CPortalVolume* pVol = nullptr;
	auto it = m_pPvsList.begin();

	while(it != m_pPvsList.end())
	{
		pVol = *it++;	
		if ( pVol->m_eRenderType == TYPE_TRUE )
		{
			fHeight = pVol->GetHeightNearstPosWithShape(vPos, fDist, pvNormal);
			if (fHeightMax < fHeight)
				fHeightMax = fHeight;
		}		
	}
	
	return fHeightMax;
}

bool CPvsMgr::IsInTerrainWithTerrain(float x, float z, __Vector3 vPosBefore)
{
	if (!m_pCurVol)
		Tick();

	float fHeight = FLT_MIN;
	if (!m_pCurVol)
		return false;

	return m_pCurVol->GetHeightWithTerrain(x, z, fHeight);
}

float CPvsMgr::GetHeightWithShape(float fX, float fZ, __Vector3* pvNormal)
{
	float fHeightMax = FLT_MIN, fHeight;

	CPortalVolume* pVol = nullptr, *pVolNe = nullptr;
	auto it = m_pPvsList.begin();

	while(it != m_pPvsList.end())
	{
		pVol = *it++;	
		fHeight = pVol->GetHeightNearstPosWithShape(__Vector3(fX, FLT_MIN, fZ), 0.0f);
		if (fHeightMax < fHeight)
		{
			fHeightMax = fHeight;
			pVolNe = pVol;
		}
	}		
	
	__Vector3 vPos; vPos.Set(fX, fHeightMax, fZ);
	if (pVolNe && pVolNe->m_eRenderType != TYPE_TRUE)
		Tick(true, vPos);

	return fHeightMax;
}

BOOL CPvsMgr::PickWideWithTerrain(int x, int y, __Vector3& vPick)
{
	BOOL bColl = FALSE;
	__Vector3 vCamPo = CGameProcedure::s_pEng->CameraGetActive()->EyePos(), vPT;
	float fDistMax = FLT_MAX, fDT;

	CPortalVolume* pVol = nullptr, *pVolNe = nullptr;
	auto it = m_pPvsList.begin();

	while(it != m_pPvsList.end())
	{
		pVol = *it++;	
		if ((pVol->m_eRenderType == TYPE_TRUE) && (pVol->PickWideWithTerrain(x, y, vPT)) )
		{	
			fDT = (vPT - vCamPo).Magnitude();
			if (fDT <= fDistMax)
			{
				fDistMax = fDT;
				vPick = vPT;
				bColl = TRUE;
			}
		}
	}
	
	return bColl;
}

CN3Shape* CPvsMgr::PickWithShape(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick)
{
	if (!m_pCurVol)
		Tick();

	if (!m_pCurVol)
		return nullptr;

	return m_pCurVol->PickWithShape(iXScreen, iYScreen, bMustHaveEvent, pvPick);
}

bool CPvsMgr::CheckCollisionWithShape(	const __Vector3& vPos,				 // crash location
																	const __Vector3& vDir,				   // direction vector
																	float fSpeedPerSec,					    // moving speed per second
																	__Vector3* pvCol,						 // crash point
																	__Vector3* pvNormal,				  // The normal vector of the colliding face
																	__Vector3* pVec)						// Polygon of collided face __Vector3[3]
{
	if (!m_pCurVol)
		Tick();

	if (!m_pCurVol)
		return NULL;

	return m_pCurVol->CheckCollisionWithShape(vPos, vDir, fSpeedPerSec, pvCol, pvNormal, pVec);
}

CN3Shape* CPvsMgr::ShapeGetByIDWithShape(int iID)
{
	if (!m_pCurVol)
		Tick();

	if (!m_pCurVol)
		return nullptr;

	return m_pCurVol->ShapeGetByIDWithShape(iID);
}