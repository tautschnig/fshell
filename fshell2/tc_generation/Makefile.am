# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = @CBMCDIR@/src/goto-programs/goto-programs.a \
						@CBMCDIR@/src/goto-instrument/dump_c.o \
						@CBMCDIR@/src/goto-instrument/goto_program2code.o \
						@CBMCDIR@/src/cpp/cpp.a \
						@CBMCDIR@/src/assembler/assembler.a \
						@CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/big-int.a \
						@CBMCDIR@/src/goto-symex/goto-symex.a \
						@CBMCDIR@/src/pointer-analysis/value_set.o \
						@CBMCDIR@/src/pointer-analysis/value_set_dereference.o \
						@CBMCDIR@/src/pointer-analysis/dereference_callback.o \
						@CBMCDIR@/src/pointer-analysis/add_failed_symbols.o \
						@CBMCDIR@/src/pointer-analysis/rewrite_index.o \
						@CBMCDIR@/src/pointer-analysis/goto_program_dereference.o \
						@CBMCDIR@/src/cbmc/bmc_cover.o \
						@CBMCDIR@/src/cbmc/bmc.o \
						@CBMCDIR@/src/cbmc/all_properties.o \
						@CBMCDIR@/src/xmllang/xmllang.a \
						@CBMCDIR@/src/cbmc/show_vcc.o \
						@CBMCDIR@/src/cbmc/cbmc_solvers.o \
						@CBMCDIR@/src/solvers/solvers.a \
						@CBMCDIR@/src/cbmc/cbmc_dimacs.o \
						@CBMCDIR@/src/cbmc/bv_cbmc.o \
						@CBMCDIR@/src/cbmc/counterexample_beautification.o \
						@CBMCDIR@/src/cbmc/symex_bmc.o \
						@CBMCDIR@/src/cbmc/fault_localization.o \
						@CBMCDIR@/src/langapi/langapi.a \
						@CBMCDIR@/src/ansi-c/ansi-c.a \
						@CBMCDIR@/src/linking/linking.a

MINISAT_OBJS = @MINISATDIR@/minisat/utils/System.o

noinst_LTLIBRARIES = libtc_generation.la
libtc_generation_la_SOURCES = constraint_strengthening.cpp test_suite_minimization.cpp test_suite_output.cpp
include_fshell2_tc_generationdir = $(includedir)/fshell2/tc_generation
include_fshell2_tc_generation_HEADERS = constraint_strengthening.hpp test_suite_output.hpp test_suite_minimization.hpp
check_PROGRAMS = constraint_strengthening test_suite_minimization test_suite_output
constraint_strengthening_SOURCES = constraint_strengthening.t.cpp
# add additional objects required to build constraint_strengthening here, like
# constraint_strengthening_LDADD = ../base/libbase.la libbla.la
constraint_strengthening_LDADD = libtc_generation.la ../fql/evaluation/libevaluation.la ../util/libutil.la ../fql/ast/libast.la ../fql/concepts/libconcepts.la ../instrumentation/libinstrumentation.la ../exception/libexception.la $(CBMC_OBJS) $(MINISAT_OBJS)
# note that this entry (constraint_strengthening_SOURCES, constraint_strengthening_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
test_suite_minimization_SOURCES = test_suite_minimization.t.cpp
# add additional objects required to build test_suite_minimization here, like
# test_suite_minimization_LDADD = ../base/libbase.la libbla.la
test_suite_minimization_LDADD = libtc_generation.la ../fql/evaluation/libevaluation.la ../util/libutil.la ../fql/ast/libast.la ../fql/concepts/libconcepts.la ../instrumentation/libinstrumentation.la ../exception/libexception.la $(CBMC_OBJS) $(MINISAT_OBJS)
# note that this entry (test_suite_minimization_SOURCES, test_suite_minimization_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
test_suite_output_SOURCES = test_suite_output.t.cpp
# add additional objects required to build test_suite_output here, like
# test_suite_output_LDADD = ../base/libbase.la libbla.la
test_suite_output_LDADD = libtc_generation.la ../fql/evaluation/libevaluation.la ../util/libutil.la ../fql/ast/libast.la ../fql/concepts/libconcepts.la ../instrumentation/libinstrumentation.la ../exception/libexception.la $(CBMC_OBJS) $(MINISAT_OBJS)
# note that this entry (test_suite_output_SOURCES, test_suite_output_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = constraint_strengthening test_suite_minimization test_suite_output
