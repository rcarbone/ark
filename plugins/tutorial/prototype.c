/*
 * prototype.c - a template used as a prototype for creating your own ark-aware plugin
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

const char * variables = { "name version author date what copyright license" };
const char * functions = { "boot halt" };


/* Plugin definitions */

/* The unique name of the plugin */
const char * name = "prototype";

/* A place for the version */
const char * version = "0.0.1";

/* Give credits to the author */
const char * author = "(C) Rocco Carbone rocco@tecsiel.it";

/* A place for the release date */
const char * date = "1Q 2007";

/* A place for a comment */
const char * what = "A dummy plugin to be used as a template!";

/* A place for legal notice */
const char * copyright = "(C) Copyright 2007-2008 Rocco Carbone";

/* A place for a license */
const char * license = "GNU General Public License V3";


/* Put here all your public functions */

/* Will be called once when the plugin is loaded */
int boot (int argc, char * argv [])
{
  return PLUGIN_OK;
}


/* Will be called once when the plugin is unloaded */
int halt (int argc, char * argv [])
{
  return PLUGIN_OK;
}

