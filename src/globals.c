/*
 * globals.c - A safe place for all the (few!!!) ark's global variables
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
#include <string.h>

/* libevent header file to add basic support for HTTP serving */
#include <evhttp.h>

/* Private header file(s) */
#include "ark.h"


/* The name of the game */
char * progname = NULL;

/* Time the program was started */
struct timeval boottime;

/* The NULL table of loaded plugins implemented via shared objects */
plugin_t ** loaded = NULL;

/* The NULL table of plugins search pathnames */
char ** plgpaths = NULL;

/* Currently running embedded HTTP Daemon (if any) */
struct evhttp * arkhttpd = NULL;


/* Set time the application started */
void started (void)
{
  gettimeofday (& boottime, NULL);
}


/* Return time in secs the application started */
time_t since (void)
{
  return boottime . tv_sec;
}


/* Return plugins search pathnames */
char ** shobjs (void)
{
  return plgpaths;
}
