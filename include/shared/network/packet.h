#pragma once


#include "platform/common.h"

#define NET_PACKET_SIZE 128

typedef enum netpacktype_t
{
  NET_PACKET_CHALLENGE,
  //NET_PACKET_CONNECT,
  NET_PACKET_DISCONNECT,
  NET_PACKET_ACCEPT,


  NET_PACKET_CLCMD,
  NET_PACKET_PFRAME,
} netpacktype_t;

typedef struct netpacket_t
{
  uint16_t type;
  uint16_t size;
  uint32_t sequence; // sequence number

  byte data[NET_PACKET_SIZE];

} netpacket_t;


netpacket_t NetPacket_Prepare(
    netpacktype_t type, 
    byte data[NET_PACKET_SIZE],
    size_t nbytes
);
