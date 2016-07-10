/*
 * commands.c - Table of available telnet commands for administer other ark's plugins
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
#include <string.h>

/* Private header file(s) */
#include "client.h"


/* Function and Pointer To Function returning void */
typedef void function_t (client_t * c, int argc, char * argv []);


/* The structure containing information on the commands the application can understand */
typedef struct
{
  char       * name;     /* user printable name of the command      */
  function_t * handler;  /* function to call to execute the command */
  char       * help;     /* help string                             */
} cmd_t;


static void clientlogout (client_t * client, int reason);
static void reply (client_t * client, const char * fmt, ...);
static void replyln (client_t * client, const char * fmt, ...);


/* Forward definitions for the builtins commands */
static void do_ava     (client_t * c, int argc, char * argv []);
static void do_help    (client_t * c, int argc, char * argv []);
static void do_ps      (client_t * c, int argc, char * argv []);
static void do_quit    (client_t * c, int argc, char * argv []);
static void do_uptime  (client_t * c, int argc, char * argv []);

#if defined(FIXME)
static void do_load    (client_t * c, int argc, char * argv []);
static void do_unload  (client_t * c, int argc, char * argv []);
static void do_all     (client_t * c, int argc, char * argv []);
static void do_kill    (client_t * c, int argc, char * argv []);

/* Related to the Plugin Virtual File System */
static void do_cd      (client_t * c, int argc, char * argv []);
static void do_ls      (client_t * c, int argc, char * argv []);
static void do_pwd     (client_t * c, int argc, char * argv []);
static void do_swap    (client_t * c, int argc, char * argv []);
static void do_usage   (client_t * c, int argc, char * argv []);
#endif /* FIXME */


/* The table with all the commands */
static cmd_t cmdtable [] =
{
  { "?",      do_help,    "Show available commands" },
  { "ava",    do_ava,     "Show the list of plugins available for loading" },
  { "bye",    do_quit,    "Disconnect" },
  { "exit",   do_quit,    "Disconnect" },
  { "help",   do_help,    "Show available commands" },
  { "ps",     do_ps,      "Report a snapshot of the running plugins" },
  { "quit",   do_quit,    "Disconnect" },
  { "uptime", do_uptime,  "Tell how long the system has been running" },

#if defined(FIXME)
  { ".",      do_load,    "no help yet" },
  { "unload", do_unload,  "no help yet" },

  { "all",    do_all,     "no help yet" },
  { "kill",   do_kill,    "no help yet" },

  { "cd",     do_cd,      "change current plugin" },
  { "ls",     do_ls,      "list plugin contents" },
  { "pwd",    do_pwd,     "print current plugin" },
  { "swap",   do_swap,    "swap current plugin" },
  { "usage",  do_usage,   "give a description about how to use a plugin" },
#endif /* FIXME */

  { NULL,     NULL,       NULL }
};

#define TABLESIZE (sizeof (cmdtable) / sizeof (cmdtable [0]))

static int cmd_max_length (void)
{
  cmd_t * cmd;
  int current_command_length = 0;
  int max_command_length = 0;

  for (cmd = cmdtable; cmd < cmdtable + TABLESIZE; cmd ++)
    {
      current_command_length = cmd -> name ? strlen (cmd -> name) : 0;
      if (max_command_length < current_command_length)
	max_command_length = current_command_length;
    }
  return max_command_length;
}


/*
 * Look up NAME as the name of a command, and return a pointer to that command.
 * Return a NULL pointer if NAME isn't a command name.
 */
