#include "pch.h"
#include "GameProcMain.h"
#include "N3WorldManager.h"
#include "N3Terrain.h"
#include "GameEng.h"
#include "GrassMng.h"

enum {GRASS_LEFT=0,GRASS_TOP,GRASS_RIGHT,GRASS_BOTTOM};

CGrassMng::CGrassMng()
{
	Release();
}

CGrassMng::~CGrassMng()
{
	CLogWriter::Write("CGrassMng::~CGrassMng -> 소멸자"); // TmpLog1122
	Release();
	CLogWriter::Write("CGrassMng::~CGrassMng -> 소멸자 finish"); // TmpLog1122
}


void CGrassMng::Init(__Vector3 CamPo)
{	
	for(int i=0;i<GRASS_TILENUM;++i)
		for(int j=0;j<GRASS_TILENUM;++j)
		{
			m_pGrasses[i][j].Init(CamPo,BOARD_Y);
			m_pGrasses[i][j].SetBrightInit(12,15);
		}
}


void CGrassMng::Tick(CGameProcedure* pProc)
{
	if(m_bChkZoneChange==false) return;

	__Vector3 vCamPos = pProc->s_pEng->s_CameraData.vEye;	// current camera position

	ChkTileRange(vCamPos.x,vCamPos.z);

	BOOL IsCamOut;
	CGrassBoard* pGrassBoard = &m_pGrasses[0][0];
	for(int i=0;i<GRASS_TILENUM;++i)
		for(int j=0;j<GRASS_TILENUM;++j,++pGrassBoard)
		{
			pGrassBoard->ScaleSet(1.5f, 1.5f, 1.5f);

			// Do not render if off-camera.
			vCamPos = pGrassBoard->GetPo();
			IsCamOut = pProc->s_pEng->s_CameraData.IsOutOfFrustum(vCamPos,3);
			if(IsCamOut==TRUE)
			{
				pGrassBoard->m_bCamOut = TRUE;
				continue;
			}

			pGrassBoard->m_bCamOut = FALSE;
			pGrassBoard->Tick(ACT_WORLD->GetTerrainRef());
		}

/*
	CGrassBoard*   pGrass = NULL; 
	__Vector3	  vPivot;		

	unsigned char GrassIndex=0;
	int mRand,nRand;
	float p,n;

	it_GrassBoard it = m_Grasses.begin();
	int iSize = m_Grasses.size();
	for(int i = 0; i < iSize; i++, it++)
	{
		pGrass = *it;

		float XResult = vCamPos.x - m_pCount[i].x;
		float ZResult = vCamPos.z - m_pCount[i].z;

		if(XResult >= 18 || XResult < -18 || ZResult >= 18 || ZResult < -18)
		{
			do{
				mRand = rand();	p = (mRand%180)/10.000;
				nRand = rand(); n = (nRand%180)/10,000;
			}while(p<15 && n<15);

			if(XResult > 0 && ZResult > 0)
			{
				m_pCount[i].x = (float)((m_pCount[i].x + XResult) + p);
				m_pCount[i].z = (float)((m_pCount[i].z + ZResult) + n);
			}

			else if(XResult > 0 && ZResult < 0)
			{
				m_pCount[i].x = (float)((m_pCount[i].x + XResult) + p);
				m_pCount[i].z = (float)((m_pCount[i].z + ZResult) - n);
			}

			else if(XResult < 0 && ZResult > 0)
			{
				m_pCount[i].x = (float)((m_pCount[i].x + XResult) - p);
				m_pCount[i].z = (float)((m_pCount[i].z + ZResult) + n);
			}

			else if(XResult < 0 && ZResult < 0)
			{
				m_pCount[i].x = (float)((m_pCount[i].x + XResult) - p);
				m_pCount[i].z = (float)((m_pCount[i].z + ZResult) - n);
			}

			XResult = 0;
			ZResult = 0;
			
			if(m_pCount[i].y == -FLT_MAX)                                              
				return;
		}

		// Terrain Information (Get the terrain information of the tile where the grass is located)
		unsigned char cGroundInfo = s_pTerrain-&gt;GetGrassAttr(((WORD)m_pCount[i].x)/TILE_SIZE, ((WORD)m_pCount[i].z)/TILE_SIZE); // information of the current tile
		if (cGroundInfo==0) // Not a cell (area, region) where grass is sown
		{
			pGrass->m_bCamOut = TRUE;
			continue;
		}

		GrassIndex |= cGroundInfo;

		m_pCount[i].y = s_pTerrain-&gt;GetHeight(m_pCount[i].x, m_pCount[i].z); // set height

    	pGrass->ScaleSet(1.5f, 1.5f, 1.5f);
		vPivot.Set(m_pCount[i].x,m_pCount[i].y,m_pCount[i].z);

		BOOL IsCamOut;
		// Do not render if off-camera.
		IsCamOut = pProc->s_pEng->s_CameraData.IsOutOfFrustum(vPivot,1);
		if(IsCamOut==TRUE)
		{
			pGrass->m_bCamOut = TRUE;
			continue;
		}

		// Calculate the distance to make it look smoother.
		__Vector3 leng = vCamPos - m_pCount[i];
		float lengs = leng.Magnitude();
		IsCamOut = pGrass-&gt;SetBrightLevel(lengs); // Alpha between distances, automatically set to render
		if (IsCamOut == TRUE) // Do not render if the distance is large.
		{
			pGrass->m_bCamOut = TRUE;
			continue;
		}

		pGrass-&gt;PosSet(vPivot); // input location
		pGrass-&gt;Tick(-1); // Transform to fit the camera&#39;s position
		if(pGrass-&gt;ThisTexIsHave(cGroundInfo) == false)
		{
			int nGrassNum;
			unsigned char uCGrassMngIndex;
			FindGrassIndex(cGroundInfo,nGrassNum,uCGrassMngIndex);
			pGrass->TexSelectNum(nGrassNum,uCGrassMngIndex);
		}
pGrass-&gt;m_bCamOut = FALSE; // Sprinkle on the screen (render)
	}

// If there is a change in the type of other pools, delete them all and refill them with pools
	if(m_ucDrawIndex!=GrassIndex)
	{
		m_ucDrawIndex = GrassIndex;
		ChkThisZoneUseGrass(GRASS_MAKEUSE);
	}
*/
}

