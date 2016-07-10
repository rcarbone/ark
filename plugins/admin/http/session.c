/*
 * session.c - Handle a table of open sessions
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


/* Forward definition */
static void expired_callback (int unused, const short event, void * session);


/* Allocate and initialize a new session */
static session_t * mksession (char * sessionId, char * user)
{
  /* Buy memory now */
  session_t * s;
  if (! (s = calloc (sizeof (session_t), 1)))
    return NULL;

  if (sessionId)
    s -> sessionId = strdup (sessionId);

  if (user)
    s -> user = strdup (user);

  gettimeofday (& s -> start, NULL);

  s -> expired . tv_sec  = run -> expired_time;
  s -> expired . tv_usec = 0;

  /* Set an idle timeout for this session */
  evtimer_set (& s -> expired_timer, expired_callback, s);

  if (s -> expired_timer . ev_flags && EVLIST_INIT)
    evtimer_add (& s -> expired_timer, & s -> expired);

  return s;
}


/* Free allocated memory and resources used */
static void rmsession (session_t * s)
{
  if (s -> sessionId)
    free (s -> sessionId);

  if (s -> user)
    free (s -> user);

  if (s)
    free (s);
}


/* Initialize the session table */
static void init_sessiontable (struct hash_table * sessions, unsigned int size)
{
  memset (sessions, 0, sizeof (sessions));
  sessions -> size = size;

  hash_table_init (sessions);
}


/* Add a new session to the table */
static int insert_session (struct hash_table * sessions, char * sessionId, session_t * s)
{
  struct hdatum pair;

  /* key: sessionId */
  pair . key   = sessionId;
  pair . ksize = strlen (sessionId);

  /* value: session */
  pair . val   = s;
  pair . vsize = sizeof (session_t);

  /* Insert (key,val) */
  if (! hash_table_insert (sessions, & pair))
    return 0;

  /* Update the sessionIds table */
  run -> sessionIds = argsadd (run -> sessionIds, sessionId);

  return 1;
}


/* Lookup for a session by its identifier */
static session_t * lookup_session (struct hash_table * sessions, char * sessionId)
{
  struct hdatum pair;
  struct hdatum * found;

  if (! sessionId)
    return NULL;

  memset (& pair, 0, sizeof (struct hdatum));

  pair . key   = sessionId;
  pair . ksize = strlen (sessionId);

  found = hash_table_search (sessions, & pair);

  if (! found)
    return NULL;

  return ((session_t *) found -> val);
}


/* Remove a session from the table */
static void remove_session (struct hash_table * sessions, char * sessionId)
{
  struct hdatum pair;
  session_t * s;

  memset (& pair, 0, sizeof (struct hdatum));

  pair . key   = sessionId;
  pair . ksize = strlen (sessionId);

  s = lookup_session (sessions, sessionId);
  if (s)
    {
      rmsession (s);
      run -> sessionIds = argsrm (run -> sessionIds, sessionId);
    }

  hash_table_delete (sessions, & pair);
}


/* FIXME: checks for memory leaks () */
static void free_sessiontable (struct hash_table * sessions)
{
  /* TODO: before to call hash_table_free() I have to remove all the session with the rmsessions() */

  session_t * s;
  char ** ids = run -> sessionIds;
  while (ids && * ids)
    {
      s = lookup_session (sessions, * ids);
      if (s)
	{
	  /* Remove the timer */
	  evtimer_del (& s -> expired_timer);

	  rmsession (s);
	}
      ids ++;
    }
  hash_table_free (sessions);
}
