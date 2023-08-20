#include "pch.h"
#include "GameEng.h"
#include "GameBase.h"
#include "N3WorldManager.h"

#include "N3Camera.h"
#include "N3Light.h"

const float LIGHTNING_DURATION = 2.0f;

CGameEng::CGameEng()
{
	m_pActiveCam	= nullptr;

	// path where the program was executed.
	char szBuf[_MAX_PATH];
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
	::GetModuleFileName(nullptr, szBuf, _MAX_PATH);
	_splitpath(szBuf, szDrive, szDir, nullptr, nullptr);
	_makepath(szBuf, szDrive, szDir, nullptr, nullptr);

	// 
	// Basic camera settings...
	auto* pCamera			= new CN3Camera();
	pCamera->m_bFogUse			= TRUE;
	pCamera->m_Data.fFOV		= D3DXToRadian(70);
	pCamera->m_Data.fNP			= 0.7f;
	pCamera->m_Data.fFP			= 512.0f;
	this->CameraAdd(pCamera);
	this->SetActiveCamera(pCamera);

	m_vEyeToReach = pCamera->EyePos();
	m_vAtToReach = pCamera->AtPos();

	// m_eViewPoint = VP_BACKWARD; // Defaults to start in 1.5 person (looking from behind the player).
	m_eViewPoint = VP_THIRD_PERSON; // The default is to start in 3rd person.
	m_fRotPitchFirstPerson = 0.0f; // Camera pitch angle when in first person view.
	m_fRotPitchBackward = 0.0f; // Camera pitch angle when in third person view.
	m_fRotPitchFoward = 0.0f; // The camera pitch angle when viewed from the front.
	m_fRotPitchThirdFirson = D3DXToRadian(25.0f);
	m_fOffsetVPGod = 7.0f; // Camera distance when viewed from above.
	m_fRotYawVPGod = 0.0f; // The angle of rotation of the camera when viewed from above.
	m_fZoomBackwardOrFoward = 1.0f; // 1.5 person, camera zoom 1.0f is standard when viewed from the front.
	// Basic camera settings...
	// 

	m_fLightningTimeRemain = 0.0f; // Timer when lightning strikes.

	// 
	// default light settings
	/*
	D3DCOLORVALUE crLgt;

	crLgt.a = 0.0f, crLgt.r = crLgt.g = crLgt.b = 0.8f;
	CN3Light* pLightGlobal = new CN3Light(); // Light to illuminate the whole..
	pLightGlobal->m_Data.InitDirection(0, __Vector3(0,-1,0), crLgt);
	this->LightAdd(pLightGlobal);

	crLgt.a = 0.0f, crLgt.r = crLgt.g = crLgt.b = 0.5f;
	CN3Light* pLightGlobal2 = new CN3Light(); // A light that illuminates the whole from the other side..
	pLightGlobal2->m_Data.InitDirection(1, __Vector3(0,1,0), crLgt);
	this->LightAdd(pLightGlobal2);

	crLgt.a = 0.0f, crLgt.r = crLgt.g = crLgt.b = 0.3f;
	CN3Light* pLight = new CN3Light(); // stick with the camera...
	pLight->m_Data.InitPoint(2, __Vector3(0,0,0), crLgt, 32.0f);
	this->LightAdd(pLight);
	*/
	// default light settings
	// 
	m_pRefLightSun = nullptr;
	m_pRefLightSupport = nullptr;
	m_pRefLightCam = nullptr;

	m_fFPDeltaCur = 1.0f; // today
	m_fFPDeltaToReach = 1.0f; // It changes by targeting this value.

	// m_fLightDeltaCur = 1.0f;
	// m_fLightDeltaToReach = 1.0f; // Target this value and change it.
}

CGameEng::~CGameEng()
{
	auto itCam = m_Cameras.begin();
	const int iSize = m_Cameras.size();
	for(int i = 0; i < iSize; i++, itCam++)
	{
		delete *itCam; 
	}
	m_Cameras.clear();

/*	it_Light itLgt = m_Lights.begin();
	iSize = m_Lights.size();
	for(auto i = 0; i < iSize; i++, itLgt++)
	{
		delete *itLgt; 
	}
	m_Lights.clear();

	itLgt = m_LightsBackup.begin();
	iSize = m_LightsBackup.size();
	for(auto i = 0; i < iSize; i++, itLgt++)
	{
		delete *itLgt; 
	}
	m_LightsBackup.clear();
*/
}

