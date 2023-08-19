#pragma once

#include "N3Eng.h"

struct __EXPORT_OPTION
{
	char szID[8];		// ID "N3Scene1"
	char szRemark[64];
	
	int nNodeCount;
	
	BOOL	bAnimationKey; 
	int		nFrmStart;
	int		nFrmEnd;
	float	fSamplingRate;

	D3DCOLORVALUE dcvBackground;
	D3DCOLORVALUE dcvAmbientLight;

//	int nCameraCount;
//	int nMaterialCount;
//	int nTextureCount;
//	int nLightCount;

	BOOL bExportCamera;
	BOOL bExportLight;
	BOOL bExportGeometry;	// Contains geometry data.
	BOOL bExportDummy;	// It has helper object (??? - refers to an object that is not needed in real game but only needed during development) data.
	BOOL bExportCharacter;	// It has helper object (??? - refers to an object that is not needed in real game but only needed during development) data.
	
	BOOL bExportSelectedOnly; // Only selected ???

	BOOL bGenerateFileName; // Change the file name to 0_0000_00_0 format..??
	BOOL bGenerateSmoothNormal; // Recompute normal vectors to make them look smooth.
//	BOOL bGenerateProgressiveMesh; // Progressive Mesh produce
	BOOL bGenerateHalfSizeTexture; // Created by automatically optimizing the texture file. Saved as an "OBM" bitmap file in accordance with the Direct3D format in units of powers of 2.
	BOOL bGenerateCompressedTexture; // Use texture compression

	char szSubDir[_MAX_DIR];		// Specifies the subfolder to save when exporting.
};

class CN3EngTool : public CN3Eng  
{
public:
	int					m_nGridLineCount;
	__VertexColor*		m_pVGrids;

	__VertexColor		m_VAxis[60];
	__VertexColor		m_VDir[6];
	__VertexTransformed m_VPreview[6];



//	LPDIRECT3DDEVICE9	m_lpD3DDevExtra;

public:
//	bool CreateExtraDevice(int nWidth, int nHeight);
	void GridCreate(int nWidth, int nHeight);
	void RenderTexturePreview(CN3Texture* pTex, HWND hDlgWndDiffuse, RECT* pRCSrc = nullptr);
	void RenderGrid(const __Matrix44& mtxWorld) const;
	void RenderAxis(bool bShowDir = false);

	CN3EngTool();
	virtual ~CN3EngTool();

};
