#include "pch.h"
#include "N3UIStatic.h"
#include "N3UIString.h"
#include "N3UIImage.h"

#include "N3SndMgr.h"
#include "N3SndObj.h"

CN3UIStatic::CN3UIStatic()
{
	m_eType = UI_TYPE_STATIC;

	m_pBuffOutRef = nullptr;
	m_pImageBkGnd = nullptr;
	m_pSnd_Click = nullptr;
}

CN3UIStatic::~CN3UIStatic()
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

void CN3UIStatic::Release()
{
	CN3UIBase::Release();
	m_pBuffOutRef = nullptr;
	m_pImageBkGnd = nullptr;
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
}

void CN3UIStatic::SetRegion(const RECT& Rect)
{
	CN3UIBase::SetRegion(Rect);
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		(*itor)->SetRegion(Rect);
	}
}

bool CN3UIStatic::Load(HANDLE hFile)
{
	if (false == CN3UIBase::Load(hFile)) return false;

	// Setting m_pImageBkGnd, m_pBuffOutRef
	for(auto itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE == pChild->UIType())
		{
			m_pImageBkGnd = (CN3UIImage*)pChild;
		}
		else if (UI_TYPE_STRING == pChild->UIType())
		{
			m_pBuffOutRef = (CN3UIString*)pChild;
		}
	}
	
	// To convert the old uif file, block the sound loading part
	int iSndFNLen = 0;
	DWORD dwNum;
	ReadFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, nullptr);		// sound file string length
	if (iSndFNLen>0)
	{
		std::vector<char> buffer(iSndFNLen+1, NULL);
		ReadFile(hFile, buffer.data(), iSndFNLen, &dwNum, nullptr);

		__ASSERT(NULL == m_pSnd_Click, "memory leak");
		m_pSnd_Click = s_SndMgr.CreateObj(buffer.data(), SNDTYPE_2D);
	}

	return true;
}

const std::string& CN3UIStatic::GetString()
{
	if (m_pBuffOutRef) return m_pBuffOutRef->GetString();
	else return s_szStringTmp;
}

void CN3UIStatic::SetString(const std::string& szString)
{
	if (m_pBuffOutRef) m_pBuffOutRef->SetString(szString);
}


DWORD CN3UIStatic::MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld)
{
	DWORD dwRet = UI_MOUSEPROC_NONE;
	if (!m_bVisible) return dwRet;

	if((dwFlags & UI_MOUSE_LBCLICK) && IsIn(ptCur.x, ptCur.y))  // If it is inside the area at the moment the left button is pressed
	{
		if (m_pSnd_Click) m_pSnd_Click->Play();	// play with sound

		if(m_pParent) m_pParent->ReceiveMessage(this, UIMSG_BUTTON_CLICK); // Notify parent on button click..
		dwRet |= (UI_MOUSEPROC_DONESOMETHING|UI_MOUSEPROC_INREGION);
		return dwRet;
	}

	dwRet |= CN3UIBase::MouseProc(dwFlags, ptCur, ptOld);
	return dwRet;
}

#ifdef _N3TOOL
void CN3UIStatic::operator = (const CN3UIStatic& other)
{
	CN3UIBase::operator = (other);

	SetSndClick(other.GetSndFName_Click());

	// Setting m_pImageBkGnd, m_pBuffOutRef
	for(UIListItor itor = m_Children.begin(); m_Children.end() != itor; ++itor)
	{
		CN3UIBase* pChild = (*itor);
		if (UI_TYPE_IMAGE == pChild->UIType())
		{
			m_pImageBkGnd = (CN3UIImage*)pChild;
		}
		else if (UI_TYPE_STRING == pChild->UIType())
		{
			m_pBuffOutRef = (CN3UIString*)pChild;
		}
	}
}

bool CN3UIStatic::Save(HANDLE hFile)
{
	if (false == CN3UIBase::Save(hFile)) return false;

	DWORD dwNum;
	int iSndFNLen = 0;
	if (m_pSnd_Click) iSndFNLen = m_pSnd_Click->m_szFileName.size();
	WriteFile(hFile, &iSndFNLen, sizeof(iSndFNLen), &dwNum, NULL);		// sound file string length
	if (iSndFNLen>0) WriteFile(hFile, m_pSnd_Click->m_szFileName.c_str(), iSndFNLen, &dwNum, NULL);
	return true;
}

void CN3UIStatic::CreateImageAndString()
{
	m_pImageBkGnd = new CN3UIImage();
	m_pImageBkGnd->Init(this);
	m_pImageBkGnd->SetRegion(m_rcRegion);

	m_pBuffOutRef = new CN3UIString();		// Create a ui string to display on the screen
	m_pBuffOutRef->Init(this);				// Initialization (automatically enters the children list)
	m_pImageBkGnd->SetRegion(m_rcRegion);
}

void CN3UIStatic::DeleteImage()
{
	if (m_pImageBkGnd) {delete m_pImageBkGnd; m_pImageBkGnd = NULL;}
}

void CN3UIStatic::SetSndClick(const std::string& strFileName)
{
	CN3Base::s_SndMgr.ReleaseObj(&m_pSnd_Click);
	if (0 == strFileName.size()) return;

	CN3BaseFileAccess tmpBase;
	tmpBase.FileNameSet(strFileName);	// Passes a relative path to the base path.

	SetCurrentDirectory(tmpBase.PathGet().c_str());
	m_pSnd_Click = s_SndMgr.CreateObj(tmpBase.FileName(), SNDTYPE_2D);
}

std::string CN3UIStatic::GetSndFName_Click() const
{
	if (m_pSnd_Click) return m_pSnd_Click->m_szFileName;
	else return std::string("");
}
#endif
