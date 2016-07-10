/*
 * request.c - Utilities to save message identifiers in a network enviroment
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

/* Private header file(s) */
#include "ark.h"
#include "request.h"


/* Create an identifier for a request with an unique timestamp at usecs resolution */
char * newreqid (void)
{
  static char reqid [REQIDLEN];

  struct timeval now;
  gettimeofday (& now, NULL);

  sprintf (reqid, "%.0f", now . tv_sec * 1000000.0 + now . tv_usec);

  return reqid;
}


/* How many items are in a NULL terminated argv? */
int reqlen (req_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Allocate and initialize a new request */
req_t * mkreq (char * id, unsigned trn, void * conn)
{
  /* Buy memory now */
  req_t * r;
  if (! (r = (req_t *) calloc (sizeof (req_t), 1)))
    return NULL;

  /* Bless it */
  r -> id       = strdup (id ? id : newreqid ());
  r -> trn      = trn;
  r -> conn     = conn;
  r -> data     = NULL;
  r -> data_org = NULL;

  return r;
}


/* Free allocated memory and resources used */
void rmreq (req_t * r)
{
  /* Delete all pending timers */
  evtimer_del (& r -> ack_timer);
  evtimer_del (& r -> resend_timer);

  argsfree (r -> data);
  argsfree (r -> data_org);

  if (r -> id)
    free (r -> id);

  if (r)
    free (r);
}


/* Add a request to the table of currently pending requests */
req_t ** morereq (req_t * argv [], req_t * req)
{
  int argc = reqlen (argv);
  argv = (req_t **) realloc (argv, (1 + argc + 1) * sizeof (req_t **));
  if (! argv)
    return NULL;
  argv [argc ++] = req;
  argv [argc]    = NULL;         /* make the table NULL terminated */

  return argv;
}


/* Lookup by reference for an item into the table of the pending requests */
static int reqbyref (req_t * argv [], req_t * r)
{
  int found = 0;

  while (argv && * argv)
    if (* argv ++ == r)
      return found;
    else
      found ++;

  return -1;
}


/* Remove a request from the table of currently pending requests */
req_t ** lessreq (req_t * argv [], req_t * req)
{
  int i;
  int j;
  int argc = reqlen (argv);

  if ((i = reqbyref (argv, req)) != -1)
    {
      rmreq (argv [i]);                  /* free the descriptor */
      for (j = i; j < argc - 1; j ++)    /* move pointers back one position */
        argv [j] = argv [j + 1];

      argv [j] = NULL;                   /* terminate the table */

      if (argc > 1)
        argv = (req_t **) realloc (argv, argc * sizeof (req_t *));
      else
        free (argv), argv = NULL;
    }

  return argv;
}


/* Cleanup the table of currently pending requests */
void nomorereq (req_t * argv [])
{
  req_t ** r = argv;

  while (r && * r)
    rmreq (* r ++);
  if (argv)
    free (argv);
}


/* Lookup for a request by its identifier */
req_t * reqbyid (req_t * argv [], char * id)
{
  while (argv && * argv)
    if (! strcmp ((* argv) -> id, id))
      return * argv;
    else
      argv ++;

  return NULL;
}
