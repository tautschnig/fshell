include $(top_srcdir)/model.am

# add further subdirectories here
SUBDIRS = . test doc

# just a hint for autoproject
# _SOURCES = model.am ChangeLog NEWS README TODO

ACLOCAL_AMFLAGS = -I m4

include $(top_srcdir)/doxygen.am

AUX_DIST = $(ac_aux_dir)/config.guess $(ac_aux_dir)/config.sub $(ac_aux_dir)/install-sh $(ac_aux_dir)/ltmain.sh $(ac_aux_dir)/missing $(ac_aux_dir)/depcomp $(ac_aux_dir)/ylwrap 

EXTRA_DIST = $(DX_CONFIG) TODO

MAINTAINERCLEANFILES += aclocal.m4 configure config.h.in $(AUX_DIST)
# add features.h here, like 
# DISTCLEANFILES = config.h.in~ _configs.sed shared/config/features.h
DISTCLEANFILES = config.h.in~ _configs.sed 

maintainer-clean-local:
	-[ -d external/diagnostics ] && (cd external/diagnostics && make maintainer-clean)
	rm -rf external/lib external/bin external/include
	$(RM) -rf $(DX_DOCDIR)
	$(RM) -rf build/
				
dist-hook:
	(cd $(distdir) && mkdir -p $(ac_aux_dir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA) ; do cp $$file $(distdir)/$$file ; done

# add the source directories here, like
# $(top_srcdir)/config/update-makefiles.pl @enable_install_headers@ @disable_update_makefiles@ @with_diagnostics@ main/ shared/
all-local:
	if [ "$(top_srcdir)" = "." ] ; then \
		$(top_srcdir)/config/update-makefiles.pl @enable_install_headers@ @disable_update_makefiles@ @with_diagnostics@ ; \
	fi