/*
void CGameEng::RestoreLighting()
{
	m_Lights.clear();	
	it_Light itLgt = m_LightsBackup.begin();
	while( itLgt != m_LightsBackup.end() )
	{
		CN3Light* pLgt = *itLgt++;
		m_Lights.push_back(pLgt);
	}
	m_LightsBackup.clear();
}

void CGameEng::BackupReleaseLighting()
{
	m_LightsBackup.clear();
	it_Light itLgt = m_Lights.begin();
	while( itLgt != m_Lights.end() )
	{
		CN3Light* pLgt = *itLgt++;
		m_LightsBackup.push_back(pLgt);
	}
	m_Lights.clear();
}
*/

void CGameEng::SetDefaultLight(CN3Light* pSun, CN3Light* pSupport, CN3Light* pCam)
{
	m_pRefLightSun = pSun;
	m_pRefLightSupport = pSupport;
	m_pRefLightCam = pCam;
}

// 
// 
// 

void CGameEng::Tick(const D3DCOLOR* crDiffuses,			// Diffuse light color.. Write 3 pieces.
					const D3DCOLOR* crAmbients,			// Ambient light color.. Write 3 pieces.
					const D3DCOLOR crFog,				// Fog color...
					const __Vector3& vPosPlayer,		// player position
					const __Quaternion& qtPlayer,		// rotation quaternion
					float fHeightPlayer,				// If you put the key as an argument, the camera and light handling...
					float fSunRadianZ)					// The Z angle of the sun..
{
	if(nullptr == m_pActiveCam) return;

	float fRadius = fHeightPlayer * 2.0f;
	float fYaw = 0;
	__Quaternion qtRot = qtPlayer;
	__Vector3 vAxis(0,1,0);
	qtRot.AxisAngle(vAxis, fYaw);
	
	if(vAxis.y < 0) // If the axis of rotation is negative.
	{
		vAxis.y *= -1.0f;
		fYaw *= -1.0f;
	}

	switch(m_eViewPoint)
	{
		case VP_BACKWARD:
		{
			::D3DXQuaternionRotationYawPitchRoll(&qtRot, fYaw, m_fRotPitchBackward, 0);
			__Matrix44 mtxRot = qtRot;

			m_vEyeToReach.Set(0, 0, -(fRadius / s_CameraData.fFOV) * m_fZoomBackwardOrFoward);
			m_vAtToReach = vPosPlayer; m_vAtToReach.y += fHeightPlayer * 0.8f;
			m_vEyeToReach = m_vAtToReach + (m_vEyeToReach * mtxRot);
		}
		break;
		case VP_FIRST_PERSON:
		{
			::D3DXQuaternionRotationYawPitchRoll(&qtRot, fYaw, m_fRotPitchFirstPerson, 0);
			__Matrix44 mtxRot = qtRot;

			m_vEyeToReach = vPosPlayer; m_vEyeToReach.y += fHeightPlayer - 0.1f;
			m_vAtToReach.Set(0,0,1);
			m_vAtToReach = m_vEyeToReach + (m_vAtToReach * mtxRot);
		}
		break;
		case VP_FOWARD:
		{
			::D3DXQuaternionRotationYawPitchRoll(&qtRot, fYaw, -m_fRotPitchFoward, 0);
			__Matrix44 mtxRot = qtRot;

			m_vEyeToReach.Set(0, 0, fRadius * m_fZoomBackwardOrFoward);
			m_vAtToReach = vPosPlayer; m_vAtToReach.y += fHeightPlayer * 0.8f;
			m_vEyeToReach = m_vAtToReach + (m_vEyeToReach * mtxRot);
		}
		break;
		case VP_THIRD_PERSON:
		{
			::D3DXQuaternionRotationYawPitchRoll(&qtRot, m_fRotYawVPGod, m_fRotPitchThirdFirson, 0);
			__Matrix44 mtxRot = qtRot;

			m_vAtToReach = vPosPlayer; m_vAtToReach.y += fHeightPlayer * 0.8f;
			m_vEyeToReach.Set(0,0,-m_fOffsetVPGod);
			m_vEyeToReach = m_vAtToReach + (m_vEyeToReach * mtxRot);
		}
		break;
	}

	// 
	// Camera collision check...
	if(VP_FIRST_PERSON == m_eViewPoint) // No collision checks in first person.
	{
		auto vUp = __Vector3(0, 1, 0);
		m_pActiveCam->LookAt(m_vEyeToReach, m_vAtToReach, vUp); // I stare..
	}
	else
	{
		__Vector3 vEyeResult = m_vEyeToReach;
		float fNP = m_pActiveCam->m_Data.fNP;
		CGameBase::ACT_WORLD->CheckCollisionCameraWithTerrain(vEyeResult, m_vAtToReach, fNP); // Terrain and Collision Check
		CGameBase::ACT_WORLD->CheckCollisionCameraWithShape(vEyeResult, m_vAtToReach, fNP); // Collision check with object.
		auto vUp = __Vector3(0, 1, 0);
		m_pActiveCam->LookAt(vEyeResult, m_vAtToReach, vUp); // I stare..
	}
	// Camera collision check...
	// 


	// Adjust the Far Plane value..
	// In ApplyCameraAndLight, we actually adjust the value of the fog lights.
	if(m_fFPDeltaCur != m_fFPDeltaToReach)
	{
		float fFPChange = (m_fFPDeltaToReach - m_fFPDeltaCur) * s_fSecPerFrm / 5.0f; // change for 5 seconds.
		m_fFPDeltaCur += fFPChange;

		if(fFPChange < 0 && m_fFPDeltaCur < m_fFPDeltaToReach) m_fFPDeltaCur = m_fFPDeltaToReach;
		if(fFPChange > 0 && m_fFPDeltaCur > m_fFPDeltaToReach) m_fFPDeltaCur = m_fFPDeltaToReach;
	}
	float fFPToRestore = m_pActiveCam->m_Data.fFP;
	m_pActiveCam->m_Data.fFP = s_Options.iViewDist * m_fFPDeltaCur;
	
	m_pActiveCam->m_FogColor = crFog; // Match the color of the fog...
	m_pActiveCam->Tick(); // Calculate values such as application and tetrahedron..

	__Matrix44 mtxRotSun;
	mtxRotSun.RotationZ(fSunRadianZ); // Adjust the angle of the sun.

	/*
	it_Light itLgt = m_Lights.begin();
	int iSize = m_Lights.size();
	for(int i = 0; i < iSize; i++, itLgt++)
	{
		CN3Light* pLight = *itLgt;
		__ASSERT(pLight, "Light pointer is NULL!!!");
		
		if(0 == pLight-&gt;m_Data.nNumber) // default directional light
		{
			// Align View Matrix angle and direction.
			// __Vector3 vDir(0.0f,-1.5f,1.0f);
			// vDir.Normalize();
			// __Matrix44 mtxVI = s_CameraData.mtxViewInverse;
			// mtxVI.PosSet(0,0,0);
			// pLight->m_Data.Direction = vDir * mtxVI;

			// Align the sun and direction.
			__Matrix44 mtxRot; mtxRot.RotationZ(fSunRadianZ);
			__Vector3 vDir(-1,0,1);
			vDir *= mtxRot;
			vDir.Normalize();
			pLight->m_Data.Direction = vDir;
			
			// Apply light color..
			pLight->m_Data.Diffuse = ::_D3DCOLOR_To_D3DCOLORVALUE(crDiffuses[0]);
			pLight->m_Data.Ambient = ::_D3DCOLOR_To_D3DCOLORVALUE(crAmbients[0]);
		}
		else if(1 == pLight->m_Data.nNumber)
		{
			__Vector3 vDir(2,-3, 2); // top to bottom...
			vDir.Normalize();
			pLight->m_Data.Direction = vDir;
			
			// Apply light color..
			pLight->m_Data.Diffuse = ::_D3DCOLOR_To_D3DCOLORVALUE(crDiffuses[1]);
			pLight->m_Data.Ambient = ::_D3DCOLOR_To_D3DCOLORVALUE(crAmbients[1]);
		}
		else if(2 == pLight->m_Data.nNumber)
		{
			__Vector3 vPos = s_CameraData.vEye;
			vPos.y += 16.0f;
			pLight-&gt;PosSet(vPos); // let it go on top of the camera..
			
			// Apply light color..
			pLight->m_Data.Diffuse = ::_D3DCOLOR_To_D3DCOLORVALUE(crDiffuses[2]);
			pLight->m_Data.Ambient = ::_D3DCOLOR_To_D3DCOLORVALUE(crAmbients[2]);
		}


		// handle lightning...
		if(m_fLightningTimeRemain > 0)
		{
			float fLightningDelta = 0;

			if(m_fLightningTimeRemain > LIGHTNING_DURATION * 0.8f)
				fLightningDelta = (m_fLightningTimeRemain - LIGHTNING_DURATION * 0.8f) / (LIGHTNING_DURATION * 0.2f);
			else
				fLightningDelta = m_fLightningTimeRemain / (LIGHTNING_DURATION * 0.8f);

			pLight->m_Data.Diffuse.r += (1.0f - pLight->m_Data.Diffuse.r) * fLightningDelta * 0.4f;
			pLight->m_Data.Diffuse.g += (1.0f - pLight->m_Data.Diffuse.g) * fLightningDelta * 0.5f;
			pLight->m_Data.Diffuse.b += (1.0f - pLight->m_Data.Diffuse.b) * fLightningDelta;

			m_fLightningTimeRemain -= CN3Base::s_fSecPerFrm;
			if(m_fLightningTimeRemain < 0) m_fLightningTimeRemain = 0;
		}

		pLight->Tick();
	}
	*/
	if(m_pRefLightSun)
	{
		// Align the direction with the sun.
		__Matrix44 mtxRot; mtxRot.RotationZ(fSunRadianZ);
		__Vector3 vDir(-1,0,1);
		vDir *= mtxRot;
		vDir.Normalize();
		m_pRefLightSun->m_Data.Direction = vDir;
		
		// Apply light color..
		m_pRefLightSun->m_Data.Diffuse = ::_D3DCOLOR_To_D3DCOLORVALUE(crDiffuses[0]);
		m_pRefLightSun->m_Data.Ambient = ::_D3DCOLOR_To_D3DCOLORVALUE(crAmbients[0]);

		// Lightning treatment...
		if(m_fLightningTimeRemain > 0)
		{
			float fLightningDelta = 0;

			if(m_fLightningTimeRemain > LIGHTNING_DURATION * 0.8f)
				fLightningDelta = (m_fLightningTimeRemain - LIGHTNING_DURATION * 0.8f) / (LIGHTNING_DURATION * 0.2f);
			else
				fLightningDelta = m_fLightningTimeRemain / (LIGHTNING_DURATION * 0.8f);

			m_pRefLightSun->m_Data.Diffuse.r += (1.0f - m_pRefLightSun->m_Data.Diffuse.r) * fLightningDelta * 0.4f;
			m_pRefLightSun->m_Data.Diffuse.g += (1.0f - m_pRefLightSun->m_Data.Diffuse.g) * fLightningDelta * 0.5f;
			m_pRefLightSun->m_Data.Diffuse.b += (1.0f - m_pRefLightSun->m_Data.Diffuse.b) * fLightningDelta;

			m_fLightningTimeRemain -= CN3Base::s_fSecPerFrm;
			if(m_fLightningTimeRemain < 0) m_fLightningTimeRemain = 0;
		}
	}
	if(m_pRefLightSupport)
	{
		__Vector3 vDir(2,-3, 2); // top to bottom...
		vDir.Normalize();
		m_pRefLightSupport->m_Data.Direction = vDir;
		
		// Apply light color..
		m_pRefLightSupport->m_Data.Diffuse = ::_D3DCOLOR_To_D3DCOLORVALUE(crDiffuses[1]);
		m_pRefLightSupport->m_Data.Ambient = ::_D3DCOLOR_To_D3DCOLORVALUE(crAmbients[1]);

		// Lightning treatment...
		if(m_fLightningTimeRemain > 0)
		{
			float fLightningDelta = 0;

			if(m_fLightningTimeRemain > LIGHTNING_DURATION * 0.8f)
				fLightningDelta = (m_fLightningTimeRemain - LIGHTNING_DURATION * 0.8f) / (LIGHTNING_DURATION * 0.2f);
			else
				fLightningDelta = m_fLightningTimeRemain / (LIGHTNING_DURATION * 0.8f);

			m_pRefLightSupport->m_Data.Diffuse.r += (1.0f - m_pRefLightSupport->m_Data.Diffuse.r) * fLightningDelta * 0.4f;
			m_pRefLightSupport->m_Data.Diffuse.g += (1.0f - m_pRefLightSupport->m_Data.Diffuse.g) * fLightningDelta * 0.5f;
			m_pRefLightSupport->m_Data.Diffuse.b += (1.0f - m_pRefLightSupport->m_Data.Diffuse.b) * fLightningDelta;

			m_fLightningTimeRemain -= CN3Base::s_fSecPerFrm;
			if(m_fLightningTimeRemain < 0) m_fLightningTimeRemain = 0;
		}
	}
	if(m_pRefLightCam)
	{
		__Vector3 vPos = s_CameraData.vEye;
		vPos.y += 16.0f;
		m_pRefLightCam->PosSet(vPos); // Let&#39;s go over the camera.
		
		// Apply light color..
		m_pRefLightCam->m_Data.Diffuse = ::_D3DCOLOR_To_D3DCOLORVALUE(crDiffuses[2]);
		m_pRefLightCam->m_Data.Ambient = ::_D3DCOLOR_To_D3DCOLORVALUE(crAmbients[2]);

		// Lightning treatment...
		if(m_fLightningTimeRemain > 0)
		{
			float fLightningDelta = 0;

			if(m_fLightningTimeRemain > LIGHTNING_DURATION * 0.8f)
				fLightningDelta = (m_fLightningTimeRemain - LIGHTNING_DURATION * 0.8f) / (LIGHTNING_DURATION * 0.2f);
			else
				fLightningDelta = m_fLightningTimeRemain / (LIGHTNING_DURATION * 0.8f);

			m_pRefLightCam->m_Data.Diffuse.r += (1.0f - m_pRefLightCam->m_Data.Diffuse.r) * fLightningDelta * 0.4f;
			m_pRefLightCam->m_Data.Diffuse.g += (1.0f - m_pRefLightCam->m_Data.Diffuse.g) * fLightningDelta * 0.5f;
			m_pRefLightCam->m_Data.Diffuse.b += (1.0f - m_pRefLightCam->m_Data.Diffuse.b) * fLightningDelta;

			m_fLightningTimeRemain -= CN3Base::s_fSecPerFrm;
			if(m_fLightningTimeRemain < 0) m_fLightningTimeRemain = 0;
		}
	}
}


