#pragma once

#include "N3Shape.h"

const DWORD AF_POS = 0x00000001;		// Pos 변화가 있다.
const DWORD AF_SCALE = 0x00000002;		// Scale 변화가 있다.
const DWORD AF_ROTATION = 0x00000004;		// Rotation 변화가 있다.

struct __ActionInfo
{
	DWORD				dwActionFlag;			// Pos, Scale, Rot 변화가 있는가?
	std::vector<__Vector3>		ActionStateInfos_Pos;		// 움직일 상태 정보들(이동 정보 있다면 m_iActionStateCount개)
	std::vector<__Vector3>		ActionStateInfos_Scale;	// 확대축소
	std::vector<__Quaternion>	ActionStateInfos_Rot;		// 회전
//	__Vector3			vCurAction_Pos;	// 현재 움직인 상태 정보(tick에 따라 변화한다)
//	__Vector3			vCurAction_Scale;	// 현재 움직인 상태 정보(tick에 따라 변화한다)
//	__Quaternion		qtCurAction_Rot;	// 현재 움직인 상태 정보(tick에 따라 변화한다)
};

class CN3SPartEx : public CN3SPart
{
	friend class CN3ShapeEx;
public:
	CN3SPartEx();
	virtual ~CN3SPartEx();
// Attributes
public:
protected:
	__ActionInfo		m_ActionInfo;	// 움직임에 관한 정보
// Operations
public:
	virtual void Release();
	virtual bool Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool Save(HANDLE hFile);
#endif // end of _N3TOOL

protected:
	void	Interpolate(const __Matrix44& mtxParent, int iCurActionState, int iNextActionState, float fPercentage);
};

class CN3ShapeEx : public CN3Shape  
{
public:
	CN3ShapeEx();
	virtual ~CN3ShapeEx();

// Structures
protected:

// Attributes
public:
protected:
	int		m_iActionStateCount;		// 상태가 몇개가 있는지 나타낸다.
	int		m_iCurActionState;		// 현재 상태
	int		m_iPrevActionState;		// 이전 상태(새로운 상태를 설정해주면 Animation되는 동안 이전상태를 저장해둔다.
	float	m_fActionTimeChanged;		// 상태가 완전히 바뀌는 시간
	float	m_fActionTimeChanging;	// 상태가 바뀌는 시작시간부터 지금까지의 경과 시간

	__ActionInfo		m_ActionInfo;	// 움직임에 관한 정보

// Operations
public:
	BOOL	SetState(int iState, float fSec);	// fSec시간동안 상태를 바꾼다. (fSec이 0일경우 즉시 바뀐다.)
	virtual void	Release();
	virtual void	ReCalcMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual bool	Load(HANDLE hFile);
protected:
};
