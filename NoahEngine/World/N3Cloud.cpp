#include "pch.h"
#include "N3Cloud.h"
#include "N3Texture.h"

CN3Cloud::CN3Cloud()
{
	for(int i = 0; i < NUM_CLOUD; i++) 
	{
		m_pTextures[i] = nullptr;
		m_szTextures[i] = "";
	}

	m_Color1.ChangeColor(0xffffffff);
	m_Color2.ChangeColor(0xffffffff);
	m_Alpha.ChangeColor(0xffffffff);
	m_eCloud1 = CLOUD_WISPS;
	m_eCloud2 = CLOUD_PUFFS;
	m_eCloud3 = CLOUD_NONE;

	m_fCloudTexTime = 0.0f;
	m_eBackupCloud = CLOUD_NONE;
	m_fBackupTime = 0.0f;
}

CN3Cloud::~CN3Cloud()
{
	int i;
	for (i=0; i<NUM_CLOUD; ++i)
	{
		s_MngTex.Delete(&m_pTextures[i]);
	}
}

void CN3Cloud::Release()
{
	CN3Base::Release();
	int i;
	for (i=0; i<NUM_CLOUD; ++i)
	{
		if (m_pTextures[i])
		{
			s_MngTex.Delete(&m_pTextures[i]);
			m_szTextures[i] = "";
		}
	}
	m_Color1.ChangeColor(0xffffffff);
	m_Color2.ChangeColor(0xffffffff);
	m_Alpha.ChangeColor(0xffffffff);
	m_eCloud1 = CLOUD_WISPS;
	m_eCloud2 = CLOUD_PUFFS;
	m_eCloud3 = CLOUD_NONE;

	m_fCloudTexTime = 0.0f;
	m_eBackupCloud = CLOUD_NONE;
	m_fBackupTime = 0.0f;
}
void CN3Cloud::Tick()
{
	int i;
	static float fCloudLayer = 0.0f;
	fCloudLayer += s_fSecPerFrm;

	// moving the cloud layer
	if (fCloudLayer>=0.05)
	{
		// float you = 0.001*fCloudLayer;
		// float dv = 0.003*fCloudLayer;
		// float du2 = 0.003*fCloudLayer;
		// float dv2 = 0.005*fCloudLayer;
		const float du = 0.005*fCloudLayer;
		const float dv = 0.015*fCloudLayer;
		const float du2 = 0.015*fCloudLayer;
		const float dv2 = 0.025*fCloudLayer;

		for (i=0; i<NUM_CLOUD_VERTEX; ++i)
		{
			m_pVertices[i].tu += du;
			m_pVertices[i].tv += dv;
			m_pVertices[i].tu2 += du2;
			m_pVertices[i].tv2 += dv2;
		}

		fCloudLayer = 0.0f;

		if (m_pVertices[0].tu > 10.0f)
		{
			for (i=0; i<NUM_CLOUD_VERTEX; ++i)	m_pVertices[i].tu -= 10.0f;
		}
		if (m_pVertices[0].tv > 10.0f)
		{
			for (i=0; i<NUM_CLOUD_VERTEX; ++i)	m_pVertices[i].tv -= 10.0f;
		}
	}

	// change color
	m_Color1.Tick();
	m_Color2.Tick();

	// cloud replacement
	if (CLOUD_NONE != m_eCloud3)
	{
		m_Alpha.Tick();
		m_fCloudTexTime -= s_fSecPerFrm;
		if (m_fCloudTexTime < 0.0f)	// When the cloud replacement is complete
		{
			m_fCloudTexTime = 0.0f;
			m_eCloud2 = m_eCloud3;
			m_eCloud3 = CLOUD_NONE;
			m_Alpha.ChangeColor(0xffffffff);

			if (CLOUD_NONE != m_eBackupCloud)	// If there is something that needs to be changed
			{
				const e_CLOUDTEX eTmp = m_eCloud1;
				m_eCloud1 = m_eCloud2;
				m_eCloud2 = eTmp;

				m_eCloud3 = m_eBackupCloud;
				m_fCloudTexTime = m_fBackupTime;
				m_eBackupCloud = CLOUD_NONE;
				m_fBackupTime = 0.0f;
				m_Alpha.ChangeColor(0x00ffffff, m_fCloudTexTime);

				// Change the uv coordinates too
				for (i=0; i<NUM_CLOUD_VERTEX; ++i)
				{
					float fTempUV = m_pVertices[i].tu;
					m_pVertices[i].tu = m_pVertices[i].tu2;
					m_pVertices[i].tu2 = fTempUV;
					fTempUV = m_pVertices[i].tv;
					m_pVertices[i].tv = m_pVertices[i].tv2;
					m_pVertices[i].tv2 = fTempUV;
				}
			}
		}
	}
}

