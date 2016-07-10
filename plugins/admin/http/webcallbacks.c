/*
 * webcallbacks.c - Callbacks for the ark-aware pluging 'webadmin' to render html pages
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


#define LATEST date

#include "milestones.c"


/* Table of colors to render rows in html tables */
static char * bgcolors [] =
  {
    "#eeeeee",
    "#dddddd",
    "#cccccc"
  };

static char * nextcolor (void)
{
  static int colors = sizeof (bgcolors) / sizeof (bgcolors [0]);
  static int color = -1;

  color = (color + 1) % colors;

  return (bgcolors [color]);
}


/* Include all embedded icons and define the array of all embedded icons */
#include "project.h"
#include "logo.h"
#include "banner.h"
#include "favicon.h"
#include "background.h"
#include "header.h"
#include "footer.h"
#include "button.h"
#include "plghome.h"
#include "wwwadmin.h"
#include MASCOTTE_H

static image_t img1  = { "project",    project,    sizeof (project)    };
static image_t img2  = { "logo",       logo,       sizeof (logo)       };
static image_t img3  = { "banner",     banner,     sizeof (banner)     };
static image_t img4  = { "favicon",    favicon,    sizeof (favicon)    };
static image_t img5  = { "header",     header,     sizeof (header)     };
static image_t img6  = { "footer",     footer,     sizeof (footer)     };
static image_t img7  = { "background", background, sizeof (background) };
static image_t img8  = { "button",     button,     sizeof (button)     };
static image_t img9  = { "plghome",    plghome,    sizeof (plghome)    };
static image_t img10 = { "plgname",    plgname,    sizeof (plgname)    };
static image_t img11 = { "mascotte",   mascotte,   sizeof (mascotte)   };

static image_t * embedded [] =
  { & img1, & img2, & img3, & img4, & img5, & img6, & img7, & img8, & img9, & img10, & img11, NULL };


