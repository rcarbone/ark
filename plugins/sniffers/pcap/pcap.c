/*
 * pcap.c - Simple ark-aware packet sniffer plugin for pcap-aware interface(s)
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
const char * functions = { "halt boot" };


/* Plugin definitions */

const char * name      = "pcap";
const char * version   = "0.0.1";
const char * author    = "Rocco Carbone rocco@tecsiel.it";
const char * date      = __DATE__;
const char * copyright = "(C) Copyright 2008 Rocco Carbone";


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

#define DEFAULT_PROMISCUOUS 1      /* promiscuous mode by default     */
#define DEFAULT_SNAPSHOT    1518   /* default snapshot length         */
#define DEFAULT_PACKETS     100    /* default # of packets to capture */
#define DEFAULT_TIMEOUT     1000   /* default timeout in msec         */
#define DEFAULT_HB          10     /* default heartbeat               */


/* All that is needed to handle a pcap-aware interface */
typedef struct
{
  char * name;             /* interface name (eg. eth0) */
  int snapshot;
  int promiscuous;
  int timeout;
  pcap_t * pcap;           /* pcap handle as returned by pcap_open_live() */
  struct event read_evt;   /* used to detect events on the file descriptor via the libevent */

  /* Packets counters */
  unsigned long maxcount;
  unsigned long partial;
  unsigned long errors;

  struct timeval started;
  struct timeval stopped;

} interface_t;



/* The table of pcap-aware interfaces handled by this plugin */
static interface_t ** interfaces = NULL;


/* Return # of items in the NULL terminated table of interfaces currently open */
static int interflen (interface_t * argv [])
{
  int argc = 0; while (argv && * argv ++) argc ++; return argc;
}


static int hb = DEFAULT_HB;        /* heartbeat */


/* Public funtions in file interval.c */
time_t delta_time_in_milliseconds (struct timeval * t2, struct timeval * t1);
void print_time_in_secs (struct timeval * t, char * label);
char * elapsed_time (struct timeval * start, struct timeval * stop);
char * xpercentage (unsigned long partial, unsigned long total);


static void sniffer_callback (int unused, const short event, void * _interface)
{
  interface_t * interface = _interface;
  pcap_t * pcap = interface -> pcap;
  struct pcap_pkthdr header;
  const unsigned char * packet;

  double delta;

  if (! interface -> maxcount || (interface -> partial + interface -> errors) < interface -> maxcount)
    {
      /* Please pcap give me next packet from the interface */
      if ((packet = pcap_next (pcap, & header)))
	{
	  interface -> partial ++;
	  if (! (interface -> partial % hb))
	    {
	      static unsigned long previous = 0;
	      static struct timeval latest;

	      struct timeval now;

	      /* Show pkts/secs in the latest period */
	      gettimeofday (& now, NULL);
	      delta = delta_time_in_milliseconds (& now, & latest);

	      if (interface -> maxcount)
		printf ("%s: pkts rcvd #%lu of #%lu %s", progname, interface -> partial, interface -> maxcount,
			xpercentage (interface -> partial, interface -> maxcount));
	      else
		printf ("%s: pkts rcvd #%lu %s", progname, interface -> partial, xpercentage (interface -> partial, interface -> maxcount));
	      if (previous && delta)
		printf (" [%8.2f pkts/sec => +%lu pkts in %s]",
			(double) (interface -> partial - previous) * 1000 / delta,
			interface -> partial - previous, elapsed_time (& latest, & now));
	      printf ("\n");

	      previous = interface -> partial;
	      latest = now;
	    }
	}
      else
	interface -> errors ++;
    }
  else
    {
      gettimeofday (& interface -> stopped, NULL);
      delta = (double) delta_time_in_milliseconds (& interface -> stopped, & interface -> started);

      printf ("              \n");

      printf ("Time:\n");
      printf ("=====\n");
      print_time_in_secs (& interface -> started, "Started:       ");
      print_time_in_secs (& interface -> stopped, "Finished:      ");
      printf ("Elapsed Time:  %s\n", elapsed_time (& interface -> started, & interface -> stopped));
      printf ("\n");

      /* Print out test results */
      printf ("Great Totals:\n");
      printf ("=============\n");
      printf ("pkts rcvd #%lu pckts of #%lu => %7.2f pkts/sec\n\n",
	      interface -> partial, interface -> maxcount, (double) interface -> partial * 1000 / delta);

      event_del (& interface -> read_evt);
    }
}


