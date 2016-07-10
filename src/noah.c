/*
 * noah.c - Noah, keeps me, my family, and a core breeding
 *          stock of the world's clients and servers safe
 *          from the Great Java Flood
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
#include <string.h>
#if !defined(DONT_HAVE_GETOPT_H)
# include <getopt.h>
#endif
#include <sys/utsname.h>

/* Private header file(s) */
#include "ark.h"
#include "parsecfg.h"

#define DEFAULT_CONF_FILE   "noah.conf"
#define DEFAULT_PID_FILE    "noah.pid"
#define DEFAULT_LOG_FILE    "*"          /* "-" means standard output; "*" means none */
#define DEFAULT_PLUGIN_DIR  "modules"

/* Section names */
#define SECTION   "main"

/* Legal configuration keywords in effect for this program in the given section */
#define PIDFILE  "PidFile"
#define LOGFILE  "LogFile"
#define PLGDIR   "PlgDir"
#define PLUGIN   "Plugin"
#define SAMECONF "SameConf"


/* How to drive the behaviour of the program via command line options */
typedef struct
{
  /* Files */
  char * conffile;    /* Master configuration file */
  char * pidfile;     /* Pid file                  */
  char * logfile;     /* Log file                  */

  char ** plgdirs;    /* Plugin directories        */
  char ** plugins;    /* Plugins to load           */

  int sameconf;       /* Same conffile for plugins */

} opttime_t;


/* How to configure the program via a master configuration file */
typedef struct
{
  char * pidfile;     /* Pid file - optional       */
  char * logfile;     /* Log file - optional       */

  char ** plgdirs;    /* Plugin directories        */
  char ** plugins;    /* Plugins to load           */

  int sameconf;       /* Same conffile for plugins */

} cfgtime_t;


/* Run-time parameters */
typedef struct
{
  char * conffile;    /* Master configuration file */
  char * pidfile;     /* Pid file                  */
  char * logfile;     /* Logging file (if any)     */
  FILE * logfp;       /* Logging file pointer      */

  char ** plugins;    /* Plugins to load           */

  unsigned sameconf;  /* Same conffile for plugins */

} runtime_t;


/* Variables to keep run-time parameters */
static opttime_t * opt = NULL;
static cfgtime_t * cfg = NULL;
static runtime_t * run = NULL;


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* The place for private functions */

/* Initialize option-time variable to its default value
 * (both -1 and NULL mean absent)
 */
static opttime_t * defaultopt (void)
{
  opttime_t * op = calloc (sizeof (opttime_t), 1);
  if (! op)
    return NULL;

  op -> conffile = NULL;
  op -> pidfile  = NULL;
  op -> logfile  = NULL;

  op -> plgdirs  = NULL;
  op -> plugins  = NULL;

  op -> sameconf = -1;

  return op;
}


/* Initialize configuration variable to its default value
 * (both -1 and NULL mean absent)
 *
 * Each field is set to a reasonable value and could be overridden
 * by the corresponding command line options if specified.
 */
static cfgtime_t * defaultcfg (void)
{
  cfgtime_t * cf = calloc (sizeof (cfgtime_t), 1);
  if (! cf)
    return NULL;

  cf -> pidfile  = NULL;
  cf -> logfile  = NULL;

  cf -> plgdirs  = NULL;
  cf -> plugins  = NULL;

  cf -> sameconf = -1;

  return cf;
}


/* Initialize run-time variable to its default value
 * (both -1 and NULL mean absent)
 *
 * Each field is set to a reasonable value and could be
 * overridden by the corresponding configuration value,
 * if one specified, or command line options.
 */
static runtime_t * defaultrun (char * progname)
{
  runtime_t * rc = calloc (sizeof (runtime_t), 1);
  if (! rc)
    return NULL;

  rc -> conffile = strdup (DEFAULT_CONF_FILE);
  rc -> pidfile  = strdup (DEFAULT_PID_FILE);
  rc -> logfile  = strdup (DEFAULT_LOG_FILE);
  if (! strcmp (rc -> logfile, "*"))
    rc -> logfp  = NULL;
  else if (! strcmp (rc -> logfile, "-"))
    rc -> logfp  = stdout;
  else
    rc -> logfp  = fopen (rc -> logfile, "w+");

  plgpaths = argsadd (NULL, DEFAULT_PLUGIN_DIR);

  rc -> plugins  = NULL;

  rc -> sameconf = 1;

  return rc;
}


/* Free the memory used to save command line parameters */
static void nomoreopt (opttime_t * op)
{
  if (! op)
    return;

  if (op -> conffile)
    free (op -> conffile);
  if (op -> pidfile)
    free (op -> pidfile);
  if (op -> logfile)
    free (op -> logfile);

  argsfree (op -> plgdirs);
  argsfree (op -> plugins);

  free (op);
}


