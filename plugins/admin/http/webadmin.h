/*
 * webadmin.h - Definition for 'webadmin' an ark-aware plugin, accesible via HTTP protocol, to remotely control other plugins
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


#if !defined(WEBADMIN_H)
# define WEBADMIN_H

/* Plugin name and logo */
#define PLGNAME          "webadmin"
#define MASCOTTE_H       "spider.h"

/* Callbacks to register for html rendering */
#define URL_MAIN_HOME    "/"
#define URL_MAIN_ABOUT   "/about"
#define URL_LOGIN        "/login"
#define URL_LOGOUT       "/logout"
#define URL_CHECK        "/check"
#define URL_ACCOUNTS     "/accounts"
#define URL_LOAD         "/load"
#define URL_UNLOAD       "/unload"

/* Clickable link names */
#define DO_LOGIN_BUTTON  "Do Login!"
#define DO_LOGOUT_BUTTON "Do Logout!"
#define LOGIN_BUTTON     "Login"
#define LOGOUT_BUTTON    "Logout"
#define LINK_ACCOUNTS    "Accounts"

#define KEY_USER        "user"
#define KEY_PWD         "pwd"


/* Default parameters for autoconfigure */
#define DEFAULT_CONF_FILE     NULL
#define DEFAULT_LOG_FILE      "*"  /* "-" means standard output while "*" means none */
#define DEFAULT_ACCESS_FILE   "-"  /* "-" means standard output while "*" means none */

/* Default local address for listening when talking the front-end protocol */
#define DEFAULT_ADDRESS       "*:5555"  /* "*" means bind on all available interfaces */

/* Table colors */
#define DEFAULT_HDCOLOR       "#F4A460"
#define DEFAULT_BGCOLORS      "#eeeeee #dddddd #cccccc"

/* Macro for web-config */
#define PARSELEN               7 /* # of chars in '/parse?' */
#define PARAMLEN               6 /* # of chars in 'pname='  */

#define OPT                    0
#define VAL                    1

/* Admin login */
#define DEFAULT_ENABLEADMIN    0 /* Default: no password required */

/* Login parameters */
#define MAX_LOGINUSER_LEN      20
#define MAX_LOGINPWD_LEN       10

/* Default admin user in the event no user was defined in the configuration file */
#define DEFAULT_ADMIN          "admin:admin"
#define WILDCARD               "speedygonzales"

#define SESSIONS_TABLE_LEN     100
#define DEFAULT_EXPIREDTIME    60   /* session expire timeout in seconds */

#define DESC_TABLE_LEN         40


/* How to drive the behaviour of the plugin via command line options */
typedef struct
{
  /* Files */
  char * conffile;          /* Master configuration file (if any)    */
  char * logfile;           /* Log file (if any)                     */
  char * accessfile;        /* Access file (if any)                  */

  char ** accounts;         /* Lista di 'utenti:pass'                */

  int expired_time;         /* Timeout per una sessione web          */

  /* Addressing */
  char ** addrs;            /* Table of '[address:]port' to listen to */

  /* Colors */
  char * hdcolor;           /* Color to render html header            */
  char ** bgcolors;         /* Table of colors to render html tables  */

} opttime_t;



/* How to configure the plugin via a master configuration file */
typedef struct
{
  /* Files */
  char * logfile;           /* Log file (if any)                      */
  char * accessfile;        /* Access file (if any)                   */

  int enable_admin;         /* Disabilita la richiesta di login      */
  char ** accounts;         /* Lista di 'utenti:pass'                */

  int expired_time;         /* Timeout per una sessione web          */

  /* Addressing */
  char ** addrs;            /* Table of '[address:]port' to listen to */

  /* Colors */
  char * hdcolor;           /* Color to render html header            */
  char ** bgcolors;         /* Table of colors to render html tables  */

} cfgtime_t;


/* Run-time parameters */
typedef struct
{
  struct timeval boottime;     /* The time it was started                */
  char * name;                 /* The name of the plugin                 */

  /* Files */
  char * conffile;             /* Master configuration file (if any)     */
  char * logfile;              /* Log file (if any)                      */
  char * accessfile;           /* Access file (if any)                   */

  int enable_admin;            /* Disabilita la richiesta di login      */
  char ** accounts;            /* Lista di 'utenti:pass' configurati    */

  char ** sessionIds;          /* Lista di Id di sessioni valide        */
  struct hash_table sessions;  /* Lista di sessioni valide              */
  int expired_time;            /* Timeout per una sessione web          */

  struct hash_table desc;      /* Breve descrizione associata al plugin */

  /* Addressing */
  char ** addrs;                /* Table of '[address:]port' to listen to */

  plugin_t ** pending;          /* Lista plugins caricati ma non loaded   */

  /* Colors */
  char * hdcolor;               /* Color to render html header            */
  char ** bgcolors;             /* Table of colors to render html tables  */

  FILE * logfp;                 /* Log file pointer                       */
  FILE * accessfp;              /* Access file pointer                    */

} runtime_t;


#endif /* WEBADMIN_H */
