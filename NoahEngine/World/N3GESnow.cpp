#include "pch.h"
#include "N3GESnow.h"
#include "N3Texture.h"

CN3GESnow::CN3GESnow()
{
	m_pTex = nullptr;
	m_pSnowParticle = nullptr;
	CN3GESnow::Release();
}

CN3GESnow::~CN3GESnow()
{
	CN3GESnow::Release();
}

void CN3GESnow::Release()
{
	m_fWidth = m_fHeight = 0.0f;
	m_vVelocity.Set(0,0,0);
	s_MngTex.Delete(&m_pTex);
	if (m_pSnowParticle) {delete [] m_pSnowParticle; m_pSnowParticle = nullptr;}
	CN3GlobalEffect::Release();
}

void CN3GESnow::Tick()
{
	if (m_bActive == FALSE || m_iVC <= 0 || m_pVB == nullptr) return;
	CN3GlobalEffect::Tick();

	const int iCount = m_iVC/3;
	int iActiveCount = iCount;
	if(m_iFadeMode > 0) // Gradually increase...
	{
		if(m_fFadeTime > 0 && m_fFadeTimeCur < m_fFadeTime) // Decide how much to skip and shoot because of the fade time..
		{
			iActiveCount = iCount * m_fFadeTimeCur / m_fFadeTime;
			if(iActiveCount > iCount) iActiveCount = iCount;
			else if(iActiveCount < 0) iActiveCount = 0;
		}
	}
	else if(m_iFadeMode < 0)
	{
		if(m_fFadeTime > 0 && m_fFadeTimeCur < m_fFadeTime) // Decide how much to skip and shoot because of the fade time..
		{
			iActiveCount = iCount * (1.0f - m_fFadeTimeCur / m_fFadeTime);
			if(iActiveCount > iCount) iActiveCount = iCount;
			else if(iActiveCount < 0) iActiveCount = 0;
		}
	}
	if(iActiveCount <= 0) return;

	int i;
	__VertexXyzT1* pVertices;
	HRESULT hr = m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_NOSYSLOCK);

	__Vector3	vN = m_vVelocity;	vN.Normalize();
	const __Vector3	vAdd = m_vVelocity*s_fSecPerFrm;
	const float		fAddRadian = D3DX_PI*s_fSecPerFrm*0.1f;

	const float fHalfWidth = m_fWidth/2.0f;
	const float fHalfHeight = m_fHeight/2.0f;
	const float fCurY = s_CameraData.vEye.y;

	static const float sqrt3 = sqrtf(3.0f);

	for (i=0; i<iActiveCount; ++i)
	{
		// Determine location
		__VertexXyzT1* pV1 = pVertices+i*3+0;
		__VertexXyzT1* pV2 = pVertices+i*3+1;
		__VertexXyzT1* pV3 = pVertices+i*3+2;
		// __Vector3 vOffset2 = __Vector3(pV2->v) - __Vector3(pV1->v);
		// __Vector3 vOffset3 = __Vector3(pV3->v) - __Vector3(pV1->v);

		__SnowParticle* pParticle = m_pSnowParticle + i;
		pParticle->vPos += vAdd;
		// pV1-&gt;x += vAdd.x; pV1-&gt;y += vAdd.y; pV1-&gt;z += vAdd.z;

		float fDiff = pParticle->vPos.y - (fCurY-fHalfHeight);
		if (fDiff < 0)	// If the height is out of range
		{
			pParticle->vPos.y -= (((int)(fDiff/m_fHeight)-1)*m_fHeight);
			pParticle->vPos.x = m_fWidth*(rand()%10000-5000)/10000.f;
			pParticle->vPos.z = m_fWidth*(rand()%10000-5000)/10000.f;

			pParticle->fRadius = (rand()%10000)/10000.f;
			pParticle->fRadian = 2*D3DX_PI*((rand()%10000)/10000.f);

			const float		fRadian = D3DX_PI*((rand()%10000)/10000.f);
			pParticle->vOffset1.Set(0, sqrt3*m_fSnowSize/3.f, 0);
			pParticle->vOffset2.Set(cosf(fRadian)*m_fSnowSize/2, -sqrt3*m_fSnowSize/6.f, sinf(fRadian)*m_fSnowSize/2);
			pParticle->vOffset3.Set(-cosf(fRadian)*m_fSnowSize/2, -sqrt3*m_fSnowSize/6.f, -sinf(fRadian)*m_fSnowSize/2);
		}
		else
		{
			fDiff = pParticle->vPos.y - (fCurY+fHalfHeight);
			if (fDiff > 0)	// If the height is out of range
				pParticle->vPos.y -= ((int)(fDiff/m_fHeight)+1)*m_fHeight;
			// If x is out of the width range
			fDiff = pParticle->vPos.x - fHalfWidth;
			if (fDiff > 0) pParticle->vPos.x -= ((int)(fDiff/m_fWidth)+1)*m_fWidth;
			fDiff = pParticle->vPos.x + fHalfWidth;
			if (fDiff < 0) pParticle->vPos.x -= ((int)(fDiff/m_fWidth)-1)*m_fWidth;
			// z out of width range
			fDiff = pParticle->vPos.z - fHalfWidth;
			if (fDiff >  0) pParticle->vPos.z -= ((int)(fDiff/m_fWidth)+1)*m_fWidth;
			fDiff = pParticle->vPos.z + fHalfWidth;
			if (fDiff < 0) pParticle->vPos.z -= ((int)(fDiff/m_fWidth)-1)*m_fWidth;
		}
		// Calculate position rotated around a central axis
		pParticle->fRadian += fAddRadian;
		__Vector3 vPos;	vPos.Set(cosf(pParticle->fRadian), 0, sinf(pParticle->fRadian));
		vPos += pParticle->vPos;
		
		// Resetting vertex buffer points
		pV1->x = vPos.x + pParticle->vOffset1.x;	pV1->y = vPos.y + pParticle->vOffset1.y;	pV1->z = vPos.z + pParticle->vOffset1.z;
		pV2->x = vPos.x + pParticle->vOffset2.x;	pV2->y = vPos.y + pParticle->vOffset2.y;	pV2->z = vPos.z + pParticle->vOffset2.z;
		pV3->x = vPos.x + pParticle->vOffset3.x;	pV3->y = vPos.y + pParticle->vOffset3.y;	pV3->z = vPos.z + pParticle->vOffset3.z;
	}
	m_pVB->Unlock();
}