/* Callback for the css */
static void css (struct evhttp_request * req, void * arg)
{
#include "css.h"
  struct evbuffer * buf = evbuffer_new ();

  set_header (req, "text/css");
  evbuffer_add (buf, css, sizeof (css));

  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Show the table of running plugins */
static void get_running (struct evbuffer * buf, char * sessionId)
{
  char ** argv;
  char ** a;
  plugin_t ** p_run;

  /* Table of plugins available for loading */
  argv = available_plugins (shobjs ());

  /* Title */
  evbuffer_add_printf (buf,
                       "<center>\n"
                       "<!-- Begin Running List -->\n"
                       "<h4>Running plugins</h4>\n");

  /*  _____________________________________________________________________  */
  /* |   Status    |    Name    |  Description  |  Explore it!  |  Unload  | */
  /* |_____________|____________|_______________|_______________|__________| */

  /* Begin Table & Form */
  evbuffer_add_printf (buf,
		       "<!-- Form -->\n"
		       "<form name=input action=unload method=get>\n"
		       "<!-- Table -->\n"
		       "<table align=center cellpadding=1 cellspacing=0 border=1 rules=cols frame=box>\n"
		       " <tr bgcolor=%s align=center>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>Status</b></font></td>\n"
		       "  <td width=%d align=left><font face=verdana size=2><b>&nbsp;&nbsp;Name</b></font></td>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>Description</b></font></td>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>Explore it!</b></font></td>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>Unload</b></font></td>\n"
		       "</tr>\n",
		       HDCOLOR, 80, 100, 350, 80, 80);

  if (run -> enable_admin && lookup_session (& run -> sessions, sessionId))
    evbuffer_add_printf (buf,
			 " <input type=hidden name=%s value=%s>\n", KEY_SESSIONID, sessionId);

  /* Per ogni plugin 'disponibile' controllo se esiste un plugin 'running' con lo stesso path */
  a = argv;
  while (argv && * argv)
    {
      /* Plugin attualmente running */
      p_run = running ();
      while (p_run && * p_run)
        {
	  /* Trovato un plugin RUNNING */
          if (! strcmp (* argv, (* p_run) -> path))
            {
	      /* Status, Name, Description */
              evbuffer_add_printf (buf,
                                   "<tr bgcolor=%s>\n"
                                   " <td align=center><font face=verdana size=2 color=green>[loaded]</font></td>\n"
                                   " <td align=left><font face=verdana size=2>&nbsp;&nbsp;<b>%s</b></font></td>\n"
                                   " <td align=left><font face=verdana size=2>&nbsp;&nbsp;%s</font></td>\n",
				   nextcolor (), pluginname (* p_run),
				   lookup_description (& run -> desc, get_basename ((* p_run) -> path)));

	      /* WEB supportato: visualizzo logo homepage */
              if (function (HOMEPAGE, (* p_run) -> symbols) || ! strcmp (pluginname (* p_run), name))
                {
                  if (run -> enable_admin && lookup_session (& run -> sessions, sessionId))
                    evbuffer_add_printf (buf,
                                         " <td align=center>\n"
                                         "  <a href=/%s?%s=%s title=\"%s's homepage\"><img border=0 src=%s></a>\n"
                                         " </td>\n",
                                         pluginname (* p_run), KEY_SESSIONID, sessionId,
					 pluginname (* p_run), URL_PLG_HOME);
		  else
                    evbuffer_add_printf (buf,
                                         " <td align=center>\n"
                                         "  <a href=/%s title=\"%s's homepage\"><img border=0 src=%s></a>\n"
                                         " </td>\n",
                                         pluginname (* p_run), pluginname (* p_run), URL_PLG_HOME);
                }
              else
		/* No web */
                evbuffer_add_printf (buf,
                                     " <td align=center>&nbsp;</td>\n");


	      /* Unload (per webadmin non disponibile) */
              if (! strcmp (pluginname (* p_run), name))
                evbuffer_add_printf (buf,
				     " <td align=center>&nbsp;</td>\n"
				     "</tr>\n");
              else
                evbuffer_add_printf (buf,
                                     " <td align=center>"
                                     "  <input type=checkbox name=%s value=unload>\n"
                                     " </td>\n"
                                     "</tr>\n",
                                     pluginname (* p_run));
	      break;
            }
          p_run ++;
        }
      argv ++;
    }
  argsfree (a);

  /* Se ci sono plugin running oltre ad webadmin, mostro il pulsante di unload */
  if (pluginsno () > 1)
    evbuffer_add_printf (buf,
                         " <tr bgcolor=%s>\n"
                         "  <td colspan=5 align=right>"
			 "   <input type=submit value=Unload>&nbsp;</td>\n"
                         "   </form></td>\n"
                         " </tr>\n"
                         "</table>\n",
                         run -> hdcolor);
  else
    evbuffer_add_printf (buf,
                         " <tr bgcolor=%s>\n"
                         "  <td colspan=5 align=center>&nbsp;</td>\n"
                         "   </form></td>\n"
                         " </tr>\n"
                         "</table>\n",
                         run -> hdcolor);

  evbuffer_add_printf (buf, "<br>");
}


/* Render the page of plugins available for loading */
static void get_available (struct evbuffer * buf, char * plugin_error, char * sessionId)
{

  char ** dirs;
  char ** argv;
  char ** a;
  int found;
  plugin_t ** p_run;

  /* Title */
  evbuffer_add_printf (buf,
                       "<center>\n"
                       "<!-- Begin AVAILABLE List -->\n"
                       "<h4>Available plugins</h4>\n");

  /*  _________________________________________________________  */
  /* |   Status    |    Name    |  Description    |  Load it!  | */
  /* |_____________|____________|_________________|____________| */

  /* Begin Table & Form */
  evbuffer_add_printf (buf,
                       "<!-- Form -->\n"
                       "<form name=input action=unload method=get>\n"
                       "<!-- Table -->\n"
                       "<table align=center cellpadding=1 cellspacing=0 border=1 rules=cols frame=box>\n"
                       " <tr bgcolor=%s align=center>\n"
                       "  <td width=%d align=center><font face=verdana size=2><b>Status</b></font></td>\n"
                       "  <td width=%d align=left><font face=verdana size=2><b>&nbsp;&nbsp;Name</b></font></td>\n"
                       "  <td width=%d align=center><font face=verdana size=2><b>Description</b></font></td>\n"
                       "  <td width=%d align=center><font face=verdana size=2><b>Load it!</b></font></td>\n"
                       "</tr>\n",
                       HDCOLOR, 100, 100, 350, 80);

  /* Build an array of directories with shared-objects */
  dirs = shobjs ();
  while (dirs && * dirs)
    {
      /* Plugins available just in a single directory passed as argument */
      argv = available_in_dir (* dirs);

      /* None found */
      if (argslen (argv) < 1)
        {
          dirs ++;
          continue;
        }

      evbuffer_add_printf (buf,
                           " <tr bgcolor=%s>\n"
                           "  <td colspan=3 align=left><font face=verdana size=2>"
                           "   <b>Directory:</b> <i>%s</i></font></td>\n"
			   "  <td>&nbsp;</td>\n"
                           " </tr>\n",
                           HDCOLOR, * dirs);

      p_run = running ();

      a = argv;
      while (argv && * argv)
        {
          p_run = running ();
          found = 0;
	  while (p_run && * p_run)
            {
              if (! strcmp (* argv, (* p_run) -> path))
                {
                  found ++;
                  break;
                }
	      p_run ++;
            }

          if (! found)
            {
              if (plugin_error != NULL && ! strcmp (* argv, plugin_error))
                {
                  evbuffer_add_printf (buf,
                                       "<tr bgcolor=%s>\n"
                                       " <td align=center><font face=verdana size=2 color=red>"
				       "  [<b>Error!</b>]</font></td>\n"
                                       " <td align=left><font face=verdana size=2>&nbsp;&nbsp;%s</font></td>\n"
                                       " <td align=left><font face=verdana size=2>&nbsp;&nbsp;%s</font></td>\n"
                                       " <td align=center><font face=verdana size=2>",
				       nextcolor (), get_basename ((char *) * argv),
				       lookup_description (& run -> desc, get_basename ((char *) * argv)));

		  if (run -> enable_admin && lookup_session (& run -> sessions, sessionId))
                    evbuffer_add_printf (buf,
                                         "  <a href=%s?path=%s&%s=%s>[load]</a></font></td>\n"
                                         "</tr>\n",
                                         URL_LOAD, * argv, KEY_SESSIONID, sessionId);
                  else
                    evbuffer_add_printf (buf,
                                         "  <a href=%s?path=%s>[load]</a></font></td>\n"
                                         "</tr>\n",
                                         URL_LOAD, * argv);
                }
              else
                {
                  evbuffer_add_printf (buf,
                                       "<tr bgcolor=%s>\n"
                                       " <td align=center><font face=verdana size=2 color=red>[not loaded]</font></td>\n"
                                       " <td align=left><font face=verdana size=2>&nbsp;&nbsp;%s</font></td>\n"
                                       " <td align=left><font face=verdana size=2>&nbsp;&nbsp;%s</font></td>\n"
                                       " <td align=center><font face=verdana size=2>",
				       nextcolor (), get_basename ((char *) * argv),
				       lookup_description (& run -> desc, get_basename ((char *) * argv)));

                  if (run -> enable_admin && lookup_session (& run -> sessions, sessionId))
                    evbuffer_add_printf (buf,
                                         "  <a href=%s?path=%s&%s=%s>[load]</a></font></td>\n"
                                         "</tr>\n",
                                         URL_LOAD, * argv, KEY_SESSIONID, sessionId);
		  else
                    evbuffer_add_printf (buf,
                                         "  <a href=%s?path=%s>[load]</a></font></td>\n"
                                         "</tr>\n",
                                         URL_LOAD, * argv);
                }
            }
          argv ++;  	  /* Next plugin */
        }
      argsfree (a);
      dirs ++;
    }

  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=4 align=center>&nbsp;</td>\n"
                       " </tr>\n"
                       "</table>\n",
                       HDCOLOR);

  evbuffer_add_printf (buf, "<br><br><br>");
}


