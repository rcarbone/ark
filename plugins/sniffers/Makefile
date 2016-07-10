#
# Recursively scan dirs to build all the sniffers ark-aware plugins
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

SUBDIRS = pcap counter

all:
	@for dir in ${SUBDIRS}; do \
           if [ -d $$dir ]; then \
             echo "Making sub-directory $$dir ..."; \
             (cd $$dir && make -s); \
           fi \
         done

clean distclean:
	@rm -f *.so
	@for dir in ${SUBDIRS}; do \
           if [ -d $$dir ]; then \
             (cd $$dir && make -s $@); \
           fi \
         done
