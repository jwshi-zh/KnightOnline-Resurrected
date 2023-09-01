#ifndef _DEFINE_H
#define _DEFINE_H

#define MAX_USER			3000
#define MAX_ID_SIZE			25
#define MAX_PW_SIZE			12
#define MAX_ITEM			28

////////////////////////////////////////////////////////////
// Socket Define
////////////////////////////////////////////////////////////
#define SOCKET_BUFF_SIZE	(1024*8)
#define MAX_PACKET_SIZE		(1024*2)

#define PACKET_START1				0XAA
#define PACKET_START2				0X55
#define PACKET_END1					0X55
#define PACKET_END2					0XAA
//#define PROTOCOL_VER				0X01

// status
#define STATE_CONNECTED			0X01
#define STATE_DISCONNECTED		0X02
#define STATE_GAMESTART			0x03

/////////////////////////////////////////////////////
// ITEM_SLOT DEFINE
#define RIGHTEAR			0
#define HEAD				1
#define LEFTEAR				2
#define NECK				3
#define BREAST				4
#define SHOULDER			5
#define RIGHTHAND			6
#define WAIST				7
#define LEFTHAND			8
#define RIGHTRING			9
#define LEG					10
#define LEFTRING			11
#define GLOVE				12
#define FOOT				13
/////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

typedef union{
	short int	i;
	BYTE		b[2];
} MYSHORT;

typedef union{
	int			i;
	BYTE		b[4];
} MYINT;

typedef union{
	DWORD		w;
	BYTE		b[4];
} MYDWORD;


// DEFINE MACRO PART...
#define BufInc(x) (x)++;(x) %= SOCKET_BUF_SIZE;

// DEFINE Shared Memory Queue Flag

#define E	0x00
#define R	0x01
#define W	0x02
#define WR	0x03

// DEFINE Shared Memory Queue Return VALUE

#define SMQ_BROKEN		10000
#define SMQ_FULL		10001
#define SMQ_EMPTY		10002
#define SMQ_PKTSIZEOVER	10003
#define SMQ_WRITING		10004
#define SMQ_READING		10005

// DEFINE Shared Memory Costumizing

#define MAX_PKTSIZE		512
#define MAX_COUNT		4096
#define SMQ_LOGGERSEND	"KNIGHT_SEND"
#define SMQ_LOGGERRECV	"KNIGHT_RECV"

// Packet Define...
#define WIZ_LOGIN				0x01	// Account Login
#define WIZ_NEW_CHAR			0x02	// Create Character DB
#define WIZ_DEL_CHAR			0x03	// Delete Character DB
#define WIZ_SEL_CHAR			0x04	// Select Character
#define WIZ_SEL_NATION			0x05	// Select Nation
#define WIZ_ALLCHAR_INFO_REQ	0x0C	// Account All Character Info Request
#define WIZ_LOGOUT				0x0F	// Request Logout
#define WIZ_DATASAVE			0x37	// User GameData DB Save Request
#define WIZ_KNIGHTS_PROCESS		0x3C	// Knights Related Packet..
#define WIZ_CLAN_PROCESS		0x4E	// Clans Related Packet..
#define WIZ_LOGIN_INFO			0x50	// define for DBAgent Communication
#define WIZ_KICKOUT				0x51	// Account ID forbid duplicate connection
#define WIZ_BATTLE_EVENT		0x57	// Battle Event Result

#define DB_COUPON_EVENT			0x10	// coupon event
		#define CHECK_COUPON_EVENT		0x01
		#define UPDATE_COUPON_EVENT		0x02

////////////////////////////////////////////////////////////////
// Knights Packet sub define 
////////////////////////////////////////////////////////////////
#define KNIGHTS_CREATE			0x11
#define KNIGHTS_JOIN			0x12
#define KNIGHTS_WITHDRAW		0x13
#define KNIGHTS_REMOVE			0x14
#define KNIGHTS_DESTROY			0x15
#define KNIGHTS_ADMIT			0x16
#define KNIGHTS_REJECT			0x17
#define KNIGHTS_PUNISH			0x18
#define KNIGHTS_CHIEF			0x19
#define KNIGHTS_VICECHIEF		0x1A
#define KNIGHTS_OFFICER			0x1B
#define KNIGHTS_ALLLIST_REQ		0x1C
#define KNIGHTS_MEMBER_REQ		0x1D
#define KNIGHTS_CURRENT_REQ		0x1E
#define KNIGHTS_STASH			0x1F
#define KNIGHTS_MODIFY_FAME		0x20
#define KNIGHTS_JOIN_REQ		0x21
#define KNIGHTS_LIST_REQ		0x22

