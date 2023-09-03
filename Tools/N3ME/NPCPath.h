// NPCPath.h: interface for the CNPCPath class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPCPATH_H__FA4815FA_8602_45C2_BEF2_5154AB5A4770__INCLUDED_)
#define AFX_NPCPATH_H__FA4815FA_8602_45C2_BEF2_5154AB5A4770__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\N3BASE\N3Base.h"
#include <list>

class CLyTerrain;

class CNPCPath : public CN3Base
{
public:
	char m_strPathName[256]; // length name..
	char m_strNPCName[80];	 // NPC name.
	int m_iNPCID;			 // NPC type..
	int m_iNumNPC;			 // How many?..

	__Vector3 m_LTStartVertex; // The two corners of the starting area..
	__Vector3 m_RBStartVertex;

	// CRect m_rcStartRect; // Area where NPCs appear.
	int m_iRegenTime; // Time it takes for an NPC to die and come back to life.
	int m_iActType;
	int m_iZoneID;

	std::list<__Vector3> m_Path; // Points to designate the path...start point~end point..

	CLyTerrain *m_pRefTerrain;

	DWORD m_dwColor;
	int m_iVersion;

	__Vector3 m_LTActVertex; // The two corners of the starting area..
	__Vector3 m_RBActVertex;

	unsigned char m_cAttr_Create;
	unsigned char m_cAttr_Regen;
	unsigned char m_cAttr_Group;
	unsigned char m_cAttr_Option;

public:
	bool CheckValid();
	void TransPos(float x, float z);
	void Load(HANDLE hFile);
	void Save(HANDLE hFile);
	int GetSize() { return m_Path.size(); }
	bool GetPath(int idx, __Vector3 *pPos); // Get the idxth point..
	void AddPos(__Vector3 Path);			// Add path.
	void DelPrevPos();
	CNPCPath();
	virtual ~CNPCPath();
};

#endif // !defined(AFX_NPCPATH_H__FA4815FA_8602_45C2_BEF2_5154AB5A4770__INCLUDED_)