void CGameEng::ApplyCameraAndLight() const
{
	if(m_pActiveCam)
	{
		m_pActiveCam->Apply(); // Instead of the above, write one...
	}
/*
	for(it_Light it = m_Lights.begin(), itEnd = m_Lights.end(); it != itEnd; it++)
	{
		CN3Light* pLight = *it;
		__ASSERT(pLight, "Light pointer is NULL!!!");
		
		pLight->Apply();
	}
*/
}

void CGameEng::ViewPointChange(e_ViewPoint eVP)
{
	if(eVP < VP_BACKWARD || eVP > VP_THIRD_PERSON)
	{
		if(m_eViewPoint < VP_BACKWARD) m_eViewPoint = VP_BACKWARD;
		if(m_eViewPoint >= VP_THIRD_PERSON) m_eViewPoint = VP_THIRD_PERSON;

		int iVP = (int)(m_eViewPoint) + 1;
		if ( iVP > (VP_THIRD_PERSON) ) iVP = VP_BACKWARD;
		m_eViewPoint = (e_ViewPoint)iVP;						// Point increase...
	}
	else
	{
		m_eViewPoint = eVP;						// Point increase...
	}
}

void CGameEng::CameraPitchAdd(float fRotXPerSec)
{
	if(nullptr == m_pActiveCam) return;
	const float fPitchMax = D3DXToRadian(70.0f);
	float fPitchMin = -D3DXToRadian(50.0f);
	if(VP_BACKWARD == m_eViewPoint)
	{
		m_fRotPitchBackward += fRotXPerSec * s_fSecPerFrm;
		if(m_fRotPitchBackward > fPitchMax) m_fRotPitchBackward = fPitchMax; // value shown below
		else if(m_fRotPitchBackward < fPitchMin) m_fRotPitchBackward = fPitchMin; // The price of the above..
	}
	else if(VP_FIRST_PERSON == m_eViewPoint)
	{
		m_fRotPitchFirstPerson += fRotXPerSec * s_fSecPerFrm;
		if(m_fRotPitchFirstPerson > fPitchMax) m_fRotPitchFirstPerson = fPitchMax; // value shown below
		else if(m_fRotPitchFirstPerson < fPitchMin) m_fRotPitchFirstPerson = fPitchMin; // The price of the above..
	}
	else if(VP_FOWARD == m_eViewPoint)
	{
		m_fRotPitchFoward += fRotXPerSec * s_fSecPerFrm;
		if(m_fRotPitchFoward > fPitchMax) m_fRotPitchFoward = fPitchMax; // value shown below
		else if(m_fRotPitchFoward < fPitchMin) m_fRotPitchFoward = fPitchMin; // The upside value..
	}
	else if(VP_THIRD_PERSON == m_eViewPoint) // From above, only the distance increases and decreases.
	{
		fPitchMin = D3DXToRadian(10.0f); // value limit under the gel

		m_fRotPitchThirdFirson += fRotXPerSec * s_fSecPerFrm;
		if(m_fRotPitchThirdFirson > fPitchMax) m_fRotPitchThirdFirson = fPitchMax; // value shown below
		else if(m_fRotPitchThirdFirson < fPitchMin) m_fRotPitchThirdFirson = fPitchMin; // The upside value..
	}
}

