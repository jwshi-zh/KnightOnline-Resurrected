#include "pch.h"
#include "N3SndMgr.h"
#include "N3SndObj.h"
#include "N3SndObjStream.h"
#include "N3Base.h"

CN3SndMgr::CN3SndMgr()
{
	m_bSndEnable = false;	
	m_bSndDuplicated = false;
}

CN3SndMgr::~CN3SndMgr()
{
	Release();
}

void CN3SndMgr::Init(HWND hWnd)
{
	Release();
	m_bSndEnable = CN3SndObj::StaticInit(hWnd);
	m_Tbl_Source.LoadFromFile("Data\\sound.tbl");
}

CN3SndObj* CN3SndMgr::CreateObj(int iID, e_SndType eType)
{
	const TABLE_SOUND* pTbl = m_Tbl_Source.Find(iID);
	if(pTbl== nullptr) return nullptr;

	return this->CreateObj(pTbl->szFN, eType);
}

CN3SndObj* CN3SndMgr::CreateObj(const std::string& szFN, e_SndType eType)
{
	if(!m_bSndEnable) return nullptr;

	CN3SndObj* pObjSrc = nullptr;
	const auto it = m_SndObjSrcs.find(szFN);
	if(it == m_SndObjSrcs.end())
	{
		pObjSrc = new CN3SndObj();
		if(false == pObjSrc->Create(szFN, eType))
		{
			delete pObjSrc; pObjSrc = nullptr;
			return nullptr;
		}
		m_SndObjSrcs.insert(std::make_pair(szFN, pObjSrc));
	}
	else pObjSrc = it->second;

	if(!m_bSndDuplicated) return pObjSrc;

	if(nullptr == pObjSrc) return nullptr;

	auto* pObjNew = new CN3SndObj();
	if(false == pObjNew->Duplicate(pObjSrc, eType))
	{
		delete pObjNew; pObjNew = nullptr;
		return nullptr;
	}
	
	if(pObjNew) m_SndObjs_Duplicated.push_back(pObjNew);
	return pObjNew;
}

CN3SndObjStream* CN3SndMgr::CreateStreamObj(const std::string& szFN)
{
	auto* pObj = new CN3SndObjStream();
	if(false == pObj->Create(szFN))
	{
		delete pObj; pObj = nullptr;
		return nullptr;
	}

	m_SndObjStreams.push_back(pObj);

	return pObj;
}

CN3SndObjStream* CN3SndMgr::CreateStreamObj(int iID)
{
	const TABLE_SOUND* pTbl = m_Tbl_Source.Find(iID);
	if(pTbl== nullptr) return nullptr;

	return this->CreateStreamObj(pTbl->szFN);
}

void CN3SndMgr::ReleaseStreamObj(CN3SndObjStream** ppObj)
{
	if(nullptr == ppObj || nullptr == *ppObj) return;

	auto it = m_SndObjStreams.begin(), itEnd = m_SndObjStreams.end();
	for(; it != itEnd; it++)
	{
		if(*ppObj == *it) 
		{
			delete *ppObj; *ppObj = nullptr;
			m_SndObjStreams.erase(it);
			break;
		}
	}
}

void CN3SndMgr::Tick()
{
	if(!m_bSndEnable) return;

//	m_Eng.SetListenerPos(&(CN3Base::s_CameraData.vEye));
//	__Vector3 vUP(0.0f, 1.0f, 0.0f);
//	__Vector3 vDir = CN3Base::s_CameraData.vAt - CN3Base::s_CameraData.vEye;
//
//	if(vDir.Magnitude() <= FLT_MIN) vDir.Set(0.0f, 0.0f, 1.0f);
//
//	m_Eng.SetListenerOrientation(&vDir, &vUP);
//

/*
	CN3SndObj* pObj = NULL;
	itl_Snd it = m_SndObjs_Duplicated.begin(), itEnd = m_SndObjs_Duplicated.end();
	for(; it != itEnd; it++)
	{
		pObj = *it;
		pObj->Tick();
	}
*/
	CN3SndObj* pObj = nullptr;
	if(!m_bSndDuplicated)
	{
		auto it_m = m_SndObjSrcs.begin(), itEnd_m = m_SndObjSrcs.end();
		for(; it_m != itEnd_m; it_m++)
		{
			pObj = it_m->second;
			pObj->Tick();
		}
	}
	else
	{
		const auto itEnd = m_SndObjs_Duplicated.end();
		for(auto it = m_SndObjs_Duplicated.begin(); it != itEnd; it++)
		{
			pObj = *it;
			pObj->Tick();
		}
	}

	const auto itEnd = m_SndObjs_PlayOnceAndRelease.end();
	for(auto it = m_SndObjs_PlayOnceAndRelease.begin(); it != itEnd; )
	{
		pObj = *it;
		pObj->Tick();
		if(false == pObj->IsPlaying())
		{
			it = m_SndObjs_PlayOnceAndRelease.erase(it);
			delete pObj; pObj = nullptr;
		}
		else it++;
	}

	auto it2 = m_SndObjStreams.begin(), itEnd2 = m_SndObjStreams.end();
	for(; it2 != itEnd2; it2++)
	{
		CN3SndObjStream* pObj2 = *it2;
		if (pObj2) pObj2->Tick();
	}

//	itm_Snd it2 = m_SndObjSrcs.begin();
//	for(; it2 != m_SndObjSrcs.end(); it2++)
//	{
//		pObj = it2->second.pSndObj;
//		if(pObj) pObj->Tick();
//	}

	CN3SndObj::StaticTick(); // CommitDeferredSetting...
}

