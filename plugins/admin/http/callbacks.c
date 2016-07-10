/*
 * callbacks.c - Callbacks to handle libevent events
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


/* Handler for timed out sessions */
static void expired_callback (int unused, const short event, void * session)
{
  session_t * s = session;

  if (run -> enable_admin && lookup_session (& run -> sessions, s -> sessionId))
    remove_session (& run -> sessions, s -> sessionId);
}
