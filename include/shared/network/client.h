#pragma once


#define SERVER_PORT 27015

#include <netinet/in.h>
#include <sys/socket.h>
#include "platform/network/network.h"



/* Ripped from Quake III
typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED, 	// not talking to a server
	CA_AUTHORIZING,		// not used any more, was checking cd key 
	CA_CONNECTING,		// sending request packets to the server
	CA_CHALLENGING,		// sending challenge packets to the server
	CA_CONNECTED,		// netchan_t established, getting gamestate
	CA_LOADING,			// only during cgame initialization, never during main loop
	CA_PRIMED,			// got gamestate, waiting for first frame
	CA_ACTIVE,			// game views should be displayed
	CA_CINEMATIC		// playing a cinematic or a static pic, not connected to a server
} connstate_t;


*/


typedef enum clientmsg_t
{
  CMSG_CONNECT,
  CMSG_DISCONNECT,

} clientmsg_t;

typedef enum clientstate_t
{
  CSTATE_EMPTY,
  CSTATE_CONNECTING,
  CSTATE_CONNECTED,
  CSTATE_ACTIVE,
  CSTATE_DISCONNECTED,

} clientstate_t;


