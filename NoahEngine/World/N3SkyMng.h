#pragma once

#include "N3BaseFileAccess.h"
#include <vector>

const float TIME_REAL_PER_GAME = 10.0f;						// Ratio of real time to game time (Currently, games run ten times faster than real time..)
const float TIME_GAME_PER_REAL = 1.0f/TIME_REAL_PER_GAME;	// Ratio of game time to real time
#define CONVERT_SEC(h, m, s) ((h)*3600 + (m)*60 + (s))

// time management
enum		eSKY_DAYCHANGE {	SDC_SKYCOLOR=0,		// Sky blue
								SDC_FOGCOLOR,		// fog color
								SDC_STARCOUNT,		// number of stars
								SDC_MOONPHASE,		// moon shape
								SDC_SUNCOLOR,		// Sea Color, Size - 1000 How full this viewport is.
								SDC_GLOWCOLOR,		// The year's glow color, size - 1000 How big this viewport fills.
								SDC_FLARECOLOR,		// The flare color of the sun, size - 1000 How much this viewport fills up.
								SDC_CLOUD1COLOR,	// Color of cloud layer 1
								SDC_CLOUD2COLOR,	// Color of cloud layer 2
								SDC_CLOUDTEX,		// change cloud texture
								
								SDC_LIGHT0COLOR,	// Direction Light 0
								SDC_LIGHT1COLOR,	// Direction Light 1
								SDC_LIGHT2COLOR,	// Light around the player
								
								NUM_SKYDAYCHANGE,
								
								SDC_UNKNOWN = 0xffffffff
};

struct		__SKY_DAYCHANGE
{
	std::string		szName;			// naming...
	eSKY_DAYCHANGE	eSkyDayChange;	// what kind of change
	DWORD			dwWhen;			// When is the change start? (time in seconds from 0:00)
	DWORD			dwParam1;		// parameter 1
	DWORD			dwParam2;		// parameter 2
	float			fHowLong;		// How long will it take? (real time seconds)

	void Init()
	{
		szName = "";
		eSkyDayChange = SDC_UNKNOWN;
		dwWhen = dwParam1 = dwParam2 = 0;
		fHowLong = 0;
	}

	void Init(const std::string& szName_Arg, eSKY_DAYCHANGE eSDC_Arg, DWORD dwWhen_Arg, DWORD dwParam1_Arg, DWORD dwParam2_Arg, float fHowLong_Arg)
	{
		szName = szName_Arg;
		eSkyDayChange = eSDC_Arg;
		dwWhen = dwWhen_Arg;
		dwParam1 = dwParam1_Arg;
		dwParam2 = dwParam2_Arg;
		fHowLong = fHowLong_Arg;
	}

	bool Load(HANDLE hFile)
	{
		szName = "";

		DWORD dwRWC = 0;
		int nL = 0;
		ReadFile(hFile, &nL, 4, &dwRWC, nullptr);
		if(nL > 0) 
		{
			std::vector<char> buffer(nL+1, NULL);
			ReadFile(hFile, buffer.data(), nL, &dwRWC, nullptr);
			szName = buffer.data();
		}

		ReadFile(hFile, &eSkyDayChange, 4, &dwRWC, nullptr);
		ReadFile(hFile, &dwWhen, 4, &dwRWC, nullptr);
		ReadFile(hFile, &dwParam1, 4, &dwRWC, nullptr);
		ReadFile(hFile, &dwParam2, 4, &dwRWC, nullptr);
		ReadFile(hFile, &fHowLong, 4, &dwRWC, nullptr);

		return true;
	}
	
	bool Save(HANDLE hFile) const
	{
		DWORD dwRWC = 0;

		const int nL = szName.size();
		WriteFile(hFile, &nL, 4, &dwRWC, nullptr);
		if(nL > 0) WriteFile(hFile, szName.c_str(), nL, &dwRWC, nullptr);

		WriteFile(hFile, &eSkyDayChange, 4, &dwRWC, nullptr);
		WriteFile(hFile, &dwWhen, 4, &dwRWC, nullptr);
		WriteFile(hFile, &dwParam1, 4, &dwRWC, nullptr);
		WriteFile(hFile, &dwParam2, 4, &dwRWC, nullptr);
		WriteFile(hFile, &fHowLong, 4, &dwRWC, nullptr);

		return true;
	}

	__SKY_DAYCHANGE() { this->Init(); }
};

const int MAX_GAME_LIGHT = 3;

class CN3SkyMng : public CN3BaseFileAccess
{
public:
	CN3SkyMng();
	virtual ~CN3SkyMng();
// Attributes
public:
	enum		eSKY_WEATHER {  SW_CLEAR=1,	// Sunny
								SW_RAINY,	// rain
								SW_SNOW};	// eye
protected:
	class CN3Sky*			m_pSky;
	class CN3Moon*			m_pMoon;
	class CN3Sun*			m_pSun;
	class CN3Cloud*			m_pCloud;
	class CN3Star*			m_pStar;
	class CN3ColorChange*	m_pLightColorDiffuses[MAX_GAME_LIGHT];
	class CN3ColorChange*	m_pLightColorAmbients[MAX_GAME_LIGHT];

