# Makefile.in generated by automake 1.6.3 from Makefile.am.
# soapcpp2/src/Makefile.  Generated from Makefile.in by configure.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



####### This is the input file for automake, which will generate Makefile.in ##########
SHELL = /bin/sh

srcdir = .
top_srcdir = ../..

prefix = /home/adsmt/uClinux/mts/user/cwmp-tr069
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include
pkgdatadir = $(datadir)/soapcpp2
pkglibdir = $(libdir)/soapcpp2
pkgincludedir = $(includedir)/soapcpp2
top_builddir = ../..

ACLOCAL = ${SHELL} /home/adsmt/uClinux/mts/user/cwmp-tr069/gsoaplib/missing --run aclocal-1.6
AUTOCONF = ${SHELL} /home/adsmt/uClinux/mts/user/cwmp-tr069/gsoaplib/missing --run autoconf
AUTOHEADER = ${SHELL} /home/adsmt/uClinux/mts/user/cwmp-tr069/gsoaplib/missing --run autoheader

am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_SCRIPT = ${INSTALL}
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = mips-linux
host_triplet = mips-mips-linux

EXEEXT = 
OBJEXT = o
PATH_SEPARATOR = :
AMTAR = ${SHELL} /home/adsmt/uClinux/mts/user/cwmp-tr069/gsoaplib/missing --run tar
AWK = gawk
BISON_DEFINE = -DWITH_BISON
CC = $(RSDK)/bin/rsdk-linux-gcc  -Os
CPP = $(RSDK)/bin/rsdk-linux-gcc  -Os -E
CXX = $(RSDK)/bin/rsdk-linux-g++ 
DEPDIR = .deps
ENABLE_SAMPLES = 
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LEX = flex
LEXLIB = 
LEX_DEFINE = -DWITH_FLEX
LEX_FLAGS = -l
LEX_OUTPUT_ROOT = lex.yy
LN_S = ln -s
PACKAGE = soapcpp2
RANLIB = $(RSDK)/bin/rsdk-linux-ranlib
SAMPLE_DIRS = 
SAMPLE_EXTRA_LIBS = 
SOAPCPP2_DEBUG = 
SOAPCPP2_NONAMESPACES = 
STRIP = $(RSDK)/bin/rsdk-linux-strip
VERSION = 2.7
YACC = bison -y
YACC_LIB = 
am__include = include
am__quote = 
install_sh = /home/adsmt/uClinux/mts/user/cwmp-tr069/gsoaplib/install-sh
platform = LINUX

AUTOMAKE_OPTIONS = foreign 1.4

#LIBS=
AM_YFLAGS = -d -v
AM_LFLAGS = $(LEX_FLAGS)

bin_PROGRAMS = soapcpp2

soapcpp2_CFLAGS = $(BISON_DEFINE) $(LEX_DEFINE) $(C_DEBUG_FLAGS) -D$(platform)
soapcpp2_LDADD = $(YACC_LIB) $(LEXLIB)
soapcpp2_SOURCES = soapcpp2_yacc.y soapcpp2_lex.l symbol2.c error2.c init2.c soapcpp2.c

CLEANFILES = *~ soapcpp2_lex.c soapcpp2_yacc.c soapcpp2_yacc.h y.output soapcpp2_yacc.output
subdir = soapcpp2/src
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
bin_PROGRAMS = soapcpp2$(EXEEXT)
PROGRAMS = $(bin_PROGRAMS)

am_soapcpp2_OBJECTS = soapcpp2-soapcpp2_yacc.$(OBJEXT) \
	soapcpp2-soapcpp2_lex.$(OBJEXT) soapcpp2-symbol2.$(OBJEXT) \
	soapcpp2-error2.$(OBJEXT) soapcpp2-init2.$(OBJEXT) \
	soapcpp2-soapcpp2.$(OBJEXT)
soapcpp2_OBJECTS = $(am_soapcpp2_OBJECTS)
soapcpp2_DEPENDENCIES =
soapcpp2_LDFLAGS =

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES =  -I. -I$(srcdir) -I$(top_builddir)
CPPFLAGS = -DLINUX -I/home/adsmt/uClinux/mts/user/cwmp-tr069/../../lib/libssl/include -I/home/adsmt/uClinux/mts/user/cwmp-tr069/matrixssl-1-8-3-open
LDFLAGS =  -L/home/adsmt/uClinux/mts/lib/librtk
LIBS = 
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
#DEP_FILES = ./$(DEPDIR)/soapcpp2-error2.Po \
#	./$(DEPDIR)/soapcpp2-init2.Po \
#	./$(DEPDIR)/soapcpp2-soapcpp2.Po \
#	./$(DEPDIR)/soapcpp2-soapcpp2_lex.Po \
#	./$(DEPDIR)/soapcpp2-soapcpp2_yacc.Po \
#	./$(DEPDIR)/soapcpp2-symbol2.Po
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
CFLAGS = -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DUSE_LIBMD5 -I/home/adsmt/uClinux/mts/include -I/home/adsmt/uClinux/mts/lib/libmd5 -Dlinux -D__linux__ -Dunix -DEMBED -D_USE_RSDK_WRAPPER_ -D_LINUX_2_6_ -D_LINUX_2_6_30_ -include linux/config.h -I/home/adsmt/uClinux/mts -DWITH_IPV6
LEXCOMPILE = $(LEX) $(LFLAGS) $(AM_LFLAGS)
YACCCOMPILE = $(YACC) $(YFLAGS) $(AM_YFLAGS)
DIST_SOURCES = $(soapcpp2_SOURCES)
DIST_COMMON = Makefile.am Makefile.in soapcpp2_lex.c soapcpp2_yacc.c \
	soapcpp2_yacc.h
