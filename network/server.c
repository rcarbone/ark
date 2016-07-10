/*
 * server.c - Utilities to implement event-driven network servers
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *                                 _
 *                       __ _ _ __| | __
 *                      / _` | '__| |/ /
 *                     | (_| | |  |   <
 *                      \__,_|_|  |_|\_\
 *
 * 'ark', is the home for asynchronous libevent-based plugins
 *
 *
 *                  (C) Copyright 2007-2008
 *         Rocco Carbone <rocco /at/ tecsiel /dot/ it>
 *
 * Released under the terms of GNU General Public License
 * at version 3;  see included COPYING file for details
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *
 */


/* Operating System header file(s) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

/* Private header file(s) */
#include "server.h"


/* Return the # of items in the table */
int serverlen (server_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Allocate and initialize a new server to listen for incoming connection at '[address]:port' */
static server_t * mkserver (char * address, unsigned backlog, unsigned maxconns,
			    char * id, char * password, char * type,
			    unsigned login, unsigned idle, unsigned ack)
{
  char * c;
  int port = DEFAULT_PORT;
  int fd;
  server_t * l;

  /* Buy memory now */
  if (! (l = calloc (sizeof (server_t), 1)))
    return NULL;

  /* Address resolution */
  if (! address)
    address = ANYINTERFACE;

  if ((c = strchr (address, ':')))
    * c = '\0',
      port = atoi (c + 1);

  /* Create an endpoint to listen for incoming TCP connections on given address and port */
  if ((fd = incoming (address, port, backlog)) < 0)
    {
      if (c)
	* c = ':';
      free (l);
      return NULL;
    }

  /* Identifiers */
  gettimeofday (& l -> uptime, NULL);

  /* TCP/IP addressing parameters */
  l -> address     = strdup (address);
  l -> port        = port;
  l -> fd          = fd;

  /* Max # of connections */
  l -> backlog     = backlog;
  l -> maxconns    = maxconns;
  l -> clients     = NULL;

  /* Authentication */
  l -> id         = id ? strdup (id) : NULL;
  l -> password   = password ? strdup (password) : NULL;
  l -> type       = type ? strdup (type) : NULL;

  /* Protocol timeouts in milliseconds */
  l -> login . tv_sec  = login / 1000;
  l -> login . tv_usec = login % 1000 * 1000;
  l -> idle . tv_sec   = idle / 1000;
  l -> idle . tv_usec  = idle / 1000 * 1000;
  l -> ack . tv_sec    = ack / 1000;
  l -> ack . tv_usec   = ack / 1000 * 1000;

  /* Connection counters */
  l -> accepted    = 0LL;
  l -> rejected    = 0LL;

  /* Disconnection counters */
  l -> memory      = 0LL;
  l -> osfault     = 0LL;
  l -> timeout     = 0LL;
  l -> decoding    = 0LL;
  l -> protocol    = 0LL;
  l -> denied      = 0LL;
  l -> hangup      = 0LL;

  /* Bytes counters */
  l -> recvbytes   = 0LL;
  l -> sentbytes   = 0LL;

  /* Global counters for the Push Service */
  l -> recvop      = 0LL;
  l -> sentacks    = 0LL;
  l -> sentnacks   = 0LL;

  /* Global counters for the Pull Service */
  l -> sentop      = 0LL;
  l -> recvacks    = 0LL;
  l -> recvnacks   = 0LL;

  /* FIXME!!! */
  /* Operation counters (both outgoing and incoming) */
  l -> supported_op   = 0LL;
  l -> unsupported_op = 0LL;
  l -> forwarded_op   = 0LL;
  l -> refused_op     = 0LL;

  /* Result counters (both outgoing and incoming) */
  l -> supported_rs    = 0LL;
  l -> unsupported_rs  = 0LL;
  l -> forwarded_rs    = 0LL;
  l -> refused_rs      = 0LL;

  /* Operation counters */
  l -> received    = 0LL;
  l -> unexpected  = 0LL;
  l -> rights      = 0LL;
  l -> errors      = 0LL;
  l -> illegal     = 0LL;

  /* Operations counters (supported + unsupported) */
  l -> expired     = 0LL;
  l -> forwarded   = 0LL;
  l -> refused     = 0LL;

  if (c)
    * c = ':';

  return l;
}


/* Free allocated memory and resources used to store a server descriptor */
static void rmserver (server_t * l)
{
  if (! l)
    return;

  /* Release the endpoint */
  if (l -> fd)
    close (l -> fd);

  /* Release used memory buffer */
  if (l -> address)
    free (l -> address);

  if (l -> id)
    free (l -> id);
  if (l -> password)
    free (l -> password);
  if (l -> type)
    free (l -> type);

  nomoreclient (l -> clients);

  free (l);
}


/* Lookup for an entry in the table of the NULL terminated argv */
server_t * serverbyaddress (char * address, int port, server_t * argv [])
{
  while (address && argv && * argv)
    if (! strcmp (address, (* argv) -> address) && (* argv) -> port == port)
      return * argv;
    else
      argv ++;
  return NULL;
}


/* Add a server to the table of currently active servers */
server_t ** moreservers (server_t * argv [], char * address, unsigned backlog, unsigned maxconns,
			     char * id, char * password, char * type,
			     unsigned login, unsigned idle, unsigned ack)
{
  int argc;
  server_t * l;

  if ((l = mkserver (address, backlog, maxconns, id, password, type, login, idle, ack)))
    {
      /* The table is always NULL terminated */
      argc = serverlen (argv);
      argv = (server_t **) realloc (argv, (1 + argc + 1) * sizeof (server_t **));
      if (! argv)
        {
          rmserver (l);
          return NULL;
        }
      argv [argc ++] = l;
      argv [argc] = NULL;         /* do the table NULL terminated */
    }
  return argv;
}


/* Free memory related to table of servers */
void nomoreservers (server_t * argv [])
{
  server_t ** l = argv;
  if (! argv)
    return;

  while (* l)
    rmserver (* l ++);
  free (argv);
}


/* Return total # of incoming connections */
counter_t server_calls (server_t * l)
{
  return l -> accepted + l -> rejected;
}


/* Return total # of disconnections (both remotely and locally initiated) */
counter_t server_dropped (server_t * l)
{
  return l -> memory + l -> osfault + l -> timeout + l -> decoding + l -> protocol + l -> denied + l -> hangup;
}


/* Return total # of remotely initiated disconnections */
counter_t server_remote (server_t * l)
{
  return l -> hangup;
}


/* Return total # of locally initiated disconnections */
counter_t server_local (server_t * l)
{
  return l -> memory + l -> osfault + l -> timeout + l -> decoding + l -> protocol + l -> denied;
}


/* Return total # of Results sent */
counter_t server_sentrs (server_t * l)
{
  return l -> sentacks + l -> sentnacks;
}


/* Return total # of Results received */
counter_t server_recvrs (server_t * l)
{
  return l -> recvacks + l -> recvnacks;
}


/* Return total # of Operations (both incoming and outgoing) */
counter_t server_op (server_t * l)
{
  return l -> recvop + l -> sentop;
}


/* Return total # of Results (both outgoing and incoming) */
counter_t server_rs (server_t * l)
{
  return l -> sentacks + l -> sentnacks + l -> recvacks + l -> recvnacks;
}


/* Return total # of outgoing Operations over all servers */
counter_t server_senttotop (server_t * argv [])
{
  counter_t argc = 0; while (argv && * argv) argc += (* argv ++) -> sentop; return argc;
}


/* Return total # of incoming Operations over all servers */
counter_t server_recvtotop (server_t * argv [])
{
  counter_t argc = 0; while (argv && * argv) argc += (* argv ++) -> recvop; return argc;
}


/* Return total # of connections over all servers */
int server_csps (server_t * argv [])
{
  int argc = 0; while (argv && * argv) argc += clientlen ((* argv ++) -> clients); return argc;
}


/* Return total # of connections so far established over all servers */
int server_established (server_t * argv [])
{
  int argc = 0; while (argv && * argv) argc += (* argv ++) -> accepted; return argc;
}
