#pragma once

#include "N3Base.h"

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_3D		    0x0008		// 3D text

// set text flag
#define D3DFONT_CENTERED    0x0004		// 3D font에서만..

// Font rendering flags (drawtext or DrawText3D 함수에서 쓰는 flag)
#define D3DFONT_TWOSIDED    0x0010		// 3D font에서만..
#define D3DFONT_FILTERED    0x0020		// texture를 D3DTSS_MAGFILTER 로 찍기


class CDFont  : public CN3Base
{
public:
	CDFont(const std::string& szFontName, DWORD dwHeight, DWORD dwFlags=0L);
	virtual ~CDFont();

// Attributes
public:
	const std::string& GetFontName() { return m_szFontName; }
	DWORD	GetFontHeight() const {return m_dwFontHeight;}
	int		GetFontHeightInLogicalUnit() const {return -MulDiv(m_dwFontHeight, GetDeviceCaps(s_hDC, LOGPIXELSY), 72);}
	DWORD	GetFontFlags() const {return m_dwFontFlags;}
	SIZE	GetSize() const {return m_Size;}
	DWORD	GetFontColor() const {return m_dwFontColor;}

protected:
	static HDC s_hDC;						// DC handle
	static int s_iInstanceCount;			// Class Instance Count
	static HFONT s_hFontOld;				// default font


	std::string	m_szFontName;				// Font properties
    DWORD		m_dwFontHeight;					// Font Size
    DWORD		m_dwFontFlags;

    LPDIRECT3DDEVICE9       m_pd3dDevice;	// A D3DDevice used for rendering
    LPDIRECT3DTEXTURE9      m_pTexture;		// The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER9 m_pVB;			// VertexBuffer for rendering text
    DWORD	m_dwTexWidth;					// Texture dimensions
    DWORD	m_dwTexHeight;					// Texture dimensions
    FLOAT	m_fTextScale;					// 쓸 폰트가 너무 클경우 비디오 카드에
											// 따른 texture 크기 제한을 넘어버리기 때문에
											// 이런 경우 Scale을 이용하여 크게 늘려 찍는다.

//	HDC			m_hDC;							// DC handle
	HFONT		m_hFont;						// Font handle
	UINT		m_iPrimitiveCount;				// 글씨 찍을 판의 갯수
	D3DXVECTOR2	m_PrevLeftTop;				// DrawText의 경우 찍는 곳의 위치가 변경되었을때를 위한 변수
	DWORD		m_dwFontColor;				// 글씨 색
	BOOL		m_Is2D;						// 2D Text인가?
	SIZE		m_Size;						// 쓴 글씨들이 차지하는 크기(pixel단위, 가로 세로)

// Operations
public:
	bool	IsSetText() { if(m_pTexture) return true; return false; }
	void	AddToAlphaManager(DWORD dwColor, float fDist, __Matrix44& mtxWorld, DWORD dwFlags);
	HRESULT	SetFontColor(DWORD dwColor);						// 글씨 색을 바꾼다.
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );	// d3d device를 정해주는 초기화 함수 (Init할때 호출)
	HRESULT RestoreDeviceObjects();								// resource를 메모리에 세팅하는 초기화 함수 (Init할때 호출)
	HRESULT InvalidateDeviceObjects();							// resource등을 무효화시키는 함수 (release할때 호출)
	HRESULT DeleteDeviceObjects();								// resource등을 메모리에서 해제 (release할때 호출)

	HRESULT SetText(const std::string& szText, DWORD dwFlags = 0L);	// 출력할 글씨가 달라졌을때만 호출하는 것이 중요.
	HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, DWORD dwFlags, FLOAT fZ = 1.0f);	// 버퍼에 저장된 글씨를 그린다.(2d)
	HRESULT DrawText3D(DWORD dwColor, DWORD dwFlags );		// 버퍼에 저장된 글씨를 그린다.(3d)

	HRESULT	SetFont(const std::string& szFontName, DWORD dwHeight, DWORD dwFlags=0L);	// Font를 바꾸고 싶을때 호출한다. (dwHeight는 point size를 넣는다.)
	BOOL	GetTextExtent(const std::string& szString, int iStrLen, SIZE* pSize );
protected:
	void Make2DVertex(const int iFontHeight, const std::string& szText);					// 입력 받은 문자를 적절하게 배치된 2d 폴리곤으로 만든다.
	void Make3DVertex(const int iFontHeight, const std::string& szText, DWORD dwFlags);	// 입력 받은 문자를 적절하게 배치된 3d 폴리곤을 만든다.

};
