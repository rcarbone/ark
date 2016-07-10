/*
 * request.h - How damn a request in a network enviroment should keep
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


#if !defined(_REQUEST_H_)
# define _REQUEST_H_

/* Operating System header file(s) */
#include <sys/time.h>

/* The primary libevent header */
#include <event.h>

/* Private header file(s) */
#include "ark.h"


/* Constant to get chuncks of memory while decoding incoming messages */
#define BUFCHUNK    1024

/* Length of the unique request identifier */
#define REQIDLEN      18

/*
 * Holds all relevant information about pending protocol messages sent/received
 *  - time the request was first created
 *  - reference to the connection over which the message was delivered
 *  - data received and passed with the request
 */
typedef struct
{
  struct timeval sent;         /* Time the request was sent                         */
  char * id;                   /* Unique identifier                                 */
  unsigned trn;                /* Unique transmission reference for the protocol    */

  void * conn;                 /* Reference to the connection owner of the message  */

  unsigned attempts;           /* # of attempts should be tried to resend - 0 never */

  /* Timers events */
  struct event ack_timer;      /* Variable used to define an ack timeout (if any)   */
  struct event resend_timer;   /* Variable used to define a resend timeout (if any) */

  /* Protocol data in terms of NULL terminated array (positional arguments)         */
  char ** data;
  char ** data_org;
} req_t;


/* Function and Pointer To Function returning void */
#if !defined(PTFRVNULL)
typedef void frv_t (req_t * req);
typedef frv_t * ptfrv_t;
# define PTFRVNULL (ptfrv_t) NULL
#endif /* ! PTFRVNULL */


/* The structure containing information on the messages the application can understand and handle */
typedef struct
{
  char  * ot;       /* operation type as passed in the protocol data */
  frv_t * handler;  /* function to call to implement the operation   */
} message_t;



/* Public functions in file request.c */
char * newreqid (void);
int reqlen (req_t * argv []);
req_t * mkreq (char * id, unsigned trn, void * conn);
void rmreq (req_t * r);
req_t ** morereq (req_t * argv [], req_t * req);
req_t ** lessreq (req_t * argv [], req_t * req);
void nomorereq (req_t * argv []);
req_t * reqbyid (req_t * argv [], char * id);

/* Public functions in file time.c */
time_t microseconds (struct timeval * t);

#endif /* _REQUEST_H_ */
