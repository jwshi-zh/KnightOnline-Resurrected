#pragma once

const int SOCKET_PORT_GAME = 15001; // The port number to which the socket is attached.
const int SOCKET_PORT_LOGIN = 15100; // The port number to which the socket is attached.

#define N3_ACCOUNT_LOGIN			0xf3
#define N3_ACCOUNT_LOGIN_MGAME		0xf4	// !!! LOGIN_REQ !!! same as
#define N3_GAMESERVER_GROUP_LIST	0xf5	// Recv - b1(ServerCount) loop( s1 (IP length), str1 (IP string), s1 (server name length), str1 (server name string), s1 (number of server connections) )

#define	N3_GAME_SERVER_LOGIN	0x01	// Game server 에 Log in..
#define	N3_NEW_CHARACTER		0x02	// Creating a new character...
#define	N3_DELETE_CHARACTER		0x03	// Delete the character you created.
#define	N3_CHARACTER_SELECT		0x04	// Game start..
#define	N3_NATION_SELECT		0x05	// Select Country - Fail if 0.
#define	N3_MOVE					0x06	// Move..
#define	N3_ROTATE				0x09	// Rotate..
#define	N3_USER_INOUT			0x07	// User inout..
#define	N3_NPC_INOUT			0x0A	// NPC inout..
#define	N3_ATTACK				0x08	// Attack..
#define	N3_NPC_MOVE				0x0B	// NPC move..
#define	N3_ALL_CHARACTER_INFO_REQUEST	0x0C	// Request all character information...
#define	N3_GAMESTART			0x0D	// Game Start..
#define	N3_MYINFO				0x0E	// NPC moveedge..
#define	N3_CHAT					0x10	// Chat..
enum e_ChatMode {
	N3_CHAT_NORMAL = 1,
	N3_CHAT_PRIVATE,
	N3_CHAT_PARTY,
	N3_CHAT_FORCE,
	N3_CHAT_SHOUT,
	N3_CHAT_CLAN,
	N3_CHAT_PUBLIC,
	N3_CHAT_WAR,
	N3_CHAT_TITLE,						// Notice to go to top row
	N3_CHAT_TITLE_DELETE,				// Delete the notice on the top line
	N3_CHAT_CONTINUE,					// continuous notice
	N3_CHAT_CONTINUE_DELETE,			// Delete persistent notice
	N3_CHAT_UNKNOWN = 0xffffffff
};

#define	N3_DEAD					0x11	// Player Dead..
#define	N3_REGENE				0x12	// Player Regene..
#define N3_TIME					0x13	// game time
#define N3_WEATHER				0x14	// weather changes in game
#define	N3_UPDATE_REGION_UPC	0x15	// Player Regene..
#define	N3_UPDATE_REGION_NPC	0x1C	// NPC Region Update
#define	N3_REQUEST_NPC_IN		0x1D	// NPC Information Request
#define N3_WARP					0x1E	// Warp !!!
#define	N3_ITEM_MOVE			0x1F	// Item Move..
#define N3_HP_CHANGE			0x17	// Player stats Update
#define N3_MSP_CHANGE			0x18	// Player stats Update
#define N3_EXP_CHANGE			0x1A	// Player stats Update
#define N3_LEVEL_CHANGE			0x1B	// Player stats Update
#define N3_REALM_POINT_CHANGE	0x2A	// Player stats Update // Country Contribution
#define N3_REQUEST_USER_IN		0x16	// User In request required after Update Region
#define N3_NPC_EVENT			0x20	// NPC Event For example, commerce..
#define	N3_ITEM_TRADE_START		0x25	// Commerce packets.. The server zooms..
#define N3_TARGET_HP			0x22	// Target Character HP Percentage Request
#define N3_ITEM_BUNDLE_DROP		0x23	// Item drops when dying
#define N3_ITEM_BUNDLE_OPEN_REQUEST 0x24	// Open an item box or search a corpse. Common to request and receive data..
#define	N3_ITEM_TRADE			0x21	// Commerce packets.. Server zooms..
enum e_SubPacket_Trade {
	N3_SP_TRADE_BUY = 0x01,
	N3_SP_TRADE_SELL = 0x02,
	N3_SP_TRADE_MOVE = 0x03
};

