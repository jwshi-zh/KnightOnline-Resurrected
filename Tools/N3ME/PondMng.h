// PondMng.h: interface for the CPondMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PONDMNG_H__B71BF3BD_5F4F_4E5B_9F9F_B8BD45E2685B__INCLUDED_)
#define AFX_PONDMNG_H__B71BF3BD_5F4F_4E5B_9F9F_B8BD45E2685B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../N3Base/N3BaseFileAccess.h"
#include "VtxPosDummy.h"
#include <afxtempl.h>
#include "PondMesh.h"

enum
{
	PCM_NONE = 0,
	PCM_SELECT = 1,
	PCM_CREATE = 2,
	PCM_SELECTING
};

// class CPondMesh;
class CMainFrame;
class CDlgPondProperty;

#include <list>
typedef std::list<CPondMesh *>::iterator it_PondMesh;
typedef std::list<__Vector3 *>::iterator it_SelVtxBak;

class CPondMng : public CN3BaseFileAccess
{
public:
	CPondMng(CMainFrame *pMainFrm);
	virtual ~CPondMng();

	struct __VertexPond
	{
	public:
		float x, y, z;
		float nx, ny, nz;
		D3DCOLOR color;
		float u, v, u2, v2;

		void Set(__VertexXyzT2 &__Vtx, float snx, float sny, float snz, DWORD dwColor)
		{
			x = __Vtx.x, y = __Vtx.y, z = __Vtx.z;
			u = __Vtx.tu, v = __Vtx.tv;
			u2 = __Vtx.tu2, v2 = __Vtx.tv2;

			nx = snx, ny = sny, nz = snz;
			color = dwColor;
		}
	};

	// Attributes
public:
	CPondMesh *GetSelPond();
	int GetDrawPondNum() { return m_PondMeshes.size(); }
	it_PondMesh GetDrawPond();
	int GetPCursorMode() const { return m_PCursorMode; }

protected:
	std::list<CPondMesh *> m_PondMeshes;					  // Pond Mesh list
	std::list<CPondMesh *> m_pSelPonds;						  // Selected PondMesh
	CTypedPtrArray<CPtrArray, __VertexXyzT2 *> m_SelVtxArray; // selected points
	std::list<__Vector3 *> m_SelVtxBakArray;
	CVtxPosDummy m_VtxPosDummy; // dummy object that can move the point
	BOOL m_bEditMode;			// Pond water editing mode?
	BOOL m_bChooseGroup;		// edit to pond water group
	BOOL m_bChooseEditPond;		// Whether to edit only within the current pond
	BOOL m_bMovePond;			// Move the entire selected pond
	int m_PCursorMode;			// Add a pond? Or pick a point?

	CMainFrame *m_pMainFrm;			  // mainframe pointer
	CDlgPondProperty *m_pDlgProperty; // property dialog

	RECT m_rcSelDrag;				  // drag area
	__VertexXyzColor m_CreateLine[5]; // Lines visible when the pond is first created

	__Vector3 m_vPondsCenter; // the midpoint of the pond(s)

	BOOL m_bShift;

	// Operations
public:
	void Tick();
	void Render();
	virtual void Release();
	virtual bool Load(HANDLE hFile);
	virtual bool Save(HANDLE hFile);

	BOOL MouseMsgFilter(LPMSG pMsg);			   // Handle Mouse message
	void MakeGameFiles(HANDLE hFile, float fSize); // save as game file

	CPondMesh *CreateNewPondMesh(); // Add a new pond with start and end points (rectangle).

	BOOL SetPondID(CPondMesh *pPondMesh, int iPondID); // set the ID of the pond
	void SetEditMode(BOOL bEditMode);				   // set pond EditMode
	CPondMesh *GetPondMesh(int iPondID);			   // get the PondMesh

	void MainInvalidate(); // retake with screen force

	// Functions used in CDlgPondProperty
	void RemovePondMesh(int iPondID); // Delete the selected pond.
	void GoPond(int iPondID);		  // Go to the selected pond.
	void StationPond();				  // Reposition the points of the selected pond
	void MovePond();				  // Attempt to move the entire selected pond

	BOOL GetChooseGroup() { return m_bChooseGroup; }
	BOOL GetChooseEditPond() { return m_bChooseEditPond; }
	void ChooseGroupPond() { m_bChooseGroup ^= 1; }
	void ChooseEditPond() { m_bChooseEditPond ^= 1; }

	// may delete later
	void ReCalcSelectedVertex(); // rearrange according to the specified scale
	void ReCalcUV();			 // Recalculate the UV coordinates of the selected pond.
protected:
	void SetSelPond(CPondMesh *pPondMesh, BOOL bChooseGroup = FALSE); // select PondMesh
	BOOL SelectVtxByDragRect(RECT *pRect, BOOL bAdd, BOOL bSelectPond = FALSE);
	void ClearSelectRcAllPond(); // Initialize the temporary area of the selected points of all ponds

	void SelPondRelease();
	void SelPondDelete(CPondMesh *pPondMesh);
	void SetSelPonds(CPondMesh *pPondMesh);
	void SetRotatePonds(float fMove);

	void SetVtxCenter();

	void VtxBackupRelease();
	void SetVtxBackup();
	void ReSetVtxBackup();

	void ReSetDrawRect(__Vector3 vStrPos, __Vector3 vEndPos); // rearrange the area to draw the pond
	void InputDummyMovePos(__Vector3 vMovePos);				  // Input the selected pond as much as the dummy moves
};

#endif // !defined(AFX_PONDMNG_H__B71BF3BD_5F4F_4E5B_9F9F_B8BD45E2685B__INCLUDED_)
