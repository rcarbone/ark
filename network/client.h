/*
 * client.h - Definition for event-driven network clients
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


#if !defined(CLIENT_H)
# define CLIENT_H

#include "connection.h"


/* Window size - some protocols have restrictions on the maximum value */
#define DEFAULT_WINDOW_SIZE       1


/* A descriptor for a connection with a client requiring service */
typedef struct
{
  struct timeval uptime;         /* Time the connection has been established                   */

  void * server;                 /* Reference to the server owner of the descriptor            */

  unsigned blocked;              /* True is the connection is blocked due to flow control      */
  state_t state;                 /* The state of the connection                                */

  /* Identifiers */
  int fd;                        /* File descriptor used to manage the connection              */
  char * hostname;               /* Address of the remote application                          */
  unsigned port;                 /* And its port number                                        */

  unsigned window;               /* Window size for outgoing flow                              */
  unsigned nexttrn;              /* Next TRN to use over the connection                        */

  /* Variables needed to asynchronous send/receive data over the connection */
  struct event write_evt;        /* Detect write events on not yet established connections     */
  struct event read_evt;         /* Detect read events on already established connections      */

  void * room;                   /* Memory area used to dynamic enqueue incoming messages      */
  unsigned roomsize;             /* Its size                                                   */
  unsigned hasread;              /* Already read bytes                                         */

  /* Protocol timeouts in microseconds */
  struct timeval login;          /* Max login timeout                                          */
  struct timeval idle;           /* Max idle timeout                                           */
  struct timeval ack;            /* Max ack timeout (used in the event message is forwarded)   */
  struct event idle_timer;       /* Variable used to disconnect after an inactivity timeout    */
  struct event push_timer;       /* Variable used to push after a given timeout                */

  /* Requests */
  req_t ** pending;              /* The table of pending requests                              */
  unsigned maxresend;            /* How many times a request should be resent                  */

  /* Bytes counters */
  counter_t sentbytes;           /* # of bytes sent over the connection                        */
  counter_t recvbytes;           /* # of bytes received over the connection                    */

  /* Operation counters (both outgoing and incoming) */
  counter_t outgoing_op;
  counter_t incoming_op;
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

  /* Results counters */
  counter_t acks;                /* # of positive results sent over the connection             */
  counter_t nacks;               /* # of negative results sent over the connection             */

  void * ssl;                    /* Descriptor to secure the transaction over SSL              */

  unsigned showrecv;             /* Boolean to dump incoming flow of messages                  */
  unsigned showsent;             /* Boolean to dump outgoing flow of messages                  */

} client_t;


/* Public functions in file client.c */
int clientlen (client_t * argv []);
client_t * mkclient (void * server, int fd, char * hostname, int port, unsigned login, unsigned idle, unsigned ack,
		     unsigned showrecv, unsigned showsent, void * ssl);
void rmclient (client_t * c);
client_t ** moreclient (client_t * argv [], void * server, int fd, char * hostname, int port,
			unsigned login, unsigned idle, unsigned ack,
			unsigned showrecv, unsigned showsent, client_t ** more, void * ssl);
client_t ** lessclient (client_t * argv [], client_t * c);
void nomoreclient (client_t * argv []);
int clientlookup (client_t * argv [], client_t * c);
client_t * clientbyfd (client_t * argv [], int fd);
int clientchuck (client_t * conn);
void clientresize (client_t * c, int msglen);

/* Public functions in file time.c */
time_t milliseconds (struct timeval * t);

#endif /* CLIENT_H */
