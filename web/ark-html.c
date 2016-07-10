/*
 * ark-html.c - Utilities to add html support to ark-aware plugins
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
 * Valeria Lorenzetti <valeria.lorenzetti /at/ gmail /dot/ com>
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
#include <stdio.h>
#include <string.h>

/* Private header file(s) */
#include "ark.h"
#include "ark-html.h"


/* Header
 *  <html>
 *    <head>...</head>
 *    <body>
 */
static void html_header (struct evbuffer * buf, char * title)
{
  evbuffer_add_printf (buf,
                       "<html>\n"
                       " <head>\n"
                       "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\n"
		       "  <link rel=\"shortcut icon\" href=%s>\n"
                       "  <title>%s</title>\n"
		       "  <link href=%s rel=stylesheet type=text/css>\n"
                       " </head>\n"
		       " \n<!-- body -->\n"
                       " <body>\n",
		       URL_FAVICON, title, URL_CSS);
}


/* What should be immediately seen on the MAIN HOME PAGE by the web admin plugin
 *  Banner and links
 */
static void main_heading (struct evbuffer * buf)
{
  evbuffer_add_printf (buf,
                       "  <div id=page>\n"
                       "   <div id=banner><img src=%s></div>\n"
                       "   <div id=caption><img src=%s></div>\n"
		       "   <div id=projectlogo><img src=%s></div>\n"
		       "   <!-- Begin Links -->\n"
                       "   <div id=menu>\n"
                       "    <ul>\n",
		       URL_BANNER, URL_MAIN_BANNER, URL_PROJECT_LOGO);
}


/* Start of a page: Add to the banner a animal mascotte and define links */
static void begin_heading (struct evbuffer * buf, char * imgtitle, char * mascotte)
{
  evbuffer_add_printf (buf,
		       "  <div id=page>\n"
		       "   <div id=banner><img src=%s></div>\n"
		       "   <!-- Immagine Title -->\n"
		       "   <div id=imgtitle><img src=%s></div>\n"
		       "   <!-- Mascotte -->\n"
		       "   <div id=mascotte><img src=%s height=80></div>\n"
		       "   <!-- Begin Links -->\n"
		       "   <div id=menu>\n"
		       "    <ul>\n",
		       URL_BANNER, imgtitle, mascotte);
}


/* Add a link (button) to the list of links */
void add_link (struct evbuffer * buf, char * href, char * linkname, int active)
{
  if (active)
    evbuffer_add_printf (buf, "     <li class=active><a href=%s>%s</a></li>\n", href, linkname);
  else
    evbuffer_add_printf (buf, "     <li ><a href=%s>%s</a></li>\n", href, linkname);
}


/* Add a button to right side for Login/Logout */
void add_login (struct evbuffer * buf, char * href, char * linkname, char * subtitle)
{
  evbuffer_add_printf (buf, "     <ri class=active><a href=%s title=\"%s\">%s</a></ri>\n", href, subtitle, linkname);
}


/* Add a link (button) to the right side */
void add_rlink (struct evbuffer * buf, char * href, char * linkname)
{
  evbuffer_add_printf (buf, "     <ri class=active><a href=%s>%s</a></ri>\n", href, linkname);
}


/* Terminate the list of links */
static void end_heading (struct evbuffer * buf)
{
  evbuffer_add_printf (buf,
		       "    </ul>\n"
		       "   </div>\n"
		       "   <!--End Links -->\n\n");
}


/* Start a page content */
void begin_content (struct evbuffer * buf)
{
  end_heading (buf);

  evbuffer_add_printf (buf,
		       "\n<!-- Begin Content -->\n"
		       "<div id=content>\n");
}


/*
 * single column layout
 * -----------------------------
 *
 *           Content
 *            page
 *
 * -----------------------------
 */

/* Start a single column page */
void begin_onecol (struct evbuffer * buf)
{
  evbuffer_add_printf (buf, "<div id=content_onecol>\n");
}


/* End a single column page */
void end_onecol (struct evbuffer * buf)
{
  evbuffer_add_printf (buf, "</div>\n");
}


/*
 * Double columns layout
 * ---------------------------------
 *                 |
 *   Content       |    Menu
 *                 |
 *                 |  - Logo
 *                 |  - Changelog
 *                 |  - ...
 *                 |
 * --------------------------------
 */

/* Start left column */
void begin_left (struct evbuffer * buf, char * title)
{
  evbuffer_add_printf (buf,
		       "<div id=col_left>\n"
		       " <h4>%s</h4>\n",
		       title);
}


