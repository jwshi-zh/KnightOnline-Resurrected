#pragma once

#include "N3SndObj.h"

class CN3SndEng;

class CN3SndObjStream : public CN3SndObj
{
private:
	int					m_PlayTime;
	int					m_FinalByte;
			
	DSBUFFERDESC		m_dsbd;
	DWORD				m_BufferSize;
	DWORD				m_BlockSize;
	
	int					m_CurrState;
	int					m_PrevState;
	DWORD				m_CurrPos;
	int					m_PastTime;

	//related wave data
	WAVEFORMATEX		m_WaveFormat;
	DWORD				m_WaveSize;
	LPSTR				m_pWaveMem;
	HGLOBAL				m_hWaveMem;
	MMCKINFO			mmCkInfoRIFF;
	MMCKINFO			mmCkInfoChunk;
	HMMIO				hMMIO;
	
public:
	bool Create(const std::string& szFN, e_SndType eType = SNDTYPE_STREAM);

//	void Play(float delay = 0.0f, float fFadeInTime = 0.0f);
//	void Stop(float fFadeOutTime = 0.0f);		
	
	void Tick();		
	void Release();
	
	CN3SndObjStream();
	virtual ~CN3SndObjStream();
			
private:
	void RealPlay();
	
	//void Stop();

	BOOL LoadWave(LPCSTR pFileName);
	BOOL WriteBuffer();
	BOOL InitWriteBuffer() const;
	void Reset();
};
