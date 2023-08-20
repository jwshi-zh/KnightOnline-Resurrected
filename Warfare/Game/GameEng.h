#pragma once

#include "N3Eng.h"
#include <list>

class CN3Camera;
class CN3Light;

// camera point of view
enum e_ViewPoint		{	VP_BACKWARD = 0,			// View from behind the player&#39;s back 1.5 person
							VP_FIRST_PERSON = 1,		// the first person
							VP_FOWARD = 2,				// Look in front of the player.
							VP_THIRD_PERSON = 3,		// Third person mode .. Mouse works too..
							VP_UNKNOWN = 0xffffffff };  // Only the camera quickly for development....

const float CAMERA_MOVE_SPEED = 5.0f;

class CGameEng : public CN3Eng  
{
protected:
	CN3Light*				m_pRefLightSun;
	CN3Light*				m_pRefLightSupport;
	CN3Light*				m_pRefLightCam;
	// std::list<CN3Light*>	m_Lights;
	// std::list<CN3Light*>	m_LightsBackup;
	std::list<CN3Camera*>	m_Cameras;
	CN3Camera*				m_pActiveCam;

	__Vector3		m_vEyeToReach;
	__Vector3		m_vAtToReach;

	float			m_fFPDeltaCur; // today
	float			m_fFPDeltaToReach; // It changes by targeting this value.

	float			m_fRotPitchBackward; // Camera pitch angle in 1.5 person view.
	float			m_fRotPitchFirstPerson; // Camera pitch angle when in first person view.
	float			m_fRotPitchFoward; // The camera pitch angle when viewed from the front.
	float			m_fRotPitchThirdFirson; // Third person view camera pitch angle
	
	float			m_fOffsetVPGod; // Distance when viewed from above.
	float			m_fRotYawVPGod; // The angle of rotation of the camera when viewed from above.

	float			m_fZoomBackwardOrFoward; // 1.5 person, camera zoom 1.0f is standard when viewed from the front.
	float			m_fLightningTimeRemain; // Timer when lightning strikes.

	e_ViewPoint		m_eViewPoint; // camera point of view.

public:
	void			SetDefaultLight(CN3Light* pSun, CN3Light* pSupport, CN3Light* pCam);
	void			RestoreLighting();
	void			BackupReleaseLighting();
	void			Lightning(); // Lightning and adjusting the light.
	float			FarPlaneDeltaCur() const { return m_fFPDeltaCur; }
	float			FarPlaneDeltaToReach() { return m_fFPDeltaToReach; }
	void			FarPlaneDeltaSet(float fFPDelta, bool bUpdateImmediately);

	// Light functions
	// void			LightAdd(CN3Light* pLight) { m_Lights.push_back(pLight); }
	// int				LightCount() { return m_Lights.size(); }
	// CN3Light*		Light(int index);

	// Camera functions
	void			CameraAdd(CN3Camera *pCamera) { m_Cameras.push_back(pCamera); }
	int				CameraCount() { return m_Cameras.size(); }
	CN3Camera*		Camera(int index);
	void			CameraSetActiveByIndex(int index) { CN3Camera* pCam = this->Camera(index); if(pCam) m_pActiveCam = pCam; }
	CN3Camera*		CameraGetActive() { return m_pActiveCam; }

	// added that...
	void			ViewPointChange(e_ViewPoint eVP = VP_UNKNOWN);
	e_ViewPoint		ViewPoint() { return m_eViewPoint; } // point of view..
	void			CameraPitchAdd(float fRotXPerSec);
	void			CameraYawAdd(float fRotYPerSec);
	void			CameraZoom(float fDelta);
	float			CameraYaw() { return m_fRotYawVPGod; } // The angle of rotation of the camera when viewed from above.
	void			SetActiveCamera(CN3Camera* pCamera) { m_pActiveCam = pCamera; }
	void			Tick(const D3DCOLOR* crDiffuses,			// Diffuse light color..
						const D3DCOLOR* crAmbients,			// Ambient light color..
						const D3DCOLOR crFog,				// Fog color...
						const __Vector3& vPosPlayer,		// player position
						const __Quaternion& qtPlayer,		// rotation quaternion
						float fHeightPlayer,				// If you put the key as an argument, the camera and light handling...
						float fSunRadianZ);					// The Z angle of the sun..
	void			ApplyCameraAndLight() const; // Apply set values to camera and light. Light varies according to the angle of the sun.

	CGameEng();
	virtual ~CGameEng();

};

/*
inline CN3Light* CGameEng::Light(int index)
{
	if(index < 0 || index >= m_Lights.size()) return NULL;
	it_Light itLgt = m_Lights.begin();
	for(int i = 0; i < index; i++, itLgt++);

	return *itLgt;
}
*/

inline CN3Camera* CGameEng::Camera(int index)
{
	if(index < 0 || index >= m_Cameras.size()) return nullptr;
	auto itCam = m_Cameras.begin();
	for(int i = 0; i < index; i++, itCam++);

	return *itCam;
}