/* Render text as a paragraph on the left column */
void left (struct evbuffer * buf, char * text)
{
  evbuffer_add_printf (buf,
                       " <p class=intro>\n"
                       "%s\n"
                       " </p>\n", text);
}


/* End left column */
void end_left (struct evbuffer * buf)
{
  evbuffer_add_printf (buf, "</div>\n");
}


/* Start right column */
void begin_right (struct evbuffer * buf)
{
  evbuffer_add_printf (buf, "<div id=col_rght>\n");
}


/* Render plugin version, release date and its clickable logo */
void pretty_version (struct evbuffer * buf, char * mascotte, char * version, char * release, char * url_onclick)
{
  evbuffer_add_printf (buf,
                       "<p class=dl onclick=\"window.location.href='%s'\">\n"
                       "<img src=%s height=80>\n"
                       "<br>\n"
                       "<strong>ver %s</strong>\n"
                       "<br>\n"
                       "Current release: %s\n"
                       "</p>\n",
                       url_onclick, mascotte, version, release);
}


/* Add a title (eg. "Changelog" or "News") */
void add_changelog (struct evbuffer * buf, char * title)
{
  evbuffer_add_printf (buf, "<h4>%s</h4>\n", title);
}


/* Add an item to the Changelog */
void new_log (struct evbuffer * buf, char * title, char * date, char * text)
{
  evbuffer_add_printf (buf,
		       "<div id=newsitem>\n"
		       "<b>%s</b>\n"
		       "<i>%s</i>\n"
		       "<p>\n"
		       "%s\n"
		       "</p>\n"
		       "</div>\n",
		       title, date, text);
}


/* End the right column */
void end_right (struct evbuffer * buf)
{
  evbuffer_add_printf (buf, "</div>\n");
}


/*
 * layout:
 * -----------------------------
 *  Menu  |
 *   - a  |    Content
 *   - b  |
 *   - c  |
 *        |
 * -----------------------------
 */

/* Start a left column */
void begin_leftmenu (struct evbuffer * buf, char * title)
{
  evbuffer_add_printf (buf,
		       "\n<!-- Begin left menu -->\n"
		       "<div id=col2left>\n"
		       " <h4>%s</h4>\n"
		       "  <ul>\n",
		       title);
}


/* Add an item to the menu in the left column */
void letf_item (struct evbuffer * buf, char * href, char * item_name, int active)
{
  if (active)
    evbuffer_add_printf (buf,
                         "   <li class=active><a href=%s>%s</a></li>\n",
                         href, item_name);
  else
    evbuffer_add_printf (buf,
                         "   <li><a href=%s>%s &raquo;</a></li>\n",
                         href, item_name);
}


/* End a left column */
void end_leftmenu (struct evbuffer * buf)
{
  evbuffer_add_printf (buf,
		       "  </ul>\n"
		       "</div>\n"
		       "<!-- End left menu -->\n\n");
}


/* Start a right column */
void begin_itempage (struct evbuffer * buf, char * title)
{
  evbuffer_add_printf (buf,
		       "<div id=col2right>\n"
		       "<center><h3>%s</h3></center>\n",
		       title);
}


/* End a right column */
void end_itempage (struct evbuffer * buf)
{
  evbuffer_add_printf (buf, "</div>\n");
}


/* Functions related to plugin information */
void begin_plginfo (struct evbuffer * buf, char * title)
{
  evbuffer_add_printf (buf,
		       "<table align=center cellpadding=1 cellspacing=0 border=0>\n"
                       " <tr bgcolor=%s>\n"
		       "  <td colspan=2 align=center><font face=verdana size=2><b>%s</b></font></td>\n"
		       " </tr>\n",
		       HDCOLOR, title);
}


void string_info (struct evbuffer * buf, char * name, char * value)
{
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=200 align=left><font face=verdana size=2>&nbsp;<b>%s</b></font></td>\n"
                       "  <td width=350 align=left><font face=verdana size=2>%s</font></td>\n"
                       " </tr>\n",
                       BGCOLOR_0, name, value);
}


void int_info (struct evbuffer * buf, char * name, int value)
{
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=200 align=left><font face=verdana size=2>&nbsp;<b>%s</b></font></td>\n"
                       "  <td width=350 align=left><font face=verdana size=2>%d</font></td>\n"
                       " </tr>\n",
                       BGCOLOR_0, name, value);
}


void end_plginfo (struct evbuffer * buf)
{
  evbuffer_add_printf (buf, "</table>\n<br><br>\n");
}


/* End page content */
static void end_content (struct evbuffer * buf)
{
  evbuffer_add_printf (buf,
                       "<div style=\"clear:both\"></div>\n"
                       "</div>\n"
		       "<!-- End Content -->\n\n");
}