void CN3Cloud::Render()
{
	__Matrix44 matWorld; matWorld.Identity();
	s_lpD3DDev->SetTransform( D3DTS_WORLD, &matWorld );

	static WORD CloudIndex[30] = {0,1,4,1,2,5,2,3,6,3,0,7,5,4,1,6,5,2,7,6,3,4,7,0,4,5,7,5,6,7};
	// static WORD CloudIndex[54] = {0,1,4,1,2,5,2,3,6,3,0,7,5,4,1,6,5,2,7,6,3,4,7,0,
	// 4,5,8,5,6,9,6,7,10,7,4,11,9,8,5,10,9,6,11,10,7,8,11,4,
	// 8,9,11,9,10,11};

	// backup state
	DWORD dwAlphaOp, dwAlphaArg1, dwAlphaArg2;
	s_lpD3DDev->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &dwAlphaOp );
	s_lpD3DDev->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &dwAlphaArg1);
	s_lpD3DDev->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &dwAlphaArg2);
	// set state
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	if (D3DTOP_MODULATE != dwAlphaOp) s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	if (D3DTA_TEXTURE != dwAlphaArg1) s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	if (D3DTA_DIFFUSE != dwAlphaArg2) s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	s_lpD3DDev->SetFVF(FVF_XYZCOLORT2);

	const D3DCOLOR color1 = m_Color1.GetCurColor();
	D3DCOLOR color2 = m_Color2.GetCurColor();
	__ASSERT(CLOUD_NONE != m_eCloud1 && CLOUD_NONE != m_eCloud2, "no cloud texture type");
	// render cloud 1
	int i;
	for (i=0; i<4; ++i) m_pVertices[i].color = color1&0x00ffffff;
	for (i=4; i<NUM_CLOUD_VERTEX; ++i) m_pVertices[i].color = color1;
	s_lpD3DDev->SetTexture(0, GetTex(m_eCloud1));
	s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 10,
						CloudIndex, D3DFMT_INDEX16, m_pVertices, sizeof(__VertexXyzColorT2));

	s_lpD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX , 1);
	if (CLOUD_NONE != m_eCloud3)
	{
		const D3DCOLOR Alpha = m_Alpha.GetCurColor();
		if (Alpha<color2) color2 = (Alpha&0xff000000) | (color2&0x00ffffff);	// If the alpha value of the existing color change is greater than the cloud replacement alpha value, it is replaced with the cloud replacement alpha value.
		// render cloud 2
		for (i=0; i<4; ++i) m_pVertices[i].color = color2&0x00ffffff;
		for (i=4; i<NUM_CLOUD_VERTEX; ++i) m_pVertices[i].color = color2;
		s_lpD3DDev->SetTexture(0, GetTex(m_eCloud2));
		s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 10,
							CloudIndex, D3DFMT_INDEX16, m_pVertices, sizeof(__VertexXyzColorT2));
		// render cloud 3
		const D3DCOLOR color3 = ((0xff-(color2>>24))<<24) | (color2&0x00ffffff);	// Change the alpha value of color2 to the value subtracted from 0xff
		for (i=4; i<NUM_CLOUD_VERTEX; ++i) m_pVertices[i].color = color3;			
		s_lpD3DDev->SetTexture(0, GetTex(m_eCloud3));
		s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 10,
							CloudIndex, D3DFMT_INDEX16, m_pVertices, sizeof(__VertexXyzColorT2));
	}
	else
	{
		// render cloud 2
		for (i=0; i<4; ++i) m_pVertices[i].color = color2&0x00ffffff;
		for (i=4; i<NUM_CLOUD_VERTEX; ++i) m_pVertices[i].color = color2;
		s_lpD3DDev->SetTexture(0, GetTex(m_eCloud2));
		s_lpD3DDev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 8, 10,
							CloudIndex, D3DFMT_INDEX16, m_pVertices, sizeof(__VertexXyzColorT2));
	}
	s_lpD3DDev->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX , 0);

	// restore state
	if (D3DTOP_MODULATE != dwAlphaOp) s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	if (D3DTA_TEXTURE != dwAlphaArg1) s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	if (D3DTA_DIFFUSE != dwAlphaArg2) s_lpD3DDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

}

