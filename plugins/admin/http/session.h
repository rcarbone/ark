/*
 * session.h - Definitions to handle HTTP sessions
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


#if !defined(_SESSION_H_)
# define _SESSION_H_

/* Web session */
typedef struct
{
  char * sessionId;

  char * user;

  struct timeval start;

  /* Validita' della sessione */
  struct timeval expired;
  struct event expired_timer;

} session_t;


static session_t * mksession (char * sessionId, char * user);
static void rmsession (session_t * s);

static void init_sessiontable (struct hash_table * sessions, unsigned int size);
static int insert_session (struct hash_table * sessions, char * sessionId, session_t * s);
static session_t * lookup_session (struct hash_table * sessions, char * sessionId);
static void remove_session (struct hash_table * sessions, char * sessionId);
static void free_sessiontable (struct hash_table * sessions);

#endif /* _SESSION_H_ */
