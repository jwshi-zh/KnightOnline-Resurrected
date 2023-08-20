#pragma once

#include "N3Base.h"

enum {	GEW_CLEAR=0,			// Sunny
		GEW_DRIZZLE_RAIN=1,		// Drizzle
		GEW_RAINY=2,			// moderate amount of rain
		GEW_HEAVY_RAIN=3,		// heavy rain
		GEW_SNOW1=11,			// warm eyes
		GEW_SNOW2=12,			// suitable eyes
		GEW_HEAVY_SNOW=13		// heavy snow
};

class CN3GERain;
class CN3GESnow;
class CN3GlobalEffectMng : public CN3Base  
{
public:
	CN3GlobalEffectMng();
	virtual ~CN3GlobalEffectMng();

// Attributes
public:
protected:
	float			m_fCellSize;	// inner cell size
	POINT			m_CurCellPos;	// Current internal cell coordinates
	__Vector3		m_vPos[9];		// Center coordinates of 9 adjacent cells

	// effects...
	CN3GERain*		m_pGERain;		// rain
	CN3GESnow*		m_pGESnow;		// eye

// Operations
public:
	virtual void	Release();
	void			Tick();
	void			Render();
// void SetWeather(int iWeather); // specify the weather
	void			WeatherSetRainy(int iPercent);
	void			WeatherSetSnow(int iPercent);
	void			WeatherSetClean() const;

protected:

};