#define	N3_ITEM_DROPPED_GET		0x26	// Regarding the items acquired by the user..
#define N3_ZONE_CHANGE			0x27	// Zone change.. Recv - b1 zone number f3 coordinates X, Z, Y | Recv
#define N3_POINT_CHANGE			0x28	// Level Change - Sending b1 (1 Strength 2 Stamina 3 Agility 4 Intelligence 5 Magic) s(-1 +1) // Receiving b1 (1 Strength 2 Stamina 3 Agility 4 Intelligence 5 Charisma) s (absolute value)
#define N3_STATE_CHANGE			0x29	// State Change Send - b2(Type, State) | Recv S1(ID) b2(Type, State) - // Type 1 Sit-Stand, 2 Looking for a party...
enum e_SubPacket_State {
	N3_SP_STATE_CHANGE_SITDOWN = 0x01,
	N3_SP_STATE_CHANGE_RECRUIT_PARTY = 0x02,
	N3_SP_STATE_CHANGE_SIZE = 0x03,
	N3_SP_STATE_CHANGE_ACTION = 0x04,			// 1 - Greetings, 11 - Taunt
	N3_SP_STATE_CHANGE_VISIBLE = 0x05
};		// Transparent 0 to 255

#define N3_VERSION_CHECK		0x2B	// Check version... s1 - Version
// #define N3_CRYPTION_PUBLIC_KEY 0x2C // Encryption 64-bit public key Recv 64i
#define N3_USER_LOOK_CHANGE		0x2D	// The player&#39;s attire or weapon changes.
#define N3_NOTICE				0x2E	// announcement..

#define N3_PARTY_OR_FORCE		0x2F	// Party Or Force Packet... Send - b1(Party:1, Force:2)
	// Sub Packet
enum e_SubPacket_Party {
	N3_SP_PARTY_OR_FORCE_CREATE = 0x01,	// Send - s1(ID)	| Recv b1(YesNoErr)
	N3_SP_PARTY_OR_FORCE_PERMIT = 0x02,	// Send - b1(YesNo) | Recv - s1(ID)
	N3_SP_PARTY_OR_FORCE_INSERT = 0x03,	// Send - s1(ID) | Recv - s3(ID, HPMax, HP) b2(Level, Class) - String is found by ID.
	N3_SP_PARTY_OR_FORCE_REMOVE = 0x04,	// Send - s1(ID) | Recv - s1(ID) - If you are yourself, you must break the party.
	N3_SP_PARTY_OR_FORCE_DESTROY = 0x05,	// Send
	N3_SP_PARTY_OR_FORCE_HP_CHANGE = 0x06,	// Recv - s3(ID, HPMax, HP)
	N3_SP_PARTY_OR_FORCE_LEVEL_CHANGE = 0x07,	// Recv - s1(ID), b1(Level)
	N3_SP_PARTY_OR_FORCE_CLASS_CHANGE = 0x08,	// Recv - s1 (ID), b1 (Class) Rarely, but when changing jobs...
	N3_SP_PARTY_OR_FORCE_STATUS_CHANGE = 0x09
};	// Recv - s1 (ID), b1 (Status) Poison, Curse, Sustainability Magic, Blessing

#define N3_PER_TRADE			0x30	// Personal Trade..
// Sub Packet
enum e_SubPacket_PerTrade {
	N3_SP_PER_TRADE_REQ = 0x01,
	N3_SP_PER_TRADE_AGREE = 0x02,
	N3_SP_PER_TRADE_ADD = 0x03,
	N3_SP_PER_TRADE_OTHER_ADD = 0x04,
	N3_SP_PER_TRADE_DECIDE = 0x05,
	N3_SP_PER_TRADE_OTHER_DECIDE = 0x06,
	N3_SP_PER_TRADE_DONE = 0x07,
	N3_SP_PER_TRADE_CANCEL = 0x08
};

// Magic effect packet structure (common to all)
// (BYTE)	N3_MAGIC
// (BYTE)	N3_MAGIC_CASTING or flying or effecting or fail
// (DWORD)	Magic ID
// (short) Caster ID
// (short) target ID
// (short) data1
// (short) data2
// (short) data3
// (short) data4
// (short) data5
// (short) data6
#define N3_MAGIC				0x31	// magic effect...
	// Sub Packet