SOURCES = $(soapcpp2_SOURCES)

all: all-am

.SUFFIXES:
.SUFFIXES: .c .l .o .obj .y
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)
binPROGRAMS_INSTALL = $(INSTALL_PROGRAM)
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	  ; then \
	    f=`echo "$$p1" | sed 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) $$p $(DESTDIR)$(bindir)/$$f"; \
	   $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) $$p $(DESTDIR)$(bindir)/$$f; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f $(DESTDIR)$(bindir)/$$f"; \
	  rm -f $(DESTDIR)$(bindir)/$$f; \
	done

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)
soapcpp2_yacc.h: soapcpp2_yacc.c
	@if test ! -f $@; then \
	  rm -f soapcpp2_yacc.c; \
	  $(MAKE) soapcpp2_yacc.c; \
	else :; fi
soapcpp2-soapcpp2_yacc.$(OBJEXT): soapcpp2_yacc.c
soapcpp2-soapcpp2_lex.$(OBJEXT): soapcpp2_lex.c
soapcpp2-symbol2.$(OBJEXT): symbol2.c
soapcpp2-error2.$(OBJEXT): error2.c
soapcpp2-init2.$(OBJEXT): init2.c
soapcpp2-soapcpp2.$(OBJEXT): soapcpp2.c
soapcpp2$(EXEEXT): $(soapcpp2_OBJECTS) $(soapcpp2_DEPENDENCIES) 
	@rm -f soapcpp2$(EXEEXT)
	$(LINK) $(soapcpp2_LDFLAGS) $(soapcpp2_OBJECTS) $(soapcpp2_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

#include ./$(DEPDIR)/soapcpp2-error2.Po
#include ./$(DEPDIR)/soapcpp2-init2.Po
#include ./$(DEPDIR)/soapcpp2-soapcpp2.Po
#include ./$(DEPDIR)/soapcpp2-soapcpp2_lex.Po
#include ./$(DEPDIR)/soapcpp2-soapcpp2_yacc.Po
#include ./$(DEPDIR)/soapcpp2-symbol2.Po

distclean-depend:
	-rm -rf ./$(DEPDIR)

.c.o:
#	source='$<' object='$@' libtool=no 
#	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(COMPILE) -c `test -f '$<' || echo '$(srcdir)/'`$<

.c.obj:
#	source='$<' object='$@' libtool=no 
#	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(COMPILE) -c `cygpath -w $<`

soapcpp2-soapcpp2_yacc.o: soapcpp2_yacc.c
#	source='soapcpp2_yacc.c' object='soapcpp2-soapcpp2_yacc.o' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-soapcpp2_yacc.Po' tmpdepfile='$(DEPDIR)/soapcpp2-soapcpp2_yacc.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-soapcpp2_yacc.o `test -f 'soapcpp2_yacc.c' || echo '$(srcdir)/'`soapcpp2_yacc.c

soapcpp2-soapcpp2_yacc.obj: soapcpp2_yacc.c
#	source='soapcpp2_yacc.c' object='soapcpp2-soapcpp2_yacc.obj' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-soapcpp2_yacc.Po' tmpdepfile='$(DEPDIR)/soapcpp2-soapcpp2_yacc.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-soapcpp2_yacc.obj `cygpath -w soapcpp2_yacc.c`

soapcpp2-soapcpp2_lex.o: soapcpp2_lex.c
#	source='soapcpp2_lex.c' object='soapcpp2-soapcpp2_lex.o' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-soapcpp2_lex.Po' tmpdepfile='$(DEPDIR)/soapcpp2-soapcpp2_lex.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-soapcpp2_lex.o `test -f 'soapcpp2_lex.c' || echo '$(srcdir)/'`soapcpp2_lex.c

soapcpp2-soapcpp2_lex.obj: soapcpp2_lex.c
#	source='soapcpp2_lex.c' object='soapcpp2-soapcpp2_lex.obj' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-soapcpp2_lex.Po' tmpdepfile='$(DEPDIR)/soapcpp2-soapcpp2_lex.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-soapcpp2_lex.obj `cygpath -w soapcpp2_lex.c`

soapcpp2-symbol2.o: symbol2.c
#	source='symbol2.c' object='soapcpp2-symbol2.o' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-symbol2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-symbol2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-symbol2.o `test -f 'symbol2.c' || echo '$(srcdir)/'`symbol2.c

soapcpp2-symbol2.obj: symbol2.c
#	source='symbol2.c' object='soapcpp2-symbol2.obj' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-symbol2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-symbol2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-symbol2.obj `cygpath -w symbol2.c`

soapcpp2-error2.o: error2.c
#	source='error2.c' object='soapcpp2-error2.o' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-error2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-error2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-error2.o `test -f 'error2.c' || echo '$(srcdir)/'`error2.c

soapcpp2-error2.obj: error2.c
#	source='error2.c' object='soapcpp2-error2.obj' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-error2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-error2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-error2.obj `cygpath -w error2.c`

soapcpp2-init2.o: init2.c
#	source='init2.c' object='soapcpp2-init2.o' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-init2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-init2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-init2.o `test -f 'init2.c' || echo '$(srcdir)/'`init2.c

soapcpp2-init2.obj: init2.c
#	source='init2.c' object='soapcpp2-init2.obj' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-init2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-init2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-init2.obj `cygpath -w init2.c`

soapcpp2-soapcpp2.o: soapcpp2.c
#	source='soapcpp2.c' object='soapcpp2-soapcpp2.o' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-soapcpp2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-soapcpp2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-soapcpp2.o `test -f 'soapcpp2.c' || echo '$(srcdir)/'`soapcpp2.c

soapcpp2-soapcpp2.obj: soapcpp2.c
#	source='soapcpp2.c' object='soapcpp2-soapcpp2.obj' libtool=no 
#	depfile='$(DEPDIR)/soapcpp2-soapcpp2.Po' tmpdepfile='$(DEPDIR)/soapcpp2-soapcpp2.TPo' 
#	$(CCDEPMODE) $(depcomp) 
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(soapcpp2_CFLAGS) $(CFLAGS) -c -o soapcpp2-soapcpp2.obj `cygpath -w soapcpp2.c`
CCDEPMODE = depmode=none

.l.c:
	$(LEXCOMPILE) `test -f $< || echo '$(srcdir)/'`$<
	sed '/^#/ s|$(LEX_OUTPUT_ROOT)\.c|$@|' $(LEX_OUTPUT_ROOT).c >$@
	rm -f $(LEX_OUTPUT_ROOT).c

.y.c:
	$(YACCCOMPILE) `test -f '$<' || echo '$(srcdir)/'`$<
	sed '/^#/ s|y\.tab\.c|$@|' y.tab.c >$@
	rm -f y.tab.c
	if test -f y.tab.h; then \
	  to=`echo "$*_H" | sed \
                -e 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/' \
                -e 's/[^ABCDEFGHIJKLMNOPQRSTUVWXYZ]/_/g'`; \
	  sed "/^#/ s/Y_TAB_H/$$to/g" y.tab.h >$*.ht; \
	  rm -f y.tab.h; \
	  if cmp -s $*.ht $*.h; then \
	    rm -f $*.ht ;\
	  else \
	    mv $*.ht $*.h; \
	  fi; \
	fi
	if test -f y.output; then \
	  mv y.output $*.output; \
	fi
