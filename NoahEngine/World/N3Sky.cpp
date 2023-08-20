#include "pch.h"
#include "N3Sky.h"
#include "N3PMesh.h"
#include "N3PMeshInstance.h"
#include "N3Texture.h"

CN3Sky::CN3Sky()
{
	// m_pTex = NULL;
	m_SkyColor.ChangeColor(0xff081021);	// night
	m_FogColor.ChangeColor(0xff102942);	// night
}

CN3Sky::~CN3Sky()
{
	// if (m_pTex) {s_MngTex.Delete(m_pTex);	m_pTex = NULL;}
}

void CN3Sky::Release()
{
	CN3Base::Release();
	// m_SkyColor.ChangeColor(0xFF5284DE);	//낮
	// m_FogColor.ChangeColor(0xFFB5C6DE);	//낮
	m_SkyColor.ChangeColor(0xff081021);	// night
	m_FogColor.ChangeColor(0xff102942);	// night
	// if (m_pTex) {s_MngTex.Delete(m_pTex);	m_pTex = NULL;}
}

void CN3Sky::Tick()
{
	m_SkyColor.Tick();
	m_FogColor.Tick();
	int i;
	const D3DCOLOR FogColor = m_FogColor.GetCurColor();
	for (i=0; i<4; ++i)
	{
		m_vFronts[i].color = (m_vFronts[i].color&0xff000000) | (FogColor&0x00ffffff);
		m_Bottom[i].color = FogColor;
	}
}

void CN3Sky::Render() const
{
    // Set up a rotation matrix to orient the billboard towards the camera.
	__Matrix44 matWorld;
    const __Vector3 vDir = s_CameraData.vEye - s_CameraData.vAt;	// direction of the camera
	if (0.0f == vDir.x) matWorld.Identity();
	else if( vDir.x > 0.0f ) matWorld.RotationY(-atanf(vDir.z/vDir.x) - (D3DX_PI * 0.5f));
	else  matWorld.RotationY(-atanf(vDir.z/vDir.x) + (D3DX_PI * 0.5f));
	s_lpD3DDev->SetTransform( D3DTS_WORLD, &matWorld );

	// Set the texture stage states
	s_lpD3DDev->SetTexture(0, nullptr);
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	s_lpD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

	// Render the skybox
	s_lpD3DDev->SetFVF(FVF_XYZCOLOR);
	// drawing the bottom plate
	s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_Bottom, sizeof(m_Bottom[0]));
	// drawing the sides
	s_lpD3DDev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_vFronts, sizeof(m_vFronts[0]));
}

void CN3Sky::Init()
{
	Release();
	
	// Take the case of sunset as an example....
	const float fWidth		= 3.5f; // front plate width
	const float fTopY		= 0.5f; // The height of the front panel.. The height above the reddish part from the sunset - the sky from above.
	const float fBottomY	= 0.1f; // Bottom level of the face plate.. Below the reddish part of the glow - below this is pure haze color..
	const float fDistance	= 1.5f; // Bottom length of front plate..
	const D3DCOLOR color = m_FogColor.GetCurColor();
	m_vFronts[0].Set( fWidth,	fTopY,		fDistance,		0x00ffffff&color);
	m_vFronts[1].Set( fWidth,	fBottomY,	fDistance,		color);
	m_vFronts[2].Set(-fWidth,	fBottomY,	fDistance,		color);
	m_vFronts[3].Set(-fWidth,	fTopY,		fDistance,		0x00ffffff&color);

	const float fBottomOffset = -5.0f;	// floor height of base plate
	m_Bottom[0].Set( fWidth,	fBottomY,	fDistance,		color);
	m_Bottom[1].Set( fWidth,	fBottomOffset,	fDistance,	color);
	m_Bottom[2].Set(-fWidth,	fBottomOffset,	fDistance,	color);
	m_Bottom[3].Set(-fWidth,	fBottomY,	fDistance,		color);
}