enum e_SubPacket_Magic {
	N3_SP_MAGIC_CASTING = 0x01,
	N3_SP_MAGIC_FLYING = 0x02,
	N3_SP_MAGIC_EFFECTING = 0x03,
	N3_SP_MAGIC_FAIL = 0x04,
	N3_SP_MAGIC_TYPE4BUFFTYPE = 0x05,
	N3_SP_MAGIC_CANCEL = 0x06
};

#define N3_SKILL_CHANGE			0x32	// Skill Point Change..
#define N3_OBJECT_EVENT			0x33	// Send - s4(Object Event ID, Parameter, NPC ID to be controlled, NPC Command to be controlled) | Recv

#define N3_CLASS_CHANGE			0x34	// Class Change..
// Sub Packet..
enum e_SubPacket_ClassChange_Main {
	N3_SP_CLASS_CHANGE_PURE = 0x01,		// Class Change Real..
	N3_SP_CLASS_CHANGE_REQ = 0x02,		// Class Change Req..
	N3_SP_CLASS_ALL_POINT = 0x03,		// Class Change All Point..
	N3_SP_CLASS_SKILL_POINT = 0x04,
	N3_SP_CLASS_POINT_CHANGE_PRICE_QUERY = 0x05
};	// Class Change Skill Point..

enum e_SubPacket_ClassChange {
	N3_SP_CLASS_CHANGE_SUCCESS = 0x01,		// Success..
	N3_SP_CLASS_CHANGE_NOT_YET = 0x02,		// Not yet..
	N3_SP_CLASS_CHANGE_ALREADY = 0x03,		// Already..
	N3_SP_CLASS_CHANGE_ITEM_IN_SLOT = 0x04,		// Item in Slot..
	N3_SP_CLASS_CHANGE_FAILURE = 0x00
};	// Failure..

#define N3_CHAT_SELECT_TARGET		0x35	// Send - s1(ID String Length) str1(ID String) | Recv - s1(ID String Length) str1(ID String) Fails if string length is 0.
#define N3_CONCURRENT_USER_COUNT	0x36	// Send - ... | Recv - s1 (current number of concurrent users)
#define N3_REQUEST_GAME_SAVE		0x37	// Request to save to server.. not received..

#define N3_DURABILITY_CHANGE		0x38	// Durability changes...

#define N3_TIME_NOTIFY				0x39	// After 2 seconds of not seeing anything in the packet... I send this...

#define N3_ITEM_TRADE_REPAIR		0x3a	// Selling, exchanging, repairing items..
#define N3_ITEM_REPAIR_REQUEST		0x3b	// Item repair response..


#define N3_KNIGHTS					0x3C	// Knights Related Packet..
enum e_SubPacket_Knights {
	N3_SP_KNIGHTS_CREATE = 0x01, // Create Send - s1(Name Length) str1 | Recv - b1 (1: success 0: failure)
	N3_SP_KNIGHTS_JOIN = 0x02, // Sign Up Send - s1(Knights ID) | Recv - b1 (1: success 0: failure)
	N3_SP_KNIGHTS_WITHDRAW = 0x03, // Unsubscribe Send - | Recv - b1 (1: success 0: failure)
	N3_SP_KNIGHTS_MEMBER_REMOVE = 0x04, // Delete member -
	N3_SP_KNIGHTS_DESTROY = 0x05, // Crossover Send - | Recv - b1 (1: success 0: failure)
	N3_SP_KNIGHTS_MEMBER_JOIN_ADMIT = 0x06, // Member Registration Permission Send - s1(Knights ID) | Recv - b1 (1: success 0: failure)
	N3_SP_KNIGHTS_MEMBER_JOIN_REJECT = 0x07, // Reject Membership Send - s1(Knights ID) | Recv - b1 (1: success 0: failure)
	N3_SP_KNIGHTS_MEMBER_PUNISH = 0x08, // Member Discipline - Same as subscription
	N3_SP_KNIGHTS_APPOINT_CHIEF = 0x09, // Appointment of a general manager - same as joining
	N3_SP_KNIGHTS_APPOINT_VICECHIEF = 0x0A, // Appointment of vice-captain - same as joining
	N3_SP_KNIGHTS_APPOINT_OFFICER = 0x0B, // Appointment of officers - same as joining
	N3_SP_KNIGHTS_GRADE_CHANGE_ALL = 0x0C, // Recv - s1(count) Loop { s1(Knights ID) b2(Grade Rank) }
	N3_SP_KNIGHTS_MEMBER_INFO_ALL = 0x0D, // All member requests Send - s1(page) | s1(Member Count) Loop { s1(Name Length) str1 (Name) }
	N3_SP_KNIGHTS_MEMBER_INFO_ONLINE = 0x0E, // List of current connections Send - s1(page) | s1(Member Count) Loop { s1(Name Length) str1 (Name) }
	N3_SP_KNIGHTS_STASH = 0x0F, // Templar Warehouse
	N3_SP_KNIGHTS_DUTY_CHANGE = 0x10, // Change a member&#39;s position.. Go to that member. Recv - s1(Knights ID) b1(Position);
	N3_SP_KNIGHTS_JOIN_REQ = 0x11, // templar index
	N3_SP_KNIGHTS_UNKNOWN
};

