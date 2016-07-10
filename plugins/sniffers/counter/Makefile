#
# Build an ark-aware plugin to sniff and count packets from network interface(s)
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

INSTALLDIR  = ../../../modules

LIBEVENTDIR = ../../../libevent
LIBWEBDIR   = ../../../web
SRCDIR      = ../../../src
LIBARKDIR   = ../../../lib
ICONDIR     = ../../../icons

INCLUDE = -I${LIBEVENTDIR} -I${LIBWEBDIR} -I${SRCDIR}

CC      = gcc
CFLAGS  = -g ${INCLUDE} -Wall -fPIC -DPIC
LDFLAGS = -shared -lpcap
USRLIBS = -L ${LIBEVENTDIR}/.libs -levent -L ${LIBARKDIR} -lark -larkwww
SYSLIBS = -lm -ldl

# Sources and object files
SRCS    = counter.c
OBJS    = $(patsubst %.c, %.o, ${SRCS})

ICONS   = plgname.h chicken.h

#
# Plugins
#
PLUGINS += counter.so

#
# Standalone binary
#
PROGRAM = counter

# Depend files
DEPS = $(patsubst %.c, %.M, ${SRCS})

# How to make a depend file
%.M: %.c
	@echo "=*= making dependencies for $< =*="
	@${CC} -MM -MT $(<:.c=.o) ${CFLAGS} ${INCLUDE} $< -o $@

# How to make an object file
%.o: %.c
	@echo "=*= making object $@ =*="
	@${CC} ${CFLAGS} -c $< -o $@

# How to make a plugin
%.so: %.o
	@echo "=*= making plugin $@ =*="
	@${CC} ${LDFLAGS} $< -o $@
	@(rm -f ${INSTALLDIR}/$@; cp $@ ${INSTALLDIR}/$@)

all: ${ICONS} ${PLUGINS}

# Standalone binary program
bin: ${SRCS}
	@echo "=*= making program ${PROGRAM} =*="
	@${CC} -DSTANDALONE ${CFLAGS} ${SRCS} ${USRLIBS} ${SYSLIBS} -o ${PROGRAM}

clean:
	@rm -f ${PLUGINS} ${OBJS} ${PROGRAM}
	@rm -f *~ core

distclean: clean
	@rm -f ${DEPS}

plgname.h: ${ICONDIR}/counter.h
	@cp $< $@

chicken.h: ${ICONDIR}/chicken.h
	@cp $< .

-include ${DEPS}
