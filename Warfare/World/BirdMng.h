#pragma once

#include "N3Base.h"


typedef struct __TABLE_BIRD
{
	DWORD dwID; // zone ID
	char* pszTerrainFN; // Terrain FileName - Terrain, Tile Map, ColorMap....
	char* pszColorMapFN;
	char* pszObjectPostDataFN; // Object ��ġ ���� - �̾ȿ� �浹 üũ �����͵� ��� �ִ�.
} TABLE_BIRD;


class CBird;

class CBirdMng : public CN3Base  
{
public:
	CBirdMng();
	virtual ~CBirdMng();

// Attributes
protected:
	CBird*			m_pBird;
	int				m_iBirdCount;

// Operations
public:
	virtual void	Release();
	void			Tick();
	void			Render();
	void			LoadFromFile(const std::string& szFN);
protected:

};
