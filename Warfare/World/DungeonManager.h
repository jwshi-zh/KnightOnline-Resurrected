#pragma once

#include "N3WorldBase.h"
#include "PvsMgr.h"

class CDungeonManager : public CN3WorldBase
{
	friend class CN3WorldManager;

	// ..
	CPvsMgr	m_pvsmgr;

	// Function..
	void InitWorld(int iZoneID, const __Vector3& vPosPlayer);
	void Tick();

	CN3Terrain* GetTerrainRef();
	CN3SkyMng* GetSkyRef();

	// Terrain..
	bool CheckCollisionCameraWithTerrain(__Vector3& vEyeResult, const __Vector3& vAt, float fNP);
	float GetHeightWithTerrain(float x, float z, bool bWarp = false);
	BOOL PickWideWithTerrain(int x, int y, __Vector3& vPick);
	bool CheckCollisionWithTerrain(__Vector3& vPos, __Vector3& vDir, float fVelocity, __Vector3* vCol);
	void GetNormalWithTerrain(float x, float z, __Vector3& vNormal);
	float GetWidthByMeterWithTerrain(); 
	bool IsInTerrainWithTerrain(float x, float z, __Vector3 vPosBefore = __Vector3());
	bool CheckInclineWithTerrain(const __Vector3& vPos, const __Vector3& vDir, float fIncline);

	// Shapes..
	bool CheckCollisionCameraWithShape(__Vector3& vEyeResult, const __Vector3& vAt, float fNP);
	float GetHeightNearstPosWithShape(const __Vector3& vPos, float fDist, __Vector3* pvNormal = nullptr); 
#ifdef _DEBUG
	void RenderCollisionWithShape(__Vector3 &vPos);
#endif	
	float GetHeightWithShape(float fX, float fZ, __Vector3* pvNormal = nullptr); 
	CN3Shape* ShapeGetByIDWithShape(int iID);
	CN3Shape* PickWithShape(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick = nullptr);
	bool CheckCollisionWithShape(	const __Vector3& vPos,				 // crash location
														const __Vector3& vDir,				   // direction vector
														float fSpeedPerSec,					    // moving speed per second
														__Vector3* pvCol = nullptr,			 // crash point
														__Vector3* pvNormal = nullptr,		 // The normal vector of the colliding face
														__Vector3* pVec = nullptr);			// Polygon of collided face __Vector3[3]

	// Rendering..
	void RenderTerrain();
	void RenderShape();	

public:
	CDungeonManager();
	virtual ~CDungeonManager();

};
