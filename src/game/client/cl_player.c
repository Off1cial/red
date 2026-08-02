#include "game/client/client.h"

clplayer_t gPlayer = {0};

void CL_ProcessPlayerFrame(playerframe_t frame)
{
  // Interpolate between new and old
  printf("Received Origin: (%0.2f, %0.2f, %0.2f)\n",
      frame.origin[0], frame.origin[1], frame.origin[2]);
  printf("Client origin: (%0.2f, %0.2f, %0.2f)\n",
      gPlayer.origin[0], gPlayer.origin[1], gPlayer.origin[2]);

  // Set old to new
  gPlayer.pframes[0] = frame;

  // Find absolute difference in posiiton
}


void CL_PlayerThink()
{
  // do pmove
}
