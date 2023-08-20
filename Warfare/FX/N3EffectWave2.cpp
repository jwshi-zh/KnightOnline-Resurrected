#include "pch.h"
#include "N3EffectWave2.h"
#include "N3Texture.h"

#define ATISQRT	4.94974747f

// Constructor.. Variable default value assignment..
CN3EffectWave2::CN3EffectWave2()
{
	m_iPondMeshNum = 0;
	m_pCPondMesh= nullptr;
	m_pfMaxVtx= nullptr;
	Release();
}

CN3EffectWave2::~CN3EffectWave2()
{
	Release();
}

void CN3EffectWave2::Release()
{
	if(m_iPondMeshNum>0)
	{
		if(m_pCPondMesh!= nullptr)
		{
			delete []m_pCPondMesh;
			m_pCPondMesh= nullptr;
		}
		m_iPondMeshNum=0;
	}

	if(m_iMaxVtxNum>0)
	{
		if(m_pfMaxVtx)
		{
			delete [] m_pfMaxVtx;
			m_pfMaxVtx = nullptr;
		}
		m_iMaxVtxNum=0;
	}

	m_fTexIndex = 0.0f;
	m_pTexWave= nullptr;
}

bool CN3EffectWave2::Load(HANDLE hFile)
{
	Release();

	DWORD dwNum;

	ReadFile(hFile, &m_iPondMeshNum, sizeof(int), &dwNum, nullptr);	// 
	if(m_iPondMeshNum<=0) {m_iPondMeshNum=0;return 1;}

	m_pCPondMesh = new CPongMesh [m_iPondMeshNum];	// 

	CPongMesh* ptmpPondMesh;
	for (int i=0;i<m_iPondMeshNum;i++)
	{
		ptmpPondMesh = &m_pCPondMesh[i];

		int iVC;
		ReadFile(hFile, &iVC, sizeof(iVC), &dwNum, nullptr);				// number of points
		ptmpPondMesh->m_iVC = iVC;	// 
		ptmpPondMesh->m_bTick2Rand = FALSE;		// 
		if(iVC<=0) 
		{
			ptmpPondMesh->m_pVertices= nullptr;
			continue;
		}

		int iWidthVertex;
		ReadFile(hFile, &iWidthVertex, sizeof(iWidthVertex), &dwNum, nullptr);				// number of dots per line
		ptmpPondMesh->m_iWidthVtx = iWidthVertex;		// 
		ptmpPondMesh->m_iHeightVtx = iVC/iWidthVertex;	// 

		float fWaterScaleX,fWaterScaleZ;
		ReadFile(hFile, &fWaterScaleX, sizeof(fWaterScaleX), &dwNum, nullptr);
		ReadFile(hFile, &fWaterScaleZ, sizeof(fWaterScaleZ), &dwNum, nullptr);
		ptmpPondMesh->m_fWaterScaleX = fWaterScaleX;	// 
		ptmpPondMesh->m_fWaterScaleZ = fWaterScaleZ;	// 

		// XyxT2 -> XyzColorT2 Converting.
		ptmpPondMesh->m_pVertices = new __VertexPond[iVC];	// 
		ReadFile(hFile,ptmpPondMesh->m_pVertices,iVC*sizeof(__VertexPond),&dwNum, nullptr);
		ptmpPondMesh->m_pVertices[0].y += 0.2f;				// If the number is high, the wave will fluctuate greatly.
		ptmpPondMesh->m_pVertices[iWidthVertex].y += 0.2f;	// If the number is high, the wave will fluctuate greatly.
		ptmpPondMesh->m_pfMaxHeight = ptmpPondMesh->m_pVertices[0].y -= 0.3f;		// maximum wave

		ptmpPondMesh->m_pfVelocityArray = new float[iVC];	// 
		memset(ptmpPondMesh->m_pfVelocityArray,0,sizeof(float)*iVC);

		
		int iIC;
		ReadFile(hFile, &iIC, sizeof(iIC), &dwNum, nullptr);				// IndexBuffer Count.
		ptmpPondMesh->m_iIC = iIC;		// 
		ptmpPondMesh->m_wpIndex = new WORD [iVC*6];		// 

		int j,k;
		int iWidth = iWidthVertex,iHeight = iVC/iWidthVertex;
		int x=0,y=iWidth;
		WORD* indexPtr = ptmpPondMesh->m_wpIndex;	// Set where to call the triangle
		iWidth--;

		const __VertexPond* ptVtx = ptmpPondMesh->m_pVertices;
		float StX,EnX,StZ,EnZ;
		StX = ptVtx[0].x,EnX = ptVtx[iWidth].x;
		StZ = ptVtx[0].z,EnZ = ptVtx[iHeight].z;
		for (j=0; j<iHeight; j++)
		{
			for (k=0; k<iWidth; k++)
			{
				// Set where to call the triangle
				indexPtr[0] = x;
				indexPtr[1] = x+1;
				indexPtr[2] = y;
				indexPtr[3] = y;
				indexPtr[4] = x+1;
				indexPtr[5] = y+1;

				indexPtr += 6;
				x++;
				y++;

				// Find the minimum and maximum position of a pond
				if(StX>ptVtx->x) StX = ptVtx->x;
				if(EnX<ptVtx->x) EnX = ptVtx->x;
				if(StZ>ptVtx->z) StZ = ptVtx->z;
				if(EnZ<ptVtx->z) EnZ = ptVtx->z;
				ptVtx++;
			}
			x++;
			y++;
		}

		ptmpPondMesh->m_vCenterPo.Set(((EnX-StX)/2.0f) + StX
									 ,ptmpPondMesh->m_pVertices[1].y
									 ,((EnZ-StZ)/2.0f) + StZ);
		if(EnX-StX > EnZ-StZ)
			ptmpPondMesh->m_fRadius = EnX-StX;
		else ptmpPondMesh->m_fRadius = EnZ-StZ;


		ptmpPondMesh->m_bTick2Rand = TRUE;		// 

		if(m_iMaxVtxNum<iVC) m_iMaxVtxNum=iVC;	// Find the largest calculation range
	}	

	m_pfMaxVtx = new float [m_iMaxVtxNum];
	m_iMaxVtxNum *= sizeof(float);

#ifdef _DEBUG
	for(int x=0;x<2000;x++) UpdateWaterPositions();
#endif

	return 0;
}

