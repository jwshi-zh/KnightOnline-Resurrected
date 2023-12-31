#include "pch.h"
#include "N3SndObj2D.h"
#include "N3SndMgr.h"
#include "N3Base.h"
#include <math.h>

CN3SndObj2D::CN3SndObj2D()
{
	m_pDSBuff = nullptr;
	m_bIsLoop = false;
	m_iVol = -1;
	m_iType = SNDTYPE_2D;
	m_szFileName = "";
	m_pRefMgr = nullptr;

	m_fFadeInTime = 0;
	m_fFadeOutTime = 0;
	m_fStartDelayTime = 0;
	m_fTmpSecPerFrm = 0;
	m_ePlayState = SNDSTATE_STOP;

	m_iMaxVolume = 100;

	m_bUse = false;
}

CN3SndObj2D::~CN3SndObj2D()
{
	Release();
}

void CN3SndObj2D::Init()
{
	Release();

	m_bIsLoop = false;
	m_iVol = -1;
	m_iType = SNDTYPE_2D;
	m_pRefMgr = nullptr;
	m_szFileName = "";

	m_fStartDelayTime = 0;
	m_fTmpSecPerFrm = 0;
	m_ePlayState = SNDSTATE_STOP;
	m_fFadeInTime = 0;
	m_fFadeOutTime = 0;

	m_iMaxVolume = 100;
	
	m_bUse = false;
}

void CN3SndObj2D::Release()
{
	if(m_pDSBuff)
	{
		m_pDSBuff->Stop();
		m_pDSBuff->Release();
		m_pDSBuff = nullptr;
	}
}

//
//	SetVolume...
//	range : [0,100]
//
void CN3SndObj2D::SetVolume(int Vol)
{
	if(m_pDSBuff)
	{
		m_iVol = Vol;
		if(Vol==0)
		{
			m_pDSBuff->SetVolume(-10000);
			return;
		}

		const float fVol = (float)(Vol) / 100.0f;

		const long dwVol = (long)(log10(fVol) * 3000);	// Decibel-related sound control..
		m_pDSBuff->SetVolume(dwVol);
	}
}

bool CN3SndObj2D::IsPlaying() const
{
	if(m_ePlayState != SNDSTATE_STOP) return true;

	DWORD dwStatus = 0;
	m_pDSBuff->GetStatus( &dwStatus );
	if((dwStatus & DSBSTATUS_PLAYING)==DSBSTATUS_PLAYING) return true;
	
	return false;
}

void CN3SndObj2D::Tick()
{
	if(m_ePlayState == SNDSTATE_STOP) return;

	m_fTmpSecPerFrm += CN3Base::s_fSecPerFrm;
	
	if(m_ePlayState==SNDSTATE_DELAY && m_fTmpSecPerFrm >= m_fStartDelayTime)
	{
		m_fTmpSecPerFrm = 0;
		m_ePlayState = SNDSTATE_FADEIN;
		RealPlay();
	}

	if(m_ePlayState==SNDSTATE_FADEIN) 
	{
		if(m_fTmpSecPerFrm >= m_fFadeInTime)
		{
			m_fTmpSecPerFrm = 0;
			m_ePlayState = SNDSTATE_PLAY;
			if(this->m_iType!=SNDTYPE_3D) SetVolume(m_iMaxVolume);
		}
		else
		{
			int vol = 0;
			if(this->m_iType!=SNDTYPE_3D)
			{
				if(m_fFadeInTime>0.0f) vol = ((m_fTmpSecPerFrm/m_fFadeInTime)*(float)m_iMaxVolume);
				SetVolume(vol);
			}			
		}
	}

	if(m_ePlayState==SNDSTATE_PLAY)
	{
		if(!m_bIsLoop) m_ePlayState = SNDSTATE_STOP;		
	}
	if(m_ePlayState==SNDSTATE_FADEOUT)
	{
		if(m_fTmpSecPerFrm >= m_fFadeOutTime)
		{
			m_fTmpSecPerFrm = 0;
			m_ePlayState = SNDSTATE_STOP;
			if(this->m_iType!=SNDTYPE_3D) SetVolume(0);

			HRESULT hr = m_pDSBuff->Stop();
		}
		else
		{
			if(this->m_iType!=SNDTYPE_3D)
			{
				int vol = 0;
				if(m_fFadeOutTime>0.0f)  vol = (((m_fFadeOutTime - m_fTmpSecPerFrm)/m_fFadeOutTime)*(float)m_iMaxVolume);
				SetVolume(vol);
			}			
		}
	}
}

void CN3SndObj2D::Play(const D3DVECTOR* pvPos, float delay, float fFadeInTime, bool bImmediately)
{
	this->SetPos(pvPos);
	if(bImmediately) this->Stop();
	m_fFadeInTime = fFadeInTime;
	m_fFadeOutTime = 0;
	m_fStartDelayTime = delay;
	m_fTmpSecPerFrm = 0;
	m_ePlayState = SNDSTATE_DELAY;
}

void CN3SndObj2D::RealPlay()
{
    if(m_pDSBuff == nullptr) return;

	DWORD dwStatus = 0;
	m_pDSBuff->GetStatus( &dwStatus );
	if((dwStatus & DSBSTATUS_PLAYING)==DSBSTATUS_PLAYING) return;

	HRESULT hr;
	hr = m_pDSBuff->SetCurrentPosition(0);

	if(m_bIsLoop) m_pDSBuff->Play(0,0,DSBPLAY_LOOPING);
	else hr |= m_pDSBuff->Play( 0, 0, 0 );

	return; 
}

void CN3SndObj2D::Stop(float fFadeOutTime)
{
	if( m_pDSBuff == nullptr) return;
	if( m_ePlayState == SNDSTATE_FADEOUT || m_ePlayState == SNDSTATE_STOP) return; 

	if(fFadeOutTime==0.0f)
	{
		m_fTmpSecPerFrm = 0;
		m_fFadeOutTime = fFadeOutTime;
		m_ePlayState = SNDSTATE_STOP;
		HRESULT hr = m_pDSBuff->Stop();
		return;
	}

	m_fTmpSecPerFrm = 0;
	m_fFadeOutTime = fFadeOutTime;
	m_ePlayState = SNDSTATE_FADEOUT;
	return;
}
