#pragma once

#include "MachineBase.h"

class CCatapult : public CMachineBase
{
public:
	CCatapult();
	virtual ~CCatapult();

// Structure
public:
	struct __Thrower
	{
		// Information read from file
		CN3SPart*	pThrowerPart;			// Thrower's part pointer
		float	fLimitRadian;			// limit rotation angle
		float	fRadianAccel;			// rotational acceleration
		float	fRadianSpeed2Reload;	// reload rotation speed
		float	fReloadDelayTime;		// Time delay between full fire and start of reload
		float	fRecoilRadian;			// Angle of recoil after firing
		class CN3Shape*	pStone;			// Rock shape pointer to fire
		__Vector3		vStoneOffset;	// Boulder offset (distance from the thrower's pivot point)

		// Information created based on information read from a file
		float	fReleaseTime;			// Time to reach limit rotation angle after launch [ sqrtf(fLimitRadian/fRadianAccel) ]

		// INFORMATION USED DURING THE GAME
		float	fTime;					// Elapsed firing time (seconds) Accumulated based on firing time zero.
		BOOL	bFire;					// Are you firing?
		float	fCurRadian;				// Rotation angle of the current Thrower
		BOOL	bDontRenderStone;		// Will you not draw the launching stones?
	};
// Attributes
public:
protected:
	__Thrower		m_Thrower;			// launching pad

// Operations
public:
	virtual void	Release();
	virtual void	Tick(float fFrm);
	virtual void	LoadMachine(FILE* stream);
	virtual void	Render();
protected:
	virtual void	ReCalcMatrix4AnimatedPart();
	void			ThrowStone(__Vector3& vPos, __Vector3& vVel, CN3Shape* pShape) const;

// related to machine motion
public:
	virtual void	Fire();		// launch
};
