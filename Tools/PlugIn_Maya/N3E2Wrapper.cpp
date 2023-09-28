// Copyright (C) 1998-2000 Alias | Wavefront, a division of Silicon Graphics Limited.//
// The information in this file is provided for the exclusive use of the
// licensees of Alias | Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias | Wavefront license agreement, without fee.
//
// ALIAS | WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS | WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
// DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

// $Revision: /main/24 $
// $Date: 2000/04/19 16:12:24 $
//
//
// Description: 
//            
// N3E2 translator for Maya    
// 
//

#ifdef WIN32
#include <process.h>
#endif

#ifdef WIN32
#define MAXPATHLEN 1024
#endif

#include <crtdbg.h>
#include "N3E2Wrapper.h"
#include "IFFReader.h"

#include "../N3Base/N3Eng.h"
#include "../N3Base/N3Scene.h"
#include "../N3Base/N3PMeshCreate.h"

extern const char * objectName( MObject object );


#include "CommCtrl.h"
#include "Resource.h"

BOOL CN3E2Wrapper::m_bCancelExport = FALSE;
__EXPORT_OPTION CN3E2Wrapper::m_Option;

CN3EngTool*		g_pEng;

// CN3E2Wrapper class methods:
CN3E2Wrapper::CN3E2Wrapper()
{
	// progress dialog
	HWND hWnd = M3dView::applicationShell();
	HINSTANCE hInst = MhInstPlugin;
	m_hDlgProgress = CreateDialog(hInst, MAKEINTRESOURCE(IDD_PROGRESS), hWnd, CN3E2Wrapper::DlgProcProgress);
	__ASSERT(m_hDlgProgress, "Failed to create progress dialog");
	m_hWndPB = GetDlgItem(m_hDlgProgress, IDC_PROGRESS);
	m_hWndLB = GetDlgItem(m_hDlgProgress, IDC_LIST_PROGRESS);

	m_pScene = NULL;
	g_pEng = NULL;

	this->Release();
}

CN3E2Wrapper::~CN3E2Wrapper()
{
	::DestroyWindow(m_hDlgProgress); // Remove the progress box...
	this->Release();
}

void CN3E2Wrapper::Release()
{
	memset(&m_Option, 0, sizeof(m_Option));

	m_Option.bExportCamera		= TRUE;
	m_Option.bExportLight		= TRUE;
	m_Option.bExportGeometry	= TRUE;
	m_Option.bExportDummy		= FALSE;
	m_Option.bExportCharacter	= TRUE;

	m_Option.bAnimationKey	= TRUE;
	
	m_Option.bGenerateFileName = FALSE;
	m_Option.bGenerateSmoothNormal = TRUE;
//	m_Option.bGenerateProgressiveMesh = FALSE;
	m_Option.bGenerateHalfSizeTexture = FALSE;
	m_Option.bGenerateCompressedTexture = TRUE;
	m_Option.fSamplingRate = 30.0f;

	lstrcpy(m_szPath, "");		// path name
	lstrcpy(m_szFileName, "");	// file name

	delete m_pScene; m_pScene = NULL;
	delete g_pEng; g_pEng = NULL;
}

void CN3E2Wrapper::SetFileName(const char *szFileName)
{
	lstrcpy(m_szFileName, szFileName);	// file name
}

void CN3E2Wrapper::SetPath(const char *szPath)
{
	lstrcpy(m_szPath, szPath);	// file name
}

// Returns the light type.
CN3Light* CN3E2Wrapper::ProcessLight(MFnLight &mLight)
{
	CN3Light* pLight = new CN3Light();
	MFnTransform mT = MFnTransform(mLight.parent(0));
	this->ProcessTransform(mT, pLight); // Transform Node

	pLight->m_szName = mT.name().asChar();
	pLight->FileNameSet("Data\\" + pLight->m_szName + ".N3Light"); // Determine the file name...

	// light type
	D3DCOLORVALUE dcv = {1,1,1,1};
	pLight->m_Data.InitPoint(m_pScene->LightCount(), __Vector3(0,0,0), dcv);
	MFn::Type Type = mLight.type();
	if(Type == MFn::kDirectionalLight)
	{
		pLight->m_Data.Type = D3DLIGHT_DIRECTIONAL; // a directional light
	}
	else if(Type == MFn::kSpotLight) // a sopt light
	{
		pLight->m_Data.Type = D3DLIGHT_SPOT;
		
		MFnSpotLight Spot(mLight.object());
		pLight->m_Data.Theta = (float)Spot.coneAngle(); // inner cone angle
		pLight->m_Data.Phi = (float)Spot.penumbraAngle(); // external cone angle
	}
	else // By default point light
	{
		pLight->m_Data.Type = D3DLIGHT_POINT;
	}

	pLight->m_Data.Position = pLight->Pos();

	MFloatVector dir = mLight.lightDirection();
	pLight->m_Data.Direction.x = dir.x;
	pLight->m_Data.Direction.y = dir.y;
	pLight->m_Data.Direction.z = -dir.z;

	MColor color = mLight.color(); // light color.

	pLight->m_Data.Ambient.a = 1.0f;
	pLight->m_Data.Ambient.r = color.r * 0.3f;
	pLight->m_Data.Ambient.g = color.g * 0.3f;
	pLight->m_Data.Ambient.b = color.b * 0.3f;

	pLight->m_Data.Diffuse.a = 1.0f;
	pLight->m_Data.Diffuse.r = color.r;
	pLight->m_Data.Diffuse.g = color.g;
	pLight->m_Data.Diffuse.b = color.b;

/*	MFnDependencyNode lightDG(mLight);
    MObject lightAttr = lightDG.attribute(MString("intensity"));
	MPlug plug(mLight, lightAttr); // robbery
	double intensity;
	plug.getValue(intensity);
	
	lightAttr = lightDG.attribute(MString("specular"));
	plug.setAttribute(lightAttr);
	plug.getValue(
*/

	// the rest defaults.
	pLight->m_Data.nNumber = m_pScene->LightCount();
	pLight->m_Data.bOn = TRUE;

	return pLight;
}

CN3Camera* CN3E2Wrapper::ProcessCamera(MFnCamera &mCamera)
{
	CN3Camera* pCamera = new CN3Camera();
	MFnTransform mT(mCamera.parent(0));
	this->ProcessTransform(mT, pCamera); // Transform processing...
	std::string szFN = "Data\\" + pCamera->m_szName + ".N3Camera";
	pCamera->FileNameSet(szFN);

//	double dHFOV, dVFOV;
//	mCamera.getPortFieldOfView(800, 600, dHFOV, dVFOV);
//	pCamera->s_CameraData.fNP = (float)(mCamera.nearClippingPlane() * 0.01); // Divide by 100 because it is in meters.
//	pCamera->s_CameraData.fFP = (float)(mCamera.farClippingPlane() * 0.01);
//	pCamera->s_CameraData.fFOV = (float)dHFOV; // Forced setting with 90 degree lens..

	return pCamera;
}

MObject CN3E2Wrapper::MeshGetShader(MFnMesh& mMesh)
{
	MObjectArray ShaderArray; MIntArray IndicesArray;
	mMesh.getConnectedShaders(0, ShaderArray, IndicesArray);
//	int nInstance = PathCur.instanceNumber();
//	Mesh.getConnectedShaders(nInstance, ShaderArray, IndicesArray);

	int nSC = ShaderArray.length();
	int nIC = IndicesArray.length();
	if(nSC > 0)
	{
		MFnDependencyNode NodeShader(ShaderArray[0]);
		MObject ssAttr = NodeShader.attribute(MString("surfaceShader"));
		MPlug Plug(ShaderArray[0], ssAttr);
		MPlugArray srcPlugArray;
		Plug.connectedTo( srcPlugArray, true, false); // as destination, as source

		if(srcPlugArray.length() > 0) 
			return srcPlugArray[0].node();
	}

	MObject nullObj;
	return nullObj;
}

void CN3E2Wrapper::SceneExport()
{
	// Option Dialog 
	HINSTANCE hInst = MhInstPlugin;
	HWND hWnd = M3dView::applicationShell();
	int rval = DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_EXPORT_OPTION), hWnd, DlgProcPane, NULL); // Control panel dialog..
	if(rval != 1) return;
	
	g_pEng = new CN3EngTool();
//	g_pEng->InitEnv();
	g_pEng->Init(TRUE, hWnd, 64, 64, 0, FALSE);
	
	char szPath[256];
	::GetCurrentDirectory(256, szPath);
	g_pEng->PathSet(szPath); // Set route...

	m_pScene = new CN3Scene();

	if(NULL == g_pEng->s_lpD3DDev)
	{
		delete m_pScene; m_pScene = NULL;
		delete g_pEng; g_pEng = NULL;

		MessageBox(::GetActiveWindow(), "Video Memory is insufficient. Secure video memory and try again.", "Data Export Error", MB_OK);
		return;
	}

	MTime mOldTime = MAnimControl::currentTime(); // Stores the current frame number.
	MTime ZeroFrm; 
	ZeroFrm.setValue(0); // ZeroFrm.setUnit(MTime::kNTSCFrame);
	MAnimControl::setCurrentTime(ZeroFrm);
	MGlobal::viewFrame(ZeroFrm);
	
	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	m_pScene->Release(); // unlock all...

