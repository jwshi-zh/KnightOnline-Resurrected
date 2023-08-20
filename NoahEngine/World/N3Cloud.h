#pragma once

#include "N3Base.h"
#include "N3ColorChange.h"
#include <string>

const int NUM_CLOUD_VERTEX = 8;
// const int NUM_CLOUD_VERTEX = 12;

enum	e_CLOUDTEX {CLOUD_NONE=-1,CLOUD_WISPS=0, CLOUD_PUFFS, CLOUD_TATTERS, CLOUD_STREAKS,
	CLOUD_DENSE, CLOUD_OVERCAST, NUM_CLOUD};

class CN3Cloud : public CN3Base  
{
	friend class CN3SkyMng;
public:
	CN3Cloud();
	virtual ~CN3Cloud();

protected:
	__VertexXyzColorT2	m_pVertices[NUM_CLOUD_VERTEX]; // Vertices of the Cloud Layer
	CN3Texture*		m_pTextures[NUM_CLOUD];				// textures...
	std::string		m_szTextures[NUM_CLOUD];			// Texture file names...

	CN3ColorChange	m_Color1;							// cloud color 1
	CN3ColorChange	m_Color2;							// cloud color 2
	CN3ColorChange	m_Alpha;							// alpha value when clouds change
	e_CLOUDTEX		m_eCloud1;							// cloud texture 1
	e_CLOUDTEX		m_eCloud2;							// cloud texture 2
	e_CLOUDTEX		m_eCloud3;							// cloud texture 3

	float			m_fCloudTexTime;					// cloud change remaining time
	e_CLOUDTEX		m_eBackupCloud;						// Save the texture type to be changed for the second cloud
	float			m_fBackupTime;						// Save the time to change the 2nd cloud

// Operations
public:
	void			ChangeColor1(D3DCOLOR color, float fSec) {m_Color1.ChangeColor(color, fSec);}
	void			ChangeColor2(D3DCOLOR color, float fSec) {m_Color2.ChangeColor(color, fSec);}
	void			SetCloud(e_CLOUDTEX eCloud1, e_CLOUDTEX eCloud2, float fSec);
	void			Init(const std::string* pszFNs);
	virtual void	Release();
	virtual void	Render();
	virtual void	Tick();
protected:
	LPDIRECT3DTEXTURE9	GetTex(e_CLOUDTEX tex);

};
