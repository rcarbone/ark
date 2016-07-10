/*
 * time.c - Utilities to handle timing concepts (eg. intrevals)
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
#include <sys/time.h>

/* Private header file(s) */
#include "ark.h"


/* The time since [t] in seconds */
time_t seconds (struct timeval * t)
{
  return t -> tv_sec + t -> tv_usec / 1000000.0;
}


/* The time since [t] in milliseconds */
time_t milliseconds (struct timeval * t)
{
  return t -> tv_sec * 1000.0 + t -> tv_usec / 1000.0;
}


/* The time since [t] in microseconds */
time_t microseconds (struct timeval * t)
{
  return t -> tv_sec * 1000000.0 + t -> tv_usec;
}


/* The time difference in seconds */
time_t delta_time_in_seconds (struct timeval * t2, struct timeval * t1)
{
  /* Compute delta in second */
  return t2 -> tv_sec - t1 -> tv_sec;
}


/* The time difference in milliseconds */
time_t delta_time_in_milliseconds (struct timeval * t2, struct timeval * t1)
{
  /* Compute delta in second and 1/1000's second units */
  time_t delta_seconds      = t2 -> tv_sec - t1 -> tv_sec;
  time_t delta_milliseconds = (t2 -> tv_usec - t1 -> tv_usec) / 1000;

  if (delta_milliseconds < 0)
    {
      delta_milliseconds += 1000;     /* manually carry a one from the seconds field */
      -- delta_seconds;
    }
  return delta_seconds * 1000 + delta_milliseconds;
}


/* The time difference in microseconds */
time_t delta_time_in_microseconds (struct timeval * t2, struct timeval * t1)
{
  /* Compute delta in second and 1/1000000's second units */
  time_t delta_seconds      = t2 -> tv_sec - t1 -> tv_sec;
  time_t delta_microseconds = t2 -> tv_usec - t1 -> tv_usec;

  if (delta_microseconds < 0)
    {
      delta_microseconds += 1000000;     /* manually carry a one from the seconds field */
      -- delta_seconds;
    }
  return delta_seconds * 1000000 + delta_microseconds;
}


/* Check for elapsed timeouts
 * t is the time an event was marked as occurred;
 * tout is the configured timeout
 */
int elapsed (struct timeval * t, struct timeval * tout)
{
  struct timeval now;
  gettimeofday (& now, NULL);

  return delta_time_in_microseconds (& now, t) >= microseconds (tout);
}


/* Time difference in days */
int days (time_t t1, time_t t2)
{
  return (t2 - t1) / SECS_PER_DAY;
}


/* Time difference in hours */
int hours (time_t t1, time_t t2)
{
  return (t2 - t1 - (days (t1, t2) * SECS_PER_DAY)) / SECS_PER_HOUR;
}


/* Time difference in minutes */
int mins (time_t t1, time_t t2)
{
  return (t2 - t1 - (days (t1, t2) * SECS_PER_DAY) - (hours (t1, t2) * SECS_PER_HOUR)) / SECS_PER_MIN;
}


void print_time_in_secs (struct timeval * t, char * label)
{
  time_t abst = t -> tv_sec;

  printf ("%s%*.*s\n", label, 24, 24, ctime (& abst));
  fflush (stdout);
}


/* -=--=--=--=--=--=--=--=--=--=--=--=--=--=--=- */

/* number of microseconds per second */
#define SECS_PER_DAY   86400
#define SECS_PER_HOUR  3600
#define SECS_PER_MIN   60
#define MSEC_PER_SEC   1000
#define USEC_PER_SEC   1000000
#define USEC_PER_MIN   (1000000 * SECS_PER_MIN)
#define USEC_PER_HOUR  (SECS_PER_HOUR * USEC_PER_SEC)


/* Number of microseconds since 00:00:00 January 1, 1970 UTC */
time_t time_now (void)
{
  struct timeval now;
  gettimeofday (& now, NULL);
  return (time_t) now . tv_sec * USEC_PER_SEC + now . tv_usec;
}


/* Return time in microseconds */
time_t time_usec (time_t t)
{
  return (time_t) t % USEC_PER_SEC;
}


/* Return time in milliseconds */
int time_msec (time_t t)
{
  return t % MSEC_PER_SEC;
}


/* Return time in seconds */
int time_sec (time_t t)
{
  return (t / MSEC_PER_SEC) % SECS_PER_MIN;
}


/* Return time in minutes */
int time_min (time_t t)
{
  return (t / (MSEC_PER_SEC * 60)) % 60;
}


/* Return time in hours */
int time_hour (time_t t)
{
  return (t / (MSEC_PER_SEC * SECS_PER_HOUR)) % 24;
}


/* Return time in days */
int time_day (time_t t)
{
  return (t / (MSEC_PER_SEC * SECS_PER_DAY));
}


/* Return a well formatted string with a time difference at millisecond resolution */
char * elapsed_time (struct timeval * start, struct timeval * stop)
{
  static char et [64];

  time_t elapsed = delta_time_in_milliseconds (stop, start);

  if (time_day (elapsed))
    sprintf (et, "%d days, %02d:%02d:%02d.%03ld",
	     time_day (elapsed), time_hour (elapsed), time_min (elapsed), time_sec (elapsed), time_usec (elapsed));
  else if (time_hour (elapsed))
    sprintf (et, "%02d:%02d:%02d.%03ld",
	     time_hour (elapsed), time_min (elapsed), time_sec (elapsed), time_usec (elapsed));
  else if (time_min (elapsed))
    sprintf (et, "%02d:%02d.%03ld", time_min (elapsed), time_sec (elapsed), time_usec (elapsed));
  else if (time_sec (elapsed))
    sprintf (et, "%d.%03d secs", time_sec (elapsed), time_msec (elapsed));
  else
    sprintf (et, "%3d msecs", time_msec (elapsed));

  return et;
}


/* Well formatted percentage */
char * xpercentage (unsigned long partial, unsigned long total)
{
#define ITEMS 10
  static char buffer [ITEMS] [64];
  static short k = -1;

#define DECIMALS 2
  float percent;

  k = (k + 1) % ITEMS;

  if (partial && total)
    {
      percent = (float) partial * 100 / (float) total;

      if (partial == total)
	sprintf (buffer [k], "(%3d%%) ", (int) percent);
      else
	sprintf (buffer [k], "(%4.*f%%)", DECIMALS, percent);  /* d.dd% */
    }
  else
    sprintf (buffer [k], " ");    /* just a single blank */

  return buffer [k];
}


void showbar (unsigned long received)
{
  static int bar = 0;

  bar = received % 8;
  if (bar == 0 || bar == 4)
    printf (" %lu |\r", received);
  else if (bar == 1 || bar == 5)
    printf (" %lu /\r", received);
  else if (bar == 2 || bar == 6)
    printf (" %lu -\r", received);
  else
    printf (" %lu \\\r", received);
  fflush (stdout);
}