	std::vector<__SKY_DAYCHANGE> m_DayChanges;		// Let's qsort after entering information
	int					m_iDayChangeCurPos;

	std::vector<__SKY_DAYCHANGE> m_WeatherChanges;		// Let's qsort after entering information
	int					m_iWeatherChangeCurPos;

	DWORD		m_dwCheckTick;	// Window TickCount (real time) when receiving time from server
	DWORD		m_dwCheckGameTime;	// Time downloaded from server (game time in seconds) 0 ~ (24*60*60)
	eSKY_WEATHER	m_eWeather;

	int			m_iYear;
	int			m_iMonth;
	int			m_iDay;

	int			m_iHourFix;		// The direction indicated by time when the sun and moon directions are fixed.


	float			m_fCellSize;	// inner cell size
	POINT			m_CurCellPos;	// Current internal cell coordinates
	__Vector3		m_vPos[9];		// Center coordinates of 9 adjacent cells

	// effects...
	class CN3GERain*		m_pGERain;		// rain
	class CN3GESnow*		m_pGESnow;		// eye
#ifdef _N3GAME // In non-game tools, you don't need it...
	class CN3SndObj*		m_pSnd_Weather_Rain;
	class CN3SndObj*		m_pSnd_Weather_Snow;
#endif // #ifdef _N3GAME

// Operations
public:
#ifdef _N3TOOL
	void InitToDefaultHardCoding();
	bool LoadFromTextFile(const char* szIniFN);
	bool SaveToTextFile(const char* szIniFN);
	bool DayChangeParse(FILE* fp, __SKY_DAYCHANGE* pDayChange);
	bool DayChangeWrite(FILE* fp, __SKY_DAYCHANGE* pDayChange);

	int					DayChangeCount() { return m_DayChanges.size(); };
	__SKY_DAYCHANGE*	DayChangeGet(int iIndex);
	__SKY_DAYCHANGE*	DayChangeAdd();
	__SKY_DAYCHANGE*	DayChangeInsert(int iIndex);
	bool				DayChangeDelete(int iIndex);

	CN3Texture*			SunTextureSet(int iIndex, const char* szPath);
	CN3Texture*			MoonTextureSet(const char* szPath);
	CN3Texture*			CloudTextureSet(int iIndex, const char* szPath);
	
	CN3Texture*			SunTextureGet(int iIndex);
	CN3Texture*			MoonTextureGet();
	CN3Texture*			CloudTextureGet(int iIndex);
	const char*			CloudTextureFileName(int iIndex);
#endif

	bool Load(HANDLE hFile);

#ifdef _N3GAME // In non-game tools, you don't need it...
	void ReleaseSound();
#endif // #ifdef _N3GAME

#ifdef _N3TOOL
	bool Save(HANDLE hFile);
#endif

	D3DCOLOR	GetFogColor();
	D3DCOLOR	GetSkyColor();
	D3DCOLOR	GetSunColor();
	D3DCOLOR	GetGlowColor();
	D3DCOLOR	GetFlareColor();
	float		GetSunRatio();
	float		GetGlowRatio();
	float		GetFlareRatio();

	D3DCOLOR	GetCloud1Color();
	D3DCOLOR	GetCloud2Color();
	
	float		GetSunAngleByRadin();

	D3DCOLOR	GetLightDiffuseColor(int iIndex);
	D3DCOLOR	GetLightAmbientColor(int iIndex);

	void	Release();
	void	Render() const; // Render the sky...
	void	RenderWeather(); // Weather rendering such as rain or snow..
	void	Tick();

	// time management
	void	GetGameTime(int* piYear, int* piMonth, int* piDay, int* piHour, int*piMin);
	void	SetGameTime(int iYear, int iMonth, int iDay, int iHour, int iMin);
	void	SetWeather(eSKY_WEATHER eWeather, int iPercentage);	// weather settings
	
	void	SunAndMoonDirectionFixByHour(int iHour); // Sun and moon angle management

	CN3Sun*	GetSunPointer()	{	return m_pSun;	}
	
protected:
	void		SetCheckGameTime(DWORD dwCheckGameTime);// Set CheckGameTime and set the current time again. (Called when forced to a specific time.)
	int			GetLatestChange(eSKY_DAYCHANGE eSDC, int iPos);// Get the most recently changed position before the specified position (iPos) in m_pDayChangeQueues
	void		ChangeSky(__SKY_DAYCHANGE* pSDC, float fTakeTime);
	int			GetDayChangePos_AfterNSec(DWORD dwCurGameTime, float fSec);		// Get position of DayChangeQueue after N seconds of real time
	static int		CompareTime(const void* pArg1, const void* pArg2);

};
