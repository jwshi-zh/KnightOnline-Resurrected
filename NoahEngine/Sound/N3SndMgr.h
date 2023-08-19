#pragma once

#include "N3SndDef.h"
#include <list>
#include <map>
#include "N3TableBase.h"

class CN3SndObj;
class CN3SndObjStream;

class CN3SndMgr
{
protected:
	CN3TableBase<__TABLE_SOUND>			m_Tbl_Source; // 사운드 소스 정보 테이블..

	bool								m_bSndEnable;
	bool								m_bSndDuplicated;
	std::map<std::string, CN3SndObj*>	m_SndObjSrcs;
	std::list<CN3SndObjStream*>			m_SndObjStreams;	// 스트리밍 사운드..
	std::list<CN3SndObj*>				m_SndObjs_Duplicated;
	std::list<CN3SndObj*>				m_SndObjs_PlayOnceAndRelease;	// 한번만 플레이 하고 릴리즈 해야 하는 사운드들
	
public:
	void		ReleaseObj(CN3SndObj** ppObj);
	void		ReleaseStreamObj(CN3SndObjStream** ppObj);
//	void		DeleteObjWithSource(CN3SndObj* pObj);
//	CN3SndObj*	GetObj(int iID);
	bool		PlayOnceAndRelease(int iSndID, const D3DVECTOR* pPos = nullptr);
		
	void		SetDuplicated(bool bDuplicate) { m_bSndDuplicated = bDuplicate; }
	bool		GetDuplicated() const { return m_bSndDuplicated; }
	void		SetEnable(bool bEnable) { m_bSndEnable = bEnable; }
	void		Init(HWND hWnd);
	void		Release();
	void		Tick();	

	CN3SndObj*			CreateObj(const std::string& szFN, e_SndType eType = SNDTYPE_3D);
	CN3SndObj*			CreateObj(int iID, e_SndType eType = SNDTYPE_3D);
	CN3SndObjStream*	CreateStreamObj(const std::string& szFN);
	CN3SndObjStream*	CreateStreamObj(int iID);

	CN3SndMgr();
	virtual ~CN3SndMgr();
};
