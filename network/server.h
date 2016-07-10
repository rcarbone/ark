/*
 * server.h - Definition for event-driven network servers
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


#if !defined(SERVER_H)
# define SERVER_H

/* The primary libevent header */
#include <event.h>

/* Private header file(s) */
#include "client.h"


/* Constants to parse addresses */
#define ANYADDRESS        "*"
#define ANYINTERFACE      "0.0.0.0"

/* Default local address for listening when talking the front-end protocol */
#define DEFAULT_INTERFACE  ANYINTERFACE
#define DEFAULT_PORT       1616

/* Reason for local/remote initiated disconnections */
#define MEMORY    1     /* Memory fault                 */
#define OSFAULT   2     /* Operating System fault       */
#define TIMEOUT   3     /* Idle timeout has expired     */
#define DECODING  4     /* Corrupted message received   */
#define PROTOCOL  5     /* Protocol mismatch            */
#define L_HANGUP  6     /* Remote has closed connection */


/* No traffic enabled by default */
#define DEFAULT_SHOWRECV          0
#define DEFAULT_SHOWSENT          0


/* What a descriptor of a generic server should keep */
typedef struct
{
  /* Identifiers */
  struct timeval uptime;     /* Time the server was made active                          */

  char * address;            /* Local address to listen for incoming connections         */
  int port;                  /* Local port to listen for incoming connections            */
  int fd;                    /* File descriptor used to listen from the network          */
  struct event listen_evt;   /* How to detect events on the listening file descriptor    */

  unsigned backlog;          /* Max length of pending connections queue                  */
  unsigned maxconns;         /* Max # of concurrent connections                          */
  client_t ** clients;       /* Table of remote clients currently connected              */

  char * id;                 /* Numeric identifier to authenticate                       */
  char * password;           /* Password used with authentication (optional)             */
  char * type;               /* System type to authenticate (optional)                   */

  /* Protocol timeouts */
  struct timeval login;      /* Login timeout                                            */
  struct timeval idle;       /* Idle timeout                                             */
  struct timeval ack;        /* Ack timeout                                              */

  /* Connection counters (incoming = accepted + rejected) */
  counter_t accepted;        /* Total # of connections accepted                          */
  counter_t rejected;        /* Total # of connections rejected                          */

  /* Disconnection counters */
  counter_t memory;          /* Total # of local disconnections due to memory faults     */
  counter_t osfault;         /* Total # of local disconnections due to system faults     */
  counter_t timeout;         /* Total # of local disconnections due to idle timeouts     */
  counter_t decoding;        /* Total # of local disconnections due to decoding errors   */
  counter_t protocol;        /* Total # of local disconnections due to protocol mismatch */
  counter_t denied;          /* Total # of local disconnections due to login failures    */
  counter_t hangup;          /* Total # of remote disconnections due to peer shutdown    */

  /* Bytes counters */
  counter_t recvbytes;       /* Total # of bytes received over all the connections       */
  counter_t sentbytes;       /* Total # of bytes sent over all the connections           */

  /* Global counters for the Push Service */
  counter_t recvop;          /* Total # of operations received                           */
  counter_t sentacks;        /* Total # of positive results sent (acks)                  */
  counter_t sentnacks;       /* Total # of negative results sent (nacks)                 */

  /* Global counters for the Pull Service */
  counter_t sentop;          /* Total # of operations sent                               */
  counter_t recvacks;        /* Total # of positive acks received                        */
  counter_t recvnacks;       /* Total # of negative acks received                        */

  /* FIXME!!! */
  counter_t supported_op;    /* Total # of supported operations received                 */
  counter_t unsupported_op;  /* Total # of unsupported operations received               */
  counter_t forwarded_op;    /* incoming only                                            */
  counter_t refused_op;      /* incoming only                                            */

  counter_t supported_rs;    /* incoming only                                            */
  counter_t unsupported_rs;  /* incoming only                                            */
  counter_t forwarded_rs;    /* incoming only                                            */
  counter_t refused_rs;      /* incoming only                                            */

  /* Operations counters */
  counter_t received;        /* Total # of messages received                             */
  counter_t unexpected;      /* Total # of unexpected messages received                  */
  counter_t rights;          /* Total # of legal messages received                       */
  counter_t errors;          /* Total # of decoding errors detected                      */
  counter_t illegal;         /* Total # of protocol mismatch received                    */

  /* Operations counters (supported + unsupported = rights) */
  counter_t expired;         /* Total # of operations expired                            */
  counter_t forwarded;       /* Total # of operations forwarded                          */
  counter_t refused;         /* Total # of operations refused                            */

} server_t;


/* Public functions in file server.c */
int serverlen (server_t * argv []);
server_t * serverbyaddress (char * address, int port, server_t * argv []);
server_t ** moreservers (server_t * argv [], char * address, unsigned backlog, unsigned maxconns,
			     char * id, char * password, char * type, unsigned login, unsigned idle, unsigned ack);
void nomoreservers (server_t * argv []);
counter_t servercalls (server_t * l);
counter_t serverdropped (server_t * l);
counter_t serverremote (server_t * l);
counter_t serverlocal (server_t * l);
counter_t serversentrs (server_t * l);
counter_t serverrecvrs (server_t * l);
counter_t serverop (server_t * l);
counter_t serverrs (server_t * l);
counter_t serversenttotop (server_t * argv []);
counter_t serverrecvtotop (server_t * argv []);
int servercsps (server_t * argv []);
int serverestablished (server_t * argv []);

/* Public functions in file tcpip.c */
int incoming (char * address, int port, int backlog);

#endif /* SERVER_H */
