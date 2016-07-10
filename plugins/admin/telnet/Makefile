#
# Build the ark-aware plugin, accessible via the Telnet Protocol, to remotely control other plugins
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
LIBNETDIR   = ../../../network
SRCDIR      = ../../../src

INCLUDE = -I${LIBEVENTDIR} -I${LIBNETDIR} -I${SRCDIR}

CC      = gcc
CFLAGS  = -g ${INCLUDE} -Wall -fPIC -DPIC
LDFLAGS = -shared
USRLIBS = -L ${LIBEVENTDIR}/.libs -levent -L ${LIBNETDIR} -lnetwork -L ${SRCDIR} -lark
SYSLIBS = -lm -ldl

# Sources and object files
SRCS    = telnetd.c
OBJS    = $(patsubst %.c, %.o, ${SRCS})

#
# Plugins
#
PLUGINS += telnetd.so

#
# Standalone binary
#
PROGRAM = ark-telnetd

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

# How to make a shared object file
%.so: %.o
	@echo "=*= making plugin $@ =*="
	@${CC} ${LDFLAGS} $< -o $@
	@(rm -f ${INSTALLDIR}/$@; cp $@ ${INSTALLDIR}/$@)

all: ${PLUGINS}

# Standalone binary program
bin: ${SRCS}
	@echo "=*= making program ${PROGRAM} =*="
	@${CC} -DSTANDALONE ${CFLAGS} ${SRCS} ${USRLIBS} ${SYSLIBS} -o ${PROGRAM}

clean:
	@rm -f ${PLUGINS} ${OBJS} ${PROGRAM}
	@rm -f *~ core

distclean: clean
	@rm -f ${DEPS}

-include ${DEPS}
