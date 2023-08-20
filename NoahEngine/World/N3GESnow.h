#pragma once

#include "N3GlobalEffect.h"

class CN3GESnow : public CN3GlobalEffect  
{
// Structures
protected:
	struct __SnowParticle
	{
		__Vector3	vPos;							// Zoom Depth of Turning Radius
		__Vector3	vOffset1, vOffset2, vOffset3;	// How far point 1 2 3 is from the midpoint of the particle
		float		fRadius;						// turning radius
		float		fRadian;						// current rotated angle
		// float fRVel // rotational angular velocity (assumed constant for now)
		// __Vector3 vVelocity // Falling velocity (assuming they are all the same for now)
	};

// Attributes
public:
	void SetVelocity(__Vector3& v) {m_vVelocity = v;}
protected:
	float		m_fWidth;		// the width of the space in which the snow falls
	float		m_fHeight;		// The height of the space in which snow falls
	float		m_fSnowSize;	// the size of an eye
	__Vector3	m_vVelocity;	// overall direction and speed
	CN3Texture*	m_pTex;			// snow texture
	__SnowParticle*	m_pSnowParticle;	// snowflake information

// Operations
public:
	void Release();
	void Tick();
	void Render(__Vector3& vPos);
	void Create(float fDensity, float fWidth, float fHeight, float fSnowSize, const __Vector3& vVelocity, float fTimeToFade = 3.0f);

	CN3GESnow();
	virtual ~CN3GESnow();

};
