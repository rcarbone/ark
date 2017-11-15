/*
 * telnetd.h - an ark-aware plugin, accesible via Telnet protocol, to remotely control other plugins
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


/* Plugin declarations */

const char * variables = { "name version author date what copyright" };
const char * functions = { "boot halt" };


/* Plugin definitions */

const char * name      = "telnetd";
const char * version   = "0.0.1";
const char * author    = "Rocco Carbone rocco@tecsiel.it";
const char * date      = __DATE__;
const char * what      = "An ark-aware plugin, accessible via the Telnet protocol, to remotely control other plugins";
const char * copyright = "(C) Copyright 2007-2008 Rocco Carbone";


/* Legal configuration keywords in effect for this plugin */
#define LOGFILE       "LogFile"
#define ACCESSFILE    "AccessFile"
#define ADDR          "Listen"
#define BACKLOG       "Backlog"
#define MAXCONNS      "MaxConns"
#define LOGIN         "Login"
#define PASSWORD      "Password"
#define LOGINTIMEOUT  "LoginTimeout"
#define IDLETIMEOUT   "IdleTimeout"
#define SHOWRECV      "ShowRecv"
#define SHOWSENT      "ShowSent"

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Operating System header file(s) */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#if !defined(DONT_HAVE_GETOPT_H)
# include <getopt.h>
#endif

/* Private header file(s) */
#include "ark.h"
#include "server.h"
#include "parsecfg.h"


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*           The place for default values                      */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#include "telnetd.h"

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*           The place for static variables                    */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Variables to keep run-time parameters */
static opttime_t * opt = NULL;
static cfgtime_t * cfg = NULL;
static runtime_t * run = NULL;

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/*           The place for private functions                   */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#undef __GNUC__
#include "argv.c"
#include "commands.c"

/*
 * Advance pointer beyond current white space.
 * Return a NULL pointer upon reaching end of line.
 */
static char * skip_white_space (char * p)
{
  if (! p)
    return NULL;

  while (* p && (* p == ' ' || * p == '\t' || * p == '\n'))
    p ++;

  return * p == '\n' || * p == '\0' ? NULL : p;
}


#if defined(FIXME)
static void setprompt (void)
{
  if (run -> prompt)
    free (run -> prompt);
  run -> prompt = calloc (256, 1);
  sprintf (run -> prompt, "%s [%s]> ", progname, name);
}
#endif /* FIXME */


/* Initialize option-time variables to their default values
 * (both -1 and NULL mean absent).
 */
static opttime_t * defaultopt (void)
{
  opttime_t * op = calloc (sizeof (opttime_t), 1);
  if (! op)
    return NULL;

  op -> conffile   = NULL;
  op -> logfile    = NULL;
  op -> accessfile = NULL;

  op -> addrs      = NULL;

  op -> backlog    = -1;
  op -> maxconns   = -1;

  op -> user       = NULL;
  op -> password   = NULL;

  op -> login      = -1;
  op -> idle       = -1;

  op -> showrecv   = -1;
  op -> showsent   = -1;

  return op;
}


/* Initialize configuration variables to their default values
 * (both -1 and NULL mean absent).
 *
 * Each field is set to a reasonable value and could be overridden
 * by the corresponding command line options if specified.
 */
static cfgtime_t * defaultcfg (void)
{
  cfgtime_t * cf = calloc (sizeof (cfgtime_t), 1);
  if (! cf)
    return NULL;

  cf -> logfile    = NULL;
  cf -> accessfile = NULL;

  cf -> addrs      = NULL;

  cf -> backlog    = -1;
  cf -> maxconns   = -1;

  cf -> user       = NULL;
  cf -> password   = NULL;

  cf -> login      = -1;
  cf -> idle       = -1;

  cf -> showrecv   = -1;
  cf -> showsent   = -1;

  return cf;
}


/* Initialize run-time variables to their default values
 * (both -1 and NULL mean absent).
 *
 * Each field is set to a reasonable value and could be
 * overridden by the corresponding configuration value,
 * if one specified, or command line options.
 */
