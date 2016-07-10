/*
 * milestones.c - 'webadmin' milestones to be rendered on html pages
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


/* A place for detailed description in the homepage */
static char * intro [] =
{
  "Webadmin is an ark-aware administration plugin, accessible via the HTTP/HTTPS protocols, to remotely control all other plugins currently running/available under the same instance of the main application.",

  "It provides an easy-to-use, colourful Web interface with both basic and advanced functionalities to administer a running istance, such as monitoring the overall status via its run-time variables and load/unload/manage/query/reconfigure running/available plugins.",

  "Being an administration plugin it can be included or not for being activated at system boot, but if included, contrary to other plugins, it cannot be unloaded.",

  " Features:<br>"
  " &#8226; Monitor main application (eg. uptime, statistics, software information, etc...)<br>"
  " &#8226; Handle main application (eg. restart, reload, reconfigure, suspend, etc...)<br>"
  " &#8226; Dynamically load/unload plugins<br>"
  " &#8226; Monitor running plugins (eg. configuration and run-time parameters, plugin general information, uptime, load average, statistics and counters, etc...)<br>"
  " &#8226; Handle running plugins (eg. restart, reload, reconfigure, suspend, counters reset, etc...)<br>"
  " &#8226; Support HTTP authentication<br>"
  " &#8226; Administer a list of allowed users to access the system<br>"
  " &#8226; Support both HTTP/HTTPs access protocols<br>",

  NULL
};


static changelog_t changelogs [] =
{
  { "Jul 27 2008", "Internet",
    "First public release announced to the libevent mailing list" },

  { "Jul 24 2008", "HTTPS",
    "Support HTTPS protocol with OpenSSL (and ongoing activities to use GNU TLS too)" },

  { "Jun 4 2008", "Authentication",
    "Load/unload plugins can be now configured using HTTP authentication to require user/password" },

  { "Apr 23 2008", "Mascottes!",
    "Accordingly to the project goal to be a safe place for plugins, each has now its own embedded \"animal\" icons to uniquely identify it" },

  { "Apr 20 2008", "Embedded icons",
    "A common css file is globally adopted among all plugins and the overall project!"
  },

  { "Apr 6 2008", "Configuration",
    "Webadmin has its own section in the configuration file that is now common and shared among the main application and all its plugins."
  },

  { "Apr 1 2008", "cvs",
    "First running version with basic load/unload features added to the cvs server."
  },

  { "Mar 10 2008", "Start coding",
    "After a short talk with the development team we agree to start coding using as main base the unfinished and never released Rocco's 2007 'big' project"
  },

  { NULL, NULL, NULL }
};


