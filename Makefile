#
# Build everything related to the 'ark' project
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

# Where to find package identifiers
DDIR = docs

#
PACKAGE=`cat ${DDIR}/PACKAGE`
VERSION=`cat ${DDIR}/VERSION`
LITTLETOY = ${PKGNAME}-${VERSION}

DOCS = ${DDIR}/AUTHORS ${DDIR}/COPYING    ${DDIR}/ChangeLog \
       ${DDIR}/PACKAGE ${DDIR}/README     ${DDIR}/VERSION   \


# Where to locally patch the libevent
SRCDIR    = .

LIBEVENT  = libevent-1.4.5-stable
OPENSSL   = openssl-0.9.8e
RRD       = rrdtool-1.2.27

SUBDIRS   = icons network web src plugins

LINKS     = ${OPENSSL} ${LIBEVENT} ${RRD}

all: libevent openssl rrd lib modules
	@for dir in ${SUBDIRS}; do \
           if [ -d $$dir ]; then \
             echo "Making sub-directory $$dir ..."; \
             (cd $$dir && make -s); \
           fi \
         done
	@echo ""
	@echo "To run noah"
	@echo "1> setenv LD_LIBRARY_PATH `pwd`/libevent/.libs:`pwd`/lib"
	@echo "2> vi etc/noah.conf (to meet your configuration)"
	@echo "3> src/noah -c etc/noah.conf"
	@echo ""
	@echo "Good luck"
	@echo "/rocco"

libevent: download patch link