static runtime_t * defaultrun (char * name)
{
  runtime_t * rc = calloc (sizeof (runtime_t), 1);
  if (! rc)
    return NULL;

  gettimeofday (& rc -> boottime, NULL);
  rc -> name       = strdup (name);

  rc -> conffile   = NULL;
  rc -> logfile    = strdup (DEFAULT_LOG_FILE);
  rc -> accessfile = strdup (DEFAULT_ACCESS_FILE);

  rc -> addrs      = argsadd (NULL, DEFAULT_ADDRESS);

  rc -> backlog    = DEFAULT_BACKLOG;
  rc -> maxconns   = DEFAULT_MAXCONNS;

  rc -> user       = NULL;
  rc -> password   = NULL;

  rc -> login      = DEFAULT_LOGIN_TIMEOUT;
  rc -> idle       = DEFAULT_IDLE_TIMEOUT;

  /* Log file descriptor */
  if (! strcmp (rc -> logfile, "*"))
    rc -> logfp = NULL;
  else if (! strcmp (rc -> logfile, "-"))
    rc -> logfp = stdout;
  else
    rc -> logfp = fopen (rc -> logfile, "w+");

  /* Access file descriptor */
  if (! strcmp (rc -> accessfile, "*"))
    rc -> accessfp = NULL;
  else if (! strcmp (rc -> accessfile, "-"))
    rc -> accessfp = stdout;
  else
    rc -> accessfp = fopen (rc -> accessfile, "w+");

  rc -> servers   = NULL;

  /* Traffic disabled by default */
  rc -> showrecv  = 0;
  rc -> showsent  = 0;

  rc -> prompt    = strdup (PROMPT);

  return rc;
}


/* Free the memory used to save command line parameters */
static void nomoreopt (opttime_t * op)
{
  if (! op)
    return;

  if (op -> conffile)
    free (op -> conffile);
  if (op -> logfile)
    free (op -> logfile);
  if (op -> accessfile)
    free (op -> accessfile);

  argsfree (op -> addrs);

  if (op -> user)
    free (op -> user);
  if (op -> password)
    free (op -> password);

  free (op);
}


/* Free the memory used to save configuration parameters */
static void nomorecfg (cfgtime_t * cf)
{
  if (! cf)
    return;

  if (cf -> logfile)
    free (cf -> logfile);
  if (cf -> accessfile)
    free (cf -> accessfile);

  argsfree (cf -> addrs);

  if (cf -> user)
    free (cf -> user);
  if (cf -> password)
    free (cf -> password);

  free (cf);
}


/* Free the memory used to save run-time parameters */
static void nomorerun (runtime_t * rc)
{
  if (! rc)
    return;

  if (rc -> name)
    free (rc -> name);

  if (rc -> conffile)
    free (rc -> conffile);
  if (rc -> logfile)
    free (rc -> logfile);
  if (rc -> accessfile)
    free (rc -> accessfile);

  argsfree (rc -> addrs);

  if (rc -> user)
    free (rc -> user);
  if (rc -> password)
    free (rc -> password);

  if (rc -> logfp && rc -> logfp != stdout)
    fclose (rc -> logfp);
  if (rc -> accessfp && rc -> accessfp != stdout)
    fclose (rc -> accessfp);

  nomoreservers (rc -> servers);

  if (rc -> prompt)
    free (rc -> prompt);

  free (rc);
}


