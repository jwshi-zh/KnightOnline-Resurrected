// PondMesh.h: interface for the CPondMesh class.
//
// 단지 가장큰 사각형의 영역과 땅의 지형이될 영역만 있음 (4m 씩의 점간의 거리)
// client에서 구현
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PONDMESH_H__314B2191_7B76_461C_AFD2_FDBC105914FC__INCLUDED_)
#define AFX_PONDMESH_H__314B2191_7B76_461C_AFD2_FDBC105914FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_PONDMESH_LINE 200
#define MAX_PONDMESH_VERTEX 200 * 4
// #define MAX_PONDMESH_INDEX		199*18
// #define INDEXBUFFER_PERLINE		18

#include "../N3Base/N3BaseFileAccess.h"
#include <list>

class CLyTerrain;

class CPondMesh : public CN3BaseFileAccess
{
public: // existing function
	CN3Texture *TexGet() const { return m_pTexture; }
	int VertexCount() const { return m_iVC; }
	int RectVCount() const { return m_iRectVC; }
	int IndexCount() const { return m_iIC; }
	int LastVertexCount() const { return GetWaterScaleWidht(); }

	void Rotation(__Matrix44 mRot, __Vector3 vCenter);

protected:
	// selected point control
	typedef struct __SELECT_PO
	{
		int ix; // number on m_iWaterScaleWidth
		int iz; // number on m_iWaterScaleHeight
	};

	typedef std::list<__SELECT_PO *>::iterator it_SelVtx;
	std::list<__SELECT_PO *> m_vSelect; // manage selected points

	// default data
	int m_iPondID;
	float m_fWaterHeight;	 // water level
	int m_iWaterScaleWidth;	 // number of points in the horizontal direction
	int m_iWaterScaleHeight; // Number of vertical points
	DWORD m_dwPondAlpha;	 // alpha of water
	float m_fTU;
	float m_fTV;
	float m_fWaterScaleX, m_fWaterScaleZ; // distance between points when sorted

	int m_iBackUpWidht;
	int m_iBackUpHeight;
	__Vector3 m_pVertices[MAX_PONDMESH_VERTEX];	   // Value used for storage and backup
	__VertexXyzT2 m_pViewVts[MAX_PONDMESH_VERTEX]; // value to be displayed on the screen
	WORD *m_pdwIndex;							   //[MAX_PONDMESH_LINE*MAX_PONDMESH_LINE*6];
	int m_iIC;									   // Index Buffer Count.
	int m_iVC;									   // Vertex Count.
	CN3Texture *m_pTexture;

	BOOL m_bUVState; // with respect to TRUE point, FALSE flat (uv noise)

	// data about the pond area
	__Vector3 m_vDrawBox[4];		// The area to be displayed on the screen (clockwise from the top left)
	__VertexXyzColor m_ViewRect[5]; // line to draw on screen
	int m_iRectVC;
	__VertexXyzT2 m_pRectVts[100]; // Points to be drawn on the area line

	__Vector3 m_vSelectBox[2]; // Area of points when points are selected (when points are out of the screen area)

	// Refer to geographic information
	CLyTerrain *m_pTerrain;

public:
	CPondMesh();
	virtual ~CPondMesh();

	virtual void Release();
	virtual bool Load(HANDLE hFile);
	virtual bool Load1000(HANDLE hFile);
	virtual bool Load1001(HANDLE hFile);
	virtual bool Save(HANDLE hFile);
	void Render();
	void RenderVertexPoint(); // Redraw only the points so that they are visible

	__Vector3 GetCenter();
	float SettingDrawPos(__Vector3 vPos1, __Vector3 vPos2, __Vector3 vPos3, __Vector3 vPos4, int iLinePosNum);
	__VertexXyzT2 *const GetVertex(int iIndex)
	{
		if (iIndex < 0 || iIndex >= m_iVC)
			return NULL;
		return m_pViewVts + iIndex;
	}
	__VertexXyzT2 *const GetRcVertex(int iIndex)
	{
		if (iIndex < 0 || iIndex >= m_iRectVC)
			return NULL;
		return m_pRectVts + iIndex;
	}

	BOOL SetTextureName(LPCTSTR pszFName);

public:
	// new or modified
	void SetTerrain(CLyTerrain *pCTerrain) { m_pTerrain = pCTerrain; }

	void MakeDrawRect(__Vector3 *pvPos); // Create a border around the area of the pond that will be visible on the screen
	void UpdateDrawPos();				 // Points to be drawn on the area line

	void UpdateWaterHeight();				  // set to the current water level
	void UpdateMovePos(__Vector3 vMovingPos); //	movement

	void MakePondPos(); // create a pond in the current box
	void MakeIndex();
	void ReCalcUV();	// Sprinkle evenly regardless of the dot
	void ReCalcVexUV(); // Sprinkle with involvement of dots

	void EstimatePos(); // Randomly set the number of dots according to the size of the line////

	void ClearSelectPos();												 // Initialize data about selection point
	void InputSelectPos(float fX, float fY, float fZ, int iVC = -1);	 // input data for the selected points
	BOOL InputDummyMovingPos(__Vector3 vDummyMovingPos, BOOL bMovePond); // To process the remaining points when the dummy has moved

protected:
	void MovingPos();
	void CalcuWidth(int iSx, int iSy, int iEx = -1, int iEy = -1); // do the first left and right calculation
	void SetAllPos(int iSx, int iSy, int iEx = -1, int iEy = -1);  // Calculate the coordinates of the points other than the first calculated points
	void ReInputBackPos();										   // Put the newly moved coordinates into the backup

public: // function used in the dialog box
	void SetPondID(int iPondID) { m_iPondID = iPondID; }
	void SetWaterHeight(float fHeight);
	void SetWaterScaleWidht(int iWidhtNum)
	{
		m_iWaterScaleWidth = iWidhtNum;
		UpdateDrawPos();
		MakePondPos();
	}
	void SetWaterScaleHeight(int iHeightNum)
	{
		m_iWaterScaleHeight = iHeightNum;
		UpdateDrawPos();
		MakePondPos();
	}
	void SetAlphaFactor(DWORD dwAlpha) { m_dwPondAlpha = dwAlpha; }
	void SetTU(float fTU)
	{
		m_fTU = fTU;
		ReCalcUV();
	}
	void SetTV(float fTV)
	{
		m_fTV = fTV;
		ReCalcUV();
	}
	void SetLeft(float fLeft);
	void SetTop(float fTop);
	void SetRight(float fRight);
	void SetBottom(float fBottom);
	void SetChangUVState() { m_bUVState ^= 1; }

	int GetPondID() const { return m_iPondID; }
	float GetWaterHeight() const { return m_fWaterHeight; }
	int GetWaterScaleWidht() const { return m_iWaterScaleWidth; }
	int GetWaterScaleHeight() const { return m_iWaterScaleHeight; }
	DWORD GetAlphaFactor() const { return m_dwPondAlpha; }
	float GetTU() const { return m_fTU; }
	float GetTV() const { return m_fTV; }
	float GetLeft() const { return m_vDrawBox[0].x; }
	float GetTop() const { return m_vDrawBox[0].z; }
	float GetRight() const { return m_vDrawBox[2].x; }
	float GetBottom() const { return m_vDrawBox[2].z; }
	float GetWaterWidthLength() const { return m_fWaterScaleX; }
	float GetWaterHeightLength() const { return m_fWaterScaleZ; }
	BOOL GetChangUVState() const { return m_bUVState; }
};

#endif // !defined(AFX_PONDMESH_H__314B2191_7B76_461C_AFD2_FDBC105914FC__INCLUDED_)
