#pragma once

#include "N3Transform.h"

class CN3Light : public CN3Transform
{
public:
	typedef struct __Light : public _D3DLIGHT9
	{
	public:
		BOOL		bOn;
		int			nNumber;
		
		void		Zero() { memset(this, 0, sizeof(__Light)); }
		void		InitPoint(int nLgtNumber, D3DXVECTOR3& dvPos, _D3DCOLORVALUE& ltColor, float fRange = 10000.0f, float fAttenuation = 0.5f)
		{
			this->Zero();
			nNumber = nLgtNumber;
			Type = D3DLIGHT_POINT;
			Position = dvPos;
			//Specular = 
			Diffuse = ltColor;
			Ambient.r = ltColor.r * 0.7f;
			Ambient.g = ltColor.g * 0.7f;
			Ambient.b = ltColor.b * 0.7f;

			Falloff = 1.0f;		// It is usually written as 1.0 because the effect is insignificant and it takes a load.
			if(fRange < 0.0001f) fRange = 0.0001f;
			Attenuation0 = 1.0f - fAttenuation;
			Attenuation1 = fAttenuation/fRange; // Attenuation Range Calculation. Make sure that half the range is exactly half the attenuation.
			Attenuation2 = fAttenuation/(fRange*fRange);
			Range = fRange * 4.0f;
			bOn = TRUE;
		}
		void		InitDirection(int nLgtNumber, D3DXVECTOR3& dvDir, _D3DCOLORVALUE& ltColor)
		{
			this->Zero();
			
			nNumber = nLgtNumber;
			bOn = TRUE;

			Type = D3DLIGHT_DIRECTIONAL;
			Direction = dvDir;

			Diffuse = ltColor;
			Ambient.r = ltColor.r * 0.7f;
			Ambient.g = ltColor.g * 0.7f;
			Ambient.b = ltColor.b * 0.7f;
		}
		void		InitSpot(int nLgtNumber, D3DXVECTOR3& dvPos, D3DXVECTOR3& dvDir, _D3DCOLORVALUE& ltColor, float fTheta, float fPhi, float fRange = 10000.0f)
		{
			this->Zero();
			nNumber = nLgtNumber;
			Type = D3DLIGHT_SPOT;
			Position = dvPos;
			Direction = dvDir;

			Diffuse = ltColor;
			Ambient.r = ltColor.r * 0.7f;
			Ambient.g = ltColor.g * 0.7f;
			Ambient.b = ltColor.b * 0.7f;

			if(fRange < 0.0001f) fRange = 0.0001f;
			Attenuation0 = 1.0f;
			Attenuation1 = 1.0f/(fRange/2.0f); // Attenuation Range Calculation. Make sure that half the range is exactly half the attenuation.
			Range = fRange;

			Falloff = 1.0f;		// It is usually written as 1.0 because the effect is insignificant and it takes a load.
			Theta = fTheta;
			Phi = fPhi;
			bOn = TRUE;
		}
	} __Light;

	__Light m_Data;

public:
	void		DirSet(const __Vector3& vDir) { m_Data.Direction = vDir; }
	void		DirSet(float fx, float fy, float fz) { m_Data.Direction.x = fx; m_Data.Direction.y = fy; m_Data.Direction.z = fz; }
	
	void		PosSet(const __Vector3& vPos) { m_vPos = m_Data.Position = vPos; }
	void		PosSet(float fx, float fy, float fz) { m_Data.Position.x = fx; m_Data.Position.y = fy; m_Data.Position.z = fz; m_vPos = m_Data.Position; }

	void		Apply() const;
	void		Tick(float fFrm = FRAME_SELFPLAY);
	
	bool		Load(HANDLE hFile);
#ifdef _N3TOOL
	bool		Save(HANDLE hFile);
#endif // end of _N3TOOL

	void		Release();
	CN3Light();
	virtual ~CN3Light();
};
