# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = $(top_builddir)/cbmc/src/goto-programs/goto-programs.o \
						$(top_builddir)/cbmc/src/pointer-analysis/pointer-analysis.o \
						$(top_builddir)/cbmc/src/util/util.o \
						$(top_builddir)/cbmc/src/big-int/bigint.o \
						$(top_builddir)/cbmc/src/goto-symex/goto-symex.o \
						$(top_builddir)/cbmc/src/cbmc/bmc.o \
						$(top_builddir)/cbmc/src/cbmc/show_vcc.o \
						$(top_builddir)/cbmc/src/cbmc/cbmc_solvers.o \
						$(top_builddir)/cbmc/src/solvers/solvers.o \
						$(top_builddir)/cbmc/src/cbmc/dimacs.o \
						$(top_builddir)/cbmc/src/cbmc/bv_cbmc.o \
						$(top_builddir)/cbmc/src/cbmc/counterexample_beautification_greedy.o \
						$(top_builddir)/cbmc/src/cbmc/counterexample_beautification.o \
						$(top_builddir)/cbmc/src/cbmc/symex_bmc.o \
						$(top_builddir)/cbmc/src/langapi/langapi.o \
						$(top_builddir)/cbmc/src/ansi-c/ansi-c.o

$(CBMC_OBJS):
	-$(MAKE) -C $(top_builddir)/cbmc/src

$(top_builddir)/cbmc/src/util/irep_ids.h:
	$(MAKE) -C $(top_builddir)/cbmc/src/util irep_ids.h

noinst_LTLIBRARIES = libtc_generation.la
libtc_generation_la_SOURCES = constraint_strengthening.cpp test_suite_minimization.cpp test_suite_output.cpp
include_fshell2_tc_generationdir = $(includedir)/fshell2/tc_generation
include_fshell2_tc_generation_HEADERS = constraint_strengthening.hpp test_suite_output.hpp test_suite_minimization.hpp
check_PROGRAMS = constraint_strengthening test_suite_minimization test_suite_output
constraint_strengthening_SOURCES = constraint_strengthening.t.cpp
# add additional objects required to build constraint_strengthening here, like
# constraint_strengthening_LDADD = ../base/libbase.la libbla.la
constraint_strengthening_LDADD = libtc_generation.la ../fql/evaluation/libevaluation.la ../util/libutil.la ../fql/ast/libast.la ../fql/concepts/libconcepts.la ../instrumentation/libinstrumentation.la ../exception/libexception.la $(CBMC_OBJS)
# note that this entry (constraint_strengthening_SOURCES, constraint_strengthening_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
test_suite_minimization_SOURCES = test_suite_minimization.t.cpp
# add additional objects required to build test_suite_minimization here, like
# test_suite_minimization_LDADD = ../base/libbase.la libbla.la
test_suite_minimization_LDADD = libtc_generation.la ../fql/evaluation/libevaluation.la ../util/libutil.la ../fql/ast/libast.la ../fql/concepts/libconcepts.la ../instrumentation/libinstrumentation.la ../exception/libexception.la $(CBMC_OBJS)
# note that this entry (test_suite_minimization_SOURCES, test_suite_minimization_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
test_suite_output_SOURCES = test_suite_output.t.cpp
# add additional objects required to build test_suite_output here, like
# test_suite_output_LDADD = ../base/libbase.la libbla.la
test_suite_output_LDADD = libtc_generation.la ../fql/evaluation/libevaluation.la ../util/libutil.la ../fql/ast/libast.la ../fql/concepts/libconcepts.la ../instrumentation/libinstrumentation.la ../exception/libexception.la $(CBMC_OBJS)
# note that this entry (test_suite_output_SOURCES, test_suite_output_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = constraint_strengthening test_suite_minimization test_suite_output