////////////////////////////////////////////////////////////////
// Clan Packet sub define
////////////////////////////////////////////////////////////////
#define CLAN_CREATE				0x01
#define CLAN_JOIN				0x02

////////////////////////////////////////////////////////////////
// Update User Data type define
////////////////////////////////////////////////////////////////
#define UPDATE_LOGOUT			0x01
#define UPDATE_ALL_SAVE			0x02
#define UPDATE_PACKET_SAVE		0x03


// Reply packet define...

#define SEND_ME					0x01
#define SEND_REGION				0x02
#define SEND_ALL				0x03

#define SLOT_MAX			14
#define HAVE_MAX			28
#define ITEMCOUNT_MAX		9999
#define WAREHOUSE_MAX		196
/////////////////////////////////////////////////////////////////////////////////
// Structure Define
/////////////////////////////////////////////////////////////////////////////////
struct	_ITEM_DATA
{
	int		nNum;
	short	sDuration;
	short	sCount;
	__int64	nSerialNum;
};

struct _ITEM_TABLE
{
	int   m_iNum;
	char  m_strName[50];
	BYTE  m_bKind;
	BYTE  m_bSlot;
	BYTE  m_bRace;
	BYTE  m_bClass;
	short m_sDamage;
	short m_sDelay;
	short m_sRange;
	short m_sWeight;
	short m_sDuration;
	int	  m_iBuyPrice;
	int	  m_iSellPrice;
	short m_sAc;
	BYTE  m_bCountable;
	int	  m_iEffect1;
	int	  m_iEffect2;
	BYTE  m_bReqLevel;
	BYTE  m_bReqRank;
	BYTE  m_bReqTitle;
	BYTE  m_bReqStr;
	BYTE  m_bReqSta;
	BYTE  m_bReqDex;
	BYTE  m_bReqIntel;
	BYTE  m_bReqCha;
	BYTE  m_bSellingGroup;
	BYTE  m_ItemType;
	short m_sHitrate;
	short m_sEvarate;
	short m_sDaggerAc;
	short m_sSwordAc;
	short m_sMaceAc;
	short m_sAxeAc;
	short m_sSpearAc;
	short m_sBowAc;
	BYTE  m_bFireDamage;
	BYTE  m_bIceDamage;
	BYTE  m_bLightningDamage;
	BYTE  m_bPoisonDamage;
	BYTE  m_bHPDrain;
	BYTE  m_bMPDamage;
	BYTE  m_bMPDrain;
	BYTE  m_bMirrorDamage;
	BYTE  m_bDroprate;
	BYTE  m_bStrB;
	BYTE  m_bStaB;
	BYTE  m_bDexB;
	BYTE  m_bIntelB;
	BYTE  m_bChaB;
	short m_MaxHpB;
	short m_MaxMpB;
	BYTE  m_bFireR;
	BYTE  m_bColdR;
	BYTE  m_bLightningR;
	BYTE  m_bMagicR;
	BYTE  m_bPoisonR;
	BYTE  m_bCurseR;
};

struct _USER_DATA
{
	char	m_id[MAX_ID_SIZE+1];
	char	m_Accountid[MAX_ID_SIZE+1];

	BYTE	m_bZone;
	float	m_curx;
	float	m_curz;
	float	m_cury;

	BYTE	m_bNation;
	BYTE	m_bRace;
	short	m_sClass;
	BYTE	m_bHairColor;
	BYTE	m_bRank;
	BYTE	m_bTitle;
	BYTE	m_bLevel;
	int		m_iExp;
	int		m_iLoyalty;
	BYTE	m_bFace;
	BYTE	m_bCity;
	short	m_bKnights;
	BYTE	m_bFame;
	short	m_sHp;
	short	m_sMp;
	short	m_sSp;
	BYTE	m_bStr;
	BYTE	m_bSta;
	BYTE	m_bDex;
	BYTE	m_bIntel;
	BYTE	m_bCha;
	BYTE	m_bAuthority;
	BYTE	m_bPoints;
	int		m_iGold;
	short	m_sBind;
	int		m_iBank;
	
	BYTE    m_bstrSkill[9];
	_ITEM_DATA m_sItemArray[HAVE_MAX+SLOT_MAX];
	_ITEM_DATA m_sWarehouseArray[WAREHOUSE_MAX];

	BYTE	m_bLogout;
	BYTE	m_bWarehouse;
	DWORD	m_dwTime;
};


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//
//	Global Function Define
//