static cmd_t * cmdlookup (char * name)
{
  char * p;
  char * q;
  cmd_t * cmd;
  cmd_t * found;

  int nmatches = 0;
  int longest = 0;

  found = NULL;
  for (cmd = cmdtable; (cmd < cmdtable + TABLESIZE) && (p = cmd -> name); cmd ++)
    {
      for (q = name; * q == * p ++; q ++)
	if (* q == 0)		/* exact match ? */
	  return cmd;
      if (! * q)
	{			/* the name was a prefix */
	  if ((q - name) > longest)
	    {
	      longest = q - name;
	      nmatches = 1;
	      found = cmd;
	    }
	  else if (q - name == longest)
	    nmatches ++;
	}
    }

  switch (nmatches)
    {
    case 0:
    case 1:
      return NULL;

    default:
      return found;
    }
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* The 'ava' command */
static void do_ava (client_t * c, int argc, char * argv [])
{
  plugin_t ** pargv = running ();
  int pargc = pluginsno ();

  int rows, cols;
  int i, j;

  int max = argslongest (argv);

  /* how many columns? */
  int width = (max + 8) &~ 7;
  cols = 80 / width;
  if (cols == 0)
    cols = 1;
  rows = (argc + cols - 1) / cols;

  for (i = 0; i < rows; i ++)
    for (j = 0; j < cols; j ++)
      if ((i + j * rows) < pargc)
	reply (c, "%-*.*s", max + 2, max + 2, pluginname (* pargv ++));
      else
	replyln (c, "");
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#define INCREASED 0

/* The 'list help' command */
static void list_help_commands (client_t * c, int argc, char * argv [])
{
  cmd_t * cmd;
  int i, j, w, k;
  int lines, columns, width;

  /*
   * width is a multiple of 8
   * (increased of 2 to show commands not yet implemented enclosed between <>)
   */
  width = (cmd_max_length () + INCREASED + 8) &~ 7;
  columns = 80 / width;
  if (columns == 0)
    columns = 1;
  lines = (TABLESIZE + columns - 1) / columns;

  replyln (c, "Commands may be abbreviated. Commands are:");

  for (i = 0; i < lines; i ++)
    {
      for (j = 0; j < columns; j ++)
	{
	  cmd = cmdtable + j * lines + i;
	  if (cmd -> name)
	    reply (c, "%s", cmd -> name);
	  else
	    for (k = 0; k < INCREASED; k ++)
	      reply (c, "%c", ' ');

	  if (cmd + lines >= cmdtable + TABLESIZE)
	    {
	      replyln (c, "");
	      break;
	    }
	  w = strlen (cmd -> name) + INCREASED;
	  while (w < width)
	    {
	      w = (w + 8) &~ 7;
	      reply (c, "%c", '\t');
	    }
	}
    }
}


/* The 'help' function */
static void do_help (client_t * c, int argc, char * argv [])
{
  cmd_t * cmd;

  if (argc == 1)
    {
      list_help_commands (c, argc, argv);
      return;
    }

  while (-- argc)
    if (! (cmd = cmdlookup (argv [argc])))
      replyln (c, "cmd_t (%s) : invalid help", argv [argc]);
    else
      replyln (c, "%-*s\t%s", 10, cmd -> name, cmd -> help);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#if defined(FIXME)
/* The 'ls' command */
static void do_ls (client_t * c, int argc, char * argv [])
{
  plugin_t * p = pwd (c);
  call_t * ls = p ? function (argv [0], p -> symbols) : NULL;

  int pargc;
  plugin_t ** pargv;

  int max;
  int rows, cols;
  int i, j;
  char * cmdname = argv [0];

  switch (argc)
    {
    case 1:
      /* Print the table of plugins */
      max = longestname ();

      /* how many columns? */
      cols = 80 / ((max + 8) &~ 7);
      if (cols == 0)
	cols = 1;
      rows = (argc + cols - 1) / cols;

      pargc = pluginsno ();
      pargv = running ();
      while (pargv && * pargv)
	for (i = 0; i < rows; i ++)
	  for (j = 0; j < cols; j ++)
	    if ((i + j * rows) < pargc)
	      reply (c, "%-*.*s", max + 2, max + 2, pluginname (* pargv ++));
	    else
	      replyln (c, "");
      break;

    default:
      /* Handle special names "." "./" ".." "../" and "/" "*" */

      /* Find the plugin by name */
      argv ++;
      while (argv && * argv)
	{
	  if ((p = pluginbyname (* argv)))
	    {
	      if ((ls = function (cmdname, p -> symbols)))
		{
		  int cargc = 3;
		  char * cargv [3] = { cmdname, (char *) replyln, (char *) c };

		  ls (cargc, cargv);
		}
	      else
		replyln (c, "%s: No such function defined for %s", cmdname, * argv);
	    }
	  else
	    replyln (c, "%s: No such plugin", * argv);
	  argv ++;
	}
      break;
    }
}
#endif /* FIXME */

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* The 'ps' command */
static void do_ps (client_t * c, int argc, char * argv [])
{
  time_t now = time (0);
  int howmany = pluginsno ();

  plugin_t ** plgs = running ();
  if (! howmany)
    {
      replyln (c, "No plugin is currently running");
      return;
    }

  replyln (c, "%d plugin%s currently running", howmany, howmany > 1 ? "s are" : " is");

  while (plgs && * plgs)
    {
      replyln (c, " [%s] running since %3d days, %2d:%02d:%02d",
	       pluginname (* plgs),
	       days ((* plgs) -> uptime . tv_sec, now),
	       hours ((* plgs) -> uptime . tv_sec, now),
	       mins ((* plgs) -> uptime . tv_sec, now),
	       (now - (* plgs) -> uptime . tv_sec) % 60);
      plgs ++;
    }
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


/* The 'quit' function, called once when a client ask to disconnect */
static void do_quit (client_t * c, int argc, char * argv [])
{
  replyln (c, LOGOUT);

  clientlogout (c, HANGUP);
}


/* The 'uptime' command */
static void do_uptime (client_t * c, int argc, char * argv [])
{
  time_t boottime = since ();
  time_t now = time (0);
  struct tm * tm = localtime (& now);

  replyln (c, "%2d:%02d%s   up %3d days, %2d:%02d:%02d,    %d plugin%s",
	   tm -> tm_hour % 12,
	   tm -> tm_min,
	   tm -> tm_hour > 12 ? "pm" : "am",
	   days (boottime, now),
	   hours (boottime, now),
	   mins (boottime, now),
	   (int) (now - boottime) % 60,
	   pluginsno (), pluginsno () > 1 ? "s" : "");
}


#if defined(FIXME)
/* The 'load' command */
static void do_load (client_t * c, int argc, char * argv [])
{
  argv ++;
  while (argv && * argv)
    {
      if (load_plugin (* argv, 1, & * argv, 1))
	replyln (c, "%s loaded", * argv);
      else
	replyln (c, "%s cannot be loaded", * argv);
      argv ++;
    }
}


/* The 'unload' command */
static void do_unload (client_t * c, int argc, char * argv [])
{
  argv ++;
  while (argv && * argv)
    {
      if (unload_plugin (* argv))
	replyln (c, "%s unloaded - running plugins now %d", * argv, pluginsno ());
      else
	replyln (c, "%s cannot be unloaded", * argv);
      argv ++;
    }
}


/* The 'all' command */
static void do_all (client_t * c, int argc, char * argv [])
{
  char * dirs [] = { ".", NULL };
  load_all_plugins (dirs, NULL);
}


/* The 'kill' command */
static void do_kill (client_t * c, int argc, char * argv [])
{
  unload_all_plugins ();
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* The Plugin's Virtual File System */

static plugin_t * pwd (client_t * c)
{
  return c -> pwd;
}


static void cd (client_t * c, plugin_t * p)
{
  c -> prev = c -> pwd;  c -> pwd = p;
}


static void popd (client_t * c, plugin_t * p)
{
  if (c -> pwd == p)
    {
      c -> pwd = c -> prev;
      c -> prev = NULL;
    }
  else if (c -> prev == p)
    c -> prev = NULL;
}


/* current = previous && previous = active && active = current */
static void swap (client_t * c)
{
  plugin_t * p;
  p = c -> prev; c -> prev = c -> pwd; c -> pwd = p;
}


/* The 'cd' command */
static void do_cd (client_t * c, int argc, char * argv [])
{
  plugin_t * p = NULL;

  switch (argc)
    {
    case 1:
      break;

    case 2:
      /* Handle special names "." "./" ".." "../" and "/" */
      if (! strcmp (argv [argc - 1], ".") || ! strcmp (argv [argc - 1], "./"))
	break;
      else if (! strcmp (argv [argc - 1], "..") || ! strcmp (argv [argc - 1], "../") ||
	       ! strcmp (argv [argc - 1], "/"))  /* only one level currently */
	{
	  /* Clean working plugin */
	  cd (c, NULL);
	  sprintf (prompt, "%s [%s]> ", progname, "/");
	}
      else
	{
	  char * name = * argv [argc - 1] == '/' ? argv [argc - 1] + 1 : argv [argc - 1];
	  /* Find the plugin by name */
	  if ((p = pluginbyname (name)))
	    {
	      /* Change working plugin */
	      cd (c, p);
	      sprintf (prompt, "%s [%s]> ", progname, pluginname (p));
	    }
	  else      
	    replyln (c, "%s: No such plugin.", argv [argc - 1]);
	}
      break;

    default:
      replyln (c, "%s: Too many arguments.", argv [0]);
      break;
    }
}


/* The 'pwd' command */
static void do_pwd (client_t * c, int argc, char * argv [])
{
  switch (argc)
    {
    case 1:
      replyln (c, "%s", pwd (c) ? pluginname (pwd (c)) : "/");
      break;

    default:
      replyln (c, "%s: ignoring non-option arguments", argv [0]);
      break;
    }
}


/* The 'swap' command */
static void do_swap (client_t * c, int argc, char * argv [])
{
  switch (argc)
    {
    case 1:
      swap (c);
      replyln (c, "%s", pwd (c) ? pluginname (pwd (c)) : "/");
      break;

    default:
      replyln (c, "%s: ignoring non-option arguments.", argv [0]);
      break;
    }
}


static void do_usage (client_t * c, int argc, char * argv [])
{
  call_t * pusage = c -> pwd ? defined ("usage", c -> pwd -> symbols) : NULL;

  if (pusage)
    {
      int argc = 3;
      char * argv [3] = { "usage", (char *) replyln, (char *) c };

      pusage (argc, argv);
    }
  else
    replyln (c, "%s: No function defined for usage.", argv [0]);
}
#endif /* FIXME */
