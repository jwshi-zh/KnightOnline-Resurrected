#pragma once

#include "N3UIBase.h"

class CN3UIImage;
class CN3UIProgress : public CN3UIBase  
{
#ifdef _N3TOOL
friend class CPropertyView;	// To access each variable in the tool
#endif

public:
	CN3UIProgress();
	virtual ~CN3UIProgress();
// Attributes
public:
	enum eIMAGE_TYPE{ IMAGETYPE_BKGND=0, IMAGETYPE_FRGND, NUM_IMAGETYPE};
	int				GetMax() const {return m_iMaxValue;}
	int				GetMin() const {return m_iMinValue;}
	int				GetCurValue() const {return (int)m_fCurValue;}

	void			SetStepValue(int iValue) {m_iStepValue = iValue;}
	void			StepIt() {SetCurValue((int)m_fCurValue + m_iStepValue);}
protected:
	CN3UIImage*		m_pBkGndImgRef;
	CN3UIImage*		m_pFrGndImgRef;

	__FLOAT_RECT	m_frcFrGndImgUV;				// uv coordinates of m_FrGndImgRef

	int				m_iMaxValue;					// maximum
	int				m_iMinValue;					// Ieast
	float			m_fCurValue;					// Current value - I set it as a float to gradually increase the value.
	float			m_fChangeSpeedPerSec;			// The speed at which the current value must change. Unit SpeedPerSec
	int				m_iValueToReach;				// Value to be reached - Necessary when the value rises smoothly.
	float			m_fTimeToDelay;					// delay time...
	
	int				m_iStepValue;					// Changed value Changed value through StepIt()
// Operations
public:
	virtual void	Tick();
// virtual void	Render();
	virtual void	Release();
	virtual void	SetRegion(const RECT& Rect);
	virtual void	SetStyle(DWORD dwStyle);
	virtual bool	Load(HANDLE hFile);

	void			SetCurValue(int iValue, float fTimeToDelay = 0, float fChangeSpeedPerSec = 0);
	void			SetCurValue_Smoothly(int iValue, float fPercentPerSec){SetCurValue(iValue, 0, fPercentPerSec*(m_iMaxValue-m_iMinValue));}	// It goes up at a few percent per second.
	void			SetRange(int iMin, int iMax) {m_iMinValue = iMin; m_iMaxValue = iMax; UpdateFrGndImage();}
	void			SetFrGndUVFromFrGndImage();	// Get the uv coordinates from m_pFrGndImgRef and set m_frcFrGndImgUV.
protected:
	void			UpdateFrGndImage() const;	// Change the area and uv coordinates of m_FrGndImgRef appropriately according to m_fCurValue.

#ifdef _N3TOOL
// Functions used by the tool
public:
	virtual void	operator = (const CN3UIProgress& other);
	virtual bool	Save(HANDLE hFile);
	CN3UIImage*		GetBkGndImgRef() const {return m_pBkGndImgRef;}
	CN3UIImage*		GetFrGndImgRef() const {return m_pFrGndImgRef;}
	void			CreateImages();		// create image
	void			DeleteBkGndImage();	// Background images may not be necessary.
#endif
};
