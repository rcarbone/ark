#
# Build the common framework to develop asynchronous event-driven
# client/server network applications as a shared object
# and the "righteous in current generation" program 'noah'
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


OS=Linux
#
LIBEVENTDIR = ../libevent
LIBARKDIR   = ../lib

CC       = gcc
CFLAGS   = -g -I. -I${LIBEVENTDIR} -Wall -fPIC -DPIC
LDFLAGS  = -shared
USRLIBS  = -L ${LIBEVENTDIR}/.libs -levent -L ${LIBARKDIR} -lnetwork -lark -larkwww
SYSLIBS  = -lm -ldl

OBJS     = noah.o

SHOBJS   = ark.o args.o globals.o parsecfg.o time.o
ifneq (Linux, $(findstring Linux, $(OS)))
SHOBJS  += getopt.o
endif

# Depend files
DEPS = $(patsubst %.o, %.M, ${OBJS}) $(patsubst %.o, %.M, ${SHOBJS})

LIB      = libark.so
TARGET   = noah

%.M: %.c
	@echo "=*= making dependencies for $< =*="
	@${CC} -MM -MT $(<:.c=.o) ${CFLAGS} ${INCLUDE} $< -o $@

all: ${LIB} ${TARGET}

${LIB}: ${SHOBJS}
	@echo "=*= making library $@ =*="
	@${CC} ${CFLAGS} ${LDFLAGS} ${SHOBJS} -o $@
	@(rm -f ../lib/$@; cp $@ ../lib/$@)

# Binary programs
${TARGET}: ${OBJS}
	@echo "=*= making program $@ =*="
	@${CC} ${OBJS} ${USRLIBS} ${SYSLIBS} -o $@

clean:
	@rm -f ${OBJS} ${SHOBJS}
	@rm -f ${LIB} ${TARGET}
	@rm -f *~ core

distclean: clean
	@rm -f ${DEPS}

-include $(DEPS)
