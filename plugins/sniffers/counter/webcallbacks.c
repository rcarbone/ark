/*
 * webcallbacks.c - Callbacks for the ark-aware plugin 'counter' to render html pages
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


/* Colors to render html tables */
#define DEFAULT_HDCOLOR  "#F4A460"


/* A place for detailed description in the homepage */
static char * intro = "Counter is a packet sniffer for pcap-aware network interface(s)";

/* Table of colors to render html tables */
static char * bgcolors [] = { "#eeeeee", "#dddddd", "#cccccc" };

static char * nextcolor (void)
{
  static int colors = sizeof (bgcolors) / sizeof (bgcolors [0]);
  static int color = -1;

  color = (color + 1) % colors;

  return (bgcolors [color]);
}


static char * mycounter (counter_t c)
{
#define HOWMANY 20
  static char buffer [HOWMANY] [64];
  static short k = -1;

  k = (k + 1) % HOWMANY;

  sprintf (buffer [k], "%.0f", (double) c);

  return buffer [k];
}


static void plugin_info (struct evbuffer * buf)
{
  begin_plginfo (buf, "Plug-in info");

  string_info (buf, "Name",    (char *) name);
  string_info (buf, "Version", (char *) version);
  string_info (buf, "Author",  (char *) author);
  string_info (buf, "Date",    (char *) date);

  end_plginfo (buf);
}


/* Add links at the top of the page */
static void links (struct evbuffer * buf, char * page, char * sessionId)
{
  if (sessionId)
    {
      char url [512];

      sprintf (url, "%s?%s=%s", URL_HOME, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_HOME, ! strcmp (page, LINK_HOME));

      sprintf (url, "%s?%s=%s", URL_PACKETS, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_PACKETS, ! strcmp (page, LINK_PACKETS));

      sprintf (url, "%s?%s=%s", URL_PLUGINS, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_PLUGINS, ! strcmp (page, LINK_PLUGINS));

      sprintf (url, "%s?%s=%s", URL_ABOUT, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_ABOUT, ! strcmp (page, LINK_ABOUT));
    }
  else
    {
      add_link (buf, URL_HOME,    LINK_HOME,    ! strcmp (page, LINK_HOME));
      add_link (buf, URL_PACKETS, LINK_PACKETS, ! strcmp (page, LINK_PACKETS));
      add_link (buf, URL_PLUGINS, LINK_PLUGINS, ! strcmp (page, LINK_PLUGINS));
      add_link (buf, URL_ABOUT,   LINK_ABOUT,   ! strcmp (page, LINK_ABOUT));
    }
}


/* Include all embedded icons and define the array of all embedded icons */
#include "plgname.h"
#include MASCOTTE_H

static image_t img1 = { "plgname",  plgname,  sizeof (plgname)  };
static image_t img2 = { "mascotte", mascotte, sizeof (mascotte) };
static image_t * embedded [] = { & img1, & img2, NULL };


