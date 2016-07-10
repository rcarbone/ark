/*
 * ark.c - The plugin manager to load/unload/query plugins
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
#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>

/* Private header file(s) */
#include "ark.h"


/* The table of mandatory names that __must__ be defined in the plugin */
static char * mandatory [] = { VARS, FUNCS, NULL };



/* Returns the address of the symbol [name] from the plugin */
static char * dlsym_variable (char * name, void * handle)
{
  char ** symbol = (char **) dlsym (handle, name);
  return symbol ? * symbol : NULL;
}


/* Returns the address of the symbol [name] from the plugin */
static call_t * dlsym_function (char * name, void * handle)
{
  call_t * symbol = (call_t *) dlsym (handle, name);
  return symbol ? symbol : NULL;
}


/* Make a new symbol */
static symbol_t * mksymbol (char * name, void * handle, char type)
{
  symbol_t * s = NULL;
  void * symbol;

  if (type == VARIABLE)
    symbol = dlsym_variable (name, handle);
  else
    symbol = dlsym_function (name, handle);
  if (symbol)
    {
      if (! (s = (symbol_t *) calloc (sizeof (symbol_t), 1)))
	return NULL;
      s -> name   = strdup (name);
      s -> symbol = symbol;
      s -> type   = type;
    }

  return s;
}


