/*
 * vfs.c - Implementation of a virtual file system in memory
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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/* Private header file(s) */
#include "ark-html.h"


/* Initialize the hash table used to keep the Virtual File System */
void vfs_init (vfs * vfs, unsigned int size)
{
  /* Initialize the hash table */
  memset (vfs, '\0', sizeof (* vfs));
  vfs -> size = size;

  hash_table_init (vfs);
}


/* Insert an element (key, value) into the hash table */
void vfs_insert (vfs * vfs, char * key, char * value)
{
  struct hdatum pair;

  /* This is the absolute pathname */
  pair . key   = key;
  pair . ksize = strlen (key);
  /* This is the content */
  pair . val   = value;
  pair . vsize = value ? strlen (value) : 0;

  /* Insert the filename and its content into the table */
  hash_table_insert (vfs, & pair);
}


/* Lookup for the value associated to a given key into the hash table */
char * vfs_lookup (vfs * vfs, char * key)
{
  struct hdatum pair;
  struct hdatum * found;

  if (! key)
    return NULL;

  memset (& pair, 0, sizeof (struct hdatum));
  pair . key   = key;
  pair . ksize = strlen (key);

  return (found = hash_table_search (vfs, & pair)) ? (char *) found -> val : NULL;
}


/* Free the memory */
void vfs_free (vfs * vfs, GNode * root)
{
  if (root)
    g_node_destroy (root);

  if (vfs)
    hash_table_free (vfs);
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


/*
 * path        dirname  basename
 *
 * "/usr/lib"  "/usr"   "lib"
 * "/usr/"     "/"      "usr"
 * "usr"       "."      "usr"
 * "/"         "/"      "/"
 * "."         "."      "."
 * ".."        "."      ".."
 */
char * get_basename (char * path)
{
  char * cp = strrchr (path, '/');

  if (cp && * (cp + 1))
    return cp + 1;
  else
    return path;
}


char * get_dirname (char * path)
{
  static char bname [1024];
  const char * endp;

  /* Empty or NULL string gets treated as "." */
  if (! path || ! * path)
    {
      strcpy (bname, ".");
      return bname;
    }

  /* Strip trailing slashes */
  endp = path + strlen (path) - 1;
  while (endp > path && * endp == '/')
    endp --;

  /* Find the start of the dir */
  while (endp > path && * endp != '/')
    endp --;

  /* Either the dir is "/" or there are no slashes */
  if (endp == path)
    {
      strcpy (bname, * endp == '/' ? "/" : ".");
      return bname;
    }
  else
    {
      do
        {
          endp --;
        } while (endp > path && * endp == '/');
    }

  if (endp - path + 1 > sizeof (bname))
    {
      errno = ENAMETOOLONG;
      return NULL;
    }
  strncpy (bname, path, endp - path + 1);
  bname [endp - path + 1] = '\0';
  return bname;
}


/* Return the full pathname (e.g. "dirname/name") */
char * get_fullname (char * dirname, char * name)
{
  if (! strcmp (dirname, "/"))
    return strcat (strcat ((char *) calloc (1, strlen (name) + 2), "/"), name);
  else
    return strcat (strcat (strcat ((char *) calloc (1, strlen (dirname) + strlen (name) + 2), dirname), "/"), name);
}


/* Create a virtual 'filename' under 'dirname' with given value */
void vfs_mkfile (vfs * vfs, GNode * node, char * dirname, char * filename, char * value)
{
  char * key = get_fullname (dirname, filename);

  g_node_append (node, g_node_new (key));

  /* Populate the file table now */
  vfs_insert (vfs, key, value);
}


/* Create a virtual directory 'dirname' under 'parent' */
void vfs_mkdir (GNode * node, char * parent, char * dirname)
{
  g_node_append (node, g_node_new (get_fullname (parent, dirname)));
}


int dotordotdot (const char * name)
{
  int len = strlen (name);
  return ! strncmp (name + len - 3, "/..", 3) || ! strncmp (name + len - 2, "/.", 2);
}


/* Check if 'filename' (which is set to an absolute pathname) is a directory */
int vfs_isdir (char * filename, GNode * root)
{
  GNode * node;

  if (! filename || ! filename [0])
    return 0;

  /* Visit the N-Way tree to find a node with the given name (if any) */
  node = g_node_find (root, G_PRE_ORDER, G_TRAVERSE_ALL, filename);

  if (! node)
    /* filename not found */
    return 0;

  if (! G_NODE_IS_LEAF (node) || dotordotdot ((char *) node -> data))
    return 1;
  else
    return 0;
}


/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


void indent (struct evbuffer * buf, unsigned level, unsigned last, unsigned more)
{
  /* TODO rivedere e modificare, solo per prova! */
  char * s = malloc (200);
  char * stringa = malloc (200);

  int i;
  for (i = 0; i < level; i ++)
    {
      sprintf (s, "%s ",
	       i == level - 1
	       ?
	       last ? "`---" : "|---"
	       :
	       more ? "|&nbsp;&nbsp;&nbsp;&nbsp;" : "|&nbsp;&nbsp;&nbsp;&nbsp;");

      stringa = strcat (stringa, s);
    }

  free (s);

  evbuffer_add_printf (buf, "%s", stringa);
}



/* TODO rivedere html per le table, errato! */
void print_html_tree (struct evbuffer * buf, const char * pname, vfs * vfs, GNode * n, unsigned lev)
{
  evbuffer_add_printf (buf, "<center>\n");

  while (n)
    {
      evbuffer_add_printf (buf,
			   "<table align=center cellpadding=1 cellspacing=0 border=0>\n"
			   " <tr>\n"
			   "  <td width=500 align=left><font face=verdana size=2>");

      indent (buf, lev, n -> next ? 0 : 1, n -> parent && n -> parent -> next ? 1 : 0);

      if (G_NODE_IS_LEAF (n))
	evbuffer_add_printf (buf,
			     "%s &nbsp;&nbsp;--->&nbsp;[%s]\n",
			     get_basename ((char *) n -> data),    /* basename (visualizzato) */
			     vfs_lookup (vfs, (char *) n -> data) ? vfs_lookup (vfs, (char *) n -> data) : "none");
      else
	evbuffer_add_printf (buf,
			     "<a href=/%s/vfs?node=%s><b>%s</b></a>\n",
			     pname,
			     (char *) n -> data,
			     (char *) n -> data);

      evbuffer_add_printf (buf, "</font></td></tr>");

      print_html_tree (buf, pname, vfs, n -> children, lev + 1);
      n = n -> next;
    }

  evbuffer_add_printf (buf, "</table>");
}
