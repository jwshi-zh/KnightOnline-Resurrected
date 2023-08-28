// LyTerrain.h: interface for the CLyTerrain class.
//	- 2001. 9.24.~ Map Editor용으로 변환...
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3TERRAIN_H__7BCDE4B8_8FBC_432E_B4CB_4A56C5FC29F0__INCLUDED_)
#define AFX_N3TERRAIN_H__7BCDE4B8_8FBC_432E_B4CB_4A56C5FC29F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>

#include "LyTerrainDef.h"
#include "../N3BASE/N3BaseFileAccess.h"

class CN3Texture;
class CQTNode;

class CDTexMng;
class CDTexGroup;
class CDTexGroupMng;
class CDlgSetLightMap;

////////////////////////////////////////////////
//Use multimap	
//Used when saving savegamedata.
//Used when takingg colormap..^^
//
typedef std::multimap<short, short> MMap;
typedef MMap::value_type MMValue;
typedef MMap::iterator MMIter;

typedef std::list<short> LList;
typedef LList::iterator LLIter;

typedef std::list<CQTNode*>::iterator it_QTNode;

class CLyTerrain : public CN3BaseFileAccess  
{
protected:
	__Material					m_Material;
	D3DFILLMODE					m_FillMode;

	//quadtree..
	CQTNode*					m_pRoot;
	CQTNode*					m_pRefCurrNode;

	//Terrain edit related...
	__VertexXyzColor			m_vBrushVertices[MAX_BRUSH_SIZE*MAX_BRUSH_SIZE*9];		// Vertex buffer representing the brush shape 
	WORD						m_wBrushIndices[MAX_BRUSH_SIZE*MAX_BRUSH_SIZE*6*4];		// Index buffer representing the shape of the brush 
	int							m_iBrushIndexCount;										// Index number representing the brush shape 
	int							m_iEditMode;											// Is it Edit mode? 

	CDTexMng*					m_pDTexMng;

	MMap						m_ColorMapTileTree;	// Tree with tiles before creating colormap.

	int							m_iDistanceTable[DISTANCE_TABLE_SIZE][DISTANCE_TABLE_SIZE];
	
public:
	float						m_fBrushIntensityMap[MAX_BRUSH_SIZE][MAX_BRUSH_SIZE];	// A map showing the brush change rate 
	float						m_fHeightScale;
	float						m_fFlatHeight;
	bool						m_bFlaten;


	int							m_iZoneID;

	//LightMap related...
	CDlgSetLightMap*			m_pDlgSetLightMap;
	LPDIRECT3DVERTEXBUFFER8		m_LightMapVB;	
	CN3Texture***				m_ppLightMapTexture;
	bool**						m_ppIsLightMap;
	int							m_iNumLightMap;
	bool						m_bDrawLineLightMap;	// In the light map drawing options.. Whether to draw in line form.. 
	__VertexXyzColor			m_vLineLightMap[2];		// If you draw in the form of a line, you have to display that line on the screen right?..^^ That's the point..

	//LOD related..
	int							m_iHeightLimit;
	int							m_iDistLimit;

	std::list<CQTNode*>			m_RenderNodes;

	LPDIRECT3DVERTEXBUFFER8		m_ColorMapVB;	//	When writing the color map...
	LPDIRECT3DVERTEXBUFFER8		m_TileVB;		// One tile and one lightmap 
	
	//common..
	MAPDATA**		m_ppMapData;
	BOOL**			m_ppRenderInfo;

	//texture..	
	CN3Texture**	m_pColorTexture;
		
	RECT			m_VisibleRect;	// Visible area in tile units.. 
	POINT			m_EyePos;		// Tile unit.. 

	int				m_iHeightMapSize; // Concept similar to Cell Count -> Subtract 1 to obtain Cell Count.
	int				m_iMaxLevel;

	int				m_iNumTileMap;	// Number of tilemaps we currently have.. 

	int				m_iColorMapTexSize;
	int				m_iColorMapPixelPerUnitDistance;
	int				m_iNumColorMap;

	//related to tile direction... 
	float			m_fTileDirU[8][4];
	float			m_fTileDirV[8][4];
	float			m_fTileDirUforColorMap[8][4];
	float			m_fTileDirVforColorMap[8][4];

	// DTex related...
	LPDIRECT3DVERTEXBUFFER8		m_pDTexVB;
	__VertexT1*					m_pDTexVertices;
	int							m_DTexFaceCnt;
	int							m_DTexBuff[5000];
	int							m_DTexAttrTable[9][14];
	int							m_DTexDirTable[9][14];

