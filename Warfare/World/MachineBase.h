#pragma once

const DWORD MS_STOP		= 0x00000000;
const DWORD MS_FORWARD	= 0x00000001;
const DWORD MS_BACKWARD = 0x00000002;
const DWORD MS_TURNRIGHT= 0x00000004;
const DWORD MS_TURNLEFT = 0x00000008;

const DWORD MACHINE_TYPE_CATAPULT = 0x21;

class CN3Texture;
class CN3PMeshInstance;

#include "N3Shape.h"

class CMachineBase : public CN3Shape
{
public:
	CMachineBase();
	virtual ~CMachineBase();

// Structure
public:
	enum {WHEEL_FL=0, WHEEL_FR=1, WHEEL_BL=2, WHEEL_BR=3, NUM_WHEEL=4};
	struct __Wheel
	{
		// wheel information
		// __Vector3			m_vPivot;	// Local 축
		CN3SPart*				pPart;	// Shape wheel part pointer

		// Values calculated after inputting wheel information.
		float				fRadius;	// wheel radius
		float				fRotateRatio;// The degree of rotation of the wheels (rad) when the machine rotates 1.0 f (rad)

		// Information that changes as the game progresses.
		float				fRadian;	// The angle of rotation of the current wheel. (Rotate on the x-axis.)
	};

	std::string 	m_szID;				// id string

protected:
	__Wheel			m_Wheel[NUM_WHEEL];
	float			m_fSpeed;			// Movement speed per second (forward and backward)
	float			m_fRotateSpeed;		// angular velocity of rotation per second
	// __Vector3 m_vDir; // direction of machine
	float			m_fDirRadian;		// forward angle (0.0f in positive z direction);
	DWORD			m_dwMachineState;
	BOOL*			m_bSkipCalcPartMtx;		// Skip part metrics calculation (allocated as much as the number of parts)
	__Vector3		m_vBalancePoint[4];		// The position of 4 points (front, rear, left and right) for balancing is local coordinates.

	float			m_fFireRadian;		// launch angle
	float			m_fFireSpeed;		// Rate of fire (force)

// Attributes
public:

	void			SetFireRadian(float fFireRadian) {m_fFireRadian = fFireRadian;}
	void			SetFireSpeed(float fFireSpeed) {m_fFireSpeed = fFireSpeed;}
	void			SetDirRadian(float fDirRadian) {m_fDirRadian = fDirRadian; ReCalcMatrix();}
	DWORD			GetMachineState() const {return m_dwMachineState;}

// Operations
public:
	virtual void	Release();
	virtual void	Render();
	virtual void	ReCalcMatrix();
	virtual void	Tick(float fFrm);
	virtual void	LoadMachine(FILE* stream);

protected:
	CN3SPart*		GetPartByPMeshName(const std::string& szName) const;
	virtual void	ReCalcMatrix4AnimatedPart();
// related to machine motion
public:
	void Stop() {m_dwMachineState = MS_STOP;}
	void ToggleMoveForward(BOOL bOn) {if (bOn) m_dwMachineState |= MS_FORWARD; else m_dwMachineState &= (~MS_FORWARD);}
	void ToggleMoveBackward(BOOL bOn) {if (bOn) m_dwMachineState |= MS_BACKWARD; else m_dwMachineState &= (~MS_BACKWARD);}
	void ToggleTurnRight(BOOL bOn) {if (bOn) m_dwMachineState |= MS_TURNRIGHT; else m_dwMachineState &= (~MS_TURNRIGHT);}
	void ToggleTurnLeft(BOOL bOn) {if (bOn) m_dwMachineState |= MS_TURNLEFT; else m_dwMachineState &= (~MS_TURNLEFT);}

	virtual void Fire();		// launch
};
