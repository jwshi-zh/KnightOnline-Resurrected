#pragma once

#include "N3Base.h"
#include "N3Shape.h"

#define INDOOR_FOLDER "N3Indoor\\"
const float fBaseVolumnSize = 1.0f;

enum e_ExtBool	{ TYPE_UNKNOWN = 0, TYPE_TRUE };

class CPvsMgr;
class CPortalVolume;

// 
typedef struct tagShapeInfo	: public CN3Transform	
{
	int									m_iID;
	std::string						m_strShapeFile;

	int									m_iBelong;				// Affiliation - 0: No Affiliation 1: Elmorad 2: Carus 3:?? ....
	int									m_iEventID;				 // Event ID
	int									m_iEventType;		  // Event Type
	int									m_iNPC_ID;			   // NPC ID for objects used as NPCs
	int									m_iNPC_Status;	    // Default Status for objects used as NPCs

	CN3Shape*					m_pShape;

	// ..
	tagShapeInfo()
	{
		m_iID = -1;
		m_strShapeFile = "";
		m_pShape = nullptr;		

		m_iBelong = 0;
		m_iEventID = 0;	
		m_iEventType = 0;
		m_iNPC_ID = 0;	
		m_iNPC_Status = 0;	
	}
	
	const tagShapeInfo& operator = (const tagShapeInfo& si)
	{
		m_iID = si.m_iID;
		m_strShapeFile = si.m_strShapeFile;
		m_pShape = si.m_pShape;
		m_iBelong = si.m_iBelong;
		m_iEventID = si.m_iEventID;	
		m_iEventType = si.m_iEventType;
		m_iNPC_ID = si.m_iNPC_ID;	
		m_iNPC_Status = si.m_iNPC_Status;
		return *this;
	}

} ShapeInfo;

typedef struct tagVisPartIndex {
	int m_iPartIndex;
	std::vector<int> m_ivVector;
} __VPI;

typedef struct tagShapePart
{
	int									m_iID;
	std::list<__VPI>			 m_viIndex;									// Visible Index List..

	tagShapePart()
	{
		m_iID = -1;
	}	
	void Clear()
	{
		__VPI vpi;
		auto vit =	m_viIndex.begin();	
		while (vit != m_viIndex.end())
		{
			vpi = *vit++;
			vpi.m_ivVector.clear();
		}
		m_viIndex.clear();
	}
	~tagShapePart()
	{
		Clear();
	}
} ShapePart;

typedef struct tagCollisionIndex
{
	int m_iID;
	std::vector<int> m_ivVector;
	tagCollisionIndex()
	{
		m_iID = -1;
	}
} __ColIndex;

typedef struct tagIDAndPriority
{
	int						m_iID;
	int						m_iPriority;
}	IDAndPriority;

typedef struct tagVisPortalPriority
{
	CPortalVolume* m_pVol;
	int						m_iPriority;

	tagVisPortalPriority()
	{
		m_pVol = nullptr;
		m_iPriority = 100;
	}
}	VisPortalPriority;

class CPortalVolume	: public CN3Transform
{
	friend class CPvsMgr;

	int m_iID;
	CPvsMgr* m_pManager;

	e_ExtBool	m_eRenderType;

	const float m_fOffs;				// Normal offset...
	const float m_fHeightOffs;		// height offset.
	const float m_fVolOffs;			  // Volume offset offset.
	const float m_fPickIncline;		// Picking tilt check..
	const float m_fCameraOffs;		// camera height.
	
	// 
	std::list<ShapeInfo* > m_plShapeInfoList;
	std::list<IDAndPriority>	m_piVisibleIDList;						// Intermediate data required for loading..
	std::list<ShapePart* > m_lpShapePartList;	
	std::list<__ColIndex* >	m_lpShapeColPartList;
	std::list<VisPortalPriority>	m_pVisiblePvsList;		
	// 

	__Vector3					  m_pvVertex[8];
	unsigned short			   m_pIndex[36];

	int									m_iPriority;							// .. initialized to 100..

private:
	void DeleteAllPvsObj();
	bool Load(HANDLE hFile);
	bool IsInVolumn(__Vector3 vec) const;

	void Render();
	void RenderShape();
	void RenderCollision();

	// 
	bool CheckCollisionCameraWithTerrain(__Vector3& vEyeResult, const __Vector3& vAt, float fNP);
	bool CheckCollisionCameraWithShape(__Vector3& vEyeResult, const __Vector3& vAt, float fNP);
	bool GetHeightWithTerrain(float fx, float fz, float& fy);
	float GetHeightNearstPosWithShape(const __Vector3& vPos, float fDist, __Vector3* pvNormal = nullptr); 
	bool IsInTerrainWithTerrain(__Vector3& vec);
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
	CPortalVolume();
	virtual ~CPortalVolume();

};
