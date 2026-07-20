#pragma once


#define SERVER_PORT 27015

#include <netinet/in.h>
#include <sys/socket.h>
#include "platform/network/network.h"

typedef enum clientmsg_t
{
  CMSG_CONNECT,
  CMSG_DISCONNECT,

} clientmsg_t;

typedef enum clientstate_t
{
  CSTATE_EMPTY,
  CSTATE_CONNECTED,

} clientstate_t;