void CGameEng::CameraYawAdd(float fRotYPerSec)
{
	if(VP_THIRD_PERSON == m_eViewPoint) // What you see above...
	{
		m_fRotYawVPGod += fRotYPerSec * s_fSecPerFrm; // The angle of rotation of the camera when viewed from above.
		return; // go back!
	}
}

void CGameEng::CameraZoom(float fDelta)
{
	if(nullptr == m_pActiveCam) return;
	if(VP_BACKWARD == m_eViewPoint || VP_FOWARD == m_eViewPoint)
	{
		m_fZoomBackwardOrFoward -= fDelta * s_fSecPerFrm; // 1.5 person, camera zoom 1.0f is standard when viewed from the front.
		if(m_fZoomBackwardOrFoward < 0.4f) m_fZoomBackwardOrFoward = 0.4f; // value shown below
		else if(m_fZoomBackwardOrFoward >= 1.6f) m_fZoomBackwardOrFoward = 1.6f;
	}
	else if(VP_THIRD_PERSON == m_eViewPoint) // From above, only the distance increases and decreases.
	{
		m_fOffsetVPGod -= 4.0f * fDelta * s_fSecPerFrm;
		if(m_fOffsetVPGod < 2.0f) m_fOffsetVPGod = 2.0f; // Stop when it gets too small...
		else if(m_fOffsetVPGod > 10.0f) m_fOffsetVPGod = 10.0f; // Stop when it gets too big.
		return; // go back!
	}
}

void CGameEng::FarPlaneDeltaSet(float fFPDelta, bool bUpdateImmediately)
{
	if(fFPDelta < 0.25f) fFPDelta = 0.25f;
	if(fFPDelta > 1.0f) fFPDelta = 1.0f;

	m_fFPDeltaToReach = fFPDelta;
	if(bUpdateImmediately) m_fFPDeltaCur = fFPDelta;
}

// void CGameEng::LightDeltaSet(float fLightDelta, bool bUpdateImmediately)
// {
// if(fLightDelta < 0.25f) fLightDelta = 0.25f;
// if(fLightDelta > 1.0f) fLightDelta = 1.0f;
//
// m_fLightDeltaToReach = fLightDelta;
// if(bUpdateImmediately) m_fLightDeltaCur = fLightDelta;
// }

void CGameEng::Lightning()
{
	m_fLightningTimeRemain = LIGHTNING_DURATION; // Timer when lightning strikes.
}
