# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = @CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/big-int.a \
						@CBMCDIR@/src/ansi-c/ansi-c.a \
						@CBMCDIR@/src/langapi/langapi.a \
						@CBMCDIR@/src/linking/linking.a

include_fshell2_fql_normalizedir = $(includedir)/fshell2/fql/normalize
noinst_LTLIBRARIES = libnormalize.la
libnormalize_la_SOURCES = normalization_visitor.cpp
include_fshell2_fql_normalize_HEADERS = normalization_visitor.hpp
check_PROGRAMS = normalization_visitor
normalization_visitor_SOURCES = normalization_visitor.t.cpp
# add additional objects required to build normalization_visitor here, like
# normalization_visitor_LDADD = ../base/libbase.la libbla.la
normalization_visitor_LDADD = libnormalize.la ../ast/libast.la $(CBMC_OBJS)
# note that this entry (normalization_visitor_SOURCES, normalization_visitor_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = normalization_visitor
