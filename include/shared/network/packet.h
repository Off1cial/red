#pragma once


#include "shared/network/client.h"


typedef enum netpacktype_t
{
  NET_PACKET_CONNECT,
  NET_PACKET_DISCONNECT
} netpacktype_t;

typedef struct netpacket_t
{
  netpacktype_t type; 
  
} netpacket_t;
