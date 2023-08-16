#pragma once

#include "N3UIBase.h"

class CN3UIStatic : public CN3UIBase  
{
public:
	CN3UIStatic();
	virtual ~CN3UIStatic();
// Attributes
protected:
	CN3UIString*	m_pBuffOutRef;		// CN3UIString ���� ������ (������ child����Ʈ���� ����)
	CN3UIImage*		m_pImageBkGnd;		// ��� �̹��� ���� ������ (������ child����Ʈ���� ����)
	CN3SndObj*		m_pSnd_Click;	// static�� ������ ���� ���� �Ҹ�
	
// Operations
public:
	virtual const std::string& GetString();
	virtual void	SetString(const std::string& szString);
	virtual void	Release();
//	virtual void	Render();
	virtual bool	Load(HANDLE);
	virtual void	SetRegion(const RECT& Rect);
	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
protected:

#ifdef _N3TOOL
// �������� ����ϴ� �Լ�
public:
	virtual	void	operator = (const CN3UIStatic& other);
	virtual bool	Save(HANDLE hFile);
	void			CreateImageAndString();		// ��� �̹��� �� string �����ϱ�
	CN3UIImage*		GetImageBkGnd() const {return m_pImageBkGnd;}
	CN3UIString*	GetUIString() const {return m_pBuffOutRef;}
	void			DeleteImage();		// �̹����� ������� �ʴ� static�� ��� ȣ���ϸ� ��������.
	void			SetSndClick(const std::string& strFileName);
	std::string		GetSndFName_Click() const;
#endif
};	