inline void GetString(char* tBuf, char* sBuf, int len, int& index)
{
	memcpy(tBuf, sBuf+index, len);
	index += len;
};

inline BYTE GetByte(char* sBuf, int& index)
{
	int t_index = index;
	index++;
	return (BYTE)(*(sBuf+t_index));
};

inline int GetShort(char* sBuf, int& index)
{
	index += 2;
	return *(short*)(sBuf+index-2);
};

inline DWORD GetDWORD(char* sBuf, int& index)
{
	index += 4;
	return *(DWORD*)(sBuf+index-4);
};

inline float Getfloat(char* sBuf, int& index)
{
	index += 4;
	return *(float*)(sBuf+index-4);
};

inline __int64 GetInt64(char* sBuf, int& index)
{
	index += 8;
	return *(__int64*)(sBuf+index-8);
};

inline void SetString(char* tBuf, char* sBuf, int len, int& index)
{
	memcpy(tBuf+index, sBuf, len);
	index += len;
};

inline void SetByte(char* tBuf, BYTE sByte, int& index)
{
	*(tBuf+index) = (char)sByte;
	index++;
};

inline void SetShort(char* tBuf, int sShort, int& index)
{
	short temp = (short)sShort;

	CopyMemory( tBuf+index, &temp, 2);
	index += 2;
};

inline void SetDWORD(char* tBuf, DWORD sDWORD, int& index)
{
	CopyMemory( tBuf+index, &sDWORD, 4);
	index += 4;
};

inline void Setfloat ( char* tBuf, float sFloat, int& index )
{
	CopyMemory( tBuf+index, &sFloat, 4);
	index += 4;
};

inline void SetInt64 ( char* tBuf, __int64 nInt64, int& index )
{
	CopyMemory( tBuf+index, &nInt64, 8);
	index += 8;
};
// sungyong 2001.11.06
inline int GetVarString(TCHAR* tBuf, TCHAR* sBuf, int nSize, int& index)
{
	int nLen = 0;
	
	if(nSize == sizeof(BYTE))	nLen = GetByte(sBuf, index);
	else nLen = GetShort(sBuf, index);

	GetString(tBuf, sBuf, nLen, index);
	*(tBuf + nLen) = 0;

	return nLen;
};

inline void SetVarString(TCHAR *tBuf, TCHAR* sBuf, int len, int &index)
{
	*(tBuf+index) = (BYTE)len;
	index ++;

	CopyMemory(tBuf+index, sBuf, len);
	index += len;
};
// ~sungyong 2001.11.06
inline int ParseSpace( char* tBuf, char* sBuf)
{
	int i = 0, index = 0;
	BOOL flag = FALSE;
	
	while(sBuf[index] == ' ' || sBuf[index] == '\t')index++;
	while(sBuf[index] !=' ' && sBuf[index] !='\t' && sBuf[index] !=(BYTE) 0){
		tBuf[i++] = sBuf[index++];
		flag = TRUE;
	}
	tBuf[i] = 0;

	while(sBuf[index] == ' ' || sBuf[index] == '\t')index++;
	if(!flag) return 0;	
	return index;
};

inline CString GetProgPath()
{
	char Buf[256], Path[256];
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	::GetModuleFileName(AfxGetApp()->m_hInstance, Buf, 256);
	_splitpath(Buf,drive,dir,fname,ext);
	strcpy(Path, drive);
	strcat(Path, dir);		
	CString _Path = Path;
	return _Path;
};

inline void LogFileWrite( LPCTSTR logstr )
{
	CString ProgPath, LogFileName;
	CFile file;
	int loglength;

	ProgPath = GetProgPath();
	loglength = strlen( logstr );

	LogFileName.Format("%s\\Aujard.log", ProgPath);
	
	file.Open( LogFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite );

	file.SeekToEnd();
	file.Write(logstr, loglength);
	file.Close();
};

inline int DisplayErrorMsg(SQLHANDLE hstmt)
{
	SQLCHAR       SqlState[6], Msg[1024];
	SQLINTEGER    NativeError;
	SQLSMALLINT   i, MsgLen;
	SQLRETURN     rc2;
	char		  logstr[512];
	memset( logstr, NULL, 512 );

	i = 1;
	while ((rc2 = SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA)
	{
		sprintf( logstr, "*** %s, %d, %s, %d ***\r\n", SqlState,NativeError,Msg,MsgLen );
		LogFileWrite( logstr );

		i++;
	}

	if( strcmp((char *)SqlState, "08S01") == 0 )
		return -1;
	else
		return 0;
};

#endif