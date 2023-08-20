#pragma once

#include "N3UIBase.h"

class CN3UIStatic : public CN3UIBase  
{
public:
	CN3UIStatic();
	virtual ~CN3UIStatic();
// Attributes
protected:
	CN3UIString*	m_pBuffOutRef;		// CN3UIString reference pointer (actually managed in child list)
	CN3UIImage*		m_pImageBkGnd;		// Background image reference pointer (actually managed in child list)
	CN3SndObj*		m_pSnd_Click;	// Sound produced when static is pressed
	
// Operations
public:
	virtual const std::string& GetString();
	virtual void	SetString(const std::string& szString);
	virtual void	Release();
	virtual bool	Load(HANDLE);
	virtual void	SetRegion(const RECT& Rect);
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
protected:

#ifdef _N3TOOL
// Functions used only by tools
public:
	virtual	void	operator = (const CN3UIStatic& other);
	virtual bool	Save(HANDLE hFile);
	void			CreateImageAndString();		// Creating a background image and string
	CN3UIImage*		GetImageBkGnd() const {return m_pImageBkGnd;}
	CN3UIString*	GetUIString() const {return m_pBuffOutRef;}
	void			DeleteImage();		// If the static image is not used, it is deleted when called.
	void			SetSndClick(const std::string& strFileName);
	std::string		GetSndFName_Click() const;
#endif
};	
