#pragma once

#include "N3BaseFileAccess.h"
#include "N3Texture.h"
#include "N3TerrainDef.h"
#include "N3TerrainPatch.h"

#include <string>

// class CN3Terrain : public CGameBase
class CN3Terrain : public CN3BaseFileAccess
{
public:
	// common..
	__Material			m_Material;
	D3DSHADEMODE		m_ShadeMode;
	D3DFILLMODE			m_FillMode;
	int					m_iLodLevel;
			
	// Patch...
	CN3TerrainPatch**	m_ppPatch;
	POINT				m_pat_LBPos;
	POINT				m_pat_PrevLBPos;
	RECT				m_pat_BoundRect;	// Relative coordinates for LBPos..
	int					m_iNumPatch;
	int					m_pat_Center2Side;	// The length from the center point to one side.
	int					m_iDistanceTable[DISTANCE_TABLE_SIZE][DISTANCE_TABLE_SIZE];
	
	// MapInfo..
	MAPDATA*			m_pMapData;
	int					m_ti_MapSize;				// How many cells go in... 4096Meter -&gt; 1024 + 1
	int					m_pat_MapSize;				// The number of patches.. depends on the size..
	__Vector3*			m_pNormal;

	// LightMap Info..
	POINT				m_pat_CenterPos;
	std::map<DWORD, CN3Texture*>		m_LightMapPatch[3][3];
	
	// bool**			m_ppIsLightMap;
	// CN3Texture***		m_pppLightMapTex;

	// Patch
	float**				m_ppPatchRadius;
	float**				m_ppPatchMiddleY;

	// Tile..
	POINT				m_ti_CenterPos;
	POINT				m_ti_PrevCenterPos;

	// Texture...
	int					m_NumTileTex;				// Number of Tile Textures
	CN3Texture*			m_pTileTex;

	// ColorMap..
	CN3Texture**		m_ppColorMapTex;
	int					m_iNumColorMap;				// The color map is divided and stored. Number =

	// The pattern map overlaid on the color map.
	CN3Texture			m_pBaseTex;

	// tile direction.
	float m_fTileDirU[8][4];
	float m_fTileDirV[8][4];

	// Grass Attr;
	char				m_pGrassTextureName[MAX_GRASS][MAX_PATH];
	char				m_pGrassFileName[MAX_PATH];
	unsigned char*		m_pGrassAttr;
	unsigned char*		m_pGrassNum;
	int					m_iNumGrass;

	class CN3River*		m_pRiver;
	class CN3Pond*		m_pPond;

	bool				m_bAvailableTile;

protected:
	void	SetNormals();
	void	SetLightMap(int dir);
	void	ReplaceLightMapPatch(int x, int z, std::map<DWORD, class CN3Texture*>& LightMapPatch);
	void	SetLightMapPatch(int x, int z, HANDLE hFile, int* pAddr);

	
	void	TestAvailableTile();
	void	MakeDistanceTable();

	inline	int	Log2(int x);	// Multipliers of 2 only....
	int Real2Tile(float x){ return ((int)x / TILE_SIZE); } // Real coordinates -&gt; Tile coordinates... (absolute coordinates)
	int Tile2Patch(int x) { return (x / PATCH_TILE_SIZE); } // Tile coordinates -&gt; Patch coordinates... (absolute coordinate system)
	int Real2Patch(float fX) { return ( ((int)fX / TILE_SIZE) / PATCH_TILE_SIZE ); } // Real coordinates -&gt; patch coordinates.. (absolute coordinate system)
		
	void	LoadTileInfo(HANDLE hFile);
	bool	CheckRenderablePatch();
	bool	CheckMovePatch();
	bool	CheckBound();
	void	DispositionPatch();
	void	SetBlunt();
	void	LoadGrassInfo();
	
public:
	CN3Texture* GetLightMap(int tx, int tz);
	float	GetWidthByMeter() const { return (m_ti_MapSize - 1) * TILE_SIZE; }
	int		GetLODLevel() { return m_iLodLevel; }
	bool	SetLODLevel(int level);
	float	GetHeight(float x, float z);
	void	Release();
	void	Init();
	bool	Load(HANDLE hFile);
	void	Tick();
	void	Render();
	void	SetFillMode(D3DFILLMODE FillMode) { m_FillMode = FillMode; }
	void	SetShadeMode(D3DSHADEMODE ShadeMode) {m_ShadeMode = ShadeMode; }
		
	CN3Terrain();
	virtual ~CN3Terrain();

public:	// additional........
	bool			GetTileTexInfo(float x, float z, TERRAINTILETEXINFO& TexInfo1, TERRAINTILETEXINFO& TexInfo2);
	CN3Texture*		GetTileTex(int x, int z);
	MAPDATA			GetMapData(int x, int z);
	unsigned short	GetGrassAttr(int x, int z) const;
	bool			LoadColorMap(const std::string& szFN);
	void			GetNormal(float x, float z, __Vector3& vNormal);
	bool			IsInTerrain(float x, float z);
	// ..
	BOOL			Pick(int x, int y, __Vector3& vPick);
	BOOL			PickWide(int x, int y, __Vector3& vPick);
	void			CalcCollisionTerrainByOTPlayer(__Vector3, __Vector3, __Vector3& );

	bool			CheckIncline(const __Vector3& vPos, const __Vector3& vDir, float fIncline); // true if the slope value from the current position and direction is greater than the value entered as a factor (if it is a place where you cannot climb), false if it is less
	bool			CheckCollisionCamera(__Vector3& vEye, const __Vector3& vAt, float fNP); // The calculated value for vEye is also entered. Calculated by inserting the camera Near Plane.
	BOOL			CheckCollisionByHeight(const __Vector3& vPos, float fUnitSize, float& fHeight)
	{
		fHeight = this->GetHeight(vPos.x, vPos.z);
		if(vPos.y < fHeight + fUnitSize) return TRUE; // If the current height is less than the terrain height + size
		return FALSE;
	}
	bool			CheckCollision(__Vector3& vPos, __Vector3& vDir, float fVelocity, __Vector3* vpCol);
	
};
