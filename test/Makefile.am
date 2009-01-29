MAINTAINERCLEANFILES = Makefile.in

noinst_HEADERS = config.hpp

check-local:
	@cd $(top_srcdir) ; \
	if [ -x test/check_files ] ; then \
		runflag='' ; \
		for f in x $$MAKEFLAGS; do \
			case $$f in \
				*=* | --[!k]*);; \
				*k*) runflag='-k';; \
			esac; \
		done; \
		echo "Running test/check_files $$runflag ." ; \
		test/check_files $$runflag . ; \
	fi

