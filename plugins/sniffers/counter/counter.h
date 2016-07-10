
#define DEFAULT_PROMISCUOUS 1      /* promiscuous mode by default */
#define DEFAULT_SNAPSHOT    1518   /* default snapshot length     */
#define DEFAULT_TIMEOUT     1000   /* default timeout in msec     */
#define DEFAULT_HEARTBEAT   3      /* default heartbeat in sec    */

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

/* Declarations for the web access */
#define PLGNAME       "counter"
#define MASCOTTE_H    "chicken.h"

/* Additional urls to register */
#define URL_PACKETS   "/"PLGNAME"/packets"

/* Additional links names */
#define LINK_PACKETS  "Packets"


#define COUNTER_T
typedef double count_t;


/* All that is needed to handle a pcap-aware interface */
typedef struct
{
  char * name;             /* interface name (eg. eth0) */
  int snapshot;
  int promiscuous;
  int timeout;
  pcap_t * pcap;           /* pcap handle as returned by pcap_open_live() */
  struct event read_evt;   /* used to detect events on the file descriptor via the libevent */

  struct timeval started;
  struct timeval latest;
  count_t previous;

  /* Packets counters */
  count_t received;
  count_t unicast;
  count_t broadcast;
  count_t multicast;

  count_t upto64;
  count_t upto128;
  count_t upto256;
  count_t upto512;
  count_t upto1024;
  count_t upto1518;
  count_t above1518;

  count_t shortest;
  count_t longest;
  count_t sum;

  struct timeval heartbeat;   /* heartbeat in seconds (0 never) */
  struct event hbtimer;

} interface_t;


/* Public funtions in file interval.c */
time_t delta_time_in_milliseconds (struct timeval * t2, struct timeval * t1);
char * elapsed_time (struct timeval * start, struct timeval * stop);
char * xpercentage (unsigned long partial, unsigned long total);
