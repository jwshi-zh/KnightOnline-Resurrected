#pragma once

#include "N3UIBase.h"

class CN3UIImage;
class CN3SndObj;
class CN3UIButton : public CN3UIBase  
{
public:
	CN3UIButton();
	virtual ~CN3UIButton();

// Attributes
public:
	enum eBTN_STATE	{BS_NORMAL=0, BS_DOWN, BS_ON, BS_DISABLE, NUM_BTN_STATE};	// button state
	void			SetClickRect(const RECT& Rect) {m_rcClick = Rect;}
	RECT			GetClickRect() const {return m_rcClick;}
protected:
	CN3UIImage*		m_ImageRef[NUM_BTN_STATE];	// ��ư�� �� ���º� image�� ���� ������(������ ������  children list�� �����ϹǷ� ������ �Ѵ�.)
	RECT			m_rcClick;					// click�Ǵ� ����

	CN3SndObj*		m_pSnd_On;		// ��ư ���� ���콺�� �ö󰡴� ���� ���� �Ҹ�
	CN3SndObj*		m_pSnd_Click;	// ��ư�� ������ ���� ���� �Ҹ�

// Operations
public:
	virtual bool	Load(HANDLE hFile);
	virtual void	Release();
	virtual void	SetRegion(const RECT& Rect);
	virtual BOOL	MoveOffset(int iOffsetX, int iOffsetY);

	virtual DWORD	MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	virtual void	Render();

#ifdef _N3TOOL
// ������ ����ϱ� ���� �Լ�
public:
	virtual void	operator = (const CN3UIButton& other);
	virtual bool	Save(HANDLE hFile);
	void			CreateImages();
	CN3UIImage*		GetImageRef(eBTN_STATE eState) const {	return m_ImageRef[eState];}
	void			SetSndOn(const std::string& strFileName);
	void			SetSndClick(const std::string& strFileName);

	std::string GetSndFName_On() const;
	std::string GetSndFName_Click() const;
#endif
};
