/*
 * webadmin.c - An ark-aware plugin, accesible via HTTP protocol, to remotely control other plugins
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


/* Plugin declarations */

const char * variables = { "name version author date what copyright" };
const char * functions = { "boot halt" };


/* Plugin definitions */

const char * name      = "webadmin";
const char * version   = "0.0.2";
const char * author    = "Valeria Lorenzetti valeria.lorenzetti@gmail.com";
const char * date      = __DATE__;
const char * what      = "An ark-aware plugin, accessible via the HTTP protocol, to remotely control other plugins";
const char * copyright = "(C) Copyright 2008 Valeria Lorenzetti and Rocco Carbone";


/* Legal configuration keywords in effect for this plugin */
#define LOGFILE       "LogFile"
#define ACCESSFILE    "AccessFile"
#define ENABLEADMIN   "EnableAdmin"
#define ACCOUNTS      "Accounts"
#define EXPIREDTIME   "ExpiredTime"
#define LISTEN        "Listen"
#define HD_COLOR      "HdColor"
#define BGCOLOR       "BgColor"

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Operating System header file(s) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <netdb.h>
#if !defined(DONT_HAVE_GETOPT_H)
# include <getopt.h>
#endif

/* Private header file(s) */
#include "ark.h"
#include "ark-html.h"
#include "parsecfg.h"


/* The place for default values */

#include "webadmin.h"
#include "session.h"

/* The place for static variables */

/* Variables to keep run-time parameters */
static opttime_t * opt = NULL;
static cfgtime_t * cfg = NULL;
static runtime_t * run = NULL;


/* The place for private functions */

#include "descriptions.c"
#include "pending.c"
#include "session.c"
#include "callbacks.c"
#include "private.c"
#include "webcallbacks.c"


/* The place for public functions */

/* Will be called once when the plugin is unloaded */
int halt (int argc, char * argv [])
{
  /* Release resources and memory */
  nomoreopt (opt);
  nomorecfg (cfg);
  nomorerun (run);

  opt = NULL;
  cfg = NULL;
  run = NULL;

  nomoreweb ();

  return PLUGIN_OK;
}


/* Will be called once when the plugin is loaded */
int boot (int argc, char * argv [])
{
  /* The place for local variables */
  int option;
  char ** addr;
  int port;
  char * c;

  /* Notice the plugin name */
  char * plgname = strrchr (argv [0], '/');
  plgname = ! plgname ? * argv : plgname + 1;

  /* Initialize optional, configuration and run-time variables to default values */
  opt = defaultopt ();
  cfg = defaultcfg ();
  run = defaultrun (plgname);

  optind = 0;
  optarg = NULL;
#define OPTSTRING "hvc:l:a:b:"
  while ((option = getopt (argc, argv, OPTSTRING)) != EOF)
    {
      switch (option)
        {
        default: break;

	  /* Reserved options */
        case 'h': break;
        case 'v': break;

	  /* Files */
        case 'c': opt -> conffile   = strdup (optarg); break;                /* Master configuration file */
        case 'l': opt -> logfile    = strdup (optarg); break;                /* Log file */
        case 'a': opt -> accessfile = strdup (optarg); break;                /* Access file */

        case 'b': opt -> addrs      = argsadd (opt -> addrs, optarg); break; /* Local addresses */
        }
    }

  /* Load configuration parameters from configuration file (if any) */
  if (opt -> conffile || run -> conffile)
    {
      printf ("%s [%s]: Reading section [%s] in file '%s' ... ",
	      progname, name, name, opt -> conffile ? opt -> conffile : run -> conffile);

      if (loadconfig (name, opt -> conffile ? opt -> conffile : run -> conffile, cfg) == -1)
	{
	  /* Release resources and memory */
	  halt (argc, argv);

	  return PLUGIN_FAIL;
	}
      printf ("OK\n");
    }

  /* Save run-time configuration parameters */
  override (opt, cfg, run);

  /* Create a new HTTP server */
  if (! (arkhttpd = evhttp_new (NULL)))
    {
      printf ("%s [%s]: plugin error, can't create the HTTP server", progname, name);

      /* Release resources and memory */
      halt (argc, argv);

      return PLUGIN_FAIL;
    }

  /* Start with a default address in case none was chosen */
  if (! run -> addrs)
    run -> addrs = argsadd (NULL, DEFAULT_ADDRESS);

  addr = run -> addrs;
  while (* addr)
    {
      if ((c = strchr (* addr, ':')))
	* c = '\0',
	  port = atoi (c + 1);
      else
	continue;

      /* Start a HTTP server on the specified address and port */
      evhttp_bind_socket (arkhttpd, strcmp (* addr, "*") ? * addr : "0.0.0.0", port);

      /* Ready message */
      printf ("%s [%s]: Plugin ready, now accepting connections on %s:%d\n",
	      progname, name, * addr ? * addr : "*", port);

      * c = ':';
      addr ++;
    }

  /* Register callbacks for web access */
  registerweb ();

  /* Fill the short-descriptions table */
  fill_descriptions (& run -> desc);

  return PLUGIN_OK;
}


#if defined(STANDALONE)

int main (int argc, char * argv [])
{
  /* Notice the program name */
  progname = strrchr (argv [0], '/');
  progname = ! progname ? * argv : progname + 1;

  /* Set the time the program was started */
  started ();

  /* Set unbuffered stdout */
  setvbuf (stdout, NULL, _IONBF, 0);

  /* Ignore writes to connections that have been closed at the other end */
  signal (SIGPIPE, SIG_IGN);

  /* Perform one-time initialization of the libevent library */
  event_init ();

  if (boot (argc, argv) == PLUGIN_OK)
    {
      /* Go for network events */
      if (event_dispatch ())
	printf ("%s: Error - no active network socket enabled!\n", progname);
    }
  else
    printf ("%s: Error - cannot initialize!\n", progname);

  return 0;
}

#endif /* STANDALONE */
