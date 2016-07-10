
 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
                                 _
                       __ _ _ __| | __
                      / _` | '__| |/ /
                     | (_| | |  |   <
                      \__,_|_|  |_|\_\

 'ark', is the home for asynchronous libevent-based plugins


                  (C) Copyright 2007-2008
         Rocco Carbone <rocco /at/ tecsiel /dot/ it>

 Released under the terms of GNU General Public License
 at version 3;  see included COPYING file for details

 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-

This README file includes:

    * Hello world!
    * How 'ark' can do all this?
    * Features
    * Plugins
    * Licensing
    * Download
    * Requirements
    * Platforms
    * Installation
    * Documentation
    * Bugs
    * References

 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-

* Hello world!
  ============

  The 'ark' project is a Libevent Application Development Framework for fast development of
  asynchronous, libevent-based applications, including of course network clients and servers,
  but not restricted to them.

  Contrary to most projects that add plugins support only when they reach a mature stage,
  the 'ark' project was born with plugins in mind, and being its primary goal, it comes out
  with plugins support since its first release.  So the 'ark' project is also a framework
  for quick developing of plugin managers.

  The 'ark' framework does not like fork() and pthread_create() mainly because it believes that
  a single, full event-driven, non-blocking, well designed process is better able to do its job.

  So 'ark' follows and extend the single-tasking methodolody with the following goals:

   o host on a single process several servers at the same time, each talking a different protocol
     and servicing as many clients as they can

   o host on a single process several clients at the same time, each talking a different protocol
     and pushing as many servers as they can

   o host on a single process both clients, servers and proxies all simultaneously, each performing
     its own job completely independent from others

   o allow for example a single process to externally appear first as a HTTP server and then as
     a SMTP server (think as simple is to unload a plugin and then start a new one)

   o allow inside the same process both clients and servers to cooperate each using the functionalities
     of the others (this is very useful writing modular protocols gateway such as from HTTP to something
     other and viceversa)

  o Reduce the effort to port already written applications using the libevent library into an 'ark-aware'
    plugin which can be then hosted on a istance of the 'ark' project

  o Enhance already available libevent application to be full controlled with the embedded Web server
    accessible with HTTP/HTTPs protocols 'ark' already provides for its own native plugins

  So the 'ark' framework aims to provide a confortable programmable environment for development first,
  and then hosting asynchronous libevent-based plugins, which can be one to one dynamically loaded,
  unloaded, managed, queried inside ark-aware main applications.

  The number of concurrent plugins in any istance of an ark-aware application is teorically limited
  only by operating system limits.


* How 'ark' can do all this?
  ==========================

  The main idea behind the implentation was to have a single place where to perform the two fundamental
  operations of libevent-based application need to be full operational: the one time step initialization
  of performed by mean of event_init() and the mechanism to loop and dispatch events usually performed
  with the event_dispatch().  So 'ark' architeture is designed to have these two operations in the main()
  while all the plugins lack these calls.  The main issues event_init() in order to allow plugins to
  found the libevent initialized and then start defining their own events callbacks, the load as many
  plugins as are configured/required and lately call the event_dispatch() for plugins event management.

  We believe that an example is much better than lot of words, so please go into plugins/tutorial directory
  and take a look at source files in it.  Also the main application 'noah' which makes part of the
  distribution uses this simply approch.


* Features
  ========

  o Provides a useful programmable environment via a C API in order to:

    * load/unload/manage plugins
      the API consists of 2 mandatory variable and two functions that a plugin has to define/implement
      plus any number of additional functions that can be also used by others plugins as global services

    * read and process all the configuration parameters via one simple ASCII text file
      using an ini-like syntax where each section is dedicated to a single plugin or
      main driver application

    * export web callbacks framework via the special ark-aministration plugin 'webadmin'

  o Two special administration plugins are part of the distribution whose main goal
    is to provide control over all available plugins over the driver application


* Plugins
  =======

This section gives you significant information about
how write an ark-aware plugin to enhance the lot
of commands it had already implemented.  It can be
also used as the 'ark' developer documentation.


1. How to write an ark-aware plugin
====================================

I believe that an example is much better than tons of words, so
before I will introduce the plugin API interface specification,
I will include here the simplest plugin you can write.
You can find the source code and Makefile for compilation
under the plugins/tutorial directory which can used as both
an evaluable tutorial and a template.


<helloworld.c>

/* Operating System header file(s) */
#include <stdio.h>

/* Private header file(s) */
#include "ark.h"


/* Plugin declarations */

const char * variables = { "name version author what when where why copyright" };
const char * functions = { "boot halt" };

/* Plugin definitions */

const char * name      = "helloworld";
const char * version   = "0.0.1";
const char * author    = "Rocco Carbone rocco@tecsiel.it";
const char * what      = "The simplest ark-aware plugin";
const char * when      = "Sun Mar 25 08:34:11 CET 2007";
const char * where     = "Pisa, Italy";
const char * why       = "Just to show how to write an ark-aware plugin";
const char * copyright = "(C) Copyright 2007-2008 Rocco Carbone";


/* Public functions */

int boot (int argc, char * argv [])
{
  printf ("%s [%s]: Hello World! My name is %s\n", progname, name, name);
  return PLUGIN_OK;
}


int halt (int argc, char * argv [])
{
  printf ("%s [%s]: Bye bye cruel world!\n", progname, name);
  return PLUGIN_OK;
}


</helloworld.c>


Warning:  This section need more and more
=========================================


* Licensing
  =========

  'ark', the Libevent Application Development Framework is released under the terms of the
  GNU General Public License at version 3; see included COPYING file for more information.


* Download
  ========

  You can download the source code and documentation at:

  https://github.com/rcarbone/ark


* Requirements
  ============

  'ark' is itself an application written using the libevent library

  Copyright (c) 2000-2007 Niels Provos <provos@citi.umich.edu>

  I developed and run several instances of 'ark' covering several
  application domains from network clients, servers and proxies, to
  passive network sniffers on my linux intel based box.

  At the time of writing latest version of libevent, an event notification library,
  can be found at:

  http://monkey.org/~provos/libevent-1.4.5-stable.tar.gz

  We need this version becuase we made two patches to the libevent library that
  are not yet included in the distribution.


* Platforms
  =========

  o i686 running Linux 2.6.18
  o sparc sun4u running Solaris 10 (not tested in last months)

  Just to be clear, my development environment is on an Intel-based box
  running a testing Debian GNU/Linux distribution, but I think the effort
  to port 'ark' on different un*x where the libevent is already available
  should be only a compilation issue


* Installation
  ============

     o To compile 'ark', you need to need to have the sources
       of the libevent on your system and build and install libevent
       from sources by yourself

     o Run 'make' at the top level of your 'ark' distribution

       I planned to modify the 'Makefile' at the top level of the 'ark'
       distribution, to also perform all required steps in order to have
       a patched version of the 'libevent' ready to be compiled with our
       patches until they are integrated in libevent
       
      The 'Makefile' also attempts to natively configure the 'libevent'
      and compile it by executing 'make'.


* Documentation
  =============

Most documentation is in the README and other files under the docs/
directory.  But I am sure the documentation is so far to be terminated
because a lot of functionalities need to be documented.


* Bugs
  ====

There are no mailing lists for 'ark' at this time.

Bugs can be reported to the author Rocco Carbone via email at:
<rocco /at/ tecsiel /dot/ it>



* References
  ==========

  http://monkey.org/~provos/libevent

