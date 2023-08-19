#pragma once

#include "N3SndDef.h"

class CN3SndObj;

class CN3SndEng  
{
protected:
	LPDIRECTSOUND8			m_pDS;

	LPDIRECTSOUND3DLISTENER m_pDSListener;			// 3D listener object
	DS3DLISTENER            m_dsListenerParams;		// Listener properties

	bool					m_Tick;

protected:
	HRESULT RestoreBuffer( LPDIRECTSOUNDBUFFER pDSB, BOOL* pbWasRestored );
	bool SetPrimaryBufferFormat( DWORD dwPrimaryChannels, DWORD dwPrimaryFreq, DWORD dwPrimaryBitRate );
	bool Get3DListenerInterface( LPDIRECTSOUND3DLISTENER* ppDSListener );

	bool FillBufferWithSound( LPSOUNDSOURCE pSrc, CWaveFile* pWaveFile);
	
public:
	LPDIRECTSOUND8*		GetDS() { return &m_pDS; }
	void SetDopplerFactor(D3DVALUE factor);
	
	void DuplicateBuff(LPSOUNDSOURCE pSrc, CN3SndObj* pDest, D3DVECTOR* pPos= nullptr);
	
	void Tick();
	
	void SetRollOffFactor(D3DVALUE value, bool IsDeferred = true);
	void SetListenerOrientation(D3DVECTOR* pVAt, D3DVECTOR* pVUp, bool IsDeferred = true);
	void SetListenerPos(D3DVECTOR* pVPos, bool IsDeferred = true);
	
	bool LoadSource(LPSOUNDSOURCE pSrc);
	bool Init( HWND hWnd, DWORD dwCoopLevel = DSSCL_PRIORITY, DWORD dwPrimaryChannels = 2, DWORD dwPrimaryFreq = 22050, DWORD dwPrimaryBitRate = 16 );
	void Release();

	CN3SndEng();
	virtual ~CN3SndEng();

};
