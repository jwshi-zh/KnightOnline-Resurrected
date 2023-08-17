#pragma once

#include "N3UIBase.h"

class CUITargetBar       : public CN3UIBase  
{
public:
	class CN3UIProgress*	m_pProgressHP;
	class CN3UIString *		m_pStringID;

	float m_fTimeSendPacketLast; // �̰� �������� 1�ʿ� �ѹ��� Ÿ���� ���� ��û..

public:
	void	UpdateHP(int iHP, int iMaxHP, bool bUpdateImmediately);
	BOOL	SetIDString(const std::string& szID, D3DCOLOR crID);
	
	bool	Load(HANDLE hFile);
	void	Release();
	CUITargetBar();
	virtual ~CUITargetBar();
};
