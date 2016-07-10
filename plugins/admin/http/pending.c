/*
 * pending.c - Handle a table of 'pending' ark's plugins
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
#include <dlfcn.h>


static int pnd_number (plugin_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


static plugin_t * pnd_bypath (plugin_t * argv [], char * path)
{
  while (path && argv && * argv)
    if (! strcmp (path, (* argv ++) -> path))
      return * -- argv;
  return NULL;
}


static plugin_t * pnd_byname (plugin_t * argv [], char * name)
{
  while (name && argv && * argv)
    if (! strcmp (name, pluginname (* argv ++)))
      return * -- argv;
  return NULL;
}


static void pnd_freeplugin (plugin_t * p)
{
  symbol_t ** s;

  if (! p)
    return;

  if (p -> handle)
    dlclose (p -> handle);

  if (p -> path)
    free (p -> path);

  argsfree (p -> declared);
  argsfree (p -> defs);

  /* Free symbols */
  s = p -> symbols;
  while (s && * s)
    free ((* s) -> name),
      free (* s ++);
  if (p -> symbols)
    free (p -> symbols);

  free (p);
}


static plugin_t ** pnd_add (plugin_t * argv [], plugin_t * p)
{
  int argc = pnd_number (argv);

  if (! p)
    return NULL;

  /* Buy memory */
  if (! (argv = realloc (argv, (1 + argc + 1) * sizeof (plugin_t *))))
    {
      pnd_freeplugin (p);
      return NULL;
    }

  argv [argc ++] = p;
  argv [argc]    = NULL;      /* make the table NULL terminated */

  return argv;
}


static int pnd_getindex (plugin_t * argv [], char * name)
{
  int index = -1;

  /* Ritorna il plugin di nome 'name' in 'pending'*/
  plugin_t * plug = pnd_byname (argv, name);

  while (plug && argv && * argv)
    if (* argv ++ == plug)
      return index + 1;
    else
      index ++;

  return index;
}


static plugin_t ** pnd_remove (plugin_t * argv [], char * name)
{
  int index;
  int j;

  int argc = pnd_number (argv);

  /* Trovo l'indice del plugin nella lista 'pending' */
  if ((index = pnd_getindex (argv, name)) != -1)
    {
      for (j = index; j < argc - 1; j ++)               /* move pointers back one position */
	argv [j] = argv [j + 1];

      argv [j] = NULL;                                 /* terminate the table */

      if (argc > 1)
	argv = realloc (argv, argc * sizeof (plugin_t *));
      else
	free (argv), argv = NULL;
    }
  return argv;
}


static int sort_by_name (const void * _a, const void * _b)
{
  return strcmp (pluginname (* (plugin_t **) _a), pluginname (* (plugin_t **) _b));
}


static int pnd_load (plugin_t * plug, int argc, char * argv [])
{
  call_t * call;
  int n;

  if (! plug)
    return 0;

  n = pluginsno ();
  if (! (loaded = realloc (loaded, (1 + n + 1) * sizeof (plugin_t *))))
    return 0;

  loaded [n ++] = plug;
  loaded [n]    = NULL;     /* do the table always NULL terminated */

  /* Always keeps sorted the table by name */
  qsort (loaded, n, sizeof (char *), sort_by_name);

  if ((call = function (START, plug -> symbols)) && ! call (argc, argv))
    {
      printf ("%s [%s]: plugin cannot be started\n", progname, pluginname (plug));
      return 0;
    }

  plug -> state = STARTED;

  return 1;
}


static void pnd_free (plugin_t * argv [])
{
  plugin_t ** list = argv;

  while (argv && * argv)
    pnd_freeplugin (* argv ++);

  if (list)
    free (list);
}

#if defined(TEST)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>
#include <sys/utsname.h>
#if !defined(DONT_HAVE_GETOPT_H)
# include <getopt.h>
#endif

/* Private header file(s) */
#include "ark.h"
#include "html.h"

plugin_t * mkplugin (char * path);