/* Free the memory used to save configuration parameters */
static void nomorecfg (cfgtime_t * cf)
{
  if (! cf)
    return;

  if (cf -> pidfile)
    free (cf -> pidfile);
  if (cf -> logfile)
    free (cf -> logfile);

  argsfree (cf -> plgdirs);
  argsfree (cf -> plugins);

  free (cf);
}


/* Free the memory used to save run-time parameters */
static void nomorerun (runtime_t * rc)
{
  if (! rc)
    return;

  if (rc -> conffile)
    free (rc -> conffile);
  if (rc -> pidfile)
    free (rc -> pidfile);
  if (rc -> logfile)
    free (rc -> logfile);

  argsfree (rc -> plugins);

  argsfree (plgpaths);

  free (rc);
}


/* Fill in the run-time values with values from command line or configuration */
static void override (opttime_t * op, cfgtime_t * cf, runtime_t * rc)
{
  /* Master configuration file */
  if (op -> conffile)
    if (rc -> conffile)
      free (rc -> conffile),
	rc -> conffile = strdup (op -> conffile);

  /* Pid file */
  if (op -> pidfile)
    {
      if (rc -> pidfile)
        free (rc -> pidfile),
	  rc -> pidfile = strdup (op -> pidfile);
    }
  else if (cf -> pidfile)
    {
      if (rc -> pidfile)
        free (rc -> pidfile),
	  rc -> pidfile = strdup (cf -> pidfile);
    }

  /* Log file */
  if (op -> logfile)
    {
      if (rc -> logfile)
        free (rc -> logfile),
	  rc -> logfile = strdup (op -> logfile);
    }
  else if (cf -> logfile)
    {
      if (rc -> logfile)
        free (rc -> logfile),
	  rc -> logfile = strdup (cf -> logfile);
    }

  /* Plugin directories */
  if (op -> plgdirs)
    argsfree (plgpaths),
      plgpaths = argsdup (op -> plgdirs);
  else if (cf -> plgdirs)
    argsfree (plgpaths),
      plgpaths = argsdup (cf -> plgdirs);

  /* Plugins to load at boot time */
  if (op -> plugins)
    argsfree (rc -> plugins),
      rc -> plugins = argsdup (op -> plugins);
  else if (cf -> plugins)
    argsfree (rc -> plugins),
      rc -> plugins = argsdup (cf -> plugins);

  /* Use the same configuration file also for the plugins */
  if (op -> sameconf != -1)
    rc -> sameconf = op -> sameconf;
  else if (cf -> sameconf != -1)
    rc -> sameconf = cf -> sameconf;
}


/* Slurp in the configuration file */
static int loadconfig (char * conffile, cfgtime_t * cf)
{
  FILE * in;

  /* Variables in the [kernel] section */
  cfgList * list;
  cfgList * item;

  if (! conffile)
    return -1;

  /* Check for existence and readability */
  in = ! strcmp (conffile, "-") ? fdopen (0, "r") : fopen (conffile, "r");
  if (! in)
    {
      printf ("Error: file '%s' does not exist or it is not readable\n", conffile);
      return -1;
    }
  fclose (in);

  /* Fetch all the configuration variables from the file */

  fetchVarFromCfgFile (conffile, PIDFILE,      & cf -> pidfile,  CFG_STRING,      CFG_INI, 0, SECTION);
  fetchVarFromCfgFile (conffile, LOGFILE,      & cf -> logfile,  CFG_STRING,      CFG_INI, 0, SECTION);
  list = NULL;
  if (! fetchVarFromCfgFile (conffile, PLGDIR, & list,           CFG_STRING_LIST, CFG_INI, 0, SECTION))
    for (item = list; item; item = item -> next)
      cf -> plgdirs = argsadd (cf -> plgdirs, item -> str);
  list = NULL;
  if (! fetchVarFromCfgFile (conffile, PLUGIN, & list,           CFG_STRING_LIST, CFG_INI, 0, SECTION))
    for (item = list; item; item = item -> next)
      cf -> plugins = argsadd (cf -> plugins, item -> str);
  fetchVarFromCfgFile (conffile, SAMECONF,     & cf -> sameconf, CFG_BOOL,       CFG_INI, 0, SECTION);

  return 0;
}


