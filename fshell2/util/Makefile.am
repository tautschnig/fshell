# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = @CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/big-int.a

MINISAT_OBJS = @MINISATDIR@/minisat/utils/System.o

include_fshell2_utildir = $(includedir)/fshell2/util
noinst_LTLIBRARIES = libutil.la
libutil_la_SOURCES = statistics.cpp smart_printer.cpp
include_fshell2_util_HEADERS = statistics.hpp smart_printer.hpp
TESTS = statistics
check_PROGRAMS = statistics
statistics_SOURCES = statistics.t.cpp
# add additional objects required to build statistics here, like
# statistics_LDADD = ../base/libbase.la libbla.la
statistics_LDADD = libutil.la $(CBMC_OBJS) $(MINISAT_OBJS)
# note that this entry (statistics_SOURCES, statistics_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