uninstall-info-am:

ETAGS = etags
ETAGSFLAGS =

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$tags$$unique" \
	  || $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = ../..
distdir = $(top_distdir)/$(PACKAGE)-$(VERSION)

distdir: $(DISTFILES)
	@list='$(DISTFILES)'; for file in $$list; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkinstalldirs) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(PROGRAMS)

installdirs:
	$(mkinstalldirs) $(DESTDIR)$(bindir)

install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-test -z "soapcpp2_yacc.hsoapcpp2_yacc.csoapcpp2_lex.c" || rm -f soapcpp2_yacc.h soapcpp2_yacc.c soapcpp2_lex.c
clean: clean-am

clean-am: clean-binPROGRAMS clean-generic mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am:

install-exec-am: install-binPROGRAMS

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic

uninstall-am: uninstall-binPROGRAMS uninstall-info-am

.PHONY: GTAGS all all-am check check-am clean clean-binPROGRAMS \
	clean-generic distclean distclean-compile distclean-depend \
	distclean-generic distclean-tags distdir dvi dvi-am info \
	info-am install install-am install-binPROGRAMS install-data \
	install-data-am install-exec install-exec-am install-info \
	install-info-am install-man install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic tags uninstall uninstall-am \
	uninstall-binPROGRAMS uninstall-info-am

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
