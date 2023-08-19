#pragma once

#ifdef _3DSERVER
#include "My_3DStruct.h"
#else
#include "N3BaseFileAccess.h"
#endif // end of #ifndef _3DSERVER

class CN3Shape;

const int CELL_MAIN_DEVIDE = 4; // The main cell is divided into 4 X 4 sub cells.
const int CELL_SUB_SIZE = 4; // 4 Meter is the size of the subcell.
const int CELL_MAIN_SIZE = CELL_MAIN_DEVIDE * CELL_SUB_SIZE; // The main cell size is the number of sub cells X the size of the sub cell.
const int MAX_CELL_MAIN = 4096 / CELL_MAIN_SIZE; // The maximum number of main cells is terrain size / main cell size.
const int MAX_CELL_SUB = MAX_CELL_MAIN * CELL_MAIN_DEVIDE; // The maximum number of subcells is the number of divisions of main cell * main cell.

#ifdef _3DSERVER
class CN3ShapeMgr
#else
#include <list>
#include <vector>

class CN3ShapeMgr : public CN3BaseFileAccess
#endif
{
public:
	struct __CellSub
	{
		int 	nCCPolyCount;
		DWORD*	pdwCCVertIndices;

		void Load(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			ReadFile(hFile, &nCCPolyCount, 4, &dwRWC, nullptr);
			if(nCCPolyCount > 0)
			{
				if(pdwCCVertIndices) delete [] pdwCCVertIndices;
				pdwCCVertIndices = new DWORD[nCCPolyCount * 3];
				__ASSERT(pdwCCVertIndices, "New memory failed");
				ReadFile(hFile, pdwCCVertIndices, nCCPolyCount * 3 * 4, &dwRWC, nullptr);
//#if _DEBUG				
//				static char szTrace[256];
//				sprintf(szTrace, "CollisionCheckPolygon : %d\n", nCCPolyCount);
//				OutputDebugString(szTrace);
//#endif
			}
		}
#ifdef _N3TOOL
		void Save(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			WriteFile(hFile, &nCCPolyCount, 4, &dwRWC, NULL);
			if(nCCPolyCount > 0)
				WriteFile(hFile, pdwCCVertIndices, nCCPolyCount * 3 * 4, &dwRWC, NULL);
		}
#endif // end of _N3TOOL

		__CellSub() { memset(this, 0, sizeof(__CellSub)); }
		~__CellSub() { delete [] pdwCCVertIndices; }
	};

	struct __CellMain
	{
		int		nShapeCount; // Shape Count;
		WORD*	pwShapeIndices; // Shape Indices
		__CellSub SubCells[CELL_MAIN_DEVIDE][CELL_MAIN_DEVIDE];

		void Load(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			ReadFile(hFile, &nShapeCount, 4, &dwRWC, nullptr);
			if(nShapeCount > 0)
			{
				if(pwShapeIndices) delete [] pwShapeIndices;
				pwShapeIndices = new WORD[nShapeCount];
				ReadFile(hFile, pwShapeIndices, nShapeCount * 2, &dwRWC, nullptr);
			}
			for(int z = 0; z < CELL_MAIN_DEVIDE; z++)
			{
				for(int x = 0; x < CELL_MAIN_DEVIDE; x++)
				{
					SubCells[x][z].Load(hFile);
				}
			}
		}

#ifdef _N3TOOL
		void Save(HANDLE hFile)
		{
			DWORD dwRWC = 0;
			WriteFile(hFile, &nShapeCount, 4, &dwRWC, NULL);
			if(nShapeCount > 0) WriteFile(hFile, pwShapeIndices, nShapeCount * 2, &dwRWC, NULL);
			for(int z = 0; z < CELL_MAIN_DEVIDE; z++)
			{
				for(int x = 0; x < CELL_MAIN_DEVIDE; x++)
				{
					SubCells[x][z].Save(hFile);
				}
			}
		}
#endif // end of _N3TOOL
		
		__CellMain() { nShapeCount = 0; pwShapeIndices = nullptr; }
		~__CellMain() { delete [] pwShapeIndices; }
	};

	__Vector3* 				m_pvCollisions;

protected:
#ifndef _3DSERVER
	std::vector<CN3Shape*>	m_Shapes;			// The reason I didn't make it a list... is because arrays are much more efficient.
	std::list<CN3Shape*>	m_ShapesToRender;
	std::list<CN3Shape*>	m_ShapesHaveID;		// Shapes that have an ID and can become NPCs....
#endif // end of #ifndef _3DSERVER
	
	float					m_fMapWidth;
	float					m_fMapLength;
	int						m_nCollisionFaceCount;
	__CellMain*				m_pCells[MAX_CELL_MAIN][MAX_CELL_MAIN];

#ifdef _N3TOOL
	std::list<__Vector3>	m_CollisionExtras; // Additional collision check data
#endif // end of #ifedef _N3TOOL

public:
#ifndef _3DSERVER
	CN3Shape* ShapeGetByID(int iID); // An object with a unique ID... Searches for and returns an object that can be used as an NPC.
	CN3Shape* Pick(int iXScreen, int iYScreen, bool bMustHaveEvent, __Vector3* pvPick = nullptr); // returns the location.
	CN3Shape* PickMovable(int iXScreen, int iYScreen, __Vector3* pvPick);
#endif // end of #ifndef _3DSERVER
	void SubCell(const __Vector3& vPos, __CellSub** ppSubCell);
	__CellSub* SubCell(float fX, float fZ) const
	// Returns the cell pointer at the location.
	{
		const int x = (int)(fX / CELL_MAIN_SIZE);
		const int z = (int)(fZ / CELL_MAIN_SIZE);
		
		__ASSERT(x >= 0 && x < MAX_CELL_MAIN && z >= 0 && z < MAX_CELL_MAIN, "Invalid cell number");
		if(x < 0 || x >= MAX_CELL_MAIN || z < 0 || z >= MAX_CELL_MAIN) return nullptr;
		if(nullptr == m_pCells[x][z]) return nullptr;

		const int xx = (((int)fX)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;
		const int zz = (((int)fZ)%CELL_MAIN_SIZE)/CELL_SUB_SIZE;
		
		return &(m_pCells[x][z]->SubCells[xx][zz]);
	}
	float		GetHeightNearstPos(const __Vector3& vPos, float fDist, __Vector3* pvNormal = nullptr);  // Returns the nearest height. If not, returns -FLT_MAX.
	float		GetHeight(float fX, float fZ, __Vector3* pvNormal = nullptr);  // Returns the highest value at the current point. If not, returns -FLT_MAX.
	int			SubCellPathThru(const __Vector3& vFrom, const __Vector3& vAt, int iMaxSubCell, __CellSub** ppSubCells) const; // Returns a cell pointer spanning between vectors.
	float		Width() { return m_fMapWidth; }
	float		Height() { return m_fMapWidth; }

#ifndef _3DSERVER
	void		ReleaseShapes();
	void		RenderCollision(__Vector3& vPos); // Draws a collision mesh at the inserted position. For debugging...
	void		Tick();
	void		Render();
	bool		Load(HANDLE hFile);
	bool		CheckCollisionCamera(__Vector3& vEye, const __Vector3& vAt, float fNP);
	static int SortByCameraDistance(const void* pArg1, const void* pArg2);
#endif // end of #ifndef _3DSERVER

	bool		CheckCollision(	const __Vector3& vPos,
								const __Vector3& vDir,
								float fSpeedPerSec,
								__Vector3* pvCol = nullptr,	
								__Vector3* pvNormal = nullptr,
								__Vector3* pVec = nullptr);

	bool		Create(float fMapWidth, float fMapLength);
	bool		LoadCollisionData(HANDLE hFile);

#ifdef _N3TOOL
	void		MakeMoveTable(short** pMoveArray);	//In the terrain, create a table in which tiles with shapes are set to 1 and tiles without shapes are set to 0.
	int			Add(CN3Shape* pShape);
	bool		AddCollisionTriangle(const __Vector3& v1, const __Vector3& v2, const __Vector3& v3);
	void		GenerateCollisionData();
	bool		Save(HANDLE hFile);
	bool		SaveCollisionData(HANDLE hFile);
#endif // end of _N3TOOL
	
	void Release();
	CN3ShapeMgr();
	virtual ~CN3ShapeMgr();

};
