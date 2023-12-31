#include "pch.h"
#include "DungeonManager.h"

#include "PlayerMySelf.h"

CDungeonManager::CDungeonManager()
{

}

CDungeonManager::~CDungeonManager()
{

}

// 

void CDungeonManager::InitWorld(int iZoneID, const __Vector3& vPosPlayer)
{
	const __TABLE_ZONE* pZone = s_pTbl_Zones->Find(s_pPlayer->m_InfoExt.iZoneCur);
	if(nullptr == pZone) { CLogWriter::Write("Null Zone data : %d", iZoneID); return; }
		
	m_pvsmgr.LoadFromFile(pZone->szTerrainFN);
}

void CDungeonManager::Tick()
{
	m_pvsmgr.Tick();
}

void CDungeonManager::RenderTerrain()
{
	m_pvsmgr.Render();
}

void CDungeonManager::RenderShape()
{
}

CN3Terrain* CDungeonManager::GetTerrainRef()
{
	return nullptr;
}

CN3SkyMng* CDungeonManager::GetSkyRef()
{
	return nullptr;
}

// 

// Terrain..
bool CDungeonManager::CheckCollisionCameraWithTerrain(__Vector3& vEyeResult, const __Vector3& vAt, float fNP)	// Done..
{
	return m_pvsmgr.CheckCollisionCameraWithTerrain(vEyeResult, vAt, fNP);						
}

float CDungeonManager::GetHeightWithTerrain(float x, float z, bool bWarp)																	// Done..
{
	return m_pvsmgr.GetHeightWithTerrain(x, z, bWarp);																		
}

BOOL CDungeonManager::PickWideWithTerrain(int x, int y, __Vector3& vPick)																// Done..
{
	return m_pvsmgr.PickWideWithTerrain(x, y, vPick);
}

bool CDungeonManager::CheckCollisionWithTerrain(__Vector3& vPos, __Vector3& vDir, float fVelocity, __Vector3* vCol)
{
	return false;	
}

void CDungeonManager::GetNormalWithTerrain(float x, float z, __Vector3& vNormal)
{
}

float CDungeonManager::GetWidthByMeterWithTerrain()
{
	return 100.0f;
}

bool CDungeonManager::IsInTerrainWithTerrain(float x, float z, __Vector3 vPosBefore)												// Done..
{
	return m_pvsmgr.IsInTerrainWithTerrain(x, z, vPosBefore);
}

bool CDungeonManager::CheckInclineWithTerrain(const __Vector3& vPos, const __Vector3& vDir, float fIncline)
{
	return false;
}

// Shapes..
bool CDungeonManager::CheckCollisionCameraWithShape(__Vector3& vEyeResult, const __Vector3& vAt, float fNP)	  // Done..
{
	return m_pvsmgr.CheckCollisionCameraWithShape(vEyeResult, vAt, fNP);						
}

float CDungeonManager::GetHeightNearstPosWithShape(const __Vector3 &vPos, float fDist, __Vector3* pvNormal)		// Done..
{
	return m_pvsmgr.GetHeightNearstPosWithShape(vPos, fDist, pvNormal);							
}

#ifdef _DEBUG
void CDungeonManager::RenderCollisionWithShape(__Vector3 &vPos)																			// No need..
{
}
#endif	

float CDungeonManager::GetHeightWithShape(float fX, float fZ, __Vector3* pvNormal)
{
	return m_pvsmgr.GetHeightWithShape(fX, fZ, pvNormal);
}

bool CDungeonManager::CheckCollisionWithShape(	const __Vector3& vPos,				 // crash location
														const __Vector3& vDir,				   // direction vector
														float fSpeedPerSec,					    // moving speed per second
														__Vector3* pvCol,						 // crash point
														__Vector3* pvNormal,				  // The normal vector of the colliding face
														__Vector3* pVec)						// Polygon of collided face __Vector3[3]
{
	return m_pvsmgr.CheckCollisionWithShape(vPos, vDir, fSpeedPerSec, pvCol, pvNormal, pVec);
}

CN3Shape* CDungeonManager::ShapeGetByIDWithShape(int iID)
{
	return m_pvsmgr.ShapeGetByIDWithShape(iID);
}

CN3Shape* CDungeonManager::PickWithShape(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick)
{
	return m_pvsmgr.PickWithShape(iXScreen, iYScreen, bMustHaveEvent, pvPick);
}