void CGrassMng::FindGrassIndex(unsigned char GrassIndex,int& nFineGrass,unsigned char& ucFineIndex)
{
	int GrassInputCount;
	int InputGrass[8];
	unsigned char InputGrassOrg[8];
	nFineGrass=0;

	FindGrassIndex(GrassIndex,InputGrass,InputGrassOrg,GrassInputCount);

	if(GrassInputCount>0) // If there are more than 2 pools, get them randomly
	{
		nFineGrass = (int)(rand()%GrassInputCount);
	}

	ucFineIndex = InputGrassOrg[nFineGrass];
	nFineGrass = InputGrass[nFineGrass];
}



void CGrassMng::Render()
{
	if(m_bChkZoneChange==false) return;

	// backup
	DWORD dwLighting;
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLighting);

	DWORD dwAlphaBlend, dwTextureFactor;
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlend);
	CN3Base::s_lpD3DDev->GetRenderState(D3DRS_TEXTUREFACTOR, &dwTextureFactor);
	
	DWORD dwAlphaOP, dwAlphaArg1,dwAlphaArg2;
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAOP, &dwAlphaOP);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
	CN3Base::s_lpD3DDev->GetTextureStageState(0, D3DTSS_ALPHAARG1, &dwAlphaArg2);

	// Set render state
	CN3Base::s_lpD3DDev->SetRenderState( D3DRS_LIGHTING,	TRUE );  
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	// texture state settings (alpha)
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	for(int i=0;i<GRASS_TILENUM;++i)
		for(int j=0;j<GRASS_TILENUM;++j)
			m_pGrasses[i][j].Render(m_txTexture);

	// restore
	CN3Base::s_lpD3DDev->SetRenderState( D3DRS_LIGHTING, dwLighting );  
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, dwAlphaOP);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg1);
	CN3Base::s_lpD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, dwAlphaArg2);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
	CN3Base::s_lpD3DDev->SetRenderState(D3DRS_TEXTUREFACTOR, dwTextureFactor);

}



void CGrassMng::Release()
{
	m_bChkZoneChange = false;

	CLogWriter::Write("CGrassMng::Release -> ZeroMemory"); // TmpLog1122
	ZeroMemory(m_fChkRange,sizeof(m_fChkRange));

	for(int i=0;i<m_iFileMaxNum;i++)
	{
		CLogWriter::Write("CGrassMng::Release s_MngTex.Delete(%d)",m_txTexture[i]); // TmpLog1122
		CGameBase::s_MngTex.Delete(&m_txTexture[i]);
	}

	m_iFileMaxNum = 0;
	m_usDrawIndex=0;

}

int CGrassMng::SetFile(int iTexIndex,unsigned char ucTexOrgIndex,__Vector3 CamPo)
{
/*	if(iTexIndex<0) return 2;

	int iGrassNum = m_Grasses.size();
	if(iGrassNum >= GRASS_MAX) return 1;

	float row = 18.0f, col = 18.0f;

	CGrassBoard* pGrass = new CGrassBoard;
	pGrass->LoadFromFile(iTexIndex,ucTexOrgIndex,CamPo);
	pGrass->SetBrightInit(12,15);
	m_Grasses.push_back(pGrass);
	
	m_pCount[iGrassNum].x  = CamPo.x+RandomGenf(row, -(row));
	m_pCount[iGrassNum].z  = CamPo.z+RandomGenf(col, -(col));
*/
	return 0;
}


