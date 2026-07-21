#pragma once


/*
 *
 *    This file is to serve as a platform-independent wrapper 
 *    for networking functions so that the engine can function
 *    on windows and linux systems
 *
 *
 */


#include <unistd.h>        
#include <arpa/inet.h>    
#include <netinet/in.h>  
#include <sys/socket.h> 

#define NET_SOCK_UDP SOCK_DGRAM
#define NET_SOCK_TCP SOCK_STREAM

#ifndef SERVER_PORT
#define SERVER_PORT 27015
#endif

typedef int netsocket_t;

typedef struct netaddr_t
{
  unsigned int ip;
  unsigned short port;
} netaddr_t;

typedef struct netmsg_t
{
  char msg[256];
  int failure; // quick parsing - error = cancel whatever received
} netmsg_t;

typedef enum netdomain_t
{
  NET_DOMAIN_IPV4 = AF_INET,
  NET_DOMAIN_IPV6 = AF_INET6,
  NET_DOMAIN_LOCAL = AF_LOCAL
} netdomain_t;

typedef enum netprotocol_t
{
  NET_PROTOCOL_UDP,
  NET_PROTOCOL_TCP,
} netprotocol_t;


netsocket_t pltNetSocketCreate(netdomain_t domain, netprotocol_t protocol);


void  pltNetSocketClose     (netsocket_t socket);
void  pltNetSocketBind      (netsocket_t socket, const netaddr_t* addr);
void  pltNetSocketListen    (netsocket_t socket);
void  pltNetSocketAccept();
void  pltNetSocketConnect();

void  pltNetSend();
void  pltNetRecv();



