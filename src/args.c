/*
 * args.c - How to handle dynamic arrays of strings
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


/* Macros for min/max */
#if !defined MAX
# define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif



/* How many items are in a the NULL terminated table? */
int argslen (char * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Add an element to the table of arguments */
char ** argsadd (char * argv [], char * s)
{
  int argc = argslen (argv);
  if (s)
    {
      /* buy memory */
      if (! (argv = (char **) realloc (argv, (1 + argc + 1) * sizeof (char *))))
	return NULL;
      argv [argc ++] = strdup (s);
      argv [argc]    = NULL;        /* do the table NULL terminated */
    }

  return argv;
}


/* Lookup for an element into the table of arguments */
int member (char * argv [], char * item)
{
  int i = -1;

  while (item && argv && * argv)
    if (! strcmp (* argv ++, item))
      return i + 1;
    else
      i ++;

  return -1;
}


/* Remove an item from the table of arguments */
char ** argsrm (char * argv [], char * item)
{
  int i;
  int j;
  int argc;

  if ((i = member (argv, item)) != -1)
    {
      argc = argslen (argv);
      free (argv [i]);                   /* free the argument */

      for (j = i; j < argc - 1; j ++)    /* move pointers back one position */
	argv [j] = argv [j + 1];

      argv [j] = NULL;                   /* terminate the table */

      if (argc > 1)
	argv = realloc (argv, argc * sizeof (char *));  /* the size is argc not argc-1 because of trailing NULL */
      else
	free (argv), argv = NULL;
    }

  return argv;
}


/* Replace the element 's' in the table of arguments with 'd' */
void argsreplace (char * argv [], char * s, char * d)
{
  int i;

  if ((i = member (argv, s)) != -1)
    {
      if (argv [i])
	free (argv [i]);                   /* free the argument */
      argv [i] = strdup (d);
    }
}


/* Free memory associated to a NULL terminated table of arguments */
void argsfree (char * argv [])
{
  char ** p = argv;

  if (! argv)
    return;

  while (* p)
    free (* p ++);
  free (argv);
}


/* Duplicate the NULL terminated table 'argv' */
char ** argsdup (char * argv [])
{
  char ** dup = NULL;
  if (argv)
    while (* argv)
      dup = argsadd (dup, * argv ++);

  return dup;
}


/* Concatenate the NULL terminated table 'b' to 'a' */
char ** argscat (char * a [], char * b [])
{
  while (b && * b)
    a = argsadd (a, * b ++);

  return a;
}


/* Print the arguments in a single line (arguments separated by character in c) */
void argsline (char * argv [], char c)
{
  while (argv && * argv)
    {
      printf ("%s", * argv ++);
      if (* argv && (strlen (* argv) != 1 || ** argv != c))
        printf ("%c", c);
    }
  printf ("\n");
}


/* Print the arguments in 'argc' rows (one argument for line) */
void argsrows (char * argv [])
{
  int argc = 0;
  while (argv && * argv)
    printf ("%3d. \"%s\"\n", ++ argc, * argv ++);
}


/* Check for an item in a blank separated list of names */
int argsmemberof (char * name, char * list)
{
  char * item;

  /* First item */
  item = strtok (list, " ");
  while (item)
    {
      if (! strcmp (item, name))
        return 1;
      /* Next item */
      item = strtok (NULL, " ");
    }

  return 0;
}


/* Split a string into pieces */
char ** argspieces (char * list, char * separator)
{
  char ** argv = NULL;
  char * param;
  char * names = strdup (list);

  while ((param = strtok (! argv ? names : NULL, separator)))
    argv = argsadd (argv, param);

  free (names);

  return argv;
}


/* Split a blank separated list of strings into pieces */
char ** argsblanks (char * list)
{
  return argspieces (list, " ");
}


/* Find the longest name */
int argslongest (char * argv [])
{
  int longest = 0;

  while (argv && * argv)
    longest = MAX (longest, strlen (* argv)),
      argv ++;
  return longest;
}


static int sort_by_name (const void * _a, const void * _b)
{
  return strcmp (* (char **) _a, * (char **) _b);
}


/* Sort a table and return the sorted one */
char ** argssort (char * argv [])
{
  int argc = argslen (argv);
  char ** sorted;

  if (! argv)
    return NULL;

  sorted = argsdup (argv);
  qsort (sorted, argc, sizeof (char *), sort_by_name);

  return sorted;
}


/* Join the items in argv */
char * argsjoin (char * argv [])
{
  int size = 0;
  char * join = NULL;
  while (argv && * argv)
    {
      size += (strlen (* argv) + 1 + 1);  /* '\n' plus a blank separator */
      if (join)
	strcat (join, " "),
	  join = realloc (join, size),
	  strcat (join, * argv ++);
      else
	join = malloc (size),
	  strcpy (join, * argv ++);
    }
  return join;
}


/* Find the next item in a list (using round robin routing algorithm) */
char * nextrr (char * argv [])
{
  static int current = -1;      /* this is the index of the first argument to try */

  int argc = argslen (argv);

  if (! argc)
    return NULL;

  current ++;
  current %= argc;              /* round robin in the table of arguments */

  return argv [current];
}



/* fit on a 25x80 terminal by rows */
void args_2d_rows (int argc, char * argv [])
{
  int rows, cols;
  int i, j;

  int max = argslongest (argv);

  if (! max)
    return;

  /* how many columns? */
  cols = 80 / ((max + 8) &~ 7);
  if (cols == 0)
    cols = 1;
  rows = (argc + cols - 1) / cols;

  for (i = 0; i < rows; i ++)
    {
      for (j = 0; j < cols; j ++)
	if (i * cols + j < argc)
	  printf ("%-*.*s", max + 1, max + 1, argv [i * cols + j]);
	else
	  break;
      printf ("\n");
    }
}


/* fit on a 25x80 terminal */
void args_2d_cols (int argc, char * argv [])
{
  int rows, cols;
  int i, j;

  int max = argslongest (argv);

  if (! max)
    return;

  /* how many columns? */
  cols = 80 / ((max + 8) &~ 7);
  if (cols == 0)
    cols = 1;
  rows = (argc + cols - 1) / cols;

  for (i = 0; i < rows; i ++)
    {
      for (j = 0; j < cols; j ++)
	if ((i + j * rows) < argc)
	  printf ("%-*.*s", max + 1, max + 1, argv [i + j * rows]);
	else
	  break;
      printf ("\n");
    }
}