/* Render the table of all available plugins */
static void plugins_list (struct evbuffer * buf, char * plugin_error, char * sessionId)
{
  get_running (buf, sessionId);
  get_available (buf, plugin_error, sessionId);
}


/* Render the main project homepage */
static void main_homepage (struct evbuffer * buf)
{
  struct timeval s;
  struct utsname machine;
  struct hostent * he;
  char * hostname = NULL;

  uname (& machine);

  if ((he = gethostbyname (machine . nodename)))
    hostname = he -> h_name;

  /* Now */
  time_t now = NOW;

  /* Time the application was started */
  time_t started = since ();

  /* # of available plugins */
  int nr_avail = argslen (available_plugins (shobjs()));

  /* # of running plugins */
  int howmany = pluginsno ();

  int bg = 0;
  evbuffer_add_printf (buf,
		       "<table align=center cellpadding=1 cellspacing=0 border=0>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=3 align=center>&nbsp;</td>\n"
                       " </tr>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=3 align=center><font face=verdana size=2><b>"
                       "   %s ver %s - on %s (%s %s)</b></font></td>\n"
                       " </tr>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td colspan=3 align=center>&nbsp;</td>\n"
                       " <tr bgcolor=%s>\n"
                       "  <td width=200 align=center>&nbsp;</td>\n"
                       "  <td width=330 align=center>&nbsp;</td>\n"
                       "  <td width=200 align=center>&nbsp;</td>\n"
		       " </tr>\n",
		       run -> hdcolor, run -> hdcolor,
		       progname, PKG_VERSION, hostname ? hostname : machine . nodename,
		       ! strcmp (machine . sysname, "SunOS") ? "Solaris" : machine . sysname,
		       machine . release,
		       run -> hdcolor, run -> bgcolors [(bg ++) % argslen (run -> bgcolors)]);

  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=200 align=center><font face=verdana size=2><b>Now</b></font></td>\n"
                       "  <td width=330 align=center><font face=verdana size=2><b>Started</b></font></td>\n"
                       "  <td width=200 align=center><font face=verdana size=2><b>Uptime</b></font></td>\n"
		       " </tr>\n",
		       run -> bgcolors [(bg ++) % argslen (run -> bgcolors)]);

  s . tv_sec = since ();

  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td align=center><font face=verdana size=2> %s </font></td>\n"
                       "  <td align=center><font face=verdana size=2> %s </font></td>\n"
		       "  <td align=center><font face=verdana size=2> %s </font></td>\n"
		       " </tr>\n",
		       run -> bgcolors [(bg ++) % argslen (run -> bgcolors)],
		       ctime (& now), ctime (& started), htmluptime (& s));

 evbuffer_add_printf (buf,
		      " <tr bgcolor=%s>\n"
		      "  <td colspan=3>&nbsp;</td>\n"
		      " </tr>\n",
		      run -> bgcolors [(bg ++) % argslen (run -> bgcolors)]);

 evbuffer_add_printf (buf,
		      " <tr bgcolor=%s>\n"
		      "  <td align=center>&nbsp;</td>\n"
		      "  <td align=center><font face=verdana size=2><b>Plugins</b></font></td>\n"
		      "  <td align=center>&nbsp;</td>\n"
		      " </tr>\n",
		      run -> bgcolors [(bg ++) % argslen (run -> bgcolors)]);

 evbuffer_add_printf (buf,
		      " <tr bgcolor=%s>\n"
		      "  <td width=200 align=center><font face=verdana size=2>Available - <b>%d</b></font></td>\n"
		      "  <td align=center>&nbsp;</td>\n"
		      "  <td width=200 align=center><font face=verdana size=2>Running - <b>%d</b></font></td>\n"
		      " </tr>\n",
		      run -> bgcolors [(bg ++) % argslen (run -> bgcolors)], nr_avail, howmany);

 evbuffer_add_printf (buf,
		      " <tr bgcolor=%s>\n"
		      "  <td colspan=3>&nbsp;</td>\n"
		      " </tr>\n"
		      " <tr bgcolor=%s>\n"
		      "  <td colspan=3>&nbsp;</td>\n"
		      " </tr>\n",
		      run -> bgcolors [(bg ++) % argslen (run -> bgcolors)], run -> hdcolor);

 evbuffer_add_printf (buf,
		      "</table>\n"
		      "<br><br><br><br>\n");
}


/* Render project information */
static void project_info (struct evbuffer * buf)
{
  struct timeval s;
  s . tv_sec = since ();

  begin_plginfo (buf, "Info");

  string_info (buf, "Program name",  (char *) progname);
  string_info (buf, "Version",       PKG_VERSION);
  string_info (buf, "Author",        PKG_AUTHOR);
  string_info (buf, "Release Date",  PKG_RELEASE_DATE);
  string_info (buf, "Uptime",        htmluptime (& s));

  end_plginfo (buf);

  evbuffer_add_printf (buf,
		       "<strong>\n"
		       "This program is open-source software; you can redistribute it and/or modify "
		       "it under the terms of the GNU General Public License as published by "
		       "the Free Software Foundation; either version 3 of the License, or "
		       "(at your option) any later version.<br><br>"
		       "This program is distributed in the hope that it will be useful, "
		       "but WITHOUT ANY WARRANTY; without even the implied warranty of "
		       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
		       "included GNU General Public License in file COPYING for more details."
                       "</strong>\n");
}