LPDIRECT3DTEXTURE9	CN3Cloud::GetTex(e_CLOUDTEX tex)
{
	if (nullptr == m_pTextures[tex])
	{
		m_pTextures[tex] = s_MngTex.Get(m_szTextures[tex]);
		if (nullptr == m_pTextures[tex]) return nullptr;
	}

	return m_pTextures[tex]->Get();
}

void	CN3Cloud::Init(	const std::string* pszFNs)
{
	Release();

	for(int i = 0; i < NUM_CLOUD; i++) m_szTextures[i] = pszFNs[i];

	// 3 when single
	/*
	const float fSqrt3 = 1.0f/sqrtf(3.0f);
	const float fOffset = 3.0f;
	const float fSmallLength = 5.0f;
	const float fMediumLength = fSmallLength + fOffset;
	const float fBigLenth = fMediumLength + fOffset*fSqrt3;
	const float fBigHeight = 0.0f;
	const float fMediumHeight = fBigHeight + fOffset;
	const float fSmallHeight = fMediumHeight + fOffset*fSqrt3;

	D3DCOLOR BigColor = 0x00ffffff;
	D3DCOLOR MediumColor = 0xffffffff;
	D3DCOLOR SmallColor = 0xffffffff;

	float fTmp1, fTmp2;
	fTmp1 = fBigLenth-fMediumLength;
	fTmp2 = fMediumHeight - fBigHeight;
	const float fBig = sqrtf(fTmp1*fTmp1 + fTmp2*fTmp2); // Distance between the largest rectangle and the middle rectangle
	fTmp1 = fMediumLength - fSmallLength;
	fTmp2 = fSmallHeight - fMediumHeight;
	const float fMedium = sqrtf(fTmp1*fTmp1 + fTmp2*fTmp2); // distance between middle and small rectangle
	const float fSmall = fSmallLength; // distance the small square is from 0
	const float fTotal = fBig + fMedium + fSmall;

	float fTexUVLeft = 0.0f, fTexUVTop = 0.0f, fTexUVRight = 3.0f, fTexUVBottom = 3.0f; // Area coordinates to display as texture cloud (because it can be all or part of the texture)
	// const float fTexOffsetU_M = (fBig/fTotal)*(fTexUVRight-fTexUVLeft)/2;
	// const float fTexOffsetV_M = (fBig/fTotal)*(fTexUVBottom-fTexUVTop)/2;
	// const float fTexOffsetU_S = ((fBig+fMedium)/fTotal)*(fTexUVRight-fTexUVLeft)/2;
	// const float fTexOffsetV_S = ((fBig+fMedium)/fTotal)*(fTexUVBottom-fTexUVTop)/2;

	const float fTexOffsetU_M = (1.0f - fMediumLength/fBigLenth)*(fTexUVRight-fTexUVLeft)/2;
	const float fTexOffsetV_M = (1.0f - fMediumLength/fBigLenth)*(fTexUVBottom-fTexUVTop)/2;
	const float fTexOffsetU_S = (1.0f - fSmallLength/fBigLenth)*(fTexUVRight-fTexUVLeft)/2;
	const float fTexOffsetV_S = (1.0f - fSmallLength/fBigLenth)*(fTexUVBottom-fTexUVTop)/2;

	// big
	m_pVertices[0].Set(-fBigLenth, fBigHeight,-fBigLenth, BigColor, fTexUVLeft,	fTexUVTop,		fTexUVLeft,		fTexUVTop);
	m_pVertices[1].Set( fBigLenth, fBigHeight,-fBigLenth, BigColor, fTexUVRight,	fTexUVTop,		fTexUVRight,	fTexUVTop);
	m_pVertices[2].Set( fBigLenth, fBigHeight, fBigLenth, BigColor, fTexUVRight,	fTexUVBottom,	fTexUVRight,	fTexUVBottom);
	m_pVertices[3].Set(-fBigLenth, fBigHeight, fBigLenth, BigColor, fTexUVLeft,	fTexUVBottom,	fTexUVLeft,		fTexUVBottom);
	// medium
	m_pVertices[4].Set(-fMediumLength, fMediumHeight,-fMediumLength, MediumColor, fTexUVLeft+fTexOffsetU_M,	fTexUVTop+fTexOffsetV_M,		fTexUVLeft+fTexOffsetU_M,	fTexUVTop+fTexOffsetV_M);
	m_pVertices[5].Set( fMediumLength, fMediumHeight,-fMediumLength, MediumColor, fTexUVRight-fTexOffsetU_M, fTexUVTop+fTexOffsetV_M,		fTexUVRight-fTexOffsetU_M, fTexUVTop+fTexOffsetV_M);
	m_pVertices[6].Set( fMediumLength, fMediumHeight, fMediumLength, MediumColor, fTexUVRight-fTexOffsetU_M, fTexUVBottom-fTexOffsetV_M,	fTexUVRight-fTexOffsetU_M, fTexUVBottom-fTexOffsetV_M);
	m_pVertices[7].Set(-fMediumLength, fMediumHeight, fMediumLength, MediumColor, fTexUVLeft+fTexOffsetU_M,	fTexUVBottom-fTexOffsetV_M,	fTexUVLeft+fTexOffsetU_M,	fTexUVBottom-fTexOffsetV_M);

	// small
	m_pVertices[8].Set( -fSmallLength, fSmallHeight,-fSmallLength, SmallColor, fTexUVLeft+fTexOffsetU_S,	fTexUVTop+fTexOffsetV_S,		fTexUVLeft+fTexOffsetU_S,	fTexUVTop+fTexOffsetV_S);
	m_pVertices[9].Set(  fSmallLength, fSmallHeight,-fSmallLength, SmallColor, fTexUVRight-fTexOffsetU_S, fTexUVTop+fTexOffsetV_S,		fTexUVRight-fTexOffsetU_S, fTexUVTop+fTexOffsetV_S);
	m_pVertices[10].Set( fSmallLength, fSmallHeight, fSmallLength, SmallColor, fTexUVRight-fTexOffsetU_S, fTexUVBottom-fTexOffsetV_S,	fTexUVRight-fTexOffsetU_S, fTexUVBottom-fTexOffsetV_S);
	m_pVertices[11].Set(-fSmallLength, fSmallHeight, fSmallLength, SmallColor, fTexUVLeft+fTexOffsetU_S,	fTexUVBottom-fTexOffsetV_S,	fTexUVLeft+fTexOffsetU_S,	fTexUVBottom-fTexOffsetV_S);
	*/ 
	// When 2 single
	const float fSqrt3 = 1.0f/sqrtf(3.0f);
	float fBigLenth = 16.0f;				// The length of the large rectangle below the cloud frustum
	const float fSmallLength = 8.0f;		// The length of the small square above the cloud frustum
	const float fBigHeight = 5.0f;			// Height below the cloud tetrahedron (based on 0.0f of the horizon)
	const float fSmallHeight = fBigHeight + (fBigLenth-fSmallLength)*fSqrt3;	// Height above the cloud frustum (relative to 0.0f of the horizon)

	fBigLenth = 24.0f;	// The code above optimizes the angle, it re-increases only the large square.

	const D3DCOLOR BigColor = 0x00ffffff;
	const D3DCOLOR SmallColor = 0xffffffff;

	float fTexUVLeft = 0.0f, fTexUVTop = 0.0f, fTexUVRight = 4.0f, fTexUVBottom = 4.0f;	// Area coordinates to display as texture cloud (because it can be all or part of the texture)
	float fTmp1 = fBigLenth - fSmallLength;
	float fTmp2 = fSmallHeight - fBigHeight;
	// const float fBig = sqrtf(fTmp1*fTmp1 + fTmp2*fTmp2);
	// const float fSmall = fSmallLength;
	// const float fTotal = fBig + fSmall;
	// const float fTexOffsetU = (fSmall/fTotal)*(fTexUVRight-fTexUVLeft)/2;
	// const float fTexOffsetV = (fSmall/fTotal)*(fTexUVBottom-fTexUVTop)/2;
	const float fTexOffsetU = (1.0f - fSmallLength/fBigLenth)*(fTexUVRight-fTexUVLeft)/2;
	const float fTexOffsetV = (1.0f - fSmallLength/fBigLenth)*(fTexUVBottom-fTexUVTop)/2;
	// big
	m_pVertices[0].Set(-fBigLenth, fBigHeight,-fBigLenth, BigColor, fTexUVLeft,		fTexUVTop,		fTexUVLeft,		fTexUVTop);
	m_pVertices[1].Set( fBigLenth, fBigHeight,-fBigLenth, BigColor, fTexUVRight,	fTexUVTop,		fTexUVRight,	fTexUVTop);
	m_pVertices[2].Set( fBigLenth, fBigHeight, fBigLenth, BigColor, fTexUVRight,	fTexUVBottom,	fTexUVRight,	fTexUVBottom);
	m_pVertices[3].Set(-fBigLenth, fBigHeight, fBigLenth, BigColor, fTexUVLeft,		fTexUVBottom,	fTexUVLeft,		fTexUVBottom);
	// small
	m_pVertices[4].Set(-fSmallLength, fSmallHeight,-fSmallLength, SmallColor, fTexUVLeft+fTexOffsetU,	fTexUVTop+fTexOffsetV,		fTexUVLeft+fTexOffsetU,	fTexUVTop+fTexOffsetV);
	m_pVertices[5].Set( fSmallLength, fSmallHeight,-fSmallLength, SmallColor, fTexUVRight-fTexOffsetU, fTexUVTop+fTexOffsetV,		fTexUVRight-fTexOffsetU, fTexUVTop+fTexOffsetV);
	m_pVertices[6].Set( fSmallLength, fSmallHeight, fSmallLength, SmallColor, fTexUVRight-fTexOffsetU, fTexUVBottom-fTexOffsetV,	fTexUVRight-fTexOffsetU, fTexUVBottom-fTexOffsetV);
	m_pVertices[7].Set(-fSmallLength, fSmallHeight, fSmallLength, SmallColor, fTexUVLeft+fTexOffsetU,	fTexUVBottom-fTexOffsetV,	fTexUVLeft+fTexOffsetU,	fTexUVBottom-fTexOffsetV);
}


