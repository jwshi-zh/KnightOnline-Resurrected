#pragma once

#include "GameBase.h"
// #include "GameProcedure.h"
#include <list>
#include "GrassBoard.h"

#define GRASS_MAX 11 // (40/4(TILE_SIZE)) + 1
#define GRASS_TILENUM 11
#define GRASS_MAKENEW	1
#define	GRASS_MAKEUSE	2

class CGameProcedure;
class CN3Terrain;

class CGrassMng   : public CGameBase
{
protected:

// std::list<class CGrassBoard*>	m_Grasses;
	CGrassBoard		m_pGrasses[GRASS_TILENUM][GRASS_TILENUM];	// grass sown area
// __Vector3		m_pCount[GRASS_MAX];
	float			m_fChkRange[4];	// For location search with camera

// __Vector3		m_vCamPo;
	bool			m_bChkZoneChange;
	unsigned short	m_usDrawIndex;

	CN3Texture*		m_txTexture[8];
#ifdef _DEBUG
	char			m_strFileName[8][_MAX_PATH];	// Back up picture names for later debugger
#endif
	int				m_iFileMaxNum;

protected:
	void	LoadFromFile(const char* szFileName,__Vector3 CamPo);
	int		SetFile(int iTexIndex,unsigned char ucTexOrgIndex,__Vector3 CamPo);
	float	RandomGenf(float max, float min);
	void	ChkThisZoneUseGrass(int nGrassUseOrder) const;

	void	FindGrassIndex(unsigned char GrassIndex,int& nFineGrass,unsigned char& ucFineIndex);
	void	FindGrassIndex(const unsigned char uCGrassMngOrder,int* pnInputGrass,unsigned char* GrassIndex,int& nGrassTotNum);

	void	ChkTileRange(float fCamX,float fCamZ);	// Inter-tile movement check, tick
	void	FineNewTile(WORD* Tile,int& iCount,float* ChkRange,float* LargeRange);

public:
	void	Init(__Vector3 CamPo);
	void	Tick(CGameProcedure* pProc);
	void	Render();
	void	Release();

	bool	ChangeZone();

	bool	IsInRect(float fPoX,float fPoY,float* fRange);	// Check if you are in range
public:
	CGrassMng();
	virtual ~CGrassMng();
};