void CN3EffectWave2::Init(const std::string& TexPath)
{
	char szFileName[30];
	for (int i=0;i<MAX_POND_TEX;i++)
	{
		sprintf(szFileName, "misc\\river\\caust%02d.dxt", i);
		m_pTexPond[i] = CN3Base::s_MngTex.Get(szFileName);
		__ASSERT(m_pTexPond[i], "CN3EffectWave2::texture load failed");
	}
	m_pTexWave = s_MngTex.Get(TexPath);// "misc\\river\\blupool3.tga");		///
	__ASSERT(m_pTexWave, "CN3EffectWave2::texture load failed");
}

void CN3EffectWave2::Tick()
{
	if(m_iPondMeshNum==0) return;

	float frame;

	m_fTexIndex += s_fSecPerFrm*15.0f;
	if (m_fTexIndex >= 32.0f)
	{
		m_fTexIndex -= 32.0f;
	}

	// If the frame is smaller than the threshold, it is discarded.
	if ( CN3Base::s_fFrmPerSec < 30 ) return;
	
	// If the frame comes out better than the desired frame rate..
	if ( 30.0f <= CN3Base::s_fFrmPerSec )
	{
		static float ftemp = 0.0f;
		frame = (30.0f / CN3Base::s_fFrmPerSec)*1.2f;
		ftemp += frame;
		if ( ftemp > 1.0f )
		{
			UpdateWaterPositions();
			ftemp -= 1.0f;
		}
	}
	// If the frame does not come out better than the Desire Frame..
	else 
	{
		static float ftemp = 0.0f;
		frame = (30.0f / CN3Base::s_fFrmPerSec)*1.2f;
		ftemp   += frame;
		const int i    = (int)ftemp;
		const float j  = ftemp - (float)i;

		for(auto k = 0; k < i; k++ )
		{
			UpdateWaterPositions();
		}
		// TRACE("Frame not Accecl \n");
		ftemp = j;
	}
}

