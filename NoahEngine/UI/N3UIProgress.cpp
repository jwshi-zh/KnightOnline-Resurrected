#include "pch.h"
#include "N3UIImage.h"
#include "N3UIProgress.h"

CN3UIProgress::CN3UIProgress()
{
	m_eType = UI_TYPE_PROGRESS;

	m_pBkGndImgRef = nullptr;
	m_pFrGndImgRef = nullptr;
	ZeroMemory(&m_frcFrGndImgUV, sizeof(m_frcFrGndImgUV));
	m_iMaxValue = m_iMinValue = 0;
	m_iValueToReach = 0;
	m_fCurValue = 0;
	m_fChangeSpeedPerSec = 0;
	m_fTimeToDelay = 0;					// delay time...
}

CN3UIProgress::~CN3UIProgress()
{
}

void CN3UIProgress::Release()
{
	CN3UIBase::Release();

	m_pBkGndImgRef = nullptr;
	m_pFrGndImgRef = nullptr;
	ZeroMemory(&m_frcFrGndImgUV, sizeof(m_frcFrGndImgUV));
	m_iMaxValue = m_iMinValue = 0;
	m_fCurValue = 0;
	m_fChangeSpeedPerSec = 0;
}

void CN3UIProgress::SetCurValue(int iValue, float fTimeToDelay, float fChangeSpeedPerSec)
{
	if (iValue<m_iMinValue) iValue = m_iMinValue;
	if (iValue>m_iMaxValue) iValue = m_iMaxValue;
	if (fTimeToDelay < 0.0f) fTimeToDelay = 0.0f;
	if (fChangeSpeedPerSec < 0.0f) fChangeSpeedPerSec = 0.0f;

	if (m_iValueToReach == iValue) return;
	
	m_iValueToReach = iValue;
	m_fTimeToDelay = fTimeToDelay; // delay time...
	m_fChangeSpeedPerSec = fChangeSpeedPerSec;
	
	if(0.0f == fTimeToDelay  && 0.0f == fChangeSpeedPerSec) // If there is no delay, set right away..
	{
		m_fCurValue = (float)iValue;
		UpdateFrGndImage();
	}
}

void CN3UIProgress::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	if (m_pBkGndImgRef) m_pBkGndImgRef->SetRegion(Rect);
	UpdateFrGndImage();
}

void CN3UIProgress::SetStyle(DWORD dwStyle)
{
	CN3UIBase::SetStyle(dwStyle);
	UpdateFrGndImage();
}

void CN3UIProgress::UpdateFrGndImage() const
{
	if (nullptr == m_pFrGndImgRef) return;
	const int iDiff = m_iMaxValue - m_iMinValue;
	if (0 == iDiff) return;
	const float fPercentage = ((float)(m_fCurValue - m_iMinValue)) / ((float)(m_iMaxValue - m_iMinValue));

	RECT			rcRegion;
	__FLOAT_RECT	frcUVRect;
	if (m_dwStyle & UISTYLE_PROGRESS_RIGHT2LEFT)
	{
		rcRegion.right = m_rcRegion.right;
		rcRegion.top = m_rcRegion.top;
		rcRegion.bottom = m_rcRegion.bottom;
		rcRegion.left = m_rcRegion.right - ((int)((m_rcRegion.right - m_rcRegion.left)*fPercentage));

		frcUVRect.right = m_frcFrGndImgUV.right;
		frcUVRect.top = m_frcFrGndImgUV.top;
		frcUVRect.bottom = m_frcFrGndImgUV.bottom;
		frcUVRect.left = m_frcFrGndImgUV.right - (m_frcFrGndImgUV.right - m_frcFrGndImgUV.left) *
			((rcRegion.right-rcRegion.left)/((float)(m_rcRegion.right - m_rcRegion.left)));
	}
	else if (m_dwStyle & UISTYLE_PROGRESS_TOP2BOTTOM)
	{
		rcRegion.left = m_rcRegion.left;
		rcRegion.right = m_rcRegion.right;
		rcRegion.top = m_rcRegion.top;
		rcRegion.bottom = m_rcRegion.top + ((int)((m_rcRegion.bottom - m_rcRegion.top)*fPercentage));

		frcUVRect.left = m_frcFrGndImgUV.left;
		frcUVRect.right = m_frcFrGndImgUV.right;
		frcUVRect.top = m_frcFrGndImgUV.top;
		frcUVRect.bottom = m_frcFrGndImgUV.top + (m_frcFrGndImgUV.bottom - m_frcFrGndImgUV.top) *
			((rcRegion.bottom-rcRegion.top)/((float)(m_rcRegion.bottom - m_rcRegion.top)));
	}
	else if (m_dwStyle & UISTYLE_PROGRESS_BOTTOM2TOP)
	{
		rcRegion.left = m_rcRegion.left;
		rcRegion.right = m_rcRegion.right;
		rcRegion.bottom = m_rcRegion.bottom;
		rcRegion.top = m_rcRegion.bottom - ((int)((m_rcRegion.bottom - m_rcRegion.top)*fPercentage));

		frcUVRect.left = m_frcFrGndImgUV.left;
		frcUVRect.right = m_frcFrGndImgUV.right;
		frcUVRect.bottom = m_frcFrGndImgUV.bottom;
		frcUVRect.top = m_frcFrGndImgUV.bottom - (m_frcFrGndImgUV.bottom - m_frcFrGndImgUV.top) *
			((rcRegion.bottom-rcRegion.top)/((float)(m_rcRegion.bottom - m_rcRegion.top)));
	}
	else
	{
		rcRegion.left = m_rcRegion.left;
		rcRegion.top = m_rcRegion.top;
		rcRegion.bottom = m_rcRegion.bottom;
		rcRegion.right = m_rcRegion.left + ((int)((m_rcRegion.right - m_rcRegion.left)*fPercentage));

		frcUVRect.left = m_frcFrGndImgUV.left;
		frcUVRect.top = m_frcFrGndImgUV.top;
		frcUVRect.bottom = m_frcFrGndImgUV.bottom;
		frcUVRect.right = m_frcFrGndImgUV.left + (m_frcFrGndImgUV.right - m_frcFrGndImgUV.left)*
			((rcRegion.right-rcRegion.left)/((float)(m_rcRegion.right - m_rcRegion.left)));
	}

	m_pFrGndImgRef->SetRegion(rcRegion);
	m_pFrGndImgRef->SetUVRect(frcUVRect.left, frcUVRect.top, frcUVRect.right, frcUVRect.bottom);
}

