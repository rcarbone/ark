/*
 * heartbeat-libevt.c - A very simply heartbeat libevent-based program
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

/* The primary libevent header */
#include <event.h>


typedef struct
{
  struct event timer;
  struct timeval interval;
} heartbeat_t;


/* Print heartbeat at given time interval */
static void heartbeat_callback (int unused, const short event, void * _heartbeat)
{
  static unsigned loops = 0;

  heartbeat_t * heartbeat = (heartbeat_t *) _heartbeat;
  time_t now = time (0);

  printf ("%*.*s  -- MARK -- %u\n", 24, 24, ctime (& now), ++ loops);

  /* Restart the heartbeat timer */
  evtimer_add (& heartbeat -> timer, & heartbeat -> interval);
}


int main (int argc, char * argv [])
{
  heartbeat_t heartbeat;

  /* heartbeart interval in seconds */
  heartbeat . interval . tv_sec  = 1;    /* 1 sec resolution */
  heartbeat . interval . tv_usec = 0;

  /* Perform one-time initialization of the libevent library */
  event_init ();

  /* Define the heartbeat callback and start the timer at given time interval */
  evtimer_set (& heartbeat . timer, heartbeat_callback, & heartbeat);
  evtimer_add (& heartbeat . timer, & heartbeat . interval);

  /* Go for timer events */
  event_dispatch ();

  return 0;
}