void CN3EffectWave2::Render() const
{
	if(m_iPondMeshNum==0) return;

	// Backup
	__Matrix44 matWorld, matOld;	matWorld.Identity();
	DWORD dwAlphaEnable, dwSrcBlend, dwDestBlend;
	s_lpD3DDev->GetTransform(D3DTS_WORLD, &matOld);
	s_lpD3DDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaEnable);
	s_lpD3DDev->GetRenderState(D3DRS_SRCBLEND, &dwSrcBlend);
	s_lpD3DDev->GetRenderState(D3DRS_DESTBLEND, &dwDestBlend);
	DWORD dwColor_0, dwColorArg1_0, dwColorArg2_0, dwMipFilter_0;
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLOROP, &dwColor_0);
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG1, &dwColorArg1_0);
	s_lpD3DDev->GetTextureStageState(0, D3DTSS_COLORARG2, &dwColorArg2_0);
	s_lpD3DDev->GetSamplerState(0, D3DSAMP_MIPFILTER, &dwMipFilter_0);
	DWORD dwColor_1, dwColorArg1_1, dwColorArg2_1, dwMipFilter_1;
	s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLOROP, &dwColor_1);
	s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLORARG1, &dwColorArg1_1);
	s_lpD3DDev->GetTextureStageState(1, D3DTSS_COLORARG2, &dwColorArg2_1);
	s_lpD3DDev->GetSamplerState(1, D3DSAMP_MIPFILTER, &dwMipFilter_1);


	// Set
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &matWorld);

	// texture state settings (alpha)
	s_lpD3DDev->SetTexture(0,m_pTexPond[(int)m_fTexIndex]->Get());
	s_lpD3DDev->SetTexture(1, m_pTexWave->Get());
	s_lpD3DDev->SetTexture(2, nullptr);

	s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    s_lpD3DDev->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
    s_lpD3DDev->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );

	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);	
	s_lpD3DDev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    s_lpD3DDev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    s_lpD3DDev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);	
    s_lpD3DDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
    s_lpD3DDev->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	s_lpD3DDev->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2);

	for(int i=0;i<m_iPondMeshNum;i++)
	{
		if(m_pCPondMesh[i].m_bTick2Rand)
			s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST
												, 0
												, m_pCPondMesh[i].m_iVC
												, m_pCPondMesh[i].m_iIC
												, m_pCPondMesh[i].m_wpIndex
												, D3DFMT_INDEX16
												, m_pCPondMesh[i].m_pVertices
												, sizeof(__VertexPond));
	}


	// restore
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &matOld);
	s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaEnable);
	s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND, dwSrcBlend);
	s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND, dwDestBlend);

	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, dwColor_0);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, dwColorArg1_0);
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, dwColorArg2_0);
	s_lpD3DDev->SetSamplerState(0, D3DSAMP_MIPFILTER, dwMipFilter_0);

	s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLOROP, dwColor_1);
	s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG1, dwColorArg1_1);
	s_lpD3DDev->SetTextureStageState(1, D3DTSS_COLORARG2, dwColorArg2_1);
	s_lpD3DDev->SetSamplerState(1, D3DSAMP_MIPFILTER, dwMipFilter_1);
}

