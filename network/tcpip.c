/*
 * tcpip.c - How clients/servers access the TCP/IP stack
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


/* Operating System header file(s) */
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


static int setsockopt_aggressive (const int fd)
{
  int keep = 1;
  struct linger li;
  int ret = 0;

#ifdef SO_KEEPALIVE
  ret |= setsockopt (fd, SOL_SOCKET, SO_KEEPALIVE, (char *) & keep, sizeof keep);
#endif

#ifdef SO_LINGER
  li . l_onoff = li . l_linger = 0;
  ret |= setsockopt (fd, SOL_SOCKET, SO_LINGER, (char *) & li, sizeof li);
#endif

  return ret;
}


static void socket_nonblock (const int fd)
{
  fcntl (fd, F_SETFL, fcntl (fd, F_GETFL) | O_NONBLOCK);
}


/*
 * Create an endpoint to listen for incoming TCP connections on given address and port
 *
 * Return values:
 *  -1 [host not found]
 *  -2 [socket error]
 *  -3 [bind error]
 */
int incoming (char * address, int port, int backlog)
{
  /* Local variables */
  int listenfd;
  int reuse = 1;
  struct sockaddr_in tcpaddr;
  struct hostent * he;

  /* Initialize address */
  memset (& tcpaddr, '\0', sizeof (tcpaddr));
  tcpaddr . sin_family = AF_INET;
  tcpaddr . sin_port   = htons (port);

  /* Get network host entry for the given name */
  if (address && strcasecmp (address, "any") && strcmp (address, "*") && strcmp (address, "0.0.0.0"))
    {
      he = gethostbyname (address);
      if (! he)
	return -1;                    /* host not found */
      memcpy (& tcpaddr . sin_addr, he -> h_addr, he -> h_length);
    }
  else
    tcpaddr . sin_addr . s_addr = htonl (INADDR_ANY);

  /* Create a socket for the management of all the incoming calls */
  if ((listenfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    return -2;

  /* Set option in order to reuse address if busy */
  setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const char *) & reuse, sizeof (reuse));

  /* Set non blocking mode of operation */
  fcntl (listenfd, F_SETFL, O_NONBLOCK);

  /* Bind to local address */
  if (bind (listenfd, (struct sockaddr *) & tcpaddr, sizeof (tcpaddr)) == -1)
    {
      close (listenfd);
      return -3;
    }

  /* Set queue limit for incoming calls. The maximum length of the queue
   * for pending connections is set to 256 */
  listen (listenfd, backlog);

  return listenfd;
}


/*
 * Accept an incoming TCP connection on the given endpoint
 * Return values:
 *  -1 [accept error]
 *  -2 [getpeername error]
 */
int welcome (int fd, char ** remote, int * rport)
{
  /* Local variables */
  int fdnew;
  int retry = 10;
  struct sockaddr_in peeraddr;
  socklen_t peerlen = sizeof (peeraddr);
  struct hostent * rhost = NULL;

  /* Try to accept the connection */
  do
    {
      errno = 0;
      fdnew = accept (fd, (struct sockaddr *) & peeraddr, & peerlen);
    } while (fdnew == -1 && errno == EINTR && retry --);

  if (fdnew == -1)
    return -1;

  /* Get the IP address of the calling application and retrieve its name */
  memset ((char *) & peeraddr, '\0', sizeof (peeraddr));
  if (getpeername (fdnew, (struct sockaddr *) & peeraddr, & peerlen))
    {
      close (fd);
      return -2;
    }

  /* Then lookup for a name.
   * I do not a forward lookup because I do not care of possible spoofing */
  rhost = gethostbyaddr ((char *) & peeraddr . sin_addr, sizeof (peeraddr . sin_addr), AF_INET);

  * rport = ntohs (peeraddr . sin_port);
  * remote = rhost ? strdup (rhost -> h_name) : strdup (inet_ntoa (peeraddr . sin_addr));

  setsockopt_aggressive (fdnew);
  socket_nonblock (fdnew);

  return fdnew;
}


/*
 * Create an endpoint for outgoing TCP connections
 *
 * Return values:
 *  -1 [host not found]
 *  -2 [socket error]
 *  -3 [connect error]
 */
int outgoing (char * local, int lport, char * remote, int rport)
{
  /* Local variables */
  int fd;
  struct sockaddr_in server;
  struct hostent * host;

  memset ((char *) & server, '\0', sizeof (struct sockaddr_in));
  server . sin_family = AF_INET;
  server . sin_port   = htons (rport);

  host = gethostbyname (remote);
  if (host)
    memcpy (& server . sin_addr, host -> h_addr_list [0], host -> h_length);
  else
    server . sin_addr . s_addr = inet_addr (remote);
  if (server . sin_addr . s_addr == -1)
    return -1;

  fd = socket (AF_INET, SOCK_STREAM, 0);
  if (fd == -1)
    return -2;

  /* non-blocking mode (asynchronous) */
  fcntl (fd, F_SETFL, O_NONBLOCK);

  /* Initiate a TCP/IP connection on a socket */
  if (connect (fd, (struct sockaddr *) & server, sizeof (struct sockaddr_in)) == -1)
    {
      if (errno != EINPROGRESS && errno != EWOULDBLOCK && errno != ECONNREFUSED)
	{
	  close (fd);
	  return -3;
	}
    }

  return fd;
}
