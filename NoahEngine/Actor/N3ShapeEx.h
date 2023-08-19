#pragma once

#include "N3Shape.h"

const DWORD AF_POS = 0x00000001;
const DWORD AF_SCALE = 0x00000002;
const DWORD AF_ROTATION = 0x00000004;

struct __ActionInfo
{
	DWORD				dwActionFlag;			// Are there changes in Pos, Scale, Rot?
	std::vector<__Vector3>		ActionStateInfos_Pos;		// State information to move (m_iActionStateCount pieces if there is movement information)
	std::vector<__Vector3>		ActionStateInfos_Scale;	// Zoom in zoom out
	std::vector<__Quaternion>	ActionStateInfos_Rot;		// rotation
//	__Vector3			vCurAction_Pos;	// Current moving state information (changes according to tick)
//	__Vector3			vCurAction_Scale;	// Current moving state information (changes according to tick)
//	__Quaternion		qtCurAction_Rot;	// Current moving state information (changes according to tick)
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
	__ActionInfo		m_ActionInfo;	// information about movement
// Operations
public:
	virtual void Release();
	virtual bool Load(HANDLE hFile);
#ifdef _N3TOOL
	virtual bool Save(HANDLE hFile);
#endif // end of _N3TOOL

protected:
	void	Interpolate(const __Matrix44& mtxParent, int iCurActionState, int iNextActionState, float fPercentage) const;
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
	int		m_iActionStateCount;		// Indicates how many states there are.
	int		m_iCurActionState;		// Current Status
	int		m_iPrevActionState;		// Previous state (If you set a new state, the previous state is saved during animation.
	float	m_fActionTimeChanged;		// Time to completely change state
	float	m_fActionTimeChanging;	// Elapsed time from the start time of the state change to now

	__ActionInfo		m_ActionInfo;	// information about movement

// Operations
public:
	BOOL	SetState(int iState, float fSec);	// Change state during fSec time. (If fSec is 0, it changes immediately.)
	virtual void	Release();
	virtual void	ReCalcMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual bool	Load(HANDLE hFile);
protected:
};
