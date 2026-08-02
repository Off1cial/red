#include "game/client/client.h"
#include "game/client/cl_player.h"
#include "corebase/time.h"
#include "game/shared/pmove.h"

#include "engine/camera.h"

static void CL_AttemptJoin()
{
  float now = pltTime_Time();
  if (now - gClient->time_lastconnectattempt < 2.0f)
    return;

  // Enough time has passed, try again

  // Can we make more attempts?
  if (gClient->connect_attempts >= gClient->connect_maxattempts)
  {
    printf("[CLIENT]: Connection failed after %d attempt(s)\n", gClient->connect_attempts);
    gClient->connect_attempts = 0;
    CL_Disconnect(gClient);
    return;
  }

  // Make an attempt
  printf("[CLIENT]: Attemping to join server....\n");
  gClient->connect_attempts++;
  gClient->time_lastconnectattempt = now;
  CL_SendChallengePacket(gClient);
}

void CL_Think(float dt)
{
  if (gClient->state == CSTATE_CONNECTED)
  {
      CL_AttemptJoin();
  }


  CL_CreatePlayerCommand(dt);
  playercmd_t* latestcmd = &gClient->cmds[(gClient->cmdcount - 1) % CLIENT_CMD_BACKUP];

  PM_Move(latestcmd, gPlayer.origin, gPlayer.velocity, gPlayer.lookdir, dt);
  VectorCopy(gPlayer.origin, gCamera->origin);

  float now = pltTime_Time();
  if (now - gClient->time_lastcmdsent >= 1.0f / 30.0f)
  {
    CL_SendPlayerCommand(gClient);
    gClient->time_lastcmdsent = now;
  }
}
