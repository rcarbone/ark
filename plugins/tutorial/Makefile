#
# Build some tutorial ark-aware plugins:
#   1. prototype  - it does nothing but can be used as a template
#   2. helloworld - it solves the classic 'hello, world!' problem
#   3. heartbeat  - it print a mark at given time interval
#   4. it-works   - based on 'heartbeat', it adds web access via the 'webadmin' plugin
#
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
#                                 _
#                       __ _ _ __| | __
#                      / _` | '__| |/ /
#                     | (_| | |  |   <
#                      \__,_|_|  |_|\_\
#
# 'ark', is the home for asynchronous libevent-based plugins
#
#
#            (C) Copyright 2007-2008
#   Rocco Carbone <rocco /at/ tecsiel /dot/ it>
#
# Released under the terms of GNU General Public License
# at version 3;  see included COPYING file for details
#
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-=-=-
#

INSTALLDIR  = ../../modules

LIBEVENTDIR = ../../libevent
LIBWEBDIR   = ../../web
SRCDIR      = ../../src
ICONDIR     = ../../icons

INCLUDE = -I${LIBEVENTDIR} -I${LIBWEBDIR} -I${SRCDIR}

CC      = gcc
CFLAGS  = -g ${INCLUDE} -Wall -fPIC -DPIC
LDFLAGS = -shared

# Sources and object files
SRCS1   = helloworld.c
OBJS1   = helloworld.o
SRCS1   = prototype.c
OBJS2   = prototype.o
SRCS3   = heartbeat.c
OBJS3   = heartbeat.o
SRCS4   = it-works.c
OBJS4   = it-works.o
SRCS    = ${SRCS1} ${SRCS2} ${SRCS3} ${SRCS4}
OBJS    = ${OBJS1} ${OBJS2} ${OBJS3} ${OBJS4}

ICONS   = it_works.h penguin.h

# libevent based example program
EXAMPLE = heartbeat-libevt
EXASRCS = heartbeat-libevt.c
EXAOBJS = heartbeat-libevt.o

#
# Plugins
#
PLUGINS += helloworld.so
PLUGINS += prototype.so
PLUGINS += heartbeat.so
PLUGINS += it-works.so

# Depend files
DEPS = helloworld.M prototype.M heartbeat.M

# How to make a depend file
%.M: %.c
	@echo "=*= making dependencies for $< =*="
	@${CC} -MM -MT $(<:.c=.o) ${CFLAGS} ${INCLUDE} $< -o $@

# How to make an object file
%.o: %.c
	@echo "=*= making object $@ =*="
	@${CC} ${CFLAGS} -c $< -o $@

# How to make a shared object file
%.so: %.o
	@echo "=*= making plugin $@ =*="
	@${CC} ${LDFLAGS} $< -o $@
	@(rm -f ${INSTALLDIR}/$@; cp $@ ${INSTALLDIR}/$@)

all: ${ICONS} ${PLUGINS} ${EXAMPLE}

${EXAMPLE}: ${EXAOBJS}
	@echo "=*= making program $@ =*="
	@${CC} $< -L ${LIBEVENTDIR}/.libs -levent -o $@

clean:
	@rm -f ${ICONS} ${PLUGINS} ${OBJS}
	@rm -f ${EXAMPLE} ${EXAOBJS}
	@rm -f *~ core

distclean: clean
	@rm -f ${DEPS}

it_works.h: ${ICONDIR}/it_works.h
	@cp $< .

penguin.h: ${ICONDIR}/penguin.h
	@cp $< .

-include ${DEPS}
