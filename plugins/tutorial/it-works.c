/*
 * it-works.c - Based on 'heartbeat', add Web support to render the classic "It Works!" page
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
#include <time.h>

/* Private header file(s) */
#include "ark.h"
#include "ark-html.h"

/* Plugin declarations */

const char * variables = { "name version author date what copyright" };
const char * functions = { "boot halt web" };


/* Plugin definitions */

const char * name      = "it-works!";
const char * version   = "0.0.1";
const char * author    = "Rocco Carbone rocco@tecsiel.it";
const char * date      = __DATE__;
const char * what      = "A tutorial plugin to show how add web support";
const char * copyright = "(C) Copyright 2008 Rocco Carbone";


/* The place for default values */

/* Plugin name and logo */
#define PLGNAME       "it-works"
#define MASCOTTE_H    "penguin.h"


/* The place for static variables */

typedef struct
{
  struct event timer;
  struct timeval interval;
} heartbeat_t;

static heartbeat_t heartbeat;

/* Include all embedded icons and define the array of all embedded icons */
#include "it_works.h"
#include MASCOTTE_H

static image_t img1 = { "plgname",  plgname,  sizeof (plgname)  };
static image_t img2 = { "mascotte", mascotte, sizeof (mascotte) };
static image_t * embedded [] = { & img1, & img2, NULL };


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


/* Render all links that should be clickable in the header */
static void links (struct evbuffer * buf, char * page, char * sessionId)
{
  add_link (buf, URL_HOME,    LINK_HOME,    ! strcmp (page, LINK_HOME));
  add_link (buf, URL_PLUGINS, LINK_PLUGINS, ! strcmp (page, LINK_PLUGINS));
  add_link (buf, URL_ABOUT,   LINK_ABOUT,   ! strcmp (page, LINK_ABOUT));
}



/* The plugin home page */
static void plugin_homepage (struct evbuffer * buf, char * sessionId)
{
  begin_page (buf, (char *) name, URL_PLGNAME, URL_MASCOTTE);
  links (buf, LINK_HOME, sessionId);
  begin_content (buf);

  begin_onecol (buf);
  evbuffer_add_printf (buf, "<center><font size=+8><b>It Works!</b></font></center>\n");
  end_onecol (buf);

  end_page (buf, name, version, progname, NOW);
}


/* Render main plugin information */
static void plugin_info (struct evbuffer * buf)
{
  begin_plginfo (buf, "Plugin Info");

  string_info (buf, "Name",    (char *) name);
  string_info (buf, "Version", (char *) version);
  string_info (buf, "Author",  (char *) author);
  string_info (buf, "Date",    (char *) date);

  end_plginfo (buf);
}


/* Callback for /name/URL_HOME */
static void home (struct evhttp_request * req, void * arg)
{
  char * sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);
  struct evbuffer * buf = evbuffer_new ();
  char * url = evhttp_decode_uri (evhttp_request_uri (req));

  evhttp_parse_query (url, req -> input_headers);
  free (url);

  plugin_homepage (buf, sessionId);

  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Callback for /name/URL_ABOUT */
static void about (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  struct evbuffer * buf;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  buf = evbuffer_new ();

  begin_page (buf, (char *) name, URL_PLGNAME, URL_MASCOTTE);
  links (buf, LINK_ABOUT, sessionId);
  begin_content (buf);

  begin_onecol (buf);
  plugin_info (buf);
  end_onecol (buf);

  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Register all the required callbacks for web rendering */
static void registerweb (void)
{
  if (arkhttpd)
    {
      if (! evhttp_is_cbset (arkhttpd, URL_HOME))
	evhttp_set_cb (arkhttpd, URL_HOME, home, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_HOME1))
	evhttp_set_cb (arkhttpd, URL_HOME1, home, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_ABOUT))
	evhttp_set_cb (arkhttpd, URL_ABOUT, about, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_IMAGE))
	evhttp_set_cb (arkhttpd, URL_IMAGE, arkimage, embedded);
    }
}


/* Cleanup all previuosly registered callbacks for web rendering */
static void nomoreweb (void)
{
  if (arkhttpd)
    evhttp_del_cb (arkhttpd, URL_HOME),
      evhttp_del_cb (arkhttpd, URL_HOME1),
      evhttp_del_cb (arkhttpd, URL_IMAGE);
}


/* The place for public functions */

/* Will be called once when the plugin is loaded by the webadmin plugin */
int web (int argc, char * argv [])
{
  char * sessionId = (char *) evhttp_find_header (((struct evhttp_request *) argv [0]) -> output_headers, KEY_SESSIONID);
  struct evbuffer * buf = evbuffer_new ();

  plugin_homepage (buf, sessionId);

  evhttp_send_reply ((struct evhttp_request *) argv [0], HTTP_OK, "OK", buf);

  evbuffer_free (buf);

  /* Register callbacks for web access */
  registerweb ();

  return PLUGIN_OK;
}


/* Will be called once when the plugin is unloaded */
int halt (int argc, char * argv [])
{
  /* Delete the heartbeat timer */
  evtimer_del (& heartbeat . timer);

  /* Unregister web callbacks */
  nomoreweb ();

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

  /* Register web callbacks */
  registerweb ();

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
