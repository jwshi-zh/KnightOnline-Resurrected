#pragma once

#include "N3UIBase.h"

struct __PositionInfo
{
	__Vector3	vPos;
	int			iID;
	D3DCOLOR	crType; // Color according to type...
};

struct __DurationMagicImg
{
	DWORD		dwSkillID;
	class CN3UIDBCLButton* pIcon;
	float		fDuration;
};

class CUIStateBar : public CN3UIBase  
{
protected:
	CN3UIString*	m_pText_Position;
	CN3UIProgress*	m_pProgress_HP;
	CN3UIProgress*	m_pProgress_MSP;
	CN3UIProgress*	m_pProgress_ExpC;
	CN3UIProgress*	m_pProgress_ExpP;

	// mini map...
	CN3UIBase*		m_pGroup_MiniMap;
	CN3UIImage*		m_pImage_Map; // Apply a minimap texture to this image.
	CN3UIButton*	m_pBtn_ZoomIn;
	CN3UIButton*	m_pBtn_ZoomOut;

	float			m_fZoom; // Magnification of the map..
	float			m_fMapSizeX;
	float			m_fMapSizeZ;
	float			m_fYawPlayer;
	__Vector3		m_vPosPlayer;

	__VertexTransformedColor	m_vArrows[6]; // Player position arrow.
	std::list<__PositionInfo>	m_Positions;
	std::list<__PositionInfo>	m_PositionsTop; // Place to draw on top.

	// Displaying colored magic skills..
	std::list<__DurationMagicImg*> m_pMagic;
	
// Attributes
public:
	bool OnKeyPress(int iKey);
	DWORD MouseProc(DWORD dwFlags, const POINT& ptCur, const POINT& ptOld);
	void	AddMagic(__TABLE_UPC_SKILL* pSkill, float fDuration);
	void	DelMagic(__TABLE_UPC_SKILL* pSkill);
	void	ClearMagic();
	void	TickMagicIcon();
	void	TickMiniMap();

	bool	ToggleMiniMap();

	void	UpdateExp(int iExp, int iExpNext, bool bUpdateImmediately) const;
	void	UpdateMSP(int iMSP, int iMSPMax, bool bUpdateImmediately);
	void	UpdateHP(int iHP, int iHPMax, bool bUpdateImmediately);

	void UpdatePosition(const __Vector3& vPos, float fYaw);

	void ZoomSet(float fZoom);
	void PositionInfoAdd(int iID, const __Vector3& vPos, D3DCOLOR crID, bool bDrawTop);
	void PositionInfoClear();
	bool LoadMap(const std::string& szMiniMapFN, float fMapSizeX, float fMapSizeZ); // minimap bitmap filename, sheet width length..(Meter);
	
	bool	ReceiveMessage(CN3UIBase* pSender, DWORD dwMsg); // Receives a message. Sender, msg

	virtual void	Tick();
	virtual void	Render(); // Minimap rendering...
	virtual bool	Load(HANDLE hFile);
	
	virtual void	Release();
	CUIStateBar();
	virtual ~CUIStateBar();

};