/* Will be called once when the plugin is unloaded */
int halt (int argc, char * argv [])
{
  int n = interflen (interfaces);
  int p;

  /* Close the pcap-handle(s) and free memory */
  for (p = 0; p < n; p ++)
    {
      pcap_close (interfaces [p] -> pcap);
      free (interfaces [p] -> name);
      free (interfaces [p]);
    }
  free (interfaces);
  interfaces = NULL;

  return PLUGIN_OK;
}


/*
 * 1. Open network interface(s) to obtain pcap-handle(s)
 * 2. Capture 'n' packets per interface
 * 3. Print global statistics information
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
  char * device   = NULL;

  unsigned long maxcount = DEFAULT_PACKETS;

  char ebuf [PCAP_ERRBUF_SIZE];
  int n;
  int i;

  /* Notice the plugin name */
  char * plgname = strrchr (argv [0], '/');
  plgname = ! plgname ? * argv : plgname + 1;

  /* Parse command-line options */
  optind = 0;
  optarg = NULL;
#define OPTSTRING "c:i:p:s:t:n:b:"
  while ((option = getopt (argc, argv, OPTSTRING)) != EOF)
    {
      switch (option)
	{
	default: return PLUGIN_FAIL;

	  /* Reserved options */
	case 'c': break;

	case 'i': devices     = argsadd (devices, optarg); break;  /* Multiple interfaces are allowed */
	case 'p': promiscuous = atoi (optarg); break;
	case 's': snapshot    = atoi (optarg); break;
	case 't': timeout     = atoi (optarg); break;
	case 'n': maxcount    = atoi (optarg); break;

	case 'b': hb = atoi (optarg);          break;
	}
    }

  if (hb == -1)
    hb = maxcount / DEFAULT_HB;
  if (! hb)
    hb = 1;

  /* Check for permissions */
  if ((getuid () && geteuid ()) || setuid (0))
    {
      printf ("%s [%s]: sorry, you must be root in order to run this program\n", progname, name);
      return PLUGIN_FAIL;
    }

  /* Find a suitable interface, if you don't have one */
  if (! devices && ! (device = pcap_lookupdev (ebuf)))
    {
      printf ("%s [%s]: no suitable interface found, please specify one with -i\n", progname, name);
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
	interfaces [i] -> maxcount    = maxcount;
	interfaces [i] -> partial     = 0;
	interfaces [i] -> errors      = 0;

	gettimeofday (& interfaces [i] -> started, NULL);  /* time the application started to capture packets on this interface */

	/* Add the file descriptor to the list of those monitored for read events */
	event_set (& interfaces [i] -> read_evt, pcap_get_selectable_fd (interfaces [i] -> pcap), EV_READ | EV_PERSIST, sniffer_callback, interfaces [i]);
	event_add (& interfaces [i] -> read_evt, NULL);

	/* Announce */
	printf ("%s [%s]: Plugin ready, now listening from %s using %s\n", progname, name, devices [i], pcap_lib_version ());
      }

  argsfree (devices);

  return PLUGIN_OK;
}


#if defined(STANDALONE)

int main (int argc, char * argv [])
{
  /* Notice the program name */
  char * progname = strrchr (argv [0], '/');
  progname = ! progname ? * argv : progname + 1;

#if defined(ROCCO)
  /* Set the time the program was started */
  started ();
#endif /* ROCCO */

  /* Set unbuffered stdout */
  setvbuf (stdout, NULL, _IONBF, 0);

  /* Ignore writes to connections that have been closed at the other end */
  signal (SIGPIPE, SIG_IGN);

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