/* Fill in the run-time values with values from command line or configuration */
static void override (opttime_t * op, cfgtime_t * cf, runtime_t * rc)
{
  if (! cf)
    return;

  /* Master configuration file */
  if (op -> conffile)
    {
      if (rc -> conffile)
        free (rc -> conffile);
      rc -> conffile = strdup (op -> conffile);
    }

  /* Log file */
  if (op -> logfile)
    {
      if (rc -> logfile)
        free (rc -> logfile);
      rc -> logfile = strdup (op -> logfile);
    }
  else if (cf -> logfile)
    {
      if (rc -> logfile)
        free (rc -> logfile);
      rc -> logfile = strdup (cf -> logfile);
    }

  /* Access file */
  if (op -> accessfile)
    {
      if (rc -> accessfile)
        free (rc -> accessfile);
      rc -> accessfile = strdup (op -> accessfile);
    }
  else if (cf -> accessfile)
    {
      if (rc -> accessfile)
        free (rc -> accessfile);
      rc -> accessfile = strdup (cf -> accessfile);
    }

  /* Max pending queue */
  if (op -> backlog != -1)
    rc -> backlog = op -> backlog;
  else if (cf -> backlog != -1)
    rc -> backlog = cf -> backlog;

  /* Max number of connections */
  if (op -> maxconns != -1)
    rc -> maxconns = op -> maxconns;
  else if (cf -> maxconns != -1)
    rc -> maxconns = cf -> maxconns;

  /* Local addresses */
  if (op -> addrs)
    argsfree (rc -> addrs),
      rc -> addrs = argsdup (op -> addrs);
  else if (cf -> addrs)
    argsfree (rc -> addrs),
      rc -> addrs = argsdup (cf -> addrs);

  /* Identifier to authenticate */
  if (op -> user)
    {
      if (rc -> user)
        free (rc -> user);
      rc -> user = strdup (op -> user);
    }
  else if (cf -> user)
    {
      if (rc -> user)
        free (rc -> user);
      rc -> user = strdup (cf -> user);
    }

  /* Password to authenticate */
  if (op -> password)
    {
      if (rc -> password)
        free (rc -> password);
      rc -> password = strdup (op -> password);
    }
  else if (cf -> password)
    {
      if (rc -> password)
        free (rc -> password);
      rc -> password = strdup (cf -> password);
    }

  /* Login timeout */
  if (op -> login != -1)
    rc -> login = op -> login;
  else if (cf -> login != -1)
    rc -> login = cf -> login;

  /* Idle timeout */
  if (op -> idle != -1)
    rc -> idle = op -> idle;
  else if (cf -> idle != -1)
    rc -> idle = cf -> idle;

  /* Show packets received */
  if (op -> showrecv != -1)
    rc -> showrecv = op -> showrecv;
  else if (cf -> showrecv != -1)
    rc -> showrecv = cf -> showrecv;

  /* Show packets sent */
  if (op -> showsent != -1)
    rc -> showsent = op -> showsent;
  else if (cf -> showsent != -1)
    rc -> showsent = cf -> showsent;
}


