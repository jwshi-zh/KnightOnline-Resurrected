#pragma once

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"
#include "N3TableBase.h"

#include <list>
#include <map>
#include "N3UIWndBase.h"

class CN3FXBundleGame;

typedef struct __FXBundleOrigin	// When managing parts in a bundle...
{
	class CN3FXBundleGame*	pBundle;
	int						iNum;
	float					fLimitedTime;
	
	__FXBundleOrigin()
	{
		pBundle = nullptr;
		iNum = 0;
		fLimitedTime = 0.0f;
	}
} FXBUNDLEORIGIN, *LPFXBUNDLEORIGIN;

class CN3FXMgr : public CGameBase
{
public:
	// std::list<CN3FXBundleGame*>		m_ListBundle;
	std::list<CN3FXBundleGame*>			m_ListBundle;

	std::map<std::string, LPFXBUNDLEORIGIN>			m_OriginBundle;

	float						m_fOriginLimitedTime;
	
public:
	void	TriggerBundle(int SourceID, int SourceJoint, int FXID, int TargetID, int TargetJoint, int idx = 0, int MoveType = FX_BUNDLE_MOVE_NONE);
	void	TriggerBundle(int SourceID, int SourceJoint, int FXID, __Vector3 TargetPos, int idx = 0, int MoveType = FX_BUNDLE_MOVE_NONE);
	void	Stop(int SourceID, int TargetID, int FXID = -1, int idx = 0, bool immediately = false);
	void	StopMine();
	void	Tick();
	void	Render();
	bool	CheckCollisionSphere(__Vector3 vSp, __Vector3 vEp, __Vector3 vCp, float fRadius, __Vector3* vCol);

	void	SetBundlePos(int FXID, int idx, __Vector3& vPos);

	void	ClearAll();		
	CN3FXMgr();	
	virtual ~CN3FXMgr();
};