//	m_pScene->m_fFrmCur = (float)MAnimControl::currentTime().value();
	m_pScene->m_fFrmStart = (float)m_Option.nFrmStart; // (float)MAnimControl::minTime().value();
	m_pScene->m_fFrmEnd = (float)m_Option.nFrmEnd; // (float)MAnimControl::maxTime().value();

	MObjectArray mObjects;

	int i = 0;

	MItDependencyNodes IterNodes1(MFn::kCamera); // Check the nodes of the scene..
	MItDependencyNodes IterNodes2(MFn::kLight); // Check the nodes of the scene..
	MItDependencyNodes IterNodes3(MFn::kMesh); // Check the nodes of the scene..
	MItDependencyNodes IterNodes4(MFn::kSkinClusterFilter); // Check the nodes of the scene..

	for(i = 0; !IterNodes1.isDone(); IterNodes1.next(), i++) mObjects.append(IterNodes1.item());
	for(i = 0; !IterNodes2.isDone(); IterNodes2.next(), i++) mObjects.append(IterNodes2.item());
	for(i = 0; !IterNodes3.isDone(); IterNodes3.next(), i++) mObjects.append(IterNodes3.item());
	for(i = 0; !IterNodes4.isDone(); IterNodes4.next(), i++) mObjects.append(IterNodes4.item());

	MSelectionList mSelList;
	if(m_Option.bExportSelectedOnly)
	{
		MGlobal::getActiveSelectionList(mSelList);
	}

	// Object Count... 
	int nObjectCount = mObjects.length();

	// Progress dialog control handle
	::SendMessage(m_hWndPB, PBM_SETRANGE, 0, MAKELPARAM(0, nObjectCount));
	::SendMessage(m_hWndLB, LB_RESETCONTENT, 0, 0);
	::ShowWindow(m_hDlgProgress, SW_SHOW); // Show progress dialog..
	::ShowWindow(m_hDlgProgress, SW_RESTORE);

	m_bCancelExport = FALSE;

	for (i = 0; i < nObjectCount && FALSE == m_bCancelExport; i++)
	{
		// Dialog message processing...
		MSG msg;
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

		if(TRUE == m_bCancelExport) break; // Cancel by pressing the Export Cancel button..

		MObject mObj = mObjects[i];
		MFn::Type mType = mObj.apiType();

		if(m_Option.bExportSelectedOnly && IsSelected(mSelList, mObj) == false) continue; // If you need to export only the selected..

		bool bExport = true;
		if(MFn::kCamera == mType && TRUE == m_Option.bExportCamera) // camera..
		{
			MFnCamera mCamera(mObj);
			const char* szCamera = mCamera.name().asChar();
			if(	strstr(szCamera, "front") == NULL && 
				strstr(szCamera, "side") == NULL && 
				strstr(szCamera, "top") == NULL)
			{
				CN3Camera* pCamera = this->ProcessCamera(mCamera);
				m_pScene->CameraAdd(pCamera);
			}
		}
		else if(	(MFn::kPointLight == mType ||
					MFn::kDirectionalLight == mType ||
					MFn::kAmbientLight == mType ||
					MFn::kSpotLight == mType) && 
					TRUE == m_Option.bExportLight )// light
		{
			CN3Light* pLight = this->ProcessLight(MFnLight(mObj));
			pLight->m_Data.nNumber = m_pScene->LightCount(); // 라이트 번호 붙이기..
			m_pScene->LightAdd(pLight); // Adding lights to the scene..
		}
		else if(mType == MFn::kMesh && TRUE == m_Option.bExportGeometry)
		{
			MFnMesh mMesh(mObj);

			const char* szMeshName = mMesh.name().asChar();

			bool bHaveJoint = false; // If you have to deal with the skeleton..
			if(TRUE == m_Option.bExportCharacter) bHaveJoint = this->HaveJoint(mMesh);
			if(true == bHaveJoint) // The mesh connected to the joint passes through..
			{
//				wsprintf(szInfo, "Ignore the skinned mesh (%s).", mMesh.name().asChar());
//				nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
//				::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
				continue; 
			}
			else
			{
				bool bProcessShape = false;
				if(mMesh.parentCount() > 0 && mMesh.parent(0).apiType() == MFn::kTransform)
				{
					MFnTransform mTM(mMesh.parent(0));
					if(mTM.childCount() == 1)  bProcessShape = true; // Only handles first child..
					else if(mTM.childCount() > 1 && mTM.child(0) == mMesh.object()) bProcessShape = true; // Only handles first child..
				}
				if(bProcessShape) ProcessShape(mMesh);
			}
		}
		else if(mType == MFn::kSkinClusterFilter && m_Option.bExportCharacter) // skinning data and character....
		{
			MFnSkinCluster mSkin(mObj);
			this->ProcessChr(mSkin);
		}
		else
		{
			wsprintf(szInfo, "Ignoring node (%s).", mObj.apiTypeStr());
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			bExport = false;
		}

		if(bExport == true)
		{
			if(mObj.hasFn(MFn::kDependencyNode) == true)
			{
				MFnDependencyNode node(mObj);
				wsprintf(szInfo, "%d/%d - Type : %s Name : %s", i, nObjectCount, mObj.apiTypeStr(), node.name().asChar());
			}
			else
			{
				wsprintf(szInfo, "%d/%d - Type : %s NoName", i, nObjectCount, mObj.apiTypeStr());
			}
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
			SendMessage(m_hWndPB, PBM_SETPOS, i+1, 0); // Progress dialog
		}
	}

	// Save scene.
	bool bSaveOK = true;
	if(m_bCancelExport == TRUE)
	{
		bSaveOK = false;
		int nYesNo = MessageBox(hWnd, "Save File", "You canceled the export. do you want to save?", MB_YESNO);
		if(nYesNo == IDYES) bSaveOK = true;
	}
	
	if(bSaveOK)
	{
		m_pScene->Tick(0);
		if(m_pScene->CameraCount() <= 0) m_pScene->DefaultCameraAdd(); // primary camera
		if(m_pScene->LightCount() <= 0) m_pScene->DefaultLightAdd(); // Add basic light...

		m_pScene->SaveDataAndResourcesToFile(m_szFileName); // Save Scene , Resource..
		m_pScene->Release();
	}

	MAnimControl::setCurrentTime(mOldTime); // Originally..
	MGlobal::viewFrame(mOldTime);

	delete m_pScene; m_pScene = NULL;
	delete g_pEng; g_pEng = NULL;
}

bool CN3E2Wrapper::ProcessIMesh(MFnMesh &mMesh, CN3IMesh* pIMesh)
{
	if(NULL == pIMesh) return false;
	pIMesh->Release();

	// make a name...
	MFnTransform mTM(mMesh.parent(0));
	pIMesh->m_szName = mTM.name().asChar();

	// Counts the number of polygons to make all triangle meshes.
	int nPC = mMesh.numPolygons();
	if(nPC <= 0) return false;

	int nFC = 0, nVI = 0, nUVI = 0;

	MIntArray mVIArray, mVITmp, mUVIArray;
	MFloatArray mFAU, mFAV;
	MPointArray mVArray;
	mMesh.getUVs(mFAU, mFAV);
	int nUVC = mFAU.length(); // UV Coordinate Count
	mMesh.getPoints(mVArray);
	int nVC = mVArray.length(); // Vertex Count
	
	std::vector<MVector> Normals(nVC, MVector());
	std::vector<int> iNormalCounts(nVC, 0);

	for(int i = 0; i < nPC; i++)
	{
		MVector vNTmp;
		mMesh.getPolygonNormal(i, vNTmp); // normal
		mMesh.getPolygonVertices(i, mVITmp); // Point Index in polygon

		mMesh.getPolygonUVid(i, 0, nUVI);
		nVI = mVITmp[0];
		Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // Add the normal value of the face.
		mVIArray.append(nVI); // Vertex Index
		mUVIArray.append(nUVI); // Record VertexIndex, UV Index..

		mMesh.getPolygonUVid(i, 1, nUVI);
		nVI = mVITmp[1];
		Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // Add the normal value of the face.
		mVIArray.append(nVI); // Vertex Index
		mUVIArray.append(nUVI); // Record VertexIndex, UV Index..

		mMesh.getPolygonUVid(i, 2, nUVI);
		nVI = mVITmp[2];
		Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // Add the normal value of the face.
		mVIArray.append(nVI); // Vertex Index
		mUVIArray.append(nUVI); // Record VertexIndex, UV Index..

		nFC++;

		int nVITmp = mVITmp.length();
		for(int j = 3; j < nVITmp; j++) // If there are more than four dots on one side...
		{
			mMesh.getPolygonUVid(i, 0, nUVI);
			nVI = mVITmp[0];
			Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // Add the normal value of the face.
			mVIArray.append(nVI); // Vertex Index
			mUVIArray.append(nUVI); // Record VertexIndex, UV Index..

			mMesh.getPolygonUVid(i, nVITmp - j + 1, nUVI);
			nVI = mVITmp[nVITmp - j + 1];
			Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // Add the normal value of the face.
			mVIArray.append(nVI); // Vertex Index
			mUVIArray.append(nUVI); // Record VertexIndex, UV Index..

			mMesh.getPolygonUVid(i, nVITmp - j + 2, nUVI);
			nVI = mVITmp[nVITmp - j + 2];
			Normals[nVI] += vNTmp; iNormalCounts[nVI]++; // Add the normal value of the face.
			mVIArray.append(nVI); // Vertex Index
			mUVIArray.append(nUVI); // Record VertexIndex, UV Index..

			nFC++;
		}
	}

	pIMesh->Create(nFC, nVC, nUVC); // Create a mesh...

	MFnTransform mTransform(mMesh.parent(0));
	MMatrix mMtxWorld; mMtxWorld.setToIdentity();
	this->GetWorldTransform(mTransform, mMtxWorld);
	MMatrix mMtxWorldRot = mMtxWorld; // Just remove the rotation matrix.
	mMtxWorldRot.matrix[3][0] = mMtxWorldRot.matrix[3][1] = mMtxWorldRot.matrix[3][2] = 0;
	mMtxWorldRot.matrix[3][3] = 1.0;
	
	__VertexXyzNormal* pVDest = pIMesh->Vertices();
	for(i = 0; i < nVC; i++) 
	{
		MPoint mVTmp = mVArray[i]; // Multiply the World Matrix...
		mVTmp *= mMtxWorld;
		pVDest[i].x = (float)(mVTmp.x * 0.01); // Vertex Data Setting - Divide by 100 because it is in meters.
		pVDest[i].y = (float)(mVTmp.y * 0.01);
		pVDest[i].z = -(float)(mVTmp.z * 0.01); // The Z axis does the reverse.

		// Handling Normal values...
//		if(iNormalCounts[i] > 0)
//		{
//			mNTmp.x = mNTmp.x / iNormalCounts[i];
//			mNTmp.y = mNTmp.y / iNormalCounts[i];
//			mNTmp.z = mNTmp.z / iNormalCounts[i];
//		}
		
		MVector mNTmp = Normals[i] * mMtxWorldRot;
		mNTmp.normalize();
		pVDest[i].n.x = (float)mNTmp.x;
		pVDest[i].n.y = (float)mNTmp.y;
		pVDest[i].n.z = -(float)mNTmp.z; // The Z axis does the reverse.

	}

	for(i = 0; i < nUVC; i++) pIMesh->UVSet(i, mFAU[i], 1.0f - mFAV[i]); // UV data setting..
//	for(i = 0; i < nUVC; i++) pIMesh->UVSet(i, mFAU[i], mFAV[i]); // UV data setting..
	for(i = 0; i < nFC; i++) // 
	{
		pIMesh->VertexIndexSet(i*3+0, mVIArray[i*3+0]); // Vertex Index Settings
		pIMesh->VertexIndexSet(i*3+1, mVIArray[i*3+2]);
		pIMesh->VertexIndexSet(i*3+2, mVIArray[i*3+1]);

		if(nUVC > 0) // Must have UV Index.
		{
			pIMesh->UVIndexSet(i*3+0, mUVIArray[i*3+0]); // UV Index setting..
			pIMesh->UVIndexSet(i*3+1, mUVIArray[i*3+2]);
			pIMesh->UVIndexSet(i*3+2, mUVIArray[i*3+1]);
		}
	}

	if(m_Option.bGenerateSmoothNormal)
	{
		pIMesh->ReGenerateSmoothNormal();
	}

	return true;
}

bool CN3E2Wrapper::ProcessVMesh(MFnMesh &mMesh, CN3VMesh* pVMesh)
{
	if(NULL == pVMesh) return false;
	pVMesh->Release();

	// Counts the number of polygons to make all triangle meshes.
	int nPC = mMesh.numPolygons();
	if(nPC <= 0) return false;

	pVMesh->CreateVertices(nPC*3);
	__Vector3* pvDests = pVMesh->Vertices();

	MIntArray mVIs;
	MPointArray mVs; mMesh.getPoints(mVs); // locations...
	MPoint mPosTmp;

	MFnTransform mTransform(mMesh.parent(0)); // world matrix
	MMatrix mMtxWorld; mMtxWorld.setToIdentity();
	this->GetWorldTransform(mTransform, mMtxWorld);
	
	for(int i = 0; i < nPC; i++)
	{
		mMesh.getPolygonVertices(i, mVIs); // Point Index in polygon
		if(mVIs.length() != 3)
		{
			char szErr[256];
			wsprintf(szErr, "%s is not a triangular polygon.", mMesh.name().asChar());
			break;
		}

		for(int j = 0; j < 3; j++)
		{
			mPosTmp = mVs[mVIs[j]];
			mPosTmp *= mMtxWorld;

			pvDests[i*3+j].Set(	(float)(mPosTmp.x*0.01),
								(float)(mPosTmp.y*0.01),
								-(float)(mPosTmp.z*0.01) );
		}
	}

	// Name processing...
	pVMesh->m_szName = "";
	this->ProcessName(mTransform.object(), pVMesh->m_szName);
	std::string szFN = pVMesh->m_szName + ".N3VMesh";
	pVMesh->FileNameSet(szFN);

	return true;
}

