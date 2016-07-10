/*
 * heartbeat.c - A very simply heartbeat ark-aware plugin
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
#include <time.h>

/* Private header file(s) */
#include "ark.h"

/* Plugin declarations */

const char * variables = { "name version author date what copyright" };
const char * functions = { "boot halt" };


/* Plugin definitions */

const char * name      = "heartbeat";
const char * version   = "0.0.1";
const char * author    = "Rocco Carbone rocco@tecsiel.it";
const char * date      = __DATE__;
const char * what      = "A heartbeart tutorial plugin";
const char * copyright = "(C) Copyright 2008 Rocco Carbone";


/* The place for static variables */

typedef struct
{
  struct event timer;
  struct timeval interval;
} heartbeat_t;

static heartbeat_t heartbeat;


/* Print heartbeat at given time interval */
static void heartbeat_callback (int unused, const short event, void * _heartbeat)
{
  static unsigned loops = 0;

  heartbeat_t * heartbeat = (heartbeat_t *) _heartbeat;
  time_t now = time (0);

  printf ("%s [%s]: %*.*s  -- MARK -- %u\n", progname, name, 24, 24, ctime (& now), ++ loops);

  /* Restart the heartbeat timer */
  evtimer_add (& heartbeat -> timer, & heartbeat -> interval);
}


/* The place for public functions */


/* Will be called once when the plugin is unloaded */
int halt (int argc, char * argv [])
{
  /* Delete the heartbeat timer */
  evtimer_del (& heartbeat . timer);

  return PLUGIN_OK;
}


/* Will be called once when the plugin is loaded */
int boot (int argc, char * argv [])
{
  /* heartbeart interval in seconds */
  heartbeat . interval . tv_sec  = 1;    /* 1 sec resolution */
  heartbeat . interval . tv_usec = 0;

  /* Define the heartbeat callback and start the timer at given time interval */
  evtimer_set (& heartbeat . timer, heartbeat_callback, & heartbeat);
  evtimer_add (& heartbeat . timer, & heartbeat . interval);

  return PLUGIN_OK;
}


int main (int argc, char * argv [])
{
  /* Perform one-time initialization of the libevent library */
  event_init ();

  /* Go for timer events */
  event_dispatch ();

  return 0;
}
