#pragma once

class CEventCell
{
public:
	short	m_sEventType;
	RECT	m_Rect;

	void	Load(HANDLE hFile);
	CEventCell();
	virtual ~CEventCell();

};

class CEventManager  
{
private:
	std::list<CEventCell*>	m_lstEvents;
	short		m_sEventType;
	RECT		m_rcEvent;

public:
	void Behavior(short sEventType, short sPreEventType);
	bool PtInRect(int x, int z, RECT rc);
	short SetPos(float fX, float fZ);
	void Release();
	bool LoadFromFile(const char* szFileName);
	CEventManager();
	virtual ~CEventManager();

};