/* Render a form page to login on the system */
static void form_login (struct evbuffer * buf)
{
#define W1 80
#define W2 200

  /* Begin Form */
  evbuffer_add_printf (buf,
		       "\n<!-- Begin login form -->\n"
		       "<center>\n"
		       " <form name=input action=\"%s\" method=get>\n"
		       "  <table align=center cellpadding=1 cellspacing=1 "
		       " border=3 frame=border rules=none bordercolor=#FF6600>\n",
		       URL_CHECK);

  /* Blank row */
  evbuffer_add_printf (buf, " <tr bgcolor=%s>\n<td colspan=2>&nbsp;</td>\n</tr>\n", BGCOLOR_0);

  /* User */
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=%d align=right><font face=verdana size=2>"
                       "   &nbsp;<b>User</b></font></td>\n"
                       "  <td width=%d align=left><font face=verdana size=2>"
                       "   &nbsp;<input type=text name=%s size=20></font></td>\n"
                       " </tr>\n",
                       BGCOLOR_0, W1, W2, KEY_USER);

  /* Password */
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s>\n"
                       "  <td width=%d align=right><font face=verdana size=2>"
                       "   &nbsp;<b>Password</b></font></td>\n"
                       "  <td width=%d align=left><font face=verdana size=2>"
                       "   &nbsp;<input type=password name=%s size=20></font></td>\n"
                       " </tr>\n",
                       BGCOLOR_0, W1, W2, KEY_PWD);

  /* Blank row */
  evbuffer_add_printf (buf, " <tr bgcolor=%s>\n <td colspan=2>&nbsp;</td>\n</tr>\n", BGCOLOR_0);

  /* Bottone 'Send' */
  evbuffer_add_printf (buf,
                       " <tr bgcolor=%s align=center>\n"
                       "  <td colspan=2>"
                       "   <input type=submit value=Login>"
                       "  </td>"
                       "</tr>\n",
                       BGCOLOR_0);

  /* Blank row */
  evbuffer_add_printf (buf, " <tr bgcolor=%s>\n <td colspan=2>&nbsp;</td>\n</tr>\n", BGCOLOR_0);

  /* End Form */
  evbuffer_add_printf (buf,
                       " </table>\n"
                       "</form>\n"
                       "<!-- End form Login -->\n\n"
                       "<br><br>\n"
                       "</center>\n");
}


/* Render a table of valid accounts */
static void accounts_list (struct evbuffer * buf)
{
  char * user;
  char ** a;
  int n = 1;

  /* Table accounts */
  evbuffer_add_printf (buf,
		       " \n<!-- Begin Accounts -->\n"
		       "<div id=col2left>\n"
		       " <center>\n"
		       " <h4>Accounts</h4>\n"
                       "  <table align=center cellpadding=1 cellspacing=1 border=1 rules=cols frame=box>\n");

  evbuffer_add_printf (buf,
		       " <tr bgcolor=%s>\n"
		       "  <td width=%d align=center><font face=verdana size=2>#</font></td>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>Users</b></font></td>\n"
		       " </tr>\n",
		       HDCOLOR, 20, 100);

  a = run -> accounts;
  while (a && * a)
    {
      user = strdup (* a ++);
      evbuffer_add_printf (buf,
			   " <tr bgcolor=%s>\n"
			   "  <td align=center><font face=verdana size=2>%d</font></td>\n"
			   "  <td align=center><font face=verdana size=2><b>%s</b></font></td>\n"
			   " </tr>\n",
			   BGCOLOR_0, n ++, strtok (user, ":"));
      free (user);
      user = NULL;
    }

  evbuffer_add_printf (buf,
		       "  </table>\n"
		       " </center>\n"
		       "</div>\n"
		       " <!-- End Accounts -->\n<br>\n");
}


/* Render a table of running sessions */
static void sessions_list (struct evbuffer * buf)
{
  char ** i;
  session_t * s;

  /* Table sessions */
  evbuffer_add_printf (buf,
                       "\n<!-- Begin Sessions -->\n"
		       "<div id=col2right>\n"
  		       "<center>\n"
		       " <h4>Valid Sessions</h4>\n"
                       "  <table align=center cellpadding=1 cellspacing=1 border=1 rules=cols frame=box>\n");

  evbuffer_add_printf (buf,
		       " <tr bgcolor=%s>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>User</b></font></td>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>Session Id</b></font></td>\n"
		       "  <td width=%d align=center><font face=verdana size=2><b>Time</b></font></td>\n"
		       " </tr>\n",
		       HDCOLOR, 80, 200, 80);

  i = run -> sessionIds;
  while (i && * i)
    {
      s = lookup_session (& run -> sessions, * i ++);

      evbuffer_add_printf (buf,
                           " <tr bgcolor=%s>\n"
                           "  <td align=center><font face=verdana size=2><b>%s</b></font></td>\n"
			   "  <td align=center><font face=verdana size=2>%s</font></td>\n"
			   "  <td align=center><font face=verdana size=2>%s</font></td>\n"
			   " </tr>\n",
                           BGCOLOR_0, s -> user, s -> sessionId, htmluptime (& s -> start));
    }

  evbuffer_add_printf (buf,
                       "  </table>\n"
		       "  <br><br><br>\n"
		       " </center>\n"
		       "</div>\n"
		       " <!-- End Sessions -->\n\n");
}