inline float CGrassMng::RandomGenf(float max, float min)
{
	if ( max == min ) return max;
	if ( min > max ) { const float b = min; min = max; max = b; }
	const int k = rand()%(int)((max*100-min*100));	

	return (float)((float)(k*0.01f)+min);
}

void CGrassMng::FindGrassIndex(const unsigned char uCGrassMngOrder,int* pnInputGrass,unsigned char* GrassIndex,int& nGrassTotNum)
{
	nGrassTotNum=0;
	if(uCGrassMngOrder & 0x01)  { pnInputGrass[nGrassTotNum] = 0 , GrassIndex[nGrassTotNum] = 0x01; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x02)  { pnInputGrass[nGrassTotNum] = 1 , GrassIndex[nGrassTotNum] = 0x02; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x04)  { pnInputGrass[nGrassTotNum] = 2 , GrassIndex[nGrassTotNum] = 0x04; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x08)  { pnInputGrass[nGrassTotNum] = 3 , GrassIndex[nGrassTotNum] = 0x08; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x10)  { pnInputGrass[nGrassTotNum] = 4 , GrassIndex[nGrassTotNum] = 0x10; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x20)  { pnInputGrass[nGrassTotNum] = 5 , GrassIndex[nGrassTotNum] = 0x20; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x40)  { pnInputGrass[nGrassTotNum] = 6 , GrassIndex[nGrassTotNum] = 0x40; nGrassTotNum++; }
	if(uCGrassMngOrder & 0x80)  { pnInputGrass[nGrassTotNum] = 7 , GrassIndex[nGrassTotNum] = 0x80; nGrassTotNum++; }
}

void CGrassMng::ChkThisZoneUseGrass(int nGrassUseOrder) const
{
	if(m_usDrawIndex==0) return;
/*
	int GrassInputCount;
	int InputGrass[8];
	unsigned char InputGrassOrg[8];

	FindGrassIndex(m_usDrawIndex,InputGrass,InputGrassOrg,GrassInputCount);

	if(nGrassUseOrder==GRASS_MAKENEW)
	{
		int iReturnMsg=0;
		while (!iReturnMsg)
		{
			for(int i=0;i<GrassInputCount && !iReturnMsg;i++)
			{
				iReturnMsg = SetFile(InputGrass[i],InputGrassOrg[i],m_vCamPo);
			}
		}
	}
	else if(nGrassUseOrder==GRASS_MAKEUSE)
	{
		CGrassBoard*   pGrass = NULL; 
		it_GrassBoard it = m_Grasses.begin();
		int iSize = m_Grasses.size();
		int nGrassCount=0,i;
		
		if(iSize==0) return;

		// for(int nGrassCount=0;nGrassCount<iSize;nGrassCount++)
		while(nGrassCount<iSize)
		{
			for(auto i =0;i<GrassInputCount && nGrassCount<iSize;i++,nGrassCount++,it++)
			{
				pGrass = *it;
				pGrass->TexSelectNum(InputGrass[i],InputGrassOrg[i]);
			}
		}
	}
*/
}

bool CGrassMng::ChangeZone()
{
	Release();

	// Maximum number of pictures to be used in this zone
	if (!ACT_WORLD->GetTerrainRef()) return false;
	m_iFileMaxNum = ACT_WORLD->GetTerrainRef()->m_iNumGrass;

	if(m_iFileMaxNum==0) 
	{
		m_usDrawIndex=0;
		return false;
	}

	int i;

	for(i=0;i<m_iFileMaxNum;i++)
	{
#ifdef _DEBUG
		if (ACT_WORLD->GetTerrainRef())
			continue;
		strcpy(m_strFileName[i], ACT_WORLD->GetTerrainRef()->m_pGrassTextureName[i]);
#endif
		m_txTexture[i] = CGameBase::s_MngTex.Get(ACT_WORLD->GetTerrainRef()->m_pGrassTextureName[i]);
	}

	for(;i<8;i++)
	{
#ifdef _DEBUG
		m_strFileName[i][0] = '\0';
#endif
		m_txTexture[i] = nullptr;
	}


	// Contains the index of the grass that will be used in the terrain you have moved to
	const __Vector3 vCamPos = CGameBase::s_CameraData.vEye;
	this->ChkTileRange(vCamPos.x,vCamPos.z);

	// Allow Tick and Render
	m_bChkZoneChange=true;

	return true;
}

