/*
 * connection.c - How to handle interactions with remote hosts in the role of client
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
#include <values.h>
#include <unistd.h>
#include <string.h>

/* Private header file(s) */
#include "connection.h"


/* Return the # of elements in argv */
int connlen (conn_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Delete all pending timers */
static void conndeletetimers (conn_t * c)
{
  /* Delete all pending events */
  if (c -> write_evt . ev_flags && EVLIST_INIT)
    event_del (& c -> write_evt);

  if (c -> read_evt . ev_flags && EVLIST_INIT)
    event_del (& c -> read_evt);

  if (c -> connect_timer . ev_flags && EVLIST_INIT)
    evtimer_del (& c -> connect_timer);

  if (c -> reconnect_timer . ev_flags && EVLIST_INIT)
    evtimer_del (& c -> reconnect_timer);

  if (c -> idle_timer . ev_flags && EVLIST_INIT)
    evtimer_del (& c -> idle_timer);
}


/* Cleanup a connection */
void zeroconn (conn_t * c)
{
  /* Delete a pending timers */
  conndeletetimers (c);

  if (c -> fd != -1)
    close (c -> fd);
  c -> fd          = -1;
  c -> nexttrn     = 0;

  /* Delete all pending requests */
  nomorereq (c -> pending);
  c -> pending     = NULL;

  /* Memory to decode incoming messages */
  if (c -> room)
    free (c -> room);
  c -> room        = NULL;
  c -> roomsize    = 0;
  c -> hasread     = 0;

  /* Connection counters */
  c -> attempted   = 0LL;
  c -> established = 0LL;

  /* Disconnection counters */
  c -> memory      = 0LL;
  c -> osfault     = 0LL;
  c -> timeout     = 0LL;
  c -> decoding    = 0LL;
  c -> protocol    = 0LL;
  c -> denied      = 0LL;
  c -> hangup      = 0LL;

  /* Bytes counters */
  c -> sentbytes   = 0LL;
  c -> recvbytes   = 0LL;

  /* Operation counters (both outgoing and incoming) */
  c -> outgoing_op = 0LL;
  c -> incoming_op = 0LL;
  c -> supported_op   = 0LL;
  c -> unsupported_op = 0LL;
  c -> forwarded_op   = 0LL;
  c -> refused_op     = 0LL;

  /* Result counters (both outgoing and incoming) */
  c -> outgoing_rs = 0LL;
  c -> incoming_rs = 0LL;
  c -> supported_rs   = 0LL;
  c -> unsupported_rs = 0LL;
  c -> forwarded_rs   = 0LL;
  c -> refused_rs     = 0LL;

  /* Message counters */
  c -> received    = 0LL;
  c -> unexpected  = 0LL;
  c -> rights      = 0LL;

  /* Operation counters */
  c -> expired     = 0LL;
  c -> overruns    = 0LL;

  /* Flow control counters */
  c -> flowwin     = 0LL;
  c -> flowtrn     = 0LL;
  c -> flownet     = 0LL;

  /* Result counters */
  c -> goods       = 0LL;
  c -> errors      = 0LL;
  c -> acks        = 0LL;
  c -> nacks       = 0LL;
  c -> supported   = 0LL;
  c -> unsupported = 0LL;
  c -> illegal     = 0LL;

  /* Statistics */
  c -> min         = MAXDOUBLE;
  c -> max         = 0.0;
  c -> avg         = 0.0;
}


/* Create a new descriptor of the connection with a remote application */
static conn_t * mkconn (void * host, unsigned port, unsigned maxresend)
{
  conn_t * c = calloc (sizeof (conn_t), 1);

  if (! c)
    return NULL;

  c -> fd = -1;
  zeroconn (c);

  /* Bless it */
  gettimeofday (& c -> uptime, NULL);

  c -> host      = host;
  c -> port      = port;   /* local port (if != 0) */
  c -> maxresend = maxresend;

  return c;
}


/* Free memory associated to a descriptor of a connection */
static void rmconn (conn_t * c)
{
  if (! c)
    return;

  zeroconn (c);

  free (c);
}


/* Add a connection to the table of currently active connections */
conn_t ** moreconn (conn_t * argv [], void * smsc, unsigned port, unsigned maxretries)
{
  int argc;
  conn_t * c;

  if ((c = mkconn (smsc, port, maxretries)))
    {
      argc = connlen (argv);
      argv = (conn_t **) realloc (argv, (1 + argc + 1) * sizeof (conn_t **));
      if (! argv)
        {
	  rmconn (c);
          return NULL;
        }
      argv [argc ++] = c;
      argv [argc]    = NULL;         /* make the table NULL terminated */
    }
  return argv;
}


/* Free memory associated to the table of arguments */
void nomoreconn (conn_t * argv [])
{
  conn_t ** c = argv;

  while (c && * c)
    rmconn (* c ++);
  if (argv)
    free (argv);
}


/* Remove an item from the table of the descriptors to the connections */
conn_t ** lessconn (conn_t * argv [], conn_t * c)
{
  int i;
  int j;
  int argc = connlen (argv);

  if ((i = connlookup (argv, c)) != -1)
    {
      rmconn (argv [i]);                 /* free the descriptor */
      for (j = i; j < argc - 1; j ++)    /* move pointers back one position */
        argv [j] = argv [j + 1];

      argv [j] = NULL;                   /* terminate the table */

      if (argc > 1)
        argv = realloc (argv, argc * sizeof (conn_t *));  /* size is argc not argc-1 because of ending NULL */
      else
        free (argv), argv = NULL;
    }

  return (argv);
}


/* Lookup for an item into the table of the descriptors to the connections */
int connlookup (conn_t * argv [], conn_t * c)
{
  int i = -1;

  if (! argv || ! c)
    return -1;

  while (* argv)
    if (* argv ++ == c)
      return i + 1;
    else
      i ++;

  return -1;
}


/* Get a chunck of memory where to attempt to read and decode data */
int connchuck (conn_t * conn)
{
  return (! conn -> room && (conn -> room = (char *) calloc (conn -> roomsize = BUFCHUNK, 1))) ||
    (conn -> roomsize - conn -> hasread < BUFCHUNK && (conn -> room = (char *) realloc (conn -> room, conn -> roomsize *= 2)));
}


/* Move surplus buffer */
void connresize (conn_t * c, int msglen)
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


/* Change the status of the connection to 'state' */
void nextstate (conn_t * conn, int state)
{
  conn -> state = state;

  /* Manage a disconnection (both locally or remotely initiated) */
  switch (state)
    {
    case DENIED:
      gettimeofday (& conn -> down, NULL);

      /* Delete all pending timers and events */
      conndeletetimers (conn);

      /* Close the file descriptor */
      close (conn -> fd);
      conn -> fd = -1;

      break;

    default:
      break;
    }
}