/* Slurp in the configuration file */
static int loadconfig (const char * section, char * conffile, cfgtime_t * cf)
{
  FILE * in;

  /* Variables in the [section] passed by the 'section' variable */
  cfgList * addrs = NULL;
  cfgList * item;

  if (! conffile)
    return -1;

  /* Check for existence and readability */
  in = ! strcmp (conffile, "-") ? fdopen (0, "r") : fopen (conffile, "r");
  if (! in)
    {
      printf ("%s [%s]: Error: file '%s' does not exist or it is not readable\n", progname, name, conffile);
      return -1;
    }
  fclose (in);

  /* Fetch all the configuration variables from the file */

  fetchVarFromCfgFile (conffile, LOGFILE,      & cf -> logfile,    CFG_STRING,      CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, ACCESSFILE,   & cf -> accessfile, CFG_STRING,      CFG_INI, 0, section);
  if (! fetchVarFromCfgFile (conffile, ADDR,   & addrs,            CFG_STRING_LIST, CFG_INI, 0, section))
    for (item = addrs; item; item = item -> next)
      cf -> addrs = argsadd (cf -> addrs, item -> str);
  fetchVarFromCfgFile (conffile, BACKLOG,      & cf -> backlog,    CFG_INT,         CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, MAXCONNS,     & cf -> maxconns,   CFG_INT,         CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, LOGIN,        & cf -> user,       CFG_STRING,      CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, PASSWORD,     & cf -> password,   CFG_STRING,      CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, LOGINTIMEOUT, & cf -> login,      CFG_INT,         CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, IDLETIMEOUT,  & cf -> idle,       CFG_INT,         CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, SHOWRECV,     & cf -> showrecv,   CFG_BOOL,        CFG_INI, 0, section);
  fetchVarFromCfgFile (conffile, SHOWSENT,     & cf -> showsent,   CFG_BOOL,        CFG_INI, 0, section);

  return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Send the output to the other side */
static void reply (client_t * client, const char * fmt, ...)
{
  char buf [MAXLINE];
  va_list ap;

  /* add the formatted output of the caller to the buffer */
  va_start (ap, fmt);
  vsnprintf (buf, sizeof (buf), fmt, ap);
  va_end (ap);

  /* send the output to the other side */
  write (client -> fd, buf, strlen (buf));
}


/* Send the output (followed by a newline) to the other side */
static void replyln (client_t * client, const char * fmt, ...)
{
  char buf [MAXLINE];
  va_list ap;

  /* add the formatted output of the caller to the buffer */
  va_start (ap, fmt);
  vsnprintf (buf, sizeof (buf), fmt, ap);
  va_end (ap);

  /* send the output to the other side */
  write (client -> fd, buf, strlen (buf));
  write (client -> fd, "\r\n", strlen ("\r\n"));
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


/* Handle a disconnection with the remote client (both locally or remotely initiated) */
static void clientlogout (client_t * client, int reason)
{
  server_t * lis = client -> server;

  if (run -> accessfp)
    {
      switch (reason)
	{
	case MEMORY:
	  lis -> memory ++;
	  fprintf (run -> accessfp, "%s [%s]: #%zu %s memory fault %s:%d\n",
		   progname, name, lis -> memory, client -> received ? "idle" : "login",
		   client -> hostname, client -> port);
	  break;

	case TIMEOUT:
	  lis -> timeout ++;
	  fprintf (run -> accessfp, "%s [%s]: #%zu %s timeout expired %s:%d\n",
		   progname, name, lis -> timeout, client -> received ? "idle" : "login",
		   client -> hostname, client -> port);
	  break;

	case DECODING:
	  lis -> decoding ++;
	  fprintf (run -> accessfp, "%s [%s]: #%zu decoding error %s:%d\n",
		   progname, name, lis -> decoding, client -> hostname, client -> port);
	  break;

	case PROTOCOL:
	  lis -> protocol ++;
	  fprintf (run -> accessfp, "%s [%s]: #%zu protocol mismatch %s:%d\n",
		   progname, name, lis -> protocol, client -> hostname, client -> port);
	  break;

	case HANGUP:
	  lis -> hangup ++;
	  fprintf (run -> accessfp, "%s [%s]: #%zu remote closed connection %s:%d\n",
		   progname, name, lis -> hangup, client -> hostname, client -> port);
	  break;
	}
      fflush (run -> accessfp);
    }

  /* Remove the client from the table */
  lis -> clients = lessclient (lis -> clients, client);
}


/* The callback to handle inactivity timeouts */
static void idle_callback (int unused, const short event, void * _client)
{
  /* Login/idle timeout elapsed */
  clientlogout (_client, TIMEOUT);
}


/* Do the dirty job!  Read a line from remote and execute a command */
static void data_callback (int unused, const short event, void * _client)
{
  client_t * client = _client;
  server_t * lis = client -> server;
  client_t ** c;

  char line [MAXLINE];
  int nread;

  char * start;
  char ** argv = NULL;

  void * call = NULL;
  cmd_t * cmd;
#if defined(FIXME)
  plugin_t * p = NULL;
  char ** q;
#endif /* FIXME */

  /* Read a line from the user */
  memset (line, '\0', sizeof (line));
  nread = read (client -> fd, line, MAXLINE);
  if (nread <= 0)
    {
      /* Handle the disconnection */
      clientlogout (client, HANGUP);
      return;
    }

  /* Update # of bytes received */
  client -> recvbytes += nread;

  /* Skip empty lines */
  if (! * line)
    goto end;

  /* Strip trailing new line */
  if (line [strlen (line) - 1] == '\n')
    line [strlen (line) - 1] = '\0';

  /* Strip trailing carriage return */
  if (line [strlen (line) - 1] == '\r')
    line [strlen (line) - 1] = '\0';

  /* One more message received */
  client -> received ++;
  lis -> received ++;

  if (client -> showrecv)
    printf ("%s [%s]: #%zu <= %s [%s:%d #%zu]\n", progname, name,
	    lis -> received, line, client -> hostname, client -> port, client -> received);

  /* Skip leading spaces */
  if (! (start = skip_white_space (line)))
    goto end;

  /* Split the line into arguments */
  if (! (argv = buildargv (start)))
    goto end;

#if defined(FIXME)
  /* Look first if the command is implemented in the current plugin (if any) */
  if ((p = pwd (client)) && (call = function (argv [0], p -> symbols)))
    {
      int cargc = 3;
      char * cargv [3] = { argv [0], (char *) replyln, (char *) client };

      /* Execute the command in the plugin */
      call (cargc, cargv);
    }
#endif /* FIXME */

  /* Look for the command in the main table */
  if (! call)
    {
      if (! (cmd = cmdlookup (argv [0])))
	replyln (client, "Undefined command: \"%s\".  Try \"help\"", argv [0]);
      else
	/* Execute the command */
	(* cmd -> handler) (client, argslen (argv), argv);
    }

 end:
  freeargv (argv);

  /* Avoid to use again the 'client' pointer in case it was removed due to logout */
  c = lis -> clients;
  while (c && * c)
    {
      if (* c == client)
	{
	  /* Add again the file descriptor to the list of those monitored for events */
	  event_add (& client -> read_evt, NULL);

	  /* Start the idle timer on the connection (if any) */
	  if (milliseconds (& client -> idle))
	    event_add (& client -> idle_timer, & client -> idle);

	  /* Put a prompt to the user */
	  reply (client, run -> prompt);
	}
      c ++;
    }
}


/* Called by the event handler each time a new connect event will occur on the listening endpoint */
static void welcome_callback (int fd, short event, void * _server)
{
  server_t * server = _server;
  char * remote = NULL;
  int port = -1;

  int clientfd;
  client_t * client;

  /* Attempt to accept the request */
  if ((clientfd = welcome (fd, & remote, & port)) == -1)
    {
      /* Update the number of connections rejected */
      server -> rejected ++;

      if (run -> accessfp)
	fprintf (run -> accessfp, "%s [%s]: #%zu incoming connection from %s:%d rejected (cannot accept)\n",
		 progname, name, server -> rejected, remote, port), fflush (run -> accessfp);

      if (remote)
	free (remote);

      return;
    }

  /* Check if the server is busy */
  if (server -> maxconns && clientlen (server -> clients) >= server -> maxconns)
    {
      /* Update the number of connections rejected */
      server -> rejected ++;

      if (run -> accessfp)
	fprintf (run -> accessfp, "%s [%s]: #%zu incoming connection from %s:%d rejected (server busy)\n",
		 progname, name, server -> rejected, remote, port), fflush (run -> accessfp);

      if (remote)
	free (remote);

      close (clientfd);

      return;
    }

  /* Update the number of connections accepted */
  server -> accepted ++;

  if (run -> accessfp)
    fprintf (run -> accessfp, "%s [%s]: #%zu incoming connection from %s:%d accepted on fd %d\n",
	     progname, name, server -> accepted, remote, port, clientfd), fflush (run -> accessfp);

  /* Allocate a new descriptor to store info about the calling application */
  server -> clients = moreclient (server -> clients, server, clientfd, remote, port,
				  milliseconds (& server -> login),
				  milliseconds (& server -> idle),
				  0,
				  run -> showrecv, run -> showsent,
				  & client, NULL);
  if (remote)
    free (remote);

  /* Define here all the timer callbacks */
  evtimer_set (& client -> idle_timer, idle_callback, client);

  /* Add the file descriptor to the list of those monitored for read events */
  event_set (& client -> read_evt, clientfd, EV_READ, data_callback, client);
  event_add (& client -> read_evt, NULL);

  /* Set here the login timeout */
  if (milliseconds (& server -> login))
    event_add (& client -> idle_timer, & server -> login);

  /* Welcome banner */
  replyln (client, BANNER);

  /* Put a prompt to the user */
  reply (client, run -> prompt);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


/* Will be called once when the plugin is unloaded */
int halt (int argc, char * argv [])
{
  /* Release resources and memory */
  nomoreopt (opt);
  nomorecfg (cfg);
  nomorerun (run);

  return PLUGIN_OK;
}


/* Will be called once when the plugin is loaded.
 * Initialize run-time parameters and bind to one or more
 * address/port to listen for incoming connections talking the Telnet Protocol */
int boot (int argc, char * argv [])
{
  /* The place for local variables */
  int option;
  char ** addr;
  server_t ** l;

  /* Notice the plugin name */
  char * plgname = strrchr (argv [0], '/');
  plgname = ! plgname ? * argv : plgname + 1;

  /* Initialize optional, configuration and run-time variables to default values */
  opt = defaultopt ();
  cfg = defaultcfg ();
  run = defaultrun (plgname);

  optind = 0;
  optarg = NULL;
#define OPTSTRING "hvc:l:a:b:q:m:L:I:rs"
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

	  /* Addressing */
        case 'b': opt -> addrs      = argsadd (opt -> addrs, optarg); break; /* Local addresses */
        case 'q': opt -> backlog    = atoi (optarg); break;                  /* Socket listen queue size */
        case 'm': opt -> maxconns   = atoi (optarg); break;                  /* Max number of connections per address */

	  /* Timeouts */
        case 'L': opt -> login      = atoi (optarg); break;                  /* Login timeout */
        case 'I': opt -> idle       = atoi (optarg); break;                  /* Idle timeout */

        case 'r': opt -> showrecv = 1; break;                                /* Show packets received */
        case 's': opt -> showsent = 1; break;                                /* Show packets sent */
        }
    }

  /* Load configuration parameters from configuration file (if any) */
  if (opt -> conffile || run -> conffile)
    {
      printf ("%s [%s]: Reading section [%s] in file '%s' ... ", progname, name, name, opt -> conffile ? opt -> conffile : run -> conffile);

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

  /* Start with a default address in case none was chosen */
  if (! run -> addrs)
    run -> addrs = argsadd (NULL, DEFAULT_ADDRESS);

  /* Bind on given address(s)/port(s) to listen for incoming connections */
  addr = run -> addrs;
  while (* addr)
    run -> servers = moreservers (run -> servers, * addr ++,
				  run -> backlog, run -> maxconns,
				  run -> user, run -> password, NULL,
				  run -> login, run -> idle, 0);
  l = run -> servers;
  if (l)
    while (* l)
      {
	/* Ready message */
	printf ("%s [%s]: Plugin ready, now accepting connections on %s:%d\n",
		progname, name, (* l) -> address ? (* l) -> address : ANYADDRESS, (* l) -> port);

	/* Add the listening file descriptor to the list of those monitored for read events */
	event_set (& (* l) -> listen_evt, (* l) -> fd, EV_READ | EV_PERSIST, welcome_callback, * l);
	event_add (& (* l) -> listen_evt, NULL);

	l ++;
      }
  else
    {
      printf ("%s [%s]: no valid local address could be used to listen to => ", progname, name);
      argsline (run -> addrs, ' ');

      /* Release resources and memory */
      halt (argc, argv);

      return PLUGIN_FAIL;
    }

  return PLUGIN_OK;
}
