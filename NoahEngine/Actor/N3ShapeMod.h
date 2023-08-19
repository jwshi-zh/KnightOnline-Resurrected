#pragma once

#include "N3Shape.h"

class CN3ShapeMod : public CN3Shape  
{
public:
	CN3ShapeMod();
	virtual ~CN3ShapeMod();

// Structures
protected:
	struct __ModPosRotScale
	{
		__Vector3 vPos;
		__Quaternion qRot;
		__Vector3 vScale;

		__ModPosRotScale() {vPos.Set(0,0,0); qRot.Set(0,0,0,0); vScale.Set(1,1,1);}
	};
	struct __ModPart
	{
		CN3SPart*			pPart;				// Part to move
		bool				bPos;				// Is there a Pos change?
		bool				bRot;				// Is there a Rot change?
		bool				bScale;				// Is there a scale change?
		__ModPosRotScale*	pStateInfos;		// State information to move (move, rotate, scale information m_iStateCount pieces)
		__ModPosRotScale	CurStateInfo;		// Current moving state information (changes according to tick)

		__ModPart() {pPart = nullptr; pStateInfos = nullptr; bPos = false; bRot = false; bScale = false; }
		~__ModPart() {pPart = nullptr; if (pStateInfos) { delete [] pStateInfos; pStateInfos = nullptr;}}
	};

	struct __ModShape
	{
		bool				bShapePos;
		bool				bShapeRot;
		bool				bShapeScale;		// Does the shape have position, rotation, or scale changes?
		__ModPosRotScale*	pShapeStateInfos;	// Change information of the entire shape
		__ModPosRotScale	CurShapeStateInfo;	// Current moving state information (changes according to tick)
		__ModShape() {pShapeStateInfos = nullptr; bShapePos = false; bShapeRot = false; bShapeScale = false;}
		~__ModShape() {if (pShapeStateInfos) {delete [] pShapeStateInfos; pShapeStateInfos = nullptr;}}
	};

// Attributes
public:
protected:
//	enum	{
//			N3SHAPEMOD_TYPE_NOT_MOD = 0,		// no information changes
//			N3SHAPEMOD_TYPE_ONLY_SHAPE = 1,		// Changing only the shape information
//			N3SHAPEMOD_TYPE_ONLY_PART = 2,		// Changing only part information
//			N3SHAPEMOD_TYPE_NORMAL = 4			// both change
//	};
//	int		m_iModType;			// type..
	int		m_iStateCount;		// Indicates how many states there are. Indicates how many states there are.
	int		m_iCurState;		// Current Status
	int		m_iPrevState;		// Previous state (If you set a new state, the previous state is saved during animation.
	float	m_fTimeChanged;		// Time to completely change state
	float	m_fTimeChanging;	// Elapsed time from the start time of the state change to now

	__ModShape	m_ModShape;			// State change information of shape
	int			m_iModPartCount;		// The number of parts that change
	__ModPart*	m_pModParts;		// Changing parts information

	__ModPart**	m_pMatchPart2ModPart;		// Array matched with __ModPart* in order of CPart list arrangement

// Operations
public:
	BOOL	SetState(int iState, float fSec);	// Change state during fSec time. (If fSec is 0, it changes immediately.)
	BOOL	LoadStateInfo(FILE* stream);	// Read state information (from text)
	virtual void	Release();
	virtual void	ReCalcMatrix();
	virtual void	Tick(float fFrm = FRAME_SELFPLAY);
	virtual bool	Load(HANDLE hFile);
protected:
	CN3SPart* GetPartByPMeshFileName(const std::string& szFN) const;	// Get PMesh pointer by name

};
