#pragma once

#include "WaveFile.h"
#include <DSound.h>
#include <string>

class CN3SndObj;

typedef struct __SoundSource
{
	int						iID;
	std::string				szFN;
	LPDIRECTSOUNDBUFFER		pDSBuff;
	int						Size;
	int						Type; // 0:2d 1:3d 2: streammin..
	int						Count;
	int						iMaxCount;
	CN3SndObj**				ppObjs;

	__SoundSource()
	{
		iID = 0;
		ppObjs = nullptr;
		szFN = "";
		pDSBuff = nullptr;
		Size = 0;
		Type = 0;
		Count = 0;
		iMaxCount = 0;
	}	
} SOUNDSOURCE, *LPSOUNDSOURCE;

typedef struct __TABLE_SOUND
{
	DWORD		dwID;
	std::string	szFN;
	int			iType;		// sound type...
	int			iNumInst;	// The maximum number of available instances.
} TABLE_SOUND;

enum e_SndType { SNDTYPE_2D=0, SNDTYPE_3D, SNDTYPE_STREAM };
enum e_SndState { SNDSTATE_STOP=0, SNDSTATE_DELAY, SNDSTATE_FADEIN, SNDSTATE_PLAY, SNDSTATE_FADEOUT };
