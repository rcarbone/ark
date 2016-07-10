#
# Build static and dynamic libraries to be used as common framework while
# developing asynchronous event-driven client/server network applications
#
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

#
LIBEVENTDIR = ../libevent
SRCDIR      = ../src

CC      = gcc
CFLAGS  = -g -I. -I${LIBEVENTDIR} -I${SRCDIR} -Wall -fPIC -DPIC
LDFLAGS = -shared
ARFLAGS = rs

# Source, object and depend files
SRCS    =  tcpip.c request.c connection.c server.c client.c fmemdmp.c
OBJS    = $(patsubst %.c, %.o, ${SRCS})
DEPS    = $(patsubst %.c, %.M, ${SRCS})

LIB     = libnetwork.a
SHLIB   = libnetwork.so
TARGETS = ${LIB} ${SHLIB}

%.M: %.c
	@echo "=*= making dependencies for $< =*="
	@${CC} -MM -MT $(<:.c=.o) ${CFLAGS} ${INCLUDE} $< -o $@

%.o: %.c
	@echo "=*= making object $@ =*="
	@${CC} -c ${CFLAGS} $<

all: ${TARGETS}

${LIB}: ${OBJS}
	@echo "=*= making static library $@ =*="
	@${AR} ${ARFLAGS} $@ ${OBJS}

${SHLIB}: ${OBJS}
	@echo "=*= making dynamic library $@ =*="
	@${CC} ${CFLAGS} ${LDFLAGS} ${OBJS} -o $@
	@if [ -d ../lib ]; then \
          (rm -f ../lib/$@; cp $@ ../lib/$@); \
         else \
          echo "Warning: library $@ not locally installed in ../lib"; \
         fi

clean:
	@rm -f ${OBJS} ${TARGETS} *~

distclean: clean
	@rm -f ${DEPS}

-include $(DEPS)
