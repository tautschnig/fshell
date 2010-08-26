include $(top_srcdir)/model.am

include $(top_srcdir)/performance.am

AM_MAKEFLAGS = CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"

# add further subdirectories here
SUBDIRS = . test doc minisat-2.2.0 fshell2 cbmc/src

# just a hint for autoproject
# _SOURCES = model.am ChangeLog NEWS README TODO

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/doxygen.am

AUX_DIST = $(ac_aux_dir)/config.guess $(ac_aux_dir)/config.sub $(ac_aux_dir)/install-sh $(ac_aux_dir)/ltmain.sh $(ac_aux_dir)/missing $(ac_aux_dir)/depcomp $(ac_aux_dir)/ylwrap m4/libtool.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4 m4/lt~obsolete.m4

EXTRA_DIST = $(DX_CONFIG) LICENSE NOTICE Artistic config/runtest.sh

MAINTAINERCLEANFILES += aclocal.m4 configure config.h.in $(AUX_DIST)
DISTCLEANFILES = config.h.in~ _configs.sed fshell2/config/features.hpp

maintainer-clean-local:
	$(RM) -rf $(DX_DOCDIR)
	$(RM) -rf build/
				
dist-hook:
	(cd $(distdir) && mkdir -p $(ac_aux_dir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA) ; do cp $$file $(distdir)/$$file ; done

check-local:
	$(MAKE) -C $(top_srcdir)/astl check

all-local:
	if [ ! -d astl/.git ] ; then \
		if [ ! -d astl ] ; then \
			tar xzf $(top_srcdir)/astl-src.tar.gz ; \
		elif [ -z "`find astl -cnewer $(top_srcdir)/astl-src.tar.gz -type f`" ] ; then \
			tar xzf $(top_srcdir)/astl-src.tar.gz ; \
			$(MAKE) -C astl/ clean ; \
		fi ; \
	fi
	$(MAKE) -C $(top_srcdir)/astl
	if [ "$(top_srcdir)" = "." ] ; then \
		$(top_srcdir)/config/update-makefiles.pl @enable_install_headers@ @disable_update_makefiles@ fshell2/ ; \
	fi
	if [ ! -d cbmc/.git ] ; then \
		if [ ! -d cbmc ] ; then \
			tar xzf $(top_srcdir)/cbmc-src.tar.gz ; \
		elif [ -z "`find cbmc -cnewer $(top_srcdir)/cbmc-src.tar.gz -type f`" ] ; then \
			tar xzf $(top_srcdir)/cbmc-src.tar.gz ; \
			$(MAKE) -C cbmc/src/ clean ; \
		fi ; \
	fi
	$(MAKE) -C $(top_builddir)/cbmc/src/util irep_ids.h
	if [ "$(top_srcdir)" != "." ] ; then \
		mkdir -p minisat-2.2.0/{core,mtl,simp,utils} ; \
		cp -p $(top_srcdir)/minisat-2.2.0/Makefile minisat-2.2.0 ; \
		cp -p $(top_srcdir)/minisat-2.2.0/core/{Makefile,*.cc,*.h} minisat-2.2.0/core ; \
		cp -p $(top_srcdir)/minisat-2.2.0/mtl/{*.h,*.mk} minisat-2.2.0/mtl ; \
		cp -p $(top_srcdir)/minisat-2.2.0/simp/{Makefile,*.cc,*.h} minisat-2.2.0/simp ; \
		cp -p $(top_srcdir)/minisat-2.2.0/utils/{Makefile,*.cc,*.h} minisat-2.2.0/utils ; \
	fi

cbmc-src.tar.gz: $(shell find cbmc -name "*.cpp" -o -name "*.h")
	rm -f $@
	make -C cbmc/src/ clean
	tar czf $@ --exclude .git cbmc/src/

astl-src.tar.gz: $(shell find astl -name "*.cpp" -o -name "*.cc" -o -name "*.h")
	rm -f $@
	make -C astl/ clean
	tar czf $@ --exclude .git astl/

