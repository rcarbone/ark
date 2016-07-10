#
# Build the ark-aware plugin, accessible via the HTTP Protocol, to remotely control other plugins
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
LIBNETDIR   = ../../../network
SRCDIR      = ../../../src
ICONDIR     = ../../../icons

INCLUDE = -I${LIBEVENTDIR} -I${LIBWEBDIR} -I${LIBNETDIR} -I${SRCDIR}

CC      = gcc
CFLAGS  = -g ${INCLUDE} -Wall -fPIC -DPIC
LDFLAGS = -shared
USRLIBS = -L ${LIBEVENTDIR}/.libs -levent -L ${LIBWEBDIR} -lweb -L ${LIBNETDIR} -lnetwork -L ${SRCDIR} -lark
SYSLIBS = -lm -ldl

# Sources and object files
SRCS    = webadmin.c
OBJS    = $(patsubst %.c, %.o, ${SRCS})

# Automatically generated icons to be embedded into html pages
ICONS   = project.h logo.h banner.h favicon.h background.h \
          header.h footer.h button.h plghome.h \
          wwwadmin.h spider.h

#
# Plugins
#
PLUGINS += webadmin.so

#
# Standalone binary
#
PROGRAM = ark-httpd

# Depend files
DEPS = $(patsubst %.c, %.M, ${SRCS})

# How to make a depend file
%.M: %.c
	@echo "=*= making dependencies for $< =*="
	@-${CC} -MM -MT $(<:.c=.o) ${CFLAGS} ${INCLUDE} $< -o $@ 1 > /dev/null 2>& 1

# How to make an object file
%.o: %.c
	@echo "=*= making object $@ =*="
	@${CC} ${CFLAGS} -c $< -o $@

# How to make a shared object file
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
	@rm -f ${ICONS} ${PLUGINS} ${OBJS} ${PROGRAM}
	@rm -f *~ core

distclean: clean
	@rm -f ${DEPS}

project.h: ${ICONDIR}/project.h
	@cp $< .

logo.h: ${ICONDIR}/logo.h
	@cp $< .

banner.h: ${ICONDIR}/banner.h
	@cp $< .

favicon.h: ${ICONDIR}/favicon.h
	@cp $< .

background.h: ${ICONDIR}/background.h
	@cp $< .

header.h: ${ICONDIR}/header.h
	@cp $< .

footer.h: ${ICONDIR}/footer.h
	@cp $< .

button.h: ${ICONDIR}/button.h
	@cp $< .

plghome.h: ${ICONDIR}/plghome.h
	@cp $< .

wwwadmin.h: ${ICONDIR}/wwwadmin.h
	@cp $< .

spider.h: ${ICONDIR}/spider.h
	@cp $< .

-include ${DEPS}