enum e_SubPacket_KNights_Create {
	N3_SP_KNIGHTS_CREATE_FAIL_DBFAIL = 0x00,
	N3_SP_KNIGHTS_CREATE_SUCCESS = 0x01,
	N3_SP_KNIGHTS_CREATE_FAIL_LOWLEVEL = 0x02,
	N3_SP_KNIGHTS_CREATE_FAIL_DUPLICATEDNAME = 0x03,
	N3_SP_KNIGHTS_CREATE_FAIL_LOWMONEY = 0x04,
	N3_SP_KNIGHTS_CREATE_FAIL_ALREADYJOINED = 0x05,
	N3_SP_KNIGHTS_CREATE_FAIL_UNKNOWN = 0x06,
	N3_SP_KNIGHTS_CREATE_FAIL_INVALIDDAY = 0x07,
	N3_SP_KNIGHTS_CREATE_FAIL_INVALIDSERVER = 0x08
};

enum e_SubPacket_KNights_Common {
	N3_SP_KNIGHTS_COMMON_DBFAIL = 0x00,	// DB search failed..
	N3_SP_KNIGHTS_COMMON_SUCCESS = 0x01,	// success
	N3_SP_KNIGHTS_COMMON_FAIL_NONE_USER = 0x02,	// no user...
	N3_SP_KNIGHTS_COMMON_FAIL_DEAD_USER = 0x03,	// Opponent is dead...
	N3_SP_KNIGHTS_COMMON_FAIL_ENEMY_USER = 0x04,	// The country of the other user is different..
	N3_SP_KNIGHTS_COMMON_FAIL_OTHER_CLAN_USER = 0x05,	// The opposing user is already a member of another clan or guild.
	N3_SP_KNIGHTS_COMMON_FAIL_INVALIDRIGHT = 0x06,	// No permission..
	N3_SP_KNIGHTS_COMMON_FAIL_NONE_CLAN = 0x07,	// Knights that don&#39;t exist...
	N3_SP_KNIGHTS_COMMON_FAIL_FULL = 0x08,	// Full staff..
	N3_SP_KNIGHTS_COMMON_FAIL_ME = 0x09,	// If you choose yourself...
	N3_SP_KNIGHTS_COMMON_FAIL_NOT_JOINED = 0x0A,	// Not affiliated with a Templar or Clan...
	N3_SP_KNIGHTS_COMMON_FAIL_REJECT = 0x0B,	// rejected by the other side...
	N3_SP_KNIGHTS_COMMON_FAIL_BATTLEZONE = 0x0C	// Rejection in the war zone.
};	// Join, appoint, leave, withdraw are all written.....

#define N3_ITEM_COUNT_CHANGE		0x3d
#define N3_KNIGHTS_LIST_BASIC		0x3e // Templar Information Request
enum e_SubPacket_KnightsList {
	N3_SP_KNIGHTS_LIST_BASIC_ALL = 0x01,  // Receive - s1(knights Count) { s21(id, namelength), str1(name) }
	N3_SP_KNIGHTS_LIST_BASIC_INSERT = 0x02, // Receive - s2(id, name length), str1(name)
	N3_SP_KNIGHTS_LIST_BASIC_REMOVE = 0x03
}; // Receive - s1(id)
#define N3_ITEM_DESTROY				0x3f
#define N3_ADMINISTRATOR			0x40 // Admin only packets..
enum e_SubPacket_Administrator {
	N3_SP_ADMINISTRATOR_ARREST = 0x01, // Change the zone to where the user is.. Send b1(Type) s1(username length), str1(username)
	N3_SP_ADMINISTRATOR_FORBID_CONNECT = 0x02, // Access prohibited and expelled.. Send b1 (Type) s1 (username length), str1 (username)
	N3_SP_ADMINISTRATOR_CHAT_FORBID = 0x03, // Chat ban Send b1 (Type) s1 (username length), str1 (username)
	N3_SP_ADMINISTRATOR_CHAT_PERMIT = 0x04
}; // Chat permission Send b1 (Type) s1 (username length), str1 (username)

