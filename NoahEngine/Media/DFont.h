#pragma once

#include "N3Base.h"

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_3D		    0x0008

#define D3DFONT_CENTERED    0x0004

#define D3DFONT_TWOSIDED    0x0010
#define D3DFONT_FILTERED    0x0020


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
    FLOAT	m_fTextScale;					// If the font to be used is too large, the video card
											// Because it exceeds the texture size limit according to
											// In this case, use the scale to greatly increase the picture.

//	HDC			m_hDC;							// DC handle
	HFONT		m_hFont;						// Font handle
	UINT		m_iPrimitiveCount;				// The number of plates to be stamped
	D3DXVECTOR2	m_PrevLeftTop;				// In the case of DrawText, a variable for when the location of the recording location is changed
	DWORD		m_dwFontColor;
	BOOL		m_Is2D;
	SIZE		m_Size;						// Size occupied by written characters (pixel unit, width and height)

// Operations
public:
	bool	IsSetText() const
	{ if(m_pTexture) return true; return false; }
	void	AddToAlphaManager(DWORD dwColor, float fDist, __Matrix44& mtxWorld, DWORD dwFlags);
	HRESULT	SetFontColor(DWORD dwColor);
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );	// Initialization function that determines d3d device (called at Init)
	HRESULT RestoreDeviceObjects();								// Initialization function that sets resources in memory (called at Init)
	HRESULT InvalidateDeviceObjects();							// A function that invalidates resources, etc. (called when releasing)
	HRESULT DeleteDeviceObjects();								// Release resources, etc. from memory (called when releasing)

	HRESULT SetText(const std::string& szText, DWORD dwFlags = 0L);	// It is important to call only when the text to be printed has changed.
	HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, DWORD dwFlags, FLOAT fZ = 1.0f);	// Draw the text stored in the buffer. (2d)
	HRESULT DrawText3D(DWORD dwColor, DWORD dwFlags );		// Draw the text stored in the buffer. (3d)

	HRESULT	SetFont(const std::string& szFontName, DWORD dwHeight, DWORD dwFlags=0L);	// Called when you want to change the font. (Input the point size for dwHeight.)
	BOOL	GetTextExtent(const std::string& szString, int iStrLen, SIZE* pSize ) const;
protected:
	void Make2DVertex(const int iFontHeight, const std::string& szText);					// Converts input text into appropriately placed 2d polygons.
	void Make3DVertex(const int iFontHeight, const std::string& szText, DWORD dwFlags);	// Creates a 3d polygon with the input text appropriately placed.

};
