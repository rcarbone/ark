/*
 * descriptions.c - Short descriptions for well known ark's plugins
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


/* The table of currently known plugins */
struct plg_t
{
  char * name;
  char * desc;
} plugins [] =
  {
    /* Tutorial */
    { "prototype.so",  "A prototype plugin. Does nothing" },
    { "helloworld.so", "The simplest plugin to solve the classic 'Hello, World!' problem" },
    { "heartbeat.so",  "A heartbeart tutorial plugin" },
    { "it-works.so",   "The simplest plugin to solve the classic 'It Works on the Web!' problem" },

    /* Main admin plugins */
    { "webadmin.so",   "Administration plugin accessible via HTTP Protocol" },
    { "telnetd.so",    "Administration plugin accessible via Telnet Protocol" },

    /* Production plugins in the role of network server */
    { "ucpd.so",       "Production server plugin accessible via UCP Protocol" },
    { "smppd.so",      "Production server plugin accessible via SMMP Protocol" },
    { "httpd.so",      "Production server plugin accessible via HTTP Protocol" },

    /* Production plugins in the role of network client */
    { "eRRe.so",       "Production client plugin using with UCP Protocol to connect to SMSC" },

    /* Test simulators plugins talking UCP protocol in the role of server for the push service */
    { "yes.so",        "Always accept all UCP requests" },
    { "no.so",         "Always deny all UCP requests" },
    { "null.so",       "Always drop all UCP requests" },
    { "delay.so",      "Always accept all UCP requests but reply after a given timeout" },
    { "lie.so",        "Always accept all UCP requests but reply with a fake response" },
    { "secure.so",     "Always accept all UCP requests over secure SSL transactions" },

    /* Test simulators plugins talking UCP protocol in the role of client for the push service */
    { "ping.so",       "Test client plugin talking UCP Protocol to connect to SMSC" },

    /* Test simulators plugins talking UCP protocol in the role of server for the pull service */
    { "pong.so",       "Test server plugin talking UCP Protocol (pull service)" },
    { "kgb.so",        "Test server plugin talking UCP Protocol (pull service)" },

    /* Loopback plugins (for internal test only) */
    { "kpush.so",       "Traffic generator (push service)" },
    { "kpull.so",       "Traffic generator (pull service)" },

    /* Sniffer plugins (for internal test only) */
    { "pcap.so",       "Simple packet sniffer (requires root)" },
    { "counter.so",    "Another packet sniffer (requires root)" },

    { NULL, NULL }
  };



static void init_desctable (struct hash_table * desc, unsigned int size)
{
  memset (desc, '\0', sizeof (* desc));
  desc -> size = size;

  hash_table_init (desc);
}


static int insert_description (struct hash_table * desc, char * name, char * description)
{
  struct hdatum pair;

  /* key: name del plugin */
  pair . key   = name;
  pair . ksize = strlen (name);

  /* value: description */
  pair . val   = description;
  pair . vsize = strlen (description);

  /* Insert (key, val) */
  if (! hash_table_insert (desc, & pair))
    return 0;

  return 1;
}


static char * lookup_description (struct hash_table * desc, char * name)
{
  struct hdatum pair;
  struct hdatum * found;

  if (! name)
    return NULL;

  memset (& pair, 0, sizeof (struct hdatum));

  pair . key   = name;
  pair . ksize = strlen (name);

  found = hash_table_search (desc, & pair);
  if (! found)
    return "no description available";

  return found -> val;
}


static void free_desctable (struct hash_table * desc)
{
  hash_table_free (desc);
}


static void fill_descriptions (struct hash_table * desc)
{
  struct plg_t * known = plugins;
  while (known -> name)
    insert_description (desc, known -> name, known -> desc),
      known ++;
}