/*
 * Lastest information on all the pages (put here all that seems relevant)
 * (eg. name of the plugin with its version, name of the application and its version, current time, etc...)
 */
static void banner (struct evbuffer * buf, const char * plugname, const char * plugversion, char * applname, time_t now)
{
  evbuffer_add_printf (buf,
		       "  <!-- Final banner -->\n"
		       " <div id=footer>\n"
		       "  <p>\n"
		       "   Created by '%s' ['%s ver. %s'] on %-*.*s\n<br>\n"
		       "   %s %s - %s<br>\n"
		       "   %s<br>\n"
		       "  </p>\n"
		       " </div>\n",
		       applname, plugname, plugversion, 24, 24, ctime (& now), PKG_COPYRIGHT, PKG_AUTHOR, PKG_RIGHTS, PKG_LICENSE);
}


/* Close the body */
static void html_end_body (struct evbuffer * buf, const char * plugname, const char * plugversion, char * appname)
{
  evbuffer_add_printf (buf,
                       " \n<!-- Created by %s ver. %s [%s] -->\n"
		       " \n<!-- End BODY -->\n"
                       " </body>\n"
                       "</html>\n",
                       plugname, plugversion, appname);
}


/* Begin the main page (only used by the webadmin plugin) */
void begin_mainpage (struct evbuffer * buf, char * title)
{
  html_header (buf, title);
  main_heading (buf);
}


/* Begin a page */
void begin_page (struct evbuffer * buf, char * title, char * imgtitle, char * mascotte)
{
  html_header (buf, title);
  begin_heading (buf, imgtitle, mascotte);
}


/* End a page */
void end_page (struct evbuffer * buf, const char * plugname, const char * plugversion, char * appname, time_t now)
{
  end_content (buf);
  banner (buf, plugname, plugversion, appname, now);
  html_end_body (buf, plugname, plugversion, appname);
}


/* Set the Content-Type */
void set_header (struct evhttp_request * req, char * con_type)
{
  evhttp_clear_headers (req -> output_headers);
  evhttp_add_header (req -> output_headers, "Content-Type", con_type);
}


/* Lookup for a given item in the array of embedded icons */
static image_t * imglookup (image_t * argv [], char * item)
{
  if (! argv || ! item)
    return NULL;

  while (argv && * argv)
    if (! strcmp ((* argv ++) -> name, item))
      return (* -- argv);
  return  NULL;
}


/* Generic callback for both static and dynamically generated images */
void arkimage (struct evhttp_request * req, void * embedded)
{
  struct evbuffer * buf = evbuffer_new ();

  char * png;
  image_t * img;

  /* Parse the parameters passed with the request */
  char * uri = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (uri, req -> input_headers);
  free (uri);

  png = (char *) evhttp_find_header (req -> input_headers, "png");
  if ((img = imglookup (embedded, png)))
    set_header (req, "image/png"),
      evbuffer_add (buf, img -> content, img -> size);
  else
    {
      FILE * fp;
      char c [1];

      if (png && (fp = fopen (png, "r")))
        while (fread (& c, 1, 1, fp))
          evbuffer_add (buf, c, 1);

      if (fp)
	fclose (fp);
      if (png)
	unlink (png);
    }

  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}



/* Well formatted percentage */
char * percentage (counter_t partial, counter_t total)
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
	sprintf (buffer [k], "&nbsp;(%3d%%)", (int) percent);
      else
	sprintf (buffer [k], "&nbsp;(%4.*f%%)", DECIMALS, percent);  /* d.dd% */
    }
  else
    sprintf (buffer [k], "&nbsp;");    /* just a single blank */

  return buffer [k];
}


char * counter (counter_t c)
{
#define HOWMANY 20
  static char buffer [HOWMANY] [64];
  static short k = -1;

  k = (k + 1) % HOWMANY;

  sprintf (buffer [k], "%llu", c);

  return buffer [k];
}


/* Tell how long the object has been running in a format suitable for web rendering */
char * htmluptime (struct timeval * started)
{
  static char html [64];

  time_t now = NOW;

  int d = days (started -> tv_sec, now);
  int h = hours (started -> tv_sec, now);
  int m = mins (started -> tv_sec, now);
  int s = (now - started -> tv_sec) % 60;

  memset (html, '\0', sizeof (html));
  if (d)
    sprintf (html, "%3d&nbsp;days,&nbsp;&nbsp;%2d:%02d:%02d", d, h, m, s);
  else
    sprintf (html, "%2d:%02d:%02d", h, m, s);

  return html;
}