#define N3_CHECK_SPEEDHACK			0x41 // Packet for speed hack check..

#define N3_COMPRESSED_PACKET		0x42 // Compressed packets,.. need to be decompressed and parsed once more..
#define N3_CONTINOUS_PACKET			0x44 // Packets coming together.. You need to parse while looping..

#define N3_WAREHOUSE				0X45 // Storage Main Packet..
enum e_SubPacket_WareHouse {
	N3_SP_WARE_OPEN = 0x01,	// Open..
	N3_SP_WARE_GET_IN = 0x02,	// †Ken..
	N3_SP_WARE_GET_OUT = 0X03,	// subtract..
	N3_SP_WARE_WARE_MOVE = 0x04,	// movement..
	N3_SP_WARE_INV_MOVE = 0x05,	// movement..
	N3_SP_WARE_INN = 0x10,	// The first time.. Received as an innkeeper..
	N3_SP_WARE_UNKNOWN
};

#define N3_SERVER_CHANGE			0x46
#define N3_REPORT_DEBUG_STRING		0x47
#define N3_HOME						0x48	// Go to town.. Receive warp in response.
#define N3_FRIEND_INFO				0x49	// Friend info.. Send s1(usercount) s1(namelength), str1(username) | Receive s1(usercount) ㅣLoop { s1(name length), str1(username), b1(connection, party or operation) }
#define N3_NOAH_CHANGE	0x4a	// Noah Change..
enum e_SubPacket_NoahChange {
	N3_SP_NOAH_GET = 0x01,	// noah Acquisition,
	N3_SP_NOAH_LOST = 0x02,	// Lost Noah...
	N3_SP_NOAH_SPEND = 0x03	// Noah consumption..
};
#define	N3_WARP_LIST					0x4b	// Warp List Recv -
// #define N3_SERVER_CHECK 0x4c // Check virtual temporary server... Recv - s1, str1(IP), s1(dwPort) | Send s1(Zone ID), s3(x*10,z*10,y*10)
// #define N3_SERVER_CONCURRENT_CONNECT 0x4d // Number of concurrent users... Send - b1 (country) | Recv - s1 (number of concurrent entries)
#define N3_CORPSE_CHAR					0x4e	// The character regenerates and leaves it as a corpse.
#define N3_PARTY_BBS					0x4f	// Register on the party bulletin board
enum e_Party_BBS {
	N3_SP_PARTY_REGISTER = 0x01,		// Register on the party bulletin board
	N3_SP_PARTY_REGISTER_CANCEL = 0x02,		// Unregister from the party bulletin board
	N3_SP_PARTY_BBS_DATA = 0x03
};	// Request Party Post Information

#define N3_TRADE_BBS					0x50	// commerce bulletin board
enum e_Trade_BBS_Kind {
	N3_SP_TRADE_BBS_BUY = 0x01,		// list of things to buy
	N3_SP_TRADE_BBS_SELL = 0x02
};		// list of things to sell

enum e_Trade_BBS_Sub {
	N3_SP_TYPE_REGISTER = 0x01,		// register an object
	N3_SP_TYPE_REGISTER_CANCEL = 0x02,		// Unregister
	N3_SP_TYPE_BBS_DATA = 0x03,		// bulletin board information request
	N3_SP_TYPE_BBS_OPEN = 0x04,		// Open commerce board
	N3_SP_TYPE_BBS_TRADE = 0X05
};	// Apply for a transaction on the bulletin board
#define N3_KICK_OUT						0x51	// Recv s1, str1(IP) s1(port) | Send s1, str1(ID)
#define N3_NPC_QUEST					0X52	// npc quest
#define N3_ALIVE_CHECK					0x53	// simple echo
#define N3_ITEM_WEIGHT_CHANGE			0x54
#define N3_QUEST_SELECT					0x55	// Select quest menu
#define N3_QUEST_TALK					0x56	// Quest Conversation

#define N3_TEMP_TEST 0xff // Temporary test...
