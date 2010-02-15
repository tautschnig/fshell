# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

AM_CPPFLAGS += -DCPP_CMD="\"@CPP@\""

CBMC_OBJS = $(top_builddir)/cbmc/src/util/util.o \
						$(top_builddir)/cbmc/src/big-int/bigint.o

$(CBMC_OBJS):
	$(MAKE) -C $(top_builddir)/cbmc/src util.dir big-int.dir

noinst_LTLIBRARIES = libmacro.la
libmacro_la_SOURCES = macro_processing.cpp
include_fshell2_macrodir = $(includedir)/fshell2/macro
include_fshell2_macro_HEADERS = macro_processing.hpp
check_PROGRAMS = macro_processing
macro_processing_SOURCES = macro_processing.t.cpp
# add additional objects required to build macro_processing here, like
# macro_processing_LDADD = ../base/libbase.la libbla.la
macro_processing_LDADD = libmacro.la ../exception/libexception.la $(CBMC_OBJS)
# note that this entry (macro_processing_SOURCES, macro_processing_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = macro_processing
