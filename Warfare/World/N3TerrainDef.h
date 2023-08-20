#pragma once

#include <list>
#include <map>

const int PATCH_TILE_SIZE = 8;		// Each patch contains 8x8 tiles.
const int TILE_SIZE = 4;			// 4m long per tile
const int MAX_LOD_LEVEL = 10;		// Level with the highest LOD level Most Detail...
const int MIN_LOD_LEVEL = 0;		// The lowest LOD level, the roughest...
const int MAX_PATCH_LEVEL = 3;		// 2 ^ 3 = 8.
const int PATCH_PIXEL_SIZE = 32;	// The number of pixels in one patch.
const int UNITUV = 32;				// The number of tiles that fit into one colormap texture.
const int LIGHTMAP_TEX_SIZE = 16;	// Lightmap texture size per tile.
const int TILE_PIXEL_SIZE = 128;	// The actual pixel size of a tile.
const int COLORMAPTEX_SIZE = 128;	// Pixel size of colormap texture..128x128
const int DISTANCE_TABLE_SIZE = 64; // Distance table size in cells (64 x 64)..
const int MAX_GRASS	= 8;
const int MAX_TERRAIN_SOUND_EFFECT = 4;

enum { DIR_LT = 0, DIR_CT = 1, DIR_RT = 2,
		DIR_LM = 3, DIR_CM = 4, DIR_RM = 5,
		DIR_LB = 6, DIR_CB = 7, DIR_RB = 8,
		DIR_WARP = 9 }; 

// Structure Definitions..
typedef struct __MapData			// map data...
{
	float	fHeight;				// Whether the tile texture is full or not..
	DWORD	bIsTileFull	: 1;		// The height of the terrain...
	DWORD	Tex1Dir		: 5;		// Direction for taking the first tile texture.
	DWORD	Tex2Dir		: 5;		// The direction to take the second tile texture.
	DWORD	Tex1Idx		: 10;		// Index of the first tile texture.
	DWORD	Tex2Idx		: 10;		// Index of the second tile texture.
	
	__MapData()
	{
		bIsTileFull = true;
		fHeight = FLT_MIN;
		Tex1Idx = 1023;
		Tex1Dir = 0;
		Tex2Idx = 1023;
		Tex2Dir = 0;
	}
} MAPDATA, *LPMAPDATA;

typedef struct __FanInfo
{
	int		NumFace;
	int		NumVertex;
} FANINFO, *LPFANINFO;

typedef struct __TerrainTileTexInfo
{
	class CN3Texture* pTex;
	float u;
	float v;
	
	__TerrainTileTexInfo()
	{
		pTex = nullptr;
		u = v = 0.0f;
	}
} TERRAINTILETEXINFO, *LPTERRAINTILETEXINFO;

typedef struct __TerrainBGMInfo
{
	std::string szBGM[MAX_TERRAIN_SOUND_EFFECT];
	float		fBGMRegenTime[MAX_TERRAIN_SOUND_EFFECT];
	std::string szBGE[MAX_TERRAIN_SOUND_EFFECT];
	float		fBGERegenTime[MAX_TERRAIN_SOUND_EFFECT];

	__TerrainBGMInfo()
	{
		for(int i=0;i<MAX_TERRAIN_SOUND_EFFECT;i++)
		{
			szBGM[i] = "";
			fBGMRegenTime[i] = 0.0f;
			szBGE[i] = "";
			fBGERegenTime[i] = 0.0f;
		}
	}
} TERRAINBGMINFO, *LPTERRAINBGMINFO;
