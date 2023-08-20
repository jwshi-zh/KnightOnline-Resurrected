#pragma once

#include "N3TerrainDef.h"
#include "N3Base.h"

class CN3Texture;
class CN3Terrain;

class CN3TerrainPatch : public CN3Base
{
public:
	BOOL						m_bIsRender;
	POINT						m_ti_LBPoint;
	CN3Terrain*					m_pRefTerrain;
	bool						m_IsBlunt[4];	// Left, Top, Right, Bottom...
	CN3Texture*					m_pRefColorTex;

protected:
	int							m_CellSize;
	int							m_NumCell;
	int							m_iLevel;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;
	int							m_VBSize[MAX_PATCH_LEVEL];
	
	std::list<FANINFO>					m_FanInfoList;
	
	// for level1
	int*						m_pTileTexIndx[2];
	bool*						m_pIsTileFull;

	// lightmap...
	LPDIRECT3DVERTEXBUFFER9		m_pLightMapVB;
	CN3Texture**				m_pRefLightMapTex;
	int							m_NumLightMapTex;

	// Settings for cards that do not support Tile.
	// LPDIRECT3DVERTEXBUFFER9		m_pFoolishTileVB;
	// int							m_NumFoolishTile;

public:
	inline float	UVConvert(float uv);
	int		GetLevel() const { return m_iLevel; }
	void	SetLevel(int level);
	void	Release();
	void	Init();
	void	Init(CN3Terrain* pTerrain);
	void	Tick();
	void	Render();
	
public:
	CN3TerrainPatch();
	virtual ~CN3TerrainPatch();
};
