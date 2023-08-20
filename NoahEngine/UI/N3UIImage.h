#pragma once

#include "N3UIBase.h"

class N3Texture;
class CN3UIImage : public CN3UIBase  
{
#ifdef _N3TOOL
	friend class CPropertyView;	// To access each variable in the tool
#endif

public:
	CN3UIImage();
	virtual ~CN3UIImage();

// Attributes
public:
	__FLOAT_RECT*			GetUVRect() {return &m_frcUVRect;}
protected:
	LPDIRECT3DVERTEXBUFFER9 m_pVB;			// vertex buffer
	CN3Texture*				m_pTexRef;		// texture reference pointer (as it is managed by s_TexMng, it is a reference pointer)
	std::string				m_szTexFN;		// The texture name.. The reason for having it separately is in case the tool fails to load the texture.
	__FLOAT_RECT			m_frcUVRect;	// save the uv coordinates
	D3DCOLOR				m_Color;		// Color (used when there is no background image)

	float					m_fAnimFrame;	// Number of images drawn per second
	int						m_iAnimCount;	// The number of images to be animated (determined by the number of children when loading)
	int						m_fCurAnimFrame;	// Animate frame index to be drawn currently (throw away the decimal and use it)
	CN3UIImage**			m_pAnimImagesRef;	// Reference pointers to child images to be animated

// Operations
public:
	D3DCOLOR	GetColor() const { return m_Color; }

	CN3Texture*		GetTex() const { return m_pTexRef; }	// Get Texture Pointer
	void			SetTex(const std::string& szFN);		// Specify Texture
	void			SetUVRect(float left, float top, float right, float bottom);	// Designation of uv coordinates of image
	void			SetColor(D3DCOLOR color);				// color designation

	virtual void	SetRegion(const RECT& Rect);					// zoning
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);
	virtual void	Release();								// Release
	virtual void	Tick();									// Tick
	virtual void	Render();								// drawing
	virtual void	RenderIconWrapper();
	virtual void	Init(CN3UIBase* pParent);				// reset
	virtual bool	Load(HANDLE hFile);

	virtual void	operator = (const CN3UIImage& other);

protected:
	bool			CreateVB();								// Create a vertex buffer with 4 vertices
	virtual void	SetVB();								// Resetting the vertex buffer
// Functions used by the tool
#ifdef _N3TOOL
public:
	virtual bool	Save(HANDLE hFile);
	virtual void	ChangeImagePath(const std::string& szPathOld, const std::string& szPathNew);
	void			GatherImageFileName(std::set<std::string>& setImgFile);
	std::string		GetTexFN() { return m_szTexFN; }

	void			ReorderChildImage();
	void			SetAnimImage(int iAnimCount);
	CN3UIImage*		GetChildImage(int iIndex);
	virtual bool	ReplaceAllTextures(const std::string& strFind, const std::string& strReplace);
#endif
};