/* The plugin home page */
static void plugin_homepage (struct evbuffer * buf, char * sessionId)
{
  begin_page (buf, (char *) name, URL_PLGNAME, URL_MASCOTTE);
  links (buf, LINK_HOME, sessionId);
  begin_content (buf);

  begin_left (buf, PLGNAME);
  left (buf, intro);
  end_left (buf);

  begin_right (buf);

  if (sessionId)
    {
      char url [512];
      sprintf (url, "%s?%s=%s", URL_ABOUT, KEY_SESSIONID, sessionId);
      pretty_version (buf, URL_MASCOTTE, (char *) version, "Jun 14 2008", url);
    }
  else
    pretty_version (buf, URL_MASCOTTE, (char *) version, "Jun 14 2008", URL_ABOUT);

  add_changelog (buf, "Changelog");
  new_log (buf, "Start development", "Sat Jun 14 2008", "Blah blah blah");
  end_right (buf);

  end_page (buf, name, version, progname, NOW);
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


/* Callback for /name/URL_PACKETS */
static void packets (struct evhttp_request * req, void * arg)
{
  char * sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);
  struct evbuffer * buf = evbuffer_new ();
  char * url = evhttp_decode_uri (evhttp_request_uri (req));

  interface_t ** argv = interfaces;
  counter_t totalrecv = 0;
  counter_t totalunic = 0;
  counter_t totalbroa = 0;
  counter_t totalmult = 0;
  counter_t totaldrop = 0;

  evhttp_parse_query (url, req -> input_headers);
  free (url);

  begin_page (buf, (char *) name, URL_PLGNAME, URL_MASCOTTE);
  links (buf, LINK_PACKETS, sessionId);
  begin_content (buf);

  begin_onecol (buf);

#define PAGE_TITLE  "Interfaces currently active"

  /* Name | Uptime | Received | Unicast | Broadcast | Multicast | Dropped */
#define COLS   7

  /* Table header and title */
  evbuffer_add_printf (buf,
                       "<center>\n"
                       " <table align=center cellpadding=1 cellspacing=0 border=0>\n"
                       "  <tr bgcolor=%s align=center>\n"
                       "   <td colspan=%d>%s</td>\n"
                       "  </tr>\n",
		       DEFAULT_HDCOLOR, COLS, PAGE_TITLE);

  /* One row description */
  evbuffer_add_printf (buf,
		       "  <tr bgcolor=%s align=center>\n"
		       "   <td>&nbsp;<b>Name</b>&nbsp;</td>\n"
		       "   <td>&nbsp;<b>Uptime</b>&nbsp;</td>\n"
		       "   <td>&nbsp;<b>Received</b>&nbsp;</td>\n"
		       "   <td>&nbsp;<b>Unicast</b>&nbsp;</td>\n"
		       "   <td>&nbsp;<b>Broadcast</b>&nbsp;</td>\n"
		       "   <td>&nbsp;<b>Multicast</b>&nbsp;</td>\n"
		       "   <td>&nbsp;<b>Dropped</b>&nbsp;</td>\n"
		       "  </tr>\n",
		       DEFAULT_HDCOLOR);

  /* Empty Row */
  evbuffer_add_printf (buf,
                       "  <tr bgcolor=%s align=center>\n"
                       "   <td colspan=%d>&nbsp;</td>\n"
                       "  </tr>\n",
		       nextcolor (), COLS);

  while (argv && * argv)
    {
      evbuffer_add_printf (buf,
			   "   <td align=center>%s</td>\n"
			   "   <td align=center>%s</td>\n"
			   "   <td align=left>&nbsp;#%s</td>\n"
			   "   <td align=left>&nbsp;#%s</td>\n"
			   "   <td align=left>&nbsp;#%s</td>\n"
			   "   <td align=left>&nbsp;#%s</td>\n"
			   "   <td align=left>&nbsp;#%s</td>\n"
			   "  </tr>\n",
			   (* argv) -> name,
			   htmluptime (& (* argv) -> started),
			   mycounter ((* argv) -> received),
			   mycounter ((* argv) -> unicast),
			   mycounter ((* argv) -> broadcast),
			   mycounter ((* argv) -> multicast),
			   mycounter (0));

      totalrecv += (* argv) -> received;
      totalunic += (* argv) -> unicast;
      totalbroa += (* argv) -> broadcast;
      totalmult += (* argv) -> multicast;
      totaldrop += 0;
      argv ++;
    }

  /* Empty Row */
  evbuffer_add_printf (buf,
                       "  <tr bgcolor=%s align=center>\n"
                       "   <td colspan=%d>&nbsp;</td>\n"
                       "  </tr>\n",
		       nextcolor (), COLS);
  
  /* Table footer with Great Totals */
  evbuffer_add_printf (buf,
		       "  <tr bgcolor=%s align=center>\n"
		       "   <td align=center>Total</td>\n"
		       "   <td>&nbsp;</td>\n"
		       "   <td align=left>&nbsp;#%s</td>\n"
		       "   <td align=left>&nbsp;#%s</td>\n"
		       "   <td align=left>&nbsp;#%s</td>\n"
		       "   <td align=left>&nbsp;#%s</td>\n"
		       "   <td align=left>&nbsp;#%s</td>\n"
		       "  </tr>\n"
		       " </table>\n"
		       "</center>\n"
		       "<br><br>\n",
		       DEFAULT_HDCOLOR,
		       mycounter (totalrecv),
		       mycounter (totalunic),
		       mycounter (totalbroa),
		       mycounter (totalmult),
		       mycounter (totaldrop));

  end_onecol (buf);

  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Callback for /name/URL_ABOUT */
static void about (struct evhttp_request * req, void * arg)
{
  char * sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);
  struct evbuffer * buf = evbuffer_new ();
  char * url = evhttp_decode_uri (evhttp_request_uri (req));

  evhttp_parse_query (url, req -> input_headers);
  free (url);

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

      if (! evhttp_is_cbset (arkhttpd, URL_PACKETS))
	evhttp_set_cb (arkhttpd, URL_PACKETS, packets, NULL);

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
    evhttp_del_cb (arkhttpd,   URL_HOME),
      evhttp_del_cb (arkhttpd, URL_HOME1),
      evhttp_del_cb (arkhttpd, URL_PACKETS),
      evhttp_del_cb (arkhttpd, URL_ABOUT),
      evhttp_del_cb (arkhttpd, URL_IMAGE);
}


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