/* Render all links that should be clickable in the header */
static void links (struct evbuffer * buf, char * page, char * sessionId)
{
  if (run -> enable_admin && lookup_session (& run -> sessions, sessionId))
    {
      /* Keep the sessionId and use it */

      char url [256];

      /* Left buttons */
      sprintf (url, "%s?%s=%s", URL_MAIN_HOME, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_HOME, ! strcmp (page, LINK_HOME));

      sprintf (url, "%s?%s=%s", URL_PLUGINS, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_PLUGINS, ! strcmp (page, LINK_PLUGINS));

      sprintf (url, "%s?%s=%s", URL_MAIN_ABOUT, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_ABOUT,  ! strcmp (page, LINK_ABOUT));

      /* Right buttons */
      sprintf (url, "%s?%s=%s", URL_LOGOUT, KEY_SESSIONID, sessionId);
      add_login (buf, url, LOGOUT_BUTTON, DO_LOGOUT_BUTTON);

      sprintf (url, "%s?%s=%s", URL_ACCOUNTS, KEY_SESSIONID, sessionId);
      add_rlink (buf, url, LINK_ACCOUNTS);
    }
  else
    {
      add_link (buf, URL_MAIN_HOME,  LINK_HOME,    ! strcmp (page, LINK_HOME));
      add_link (buf, URL_PLUGINS,    LINK_PLUGINS, ! strcmp (page, LINK_PLUGINS));
      add_link (buf, URL_MAIN_ABOUT, LINK_ABOUT,   ! strcmp (page, LINK_ABOUT));

      if (run -> enable_admin)
	add_login (buf, URL_LOGIN, LOGIN_BUTTON, DO_LOGIN_BUTTON);
    }
}


/* Render a not found page (generic callback for all not registered urls */
static void page_notfound (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  struct evbuffer * buf;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  buf = evbuffer_new ();

  begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
  links (buf, LINK_HOME, sessionId);
  begin_content (buf);

  begin_onecol (buf);
  evbuffer_add_printf (buf,
                       "<br><br><br>\n"
                       "<center>\n"
                       "<font color=\"#CC0033\" face=verdana size=3>\n"
                       "<b>Page non found:</b>\n"
                       "<br><br>\n"
                       "%s\n"
                       "</font>\n"
                       "<br><br><br>\n",
                       url);
  end_onecol (buf);

  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_NOTFOUND, "Not Found", buf);

  free (url);
  evbuffer_free (buf);
}


/* Render an error page in the event some mandatory parameter is missing */
static void error_page (struct evhttp_request * req, void * arg, char * par, char * sessionId)
{
  struct evbuffer * buf = evbuffer_new ();

  begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
  links (buf, LINK_HOME, sessionId);
  begin_content (buf);

  begin_onecol (buf);
  evbuffer_add_printf (buf,
                       "<br><br>\n<center>\n"
                       "<font color=\"#CC0033\" face=verdana size=3>\n"
                       "<b>BAD REQUEST\n"
		       "<br><br><br>\n"
		       "Missing parameter:&nbsp;&nbsp; %s</b></font>\n"
		       "<br><br><br><br><br><br>\n"
                       "<br><br><br>\n",
		       par);
  end_onecol (buf);
  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_BADREQUEST, "Missing parameters", buf);

  evbuffer_free (buf);
}