int main (int argc, char * argv [])
{
  char * path  = "../../../modules/pong.so";
  char * path1 = "../../../modules/pong.so";
  char * path2 = "../../../modules/ping.so";
  char * path3 = "../../../modules/ucpd.so";

  plugin_t * plug;
  plugin_t * plug1;
  plugin_t * plug2;
  plugin_t * plug3;
  plugin_t ** pending = NULL;

  plugin_t ** p;
  int max = 3;
  int i;
  int n;
  int m;

  if (argc > 1)
    path = argv [1];

  /* Test #1 => single item */
  rmplugin (mkplugin (path1), NULL);

  /* Test #2 => single item in a table */
  pnd_free (pnd_add (pending, mkplugin (path1)));
  pending = NULL;

  /* Test #3 => single item in a table removed by hand */
  plug = mkplugin (path1);
  pending = pnd_add (pending, plug);
  pending = pnd_remove (pending, pluginname (plug));
  rmplugin (plug, NULL);
  pending = NULL;

  /* Test #4 => more items in a table */
  plug = mkplugin (path1);
  pending = pnd_add (pending, plug);
  plug = mkplugin (path2);
  pending = pnd_add (pending, plug);
  plug = mkplugin (path3);
  pending = pnd_add (pending, plug);
  pnd_free (pending);
  pending = NULL;

  /* Test #5 => more items in a table removed in reverse order */
  plug1 = mkplugin (path1);
  pending = pnd_add (pending, plug1);
  plug2 = mkplugin (path2);
  pending = pnd_add (pending, plug2);
  plug3 = mkplugin (path3);
  pending = pnd_add (pending, plug3);
  pending = pnd_remove (pending, pluginname (plug3));
  pending = pnd_remove (pending, pluginname (plug2));
  pending = pnd_remove (pending, pluginname (plug1));
  rmplugin (plug3, NULL);
  rmplugin (plug2, NULL);
  rmplugin (plug1, NULL);
  pending = NULL;

  /* Test #6 => more items in a table removed in same order */
  plug1 = mkplugin (path1);
  pending = pnd_add (pending, plug1);
  plug2 = mkplugin (path2);
  pending = pnd_add (pending, plug2);
  plug3 = mkplugin (path3);
  pending = pnd_add (pending, plug3);
  pending = pnd_remove (pending, pluginname (plug1));
  pending = pnd_remove (pending, pluginname (plug2));
  pending = pnd_remove (pending, pluginname (plug3));
  rmplugin (plug1, NULL);
  rmplugin (plug2, NULL);
  rmplugin (plug3, NULL);
  pending = NULL;

  /* Test #7 => just a single item in a table searched by path */
  plug1 = mkplugin (path1);
  pending = pnd_add (pending, plug1);
  plug2 = pnd_bypath (pending, path1);
  if (plug2 != plug1)
    printf ("Error: pnd_bypath() failed due to different pointers\n");
  plug2 = pnd_bypath (pending, path2);
  if (plug2)
    printf ("Error: pnd_bypath() failed due to unexpected pointer\n");
  pnd_free (pending);
  pending = NULL;

  /* Test #8 => just a single item in a table searched by name */
  plug1 = mkplugin (path1);
  pending = pnd_add (pending, plug1);
  plug2 = pnd_byname (pending, pluginname (plug1));
  if (plug2 != plug1)
    printf ("Error: pnd_byname() failed due to different pointers\n");
  plug2 = pnd_byname (pending, "unloaded");
  if (plug2)
    printf ("Error: pnd_byname() failed due to unexpected pointer\n");
  pnd_free (pending);
  pending = NULL;

  return 0;
}

/*
 * Compile commands:
 * gcc -g -DTEST -I../../../libevent -I ../../../src pending.c ../../../libevent/.libs/libevent.so ../../../libucp/libucp.so ../../../src/libark.so -lsocket -lnsl -lm -ldl -o test
 */

#endif /* TEST */
