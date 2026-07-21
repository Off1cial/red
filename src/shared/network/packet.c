#include "shared/network/packet.h"

netpacket_t NetPacket_Prepare(
    netpacktype_t type, 
    byte data[NET_PACKET_SIZE],
    size_t nbytes
)
{
  netpacket_t packet = {0};
  packet.type = type;
  packet.size = nbytes;
  memcpy(packet.data, data, nbytes);
  return packet;
}