/* How many items are in a the NULL terminated table? */
static int lensymbol (symbol_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Add an element to the table of arguments */
static symbol_t ** addsymbol (symbol_t * argv [], symbol_t * s)
{
  int argc = lensymbol (argv);
  if (s)
    {
      /* buy memory */
      if (! (argv = (symbol_t **) realloc (argv, (1 + argc + 1) * sizeof (symbol_t *))))
	return NULL;
      argv [argc ++] = s;
      argv [argc] = NULL;            /* make the table NULL terminated */
    }

  return argv;
}


/* Free the symbol table */
static void freesymbols (symbol_t * argv [])
{
  symbol_t ** s = argv;
  while (argv && * argv)
    free ((* argv) -> name),
      free (* argv ++);
  if (s)
    free (s);
}


/* Is 'name' defined? */
void * defined (char * name, symbol_t * argv [])
{
  while (name && argv && * argv)
    if (! strcmp (name, (* argv ++) -> name))
      return (* -- argv) -> symbol;
  return NULL;
}


/* Is 'name' a variable? */
char * variable (char * name, symbol_t * argv [])
{
  while (name && argv && * argv)
    if (! strcmp (name, (* argv) -> name) && (* argv) -> type == VARIABLE)
      return (* argv) -> symbol;
    else
      argv ++;
  return NULL;
}


/* Is 'name' a function? */
call_t * function (char * name, symbol_t * argv [])
{
  while (name && argv && * argv)
    if (! strcmp (name, (* argv) -> name) && (* argv) -> type == FUNCTION)
      return (* argv) -> symbol;
    else
      argv ++;
  return NULL;
}


/* The function plgerror() keeps a human readable string describing
 * the most recent error that occurred since the last call to load_plugin().
 * It returns "" if no errors have occurred since initialization
 */
static char * plgerror (const char * fmt, ...)
{
  static char error [512] = "";
  va_list ap;

  if (fmt)
    {
      va_start (ap, fmt);
      vsnprintf (error, sizeof (error), fmt, ap);
      va_end (ap);
      return NULL;
    }
  return error;
}


/* Return the plugin name */
char * pluginname (plugin_t * p)
{
  char * name;
  return p && (name = variable ("name", p -> symbols)) ? name : "unknown";
}


/* Return the plugins currently loaded in form of a NULL terminated array */
plugin_t ** running (void)
{
  return loaded;
}


/* Return the number of currently loaded plugins */
int pluginsno (void)
{
  plugin_t ** argv = loaded;
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Lookup into the table of the loaded plugins for that referenced by [name] */
plugin_t * pluginbyname (char * name)
{
  plugin_t ** argv = loaded;
  while (name && argv && * argv)
    if (! strcmp (name, pluginname (* argv ++)))
      return * -- argv;
  return NULL;
}


/* Lookup into the table of the loaded plugins for the state of that referenced by [name] */
int pluginstate (char * name)
{
  plugin_t ** argv = loaded;
  while (name && argv && * argv)
    if (! strcmp (name, pluginname (* argv ++)))
      return (* -- argv) -> state;
  return UNKNOWN;
}


/* Lookup into the table of the loaded plugins for that referenced by pointer [p] */
static int pluginlookup (plugin_t * argv [], plugin_t * p)
{
  int found = -1;

  while (p && argv && * argv)
    if (* argv ++ == p)
      return found + 1;
    else
      found ++;

  return found;
}


/* Free allocated memory and resources used to store a plugin descriptor */
plugin_t * rmplugin (plugin_t * p, const char * fmt, ...)
{
  static char error [512] = "";
  va_list ap;

  if (p)
    {
      call_t * halt;
      char * argv [3] = { STOP, "now", NULL };

      if (fmt)
	{
	  va_start (ap, fmt);
	  vsnprintf (error, sizeof (error), fmt, ap);
	  plgerror ("%s", error);
	  va_end (ap);
	}

      /* First halt the plugin (if started) */
      if (p -> state == STARTED && (halt = function (STOP, p -> symbols)))
	halt (3, argv);

      if (p -> path)
	free (p -> path);

      argsfree (p -> declared);
      argsfree (p -> defs);
      freesymbols (p -> symbols);

      if (p -> handle)
	dlclose (p -> handle);

      free (p);
    }

  return NULL;
}


/* Allocate and initialize a new descriptor to keep info about a plugin */
plugin_t * mkplugin (char * path)
{
  void * handle;
  plugin_t * p = NULL;
  char ** names;
  char * symb;
  symbol_t * s = NULL;

  /* Load the shared object into memory */
  if (! (handle = dlopen (path, RTLD_NOW)))
    return (plugin_t *) plgerror ("Error [%s]\n", dlerror ());

  /* Time for memory */
  if (! (p = (plugin_t *) calloc (sizeof (plugin_t), 1)))
    return dlclose (handle), (plugin_t *) plgerror ("Not enough memory to allocate a plugin object\n");

  p -> handle   = handle;
  p -> path     = strdup (path);
  p -> declared = NULL;
  p -> defs     = NULL;
  p -> symbols  = NULL;
  p -> state    = DUMMY;

  /* Load plugin declarations.  Stop if not all mandatory plugin declarations have been found */
  names = mandatory;
  symb = (char *) 1;                 /* a true value */
  while (symb && names && * names)
    p -> declared = argsadd (p -> declared, symb = dlsym_variable (* names ++, handle));

  if (! symb)
    return rmplugin (p, "Missing or illegal mandatory plugin declaration '%s' in file %s\n", * -- names, path);

  /* Split declarations into arguments and load symbol table */
  names = p -> declared;
  while (names && * names)
    {
      char ** defs;
      defs = p -> defs = argsblanks (* names);       /* split at blanks */

      s = (symbol_t *) 1;            /* a true value */
      while (s && defs && * defs)
	p -> symbols = addsymbol (p -> symbols,
				  s = mksymbol (* defs ++, handle,
						names == p -> declared ? VARIABLE : FUNCTION));
      if (! s)
	return rmplugin (p, "symbol '%s' declared but not defined in file '%s'\n", * -- defs, path);

      argsfree (p -> defs);
      p -> defs = NULL;

      ++ names;
    }

  /* Check now for an unique plugin name */
  if (pluginbyname (pluginname (p)))
    return rmplugin (p, "A plugin named '%s' is already loaded\n", pluginname (p));

  /* Bless it! */
  gettimeofday (& p -> uptime, NULL);
  p -> state = LOADED;

  return p;
}


static int sort_by_name (const void * _a, const void * _b)
{
  return strcmp (pluginname (* (plugin_t **) _a), pluginname (* (plugin_t **) _b));
}


/* Load shared file at [path] and initialize a plugin object */
static plugin_t ** add_plugin (plugin_t * argv [], char * path, plugin_t ** p)
{
  int argc;
  if ((* p = mkplugin (path)))
    {
      argc = pluginsno ();
      if (! (argv = (plugin_t **) realloc (argv, (1 + argc + 1) * sizeof (plugin_t **))))
        {
          rmplugin (* p, "Not enough memory to reallocate the table of plugins\n");
          return NULL;
        }
      argv [argc ++] = * p;
      argv [argc]    = NULL;     /* do the table always NULL terminated */

      /* Always keeps sorted the table by name */
      qsort (argv, argc, sizeof (char *), sort_by_name);
    }

  return argv;
}


/* Unload the plugin by name */
plugin_t ** unload_plugin (char * name)
{
  int found;
  int j;
  int argc = pluginsno ();
  plugin_t * p;

  if ((found = pluginlookup (loaded, p = pluginbyname (name))) != -1)
    {
      /* [name] is no longer available after plugin has been removed */
      if (p -> state == STARTED)
	printf ("%s [%s]: Plugin halted\n", progname, name);

      rmplugin (loaded [found], NULL);           /* free the descriptor of the item found */

      for (j = found; j < argc - 1; j ++)        /* move pointers back one position */
        loaded [j] = loaded [j + 1];

      loaded [j] = NULL;                         /* terminate the table */

      if (argc > 1)
        loaded = (plugin_t **) realloc (loaded, argc * sizeof (plugin_t *));
      else
        free (loaded), loaded = NULL;
    }

  return loaded;
}


/* Try to load the plugin at [path] and add the plugin to the array of loaded plugins */
plugin_t ** load_plugin (char * path, int argc, char * argv [], int verbose)
{
  plugin_t * p = NULL;
  call_t * call;

  /* Attempt to load and start the plugin */
  loaded = add_plugin (loaded, path, & p);
  if (! p)
    {
      if (verbose)
	printf ("%s: %s", progname, plgerror (NULL));
      return NULL;
    }
  else if ((call = function (START, p -> symbols)) && ! call (argc, argv))
    {
      printf ("%s [%s]: plugin cannot be started\n", progname, pluginname (p));
      unload_plugin (pluginname (p));
      return NULL;
    }

  p -> state = STARTED;

  return loaded;
}


/* Load a plugin ande, if it supports web interface show its homepage */
plugin_t * homepage (char * path, int argc, char * argv [])
{
  plugin_t * p = NULL;
  call_t * call;

  /* Attempt to load the plugin */
  if (! (p = mkplugin (path)))
    {
      printf ("%s: %s", progname, plgerror (NULL));
      return NULL;
    }
  else if (! (call = function (HOMEPAGE, p -> symbols)) || ! call (argc, argv))
    {
      /* No web */
      rmplugin (p, NULL);
      return NULL;
    }

  return p;
}


/* Fill the table of available plugins (filenames with "*.so" extension) from the directory pointed to by [dir] */
char ** available_in_dir (char * dir)
{
  DIR * pd;
  struct dirent * ep;
  char * file;
  int len;
  char ** argv = NULL;

  if (dir && (pd = opendir (dir)))
    {
      while ((ep = readdir (pd)))
	{
	  file = calloc (strlen (dir) + 1 + strlen (ep -> d_name) + 1, 1);
	  sprintf (file, "%s/%s", dir, ep -> d_name);
	  len = strlen (file);

	  if (len > 3 && ! strncmp (& file [len - 3], ".so", 3) &&
	      strncmp (& file [len - strlen (INTERNAL_LIB)], INTERNAL_LIB, strlen (INTERNAL_LIB)))
	    argv = argsadd (argv, file);    /* Add the item to the table */
	  free (file);
	}
      closedir (pd);
    }

  return argv;
}



/* Fill the table of all available plugins (filenames with "*.so" extension except INTERNAL_LIB)
 * from the directories pointed to by [dirs] */
char ** available_plugins (char * dirs [])
{
  DIR * pd;
  struct dirent * ep;
  char * file;
  int len;
  char ** argv = NULL;

  /* Directories with plugins */
  char ** d = dirs;

  while (d && * d)
    {
      if ((pd = opendir (* d)))
	{
	  while ((ep = readdir (pd)))
	    {
	      file = calloc (strlen (* d) + 1 + strlen (ep -> d_name) + 1, 1);
	      sprintf (file, "%s/%s", * d, ep -> d_name);
	      len = strlen (file);

	      if (len > 3 && ! strncmp (& file [len - 3], ".so", 3) &&
		  strncmp (& file [len - strlen (INTERNAL_LIB)], INTERNAL_LIB, strlen (INTERNAL_LIB)))
		argv = argsadd (argv, file);    /* Add the item to the table */
	      free (file);
	    }
	  closedir (pd);
	}
      d ++;      /* Next directory */
    }
  return argv;
}


/* Try to load plugins in [plugins] (filenames with the extension "*.so" except INTERNAL_LIB)
 * looking in the directories pointed to by [dirs] */
void load_plugins (char * plugins [], char * dirs [], char * conffile)
{
  char * file;
  int len;

  char ** argv;
  char ** d;
  char ** p = plugins;
  int done;

  while (p && * p)
    {
      argv = argsblanks (* p);    /* split at blanks */
      if (conffile)
	argv = argsadd (argv, "-c"),
	  argv = argsadd (argv, conffile);

      /* Check for absolute path name */
      if (* argv [0] == '/')
	load_plugin (argv [0], argslen (argv), argv, 1);
      else
	{
	  done = 0;
	  d = dirs;
	  while (! done && d && * d)
	    {
	      file = (char *) malloc (strlen (* d) + strlen (argv [0]) + 2);
	      sprintf (file, "%s/%s", * d, argv [0]);
	      len = strlen (file);
	      if (len > 3 && ! strncmp (& file [len - 3], ".so", 3) &&
		  strncmp (& file [len - strlen (INTERNAL_LIB)], INTERNAL_LIB, strlen (INTERNAL_LIB)))
		if (load_plugin (file, argslen (argv), argv, 1))
		  done = 1;
	      free (file);
	      d ++;    /* next directory */
	    }
	}
      argsfree (argv);
      p ++;            /* next plugin */
    }
}


/* Try to load all plugins (filenames with "*.so" extension except INTERNAL_LIB) from the directories pointed to by [dirs] */
void load_all_plugins (char * dirs [], char * conffile)
{
  DIR * pd;
  struct dirent * ep;
  char ** argv;
  char * file;
  int len;

  char ** d = dirs;

  while (d && * d)
    {
      if ((pd = opendir (* d)))
	{
	  while ((ep = readdir (pd)))
	    {
	      file = calloc (strlen (* d) + 1 + strlen (ep -> d_name) + 1, 1);
	      sprintf (file, "%s/%s", * d, ep -> d_name);
	      len = strlen (file);
	      if (len > 3 && ! strncmp (& file [len - 3], ".so", 3) &&
		  strncmp (& file [len - strlen (INTERNAL_LIB)], INTERNAL_LIB, strlen (INTERNAL_LIB)))
		{
		  argv = NULL;
		  argv = argsadd (argv, file);
		  if (conffile)
		    argv = argsadd (argv, "-c"),
		      argv = argsadd (argv, conffile);
		  if (! load_plugin (file, argslen (argv), argv, 1))
		    printf ("%s: Failed to load/start shared object '%s'\n", progname, file);
		  argsfree (argv);
		}
	      free (file);
	    }
	  closedir (pd);
	}
      else
	printf ("%s: Error while reading directory '%s' [%s]\n", progname, * d, strerror (errno));
      d ++;  /* next directory */
    }
}


/* Unload all loaded plugins and free unneeded resources */
void unload_all_plugins (void)
{
  plugin_t ** p = loaded;

  while (p && * p)
    p = unload_plugin (pluginname (* p));

  loaded = NULL;
}


/* Return (if any) the plugin speaking a given protocol and having the push() function available */
static plugin_t * push (int protocol)
{
  plugin_t ** p = loaded;
  call_t * ppush;

  while (p && * p)
    if ((ppush = defined (PUSH, (* p ++) -> symbols)))   /* FIXME!!! add here the check on protocol */
      return * -- p;

  return NULL;
}


/* Return (if any) the plugin speaking a given protocol with the given pull() function */
static plugin_t * pull (int protocol)
{
  plugin_t ** p = loaded;
  call_t * ppull;

  while (p && * p)
    if ((ppull = defined (PULL, (* p ++) -> symbols)))   /* FIXME!!! add here the check on protocol */
      return * -- p;
  return NULL;
}


/*
 * Callback to push data in a buffer
 *
 * Return 0
 *   if data cannot be pushed because no plugin is available at this time
 *     a plugin is not available when:
 *       it is not loaded
 *       it does not have the push() function defined
 *       it does not speak the given protocol
 *   if the called push() funtion of the plugin returned 0
 */
int kpush (int protocol, int argc, char * argv [])
{
  plugin_t * p;
  call_t * ppush;

  return argc && (p = push (protocol)) && (ppush = function (PUSH, p -> symbols)) && ppush (argc, argv);
}


/*
 * Callback to pull data in a buffer
 *
 * Return 0
 *   if data cannot be pulled because no plugin is available at this time
 *     a plugin is not available when:
 *       it is not loaded
 *       it does not have the pull() function defined
 *       it does not speak the given protocol
 *   if the called pull() funtion of the plugin returned 0
 */
int kpull (int protocol, int argc, char * argv [])
{
  plugin_t * p;
  call_t * ppull;

  return argc && (p = pull (protocol)) && (ppull = function (PULL, p -> symbols)) && ppull (argc, argv);
}