void CN3GESnow::Render(__Vector3& vPos)
{
	if (m_bActive == FALSE || m_iVC <= 0 || m_pVB == nullptr) return;
	
	CN3GlobalEffect::Render(vPos);

	const int iCount = m_iVC / 3;
	int iActiveCount = iCount;
	if(m_iFadeMode > 0) // Gradually increase...
	{
		if(m_fFadeTime > 0 && m_fFadeTimeCur < m_fFadeTime) // Decide how much to skip and shoot because of the fade time..
		{
			iActiveCount = iCount * m_fFadeTimeCur / m_fFadeTime;
			if(iActiveCount > iCount) iActiveCount = iCount;
			else if(iActiveCount < 0) iActiveCount = 0;
		}
	}
	else if(m_iFadeMode < 0)
	{
		if(m_fFadeTime > 0 && m_fFadeTimeCur < m_fFadeTime) // Decide how much to skip and shoot because of the fade time..
		{
			iActiveCount = iCount * (1.0f - m_fFadeTimeCur / m_fFadeTime);
			if(iActiveCount > iCount) iActiveCount = iCount;
			else if(iActiveCount < 0) iActiveCount = 0;
		}
	}
	if(iActiveCount <= 0) return;

	// Save previous render state
	DWORD dwAlphaBlend, dwSrcAlpha, dwDestAlpha, dwCullMode, dwLight;
	s_lpD3DDev->GetRenderState( D3DRS_ALPHABLENDENABLE, &dwAlphaBlend );
	s_lpD3DDev->GetRenderState( D3DRS_SRCBLEND, &dwSrcAlpha );
	s_lpD3DDev->GetRenderState( D3DRS_DESTBLEND, &dwDestAlpha );
	s_lpD3DDev->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
	s_lpD3DDev->GetRenderState(D3DRS_LIGHTING, &dwLight);

	DWORD dwAddressU, dwAddressV;
	s_lpD3DDev->GetSamplerState(0, D3DSAMP_ADDRESSU, &dwAddressU);
	s_lpD3DDev->GetSamplerState(0, D3DSAMP_ADDRESSV, &dwAddressV);

	// set render state
	s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA);
	s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA);
	s_lpD3DDev->SetRenderState(D3DRS_CULLMODE,   D3DCULL_NONE);
	s_lpD3DDev->SetRenderState(D3DRS_LIGHTING,	 FALSE);

	// set transform
	s_lpD3DDev->SetTransform(D3DTS_WORLD, &m_Matrix);

	// set texture
	__ASSERT(m_pTex, "Texture pointer is NULL!");
	s_lpD3DDev->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0xffff0000);
	s_lpD3DDev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	s_lpD3DDev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	s_lpD3DDev->SetTexture(0, m_pTex->Get());

	// render
	s_lpD3DDev->SetFVF(FVF_XYZT1);
	s_lpD3DDev->SetStreamSource(0, m_pVB, 0, sizeof(__VertexXyzT1)); // Specify vertex buffer
	s_lpD3DDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, iActiveCount);

	// restore
	s_lpD3DDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlend);
	s_lpD3DDev->SetRenderState(D3DRS_SRCBLEND,   dwSrcAlpha);
	s_lpD3DDev->SetRenderState(D3DRS_DESTBLEND,  dwDestAlpha);
	s_lpD3DDev->SetRenderState( D3DRS_CULLMODE, dwCullMode );
	s_lpD3DDev->SetRenderState(D3DRS_LIGHTING, dwLight);
	s_lpD3DDev->SetSamplerState(0, D3DSAMP_ADDRESSU, dwAddressU);
	s_lpD3DDev->SetSamplerState(0, D3DSAMP_ADDRESSV, dwAddressV);
}