void CN3Cloud::SetCloud(e_CLOUDTEX eCloud1, e_CLOUDTEX eCloud2, float fSec)
{
	if (CLOUD_NONE == eCloud1 || CLOUD_NONE == eCloud2) return;

	// variable initialization
	m_Alpha.ChangeColor(0xffffffff);
	m_fCloudTexTime = 0.0f;
	m_eCloud3 = CLOUD_NONE;
	m_eBackupCloud = CLOUD_NONE;
	m_fBackupTime = 0.0f;

	// cloud judgment to change
	int iSameCount = 0;
	if (eCloud1 == m_eCloud1) ++iSameCount;
	else if (eCloud1 == m_eCloud2) ++iSameCount;
	if (eCloud2 == m_eCloud1) ++iSameCount;
	else if (eCloud2 == m_eCloud2) ++iSameCount;

	if (2 == iSameCount)		// Since it is the same cloud, there is no need to change it
	{
		return;
	}

	if (1 == iSameCount)		// You only need to replace one.
	{
		if (m_eCloud1 == eCloud1)
		{
			m_eCloud3 = eCloud2;
		}
		else if (m_eCloud1 == eCloud2)
		{
			m_eCloud3 = eCloud1;
		}
		else if (m_eCloud2 == eCloud1)
		{
			m_eCloud2 = m_eCloud1;
			m_eCloud1 = eCloud1;
			m_eCloud3 = eCloud2;
		}
		else
		{
			m_eCloud2 = m_eCloud1;
			m_eCloud1 = eCloud2;
			m_eCloud3 = eCloud1;
		}
		if (0.0f == fSec)
		{
			m_eCloud2 = m_eCloud3;
			m_eCloud3 = CLOUD_NONE;
		}
		else
		{
			m_Alpha.ChangeColor(0x00ffffff, fSec);
			m_fCloudTexTime = fSec;
		}
		return;
	}

	// replace both
	if (0.0f == fSec)
	{
		m_eCloud1 = eCloud1;
		m_eCloud2 = eCloud2;
		m_eCloud3 = CLOUD_NONE;
		return;
	}
	m_eCloud3 = eCloud1;
	m_eBackupCloud = eCloud2;
	m_fCloudTexTime = fSec*0.5f;
	m_fBackupTime = m_fCloudTexTime;
	m_Alpha.ChangeColor(0x00ffffff, m_fCloudTexTime);
}

