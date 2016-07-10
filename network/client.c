/*
 * client.c - How to handle interactions with remote hosts in the role of client
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* The primary libevent header */
#include <event.h>

/* Private header file(s) */
#include "client.h"


/* Return the number of items in the table */
int clientlen (client_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Allocate and initialize a new descriptor to keep info about connection with a remote client application */
client_t * mkclient (void * server, int fd, char * hostname, int port, unsigned login, unsigned idle, unsigned ack,
		     unsigned showrecv, unsigned showsent, void * ssl)
{
  /* Buy memory now */
  client_t * c;
  if (! (c = (client_t *) calloc (sizeof (client_t), 1)))
    return NULL;

  /* Bless it */
  gettimeofday (& c -> uptime, NULL);

  c -> server      = server;

  c -> blocked     = 0;
  c -> state       = IDLE;

  /* Identifiers */
  c -> fd          = fd;
  c -> hostname    = strdup (hostname);
  c -> port        = port;

  c -> window      = 1;  /* FIXME!!! */
  c -> nexttrn     = 0;

  c -> room        = NULL;
  c -> roomsize    = 0;
  c -> hasread     = 0;

  c -> login . tv_sec  = login / 1000;
  c -> login . tv_usec = login % 1000 * 1000;

  c -> idle . tv_sec   = idle / 1000;
  c -> idle . tv_usec  = idle % 1000 * 1000;

  c -> ack . tv_sec    = ack / 1000;
  c -> ack . tv_usec   = ack % 1000 * 1000;

  /* The table of pending requests */
  c -> pending     = NULL;
  c -> maxresend   = 0;   /* FIXME!!! */

  /* Bytes counters */
  c -> sentbytes   = 0LL;
  c -> recvbytes   = 0LL;

  /* Operation counters (both outgoing and incoming) */
  c -> outgoing_op    = 0LL;
  c -> incoming_op    = 0LL;
  c -> supported_op   = 0LL;
  c -> unsupported_op = 0LL;
  c -> forwarded_op   = 0LL;
  c -> refused_op     = 0LL;

  /* Result counters (both outgoing and incoming) */
  c -> outgoing_rs    = 0LL;
  c -> incoming_rs    = 0LL;
  c -> supported_rs   = 0LL;
  c -> unsupported_rs = 0LL;
  c -> forwarded_rs   = 0LL;
  c -> refused_rs     = 0LL;

  /* Message counters */
  c -> received    = 0LL;
  c -> unexpected  = 0LL;
  c -> rights      = 0LL;
  c -> errors      = 0LL;
  c -> illegal     = 0LL;

  /* Operation counters */
  c -> expired     = 0LL;

  /* Result counters */
  c -> acks        = 0LL;
  c -> nacks       = 0LL;

  c -> ssl         = ssl;

  c -> showrecv    = showrecv;
  c -> showsent    = showsent;

  return c;
}


/* Free allocated memory and resources used to store a client descriptor */
void rmclient (client_t * c)
{
  if (! c)
    return;

  /* Release used file descriptor */
  if (c -> fd != -1)
    close (c -> fd);

  /* Release used memory buffer */
  if (c -> hostname)
    free (c -> hostname);
  if (c -> room)
    free (c -> room);

  if (EVENT_FD ((& c -> read_evt)) != -1)
    event_del (& c -> read_evt);

  if (EVENT_FD ((& c -> write_evt)) != -1)
    event_del (& c -> write_evt);

  /* Delete all pending timers */
  evtimer_del (& c -> idle_timer);
  evtimer_del (& c -> push_timer);

  /* Delete all pending requests */
  nomorereq (c -> pending);

  free (c);
}


/* Add a new item to the table of currently connected client applications on this server */
client_t ** moreclient (client_t * argv [], void * server, int fd, char * address, int port,
			unsigned login, unsigned idle, unsigned ack,
			unsigned showrecv, unsigned showsent, client_t ** more, void * ssl)
{
  int argc;
  client_t * c;

  if ((c = mkclient (server, fd, address, port, login, idle, ack, showrecv, showsent, ssl)))
    {
      /* the table is always NULL terminated */
      argc = clientlen (argv);
      argv = (client_t **) realloc (argv, (1 + argc + 1) * sizeof (client_t **));
      if (! argv)
        {
          rmclient (c);
          return NULL;
        }
      argv [argc ++] = c;
      argv [argc] = NULL;         /* do the table NULL terminated */

      if (more)
	* more = c;
    }
  return argv;
}


/* Remove an item from the table of the descriptors of the client applications */
client_t ** lessclient (client_t * argv [], client_t * c)
{
  int found;
  int j;
  int argc = clientlen (argv);

  if ((found = clientlookup (argv, c)) != -1)
    {
      rmclient (argv [found]);                      /* free the descriptor of the item found */
      for (j = found; j < argc - 1; j ++)        /* move pointers back one position */
        argv [j] = argv [j + 1];

      argv [j] = NULL;                           /* terminate the table */

      if (argc > 1)
        argv = (client_t **) realloc (argv, argc * sizeof (client_t *));
      else
        free (argv), argv = NULL;
    }
  return argv;
}


void nomoreclient (client_t * argv [])
{
  client_t ** c = argv;
  if (! argv)
    return;

  while (* c)
    rmclient (* c ++);
  free (argv);
}


/* Lookup for an item into the table of the client applications */
int clientlookup (client_t * argv [], client_t * c)
{
  int found = -1;

  while (c && argv && * argv)
    if (* argv ++ == c)
      return found + 1;
    else
      found ++;

  return -1;
}


/* Lookup for a file descriptor into the table of the client applications */
client_t * clientbyfd (client_t * argv [], int fd)
{
  client_t * found = NULL;

  while (argv && * argv)
    if ((* argv) -> fd  == fd)
      return * argv;
    else
      argv ++;

  return found;
}


/* Get a chunck of memory where to attempt to read and decode data */
int clientchuck (client_t * client)
{
  return (! client -> room && (client -> room = (char *) calloc (client -> roomsize = BUFCHUNK, 1))) ||
    (client -> roomsize - client -> hasread < BUFCHUNK && (client -> room = (char *) realloc (client -> room, client -> roomsize *= 2)));
}


/* Move surplus buffer */
void clientresize (client_t * c, int msglen)
{
  if (c -> room && c -> hasread - msglen)
    {
      memmove (c -> room, c -> room + msglen, c -> hasread - msglen);
      c -> hasread -= msglen;
    }
  else
    {
      if (c -> room)
	free (c -> room);
      c -> room = NULL;
      c -> roomsize = c -> hasread = 0;
    }
}