void CN3UIProgress::SetFrGndUVFromFrGndImage()
// Get the uv coordinates from m_pFrGndImgRef and set m_frcFrGndImgUV.
{
	__ASSERT(m_pFrGndImgRef, "not found foreground image in N3UIProgress");
	if (nullptr == m_pFrGndImgRef) return;
	m_frcFrGndImgUV = *(m_pFrGndImgRef->GetUVRect());
	UpdateFrGndImage();
}

bool CN3UIProgress::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	// Setting m_ImageRef
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// Pick out only images
		const int iImageType = (int)(pChild->GetReserved());
		if (iImageType == IMAGETYPE_BKGND)
		{
			m_pBkGndImgRef = (CN3UIImage*)pChild;
		}
		else if (iImageType == IMAGETYPE_FRGND)
		{
			m_pFrGndImgRef = (CN3UIImage*)pChild;
		}
	}

	SetFrGndUVFromFrGndImage();
	return true;
}

#ifdef _N3TOOL
void CN3UIProgress::operator = (const CN3UIProgress& other)
{
	CN3UIBase::operator = (other);

	m_frcFrGndImgUV = other.m_frcFrGndImgUV;				// uv coordinates of m_FrGndImgRef
	m_iMaxValue = other.m_iMaxValue;					// maximum
	m_iMinValue = other.m_iMinValue;					// Ieast
	m_fCurValue = other.m_fCurValue;					// Current value - I set it as a float to gradually increase the value.
	m_fChangeSpeedPerSec = other.m_fChangeSpeedPerSec;			// The speed at which the current value must change. Unit SpeedPerSec
	m_iValueToReach = other.m_iValueToReach;				// Value to be reached - Necessary when the value rises smoothly.
	m_fTimeToDelay = other.m_fTimeToDelay;					// delay time...
	m_iStepValue = other.m_iStepValue;					// Changed value Changed value through StepIt()

	// Setting m_ImageRef
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE != pChild->UIType()) continue;	// Pick out only images
		int iImageType = (int)(pChild->GetReserved());
		if (iImageType == IMAGETYPE_BKGND)
		{
			m_pBkGndImgRef = (CN3UIImage*)pChild;
		}
		else if (iImageType == IMAGETYPE_FRGND)
		{
			m_pFrGndImgRef = (CN3UIImage*)pChild;
		}
	}
	SetFrGndUVFromFrGndImage();
}

bool CN3UIProgress::Save(HANDLE hFile)
{
	int iCur = (int)m_fCurValue;	// remember the previous state
	SetCurValue(m_iMaxValue);	// Creating an image with a full foreground
	bool bRet = CN3UIBase::Save(hFile);	// save

	SetCurValue((int)m_fCurValue);	// revert to previous state

	return bRet;
}

void CN3UIProgress::CreateImages()
{
	m_pBkGndImgRef = new CN3UIImage();
	m_pBkGndImgRef->Init(this);
	m_pBkGndImgRef->SetRegion(m_rcRegion);
	m_pBkGndImgRef->SetReserved(IMAGETYPE_BKGND);	// Specifies that this image is the background image

	m_pFrGndImgRef = new CN3UIImage();
	m_pFrGndImgRef->Init(this);
	m_pFrGndImgRef->SetRegion(m_rcRegion);
	m_pFrGndImgRef->SetReserved(IMAGETYPE_FRGND);	// Specifies that this image is the foreground image

	// put temporarily
	m_iMaxValue = 100;
	m_iMinValue = 0;
	m_fCurValue = 70;
}

void CN3UIProgress::DeleteBkGndImage()	// Background images may not be necessary.
{
	if (m_pBkGndImgRef)	{ delete m_pBkGndImgRef; m_pBkGndImgRef = NULL;}
}

#endif

void CN3UIProgress::Tick()
{
	CN3UIBase::Tick();

	if(m_fTimeToDelay > 0)
	{
		m_fTimeToDelay -= s_fSecPerFrm; // time delay
		if(m_fTimeToDelay < 0) m_fTimeToDelay = 0;
		return;
	}

	if(m_fChangeSpeedPerSec <= 0) return;
	if((int)m_fCurValue == m_iValueToReach) return;
	
	if(m_fCurValue < m_iValueToReach)
	{
		m_fCurValue += m_fChangeSpeedPerSec * s_fSecPerFrm; // Adjust it to go up 30 percent per second.
		if(m_fCurValue > m_iValueToReach) m_fCurValue = (float)m_iValueToReach;
		UpdateFrGndImage();
	}
	else if(m_fCurValue > m_iValueToReach)
	{
		m_fCurValue -= m_fChangeSpeedPerSec * s_fSecPerFrm; // Adjust it to go up 30 percent per second.
		if(m_fCurValue < m_iValueToReach) m_fCurValue = (float)m_iValueToReach;
		UpdateFrGndImage();
	}
}
