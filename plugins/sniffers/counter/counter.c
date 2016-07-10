/*
 * counter.c - A packet counter ark-aware sniffer plugin with Web rendering
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


/* Plugin declarations */

const char * variables = { "name version author date copyright" };
const char * functions = { "halt boot web" };


/* Plugin definitions */

const char * name      = "counter";
const char * version   = "0.0.1";
const char * author    = "Rocco Carbone rocco@tecsiel.it";
const char * date      = __DATE__;
const char * copyright = "(C) Copyright Rocco Carbone 2008";


/* Operating System header file(s) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>

/* Packet Capture Library */
#include <pcap.h>

/* Private header file(s) */
#include "ark.h"
#include "ark-html.h"
#include "counter.h"

/* The table of pcap-aware interfaces handled by this plugin */
static interface_t ** interfaces = NULL;

#include "webcallbacks.c"


/* Return # of items in the NULL terminated table of interfaces currently open */
static int interflen (interface_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


/* Dump statistics information at given time interval */
static void heartbeat_callback (int unused, const short event, void * _interface)
{
  interface_t * interface = _interface;
  struct timeval now;
  double delta;

  /* Show pkts/secs in the latest period */
  gettimeofday (& now, NULL);
  delta = delta_time_in_milliseconds (& now, & interface -> latest);

  if (interface -> received)
    {
      printf ("%s [%s@%s]: pkts rcvd #%.0f", progname, name, interface -> name, interface -> received);
      if (interface -> previous && delta)
	printf (" [%8.2f pkts/sec => +%.0f pkts in %s]",
		(double) (interface -> received - interface -> previous) * 1000 / delta,
		interface -> received - interface -> previous, elapsed_time (& interface -> latest, & now));
      printf ("\n");
    }

  interface -> previous = interface -> received;
  interface -> latest = now;

  /* Restart the heartbeat timer */
  evtimer_add (& interface -> hbtimer, & interface -> heartbeat);
}


/* Sniff packets from the interface */
static void sniffer_callback (int unused, const short event, void * _interface)
{
  interface_t * interface = _interface;
  pcap_t * pcap = interface -> pcap;
  struct pcap_pkthdr header;
  const unsigned char * packet;

  /* Read next packet from the interface */
  if ((packet = pcap_next (pcap, & header)))
    {
      interface -> received ++;

      if (header . len <= 64)
	interface -> upto64 ++;
      else if (header . len <= 128)
	interface -> upto128 ++;
      else if (header . len <= 256)
	interface -> upto256 ++;
      else if (header . len <= 512)
	interface -> upto512 ++;
      else if (header . len <= 1024)
	interface -> upto1024 ++;
      else if (header . len <= 1518)
	interface -> upto1518 ++;
      else
	interface -> above1518 ++;

      interface -> shortest = min (interface -> shortest, header . len);
      interface -> longest  = max (interface -> longest, header . len);
      interface -> sum     += header . len;
    }
}


/* Will be called once when the plugin is unloaded */
int halt (int argc, char * argv [])
{
  int n = interflen (interfaces);
  int i;

  /* Close the pcap-handle(s) and free memory */
  for (i = 0; i < n; i ++)
    {
      pcap_close (interfaces [i] -> pcap);
      if (event_initialized (& interfaces [i] -> hbtimer))
	event_del (& interfaces [i] -> hbtimer);
      free (interfaces [i] -> name);
      free (interfaces [i]);
    }
  free (interfaces);
  interfaces = NULL;

  /* Unregister web callbacks */
  nomoreweb ();

  return PLUGIN_OK;
}


/*
 * 1. Open network interface(s) to obtain pcap-handle(s)
 * 2. Capture packets from the interfaces
 * 3. Print statistics information at given time intervals
 */
int boot (int argc, char * argv [])
{
  /* The place for local variables */
  int option;

  /* The table of interfaces to open */
  char ** devices = NULL;
  int promiscuous = DEFAULT_PROMISCUOUS;
  int snapshot    = DEFAULT_SNAPSHOT;
  int timeout     = DEFAULT_TIMEOUT;
  char * device     = NULL;
  int heartbeat   = DEFAULT_HEARTBEAT;

  char ebuf [PCAP_ERRBUF_SIZE];
  int n;
  int i;

  /* Notice the plugin name */
  char * plgname = strrchr (argv [0], '/');
  plgname = ! plgname ? * argv : plgname + 1;

  /* Parse command-line options */
  optind = 0;
  optarg = NULL;
#define OPTSTRING "c:i:p:s:t:"
  while ((option = getopt (argc, argv, OPTSTRING)) != EOF)
    {
      switch (option)
	{
	default: return PLUGIN_FAIL;

	  /* Reserved options */
	case 'c': break;

	case 'i': devices     = argsadd (devices, optarg); break;  /* Multiple interfaces are allowed */
	case 'p': promiscuous = atoi (optarg);          break;
	case 's': snapshot    = atoi (optarg);          break;
	case 't': timeout     = atoi (optarg);          break;
	case 'b': heartbeat   = atoi (optarg);          break;
	}
    }

  /* Check for permissions */
  if ((getuid () && geteuid ()) || setuid (0))
    {
      printf ("%s [%s]: sorry, you must be root in order to run this program\n", progname, name);
      return PLUGIN_FAIL;
    }

  /* Find a suitable interface, if you don't have one */
  if (! devices && ! (device = pcap_lookupdev (ebuf)))
    {
      printf ("%s [%s]: no suitable interface found, please specify one with -d\n", progname, name);
      return PLUGIN_FAIL;
    }

  /* Allocate enough memory to keep the pointers to the interface(s) and to the pcap-handle(s) */
  if (device)
    devices = argsadd (devices, device);   /* default interface name */
  n = argslen (devices);

  interfaces = calloc ((n + 1) * sizeof (interface_t *), 1);
  for (i = 0; i < n; i ++)
    interfaces [i] = calloc (sizeof (interface_t), 1);
  interfaces [i] = NULL;

  /* Open the interface(s) for packet capturing */
  for (i = 0; i < n; i ++)
    if (! (interfaces [i] -> pcap = pcap_open_live (devices [i], snapshot, promiscuous, timeout, ebuf)))
      {
	argsfree (devices);
	printf ("%s [%s]: cannot open interface '%s' [error '%s']\n", progname, name, devices [i], ebuf);
	halt (argc, argv);
	return PLUGIN_FAIL;
      }
    else
      {
	interfaces [i] -> name        = strdup (devices [i]);
	interfaces [i] -> snapshot    = snapshot;
	interfaces [i] -> promiscuous = promiscuous;
	interfaces [i] -> timeout     = timeout;

	gettimeofday (& interfaces [i] -> started, NULL);  /* time the application started to capture packets on this interface */

	interfaces [i] -> received    = 0;
	interfaces [i] -> unicast     = 0;
	interfaces [i] -> broadcast   = 0;
	interfaces [i] -> multicast   = 0;

	interfaces [i] -> upto64      = 0;
	interfaces [i] -> upto128     = 0;
	interfaces [i] -> upto256     = 0;
	interfaces [i] -> upto512     = 0;
	interfaces [i] -> upto1024    = 0;
	interfaces [i] -> upto1518    = 0;
	interfaces [i] -> above1518   = 0;

	interfaces [i] -> shortest    = snapshot;
	interfaces [i] -> longest     = 0;
	interfaces [i] -> sum         = 0;

	interfaces [i] -> heartbeat . tv_sec = heartbeat;
	interfaces [i] -> heartbeat . tv_usec = 0;

	/* Add the file descriptor to the list of those monitored for read events */
	event_set (& interfaces [i] -> read_evt, pcap_get_selectable_fd (interfaces [i] -> pcap), EV_READ | EV_PERSIST, sniffer_callback, interfaces [i]);
	event_add (& interfaces [i] -> read_evt, NULL);

	/* Announce */
	printf ("%s [%s]: Plugin ready, now listening from %s using %s\n", progname, name, devices [i], pcap_lib_version ());

	/* Define the heartbeat callback and start the timer at given time interval */
	if (heartbeat)
	  evtimer_set (& interfaces [i] -> hbtimer, heartbeat_callback, interfaces [i]),
	    evtimer_add (& interfaces [i] -> hbtimer, & interfaces [i] -> heartbeat);
      }

  argsfree (devices);

  /* Register web callbacks */
  registerweb ();

  return PLUGIN_OK;
}


#if defined(STANDALONE)

int main (int argc, char * argv [])
{
  /* Notice the program name */
  progname = strrchr (argv [0], '/');
  progname = ! progname ? * argv : progname + 1;

  /* Set unbuffered stdout */
  setvbuf (stdout, NULL, _IONBF, 0);

  /* Perform one-time initialization of the libevent library */
  event_init ();

  if (boot (argc, argv) == PLUGIN_OK)
    {
      /* Go for network events */
      if (event_dispatch ())
	printf ("%s: Error - no active network socket enabled!\n", progname);
    }
  else
    printf ("%s: Error - cannot initialize!\n", progname);

  return 0;
}

#endif /* STANDALONE */