void CN3SndMgr::ReleaseObj(CN3SndObj** ppObj)
{
	if(nullptr == ppObj || nullptr == *ppObj) return;
	std::string szFN = (*ppObj)->m_szFileName;

	auto it = m_SndObjs_Duplicated.begin(), itEnd = m_SndObjs_Duplicated.end();
	for(; it != itEnd; it++)
	{
		if(*ppObj == *it)
		{
			m_SndObjs_Duplicated.erase(it);
			delete *ppObj; *ppObj = nullptr;
			return;
		}
	}

	it = m_SndObjs_PlayOnceAndRelease.begin();
	itEnd = m_SndObjs_PlayOnceAndRelease.end();
	for(; it != itEnd; it++)
	{
		if(*ppObj == *it)
		{
			m_SndObjs_PlayOnceAndRelease.erase(it);
			delete *ppObj; *ppObj = nullptr;
			return;
		}
	}

	*ppObj = nullptr;

/*	itm_Snd it = m_SndObjSrcs.find(szFN);
	if(it != m_SndObjSrcs.end()) // 찾았다..
	{
		CN3SndObj* pObj = it->second;
		delete pObj;
		m_SndObjSrcs.erase(it);
	}
	else
	{
		itl_Snd it2 = m_SndObjs_PlayOnceAndRelease.begin();
		for(; it2 != m_SndObjs_PlayOnceAndRelease.end(); it2++)
		{
			CN3SndObj* pObj = *it2;
			if(pObj == *ppObj)
			{
				delete pObj;
				m_SndObjs_PlayOnceAndRelease.erase(it2);
				break;
			}		
		}
	}
*/
}

void CN3SndMgr::Release()
{
	if(!m_bSndEnable) return;

	const CN3SndObj* pObj = nullptr;
	auto it = m_SndObjSrcs.begin(), itEnd = m_SndObjSrcs.end();
	for(; it != itEnd; it++)
	{
		pObj = it->second;
		if(pObj) delete pObj;
	}
	m_SndObjSrcs.clear();

	auto it2 = m_SndObjs_Duplicated.begin(), itEnd2 = m_SndObjs_Duplicated.end();
	for(; it2 != itEnd2; it2++)
	{
		pObj = *it2;
		if(pObj) delete pObj;
	}
	m_SndObjs_Duplicated.clear();

	it2 = m_SndObjs_PlayOnceAndRelease.begin();
	itEnd2 = m_SndObjs_PlayOnceAndRelease.end();
	for(; it2 != itEnd2; it2++)
	{
		pObj = *it2;
		if(pObj) delete pObj;
	}
	m_SndObjs_PlayOnceAndRelease.clear();

	auto it3 = m_SndObjStreams.begin(), itEnd3 = m_SndObjStreams.end();
	for(; it3 != itEnd3; it3++)
	{
		const CN3SndObjStream* pObj2 = *it3;
		if(pObj2) delete pObj2;
	}
	m_SndObjStreams.clear();

	CN3SndObj::StaticRelease();
}

bool CN3SndMgr::PlayOnceAndRelease(int iSndID, const _D3DVECTOR* pPos)
{
	if(!m_bSndEnable) return false;

	TABLE_SOUND* pTbl = m_Tbl_Source.Find(iSndID);
	if(pTbl== nullptr || pTbl->szFN.empty()) return false;
	
	CN3SndObj* pObjSrc = nullptr;
	const auto it = m_SndObjSrcs.find(pTbl->szFN);
	if(it == m_SndObjSrcs.end())
	{
		pObjSrc = new CN3SndObj();
		if(false == pObjSrc->Create(pTbl->szFN, (e_SndType)pTbl->iType))
		{
			delete pObjSrc; pObjSrc = nullptr;
			return NULL;
		}
		m_SndObjSrcs.insert(std::make_pair(pTbl->szFN, pObjSrc));
		if(!m_bSndDuplicated) pObjSrc->Play(pPos);
	}
	else pObjSrc = it->second;

	if(nullptr == pObjSrc) return false;

	if(!m_bSndDuplicated)
	{
		pObjSrc->Play(pPos);
		return true;
	}

	auto* pObj = new CN3SndObj();
	if(false == pObj->Duplicate(pObjSrc, (e_SndType)pTbl->iType))
	{
		delete pObj; pObj = nullptr;
		return NULL;
	}
	
	if(pObj)
	{
		m_SndObjs_PlayOnceAndRelease.push_back(pObj);
		pObj->Play(pPos);
		return true;
	}
	return false;
/*
	CN3SndObj* pObj = new CN3SndObj();
	if(false == pObj->Create(pTbl->szFN, (e_SndType)pTbl->iType))
	{
		delete pObj; pObj = NULL;
		return false;
	}
	pObj->Play(pPos);
	m_SndObjs_PlayOnceAndRelease.push_back(pObj);
	return true;
*/
}