	// ColorMap  related...
	LPDIRECT3DVERTEXBUFFER8		m_pColorMapTmpVB;
	__VertexTransformedT2*		m_pColorMapTmpVertices;

	//Brush  related..
	BOOL						m_bBrushFlat;	// Do you want to flatten the brush that raises and lowers the terrain? 

protected:
	int DetectRealLightMap(int sx, int sz, int range);
	void MakeDistanceTable();
	void FillLightMap(int x, int z, int lcx, int lcz, DWORD Color, float alpha);
	void SetLightMap(int x, int z, int lcx, int lcz);

	void PutColorMapTile(int x, int z);		// Put tiles to create colormap...(Queue those tiles that haven't been created yet)...
	void VerifyTexInfo(LPDTEXINFO pTex);
	void SetColorMap(int x, int y);
	void SetPertinentTile(CDTexGroupMng* pDTexGroupMng, LPDTEXINFO pDest1, LPDTEXINFO pDest2); 

	inline	int	Log2(int x);
	inline	void ClearRenderInfo();
	void	SetVisibleRect();	// Tile unit..

	//map editor  related..
	//
	void SetDTexManual(int x, int y);	// Manual capture 
	void SetDTex(int x, int y);			// Auto capture. 
	BOOL IntersectTriangle( const D3DXVECTOR3 orig, const D3DXVECTOR3 dir, 
						D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, float* t, float* u, float* v);
	void Heighten(POINT ptCenter, float fHeight);	// // Function to heighten or lower the terrain 
	void Smooth(POINT ptCenter);		// Smooth the terrain by comparing it to the height of the surrounding map. 
	void Flaten(POINT ptCenter);
	void SetApexHeight(int x, int z, float fHeight);		// Set the height value of the heightmap 
	float GetApexHeight(int x, int z) const;				// Get the height value of the heightmap 
	float GetFallOffValue(float fFallOff, float x);			// Expression used to calculate BrushIntensity.

	//Related to improved light map...(2002. 11. 13..) 
	void ConvertLightMapToolDataV2toV3();
	//
			
public:
	void UpdateBrushArea(POINT ptCenter);				// Update brush area display vertex 
	void RenderBrushArea();								// Draw brush area mark 

	void	ExportHeightBMP(const char* szPathName);
	void	ImportHeightBMP(const char* szPathName);

	void	MakeMoveTable(short** ppEvent);
	void	Init(int HeightMapSize = 5);
	bool	LoadFromFile(const char* lpszPath);
	bool	SaveToFile(const char* lpszPath);
	bool	SaveToFilePartition(const char* lpszPath, float psx, float psz, float width);
	void	Tick();
	void	Render();
	void	Release();

	void	MakeGameLightMap(char* szFullPathName);
	void	MakeGameColorMap(char* szFullPathName);
	void	GeneraterColorMap(bool bIsAll = false);
	void	TilingAll(); // Paint the entire terrain with the selected tiles.. 

	void	UpDateDistLimit();

	CN3Texture* GetTileTex(int id);
	void	SetFillMode(D3DFILLMODE fillmode);
	float	GetHeight(float x, float z);

	//map editor  related..
	//
	SIZE GetPatchNum(float fSize);
	BOOL MouseMsgFilter(LPMSG pMsg);	// Handle mouse messages when terrain is modified 
	bool Pick(int x, int y, __Vector3* vec, POINT* pHeightMapPos = NULL);
	void Import(LPCTSTR pFileName, float fSize);
	void ImportHeight(LPCTSTR pFileName);
	void SaveServerData(HANDLE hFile);
	void SetEditMode(int iEditMode);		// Change to Terrain Edit mode 
	void UpdateBrushIntensityMap(int iShape, int iSize, float fFallOff);	// Reconstruct the IntensityMap according to the shape and size of the brush
	void SaveGameData(HANDLE hFile);
	void ColorMapImport(LPCTSTR lpszPathName);
	void ColorMapExport(LPCTSTR lpszPathName);
	void GenerateMiniMap(LPCTSTR lpszPathName, int size);
	//
		
	CLyTerrain();
	virtual ~CLyTerrain();
};

#endif // !defined(AFX_N3TERRAIN_H__7BCDE4B8_8FBC_432E_B4CB_4A56C5FC29F0__INCLUDED_)

