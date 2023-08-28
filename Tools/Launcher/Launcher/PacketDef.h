#ifndef __PACKET_DEF_H_
#define __PACKET_DEF_H_

///////////////////////////////////////////////////////////////
// Packet Define 
///////////////////////////////////////////////////////////////
#define VERSION_REQ				0x01	// Send 
#define DOWNLOAD_INFO_REQ		0x02	// Send - s1 (current version) | Recv FTP
#define LOGIN_REQ				0x03	// Send - s1 (ID length) str1 (ID string: 20 bytes or less) s1 (PW length) str1 (PW string: 12 bytes or less) | Recv - b1 (0: Failure 1: Success 2: No ID 3: PW Wrong 4: Server Checking)
#define MGAME_LOGIN_REQ			0x04	// !!! LOGIN_REQ !!! same as
#define SERVER_LIST				0x05	// Recv - b1(ServerCount) loop( s1 (IP length), str1 (IP string), s1 (server name length), str1 (server name string), s1 (number of server connections) )
///////////////////////////////////////////////////////////////

#endif // end of #ifndef __PACKET_DEF_H_