/* Display the syntax for using this program */
static void usage (char * progname)
{
  printf ("Usage: %s [options]\n", progname);
  printf ("\n");

  printf (" -h         show this help message and exit\n"
	  " -v         print version number and exit\n"
	  " -c FILE    specify master configuration file [default %s]\n"
	  " -p FILE    specify pid file [default %s]\n"
	  " -l FILE    specify log file [default %s]\n"
	  " -d DIR     add dir to the loadable plugin search path - multiple directories are allowed [default %s]\n"
	  " -m module  add module to the list of plugins to load - multiple modules are allowed\n",
	  DEFAULT_CONF_FILE,
	  DEFAULT_PID_FILE,
	  DEFAULT_LOG_FILE,
	  DEFAULT_PLUGIN_DIR);
}


/* You are running this version of the software */
static void version (char * progname)
{
  printf ("%s %s built on %s %s\n", progname, PKG_VERSION, __DATE__, __TIME__);
  printf ("Written by %s\n", PKG_AUTHOR);
  printf ("\n");
  fflush (stdout);
}


/* What should be done when the program is interrupted */
static void on_control_c (int sig)
{
  printf ("Caught signal %d\n", sig);

  printf ("Unloading all running plugins ...\n");
  unload_all_plugins ();

  /* Release resources and memory */
  nomorerun (run);
  nomorecfg (cfg);
  nomoreopt (opt);

  exit (0);
}


/* Announce to the world! */
static void announce (char * progname)
{
  time_t now = time (0);
  char * nowstring = ctime (& now);
  struct utsname machine;

  nowstring [24] = '\0';
  uname (& machine);

  printf ("This is %s %s of %s\n", progname, PKG_VERSION, PKG_RELEASE_DATE);
  printf ("Author %s\n", PKG_AUTHOR);
  printf ("Started at %s on %s\n\n", nowstring, machine . nodename);

  printf ("Please relax: '%s' has nothing to do with object-oriented technology!\n\n", progname);

  fflush (stdout);
}


/*
 * Sirs and Ladies, here to you... Noah!!!
 *
 * It keep me, my family, and a core breeding stock of the
 * world's clients and servers safe from the Great Java Flood
 */
int main (int argc, char * argv [])
{
  int option;

  /* Notice the program name */
  progname = strrchr (argv [0], '/');
  progname = ! progname ? * argv : progname + 1;

  /* Set the time the program was started */
  started ();

  /* Set unbuffered stdout */
  setvbuf (stdout, NULL, _IONBF, 0);

  /* Initialize optional, configuration and run-time variables to their default values */
  opt = defaultopt ();
  cfg = defaultcfg ();
  run = defaultrun (progname);

#define OPTSTRING "hvc:p:l:d:m:"
  while ((option = getopt (argc, argv, OPTSTRING)) != EOF)
    {
      switch (option)
        {
        default: usage (progname); return 1;

        case 'h': usage (progname); return 0;
        case 'v': version (progname); return 0;

        case 'c': opt -> conffile = strdup (optarg); break;                  /* Master configuration file */
        case 'p': opt -> pidfile  = strdup (optarg); break;                  /* Pid file                  */
        case 'l': opt -> logfile  = strdup (optarg); break;                  /* Log file                  */

        case 'd': opt -> plgdirs  = argsadd (opt -> plgdirs, optarg); break; /* Plugins directory         */
        case 'm': opt -> plugins  = argsadd (opt -> plugins, optarg); break; /* Plugins to load at boot   */
        }
    }

  /* Hey boys, noah speaking! */
  announce (progname);

  /* Load configuration parameters from configuration file (if any) */
  if (opt -> conffile || run -> conffile)
    {
      printf ("%s: Reading configuration file '%s' ... ", progname, opt -> conffile ? opt -> conffile : run -> conffile);

      if (loadconfig (opt -> conffile ? opt -> conffile : run -> conffile, cfg) == -1)
	return 1;
      printf ("OK\n");
    }

  /* Save run-time configuration parameters */
  override (opt, cfg, run);

  /* Ignore writes to connections that have been closed at the other end */
  signal (SIGPIPE, SIG_IGN);

  /* Handle interrupts */
  signal (SIGINT, on_control_c);
  signal (SIGQUIT, on_control_c);
  signal (SIGTERM, on_control_c);

  /* Perform one-time initialization of the libevent library */
  event_init ();

  /* Load given/all available plugins that can be found in the search path */
  if (run -> plugins)
    load_plugins (run -> plugins, plgpaths, run -> sameconf ? run -> conffile : NULL);
  else
    load_all_plugins (plgpaths, run -> sameconf ? run -> conffile : NULL);

  if (pluginsno ())
    {
      printf ("\n%s: waiting for timeouts and network events...\n\n", progname);

      /* Go for network events */
      if (event_dispatch ())
	printf ("%s: Error - no active network socket enabled!\n", progname);
    }
  else
    printf ("%s: Error - no plugin could be loaded!\n", progname);

  unload_all_plugins ();

  return 0;
}
