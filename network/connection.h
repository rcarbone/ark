/*
 * connection.h - definitions to handle network connections with remote hosts
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


#if !defined(CONNECTION_H)
# define CONNECTION_H

/* Operating System header file(s) */
#include <sys/time.h>

/* The primary libevent header */
#include <event.h>

/* Private header file(s) */
#include "request.h"


/* Reasons for local/remote initiated disconnections */
#define MEMORY       1     /* Memory fault                 */
#define OSFAULT      2     /* Operating System fault       */
#define TIMEOUT      3     /* Idle timeout has expired     */
#define DECODING     4     /* Corrupted message received   */
#define PROTOCOL     5     /* Protocol mismatch            */
#define AUTH_FAILED  6     /* Protocol mismatch            */
#define HANGUP       7     /* Remote has closed connection */


typedef enum
{
  IDLE = 0, FAILED, CONNECTING, RETRY, NEEDLOGIN, LOGGEDIN, AUTHENTICATING, DATAXFER, DENIED, BLOCKED, DONE
} state_t;


/* A descriptor for a connection with a remote host */
typedef struct
{
  struct timeval uptime;         /* Time the connection has been established                   */

  void * host;                   /* Reference to the host owner of this descriptor             */

  state_t state;                 /* State of the connection                                    */

  /* Identifiers */
  int fd;                        /* File descriptor used to manage the connection              */
  unsigned port;                 /* Local port                                                 */

  unsigned nexttrn;              /* Next TRN to use over the connection                        */

  /* Variables needed to asynchronous send/receive data over the connection */
  struct event write_evt;        /* Detect write events on not yet established connections     */
  struct event read_evt;         /* Detect read events on already established connections      */

  /* Protocol timeouts in microseconds */
  struct event connect_timer;    /* Variable used to connect after a given timeout             */
  struct event idle_timer;       /* Variable used to disconnect after an inactivity timeout    */
  struct event reconnect_timer;  /* Variable used to reconnect at given time interval          */
  struct event push_timer;       /* Variable used to push after a given timeout                */

  void * room;                   /* Memory area used to dynamic enqueue incoming messages      */
  unsigned roomsize;             /* Its size                                                   */
  unsigned hasread;              /* Already read bytes                                         */

  /* Requests */
  req_t ** pending;              /* The table of pending requests                              */
  unsigned maxresend;            /* How many times a request should be resent                  */

  /* Connection counters */
  counter_t attempted;           /* Total # of attempts to connect to the remote host          */
  counter_t established;         /* Total # of connections established                         */

  /* Disconnection counters */
  counter_t memory;              /* Total # of disconnections due to memory faults             */
  counter_t osfault;             /* Total # of local disconnections due to system faults       */
  counter_t timeout;             /* Total # of disconnections due to idle timeouts             */
  counter_t decoding;            /* Total # of disconnections due to decoding errors           */
  counter_t protocol;            /* Total # of disconnections due to protocol mismatch         */
  counter_t denied;              /* Total # of disconnections due to authentication failures   */
  counter_t hangup;              /* Total # of disconnections due to peer shutdown             */

  /* Bytes counters */
  counter_t sentbytes;           /* # of bytes sent over the connection                        */
  counter_t recvbytes;           /* # of bytes received over the connection                    */

  /* Operation counters (both outgoing and incoming) */
  counter_t outgoing_op;         /* Total # of operations sent over the connection             */
  counter_t incoming_op;         /* Total # of operations received over the connection         */
  counter_t supported_op;        /* incoming only                                              */
  counter_t unsupported_op;      /* incoming only                                              */
  counter_t forwarded_op;        /* incoming only                                              */
  counter_t refused_op;          /* incoming only                                              */

  /* Result counters (both outgoing and incoming) */
  counter_t outgoing_rs;
  counter_t incoming_rs;
  counter_t supported_rs;        /* incoming only                                              */
  counter_t unsupported_rs;      /* incoming only                                              */
  counter_t forwarded_rs;        /* incoming only                                              */
  counter_t refused_rs;          /* incoming only                                              */

  /* Message counters */
  counter_t received;            /* # of messages received over the connection                 */
  counter_t unexpected;          /* # of unexpected messages received over the connection      */
  counter_t rights;              /* # of legal messages received over the connection           */
  counter_t errors;              /* # of decoding errors detected over the connection          */
  counter_t illegal;             /* # of protocol mismatch received over the connection        */

  /* Operations counters (supported + unsupported = rights) */
  counter_t expired;             /* # of operations expired                                    */
  counter_t overruns;            /* # of messages re-sent over the connection                  */

  /* Flow control counters */
  counter_t flowwin;             /* # total of flow control due to window size limits          */
  counter_t flowtrn;             /* # total of flow control due to protocol limits             */
  counter_t flownet;             /* # total of flow control due to network congestion          */

  /* Result counters */
  counter_t goods;
  counter_t acks;
  counter_t nacks;
  counter_t supported;
  counter_t unsupported;

  /* Timers for events */
  struct timeval tried;          /* Time latest connection was tried                           */
  struct timeval connected;      /* Time latest connection was established                     */
  struct timeval disconnected;   /* Time latest connection has been locally dropped            */
  struct timeval down;           /* Time latest connection has been shutted down               */

  struct timeval retried;        /* Time latest last reconnection was tried again              */
  struct timeval firstsent;      /* Time first data were sent over this connection             */
  struct timeval lastsent;       /* Time last sent data over the connection                    */
  struct timeval lastrecv;       /* Time last received data over the connection                */

  /* Counters for statistics */
  double min;                    /* Shortest response time                                     */
  double max;                    /* Longest response time                                      */
  double avg;                    /* Average response time                                      */

} conn_t;


/* Public functions in file connection.c */
int connlen (conn_t * argv []);
void zeroconn (conn_t * c);
conn_t ** moreconn (conn_t * argv [], void * smsc, unsigned port, unsigned maxretries);
void nomoreconn (conn_t * argv []);
conn_t ** lessconn (conn_t * argv [], conn_t * c);
int connlookup (conn_t * argv [], conn_t * c);
int connchuck (conn_t * conn);
void connresize (conn_t * c, int msglen);
void nextstate (conn_t * conn, int state);

/* Public functions in file tcpip.c */
int outgoing (char * local, int lport, char * remote, int rport);

/* Public functions in file time.c */
time_t delta_time_in_seconds (struct timeval * t2, struct timeval * t1);
time_t delta_time_in_milliseconds (struct timeval * t2, struct timeval * t1);
time_t delta_time_in_microseconds (struct timeval * t2, struct timeval * t1);


#endif /* CONNECTION_H */