bool CN3E2Wrapper::ProcessSkin(MFnSkinCluster &mSkin, CN3Skin* pSkin)
{
	if(NULL == pSkin) return false;
	pSkin->Release();

	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	MObjectArray mMeshArray;
	mSkin.getOutputGeometry(mMeshArray);
	__ASSERT(mMeshArray.length() >= 1, "Input geometry count is not 1");
	MFnMesh mMeshOutput(mMeshArray[0]); // Original Mesh

	MStatus stat;
	MDagPath mSkinPath;
	int nGI = mSkin.indexForOutputConnection(0, &stat); // Geometry Index
	mSkin.getPathAtIndex(nGI, mSkinPath); // Geometry Path
	MItGeometry mGIt(mSkinPath); // iterate through the components of this geometry

	int nGVC = mGIt.count(); // Geometry Data
	MPointArray mVArray;
	mMeshOutput.getPoints(mVArray);
	int nVC = mVArray.length();
	if(nGVC != nVC) 
	{
		char szWarning[256];
		MFnDagNode nodeTmp(mMeshOutput.parent(0));
		wsprintf(szWarning, "Mesh - %s, Skin - %s", nodeTmp.name().asChar(), mSkin.name().asChar());
		MessageBox(::GetActiveWindow(), szWarning, "Warning - Skin vertex count is different to mesh vertex count", MB_OK);
	}











	if(false == this->ProcessIMesh(mMeshOutput, pSkin)) return false; // Indexed mesh processing.

	// If Joint is grouped, Parent Transform processing..
/*	if(mJointRoot.parentCount() > 0 && mJointRoot.parent(0).hasFn(MFn::kTransform))
	{
		MFnTransform mTJP(mJointRoot.parent(0));

		// Reset normal values..
		MMatrix mMtx; mMtx.setToIdentity();
		this->GetWorldTransform(mTJP, mMtx);
		MTransformationMatrix mTMtx(mMtx);

		double dRots[3];
		MTransformationMatrix::RotationOrder rotOrder = MTransformationMatrix::kXYZ;
		mTMtx.getRotation(dRots, rotOrder);
		__Matrix44 mtxRot;
		mtxRot.Rotation((float)dRots[0], (float)dRots[1], -(float)dRots[2]);

		int nVC = pIMesh->VertexCount();
		__VertexXyzNormal* pVs = pIMesh->Vertices();
		__Vector3 vNTmp;
		
		for(int i = 0; i < nVC; i++)
		{
			vNTmp = pVs[i].n;
			vNTmp *= mtxRot;
			pVs[i].n = vNTmp;
		}
	}
*/


	__VertexSkinned* pVBone = pSkin->SkinVertices();
	MFnTransform mTM(mMeshOutput.parent(0)); // Transform applied to Mesh and Joint
	MMatrix mMtxM; mMtxM.setToIdentity();
	this->GetWorldTransform(mTM, mMtxM);

	int iFind = pSkin->m_szName.find('_');
	if(iFind > 0) pSkin->m_szName = pSkin->m_szName.substr(iFind + 1); // If there is an underbar, cut it off.
	pSkin->FileNameSet("Item\\" + pSkin->m_szName + ".N3Skin"); // Determine the file name...

	for(int i = 0; !mGIt.isDone(); mGIt.next(), i++) // Process the component (a point of Geometry...) while looping.
	{
		MObject mComp = mGIt.component();
		MFn::Type mType = mComp.apiType();

		// Get the weight value.
		MFloatArray mWTs;
		unsigned int nWTs;
		mSkin.getWeights(mSkinPath, mComp, mWTs, nWTs);
		if (nWTs == 0)
		{
			lstrcpy(szInfo, "Skin processing error: no weight value.");
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			continue;
		}

		MPoint mPt;
		mPt = mGIt.position();
//		mPt = mVArray[j]; // Originally, this is normal.. but I have to go back to the binding pose..

		mPt *= mMtxM; // Multiply the matrix to address..
		mPt.x *= 0.01; mPt.y *= 0.01; mPt.z *= -0.01; // Z axis is negative..
		
		pVBone[i].vOrigin.Set((float)mPt.x, (float)mPt.y, (float)mPt.z);

		
		// Only those whose weight value is greater than 0 are selected.
		int nAffect = 0;
		static int s_nJoints[128];
		static float s_fWeights[128];
		for (int j = 0; j < (int)nWTs ; j++ )
		{
			if(mWTs[j] > 0) 
			{
				s_nJoints[nAffect] = j;
				s_fWeights[nAffect] = mWTs[j];
				nAffect++;
			}
		}

		if(nAffect > 0)
		{
			pVBone[i].nAffect = nAffect;
			pVBone[i].pnJoints = new int[nAffect];
			if(nAffect > 1) pVBone[i].pfWeights = new float[nAffect];

			for (j = 0; j < nAffect; j++ )
			{
				pVBone[i].pnJoints[j] = s_nJoints[j];
				if(nAffect > 1) pVBone[i].pfWeights[j] = s_fWeights[j];
			}
		}
	}

	return true;
}

bool CN3E2Wrapper::ProcessShape(MFnMesh& mMesh)
{
	if(mMesh.numPolygons() > 10000) // If the number of faces exceeds 10,000... Cannot be processed with CN3IMesh...
	{
		CN3VMesh VMesh;
		this->ProcessVMesh(mMesh, &VMesh);
		VMesh.SaveToFile();
		return false;
	}

	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	MFnDagNode mNodeTmp(mMesh.object());
	MFnTransform mTG;
	bool bValid = false;
	while(1)
	{
		if(mNodeTmp.parent(0).apiType() == MFn::kWorld)
		{
			mTG.setObject(mNodeTmp.object());
			bValid = true;
			break;
		}

		mNodeTmp.setObject(mNodeTmp.parent(0));
	}
	if(false == bValid) return false;

	MFnTransform mTM(mMesh.parent(0));

	std::string szNodeFullName;
	this->ProcessName(mTG.object(), szNodeFullName);
	int nSC = m_pScene->ShapeCount();
	CN3Shape* pShape = NULL;
	for(int i = 0; i < nSC; i++)
	{
		if(szNodeFullName == m_pScene->ShapeGet(i)->m_szName)
		{
			pShape = m_pScene->ShapeGet(i);
			break;
		}
	}

	if(NULL == pShape)
	{
		pShape = new CN3Shape();
		this->ProcessTransform(mTG, pShape);
		pShape->m_szName = ""; this->ProcessName(mTG.object(), pShape->m_szName); // Rename.
		pShape->FileNameSet("Object\\" + pShape->m_szName + ".N3Shape"); // Naming the file...
		
		m_pScene->ShapeAdd(pShape);

		MMatrix mMtxJWorld; mMtxJWorld.setToIdentity();
		this->GetWorldTransform(mTG, mMtxJWorld);

		// Pivot Point Calculation
		MPoint mvPivot;
		mvPivot = mTG.rotatePivot(MSpace::kTransform);
		pShape->PosSet((float)(mvPivot.x*0.01), (float)(mvPivot.y*0.01), -(float)(mvPivot.z*0.01));
	}

	CN3IMesh IMesh;
	this->ProcessIMesh(mMesh, &IMesh);
	if(IMesh.FaceCount() <= 0) return false;

	// If the name contains the string "collision", it is a collision mesh..
	std::string szTmp = IMesh.m_szName;
	if(szTmp.size()) CharLower(&(szTmp[0]));
	if(szTmp.find("coll") != -1 || szTmp.find("climb") != -1)
	{
		// Recalculate the point position of the mesh with respect to the pivot point. Align it with the local coordinates of the Shape.
		IMesh.ApplyOffset(pShape->Pos() * -1.0f);
		int nFC = IMesh.FaceCount();

		CN3VMesh* pVMesh = new CN3VMesh();
		szNodeFullName = ""; this->ProcessName(mTM.object(), szNodeFullName);
		pVMesh->m_szName = mTM.name().asChar(); // Renamed...
		pVMesh->FileNameSet("Object\\" + szNodeFullName + ".N3VMesh"); // Determine the file name...

		pVMesh->CreateVertices(nFC*3);

		__VertexT1* pVSrc = IMesh.BuildVertexList();
		__Vector3* pVDest = pVMesh->Vertices();
		for(i = 0; i < nFC * 3; i++)
		{
			pVDest[i].x = pVSrc[i].x; // Just set the location...
			pVDest[i].y = pVSrc[i].y; // Just set the location...
			pVDest[i].z = pVSrc[i].z; // Just set the location...
		}
		pShape->s_MngVMesh.Add(pVMesh);
		if(szTmp.find("coll") != -1) pShape->CollisionMeshSet(pVMesh->FileName());
		else if(szTmp.find("climb") != -1) pShape->ClimbMeshSet(pVMesh->FileName());
	}
	else // Unless it's a collision mesh...
	{
		CN3SPart* pPD = pShape->PartAdd(); // Add a part...
		szNodeFullName = ""; this->ProcessName(mTM.object(), szNodeFullName);
		pPD->m_szName = mTM.name().asChar(); // Naming the Part...
		pPD->FileNameSet("Object\\" + szNodeFullName + "N3CPart");

		// Pivot Point Calculation
		MPoint mvPivot = mTM.rotatePivot(MSpace::kTransform);
		pPD->m_vPivot.Set((float)(mvPivot.x*0.01), (float)(mvPivot.y*0.01), -(float)(mvPivot.z*0.01));
		pPD->m_vPivot -= pShape->Pos();

		// Recalculate the point position of the mesh with respect to the pivot point of the shape part. Align it with the local coordinates of the part of the shape.
		__Vector3 vOffset((float)(mvPivot.x * 0.01), (float)(mvPivot.y * 0.01), -(float)(mvPivot.z * 0.01));
		IMesh.ApplyOffset(vOffset * -1.0f);

		// Convert to CN3Mesh
		CN3Mesh N3Mesh;
		N3Mesh.Import(&IMesh);
		N3Mesh.MakeIndexed();
		if(m_Option.bGenerateSmoothNormal)
		{
			N3Mesh.ReGenerateSmoothNormal();
		}

		// Create Progressive Mesh..
		CN3PMeshCreate PMeshCreate;
		if (PMeshCreate.ConvertFromN3Mesh(&N3Mesh)==false)
		{
			MessageBox(::GetActiveWindow(), "N3Mesh that cannot be converted or has no content.", "PMesh Convert error", MB_OK);
			//return;
		}

		// Automatic PMesh generation option
		PMeshCreate.m_PMCOption.bUseSumOfLoss = true;	// Use the sum of losses
		PMeshCreate.m_PMCOption.bTriangleWeight = true;	// Using vanishing triangle weights
		PMeshCreate.m_PMCOption.bArea = true;	// false: The three sides are the sum of the lengths.
		PMeshCreate.m_PMCOption.fWeight = 1.0f;	// Disappearing triangle weights (importance)

		CN3PMesh *pPMesh = PMeshCreate.CreateRendererMesh(); // Create Progressive Mesh
		pPMesh->m_szName = mTM.name().asChar(); // Just a name..
		std::string szFN; this->ProcessName(mTM.object(), szFN);

		int iLen = szFN.size();
		if(m_Option.bGenerateFileName && iLen >= 11) // Item Code ::: 0_2345_78_0
			szFN = szFN.substr(iLen - 11);
		pPMesh->FileNameSet("Object\\" + szFN + ".N3PMesh"); // Determine the file name...

		pShape->s_MngPMesh.Add(pPMesh); // Added to Progressive Mesh Manager..
		pPD->MeshSet(pPMesh->FileName()); // Mesh settings...
		
		////////////////////////////////////////////////
		// Material and texture handling..
		pPD->m_Mtl.dwColorArg1 = D3DTA_DIFFUSE;
		pPD->m_Mtl.dwColorArg2 = D3DTA_TEXTURE;

		CN3Texture* pTex = this->ProcessTexture(mMesh);
		if(pTex) // If the material has a texture, the material is basically white. The color operation is Modulate.
		{
			std::string szTFN = pTex->FileName(); // Checking the file name...
			if(szTFN.size() > 0) CharLower(&(szTFN[0]));
			if(-1 == szTFN.find("object\\")) // If there is no string called "Item\"
			{
				szTFN = "Object\\" + pTex->FileName(); // Attach the path named Item.
				pTex->FileNameSet(szTFN);
			}

			
			pPD->m_Mtl.dwColorOp = D3DTOP_MODULATE;
			pPD->TexAlloc(1);
			pPD->TexSet(0, pTex); // Texture Setting ... 
			
//			D3DFORMAT fmtTex = pTex->PixelFormat();
//			if(	fmtTex == D3DFMT_DXT2 ||
//				fmtTex == D3DFMT_DXT3 || 
//				fmtTex == D3DFMT_DXT4 || 
//				fmtTex == D3DFMT_DXT5 ) pPD->m_Mtl.nRenderFlags = RF_ALPHABLENDING;
		}
		else
		{
//			this->ProcessMaterial(this->MeshGetShader(mMesh), &(pPD->m_Mtl));
			pPD->m_Mtl.dwColorOp = D3DTOP_SELECTARG1;
		}
		// Material and texture handling..
		////////////////////////////////////////////////
	}

	return true;
}