inline bool CGrassMng::IsInRect(float fPoX,float fPoY,float* fRange)
{
	return (fPoX>fRange[GRASS_LEFT] && fPoX<fRange[GRASS_RIGHT] 
		 && fPoY>fRange[GRASS_TOP] && fPoY<fRange[GRASS_BOTTOM]);
}

void CGrassMng::ChkTileRange(float fCamX,float fCamZ)
{
	if(IsInRect(fCamX,fCamZ,m_fChkRange)==true) return;	// nothing to fix

	// The area of one square of the character
	float fCompRange[4];
	fCompRange[GRASS_LEFT]   = (fCamX/TILE_SIZE)*TILE_SIZE;
	fCompRange[GRASS_RIGHT]  = fCompRange[GRASS_LEFT]+TILE_SIZE;
	fCompRange[GRASS_TOP]    = (fCamZ/TILE_SIZE)*TILE_SIZE;	
	fCompRange[GRASS_BOTTOM] = fCompRange[GRASS_TOP]+TILE_SIZE;

	// whole area
	float fLargeRange[4];
	fLargeRange[GRASS_LEFT]	  = fCompRange[GRASS_LEFT] - 5*TILE_SIZE;
	fLargeRange[GRASS_RIGHT]  = fCompRange[GRASS_RIGHT] + 5*TILE_SIZE;
	fLargeRange[GRASS_TOP]    = fCompRange[GRASS_TOP] - 5*TILE_SIZE;
	fLargeRange[GRASS_BOTTOM] = fCompRange[GRASS_BOTTOM] + 5*TILE_SIZE;


	// Retrieves the area of a new tile
	WORD FineTile[GRASS_TILENUM*2];
	int iFineTile=0;
	FineNewTile(FineTile,iFineTile,fCompRange,fLargeRange);	// Find a tile to change


	int iTileCount=0;
	__Vector3 vCenter;
	CGrassBoard* pBoard;
	unsigned short usGrassInfo;
	for(int x=0; x<GRASS_TILENUM; ++x)
	{
		for(int z=0; z<GRASS_TILENUM; ++z)
		{
			pBoard = &m_pGrasses[x][z];

			vCenter = pBoard->GetPo();

			// Add the found area to the out of area area
			if(IsInRect( vCenter.x,vCenter.z,fLargeRange )==false)
			{
				if (!ACT_WORLD->GetTerrainRef())
					continue;
				usGrassInfo = ACT_WORLD->GetTerrainRef()->GetGrassAttr(((WORD)FineTile[iTileCount])/TILE_SIZE
													,((WORD)FineTile[iTileCount+1])/TILE_SIZE);	// information of the current tile

				vCenter.Set(FineTile[iTileCount],0.0f,FineTile[iTileCount+1]);	// top left coordinates
				vCenter.y = ACT_WORLD->GetTerrainRef()->GetHeight(vCenter.x,vCenter.z);

				pBoard->SetInfo(vCenter,usGrassInfo);	// Pool update based on tile information

				iTileCount+=2;
			}
		}
	}

	memcpy(m_fChkRange,fCompRange,sizeof(float)*4);
}

void CGrassMng::FineNewTile(WORD* Tile,int& iCount,float* fCompRange,float* fLargeRange)
{
	float sx=-1,sy=-1;
	if(fCompRange[GRASS_LEFT] != m_fChkRange[GRASS_LEFT])
	{
		sy = fLargeRange[GRASS_TOP];

		if(fCompRange[GRASS_LEFT] > m_fChkRange[GRASS_LEFT]) sx = fLargeRange[GRASS_RIGHT] - TILE_SIZE;
		else												 sx = fLargeRange[GRASS_LEFT];

		for(int i=0;i<GRASS_TILENUM;++i)
		{
			Tile[iCount] = sx , Tile[iCount+1] = (sy+=TILE_SIZE);	iCount+=2;
		}
	}

	if(fCompRange[GRASS_TOP] != m_fChkRange[GRASS_TOP])
	{
		if(fCompRange[GRASS_TOP] > m_fChkRange[GRASS_TOP]) sy = fLargeRange[GRASS_TOP];
		else											   sy = fLargeRange[GRASS_BOTTOM] - TILE_SIZE;

		int si,ei;
		if(sx == -1)						   si = 0 , ei = GRASS_TILENUM   ; 
		else if(sx == fLargeRange[GRASS_LEFT]) si = 1 , ei = GRASS_TILENUM   ;
		else								   si = 0 , ei = GRASS_TILENUM-1 ;

		sx = fLargeRange[GRASS_LEFT] + TILE_SIZE*si;
		
		for(int i=si;i<ei;++i)
		{
			Tile[iCount] = (sx+=TILE_SIZE) , Tile[iCount+1] = sy;	iCount+=2;
		}
	}
}