void CN3GESnow::Create(float fDensity, float fWidth, float fHeight, float fSnowSize, const __Vector3& vVelocity, float fTimeToFade)
{
	if(nullptr == s_lpD3DDev) return;

	Release();

	CN3GlobalEffect::FadeSet(fTimeToFade, true);

	m_fWidth = fWidth;	m_fHeight = fHeight;	m_fSnowSize = fSnowSize;
	m_vVelocity = vVelocity;
	const float fVolume = m_fWidth*m_fWidth*fHeight;
	__ASSERT(fVolume>0, "Snow volume is less than 0");
	const int iSnowCount = (int)(fVolume*fDensity);

	// Create m_pVB, m_pIB
	__ASSERT(s_lpD3DDev, "D3D Device pointer is NULL!");
	m_iVC = iSnowCount*3;
	HRESULT hr = s_lpD3DDev->CreateVertexBuffer(m_iVC*sizeof(__VertexXyzT1), D3DUSAGE_DYNAMIC, FVF_XYZT1, D3DPOOL_DEFAULT, &m_pVB, nullptr);
	if (FAILED(hr)) return;
	__VertexXyzT1* pVertices;
	hr = m_pVB->Lock(0, iSnowCount*3*sizeof(__VertexXyzT1), (void**)&pVertices, D3DLOCK_NOSYSLOCK);
	if (FAILED(hr)) return;

	// Populate __SnowParticle information
	m_pSnowParticle = new __SnowParticle[iSnowCount];

	const float sqrt3 = sqrtf(3.0f);
	int i;
	for (i=0; i<iSnowCount; ++i)
	{
		m_pSnowParticle[i].vPos.Set(fWidth*(rand()%10000-5000)/10000.f,
									fHeight*(rand()%10000-5000)/10000.f,
									fWidth*(rand()%10000-5000)/10000.f);
		m_pSnowParticle[i].fRadius = (rand()%10000)/10000.f;
		m_pSnowParticle[i].fRadian = 2*D3DX_PI*((rand()%10000)/10000.f);

		const float		fRadian = D3DX_PI*((rand()%10000)/10000.f);
		// equilateral triangle (one side length is fSnowSize)
		// m_pSnowParticle[i].vOffset1.Set(0, sqrt3*fSnowSize/3.f, 0);
		// m_pSnowParticle[i].vOffset2.Set(cosf(fRadian)*fSnowSize/2, -sqrt3*fSnowSize/6.f, class(fRadian)*fSnowSize/2);
		// m_pSnowParticle[i].vOffset3.Set(-cosf(fRadian)*fSnowSize/2, -sqrt3*fSnowSize/6.f, -inf(fRadian)*fSnowSize/2);

		// Isosceles triangle (base length fSnowSize, height fSnowSize)
		m_pSnowParticle[i].vOffset1.Set(0, fSnowSize/2.f, 0);
		m_pSnowParticle[i].vOffset2.Set(cosf(fRadian)*fSnowSize/2.f, -fSnowSize/2.f, sinf(fRadian)*fSnowSize/2.f);
		m_pSnowParticle[i].vOffset3.Set(-cosf(fRadian)*fSnowSize/2.f, -fSnowSize/2.f, -sinf(fRadian)*fSnowSize/2.f);

		// Enter uv coordinates
		__VertexXyzT1* pV1 = pVertices + i*3,	*pV2 = pVertices + i*3+1,	*pV3 = pVertices + i*3+2;
		// Arrange the UV coordinates so that the eye circle fills the triangle in the equilateral triangle (because the border color was not properly applied in the geforce2 card..)
		// pV1->tu = 0.5f;	pV1->tv = 0.5f - sqrt3/2.f;
		// pV2-&gt;tu = 0.5f + sqrt3/2.f; pV2-&gt;tv = 1.0f;
		// pV3-&gt;tu = 0.5f - sqrt3/2.f; pV3-&gt;tv = 1.0f;

		// Putting UV Coordinates in an Isosceles Triangle
		pV1->tu = 0.5f;	pV1->tv = 0.0f;
		pV2->tu = 1.0f;	pV2->tv = 1.0f;
		pV3->tu = 0.0f;	pV3->tv = 1.0f;

		// This method is to put a triangle in the eye texture square (so let&#39;s draw the eye texture to fit the triangle)

	}

	m_pVB->Unlock();
	m_pTex = s_MngTex.Get("Misc\\Snow.DXT", TRUE);
}
