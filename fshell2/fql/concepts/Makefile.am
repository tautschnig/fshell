# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

noinst_LTLIBRARIES = libconcepts.la
libconcepts_la_SOURCES = cfa.cpp path_predicate.cpp path_set_predicate.cpp state_predicate.cpp
include_fshell2_fql_conceptsdir = $(includedir)/fshell2/fql/concepts
include_fshell2_fql_concepts_HEADERS = path_set_predicate.hpp cfa.hpp state_predicate.hpp path_predicate.hpp
check_PROGRAMS = cfa
cfa_SOURCES = cfa.t.cpp
# add additional objects required to build cfa here, like
# cfa_LDADD = ../base/libbase.la libbla.la
cfa_LDADD = libconcepts.la
# note that this entry (cfa_SOURCES, cfa_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually