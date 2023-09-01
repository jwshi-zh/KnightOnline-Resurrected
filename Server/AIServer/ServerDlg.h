// ServerDlg.h : header file
//

#if !defined(AFX_SERVERDLG_H__7E2A41F8_68A3_4C94_8A6E_7C80636869D3__INCLUDED_)
#define AFX_SERVERDLG_H__7E2A41F8_68A3_4C94_8A6E_7C80636869D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "IOCPort.h"
#include "GameSocket.h"

#include "MAP.h"
#include "NpcTable.h"
#include "NpcItem.h"
#include "Pathfind.h"
#include "User.h"
#include "Npc.h"
#include "NpcThread.h"
#include "Server.h"
#include "Compress.h"
#include "Party.h"

#include "extern.h"

#include "STLMap.h"
#include <vector>
#include <list>

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog

typedef std::vector <CNpcThread*>			NpcThreadArray;
typedef CSTLMap <CNpcTable>					NpcTableArray;
typedef CSTLMap <CNpc>						NpcArray;
typedef CSTLMap <_MAGIC_TABLE>				MagictableArray;
typedef CSTLMap <_MAGIC_TYPE1>				Magictype1Array;
typedef CSTLMap <_MAGIC_TYPE2>				Magictype2Array;
typedef CSTLMap <_MAGIC_TYPE3>				Magictype3Array;
typedef CSTLMap	<_MAGIC_TYPE4>				Magictype4Array;
typedef CSTLMap <_PARTY_GROUP>				PartyArray;
typedef CSTLMap <_MAKE_WEAPON>				MakeWeaponItemTableArray;
typedef CSTLMap <_MAKE_ITEM_GRADE_CODE>		MakeGradeItemTableArray;
typedef CSTLMap <_MAKE_ITEM_LARE_CODE>		MakeLareItemTableArray;
typedef std::list <int>						ZoneNpcInfoList;
typedef std::vector <MAP*>				ZoneArray;

class CServerDlg : public CDialog
{
private:
	void ResumeAI();
	BOOL CreateNpcThread();
	BOOL GetMagicTableData();
	BOOL GetMagicType1Data();
	BOOL GetMagicType2Data();
	BOOL GetMagicType3Data();
	BOOL GetMagicType4Data();
	BOOL GetMonsterTableData();
	BOOL GetNpcTableData();
	BOOL GetNpcItemTable();
	BOOL GetMakeWeaponItemTableData();
	BOOL GetMakeDefensiveItemTableData();
	BOOL GetMakeGradeItemTableData();
	BOOL GetMakeLareItemTableData();
	BOOL MapFileLoad();
	void GetServerInfoIni();
	
	void SyncTest();
	void RegionCheck();		// Checking users entering the region (a trick to reduce the load of the FindEnermy() function in the thread)
	void TestCode();
// Construction
public:
	void GameServerAcceptThread();
	BOOL AddObjectEventNpc(_OBJECT_EVENT* pEvent, int zone_number);
	void AllNpcInfo();			// ~sungyong 2002.05.23
	CUser* GetUserPtr(int nid);
	CUser* GetActiveUserPtr(int index);
	CNpc*  GetNpcPtr(TCHAR* pNpcName);
	CNpc*  GetEventNpcPtr();
	BOOL   SetSummonNpcData(CNpc* pNpc, int zone, float fx, float fz);
	int    MonsterSummon(TCHAR* pNpcName, int zone, float fx, float fz);
	int GetZoneIndex( int zonenumber );
	int GetServerNumber( int zonenumber );
	void ClostSocket( int zonenumber );

	void CheckAliveTest();
	void DeleteUserList(int uid);
	void DeleteAllUserList(int zone);
	void SendCompressedData(int nZone);			// Packets are compressed and sent.
	int Send(char* pData, int length, int nZone=0);
	void SendSystemMsg( char* pMsg, int zone, int type=0, int who=0 );
	void ResetBattleZone();

	CServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CServerDlg)
	enum { IDD = IDD_SERVER_DIALOG };
	CListBox	m_StatusList;
	CString	m_strStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerDlg)
	public:
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
//	ZoneArray			m_arZone;
	NpcArray			m_arNpc;
	NpcTableArray		m_arMonTable;
	NpcTableArray		m_arNpcTable;
	NpcThreadArray		m_arNpcThread;
	NpcThreadArray		m_arEventNpcThread;	// Event Npc Logic
	PartyArray			m_arParty;
	ZoneNpcInfoList		m_ZoneNpcList;
	MagictableArray		m_MagictableArray;
	Magictype1Array		m_Magictype1Array;
	Magictype2Array		m_Magictype2Array;
	Magictype3Array		m_Magictype3Array;
	Magictype4Array		m_Magictype4Array;
	MakeWeaponItemTableArray	m_MakeWeaponItemArray;
	MakeWeaponItemTableArray	m_MakeDefensiveItemArray;
	MakeGradeItemTableArray  m_MakeGradeItemArray;
	MakeLareItemTableArray  m_MakeLareItemArray;
	ZoneArray g_arZone;

	CWinThread* m_pZoneEventThread;		// zone

	CUser* m_pUser[MAX_USER];

	CNpcItem				m_NpcItem;

	CFile					m_UserLogFile;
	CFile					m_ItemLogFile;

	//BOOL			m_bNpcExit;
	long			m_TotalNPC;
	long			m_CurrentNPCError;
	long			m_CurrentNPC;
	short			m_sTotalMap;
	short			m_sMapEventNpc;

	// sungyong 2002.05.23
	BOOL			m_bFirstServerFlag;		// 1 if the game server is attached after the server is first started, 0 if not.
	short m_sSocketCount;
	short m_sReSocketCount;
	float m_fReConnectStart;
	short m_sErrorSocketCount;
	// ~sungyong 2002.05.23
	BYTE  m_byBattleEvent;				   // Flags related to war events ( 1: not at war, 0: at war)
	short m_sKillKarusNpc, m_sKillElmoNpc; // Number of NPCs killed during the war

	int m_iYear, m_iMonth, m_iDate, m_iHour, m_iMin, m_iWeather, m_iAmount;
	BYTE	m_byNight;			// Determine whether it is night or day... 1: day, 2: night
	BYTE    m_byTestMode;

	char	m_ODBCServer[32];
	char	m_ODBCDatabase[32];
	char	m_ODBCLogin[32];
	char	m_ODBCPwd[32];

	CIOCPort m_Iocport;	

private:
	CCompressManager	m_CompMng;				
	int					m_CompCount;
	TCHAR				m_CompBuf[10240];
	int					m_iCompIndex;

	BYTE				m_byZone;
	

// Implementation
protected:
	void DefaultInit();

	
//	CGameSocket m_GameSocket;

	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LRESULT OnGameServerLogin( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERDLG_H__7E2A41F8_68A3_4C94_8A6E_7C80636869D3__INCLUDED_)