CN3Joint* CN3E2Wrapper::ProcessJoint(MFnIkJoint &mJoint)
{
	CN3Joint* pJoint = new CN3Joint();
	//////////////////////////////////////////////////////////////////////////////////////////
	// Transform of Joint is processed..
	this->ProcessTransform(mJoint, pJoint);
	
	// Rotation Order
	MTransformationMatrix::RotationOrder rotOrder = mJoint.rotationOrder(); // MTransformationMatrix::kXYZ;
//	if(rotOrder == MTransformationMatrix::kXYZ) pJoint->m_RotSeq = CN3Joint::ROT_SEQ_XYZ;
//	else if(rotOrder == MTransformationMatrix::kYXZ) pJoint->m_RotSeq = CN3Joint::ROT_SEQ_YXZ;
//	else
//	{
//		MessageBox(::GetActiveWindow(), "NotSupported rotation order. Must kXYZ or kYXZ", "Joint export Warning", MB_OK);
//	}

	char szName[512];
	if(mJoint.parent(0).apiType() == MFn::kTransform)
	{
		MFnTransform mTJ(mJoint.parent(0));
		wsprintf(szName, "Chr\\%s.N3Joint", mTJ.name().asChar()); // Naming the file...
	}
	else
	{
		wsprintf(szName, "Chr\\%s.N3Joint", mJoint.name().asChar()); // Naming the file...
	}
	
	pJoint->m_szName = mJoint.name().asChar(); // Naming.. Do not build with FullName..

	// Get the rotation axis value.
	if(pJoint->m_KeyOrient.Count() <= 0) // If there is no Joint Orient Key value... Twist the Rotation Key value as much as Orient.
	{
		double dAxis[3];
		MStatus stat = MStatus::kSuccess;
		stat = mJoint.getOrientation(dAxis, rotOrder); // local axis...

		// If there is an Origin Rotation value...
		if(dAxis[0] || dAxis[1] || dAxis[2]) // If you have Origin Rotation...
		{
			__Matrix44 mtxRot;
			mtxRot.Rotation((float)-dAxis[0], (float)-dAxis[1], (float)dAxis[2]);
			__Quaternion qRot = mtxRot;
			this->ProcessJointTransform(pJoint, NULL, &qRot, NULL, false);
		}
	}
	
	// Child joint treatment..
	int nCC = mJoint.childCount();
	for(int i = 0; i < nCC; i++)
	{
		MObject mObj = mJoint.child(i);
		if(mObj.hasFn(MFn::kJoint)) // In case of joint...
		{
			MFnIkJoint mJointChild(mObj);
			CN3Joint* pJointChild = this->ProcessJoint(mJointChild);
			pJoint->ChildAdd(pJointChild);
		}
		else if(mObj.hasFn(MFn::kTransform)) // In the case of a normal Transform, see if the child is a mesh.
		{
			int ttt = 0;
		}
	}

	return pJoint;
}

void CN3E2Wrapper::ProcessJointTransform(CN3Joint *pJoint, __Vector3* pvTrans, __Quaternion* pqRot, __Vector3* pvScale, bool bProcessChild)
{
	if(NULL == pJoint) return;

	bool bRoot = true;
	if(pJoint->Parent()) bRoot = false;

	if(pvScale)
	{
		pJoint->PosSet(pJoint->Pos() * (*pvScale)); // Adjust the trans key according to the scale.
		int nK = pJoint->m_KeyPos.Count(); // It also changes the animation key.
		for(int i = 0; i < nK; i++)
		{
			__Vector3* pvKey = (__Vector3*)(pJoint->m_KeyPos.DataGet(i));
			(*pvKey) *= (*pvScale);
		}
	}

	if(pvTrans) // Apply movement value
	{
		pJoint->PosSet(pJoint->Pos() + (*pvTrans));
		int nK = pJoint->m_KeyPos.Count(); // It also changes the animation key.
		for(int i = 0; i < nK; i++)
		{
			__Vector3* pvKey = (__Vector3*)(pJoint->m_KeyPos.DataGet(i));
			(*pvKey) += (*pvTrans);
		}
	}
	
	if(pqRot) // Change the rotation value.
	{
		__Quaternion qRot2 = pJoint->Rot() * (*pqRot);
		pJoint->RotSet(qRot2);

		int nK = pJoint->m_KeyRot.Count(); // It also changes the rotation value key.
		for(int i = 0; i < nK; i++)
		{
			__Quaternion* pqRot2 = (__Quaternion*)pJoint->m_KeyRot.DataGet(i);
			(*pqRot2) *= (*pqRot);
		}
	}
	
	if(bProcessChild && pvScale) // subnode processing ?? 
	{
		int nCC = pJoint->ChildCount();
		for(int i = 0; i < nCC; i++) 
			this->ProcessJointTransform((CN3Joint*)pJoint->Child(i), NULL, NULL, pvScale, true); // Sub-nodes handle scale only..
	}
}

void CN3E2Wrapper::ProcessMatrix(MFnTransform &mTransform, __Matrix44& mtx, __Vector3& vPos, __Quaternion& qtRot, __Vector3& vScale)
{
	// matrix transformation..
	static double dRs[4], dSs[3];
//	MTransformationMatrix::RotationOrder RotOrder = MTransformationMatrix::kXYZ;
//	mTransform.getRotation(dRs, RotOrder, MSpace::kTransform); // rotation
	MVector mVecPos;
	mVecPos = mTransform.translation(MSpace::kTransform);
	mVecPos *= 0.01; // 1 in 100. Set in metric coordinates...
	mVecPos.z = -mVecPos.z;

	mTransform.getScale(dSs); // enlargement

	vPos.Set((float)mVecPos.x, (float)mVecPos.y, (float)mVecPos.z);
//	vRot.Set((float)-dRs[0], (float)-dRs[1], (float)dRs[2]);
	vScale.Set((float)dSs[0], (float)dSs[1], (float)dSs[2]);

	mTransform.getRotationQuaternion(dRs[0], dRs[1], dRs[2], dRs[3]); // rotation quaternion
	qtRot.x = (float)dRs[0];
	qtRot.y = (float)dRs[1];
	qtRot.z = (float)dRs[2];
	qtRot.w = (float)dRs[3];

	__Vector3 vAxis; float fAngle;
	qtRot.AxisAngle(vAxis, fAngle);
	vAxis.x *= -1; // x-axis rotation is reversed..
	vAxis.y *= -1; // y-axis rotation is reversed..
	vAxis.Normalize();
	qtRot.RotationAxis(vAxis, fAngle);

	// final matrix calculation..
	mtx.Identity();
	static __Matrix44 mtxTmp;
//	mtxTmp.Rotation((float)-dRs[0], (float)-dRs[1], (float)dRs[2]);	mtx *= mtxTmp;
	mtxTmp = qtRot; mtx *= mtxTmp;
	mtxTmp.Scale((float)dSs[0], (float)dSs[1], (float)dSs[2]);		mtx *= mtxTmp;
	mtx.PosSet((float)mVecPos.x, (float)mVecPos.y, (float)mVecPos.z); // location..
}

bool CN3E2Wrapper::FindAnimCurve(MObject &mObj, MObjectArray &mAnimCurveArray)
{
	MItDependencyGraph::Direction Direction = MItDependencyGraph::kUpstream;
//	MItDependencyGraph::Traversal TraversalType = MItDependencyGraph::kBreadthFirst;
//	MItDependencyGraph::Level Level = MItDependencyGraph::kNodeLevel;
	MItDependencyGraph::Traversal TraversalType = MItDependencyGraph::kDepthFirst;
	MItDependencyGraph::Level Level = MItDependencyGraph::kPlugLevel;
	MFn::Type Filter = MFn::kAnimCurve;
	
	MStatus stat = MS::kSuccess;
	MItDependencyGraph dgIter(mObj, Filter, Direction, TraversalType, Level, &stat);

	bool bFind = false;
	for (int i = 0; !dgIter.isDone(); dgIter.next(), i++)
	{
		bFind = true;
		mAnimCurveArray.append(dgIter.thisNode(&stat));
	}

	return bFind;
}

bool CN3E2Wrapper::HaveJoint(MFnMesh &mMesh)
{
	MFnTransform mTransform(mMesh.parent(0));
	int nCC = mTransform.childCount();
	int nMC = 0;
	for(int i = 0; i < nCC; i++)
	{
		if(mTransform.child(i).apiType() == MFn::kMesh) nMC++;
	}

	if(nMC >= 2) // return true; // greater than two.. 
	{
		MItDependencyGraph::Direction Direction = MItDependencyGraph::kUpstream;
		MItDependencyGraph::Traversal TraversalType = MItDependencyGraph::kBreadthFirst;
		MItDependencyGraph::Level Level = MItDependencyGraph::kNodeLevel;
		MFn::Type Filter = MFn::kJoint;

		MItDependencyGraph dgIter(mMesh.object(), Filter, Direction, TraversalType, Level);

		for (int i = 0; !dgIter.isDone(); dgIter.next(), i++)
		{
			MObject mObjTmp = dgIter.thisNode();
			if(mObjTmp.apiType() == MFn::kJoint)
				return true; // if there is a joint..
		}
	}
	return false;
}