/* Render the initial page with system uptime, # of available and # of running plugins */
static void initial_page (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  struct evbuffer * buf;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  buf = evbuffer_new ();

  begin_mainpage (buf, ARKTITLE);
  links (buf, LINK_HOME, sessionId);
  begin_content (buf);

  begin_onecol (buf);
  main_homepage (buf);
  end_onecol (buf);

  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Render the login page */
static void login_page (struct evhttp_request * req, void * arg)
{
  if (run -> enable_admin)
    {
      struct evbuffer * buf = evbuffer_new ();

      begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
      add_link (buf, URL_MAIN_HOME, LINK_HOME,    1);
      add_link (buf, URL_PLUGINS,   LINK_PLUGINS, 0);
      begin_content (buf);

      begin_onecol (buf);
      form_login (buf);
      end_onecol (buf);

      end_page (buf, name, version, progname, NOW);
      evhttp_send_reply (req, HTTP_OK, "Login required", buf);

      evbuffer_free (buf);
    }
  else
    page_notfound (req, arg);
}


/* Check login accounts */
static void check_page (struct evhttp_request * req, void * arg)
{
  char * url;
  char * user = NULL;
  char * pass = NULL;

  char sessionId [256];
  session_t * session;
  struct timeval now;

  if (run -> enable_admin)
    {
      struct evbuffer * buf;

      url = evhttp_decode_uri (evhttp_request_uri (req));
      evhttp_parse_query (url, req -> input_headers);
      free (url);

      user = (char *) evhttp_find_header (req -> input_headers, KEY_USER);
      pass = (char *) evhttp_find_header (req -> input_headers, KEY_PWD);

      if (user && (! strcmp (WILDCARD, user) || (pass && accountok (run -> accounts, user, pass))))
	{
	  /* Need a new sessionId */
	  srand (time (NULL));
	  gettimeofday (& now, NULL);
	  sprintf (sessionId, "%d%.0f", rand () % 10000 + 10000, now . tv_sec * 1000000.0 + now . tv_usec);

	  /* Create a new session */
	  session = mksession (sessionId, user);

	  /* And add it to the session table */
	  if (! insert_session (& run -> sessions, sessionId, session))
	    printf ("Error: insertion of session %s failed\n", sessionId);

	  /* Render the home page in admin mode */
	  buf = evbuffer_new ();

	  begin_page (buf, ARKTITLE, URL_PLGNAME, URL_PROJECT_LOGO);
	  links (buf, LINK_PLUGINS, sessionId);
	  begin_content (buf);

	  begin_onecol (buf);
	  plugins_list (buf, NULL, sessionId);
	  end_onecol (buf);

	  end_page (buf, name, version, progname, NOW);
      	  evhttp_send_reply (req, HTTP_OK, "OK", buf);

	  evbuffer_free (buf);
	}
      else
	{
	  buf = evbuffer_new ();

	  begin_page (buf, ARKTITLE, URL_PLGNAME, URL_PROJECT_LOGO);
	  add_link (buf, URL_MAIN_HOME, LINK_HOME, 1);
	  begin_content (buf);
	  begin_onecol (buf);

	  /* Render again the form page */
	  form_login (buf);

	  /* And render now an error message */
	  evbuffer_add_printf (buf,
			       "<center>"
			       "<font color=\"#CC0033\" face=verdana size=2>\n"
			       "<b>Cannot Login</b><br></font>"
			       "<font color=black face=verdana size=2>\n"
			       "username or password are invalid"
			       "</font></center>\n"
			       "<br><br><br>\n");

	  end_onecol (buf);

	  end_page (buf, name, version, progname, NOW);
	  evhttp_send_reply (req, HTTP_OK, "OK", buf);
	  evbuffer_free (buf);
	}
    }
  else
    page_notfound (req, arg);
}


/* Render the accounts page */
static void accounts_page (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  struct evbuffer * buf;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  /* An attempt was made without the admin enabled */
  if (! run -> enable_admin)
    {
      page_notfound (req, arg);
      return;
    }

  /* An attempt was made with an invalid session id */
  if (! lookup_session (& run -> sessions, sessionId))
    {
      login_page (req, arg);
      return;
    }

  buf = evbuffer_new ();

  begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
  links (buf, LINK_ACCOUNTS, sessionId);
  begin_content (buf);

  /* Left column */
  accounts_list (buf);

  /* Right column */
  sessions_list (buf);

  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Render the logout page */
static void logout_page (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  session_t * s;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  /* An attempt was made without the admin enabled */
  if (! run -> enable_admin)
    {
      page_notfound (req, arg);
      return;
    }

  /* An attempt was made with an invalid session id */
  if (! lookup_session (& run -> sessions, sessionId))
    {
      login_page (req, arg);
      return;
    }

  /* Remove the idle timer */
  s = lookup_session (& run -> sessions, sessionId);
  evtimer_del (& s -> expired_timer);

  remove_session (& run -> sessions, sessionId);

  initial_page (req, arg);
}


/* Render the plugins page with all available and running plugins */
static void plugins_page (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  struct evbuffer * buf;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  if (! sessionId)
    sessionId = (char *) evhttp_find_header (req -> output_headers, KEY_SESSIONID);

  buf = evbuffer_new ();

  begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
  links (buf, LINK_PLUGINS, sessionId);
  begin_content (buf);

  begin_onecol (buf);
  plugins_list (buf, NULL, sessionId);
  end_onecol (buf);

  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Render the about page */
static void about_page (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  struct evbuffer * buf;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  buf = evbuffer_new ();

  begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
  links (buf, LINK_ABOUT, sessionId);
  begin_content (buf);

  begin_onecol (buf);
  project_info (buf);
  end_onecol (buf);

  end_page (buf, name, version, progname, NOW);
  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Load only a given plugin */
static void load_plug (struct evhttp_request * req, void * arg)
{
  char * url;
  char * path;
  char * sessionId;

  plugin_t * plug;
  call_t * call;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  /* sessionId */
  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);
  if (run -> enable_admin && ! lookup_session (& run -> sessions, sessionId))
    {
      login_page (req, arg);
      return;
    }

  /* Pathname of the plugin to load */
  path = (char *) evhttp_find_header (req -> input_headers, "path");
  if (! path)
    {
      error_page (req, arg, "path", sessionId);
      return;
    }

  /* Check if the plugin is currently pending */
  if ((plug = pnd_bypath (run -> pending, path)))
    {
      /* The plugin is currently pending, so render only its homepage */
      char * argv [] = { (char *) req, NULL };

      /* Add the sessionId to the header of the req */
      if (sessionId)
	evhttp_add_header (req -> output_headers, KEY_SESSIONID, sessionId);

      /* Call the 'web' plugin function (if any) to render its homepage  */
      if (! (call = function (HOMEPAGE, plug -> symbols)) || ! call (1, argv))
	plugins_page (req, arg);
    }
  else
    {
      char * argv [] = { (char *) req, NULL };

      /* The plugin is not currently pending, so I need to check if it supports the web interface */

      /* Add the sessionId to the header of the req */
      if (sessionId)
	evhttp_add_header (req -> output_headers, KEY_SESSIONID, sessionId);

      plug = homepage (path, 1, argv);
      if (plug)
	/* Web supported, then add the plugin to the list of pending and render its homepage */
	run -> pending = pnd_add (run -> pending, plug);
      else
	{
	  /* Unsupported web, then simply load the plugin */
	  char * load_params [] = { path, NULL };
	  plugin_t ** p = load_plugin (load_params [0], 1, load_params, 1);

	  if (p)
	    plugins_page (req, arg);
	  else
	    {
	      struct evbuffer * buf = evbuffer_new ();

	      begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
	      links (buf, LINK_PLUGINS, sessionId);
	      begin_content (buf);

	      /* Render the plugin list with marked the plugin name that failed to load */
	      begin_onecol (buf);
	      plugins_list (buf, path, sessionId);
	      end_onecol (buf);

	      end_page (buf, name, version, progname, NOW);
	      evhttp_send_reply (req, HTTP_OK, "OK", buf);

	      evbuffer_free (buf);
	    }
	}
    }
}


/* Unload all checked plugins */
static void unload_plug (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;

  plugin_t ** p_run;
  char ** list = NULL;
  char ** tounload;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);
  if (run -> enable_admin && ! lookup_session (& run -> sessions, sessionId))
    {
      login_page (req, arg);
      return;
    }

  /* Elenco di plugins attualmente 'running' */
  p_run = running ();

  /* Elenco di nomi di plugins di cui eseguire l'unload */
  while (p_run && * p_run)
    {
      if (evhttp_find_header (req -> input_headers, pluginname (* p_run)) && strcmp (name, pluginname (* p_run)))
	list = argsadd (list, pluginname (* p_run));
      p_run ++;
    }

  /* Scorro la lista dei plugin selezionati ed eseguo l'unload */
  tounload = list;
  while (tounload && * tounload)
    unload_plugin (* tounload ++);

  argsfree (list);

  plugins_page (req, arg);
}


/*
 * Parse the parameters related to a new load of a plugin via its form page
 *
 * url is a string as "/parse?name=....&opt=val&op1=val1&opt2=val2&.."
 */
static void parse (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;

  char * plugname;
  char * path;
  plugin_t * plug;

  char * parseurl;
  char * list;
  int i;
  int k;
  int nr_pairs;
  char ** pairs_list;
  char ** pairs;
  char ** argv;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  /* Plugin name */
  plugname = (char *) evhttp_find_header (req -> input_headers, "pname");
  plug = pnd_byname (run -> pending, plugname);

  if (! plugname || ! plug)
    {
      error_page (req, arg, "plugin name", sessionId);
      free (url);
      return;
    }

  /* Advance to the parameters position */
  parseurl = url;
  parseurl = parseurl + (PARSELEN + PARAMLEN + strlen (plugname));

  /* Remove the first '&' that identifies the beginning of the parameters */
  parseurl ++;

  /* Warning: string is now "opt=val&op1=val1&opt2=val2&.."  */

  /* Sostitute all occurrences of '&' with ' ' */
  i = 0;
  list = strdup (parseurl);
  while (parseurl && * parseurl)
    {
      if (* parseurl == '&')
	* parseurl = ' ';

      list [i ++] = * parseurl ++;
    }
  list [i] = '\0';

  /* Warning: list is now "opt=val op1=val1 opt2=val2 .." */

  /* Split the list into an array of pairs 'opt=val' */
  pairs_list = argsblanks (list);

  free (list);
  free (url);

  /* NB: pairs_list is an array of strings "opt=val" */

  nr_pairs = argslen (pairs_list);
  pairs = pairs_list;

  /* Built Now the array of parameters to pass to the plugin while loading */

  /* The first parameter is the plugin name */
  argv = argsadd (NULL, plugname);

  /* Process other optional parameters from the form */
  k = 0;
  while (k < nr_pairs)  /* foreach pair 'opt=val' */
    {
      char * ith = pairs [k];

      /* Sostitute '=' with ' ' */
      while (* pairs [k])
	{
	  if (* pairs [k] == '=')
	    {
	      * pairs [k] = ' ';
	      break;
	    }
	  pairs [k] ++;
	}

      /* NB: ith = stringa del tipo 'opt val' */

      /* Split the string 'opt' and 'val'} */
      char ** coppia = argsblanks (ith);

      /*
       * Ckeck on values contained in 'coppia'
       *
       * 'opt' != NULL
       * 'val'
       *    - NULL: Neither 'opt' nor 'val' will be added to list of parameters (eg. -c NULL)
       *    - NONE: in such event the parameter does not want an argument, then
       *            only 'opt' will be added to the list of parameters (eg. -r)
       *    - val: with a true value
       *           In such event both 'opt' and 'val' will be added to the list of parameters
       *           (eg. -b *:5555)
       */

      if (! coppia [OPT] || ! coppia [VAL])     /* eg. -c NULL */
	k ++;
      else
	{
	  if (! strcmp (coppia [VAL], NONE))   /* eg. -r without argument */
	    argv = argsadd (argv, coppia [OPT]);
	  else
	    {
	      /* 'sessionId' is meaningful while loading a plugin */
	      if (strcmp (coppia [OPT], KEY_SESSIONID))
		{
		  argv = argsadd (argv, coppia [OPT]);
		  argv = argsadd (argv, coppia [VAL]);
		}
	    }

	  k ++;
	}
      argsfree (coppia);
    }

  if (pairs_list)
    free (pairs_list);

  /* Warning: argv keeps now the list of all and _only_  parameters required to load the plugin */

  /* Remove the plugin from the 'pending' array and insert it to the 'loaded' */
  if (pnd_load (plug, argslen (argv), argv))
    {
      run -> pending = pnd_remove (run -> pending, plugname);
      plugins_page (req, arg);
    }
  else
    {
      /* Plugin failed to load */
      struct evbuffer * buf;

      path = strdup (plug -> path);

      /* Remove it from the 'pending' array */
      run -> pending = pnd_remove (run -> pending, plugname);

      /* Unload it and remove it from 'loaded' array */
      loaded = unload_plugin (plugname);

      buf = evbuffer_new ();

      begin_page (buf, ARKTITLE, URL_PROJECT_NAME, URL_PROJECT_LOGO);
      links (buf, LINK_PLUGINS, sessionId);
      begin_content (buf);

      /* Render the plugin list with marked the plugin name that failed to load */
      begin_onecol (buf);
      plugins_list (buf, path, sessionId);
      end_onecol (buf);

      end_page (buf, name, version, progname, NOW);
      evhttp_send_reply (req, HTTP_OK, "OK", buf);

      evbuffer_free (buf);
      free (path);

      argsfree (argv);
    }
}


/* Render all links that should be included in the header plugin pages */
static void plugin_links (struct evbuffer * buf, char * page, char * sessionId)
{
  if (sessionId)
    {
      char url [256];

      sprintf (url, "%s?%s=%s", URL_HOME, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_HOME, ! strcmp (page, LINK_HOME));

      sprintf (url, "%s?%s=%s", URL_PLUGINS, KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_PLUGINS, ! strcmp (page, LINK_PLUGINS));

      sprintf (url, "%s?%s=%s", URL_ABOUT,   KEY_SESSIONID, sessionId);
      add_link (buf, url, LINK_ABOUT, ! strcmp (page, LINK_ABOUT));
    }
  else
    {
      add_link (buf, URL_HOME,    LINK_HOME,    ! strcmp (page, LINK_HOME));
      add_link (buf, URL_PLUGINS, LINK_PLUGINS, ! strcmp (page, LINK_PLUGINS));
      add_link (buf, URL_ABOUT,   LINK_ABOUT,   ! strcmp (page, LINK_ABOUT));
    }
}


/* Render plugin homepage */
static void plugin_homepage (struct evbuffer * buf, char * sessionId)
{
  char ** p;
  changelog_t * c;

  begin_page (buf, (char *) name, URL_PLGNAME, URL_MASCOTTE);
  plugin_links (buf, LINK_HOME, sessionId);
  begin_content (buf);

  begin_left (buf, (char *) name);
  p = intro;
  while (* p)
    left (buf, * p ++);
  end_left (buf);

  begin_right (buf);

  if (sessionId)
    {
      char about [256];
      sprintf (about, "%s?%s=%s", URL_ABOUT, KEY_SESSIONID, sessionId);
      pretty_version (buf, URL_MASCOTTE, (char *) version, (char *) LATEST, about);
    }
  else
    pretty_version (buf, URL_MASCOTTE, (char *) version, (char *) LATEST, URL_ABOUT);

  /* Add the changelog section on the right side of the page */
  add_changelog (buf, "Changelog");

  c = changelogs;
  while (c -> when)
    new_log (buf, c -> what, c -> when, c -> desc),
      c ++;

  end_right (buf);
  end_page (buf, name, version, progname, NOW);
}


/* Callback for /name/URL_HOME */
static void home (struct evhttp_request * req, void * arg)
{
  char * url;
  char * sessionId;
  struct evbuffer * buf;

  url = evhttp_decode_uri (evhttp_request_uri (req));
  evhttp_parse_query (url, req -> input_headers);
  free (url);

  sessionId = (char *) evhttp_find_header (req -> input_headers, KEY_SESSIONID);

  buf = evbuffer_new ();

  plugin_homepage (buf, sessionId);

  evhttp_send_reply (req, HTTP_OK, "OK", buf);

  evbuffer_free (buf);
}


/* Render main plugin information */
static void plugin_info (struct evbuffer * buf)
{
  begin_plginfo (buf, "Plugin Info");

  string_info (buf, "Name",    (char *) name);
  string_info (buf, "Version", (char *) version);
  string_info (buf, "Author",  (char *) author);
  string_info (buf, "Date",    (char *) date);

  if (run)
    string_info (buf, "Uptime", htmluptime (& run -> boottime));

  end_plginfo (buf);
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
  plugin_links (buf, LINK_ABOUT, sessionId);
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
      /* Main callbacks */
      if (! evhttp_is_cbset (arkhttpd, URL_MAIN_HOME))
	evhttp_set_cb (arkhttpd, URL_MAIN_HOME, initial_page, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_PLUGINS))
	evhttp_set_cb (arkhttpd, URL_PLUGINS, plugins_page, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_MAIN_ABOUT))
	evhttp_set_cb (arkhttpd, URL_MAIN_ABOUT, about_page, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_LOGIN))
        evhttp_set_cb (arkhttpd, URL_LOGIN, login_page, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_CHECK))
        evhttp_set_cb (arkhttpd, URL_CHECK, check_page, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_ACCOUNTS))
        evhttp_set_cb (arkhttpd, URL_ACCOUNTS, accounts_page, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_LOGOUT))
        evhttp_set_cb (arkhttpd, URL_LOGOUT, logout_page, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_LOAD))
	evhttp_set_cb (arkhttpd, URL_LOAD, load_plug, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_UNLOAD))
	evhttp_set_cb (arkhttpd, URL_UNLOAD, unload_plug, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_PARSE))
	evhttp_set_cb (arkhttpd, URL_PARSE, parse, NULL);

      if (! evhttp_is_cbset (arkhttpd, IMAGE))
	evhttp_set_cb (arkhttpd, IMAGE, arkimage, embedded);

      if (! evhttp_is_cbset (arkhttpd, URL_CSS))
	evhttp_set_cb (arkhttpd, URL_CSS, css, NULL);

      /* Private callbacks */
      if (! evhttp_is_cbset (arkhttpd, URL_HOME))
	evhttp_set_cb (arkhttpd, URL_HOME, home, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_HOME1))
	evhttp_set_cb (arkhttpd, URL_HOME1, home, NULL);

      if (! evhttp_is_cbset (arkhttpd, URL_IMAGE))
	evhttp_set_cb (arkhttpd, URL_IMAGE, arkimage, embedded);

      if (! evhttp_is_cbset (arkhttpd, URL_ABOUT))
	evhttp_set_cb (arkhttpd, URL_ABOUT, about, NULL);

      /* Generic callback */
      evhttp_set_gencb (arkhttpd, page_notfound,  NULL);
    }
}


