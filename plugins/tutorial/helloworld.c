/*
 * helloworld.c - the simplest ark-aware plugin
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

/* Private header file(s) */
#include "ark.h"


/* Plugin declarations */

const char * variables = { "name version author what when where why copyright" };
const char * functions = { "boot halt" };

/* Plugin definitions */

const char * name      = "helloworld";
const char * version   = "0.0.1";
const char * author    = "Rocco Carbone rocco@tecsiel.it";
const char * what      = "The simplest ark-aware plugin";
const char * when      = "Sun Mar 25 08:34:11 CET 2007";
const char * where     = "Pisa, Italy";
const char * why       = "Just to show how to write an ark-aware plugin";
const char * copyright = "(C) Copyright 2007-2008 Rocco Carbone";


/* Public functions */

int boot (int argc, char * argv [])
{
  printf ("%s [%s]: Hello World! My name is %s\n", progname, name, name);
  return PLUGIN_OK;
}


int halt (int argc, char * argv [])
{
  printf ("%s [%s]: Bye bye cruel world!\n", progname, name);
  return PLUGIN_OK;
}
