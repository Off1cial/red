#include "game/server/server.h"
#include "game/server/sv_pmove.h"

#include "game/shared/cvar.h"

#include "game/shared/pmove.h"

void SV_CreatePlayer(svclient_t* client)
{
  if (!client)
    return;
  
  client->playerid = gServer->playercount;
  svplayer_t* player = &gServer->players[client->playerid];
  player->clientid = client->id;
  player->respawn_time = 1.0f;
  // Physbodies?
}

void SV_PlayerThink(svplayer_t* player, float dt)
{
  
    playercmd_t* cmd = &player->cmd;
/*
    // 1. friction acts on EXISTING velocity, read/write in place
    SV_UserFriction(player->velocity, dt);

    // 2. wishdir/wishspeed computed fresh from this tick's input
    vec3_t forward, right, wishdir;
    QAngleVector(cmd->viewangles, forward);
    VectorCrossNorm(forward, VEC_AXIS_Y, right);

    wishdir[0] = forward[0] * cmd->pm_forward + right[0] * cmd->pm_side;
    wishdir[1] = 0;
    wishdir[2] = forward[2] * cmd->pm_forward + right[2] * cmd->pm_side;

    float wishspeed = VectorNormalise(wishdir); // normalizes wishdir in place, returns its old length
    if (wishspeed > sv_maxspeed) wishspeed = sv_maxspeed;

    // 3. accelerate — reads player->velocity, writes player->velocity
    SV_Accelerate(player->velocity, wishdir, wishspeed, dt);

    // 4. integrate position from the (now updated) velocity
    player->origin[0] += player->velocity[0] * dt;
    player->origin[1] += player->velocity[1] * dt;
    player->origin[2] += player->velocity[2] * dt;
*/
    PM_Move(cmd, player->origin, player->velocity, player->frame.lookdir, dt);
    // 5. THEN, and only then, snapshot into the outgoing frame
    VectorCopy(player->origin, player->frame.origin);
    VectorCopy(player->velocity, player->frame.velocity);
    //VectorCopy(forward, player->frame.lookdir);
    player->frame.pm_flags = player->onground ? PMF_ONGROUND : 0;
}


