#include "game/client/client.h"
#include "game/shared/cvar.h"


int CL_Init(client_t* client, const char* name)
{
  if (!client)
    return 0;

  memset(client, 0, sizeof(client_t));
  client->socket_tcp = -1;
  client->socket_udp = -1;

  int len = strlen(name);
  strncpy(client->name, name, len);
  client->name[len] = '\0';


  //Cvar_InitAll();
  return 1;
}
