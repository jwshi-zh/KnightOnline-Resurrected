#pragma once

#include "My_3DStruct.h"

//const DWORD RF_NOTHING = 0;
//const DWORD RF_ALPHABLENDING = 1;
//const DWORD RF_NOTUSEFOG = 2;
//const DWORD RF_DOUBLESIDED = 4;

struct __AlphaPrimitive
{
	float				fCameraDistance;	// distance from the camera.
	DWORD				dwBlendSrc;			
	DWORD				dwBlendDest;
	int 				nRenderFlags;		// Render flags.. RF_... See also..
	LPDIRECT3DTEXTURE9	lpTex;				// Texture pointer
	DWORD				dwFVF;				// flexible vertex format
	D3DPRIMITIVETYPE	ePrimitiveType;	// Primitive Type 
	int					nPrimitiveCount;	// PrimitiveCount
	DWORD				dwPrimitiveSize;	// Primitive Size .. stream 0 stride
	BOOL				bUseVB;				// Whether to use vertex buffer or index buffer
	const void*			pwIndices;			// If it is index-based... just put something other than NULL.
	int					nVertexCount;
	const void*			pVertices;			// Triangle.. Although it is a vector type.. It must be written so that various point types can be entered through forced type conversion..
	__Matrix44			MtxWorld;			// Matrix
};

const int MAX_ALPHAPRIMITIVE_BUFFER = 1024;

class CN3AlphaPrimitiveManager
{
protected:
	int					m_nToDrawCount; // Number of buffers to draw
	__AlphaPrimitive	m_Buffers[MAX_ALPHAPRIMITIVE_BUFFER]; // Primitive buffer...

public:
	int					ToDrawCount() const { return m_nToDrawCount; }
	__AlphaPrimitive*	Add();

	void		Render();

	static int SortByCameraDistance(const void *pArg1, const void *pArg2); // sort function.
	
	CN3AlphaPrimitiveManager();
	virtual ~CN3AlphaPrimitiveManager();
};
