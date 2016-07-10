/*
 * icon2h.c - Convert an icon (.png .jpg .gif) to hexadecimal data
 *            (html icons without files for ark-aware plugins)
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
#include <unistd.h>
#include <string.h>
#include <ctype.h>


static void banner (char * filename)
{
  printf (
	  "/*\n"
	  " * %s.h - html icon without file for ark-aware plugins\n"
	  " *\n"
	  " * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-\n"
	  " *                                 _\n"
	  " *                       __ _ _ __| | __\n"
	  " *                      / _` | '__| |/ /\n"
	  " *                     | (_| | |  |   <\n"
	  " *                      \\__,_|_|  |_|\\_\\\n"
          " *\n"
          " * 'ark', is the home for asynchronous libevent-based plugins\n"
          " *\n"
          " *\n"
          " *                  (C) Copyright 2007-2008\n"
          " *         Rocco Carbone <rocco /at/ tecsiel /dot/ it>\n"
          " *\n"
          " * Released under the terms of GNU General Public License\n"
          " * at version 3;  see included COPYING file for details\n"
          " *\n"
          " * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-\n"
          " *\n"
          " */\n\n\n", filename);
}


int main (int argc, char * argv [])
{
  int option;
  int i;
  unsigned char ch;
  char * filename = "-";
  char * name = NULL;
  FILE * in = stdin;
  char * prefix;
  char * p;

  /* Notice the program name */
  char * progname = strrchr (argv [0], '/');
  progname = ! progname ? * argv : progname + 1;

#define OPTSTRING "f:n:"
  while ((option = getopt (argc, argv, OPTSTRING)) != EOF)
    {
      switch (option)
        {
        case 'f': filename = optarg; break;
        case 'n': name = optarg; break;
	}
    }

  if (strcmp (filename, "-") && ! (in = fopen (filename, "r")))
    {
      printf ("Error: cannot open %s\n", filename);
      return 0;
    }


  prefix = strdup (filename);
  if ((p = strchr (prefix, '.')))
    * p = '\0';
  banner (prefix);

  p = prefix;
  while (* p)
    * p = toupper (* p),
      p ++;

  printf ("#if !defined(_%s_H_)\n", prefix);
  printf ("# define _%s_H_\n", prefix);
  printf ("\n");

  p = prefix;
  while (* p)
    * p = tolower (* p),
      p ++;

  printf ("/* Converted by %s from %s */\n", progname, ! strcmp (filename, "-") ? "<stdin>" : filename);
  printf ("static unsigned char %s [] =\n", name ? name : prefix);
  printf ("{\n");

  i = 0;
  while (fread (& ch, 1, 1, in))
    {
      if (! i)
	printf ("  ");

      printf ("0x%02x,", ch);

      i ++;
      if (i > 16)
	i = 0,
	  printf ("\n");
      else
	printf (" ");
    }

 printf ("\n};\n\n");

  p = prefix;
  while (* p)
    * p = toupper (* p),
      p ++;

 printf ("#endif /* _%s_H_ */\n\n", prefix);

 free (prefix);

 return 0;
}
