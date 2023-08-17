#pragma once

#include "N3FXBundle.h"

class CN3FXBundleGame : public CN3FXBundle
{
public:
	int					m_iID;			//�׿��� ���� �Ƶ�..
	int					m_iIdx;
	bool				m_bRegion;

	//������ ��� ����...���ΰ �̿��Դ�..
	float				m_fDistance;
	float				m_fHeight;
	
public:
	void	SetPreBundlePos(int iSourceID, int iJoint);
	void	Trigger(int iSourceID=0, int iTargetID = 0, int iTargetJoint = 0, int iSndID = -1);
	void	Trigger(int iSourceID, __Vector3 TargetPos, int iSndID = -1);
	bool	Tick();
	
	void	Duplicate(CN3FXBundleGame* pDestBundle);
	bool	Load(HANDLE hFile);

	CN3FXBundleGame();
	virtual ~CN3FXBundleGame();

};
