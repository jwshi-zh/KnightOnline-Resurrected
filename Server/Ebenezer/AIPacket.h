#ifndef _AIPACKET_H_
#define _AIPACKET_H_

// ---------------------------------------------------------------------
// Packets related to NPCs between the AI Server and the game server are numbers 1 to 49.
// ---------------------------------------------------------------------
const BYTE AI_SERVER_CONNECT	= 1;
const BYTE	NPC_INFO_ALL		= 2;
const BYTE	MOVE_REQ			= 3;
const BYTE	MOVE_RESULT			= 4;
const BYTE	MOVE_END_REQ		= 5;
const BYTE	MOVE_END_RESULT		= 6;
const BYTE	AG_NPC_INFO			= 7;	
const BYTE	AG_NPC_GIVE_ITEM	= 8;
const BYTE	AG_NPC_GATE_OPEN	= 9;	
const BYTE	AG_NPC_GATE_DESTORY	= 10;	
const BYTE	AG_NPC_INOUT		= 11;	
const BYTE	AG_NPC_EVENT_ITEM	= 12;	
const BYTE	AG_NPC_HP_REQ		= 13;	// sungyong tw

// ---------------------------------------------------------------------
// User, Npc common related packets between AI Server and game server are 50 to 100 times
// ---------------------------------------------------------------------
const BYTE	AG_SERVER_INFO			= 50;	// 
const BYTE	AG_ATTACK_REQ			= 51;	// Attck Packet
const BYTE	AG_ATTACK_RESULT		= 52;	// Attck Packet
const BYTE	AG_DEAD					= 53;	// Dead Packet
const BYTE	AG_SYSTEM_MSG			= 54;	// System message Packet
const BYTE	AG_CHECK_ALIVE_REQ		= 55;	// Server alive check
const BYTE	AG_COMPRESSED_DATA		= 56;	// Packet Data compressed
const BYTE	AG_ZONE_CHANGE			= 57;	// Zone change
const BYTE	AG_MAGIC_ATTACK_REQ		= 58;	// Magic Attck Packet
const BYTE	AG_MAGIC_ATTACK_RESULT	= 59;	// Magic Attck Packet
const BYTE	AG_USER_INFO_ALL		= 60;	// Send all user information
const BYTE	AG_LONG_MAGIC_ATTACK	= 61;	// Magic Attck Packet
const BYTE	AG_PARTY_INFO_ALL		= 62;	// Sending all information of Party
const BYTE	AG_HEAL_MAGIC			= 63;	// Healing magic
const BYTE	AG_TIME_WEATHER			= 64;	// time and whether info
const BYTE	AG_BATTLE_EVENT			= 65;	// battle event
		// ---------------------------------------------------------------------
		// Battle Event Sub Packet
		// ---------------------------------------------------------------------
		const BYTE	BATTLE_EVENT_OPEN   = 1;	// battle event open
		const BYTE	BATTLE_MAP_EVENT_RESULT = 2;	// battle zone map event result
		const BYTE	BATTLE_EVENT_RESULT		= 3;	// battle event result ( victory nation )
		const BYTE	BATTLE_EVENT_MAX_USER	= 4;	// battle event result ( user name )
		const BYTE	BATTLE_EVENT_KILL_USER	= 5;	// battle event result ( user kill count )

// ---------------------------------------------------------------------
// Packets related to users between the AI Server and the game server start from number 101.
// ---------------------------------------------------------------------
const BYTE	AG_USER_INFO		= 101;	// User's information
const BYTE	AG_USER_INOUT		= 102;	// User's In,Out
const BYTE	AG_USER_MOVE		= 103;	// User's move
const BYTE	AG_USER_MOVEEDGE	= 104;	// User's move end
const BYTE	AG_USER_SET_HP		= 105;	// User's HP
const BYTE	AG_USER_LOG_OUT		= 106;	// User's LogOut
const BYTE	AG_USER_REGENE		= 107;	// User's Regene
const BYTE	AG_USER_EXP			= 108;	// User's Exp
const BYTE	AG_USER_UPDATE		= 109;	// User's Update Info
const BYTE	AG_USER_FAIL		= 110;	// Dealing with the wrong user...
const BYTE	AG_USER_PARTY		= 111;	// party handling

#endif