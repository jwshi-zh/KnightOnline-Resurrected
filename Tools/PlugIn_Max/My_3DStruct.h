#include "D3DX8.h"

struct __EXPORT_HEADER
{
	char szID[8];		// ID "N3Scene1"
	char szRemark[64];	// explanation..
	
	int nNodeCount; // total node count
	int nStaticFrame; // One frame of a still scene...
	BOOL bExportCurrentFrame; // the whole animation
	int nFrmStart;	// animation start frame
	int nFrmEnd;		// animation end frame

	D3DCOLORVALUE dcvBackground;	// background color
	D3DCOLORVALUE dcvAmbientLight;	// default light color

	int nCameraCount;	// number of cameras in the scene
	int nMaterialCount;	// number of materials in the scene
	int nTextureCount;	// number of textures in scene
	int nLightCount;	// number of lights in the scene

	BOOL bObjGeometry;	// I have 2D polygon data.
	BOOL bObjShape;		// I have 2D polygon data.
	BOOL bObjCamera;	// It has camera data.
	BOOL bObjLight;		// It has light data.
	BOOL bObjDummy;		// It has helper object (??? - refers to an object that is not needed in real game but only needed during development) data.

	BOOL bMesh;						// I have polygon data.
	BOOL bMaterial;					// Contains material data.
	
	BOOL bAnimationKey;				// I have an animation key.
	BOOL bMeshAnimation;			// Contains polygon animation keys.
	BOOL bCameraAndLightAnimation;	// It has a camera and lighting animation.
	BOOL bInverseKinematicsJoint;	// It has IK joints.

	BOOL bGenerateTexture; // Created by automatically optimizing the texture file. Saved as an "OBM" bitmap file in accordance with the Direct3D format in units of powers of 2.
	BOOL bGenerateIndexedVertex; // Encode and store as indexed points. More efficient in memory.
	BOOL bGenerateSmoothNormal; // Recompute normal vectors to make them look smooth.
};

struct __UVWH
{
	float	fV;
	float	fU;
	float	fW;
	float	fH;

	float	fUOffset;
	float	fVOffset;
	float	fUTile;
	float	fVTile;
};

