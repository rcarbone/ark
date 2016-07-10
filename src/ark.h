/*
 * ark.c - Definition for ark's plugins
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
 *            (C) Copyright 2007-2008
 *   Rocco Carbone <rocco /at/ tecsiel /dot/ it>
 *
 * Released under the terms of GNU General Public License
 * at version 3;  see included COPYING file for details
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
 *
 */


#if !defined(_ARK_H_)
# define _ARK_H_

#define PKG_ID            "ark"
#define PKG_VERSION       "0.1.0"
#define PKG_AUTHOR        "Rocco Carbone rocco@tecsiel.it"
#define PKG_RELEASE_DATE  __DATE__
#define PKG_COPYRIGHT    "(C) Copyright 2007-2008"
#define PKG_RIGHTS       "All Rights Reserved"
#define PKG_LICENSE      "GNU General Public License V3"


/* Operating System header file(s) */
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

/* The primary libevent header */
#include <event.h>

/* Reserved keyword for the internal library */
#define INTERNAL_LIB      "libark.so"

/* Symbol names */
#define VARIABLE 1
#define FUNCTION 2

/* Reserved keywords to initialize and terminate the plugin */
#define START    "boot"
#define STOP     "halt"

/* Reserved keywords with mandatory names that __must__ be defined in each plugin */
#define VARS     "variables"
#define FUNCS    "functions"

/* Reserved keyword to access the plugin's home page */
#define HOMEPAGE "web"

/* Reserved keywords to push and pull data to the other sites */
#define PUSH     "push"
#define PULL     "pull"


/* Useful timing macros */
#define NOW               ((time_t) time ((time_t *) 0))
#define SECS_PER_DAY      86400
#define SECS_PER_HOUR     3600
#define SECS_PER_MIN      60


/* Known protocols */
#define PROTOCOL_UCP      1
#define PROTOCOL_SMPP     2
#define PROTOCOL_SMTP     3
#define PROTOCOL_HTTP     4
#define PROTOCOL_FTP      5
#define PROTOCOL_PRIVATE  8


/* Reasons kpush() and kpull() could fail */
#define NACK_NO_RESOURCES  65
#define NACK_NETWORK_FLOW  66
#define NACK_FLOWCONTROL   67


/* definitions for various types of counters */
typedef uint64_t counter_t;



/* The prototype for the a generic function that could be implemented by the plugin */
typedef int call_t (int argc, char * argv []);

typedef struct
{
  char * name;             /* User printable variable/function name */
  void * symbol;           /* Its symbol as obtained by dlsym()     */
  char   type;             /* Variable or function?                 */
} symbol_t;


/* Keeps all information about the plugin implemented in a shared object */
typedef struct plugin_t
{
  struct timeval uptime;   /* Time it was loaded                      */

  void      * handle;      /* Handle returned by dlopen()             */
  char      * path;        /* Where the shared object is located      */

  char     ** declared;    /* Table of plugin declarations            */
  char     ** defs;        /* Table of plugin definitions             */

  symbol_t ** symbols;     /* Table of variables/funtions implemented */

  enum
    { UNKNOWN, DUMMY, LOADED, STARTED }
    state;                 /* State of the plugin                     */

} plugin_t;


/* -=-=-=-=-=-=-= API -=-=-=-=-=-=-= */

#define PLUGIN_OK    1
#define PLUGIN_FAIL  0


void * defined (char * name, symbol_t * argv []);
char * variable (char * name, symbol_t * argv []);
call_t * function (char * name, symbol_t * argv []);

char * pluginname (plugin_t * p);
plugin_t * pluginbyname (char * name);
int pluginstate (char * name);

/* Return the number of currently loaded plugins */
int pluginsno (void);

/* Return the plugins currently loaded in form of a NULL terminated array */
plugin_t ** running (void);

/* Fill the table of all available plugins (filename with the extension "*.so") from the directory pointed to by [dir] */
char ** available_in_dir (char * dir);

/* Fill the table of all available plugins (filename with the extension "*.so" except INTERNAL_LIB)
 * from the directory pointed to by [dir] */
char ** available_plugins (char * dirs []);

/* Try to load the plugin at [path] and add the plugin to the table of loaded plugins */
plugin_t ** load_plugin (char * path, int argc, char * argv [], int verbose);

/* Load a plugin in memory, do not start it but check instead it supports a web interface.
 * In such event show the plugin's homepage and return 1, 0 otherwise */
plugin_t * homepage (char * path, int argc, char * argv []);

/* Unload the plugin with given name */
plugin_t ** unload_plugin (char * name);

/* Try to load plugins in [plugins] (filenames with "*.so" extension except INTERNAL_LIB)
 * looking in the directories pointed to by [dirs] */
void load_plugins (char * plugins [], char * dirs [], char * conffile);

/* Try to load all plugins (filenames with "*.so" extension) from the directories pointed to by [dirs] */
void load_all_plugins (char * dirs [], char * conffile);

/* Unload all currently loaded plugins */
void unload_all_plugins (void);

/* The function defined in order to push data to the other site (Push Service) */
int kpush (int protocol, int argc, char * argv []);

/* The function defined in order to pull data to the other site (Pull Service) */
int kpull (int protocol, int argc, char * argv []);


/* Public variables defined in file globals.c  */
extern char * progname;
extern plugin_t ** loaded;
extern char ** plgpaths;
extern struct evhttp * arkhttpd;


/* Public functions defined in file globals.c */
void started (void);
time_t since (void);
char ** shobjs (void);


/* Public functions defined in file args.c */
int argslen (char * argv []);
char ** argsadd (char * argv [], char * s);
char ** argsrm (char * argv [], char * item);
int member (char * argv [], char * item);
char ** argsrm (char * argv [], char * item);
void argsreplace (char * argv [], char * s, char * d);
void argsfree (char * argv []);
char ** argsdup (char * argv []);
char ** argscat (char * a [], char * b []);
void argsline (char * argv [], char c);
void argsrows (char * argv []);
int argsmemberof (char * name, char * list);
char ** argspieces (char * list, char * separator);
char ** argsblanks (char * list);
int argslongest (char * argv []);
char ** argssort (char * argv []);
char * argsjoin (char * argv []);
char * nextrr (char * argv []);
void args_2d_rows (int argc, char * argv []);
void args_2d_cols (int argc, char * argv []);


/* Public functions defined in file time.c */
time_t seconds (struct timeval * t);
time_t milliseconds (struct timeval * t);
time_t microseconds (struct timeval * t);
int days (time_t t1, time_t t2);
int hours (time_t t1, time_t t2);
int mins (time_t t1, time_t t2);


/* Public functions defined in file tcpip.c */
int incoming (char * address, int port, int backlog);
int welcome (int fd, char ** remote, int * port);
int outgoing (char * local, int lport, char * remote, int rport);


/* Public functions defined in file fmemdmp.c */
void fmemdmp (FILE * fd, char * ptr, int size, char * text);

#endif /* _ARK_H_ */
