// SwappedDummy.cpp: implementation of the CSwappedDummy class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "N3Indoor.h"
#include "SwappedDummy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSwappedDummy::CSwappedDummy()
{
	m_ceDType = DUMMY_SWAP;
	const float fCubeOffset = 10.0f;
	InitDummyCube(DUMMY_CENTER, &(m_DummyCubes[DUMMY_CENTER]), __Vector3(0,0,0), D3DCOLOR_ARGB(0xff, 0xaa, 0xaa, 0xaa));
	InitDummyCube(DUMMY_X, &(m_DummyCubes[DUMMY_X]), __Vector3(fCubeOffset,0,0), D3DCOLOR_ARGB(0xff, 0xaa, 0xaa, 0xaa));
	InitDummyCube(DUMMY_Y, &(m_DummyCubes[DUMMY_Y]), __Vector3(0,fCubeOffset,0), D3DCOLOR_ARGB(0xff, 0xaa, 0xaa, 0xaa));
	InitDummyCube(DUMMY_Z, &(m_DummyCubes[DUMMY_Z]), __Vector3(0,0,fCubeOffset), D3DCOLOR_ARGB(0xff, 0xaa, 0xaa, 0xaa));
	D3DCOLOR LineColor = D3DCOLOR_ARGB(0xff,0xaa,0xaa,0xaa);
	m_LineVertices[0].Set(0,0,0, LineColor);	m_LineVertices[1].Set(fCubeOffset,0,0, LineColor);
	m_LineVertices[2].Set(0,0,0, LineColor);	m_LineVertices[3].Set(0,fCubeOffset,0, LineColor);
	m_LineVertices[4].Set(0,0,0, LineColor);	m_LineVertices[5].Set(0,0,fCubeOffset, LineColor);

	Release();
}

CSwappedDummy::~CSwappedDummy()
{
	Release();
}

BOOL CSwappedDummy::MouseMsgFilter(LPMSG pMsg)
{
	switch(pMsg->message)
	{
		case WM_LBUTTONDOWN:
			{
				POINT point = {short(LOWORD(pMsg->lParam)), short(HIWORD(pMsg->lParam))};
				m_pSelectedCube = Pick(point.x, point.y);
				if (m_pSelectedCube)
				{
					m_vPrevPos = Pos();
					m_qPrevRot = Rot();

					SetCapture(pMsg->hwnd);
					return TRUE;
				}
			}
			break;

		case WM_MOUSEMOVE:
			{
				POINT point = {short(LOWORD(pMsg->lParam)), short(HIWORD(pMsg->lParam))};
				DWORD nFlags = pMsg->wParam;
				if (m_pSelectedCube && (nFlags & MK_LBUTTON))
				{
					__Vector3 vRayDir, vRayOrig;	// The direction and origin of a straight line connecting the center of the screen (viewpoint) and the mouse pointer
					__Vector3 vPN, vPV;	// Normals of the plane and included points
					__Vector3 vPos;	// The intersection point of the above plane and the straight line (point to find)
					__Vector3 vCameraDir = s_CameraData.vAt - s_CameraData.vEye;	vCameraDir.Normalize();
					GetPickRay(point, vRayDir, vRayOrig);
					vPV = m_vPrevPos;
					__Matrix44 mat = m_Matrix;	mat.PosSet(0,0,0);

					switch(m_pSelectedCube->iType)
					{
					case DUMMY_X:
						{
							vPN.Set(0, vCameraDir.y, vCameraDir.z);		vPN.Normalize();
							float fT = D3DXVec3Dot(&vPN,&(vPV-vRayOrig)) / D3DXVec3Dot(&vPN, &vRayDir);
							vPos = vRayOrig + vRayDir*fT;
							vPos += ((m_pSelectedCube->vCenterPos*(-1.0f))*mat);
							m_vPos.x = vPos.x;
						}
						break;
					case DUMMY_Y:
						{
							vPN.Set(vCameraDir.x, 0, vCameraDir.z);		vPN.Normalize();
							float fT = D3DXVec3Dot(&vPN,&(vPV-vRayOrig)) / D3DXVec3Dot(&vPN, &vRayDir);
							vPos = vRayOrig + vRayDir*fT;
							vPos += ((m_pSelectedCube->vCenterPos*(-1.0f))*mat);
							m_vPos.y = vPos.y;
						}
						break;
					case DUMMY_Z:
						{
							vPN.Set(vCameraDir.x, vCameraDir.y, 0);		vPN.Normalize();
							float fT = D3DXVec3Dot(&vPN,&(vPV-vRayOrig)) / D3DXVec3Dot(&vPN, &vRayDir);
							vPos = vRayOrig + vRayDir*fT;
							vPos += ((m_pSelectedCube->vCenterPos*(-1.0f))*mat);
							m_vPos.z = vPos.z;
						}
						break;
					}
					return TRUE;
				}
			}
			break;
		case WM_LBUTTONUP:
			{
				if (m_pSelectedCube)
				{
					ReleaseCapture();
					m_pSelectedCube = NULL;
					return TRUE;
				}
			}
			break;
		case WM_RBUTTONUP:
			{
				if (m_pSelectedCube)
				{
					ReleaseCapture();
					m_pSelectedCube = NULL;
					return TRUE;
				}
			}
			break;
	}

	return FALSE;
}