# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = @CBMCDIR@/src/goto-programs/goto-programs.a \
						@CBMCDIR@/src/assembler/assembler.a \
						@CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/big-int.a \
						@CBMCDIR@/src/langapi/langapi.a \
						@CBMCDIR@/src/ansi-c/ansi-c.a \
						@CBMCDIR@/src/linking/linking.a

noinst_LTLIBRARIES = libconcepts.la
libconcepts_la_SOURCES = cfa.cpp    trace_automaton.cpp
include_fshell2_fql_conceptsdir = $(includedir)/fshell2/fql/concepts
include_fshell2_fql_concepts_HEADERS =  cfa.hpp   target_graph.hpp trace_automaton.hpp test_goal.hpp
check_PROGRAMS = cfa trace_automaton
cfa_SOURCES = cfa.t.cpp
# add additional objects required to build cfa here, like
# cfa_LDADD = ../base/libbase.la libbla.la
cfa_LDADD = libconcepts.la ../../instrumentation/libinstrumentation.la $(CBMC_OBJS)
# note that this entry (cfa_SOURCES, cfa_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = cfa trace_automaton
trace_automaton_SOURCES = trace_automaton.t.cpp
# add additional objects required to build trace_automaton here, like
# trace_automaton_LDADD = ../base/libbase.la libbla.la
trace_automaton_LDADD = libconcepts.la ../../exception/libexception.la
# note that this entry (trace_automaton_SOURCES, trace_automaton_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
