#
# Convert icon files (png, jpg, gif, etc) to C variables for
# html icons without files to be embedded in ark-aware plugins
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

CC      = gcc
CFLAGS  = -g -Wall

# Sources and objects files
SRCS    = icon2h.c
OBJS    = $(patsubst %.c, %.o, ${SRCS})
PROG    = icon2h

# Icon files

# General purpose
GENERAL  = project.png logo.png banner.png favicon.png background.png header.png footer.png button.png

# Related to the webadmin plugin
WEBADMIN = wwwadmin.png spider.png plghome.png

# Related to the it-works plugin
ITWORKS  = it_works.png penguin.png

# Related to the counter plugin
COUNTER  = counter.png chicken.png

# All the embedded icons
ICONS    = ${GENERAL} ${WEBADMIN} ${ITWORKS} ${COUNTER}

HEADERS = $(patsubst %.png, %.h, ${ICONS})

# How to make an object file
%.o: %.c
	@echo "=*= making object $@ =*="
	@${CC} ${CFLAGS} -c $< -o $@

# How to make a header file
%.h: %.png ${PROG}
	@echo "=*= making header $@ =*="
	@case $< in \
           spider.png)   ./${PROG} -f $< -n mascotte > $@ ;; \
           wwwadmin.png) ./${PROG} -f $< -n plgname > $@ ;; \
           penguin.png)  ./${PROG} -f $< -n mascotte > $@ ;; \
           it_works.png) ./${PROG} -f $< -n plgname > $@ ;; \
           chicken.png)  ./${PROG} -f $< -n mascotte > $@ ;; \
           counter.png)  ./${PROG} -f $< -n plgname > $@ ;; \
           *)            ./${PROG} -f $< > $@ ;; \
         esac

TARGETS = icon2h ${HEADERS}

all: ${TARGETS}

${PROG}: ${OBJS}
	@echo "=*= making program ${PROG} =*="
	@${CC} ${CFLAGS} ${OBJS} -o $@

clean:
	@rm -f ${OBJS} ${PROG} ${HEADERS}
	@rm -f *~ core

distclean: clean
	@rm -f ${DEPS}

-include ${DEPS}
