#pragma once

#include "N3BaseFileAccess.h"
#include "N3Shape.h"

#include "PortalVolume.h"
#include <string>

const int ciVersion = 1;

class CPvsMgr : public CN3BaseFileAccess
{
	friend class CDungeonManager;
	friend class CPortalVolume;

	const std::string m_IndoorFolder;
	const float m_fVolumeOffs;		// Volume check height..

	std::list<CPortalVolume*> m_pPvsList;

	// .. Current Portal Volume.. ^^
	CPortalVolume* m_pCurVol;

public:
	static CN3Mng <class CN3Shape> s_MngShape;
	static CN3Mng <class CN3ShapeExtra> s_MngShapeExt;
	static std::list<ShapeInfo* > s_plShapeInfoList;
	static ShapeInfo* GetShapeInfoByManager(int iID);

private:
	void Tick(bool bWarp = false, __Vector3 vPos = __Vector3());
	void Render();

	void DeleteAllPvsObj();

	// String Cryptograph.. ^^
	static std::string ReadDecryptString(HANDLE hFile);
	std::string GetIndoorFolderPath() const {	return m_IndoorFolder; }

	CPortalVolume* GetPortalVolPointerByID(int iID);

	// 
	bool CheckCollisionCameraWithTerrain(__Vector3& vEyeResult, const __Vector3& vAt, float fNP);
	bool CheckCollisionCameraWithShape(__Vector3& vEyeResult, const __Vector3& vAt, float fNP);
	float GetHeightWithTerrain(float x, float z, bool bWarp = false );
	float GetHeightNearstPosWithShape(const __Vector3& vPos, float fDist, __Vector3* pvNormal = nullptr); 
	bool IsInTerrainWithTerrain(float x, float z, __Vector3 vPosBefore = __Vector3());
	float GetHeightWithShape(float fX, float fZ, __Vector3* pvNormal = nullptr); 
	BOOL PickWideWithTerrain(int x, int y, __Vector3& vPick);
	CN3Shape* PickWithShape(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick = nullptr);
	CN3Shape* ShapeGetByIDWithShape(int iID);
	bool CheckCollisionWithShape(	const __Vector3& vPos,				 // crash location
														const __Vector3& vDir,				   // direction vector
														float fSpeedPerSec,					    // moving speed per second
														__Vector3* pvCol,						 // crash point
														__Vector3* pvNormal,				  // The normal vector of the colliding face
														__Vector3* pVec);						// Polygon of collided face __Vector3[3]

public:
	CPvsMgr();
	virtual ~CPvsMgr();

	// ..
	bool Load(HANDLE hFile);
	bool LoadOldVersion(HANDLE hFile, int iVersionFromData);
};
