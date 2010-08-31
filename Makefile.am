include $(top_srcdir)/model.am

include $(top_srcdir)/performance.am

AM_MAKEFLAGS = CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"

# add further subdirectories here
SUBDIRS = . minisat-2.2.0 test doc fshell2 benchmarks

# just a hint for autoproject
# _SOURCES = model.am performance.am ChangeLog NEWS README TODO

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/doxygen.am

AUX_DIST = $(ac_aux_dir)/config.guess $(ac_aux_dir)/config.sub \
					 $(ac_aux_dir)/install-sh $(ac_aux_dir)/ltmain.sh $(ac_aux_dir)/missing \
					 $(ac_aux_dir)/depcomp $(ac_aux_dir)/ylwrap \
					 m4/libtool.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4 m4/lt~obsolete.m4

EXTRA_DIST = $(DX_CONFIG) LICENSE NOTICE Artistic config/runtest.sh $(AUX_DIST) \
						 $(top_srcdir)/cbmc-src.tar.gz $(top_srcdir)/astl-src.tar.gz

MAINTAINERCLEANFILES += aclocal.m4 configure config.h.in $(AUX_DIST)
DISTCLEANFILES = config.h.in~ _configs.sed fshell2/config/features.hpp

clean-local:
	$(MAKE) $(AM_MAKEFLAGS) -C astl clean
	$(MAKE) $(AM_MAKEFLAGS) -C cbmc/src clean

distclean-local:
	$(MAKE) $(AM_MAKEFLAGS) -C astl distclean
	$(MAKE) $(AM_MAKEFLAGS) -C cbmc/src distclean

maintainer-clean-local:
	$(MAKE) $(AM_MAKEFLAGS) -C astl maintainer-clean
	$(MAKE) $(AM_MAKEFLAGS) -C cbmc/src maintainer-clean
	$(RM) -rf $(DX_DOCDIR)
	$(RM) -rf build/
	
check-local:
	$(MAKE) $(AM_MAKEFLAGS) -C astl check
				
all-local:
	if [ "$(top_srcdir)" = "." ] ; then \
		$(top_srcdir)/config/update-makefiles.pl @enable_install_headers@ @disable_update_makefiles@ fshell2/ ; \
	fi
	if [ ! -d astl/.git ] ; then \
		if [ ! -d astl ] ; then \
			tar xzf $(top_srcdir)/astl-src.tar.gz ; \
		elif [ -z "`find astl -cnewer $(top_srcdir)/astl-src.tar.gz -type f`" ] ; then \
			tar xzf $(top_srcdir)/astl-src.tar.gz ; \
			$(MAKE) $(AM_MAKEFLAGS) -C astl/ clean ; \
		fi ; \
	fi
	$(MAKE) $(AM_MAKEFLAGS) -C astl all
	if [ "$(top_srcdir)" != "." ] ; then \
		if [ ! -d benchmarks ] ; then \
		  mkdir benchmarks ; \
		  echo 'all install install-strip uninstall check installcheck dist clean distclean maintainer-clean:' > benchmarks/Makefile ; \
		  echo >> benchmarks/Makefile ; \
		  echo '.PHONY: all install install-strip uninstall check installcheck dist clean distclean maintainer-clean' >> benchmarks/Makefile ; \
		fi ; \
		mkdir -p minisat-2.2.0/{core,mtl,simp,utils} ; \
		cp -p $(top_srcdir)/minisat-2.2.0/Makefile minisat-2.2.0 ; \
		cp -p $(top_srcdir)/minisat-2.2.0/core/{Makefile,*.cc,*.h} minisat-2.2.0/core ; \
		cp -p $(top_srcdir)/minisat-2.2.0/mtl/{*.h,*.mk} minisat-2.2.0/mtl ; \
		cp -p $(top_srcdir)/minisat-2.2.0/simp/{Makefile,*.cc,*.h} minisat-2.2.0/simp ; \
		cp -p $(top_srcdir)/minisat-2.2.0/utils/{Makefile,*.cc,*.h} minisat-2.2.0/utils ; \
	fi
	$(MAKE) $(AM_MAKEFLAGS) -C minisat-2.2.0
	if [ ! -d cbmc/.git ] ; then \
		if [ ! -d cbmc ] ; then \
			tar xzf $(top_srcdir)/cbmc-src.tar.gz ; \
		elif [ -z "`find cbmc -cnewer $(top_srcdir)/cbmc-src.tar.gz -type f`" ] ; then \
			tar xzf $(top_srcdir)/cbmc-src.tar.gz ; \
			$(MAKE) $(AM_MAKEFLAGS) -C cbmc/src/ clean ; \
		fi ; \
	fi
	$(MAKE) $(filter-out CXXFLAGS="$(CXXFLAGS)", $(AM_MAKEFLAGS)) CXXFLAGS="$(CXXFLAGS) -Wno-parentheses -Wno-sign-compare" -C cbmc/src

cbmc-src.tar.gz: $(shell find cbmc -name "*.cpp" -o -name "*.h")
	rm -f $@
	make -C cbmc/src/ clean
	tar czf $@ --exclude .git cbmc/src/

astl-src.tar.gz: $(shell find astl -name "*.cpp" -o -name "*.cc" -o -name "*.h")
	rm -f $@
	make -C astl/ clean
	tar czf $@ --exclude .git astl/