// Texture index calculation..
CN3Texture* CN3E2Wrapper::ProcessTexture(MFnMesh &mMesh)
{
	int nLI = 0;
	DWORD dwRWC = 0;
	char szInfo[1024] = "";

	MStringArray szResult;
	MFnLambertShader mShader(this->MeshGetShader(mMesh));
	MString szCmd = MString("listConnections ") + mShader.name() + MString(".color;");
	MGlobal::executeCommand( szCmd, szResult );

	MString szTexture = szResult[0];
	MString szSurface = mMesh.name();

	if(szTexture.length() <= 0) return NULL;

	// Get your size...
	MString szCommand = MString( "getAttr " ) + szTexture + MString( ".outSize" );
	MIntArray nWH;
	if(MGlobal::executeCommand(szCommand, nWH) != MS::kSuccess)
	{
		wsprintf(szInfo, "Texture file processing error : Surface - %s, Texture - %s", szSurface.asChar(), szTexture.asChar());
		nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

		return NULL;
	}

//	int nW = nWH[1], nH = nWH[0]; // Find out your size...
//	if(nW < 4 || nH < 4)
//	{
//		wsprintf(szInfo, "Texture file processing error: width, height too small. Surface - %s, Texture - %s", szSurface.asChar(), szTexture.asChar());
//		nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
//		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
//
//		return NULL;
//	}

	// Convert texture..
	MString szFile;
	MStringArray szResult2;
	szCommand = MString("getAttr ") + szTexture + MString(".fileTextureName");
	MGlobal::executeCommand(szCommand, szResult2);
	szFile = szResult2[0];

	////////////////////////////////////////////
	// See if it is registered in the Texture List.
	char szFNSrc[1024]; // Name the file and...
	lstrcpy(szFNSrc, szFile.asChar());
	int nFN = lstrlen(szFNSrc);
	for(int i = 0; i < nFN; i++) 
		if(szFNSrc[i] == '/') szFNSrc[i] = '\\';

	// If there is no ":" in the string, the full name must be created unless the first two characters are '\\' or '//'.
	if(!strstr(szFNSrc, ":") && !(szFNSrc[0] == '\\' && szFNSrc[1] == '\\'))
	{
		MString szWorkSpace;
		MGlobal::executeCommand(MString("workspace -fullName"), szWorkSpace);
		lstrcpy(szFNSrc, szWorkSpace.asChar());
		lstrcat(szFNSrc, szFile.asChar());
	}
	// Get the WorkSpace name...

	char szFNDest[1024]; // name to save
	lstrcpy(szFNDest, szFNSrc);
	nFN = lstrlen(szFNDest);
	for(i = nFN - 1; i >= 0; i--) // Create a name to save.
	{
		if(szFNDest[i] == '.') { szFNDest[i+1] = 'D'; szFNDest[i+2] = 'X'; szFNDest[i+3] = 'T'; szFNDest[i+4] = NULL; }
		if(szFNDest[i] == '\\' || szFNDest[i] == '/') 
		{
			lstrcpy(szFNDest, &szFNDest[i+1]);
			break;
		}
	}
	
	CN3Texture* pTex = NULL;

	static char szFNs[1024][256]; // Check for duplicate file names...
	if(m_pScene->s_MngTex.Count() <= 0) memset(szFNs, 0, sizeof(szFNs));
	for(i = 0; i < 1024; i++)
	{
		if(NULL == szFNs[i][0]) break;
		if(lstrcmpi(szFNDest, szFNs[i]) == 0)
		{
			pTex = m_pScene->s_MngTex.Get(szFNDest);
			return pTex;
		}
	}

	lstrcpy(szFNs[i], szFNDest);
	pTex = new CN3Texture();
	if(pTex->LoadFromFile(szFNSrc)) // Read the file...
	{
		pTex->m_szName = szFNDest; // Set the name... - Set the file name.
		pTex->FileNameSet(szFNDest); // Name the file.
		CN3Base::s_MngTex.Add(pTex); // Register to Manager
	}
	else
	{
		wsprintf(szInfo, "Texture file processing error: cannot read file. - %s", szFNSrc);
		nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

		delete pTex;
		return NULL;
	}

	if(m_Option.bGenerateHalfSizeTexture) // Output by reducing the Texture Size by half..
	{
		D3DFORMAT fmt = pTex->PixelFormat();
		int nW2 = pTex->Width() / 2;
		int nH2 = pTex->Height() / 2;
		pTex->Convert(fmt, nW2, nH2);
	}

	if(m_Option.bGenerateCompressedTexture) // Use texture compression
	{
		D3DFORMAT fmt = pTex->PixelFormat(), NewFormat = D3DFMT_UNKNOWN;
		if(D3DFMT_X8R8G8B8 == fmt) NewFormat = D3DFMT_DXT1;
		else if(D3DFMT_A8R8G8B8 == fmt) NewFormat = D3DFMT_DXT3;

		if(D3DFMT_UNKNOWN != NewFormat)
		{
			pTex->Convert(NewFormat);
		}
	}

	return pTex;
/*
	// This part reads the Maya Image directly...
	IFFimageReader Reader;
	if(Reader.open(szFile) == MS::kSuccess)
	{
		int nW2, nH2;
		Reader.getSize(nW2, nH2);
		if(nW != nW2 || nH != nH2)
		{
			wsprintf(szInfo, "Texture file processing error: cannot read file. - %s", szFile.asChar());
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			delete pTex;
			return NULL;
		}

	//	if(Reader.isRGB()) nBPP += 3;
	//	else
	//	{
	//		Reader.close();
	//		DeleteFile(szFile.asChar()); // clear file..
	//		wsprintf(szInfo, "Texture file handling error: GrayScale Textures will be supported in the future. Surface - %s, Texture - %s", szSurface.asChar(), szTexture.asChar());
	//		::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
	//		::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
	//		return false;
	//	}
		
		// Create and fill surfaces.
		LPDIRECT3DSURFACE8 lpSurf;
		g_pEng->s_lpD3DDev->CreateImageSurface(nW, nH, D3DFMT_A8R8G8B8, &lpSurf);

		if(NULL == lpSurf)
		{
			Reader.close();
			wsprintf(szInfo, "Texture file processing error: Failed to create Direct3D Texture(%d, %d)", nW, nH);
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			return NULL;
		}

		Reader.readImage(); // read image..
		int nBPP = 0;
		nBPP = Reader.getBytesPerChannel() * 4;
		BOOL bAlpha = Reader.hasAlpha();
		const unsigned char * pBuffer = Reader.getPixelMap();

		D3DLOCKED_RECT LR;
		lpSurf->LockRect(&LR, NULL, 0);
		unsigned char *pDest = NULL, *pSrc = NULL;
		for(int y = 0; y < nH; y++)
		{
			pSrc = (unsigned char*)pBuffer + (nH - y - 1) * nW * nBPP; // stored upside down..
			pDest = (unsigned char*)LR.pBits + y * LR.Pitch;
			for(int x = 0; x < nW; x++, pDest += 4, pSrc += nBPP)
			{
				*(pDest+3) = *(pSrc+3); // Alpha
				*(pDest+2) = *(pSrc+2); // Red
				*(pDest+1) = *(pSrc+1); // Green
				*(pDest+0) = *(pSrc+0); // Blue
			}
		}
		lpSurf->UnlockRect();
		Reader.close();

		if(bAlpha) pTex->CreateFromSurface(lpSurf, D3DFMT_DXT3, TRUE);
//		if(bAlpha) pTex->CreateFromSurface(lpSurf, D3DFMT_DXT4, TRUE);
		else pTex->CreateFromSurface(lpSurf, D3DFMT_DXT1, TRUE);
		
		lpSurf->Release(); lpSurf = NULL;

		if(pTex->Get() == NULL)
		{
			wsprintf(szInfo, "### !!! Texture Export failure(%.3d) : w,h,w2,h2(%.4d, %.4d, %.4d, %.4d) FileName : \"%s\" TextureName \"%s\" MeshName - \"%s\"", 
				m_pScene->s_MngTex.Count(), pTex->Width(), pTex->Height(), nW, nH, szFile.asChar(), pTex->m_szName.c_str(), mMesh.name().asChar());
			nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
			::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog

			delete pTex;
			return NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Output the texture to the Output Window..
	wsprintf(szInfo, "### Texture Export (%.3d) : w,h,w2,h2(%.4d, %.4d, %.4d, %.4d) FileName : \"%s\" TextureName \"%s\" MeshName - \"%s\"", 
		m_pScene->s_MngTex.Count(), pTex->Width(), pTex->Height(), nW, nH, szFile.asChar(), pTex->m_szName.c_str(), mMesh.name().asChar());
	nLI = ::SendMessage(m_hWndLB, LB_ADDSTRING, 0, (LPARAM)szInfo); // Progress dialog
	::SendMessage(m_hWndLB, LB_SETCURSEL, (WPARAM)nLI,0); // Progress dialog
*/
}