download:
	@if [ ! -d pub ]; then mkdir pub; fi
	@if [ ! -f pub/${LIBEVENT}.tar.gz ]; then \
          (cd pub && wget http://monkey.org/~provos/${LIBEVENT}.tar.gz); \
         fi
	@if [ ! -d ${LIBEVENT} ]; then \
	  tar xfz pub/${LIBEVENT}.tar.gz; \
         fi

patch: download
	@if ([ -f patches.libevent/libevent-webcallback.diff ] && [ ! -f ${SRCDIR}/${LIBEVENT}/libevent-webcallback.diff ]) || \
	    ([ -f patches.libevent/libevent-openssl.diff ] && [ ! -f ${SRCDIR}/${LIBEVENT}/libevent-openssl.diff ]); then \
          if [ ! -f ${SRCDIR}/${LIBEVENT}/http.c.ORG ]; then \
            cp ${SRCDIR}/${LIBEVENT}/http.c ${SRCDIR}/${LIBEVENT}/http.c.ORG; \
          fi; \
	  if [ ! -f ${SRCDIR}/${LIBEVENT}/evhttp.h.ORG ]; then \
            cp ${SRCDIR}/${LIBEVENT}/evhttp.h ${SRCDIR}/${LIBEVENT}/evhttp.h.ORG; \
          fi; \
	  if [ ! -f ${SRCDIR}/${LIBEVENT}/Makefile.in.ORG ]; then \
            cp ${SRCDIR}/${LIBEVENT}/Makefile.in ${SRCDIR}/${LIBEVENT}/Makefile.in.ORG; \
          fi; \
          if [ ! -f ${SRCDIR}/${LIBEVENT}/http-internal.h.ORG ]; then \
            cp ${SRCDIR}/${LIBEVENT}/http-internal.h ${SRCDIR}/${LIBEVENT}/http-internal.h.ORG; \
          fi; \
	  if [ ! -f ${SRCDIR}/${LIBEVENT}/libevent-webcallback.diff ]; then \
            echo "Patching libevent (1) ..."; \
	    cp patches.libevent/libevent-webcallback.diff ${SRCDIR}/${LIBEVENT}; \
            (cd ${SRCDIR}/${LIBEVENT}; cat libevent-webcallback.diff | patch -u;) \
          fi; \
          if [ ! -f ${SRCDIR}/${LIBEVENT}/libevent-openssl.diff ]; then \
            echo "Patching libevent (2) ..."; \
            cp patches.libevent/libevent-openssl.diff ${SRCDIR}/${LIBEVENT}; \
            (cd ${SRCDIR}/${LIBEVENT}; cat libevent-openssl.diff | patch -u;) \
          fi; \
	  (cd ${SRCDIR}/${LIBEVENT}; ./configure && make;) \
	fi

link: patch
	@if [ ! -h ${LIBEVENT} -a -d ${SRCDIR}/${LIBEVENT} ]; then \
           echo "Linking libevent ..."; \
           ln -sf ${SRCDIR}/${LIBEVENT} libevent; \
        fi

openssl:
	@if [ ! -h ${OPENSSL} -a -d ${SRCDIR}/${OPENSSL} ]; then \
           echo "Linking openssl ..."; \
           ln -s ${SRCDIR}/${OPENSSL} openssl; \
        fi

rrd:
	@if [ ! -h ${RRD} -a -d ${SRCDIR}/${RRD} ]; then \
           echo "Linking rrd ..."; \
           ln -s ${SRCDIR}/${RRD} rrd; \
        fi

# a place where to install locally generated ark libraries
lib:
	@if [ ! -d lib ]; then \
           mkdir lib; \
        fi

# a place where to install locally generated ark plugins
modules:
	@if [ ! -d modules ]; then \
           mkdir modules; \
        fi

clean:
	@for dir in ${SUBDIRS}; do \
           if [ -d $$dir ]; then \
             (cd $$dir && make -s $@); \
           fi \
         done
	@rm -f modules/*.so
	@rm -f *~

distclean: clean
	@rm -f lib/*.so
	@for dir in ${SUBDIRS}; do \
           if [ -d $$dir ]; then \
             (cd $$dir && make -s $@); \
           fi \
         done
	@rm -f libevent
	@if [ -d lib ]; then rmdir lib; fi
	@if [ -d modules ]; then rmdir modules; fi

# -=-=-=-=-=-=-=-=-=-=
# Source distribution
# -=-=-=-=-=-=-=-=-=-=
dist:
	@rm -f *~ icons/*~ etc/*~ docs/*~
	@rm -f *~ src/*.o src/*~ src/#*
	@rm -f *~ web/*.o web/*~ web/#*
	@rm -f *~ network/*.o network/*~ network/#*
#
	@if [ ! -d ${PACKAGE}-${VERSION} ]; then mkdir ${PACKAGE}-${VERSION}; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/docs ]; then mkdir ${PACKAGE}-${VERSION}/docs; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/etc ]; then mkdir ${PACKAGE}-${VERSION}/etc; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/icons ]; then mkdir ${PACKAGE}-${VERSION}/icons; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/network ]; then mkdir ${PACKAGE}-${VERSION}/network; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/patches.libevent ]; then mkdir ${PACKAGE}-${VERSION}/patches.libevent; fi
#
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins ]; then mkdir ${PACKAGE}-${VERSION}/plugins; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins/admin ]; then mkdir ${PACKAGE}-${VERSION}/plugins/admin; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins/admin/http ]; then mkdir ${PACKAGE}-${VERSION}/plugins/admin/http; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins/admin/telnet ]; then mkdir ${PACKAGE}-${VERSION}/plugins/admin/telnet; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins/sniffers ]; then mkdir ${PACKAGE}-${VERSION}/plugins/sniffers; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins/sniffers/counter ]; then mkdir ${PACKAGE}-${VERSION}/plugins/sniffers/counter; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins/sniffers/pcap ]; then mkdir ${PACKAGE}-${VERSION}/plugins/sniffers/pcap; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/plugins/tutorial ]; then mkdir ${PACKAGE}-${VERSION}/plugins/tutorial; fi
#
	@if [ ! -d ${PACKAGE}-${VERSION}/src ]; then mkdir ${PACKAGE}-${VERSION}/src; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/pub ]; then mkdir ${PACKAGE}-${VERSION}/pub; fi
	@if [ ! -d ${PACKAGE}-${VERSION}/web ]; then mkdir ${PACKAGE}-${VERSION}/web; fi
#
	@cp Makefile ${PACKAGE}-${VERSION}/
	@cp ${DOCS} ${PACKAGE}-${VERSION}/docs/
	@cp etc/*.conf ${PACKAGE}-${VERSION}/etc/
	@cp etc/*.template ${PACKAGE}-${VERSION}/etc/
	@cp icons/Makefile ${PACKAGE}-${VERSION}/icons/
	@cp icons/*.c ${PACKAGE}-${VERSION}/icons/
	@cp icons/*.png ${PACKAGE}-${VERSION}/icons/
	@cp network/Makefile ${PACKAGE}-${VERSION}/network/
	@cp network/*.h ${PACKAGE}-${VERSION}/network/
	@cp network/*.c ${PACKAGE}-${VERSION}/network/
	@cp patches.libevent/*.diff ${PACKAGE}-${VERSION}/patches.libevent/
#
	@cp plugins/Makefile ${PACKAGE}-${VERSION}/plugins/
	@cp plugins/admin/Makefile ${PACKAGE}-${VERSION}/plugins/admin/
	@cp plugins/admin/http/Makefile ${PACKAGE}-${VERSION}/plugins/admin/http/
	@cp plugins/admin/http/*.c ${PACKAGE}-${VERSION}/plugins/admin/http/
	@cp plugins/admin/http/*.h ${PACKAGE}-${VERSION}/plugins/admin/http/
	@cp plugins/admin/telnet/Makefile ${PACKAGE}-${VERSION}/plugins/admin/telnet/
	@cp plugins/admin/telnet/*.c ${PACKAGE}-${VERSION}/plugins/admin/telnet/
	@cp plugins/admin/telnet/*.h ${PACKAGE}-${VERSION}/plugins/admin/telnet/
	@cp plugins/sniffers/Makefile ${PACKAGE}-${VERSION}/plugins/sniffers/
	@cp plugins/sniffers/counter/Makefile ${PACKAGE}-${VERSION}/plugins/sniffers/counter/
	@cp plugins/sniffers/counter/*.c ${PACKAGE}-${VERSION}/plugins/sniffers/counter/
	@cp plugins/sniffers/counter/*.h ${PACKAGE}-${VERSION}/plugins/sniffers/counter/
	@cp plugins/sniffers/pcap/Makefile ${PACKAGE}-${VERSION}/plugins/sniffers/pcap/
	@cp plugins/sniffers/pcap/*.c ${PACKAGE}-${VERSION}/plugins/sniffers/pcap/
	@cp plugins/tutorial/Makefile ${PACKAGE}-${VERSION}/plugins/tutorial/
	@cp plugins/tutorial/*.c ${PACKAGE}-${VERSION}/plugins/tutorial/
	@cp plugins/tutorial/*.h ${PACKAGE}-${VERSION}/plugins/tutorial/
#
#	@cp pub/${LIBEVENT}.tar.gz ${PACKAGE}-${VERSION}/pub/
	@cp src/Makefile ${PACKAGE}-${VERSION}/src/
	@cp src/*.h ${PACKAGE}-${VERSION}/src/
	@cp src/*.c ${PACKAGE}-${VERSION}/src/
	@cp web/Makefile ${PACKAGE}-${VERSION}/web/
	@cp web/*.h ${PACKAGE}-${VERSION}/web/
	@cp web/*.c ${PACKAGE}-${VERSION}/web/
#
	@tar cfz ${PACKAGE}-${VERSION}.tar.gz ${PACKAGE}-${VERSION}
	@mv ${PACKAGE}-${VERSION}.tar.gz pub/
	@rm -rf ${PACKAGE}-${VERSION}
	@echo "your backup file is in pub/${PACKAGE}-${VERSION}.tar.gz"


# -=-=-=-=-=-=-=-=-=-=
# Binary distribution
# -=-=-=-=-=-=-=-=-=-=
distrib:
	@-if [ -d distribution ]; then true; else mkdir distribution; fi
	@-if [ -d distribution/${LITTLETOY} ]; then true; else mkdir distribution/${LITTLETOY}; fi
	@echo "== distribution => start"
#
	@echo "(1) copying libraries ............. please wait"
	@rm -rf distribution/${LITTLETOY}/lib; mkdir distribution/${LITTLETOY}/lib
	@cp lib/libbig.so distribution/${LITTLETOY}/lib/
	@cp lib/libproto.so distribution/${LITTLETOY}/lib/
#
# User libraries
#
	@cp libevent/.libs/libevent-1.4.so.2.0.0 distribution/${LITTLETOY}/lib/
	@(cd distribution/${LITTLETOY}/lib && ln -s libevent-1.4.so.2.0.0 libevent.so)
	@(cd distribution/${LITTLETOY}/lib && ln -s libevent-1.4.so.2.0.0 libevent-1.4.so.2)
	@chmod 444 distribution/${LITTLETOY}/lib/*
#
	@cp rrd/src/.libs/librrd.so.2.0.13 distribution/${LITTLETOY}/lib/
	@(cd distribution/${LITTLETOY}/lib && ln -s librrd.so.2.0.13 librrd.so)
	@(cd distribution/${LITTLETOY}/lib && ln -s librrd.so.2.0.13 librrd.so.2)
	@chmod 444 distribution/${LITTLETOY}/lib/*
#
# System libraries
#
	@cp /usr/lib/libart_lgpl_2.so.2.3.20 distribution/${LITTLETOY}/lib/
	@(cd distribution/${LITTLETOY}/lib && ln -s libart_lgpl_2.so.2.3.20 libart_lgpl_2.so)
	@(cd distribution/${LITTLETOY}/lib && ln -s libart_lgpl_2.so.2.3.20 libart_lgpl_2.so.2)
	@chmod 444 distribution/${LITTLETOY}/lib/*
#
	@cp /usr/lib/libfreetype.so.6.3.16 distribution/${LITTLETOY}/lib/
	@(cd distribution/${LITTLETOY}/lib && ln -s libfreetype.so.6.3.16 libfreetype.so.6)
	@(cd distribution/${LITTLETOY}/lib && ln -s libfreetype.so.6.3.16 libfreetype.so)
	@chmod 444 distribution/${LITTLETOY}/lib/*
#
	@echo "(2) copying binary files .......... please wait"
	@rm -rf distribution/${LITTLETOY}/bin; mkdir distribution/${LITTLETOY}/bin
	@cp kernel/big distribution/${LITTLETOY}/bin/
	@chmod 775 distribution/${LITTLETOY}/bin/*
#
	@echo "(3) copying plugins ............... please wait"
	@rm -rf distribution/${LITTLETOY}/plugins; mkdir distribution/${LITTLETOY}/plugins
	@cp modules/*.so distribution/${LITTLETOY}/plugins/
	@chmod 755 distribution/${LITTLETOY}/plugins/*
#
	@echo "(4) copying configuration files ... please wait"
	@rm -rf distribution/${LITTLETOY}/etc; mkdir distribution/${LITTLETOY}/etc
	@cp etc/big.conf distribution/${LITTLETOY}/etc/
	@cp etc/hosts.smsc distribution/${LITTLETOY}/etc/
	@chmod 644 distribution/${LITTLETOY}/etc/*
#
	@echo "(5) making tar file ............... please wait"
	@(cd distribution; tar chfz ${LITTLETOY}-`uname -s`-`uname -r | cut -d '.' -f1-2`.tar.gz ${LITTLETOY})
	@rm -rf distribution/${LITTLETOY}
	@echo "== distribution done => distribution/${LITTLETOY}-`uname -s`-`uname -r | cut -d '.' -f1-2`.tar.gz"
