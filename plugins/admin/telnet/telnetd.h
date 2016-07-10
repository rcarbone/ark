/*
 * telnetd.h - Definition for ark's plugin responsible to remotely control other plugins using the Telnet protocol
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

#define DEFAULT_CONF_FILE     NULL
#define DEFAULT_LOG_FILE      "*"  /* "-" means standard output while "*" means none */
#define DEFAULT_ACCESS_FILE   "-"  /* "-" means standard output while "*" means none */

/* Default local address for listening when talking the front-end protocol */
#define DEFAULT_ADDRESS       "*:5960"  /* "*" means bind on all available interfaces */

/* Socket listen queue size */
#define DEFAULT_BACKLOG       256

/* Max # of concurrent connections per each server - 0 means unlimited */
#define DEFAULT_MAXCONNS      0

/* Protocol timeouts in milliseconds - 0 means unlimited */
#define DEFAULT_LOGIN_TIMEOUT 60000
#define DEFAULT_IDLE_TIMEOUT  180000


#define PROMPT   "admin@rocco> "
#define MAXLINE  2048  /* max size of a reply line */
#define BANNER   "Welcome on the ark!"
#define LOGOUT   "Thanks for having used this service!"


/* How to drive the behaviour of the plugin via command line options */
typedef struct
{
  /* Files */
  char * conffile;           /* Master configuration file (if any)      */
  char * logfile;            /* Log file (if any)                       */
  char * accessfile;         /* Access file (if any)                    */

  /* Addressing */
  char ** addrs;            /* Table of '[address:]port' to listen to  */

  /* Global parameters per server */
  int backlog;              /* Max length of pending connections queue */
  int maxconns;             /* Max number of concurrent connections    */

  /* Authentication */
  char * user;              /* Numeric identifier to authenticate      */
  char * password;          /* Password used with authentication       */

  /* Protocol timeouts in milliseconds */
  int login;                /* Login timeout                           */
  int idle;                 /* Idle timeout                            */

  int showrecv;             /* Boolean to show packets received        */
  int showsent;             /* Boolean to show packets sent            */

} opttime_t;


/* How to configure the plugin via a master configuration file */
typedef struct
{
  /* Files */
  char * logfile;           /* Log file (if any)                       */
  char * accessfile;        /* Access file (if any)                    */

  /* Addressing */
  char ** addrs;            /* Table of '[address:]port' to listen to  */

  /* Global parameters per server */
  int backlog;              /* Max length of pending connections queue */
  int maxconns;             /* Max number of concurrent connections    */

  /* Authentication */
  char * user;              /* Numeric identifier to authenticate      */
  char * password;          /* Password used with authentication       */

  /* Protocol timeouts in milliseconds */
  int login;                /* Login timeout                           */
  int idle;                 /* Idle timeout                            */

  int showrecv;             /* Boolean to show packets received        */
  int showsent;             /* Boolean to show packets sent            */

} cfgtime_t;


/* Run-time parameters */
typedef struct
{
  struct timeval boottime;  /* The time it was started                 */
  char * name;              /* The name of the plugin                  */

  /* Files */
  char * conffile;          /* Master configuration file (if any)      */
  char * logfile;           /* Log file (if any)                       */
  char * accessfile;        /* Access file (if any)                    */

  /* Addressing */
  char ** addrs;            /* Table of '[address:]port' to listen to  */

  /* Global parameters per server */
  unsigned backlog;         /* Max length of pending connections queue */
  unsigned maxconns;        /* Max number of concurrent connections    */

  /* Authentication */
  char * user;              /* Numeric identifier to authenticate      */
  char * password;          /* Password used with authentication       */

  /* Protocol timeouts in milliseconds */
  unsigned login;           /* Login timeout                           */
  unsigned idle;            /* Idle timeout                            */

  FILE * logfp;             /* Log file pointer                        */
  FILE * accessfp;          /* Access file pointer                     */

  server_t ** servers;      /* Table of currently active servers       */

  unsigned showrecv;        /* Boolean to show packets received        */
  unsigned showsent;        /* Boolean to show packets sent            */

  char * prompt;            /* prompt to the user                      */

} runtime_t;