void CN3EffectWave2::UpdateWaterPositions()
{
	CPongMesh* pPondMesh;

	// basic data
	int	x, y,n,m;
	float d;
	float tempX,tempZ;
	__VertexPond* pVtx,*ptmpVtx,*ptmpVtxSub,*ptmpVtxPlus;
	float* pForceArray,*ptmpForceArray,*ptmpFArrSub,*ptmpFArrPlus;

	// calculation variable
	float max,min,mincal,maxcal;

	for(int i=0;i<m_iPondMeshNum;i++)
	{
		pPondMesh = &m_pCPondMesh[i];

		// Skip if not used this time
		if(CN3Base::s_CameraData.IsOutOfFrustum(pPondMesh->m_vCenterPo,pPondMesh->m_fRadius)==TRUE)
		{
			pPondMesh->m_bTick2Rand = FALSE;
			continue;
		}
		else pPondMesh->m_bTick2Rand = TRUE;

		// TRACE("Pond Is Chk  ---------- %d \n",i);
		// Create basic data
		m = pPondMesh->m_iWidthVtx;
		n = pPondMesh->m_iHeightVtx;
		// max = 0.16f*pPondMesh->m_fWaterScaleX;
		// min = -0.16f*pPondMesh->m_fWaterScaleX;
		max = 0.04f*pPondMesh->m_fWaterScaleX;
		min = -0.04f*pPondMesh->m_fWaterScaleX;
		maxcal = max*ATISQRT;
		mincal = min*ATISQRT;

		memset(m_pfMaxVtx, 0, m_iMaxVtxNum);

		pVtx = pPondMesh->m_pVertices;
		pForceArray = m_pfMaxVtx;
		
		// calculate
		for (x=1; x<n-1; x++)
		{
			ptmpFArrSub = pForceArray;
			pForceArray += m;
			ptmpForceArray = pForceArray;
			ptmpFArrPlus = ptmpForceArray+m;

			ptmpVtxSub = pVtx;
			pVtx += m;
			ptmpVtx = pVtx;
			ptmpVtxPlus = ptmpVtx+m;

			for (y=1; y<m-1; y++) 
			{
				 // Kernel looks like this:
				 //
				 // 1/Root2 | 1 | 1/Root2
				 // ---------+---------+---------
				 // 1    |    0    |    1
				 // ---------+---------+---------
				 // 1/Root2 | 1 | 1/Root2
				 
				ptmpForceArray++,ptmpFArrPlus++,ptmpFArrSub++;
				ptmpVtx++,ptmpVtxPlus++,ptmpVtxSub++;

				d = ptmpVtx->y - (ptmpVtx-1)->y;
				 if ( d < min ) d = min;	if ( d > max ) d = max;
				 *ptmpForceArray	-= d;
				 *(ptmpForceArray-1)+= d;

				 d = ptmpVtx->y - ptmpVtxSub->y;
				 if ( d < min ) d = min;	if ( d > max ) d = max;
				 *ptmpForceArray	-= d;
				 *ptmpFArrSub		+= d;

				 d = ptmpVtx->y - (ptmpVtx+1)->y;
				 if ( d < min ) d = min;	if ( d > max ) d = max;
				 *ptmpForceArray	-= d;
				 *(ptmpForceArray+1)+= d;

				 d = ptmpVtx->y - ptmpVtxPlus->y;
				 if ( d < min ) d = min;	if ( d > max ) d = max;
				 *ptmpForceArray	-= d;
				 *ptmpFArrPlus		+= d;

				 d = (ptmpVtx->y - (ptmpVtxPlus+1)->y)*ATISQRT;
				 if ( d < mincal ) d = mincal;	if ( d > maxcal ) d = maxcal;
				 *ptmpForceArray	-= d;
				 *(ptmpFArrPlus+1)	+= d;

				 d = (ptmpVtx->y - (ptmpVtxSub-1)->y)*ATISQRT;
				 if ( d < mincal ) d = mincal;	if ( d > maxcal ) d = maxcal;
				 *ptmpForceArray	-= d;
				 *(ptmpFArrSub-1)	+= d;

				 d = (ptmpVtx->y - (ptmpVtxPlus-1)->y)*ATISQRT;
				 if ( d < mincal ) d = mincal;	if ( d > maxcal ) d = maxcal;
				 *ptmpForceArray	-= d;
				 *(ptmpFArrPlus-1)	+= d;

				 d = (ptmpVtx->y - (ptmpVtxSub+1)->y)*ATISQRT;
				 if ( d < mincal ) d = mincal;	if ( d > maxcal ) d = maxcal;
				 *ptmpForceArray	-= d;
				 *(ptmpFArrSub+1)	+= d;
			}
		}

		// tempX = pPondMesh->m_fWaterScaleX*pPondMesh->m_iWidthVtx*32.0f;
		// tempZ = pPondMesh->m_fWaterScaleZ*pPondMesh->m_iHeightVtx*32.0f;
		tempX = pPondMesh->m_fWaterScaleX*32.0f;
		tempZ = pPondMesh->m_fWaterScaleZ*32.0f;

		ptmpForceArray = pPondMesh->m_pfVelocityArray;	// I borrowed it because it&#39;s the same type
		pForceArray = m_pfMaxVtx;
		pVtx = pPondMesh->m_pVertices;
		for (x=0; x<pPondMesh->m_iVC; x++)
		{
			// *ptmpForceArray += *pForceArray*0.02f;
			(*ptmpForceArray) += (*pForceArray)*0.001f;
			
			pVtx->y += (*ptmpForceArray);
			if(pVtx->y > pPondMesh->m_pfMaxHeight)
				pVtx->y = pPondMesh->m_pfMaxHeight;

			pVtx->u = pVtx->x/tempX + 0.5f;
			pVtx->v = pVtx->z/tempZ + 0.5f;
			pVtx->u2 = pVtx->u;
			pVtx->v2 = pVtx->v;

			pVtx->color = 0xddffffff;

			pForceArray++,pVtx++,ptmpForceArray++;
		}
	}
}
