#pragma once


#include "platform/common.h"

#define NET_PACKET_SIZE 512

typedef enum netpacktype_t
{
  NET_PACKET_CONNECT,
  NET_PACKET_DISCONNECT,
  NET_PACKET_ACCEPT,
} netpacktype_t;

typedef struct netpacket_t
{
  uint16_t type;
  uint16_t size;

  uint32_t sequence; // sequence number

  byte data[512]; 

} netpacket_t;