int CN3E2Wrapper::ProcessMaterial(MObject mShaderObj, __Material *pMtl)
{
	if(NULL == pMtl) return -1;

	pMtl->Init();

	MColor DC, AC, SC, TC, EC(0.0, 0.0, 0.0, 0.0);
	AC = SC = TC = EC;
	DC.r = 1.0; DC.g = 1.0; DC.b = 1.0; DC.a = 1.0;

	if(mShaderObj.apiType() == MFn::kLambert)
	{
		MFnLambertShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
//		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else if(mShaderObj.apiType() == MFn::kReflect)
	{
		MFnReflectShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else if(mShaderObj.apiType() == MFn::kBlinn)
	{
		MFnBlinnShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else if(mShaderObj.apiType() == MFn::kPhong)
	{
		MFnPhongShader mShader(mShaderObj);
		DC = mShader.color();
		AC = mShader.ambientColor();
		SC = mShader.specularColor();
		TC = mShader.transparency();
		EC = mShader.incandescence();
	}
	else
	{
	}

	pMtl->Diffuse.r = DC.r;
	pMtl->Diffuse.g = DC.g;
	pMtl->Diffuse.b = DC.b;
	pMtl->Diffuse.a = ((1.0f - TC.r) + (1.0f - TC.g) + (1.0f - TC.b))/3.0f; // transparency value..

	pMtl->Ambient.r = DC.r * 0.7f;
	pMtl->Ambient.g = DC.g * 0.7f;
	pMtl->Ambient.b = DC.b * 0.7f;
	pMtl->Ambient.a = pMtl->Diffuse.a;

//	pMtl->Specular.r = SC.r;
//	pMtl->Specular.g = SC.g;
//	pMtl->Specular.b = SC.b;
//	pMtl->Specular.a = 1.0f;

//	pMtl->Emissive.r = EC.r;
//	pMtl->Emissive.g = EC.g;
//	pMtl->Emissive.b = EC.b;
//	pMtl->Emissive.a = 1.0f;

//	pMtl->Power = 0.0f;

	return 0;
}

int CN3E2Wrapper::ProcessTransform(MFnTransform &mTransform, CN3Transform *pTransform)
{
	int nLI = 0;
	char szInfo[1024] = "";

	if(NULL == pTransform) return -1;
	pTransform->m_szName = mTransform.name().asChar();  // Naming...

	__Vector3 vPos, vScale;
	__Quaternion qtRot;
	this->ProcessMatrix(mTransform, pTransform->m_Matrix, vPos, qtRot, vScale); // matrix processing...

	if(pTransform->Type() & OBJ_CAMERA) // In case of camera
	{
		CN3Camera* pCamera = (CN3Camera*)pTransform;
		
		pCamera->EyePosSet(vPos);
		__Matrix44 mtxR = qtRot;
		__Vector3 vAt(0,0,50); // As far as distance...
		vAt *= mtxR; 
		pCamera->AtPosSet(vAt);
		pCamera->UpVectorSet(0,1,0);
	}
	else
	{
		pTransform->PosSet(vPos);
		pTransform->RotSet(qtRot); // Rotation goes into quaternions.
		pTransform->ScaleSet(vScale);
	}

	if(FALSE == m_Option.bAnimationKey) return -1; // Only when the Animation Key processing option is turned on.

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// animated movie..
	MObjectArray mAKs;
	this->FindAnimCurve(mTransform.object(), mAKs);
//	MStatus stat2 = mTransform.setRotationOrder(MTransformationMatrix::kXYZ, true);

	MFnAnimCurve mACs[3][3]; // Position, Rotation, Scale...
	MFnAnimCurve mACJointOrients[3]; // Joint Orient

	int nTranslateCount = 0;

	int nAK = mAKs.length();
	int nKType = -1, nT = 0, nR = 0, nS = 0;
	char szTmp[512];
	for(int i = 0; i < nAK; i++)
	{
		MFnAnimCurve mAC(mAKs[i]);

		lstrcpy(szTmp, mAC.name().asChar()); // Just compare them by name...
		CharLower(szTmp);

		if(NULL != strstr(szTmp, "translatex")) // In the case of a camera, two translate values are entered.
		{
			 nTranslateCount++; 
		}

		nKType = -1;
		if(NULL != strstr(szTmp, "translatex")) { if(nTranslateCount == 2) nKType = 3; else nKType = 0; }
		else if(NULL != strstr(szTmp, "translatey")) { if(nTranslateCount == 2) nKType = 4; else nKType = 1; }
		else if(NULL != strstr(szTmp, "translatez")) { if(nTranslateCount == 2) nKType = 5; else nKType = 2; }
		else if(NULL != strstr(szTmp, "rotatex")) nKType = 3;
		else if(NULL != strstr(szTmp, "rotatey")) nKType = 4;
		else if(NULL != strstr(szTmp, "rotatez")) nKType = 5;
		else if(NULL != strstr(szTmp, "scalex")) nKType = 6;
		else if(NULL != strstr(szTmp, "scaley")) nKType = 7;
		else if(NULL != strstr(szTmp, "scalez")) nKType = 8;
		else if(NULL != strstr(szTmp, "jointorientx")) nKType = 9;
		else if(NULL != strstr(szTmp, "jointorienty")) nKType = 10;
		else if(NULL != strstr(szTmp, "jointorientz")) nKType = 11;
		else if(NULL != strstr(szTmp, "visibility")) nKType = -1;
		else // If you can't judge by name.. judge by ID...??
		{
//			MTypeId id = mAC.typeId();
//			DWORD dwID = id.id();
//			if(dwID == 0x5043544c) { nKType = 0 + nT; nT++; } // Translate
//			else if(dwID == 0x50435441) { nKType = 3 + nR; nR++; } // Rotate
//			else if(dwID == 0x50435455) { nKType = 6 + nS; nS++; } // Scale
//			else nKType = -1;
		
			lstrcpy(szTmp, mAC.typeName().asChar());
			if(NULL != strstr(szTmp, "animCurveTL")) { nKType = 0 + nT; nT++; } // Translate
			else if(NULL != strstr(szTmp, "animCurveTA")) { nKType = 3 + nR; nR++; } // Rotate
			else if(NULL != strstr(szTmp, "animCurveTU")) { nKType = 6 + nS; nS++; } // Scale
			else nKType = -1;
		}

		CN3AnimKey* pKey = NULL;
		if(nKType == 0) { mACs[0][0].setObject(mAC.object()); }
		else if(nKType == 1) { mACs[0][1].setObject(mAC.object()); }
		else if(nKType == 2) { mACs[0][2].setObject(mAC.object()); }
		else if(nKType == 3) { mACs[1][0].setObject(mAC.object()); }
		else if(nKType == 4) { mACs[1][1].setObject(mAC.object()); }
		else if(nKType == 5) { mACs[1][2].setObject(mAC.object()); }
		else if(nKType == 6) { mACs[2][0].setObject(mAC.object()); }
		else if(nKType == 7) { mACs[2][1].setObject(mAC.object()); }
		else if(nKType == 8) { mACs[2][2].setObject(mAC.object()); }

		// Joint Orient Key value..
		else if(nKType == 9) { mACJointOrients[0].setObject(mAC.object()); }
		else if(nKType == 10) { mACJointOrients[1].setObject(mAC.object()); }
		else if(nKType == 11) { mACJointOrients[2].setObject(mAC.object()); }

		else continue;
	}

	for(i = 0; i < 3; i++)
	{
		int nKs[3];
		nKs[0] = mACs[i][0].numKeys();
		nKs[1] = mACs[i][1].numKeys();
		nKs[2] = mACs[i][2].numKeys();
		if(nKs[0] > 0 || nKs[1] > 0 || nKs[2] > 0)
		{
			if(nKs[0] <= 0 || nKs[1] <= 0 || nKs[2] <= 0)
			{
				wsprintf(szInfo, "Transform %s : All Animation Key must have at least over 1 Key value.", mTransform.name().asChar());
				MessageBox(::GetActiveWindow(), szInfo, "Invalid Animation Key", MB_OK);
			}
			else
			{
//				if(i == 0) this->ProcessAnimKey(mACs[i], &pTransform->m_KeyPos, true, 0.01f, false); // Process Translation Animation Key..
				if(i == 0) this->ProcessAnimKey(mACs[i], &pTransform->m_KeyPos, true, 0.01f, false); // Process Translation Animation Key..
				else if(i == 1)
				{
					if(nTranslateCount == 2) // View Translate ... 요건 카메라일대 타겟처럼 처리한다.
					{
						this->ProcessAnimKey(mACs[i], &pTransform->m_KeyRot, true, 0.01f, false); // Process ViewTarget Position Key.
					}
					else
					{
						MTransformationMatrix::RotationOrder rotOrder = mTransform.rotationOrder(); // Depending on the rotation order, the processing should be different.
						this->ProcessAnimKey(mACs[i], &pTransform->m_KeyRot, true, 1.0f, true, rotOrder); // Rotation Animation Key 를 쿼터니언으로 처리한다.. 

//						if(pTransform->Type() & OBJ_JOINT) // If joint...
//						{
						// Make it between -180 degrees and 180 degrees..
//						int nKC = pTransform->m_KeyRot.Count();
//						for(int i = 0; i < nKC; i++)
//						{
//							__Vector3* pV = pTransform->m_KeyRot.DataGet(i);
//							if(pV->x > D3DX_PI) { while(pV->x > D3DX_PI) pV->x -= 3.1415192654f * 2.0f; }
//							if(pV->y > D3DX_PI) { while(pV->y > D3DX_PI) pV->y -= 3.1415192654f * 2.0f; }
//							if(pV->z > D3DX_PI) { while(pV->z > D3DX_PI) pV->z -= 3.1415192654f * 2.0f; }
//
//							if(pV->x < -D3DX_PI) { while(pV->x < -D3DX_PI) pV->x += 3.1415192654f * 2.0f; }
//							if(pV->y < -D3DX_PI) { while(pV->y < -D3DX_PI) pV->y += 3.1415192654f * 2.0f; }
//							if(pV->z < -D3DX_PI) { while(pV->z < -D3DX_PI) pV->z += 3.1415192654f * 2.0f; }
//						}
//						}
					} // end of else
				}
				else if(i == 2)
				{
					this->ProcessAnimKey(mACs[i], &pTransform->m_KeyScale, false, 1.0f, false, MTransformationMatrix::kXYZ); // Process Scale Animation Key..
				} // end of if(i == 0)
			} // end of if(nKs[0] <= 0 || nKs[1] <= 0 || nKs[2] <= 0)
		} // end of if(nKs[0] > 0 || nKs[1] > 0 || nKs[2] > 0)
	} // end of for(i = 0; i < 3; i++)

	if(pTransform->Type() & OBJ_JOINT) // If it is a Joint... Also look at the Orient key value.
	{
		CN3Joint* pJoint = (CN3Joint*)pTransform;
		this->ProcessAnimKey(mACJointOrients, &(pJoint->m_KeyOrient), true, 1.0f, true, MTransformationMatrix::kXYZ);
	}
	// animated movie..
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return 0;
}

void CN3E2Wrapper::ProcessAnimKey(	MFnAnimCurve* pmACs,
									CN3AnimKey *pKey,
									bool bReverseZ,
									float fScale,
									bool bQuaternion,
									MTransformationMatrix::RotationOrder mRotOrder)
{
	// Find the longest one.
	double dfFrmMax = 0.0f, dfFrmTmp = 0.0f;
	int nKC = 0;
	for(int i = 0; i < 3; i++)
	{
		nKC = pmACs[i].numKeys();
		dfFrmTmp = pmACs[i].time(nKC - 1).value();
		if(dfFrmTmp > dfFrmMax) dfFrmMax = dfFrmTmp;
	}

	if(dfFrmMax <= 1) return;
	dfFrmMax = m_pScene->m_fFrmEnd - m_pScene->m_fFrmStart; // Targets the entire scene.

	int nFrmMax = (int)(dfFrmMax * m_Option.fSamplingRate / 30.0f); // 30 Frame Per Sec Sampling
	
	if(bQuaternion) pKey->Alloc(nFrmMax, m_Option.fSamplingRate, KEY_QUATERNION); // If it's a rotation, assign it as a quaternion.
	else pKey->Alloc(nFrmMax, m_Option.fSamplingRate, KEY_VECTOR3); // Assign to vector..

	MTime mTime; mTime.setUnit(MTime::kNTSCFrame);
	if(bQuaternion)
	{
		double dRs[4];
		__Quaternion* pQt;
//		MMatrix mMtx;
//		MTransformationMatrix mTMtx;
		__Matrix44 mtxRotFinal, mtxRots[3];
		int nRotSeqs[3] = {1,0,2}; // Basic YXZ turns..

		// rotation order
		if(MTransformationMatrix::kXYZ == mRotOrder) { nRotSeqs[0] = 0; nRotSeqs[1] = 1; nRotSeqs[2] = 2; }
		else if(MTransformationMatrix::kYZX == mRotOrder) { nRotSeqs[0] = 1; nRotSeqs[1] = 2; nRotSeqs[2] = 0; }
		else if(MTransformationMatrix::kZXY == mRotOrder) { nRotSeqs[0] = 2; nRotSeqs[1] = 0; nRotSeqs[2] = 1; }
		else if(MTransformationMatrix::kXZY == mRotOrder) { nRotSeqs[0] = 0; nRotSeqs[1] = 2; nRotSeqs[2] = 1; }
		else if(MTransformationMatrix::kYXZ == mRotOrder) { nRotSeqs[0] = 1; nRotSeqs[1] = 0; nRotSeqs[2] = 2; }
		else if(MTransformationMatrix::kZYX == mRotOrder) { nRotSeqs[0] = 2; nRotSeqs[1] = 1; nRotSeqs[2] = 0; }
		else MessageBox(::GetActiveWindow(), "Not supported rotation order", "Warning - animation key", MB_OK);

		for(i = 0; i < nFrmMax; i++)
		{
			pQt = (__Quaternion*)(pKey->DataGet(i));

			double dfFrm = i * 30.0f / m_Option.fSamplingRate; // Sampling at 60 Frames Per Sec
			mTime.setValue(dfFrm);

			memset(dRs, 0, sizeof(double) * 4);

			pmACs[0].evaluate(mTime, dRs[0]);
			pmACs[1].evaluate(mTime, dRs[1]);
			pmACs[2].evaluate(mTime, dRs[2]);

			// Adjust between -180 and 180 degrees.
			if(dRs[0] < -D3DX_PI)
				dRs[0] -= __PI2 * (int)(dRs[0]/__PI2 - 1);
			if(dRs[0] > D3DX_PI)
				dRs[0] -= __PI2 * (int)(dRs[0]/__PI2 + 1);
			
			if(dRs[1] < -D3DX_PI)
				dRs[1] -= __PI2 * (int)(dRs[1]/__PI2 - 1);
			if(dRs[1] > D3DX_PI)
				dRs[1] -= __PI2 * (int)(dRs[1]/__PI2 + 1);
			
			if(dRs[2] < -D3DX_PI)
				dRs[2] -= __PI2 * (int)(dRs[2]/__PI2 - 1);
			if(dRs[2] > D3DX_PI)
				dRs[2] -= __PI2 * (int)(dRs[2]/__PI2 + 1);

			mtxRots[0].RotationX(-dRs[0]);
			mtxRots[1].RotationY(-dRs[1]);
			mtxRots[2].RotationZ(dRs[2]);

//			mMtx.setToIdentity();
//			mTMtx = mMtx;
//			mTMtx.setRotation(dRs, MTransformationMatrix::kYXZ, MSpace::kTransform);
//			mTMtx.getRotationQuaternion(dRs[0], dRs[1], dRs[2], dRs[3], MSpace::kTransform);
//			pQt->x = (float)dRs[0];
//			pQt->y = (float)dRs[1];
//			pQt->z = (float)dRs[2];
//			pQt->w = -(float)dRs[3];

			mtxRotFinal.Identity();
			mtxRotFinal *= mtxRots[nRotSeqs[0]];
			mtxRotFinal *= mtxRots[nRotSeqs[1]];
			mtxRotFinal *= mtxRots[nRotSeqs[2]];

			*pQt = mtxRotFinal;
		}
	}
	else
	{
		double dfValue;
		__Vector3* pV = NULL;
		for(i = 0; i < nFrmMax; i++)
		{
			pV = (__Vector3*)(pKey->DataGet(i));

			double dfFrm = i * 30.0f / m_Option.fSamplingRate;
			mTime.setValue(dfFrm);

			pmACs[0].evaluate(mTime, dfValue); pV->x = (float)dfValue;
			pmACs[1].evaluate(mTime, dfValue); pV->y = (float)dfValue;
			pmACs[2].evaluate(mTime, dfValue); pV->z = (float)dfValue;

			if(1.0f != fScale) *pV *= fScale; // That's because the units are meters.
			if(TRUE == bReverseZ) pV->z = -(pV->z);// Z-axis is negative.
		}
	}
}

bool CN3E2Wrapper::ProcessChr(MFnSkinCluster &mSkin)
{
	/////////////////////////////////////////////////////////////////////////////////////
	// Find and treat affected joints.
	MStatus stat;
	MDagPathArray PathArray;
	mSkin.influenceObjects(PathArray, &stat);
	int nJC = PathArray.length(); // Note the number of joints.
	MFnIkJoint mJointRoot;
	for(int i = 0; i < nJC; i++)
	{
		mJointRoot.setObject(PathArray[i].node());
		if(mJointRoot.parent(0).apiType() != MFn::kJoint) // Treatment in case of root joint..
			break;
	}
	if(i == nJC) return false; // If there is no root joint...

	// Look for duplicates.
	std::string szJointFN;
	if(mJointRoot.parentCount() > 0 && mJointRoot.parent(0).hasFn(MFn::kTransform))
	{
		MFnTransform mP1(mJointRoot.parent(0));
		szJointFN = ""; this->ProcessName(mP1.parent(0), szJointFN); // Find out the name of the bone.
		szJointFN = "Chr\\" + szJointFN + ".N3Joint";
	}
	else
		szJointFN = mJointRoot.name().asChar(); // Find out the name of the bone.

	static std::string szJointFNs[512]; // Check for duplicates...
	if(m_pScene->s_MngJoint.Count() <= 0) 
	{
		for(i = 0; i < 512; i++) szJointFNs[i] = "";
	}

	bool bOverlapped = false;
	for(i = 0; i < 256; i++)
	{
		if(szJointFNs[i].size() <= 0) break;
		if(szJointFN == szJointFNs[i])
		{
			bOverlapped = true;
			break;
		}
	}
	
	CN3Chr* pChr = NULL;
	CN3Joint* pJoint = NULL;

	if(false == bOverlapped) // If it doesn't overlap, create a new one.
	{
		szJointFNs[i] = szJointFN; // Write down the joint name.

		pChr = new CN3Chr();
		m_pScene->ChrAdd(pChr); // Add a character to the scene..
		pChr->PartAlloc(64); // Sufficient Part Data allocation..

		if(mJointRoot.parentCount() > 0)
		{
			if(mJointRoot.parent(0).hasFn(MFn::kTransform))
			{
				MFnTransform mP1(mJointRoot.parent(0));
				if(mP1.parentCount() > 0) this->ProcessName(mP1.parent(0), pChr->m_szName); // Naming...
				else this->ProcessName(mP1.parent(0), pChr->m_szName); // Naming...
				pChr->FileNameSet("Chr\\" + pChr->m_szName + ".N3Chr"); // Determine the file name...
			}
			else
			{
				this->ProcessName(mJointRoot.parent(0), pChr->m_szName); // Naming...
				pChr->FileNameSet("Chr\\" + pChr->m_szName + ".N3Chr"); // Determine the file name..);
			}
		}
		else 
		{
			pChr->m_szName = mJointRoot.name().asChar();
			std::string szFN = "Chr\\"; szFN += mJointRoot.name().asChar(); szFN += ".N3Chr"; // Determine the file name...
			pChr->FileNameSet(szFN);
		}

		pJoint = this->ProcessJoint(mJointRoot); // Joint processing
		if(NULL == pJoint) 
		{
			delete pChr;
			return NULL;
		}
		pJoint->FileNameSet(szJointFN); // Set file name..
		
		pChr->s_MngJoint.Add(pJoint);
		pChr->JointSet(pJoint->FileName()); // Joint Setting;
		
		///////////////////////////////////////////////////////////////////////
		// If Joint is grouped, Parent Transform processing..
		if(mJointRoot.parentCount() > 0 && mJointRoot.parent(0).hasFn(MFn::kTransform))
		{
			MFnTransform mTJ(mJointRoot.parent(0)); // Joint Transform Node
			
			MTransformationMatrix mTMJ = mTJ.transformation();
//			MMatrix mMtxJT; mMtxJT.setToIdentity();
//			this->GetWorldTransform(mTJ, mMtxJT);
//			MTransformationMatrix mTMJ(mMtxJT);

			MVector mVPos;
			double dfScale[3], dfRot[3];
			MTransformationMatrix::RotationOrder RotOrder = MTransformationMatrix::kXYZ;

			mVPos = mTMJ.translation(MSpace::kTransform) * 0.01;
			
			MMatrix mMtxJWorld; mMtxJWorld.setToIdentity();
			this->GetWorldTransform(mTJ, mMtxJWorld);
			mVPos.x = mMtxJWorld.matrix[3][0] * 0.01;
			mVPos.y = mMtxJWorld.matrix[3][1] * 0.01;
			mVPos.z = mMtxJWorld.matrix[3][2] * 0.01;
			
			MTransformationMatrix mTMtxJWorld(mMtxJWorld);
			mTMtxJWorld.getScale(dfScale, MSpace::kTransform);
			mTMtxJWorld.getRotation(dfRot, RotOrder, MSpace::kTransform);

			__Vector3 vTrans((float)mVPos.x, (float)mVPos.y, -(float)mVPos.z);
			__Vector3 vScale((float)dfScale[0], (float)dfScale[1], (float)dfScale[2]);
			__Vector3 vRot((float)-dfRot[0], (float)-dfRot[1], (float)dfRot[2]);
			
			__Matrix44 mtxRotG; mtxRotG.Rotation(vRot);
			__Quaternion qRot = mtxRotG;
			vTrans *= mtxRotG;
			this->ProcessJointTransform(pJoint, &vTrans, &qRot, &vScale, true);

			pJoint->PosSet(pJoint->Pos() * mtxRotG); // The position value is also adjusted according to the direction and scale
			int nK = pJoint->m_KeyPos.Count(); // It also changes the animation key.
			for(i = 0; i < nK; i++)
			{
				__Vector3* pvKey = (__Vector3*)(pJoint->m_KeyPos.DataGet(i));
				(*pvKey) *= mtxRotG;
			}
		}
	}
	else
	{
		pJoint = m_pScene->s_MngJoint.Get(szJointFN);

		int nChrCount = m_pScene->ChrCount(); // Find a character pointer with the same joint pointer.
		for(int i = 0; i < nChrCount; i++)
		{
			CN3Chr* pChrTmp = m_pScene->ChrGet(i);
			if(pChrTmp->Joint() == pJoint)
			{
				pChr = pChrTmp;
				break;
			}
		}
	}

	if(NULL == pChr) return false;

	/////////////////////////////////////////////////// ............ // Mesh
	// Original Mesh
	MObjectArray mMeshArray;
	mSkin.getInputGeometry(mMeshArray);
	__ASSERT(1 == mMeshArray.length(), "binding mesh count is not 1");
	MFnMesh mMeshOrg(mMeshArray[0]); // Original Mesh

	mSkin.getOutputGeometry(mMeshArray); 
	__ASSERT(1 == mMeshArray.length(), "Output mesh count is not 1");
	MFnMesh mMeshOutput(mMeshArray[0]); // Output Mesh

	bool bCollisionMesh = false;

	MFnTransform mMT(mMeshOrg.parent(0)); // Mesh Transform
	MFnTransform mGT;// Mesh GroupTransform
	if(mMT.parentCount() > 0 && mMT.parent(0).hasFn(MFn::kTransform))
		mGT.setObject(mMT.parent(0));
	else
		mGT.setObject(mMT.object());

	// See if it is a mesh that will be used for collision checking.
	std::string szFNM = mMT.name().asChar();
	if(szFNM.size() > 0) CharLower(&(szFNM[0]));
	if(szFNM.find("coll") != -1) bCollisionMesh = true;

	if(true == bCollisionMesh) // 충돌 체크 메시면.... 
	{
/*		CN3IMesh* pN3Mesh = this->ProcessIMesh(mMeshOrg); // Indexed mesh processing.
		int nVC = pN3Mesh->VertexCount();
		int nFC = pN3Mesh->FaceCount();
		__VertexXyzNormal* pVSrc = pN3Mesh->Vertices();
		WORD* pwISrc = pN3Mesh->VertexInices();

		
		CN3VMesh* pVMesh = new CN3VMesh();
		pVMesh->CreateVertices(nVC);
		pVMesh->CreateIndex(nFC * 3);
		__Vector3* pVDest = pVMesh->Vertices();
		WORD* pwIDest = pVMesh->Indices();

		memcpy(pwIDest, pwISrc, nFC * 3 * 2);
		for(int i = 0; i < nVC; i++) pVDest[i] = pVSrc[i].v;

		delete pN3Mesh;

		pVMesh->m_szName = "";
		this->ProcessName(mMeshOrg.object(), pVMesh);
		std::string szFN = "Chr\\" + pVMesh->m_szName + ".N3VMesh"; // Determine the file name...
		pVMesh->FileNameSet(szFN);

		pChr->s_MngVMesh.Add(pVMesh);
		pChr->CollisionMeshSet(pVMesh->m_szName);

		this->ProcessSkin(mSkin, pChr->CollisionSkin()); // Skin treatment.
*/
	}
	else if(false == bCollisionMesh) // Collision check mesh or not....Proceed normally..
	{
		int nLOD = 0;
		int nPart = 0;

		if(mGT.parentCount() > 0 && mGT.parent(0).hasFn(MFn::kTransform)) // If there is a parent node of the group transform...
		{
			MFnTransform mCT(mGT.parent(0)); // Character Transform
			int nC = mCT.childCount();
			for(i = 0; i < nC; i++)
			{
				if(!mCT.child(i).hasFn(MFn::kTransform)) continue;

				MFnTransform mTTmp(mCT.child(i));
				if(mTTmp.object() == mGT.object()) break; // If the name of the child under the character transform and the name of the group are the same...
				nPart++; // If not equal, increase Part.
			}

			int nGTC = mGT.childCount();
			for(i = 0; i < nGTC; i++)
			{
				if(!mGT.child(i).hasFn(MFn::kTransform)) continue; 

				MFnTransform mTTmp(mGT.child(i));
				if(mTTmp.name() == mMT.name()) break; // If the Mesh Transform and the child mesh in the Mesh group have the same name...
				nLOD++;
			}
		}
		else // pair of if(mGT.parentCount() > 0 && mGT.parent(0).apiType() == MFn::kTransform) // If there is a parent node of the group transform...
		{
			int nC = mGT.childCount();
			for(i = 0; i < nC; i++)
			{
				if(mGT.child(i).apiType() != MFn::kTransform) continue;

				MFnTransform mTTmp(mGT.child(i));
				if(mTTmp.name() == mMT.name()) break; // If the name of the child under the character transform and the name of the group are the same...
				nPart++; // If not equal, increase Part.
			}
		}
		
		__ASSERT(nPart >= 0 && nPart < 64, "Part count is bigger than 64");
		__ASSERT(nLOD >= 0 && nLOD < MAX_CHR_LOD, "LOD Count is bigger than MAX_CHR_LOD");
		
		CN3CPart* pPart = pChr->Part(nPart);
		CN3Skin* pSkin = pPart->Skin(nLOD);

		if(pPart->m_szName.empty()) // If there is no part name...
		{
			pPart->m_szName = mGT.name().asChar();
			std::string szFN; this->ProcessName(mGT.object(), szFN);

			int iLen = szFN.size();
			int iLen2 = pPart->m_szName.size();
			if(m_Option.bGenerateFileName && iLen >= 11 && iLen2 >= 11) // Item Code ::: 0_2345_78_0
				szFN = szFN.substr(iLen - 11);
			pPart->FileNameSet("item\\" + szFN + ".N3CPart");
		}
		
		if(NULL == pSkin) // If there is no skin, enter it.
		{
			CN3CPartSkins* pSkinsAdd = new CN3CPartSkins();
			pSkinsAdd->m_szName = mGT.name().asChar(); // Set the name and file name.
			std::string szFN; this->ProcessName(mGT.object(), szFN);

			int iLen = szFN.size();
			int iLen2 = pSkinsAdd->m_szName.size();
			if(m_Option.bGenerateFileName && iLen >= 11 && iLen2 >= 11) // Item Code ::: 0_2345_78_0
				szFN = szFN.substr(iLen - 11);
			pSkinsAdd->FileNameSet("item\\" + szFN + ".N3CSkins");

			CN3Base::s_MngSkins.Add(pSkinsAdd);
			pPart->SkinsSet(pSkinsAdd->FileName());
			pSkin = pPart->Skin(nLOD); // Get the pointer again.
		} 

		if(false == this->ProcessSkin(mSkin, pSkin)) // Skin treatment..
		{
			MessageBox(::GetActiveWindow(), mSkin.name().asChar(), "Skin processing failed", MB_OK);
			return false;
		}

		// texture processing...
		pPart->m_Mtl.Init();
		pPart->m_Mtl.dwColorArg1 = D3DTA_DIFFUSE;
		pPart->m_Mtl.dwColorArg2 = D3DTA_TEXTURE;

		CN3Texture* pTex = ProcessTexture(mMeshOutput);
		if(pTex)
		{
			pTex->m_szName = mGT.name().asChar(); // Set the name and file name.
			std::string szFN; this->ProcessName(mGT.object(), szFN);
			int iLen = szFN.size();
			int iLen2 = pTex->m_szName.size();
			if(m_Option.bGenerateFileName && iLen >= 11 && iLen2 >= 11) // Item Code ::: 0_2345_78_0
				szFN = szFN.substr(iLen - 11);
			pTex->FileNameSet("Item\\" + szFN + ".DXT"); // Make the Part name and Texture name the same.
			
			pPart->m_Mtl.dwColorOp = D3DTOP_MODULATE;
			pPart->TexSet(pTex);

//			D3DFORMAT fmtTex = pTex->PixelFormat();
//			if(	fmtTex == D3DFMT_DXT2 ||
//				fmtTex == D3DFMT_DXT3 || 
//				fmtTex == D3DFMT_DXT4 || 
//				fmtTex == D3DFMT_DXT5 ) pPart->m_Mtl.nRenderFlags = RF_ALPHABLENDING;
		}
		else // Material only if there is no texture
		{
//			this->ProcessMaterial(this->MeshGetShader(mMeshOutput), &(pPart->m_Mtl)); 
			pPart->m_Mtl.dwColorOp = D3DTOP_SELECTARG1;
		}
	} // if(false == bCollisionMesh) // Collision check mesh or not....Proceed normally..

	return true;
}

void CN3E2Wrapper::ProcessName(MObject mObj, std::string& szName)
{
	if(mObj.hasFn(MFn::kWorld) == FALSE && mObj.hasFn(MFn::kDependencyNode)) // If it's not the top node...
	{
		MFnDependencyNode mNode(mObj);

		std::string szNameBack = szName;
		szName = mNode.name().asChar();
		if(szNameBack.size() > 0)
		{
			szName += '_';
			szName += szNameBack;
		}

		if(mObj.hasFn(MFn::kDagNode))
		{
			MFnDagNode mDAG(mObj);
			if(mDAG.parentCount() > 0) this->ProcessName(mDAG.parent(0), szName);
		}
	}
}
/*
const DWORD OBJ_UNKNOWN				= 0;
const DWORD OBJ_BASE				= 0x1;
const DWORD OBJ_BASE_HIERARCHY 		= 0x2;
const DWORD OBJ_TRANSFORM 			= 0x4;
const DWORD OBJ_SCENE				= 0x8;
const DWORD OBJ_TEXTURE				= 0x10;

const DWORD OBJ_CAMERA				= 0x20;
const DWORD OBJ_LIGHT				= 0x40;
const DWORD OBJ_SHAPE				= 0x80;
const DWORD OBJ_CHARACTER			= 0x100;

const DWORD OBJ_MESH				= 0x200;
const DWORD OBJ_MESH_PROGRESSIVE	= 0x400;
const DWORD OBJ_MESH_INDEXED		= 0x800;
const DWORD OBJ_MESH_VECTOR3		= 0x1000;
const DWORD OBJ_JOINT				= 0x2000;
const DWORD OBJ_SKIN				= 0x4000;

const DWORD OBJ_DUMMY				= 0x8000;
const DWORD OBJ_EFFECT				= 0x10000;

const DWORD OBJ_ANIM_CONTROL		= 0x20000;
const DWORD OBJ_SUBSCENE			= 0x40000;
*/


void CN3E2Wrapper::GetWorldTransform(MFnTransform &mTransform, MMatrix &mMtx)
{
	int nParent = mTransform.parentCount();
	mMtx *= mTransform.transformationMatrix();
	if(1 == nParent && mTransform.parent(0).hasFn(MFn::kTransform))
	{
		MFnTransform mPT(mTransform.parent(0));
		this->GetWorldTransform(mPT, mMtx);
	}
}

bool CN3E2Wrapper::IsSelected(MSelectionList& mSelList, MObject mObj)
{
	if(mObj.hasFn(MFn::kDependencyNode))
	{
		MFnDependencyNode mDag(mObj);
		const char* szName = mDag.name().asChar();
		int ttttt = 0;
	}

	bool bSel = false;
	int nSelCount = mSelList.length();
	MObject mObjTmp;
	for(int i = 0; i < nSelCount; i++)
	{
		mSelList.getDependNode(i, mObjTmp);
		if(mObjTmp == mObj)
		{
			return true;
		}
	}

	if(mObj.hasFn(MFn::kDagNode))
	{
		MFnDagNode mDNode(mObj);
		if(mDNode.parentCount() > 0)
		{
			return IsSelected(mSelList, mDNode.parent(0));
		}
	}
	else if(mObj.hasFn(MFn::kSkinClusterFilter))
	{
		MFnSkinCluster mSkin(mObj);
		MObjectArray mMeshArray;
		mSkin.getInputGeometry(mMeshArray);
		__ASSERT(1 == mMeshArray.length(), "binding mesh count is not 1");
		return IsSelected(mSelList, mMeshArray[0]); // Original Mesh
	}

	return false;
}


INT_PTR CALLBACK CN3E2Wrapper::DlgProcProgress( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg ) 
    {
        case WM_INITDIALOG:
        {
			break;
        }
        case WM_COMMAND:
        {
			switch( LOWORD(wParam) )
			{
			case IDC_B_CANCEL:
				m_bCancelExport = TRUE;
				break;
			}
			break;
		}
		case WM_CLOSE:
		{
			ShowWindow(hDlg, SW_HIDE);
			break;
		}
    }
    return FALSE; // Didn't handle message
}

BOOL CALLBACK CN3E2Wrapper::DlgProcPane(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HKEY hKey = g_pEng->RegistryOpen("N3Export Option");
			if(hKey) 
			{
				g_pEng->RegistryValueGet(hKey, "Export Camera", &m_Option.bExportCamera, 4);
				g_pEng->RegistryValueGet(hKey, "Export Light", &m_Option.bExportLight, 4);
				g_pEng->RegistryValueGet(hKey, "Export Geometry", &m_Option.bExportGeometry, 4);
				g_pEng->RegistryValueGet(hKey, "Export Dummy", &m_Option.bExportDummy, 4);
				g_pEng->RegistryValueGet(hKey, "Export Character", &m_Option.bExportCharacter, 4);

				g_pEng->RegistryValueGet(hKey, "Animation Key", &m_Option.bAnimationKey, 4);
				g_pEng->RegistryValueGet(hKey, "Generate File Name", &m_Option.bGenerateFileName, 4);
				g_pEng->RegistryValueGet(hKey, "Generate Smooth Normal", &m_Option.bGenerateSmoothNormal, 4);
//				g_pEng->RegistryValueGet(hKey, "Generate Progressive Mesh", &m_Option.bGenerateProgressiveMesh, 4);
				g_pEng->RegistryValueGet(hKey, "Generate Half Size Texture", &m_Option.bGenerateHalfSizeTexture, 4);
				g_pEng->RegistryValueGet(hKey, "Generate Compressed Texture", &m_Option.bGenerateCompressedTexture, 4);
				
				g_pEng->RegistryClose(hKey);
			}

			CheckDlgButton(hDlg, IDC_C_OBJ_CAMERA, m_Option.bExportCamera);
			CheckDlgButton(hDlg, IDC_C_OBJ_LIGHT, m_Option.bExportLight);
			CheckDlgButton(hDlg, IDC_C_OBJ_GEOMETRY, m_Option.bExportGeometry);
			CheckDlgButton(hDlg, IDC_C_OBJ_DUMMY, m_Option.bExportDummy);
			CheckDlgButton(hDlg, IDC_C_OBJ_CHARACTER, m_Option.bExportCharacter);

			CheckDlgButton(hDlg, IDC_C_ANIMATION_KEY, m_Option.bAnimationKey);

			CheckDlgButton(hDlg, IDC_C_GENERATE_FILE_NAME, m_Option.bGenerateFileName);
			CheckDlgButton(hDlg, IDC_C_GENERATE_SMOOTH_NORMAL, m_Option.bGenerateSmoothNormal);
//			CheckDlgButton(hDlg, IDC_C_GENERATE_PROGRESSIVE_MESH, m_Option.bGenerateProgressiveMesh);
			CheckDlgButton(hDlg, IDC_C_GENERATE_HALF_SIZE_TEXTURE, m_Option.bGenerateHalfSizeTexture);
			CheckDlgButton(hDlg, IDC_C_GENERATE_COMPRESSED_TEXTURE, m_Option.bGenerateCompressedTexture);

			m_Option.nFrmStart = (int)(MAnimControl::minTime().value());
			m_Option.nFrmEnd = (int)(MAnimControl::maxTime().value() +0.5);

			SetDlgItemInt(hDlg, IDC_E_FRAME_START, m_Option.nFrmStart, FALSE);
			SetDlgItemInt(hDlg, IDC_E_FRAME_END, m_Option.nFrmEnd, FALSE);
			SetDlgItemInt(hDlg, IDC_E_SAMPLING_RATE, (int)(m_Option.fSamplingRate), FALSE);
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			{
				memset(&m_Option, 0, sizeof(m_Option));

				m_Option.bExportCamera = IsDlgButtonChecked(hDlg, IDC_C_OBJ_CAMERA);
				m_Option.bExportLight = IsDlgButtonChecked(hDlg, IDC_C_OBJ_LIGHT);
				m_Option.bExportGeometry = IsDlgButtonChecked(hDlg, IDC_C_OBJ_GEOMETRY);
				m_Option.bExportDummy = IsDlgButtonChecked(hDlg, IDC_C_OBJ_DUMMY);
				m_Option.bExportCharacter = IsDlgButtonChecked(hDlg, IDC_C_OBJ_CHARACTER);

				m_Option.bAnimationKey = IsDlgButtonChecked(hDlg, IDC_C_ANIMATION_KEY);
				m_Option.nFrmStart = GetDlgItemInt(hDlg, IDC_E_FRAME_START, NULL, FALSE);
				m_Option.nFrmEnd = GetDlgItemInt(hDlg, IDC_E_FRAME_END, NULL, FALSE);
				m_Option.fSamplingRate = (float)GetDlgItemInt(hDlg, IDC_E_SAMPLING_RATE, NULL, FALSE); // Sampling Rate

				m_Option.bGenerateFileName = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_FILE_NAME);
				m_Option.bGenerateSmoothNormal = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_SMOOTH_NORMAL);
//				m_Option.bGenerateProgressiveMesh = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_PROGRESSIVE_MESH);
				m_Option.bGenerateHalfSizeTexture = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_HALF_SIZE_TEXTURE);
				m_Option.bGenerateCompressedTexture = IsDlgButtonChecked(hDlg, IDC_C_GENERATE_COMPRESSED_TEXTURE);

				EndDialog(hDlg, 1);

				// I put the option value in Registry..
				HKEY hKey = g_pEng->RegistryOpen("N3Export Option");
				if(NULL == hKey) RegCreateKey(HKEY_CURRENT_USER, "N3Export Option", &hKey);
				if(hKey) 
				{
					g_pEng->RegistryValueSet(hKey, "Export Camera", &m_Option.bExportCamera, 4);
					g_pEng->RegistryValueSet(hKey, "Export Light", &m_Option.bExportLight, 4);
					g_pEng->RegistryValueSet(hKey, "Export Geometry", &m_Option.bExportGeometry, 4);
					g_pEng->RegistryValueSet(hKey, "Export Dummy", &m_Option.bExportDummy, 4);
					g_pEng->RegistryValueSet(hKey, "Export Character", &m_Option.bExportCharacter, 4);

					g_pEng->RegistryValueSet(hKey, "Animation Key", &m_Option.bAnimationKey, 4);
					g_pEng->RegistryValueSet(hKey, "Generate File Name", &m_Option.bGenerateFileName, 4);
					g_pEng->RegistryValueSet(hKey, "Generate Smooth Normal", &m_Option.bGenerateSmoothNormal, 4);
//					g_pEng->RegistryValueSet(hKey, "Generate Progressive Mesh", &m_Option.bGenerateProgressiveMesh, 4);
					g_pEng->RegistryValueSet(hKey, "Generate Half Size Texture", &m_Option.bGenerateHalfSizeTexture, 4);
					g_pEng->RegistryValueSet(hKey, "Generate Compressed Texture", &m_Option.bGenerateCompressedTexture, 4);
					
					g_pEng->RegistryClose(hKey);
				}
			}
			return TRUE;
		case IDCANCEL:
			{
				EndDialog(hDlg, 0);
				return TRUE;
			}
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	
	return FALSE;
}