/* Cleanup all previuosly registered callbacks for web rendering */
static void nomoreweb (void)
{
  if (arkhttpd)
    {
      /* Main callbacks */
      evhttp_del_cb (arkhttpd, URL_MAIN_HOME);
      evhttp_del_cb (arkhttpd, URL_PLUGINS);
      evhttp_del_cb (arkhttpd, URL_MAIN_ABOUT);

      evhttp_del_cb (arkhttpd, URL_LOGIN);
      evhttp_del_cb (arkhttpd, URL_CHECK);
      evhttp_del_cb (arkhttpd, URL_ACCOUNTS);
      evhttp_del_cb (arkhttpd, URL_LOGOUT);

      evhttp_del_cb (arkhttpd, URL_LOAD);
      evhttp_del_cb (arkhttpd, URL_UNLOAD);

      evhttp_del_cb (arkhttpd, URL_PARSE);
      evhttp_del_cb (arkhttpd, URL_IMAGE);
      evhttp_del_cb (arkhttpd, URL_CSS);

      /* Private callbacks */
      evhttp_del_cb (arkhttpd, URL_HOME);
      evhttp_del_cb (arkhttpd, URL_HOME1);
      evhttp_del_cb (arkhttpd, IMAGE);
      evhttp_del_cb (arkhttpd, URL_ABOUT);
    }

  /* Free the previously created HTTP server */
  evhttp_free (arkhttpd);
}
