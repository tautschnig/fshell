include $(top_srcdir)/model.am

include $(top_srcdir)/performance.am

AM_MAKEFLAGS = CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"

# add further subdirectories here
SUBDIRS = . test doc fshell2 benchmarks

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
	[ ! -d astl ] || $(MAKE) $(AM_MAKEFLAGS) -C astl clean
	[ ! -d cbmc ] || $(MAKE) $(AM_MAKEFLAGS) -C cbmc/src clean
if BUILD_LINENOISE
	$(MAKE) $(AM_MAKEFLAGS) -C linenoise clean
endif

distclean-local:
	[ ! -d astl ] || $(MAKE) $(AM_MAKEFLAGS) -C astl distclean
	[ ! -d cbmc ] || $(MAKE) $(AM_MAKEFLAGS) -C cbmc/src clean
if BUILD_LINENOISE
	$(MAKE) $(AM_MAKEFLAGS) -C linenoise distclean
endif

maintainer-clean-local:
	[ ! -d astl ] || $(MAKE) $(AM_MAKEFLAGS) -C astl maintainer-clean
	[ ! -d cbmc ] || $(MAKE) $(AM_MAKEFLAGS) -C cbmc/src clean
if BUILD_LINENOISE
	$(MAKE) $(AM_MAKEFLAGS) -C linenoise maintainer-clean
endif
	$(RM) -rf $(DX_DOCDIR)
	$(RM) -rf build/
	$(RM) -rf examples/
	
check-local:
	if ! test x$(build_os) = xmingw32 ; then \
		$(MAKE) $(AM_MAKEFLAGS) -C astl check ; \
	fi

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
		if [ '$$(top_srcdir)/minisat-2.2.0' = "@MINISATDIR@" ] ; then \
			mkdir -p minisat-2.2.0/{core,mtl,simp,utils} ; \
			cp -p @MINISATDIR@/Makefile minisat-2.2.0 ; \
			cp -p @MINISATDIR@/core/{Makefile,*.cc,*.h} minisat-2.2.0/core ; \
			cp -p @MINISATDIR@/mtl/{*.h,*.mk} minisat-2.2.0/mtl ; \
			cp -p @MINISATDIR@/simp/{Makefile,*.cc,*.h} minisat-2.2.0/simp ; \
			cp -p @MINISATDIR@/utils/{Makefile,*.cc,*.h} minisat-2.2.0/utils ; \
		fi ; \
		mkdir -p linenoise ; \
		cp -p $(top_srcdir)/linenoise/{Makefile,linenoise.c,linenoise.h} linenoise ; \
	fi
	if [ '$$(top_srcdir)/minisat-2.2.0' = "@MINISATDIR@" ] ; then \
		$(MAKE) $(AM_MAKEFLAGS) -C minisat-2.2.0 ; \
	fi
	if [ '$$(top_srcdir)/cbmc' = "@CBMCDIR@" ] ; then \
		if [ ! -d cbmc/.svn ] ; then \
			if [ ! -d cbmc ] ; then \
				tar xzf $(top_srcdir)/cbmc-src.tar.gz ; \
			elif [ -z "`find cbmc -cnewer $(top_srcdir)/cbmc-src.tar.gz -type f`" ] ; then \
				tar xzf $(top_srcdir)/cbmc-src.tar.gz ; \
				$(MAKE) $(AM_MAKEFLAGS) -C cbmc/src/ clean ; \
			fi ; \
		fi
		$(MAKE) $(filter-out CXXFLAGS="$(CXXFLAGS)", $(AM_MAKEFLAGS)) \
			CXXFLAGS="$(CXXFLAGS) -Wno-long-long -Wno-sign-compare -Wno-parentheses" \
			MAKEOVERRIDES="MINISAT2=../../../minisat-2.2.0/" \
			-C cbmc/src cbmc.dir ; \
	fi
if BUILD_LINENOISE
	$(MAKE) $(AM_MAKEFLAGS) -C linenoise CC="$(CC)" CFLAGS="$(CXXFLAGS)"
endif

cbmc-src.tar.gz: $(shell find cbmc -name "*.cpp" -o -name "*.h")
	rm -f $@
	make -C cbmc/src/ clean
	tar czf $@ cbmc/src/

astl-src.tar.gz: $(shell find astl -name "*.cpp" -o -name "*.cc" -o -name "*.h")
	rm -f $@
	make -C astl/ clean
	tar czf $@ --exclude .git astl/

examples:
	mkdir -p examples
	cd $(top_srcdir)/benchmarks/code ; tar xjf ase-examples.tar.bz2
	cp $(top_srcdir)/benchmarks/code/ase-examples/*.c examples/
	echo "Queries described in our ASE'10 paper" > examples/ase-queries.txt
	echo >> examples/ase-queries.txt
	for c in $(top_srcdir)/benchmarks/conf/ase-examples* ; do \
		q="`echo $$c | cut -f3 -d_`" ; \
		echo "Query $$q:" >> examples/ase-queries.txt ; \
		( . $$c ; echo "Use fshell $$extra_params" | \
			sed 's# code/ase-examples# examples#g' >> examples/ase-queries.txt ; \
			echo $$query >> examples/ase-queries.txt ) ; \
		echo >> examples/ase-queries.txt ; \
	done

if WIN32_BUILD
release: CXXFLAGS += -Wno-deprecated -Wno-long-long
else
release: CXXFLAGS := $(filter-out -L%,$(CXXFLAGS)) -Wno-deprecated -Wno-long-long
endif
release-local: release
	$(MAKE) -C $(top_srcdir)/build/release examples
	$(MAKE) -C $(top_srcdir)/build/release/fshell2/main \
		install-binPROGRAMS bindir=$$PWD/$(top_srcdir)/build/release/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION)
	distdir=$(PACKAGE_TARNAME)-$(PACKAGE_VERSION) ; \
	cd $(top_srcdir)/build/release && \
	cp ../../{LICENSE,NOTICE,LGPL,THANKS,ChangeLog} $$distdir && \
	cp -a examples $$distdir && \
	if test x$(build_os) = xmingw32 ; then \
		cp /usr/bin/libgcc_s_dw2-1.dll $$distdir ; \
		zip -r -9 -x.svn -x.git $$distdir-$(build_cpu)-$(build_os).zip $$distdir ; \
	else \
		tar czf $$distdir-$(build_cpu)-$(build_os).tar.gz --exclude .git --exclude .svn $$distdir ; \
	fi

update-changelog:
	[ -d .git ]
	lastmod="`git log --pretty=format:%H ChangeLog | head -n1`" ; \
	git log --date-order --date=short $$lastmod..HEAD \
		--pretty="format:%ad  %an  <%ae>%n%n%w(80,2,2)  * %s%n" \
		> ChangeLog.new
	mv ChangeLog ChangeLog.old
	mv ChangeLog.new ChangeLog
	echo >> ChangeLog
	cat ChangeLog.old >> ChangeLog
	rm ChangeLog.old

