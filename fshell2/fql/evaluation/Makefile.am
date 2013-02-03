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
						@CBMCDIR@/src/big-int/bigint.o \
						@CBMCDIR@/src/goto-symex/goto-symex.a \
						@CBMCDIR@/src/pointer-analysis/value_set.o \
						@CBMCDIR@/src/pointer-analysis/dereference.o \
						@CBMCDIR@/src/pointer-analysis/add_failed_symbols.o \
						@CBMCDIR@/src/pointer-analysis/rewrite_index.o \
						@CBMCDIR@/src/pointer-analysis/goto_program_dereference.o \
						@CBMCDIR@/src/cbmc/cover.o \
						@CBMCDIR@/src/cbmc/bmc.o \
						@CBMCDIR@/src/cbmc/show_vcc.o \
						@CBMCDIR@/src/cbmc/cbmc_solvers.o \
						@CBMCDIR@/src/solvers/solvers.a \
						@CBMCDIR@/src/cbmc/dimacs.o \
						@CBMCDIR@/src/cbmc/bv_cbmc.o \
						@CBMCDIR@/src/cbmc/counterexample_beautification.o \
						@CBMCDIR@/src/cbmc/symex_bmc.o \
						@CBMCDIR@/src/langapi/langapi.a \
						@CBMCDIR@/src/ansi-c/ansi-c.a \
						@CBMCDIR@/src/linking/linking.a

MINISAT_OBJS = @MINISATDIR@/utils/System.o

noinst_LTLIBRARIES = libevaluation.la
libevaluation_la_SOURCES =  evaluate_filter.cpp predicate_instrumentation.cpp evaluate_path_pattern.cpp evaluate_coverage_pattern.cpp automaton_inserter.cpp compute_test_goals.cpp
include_fshell2_fql_evaluationdir = $(includedir)/fshell2/fql/evaluation
include_fshell2_fql_evaluation_HEADERS =  evaluate_filter.hpp evaluate_path_pattern.hpp automaton_inserter.hpp compute_test_goals.hpp evaluate_coverage_pattern.hpp predicate_instrumentation.hpp
check_PROGRAMS =  evaluate_filter predicate_instrumentation evaluate_path_pattern evaluate_coverage_pattern automaton_inserter compute_test_goals
evaluate_filter_SOURCES = evaluate_filter.t.cpp
# add additional objects required to build evaluate_filter here, like
# evaluate_filter_LDADD = ../base/libbase.la libbla.la
evaluate_filter_LDADD = libevaluation.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (evaluate_filter_SOURCES, evaluate_filter_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
compute_test_goals_SOURCES = compute_test_goals.t.cpp
# add additional objects required to build compute_test_goals here, like
# compute_test_goals_LDADD = ../base/libbase.la libbla.la
compute_test_goals_LDADD = libevaluation.la ../../util/libutil.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS) $(MINISAT_OBJS)
# note that this entry (compute_test_goals_SOURCES, compute_test_goals_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
automaton_inserter_SOURCES = automaton_inserter.t.cpp
# add additional objects required to build automaton_inserter here, like
# automaton_inserter_LDADD = ../base/libbase.la libbla.la
automaton_inserter_LDADD = libevaluation.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (automaton_inserter_SOURCES, automaton_inserter_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
evaluate_path_pattern_SOURCES = evaluate_path_pattern.t.cpp
# add additional objects required to build evaluate_path_pattern here, like
# evaluate_path_pattern_LDADD = ../base/libbase.la libbla.la
evaluate_path_pattern_LDADD = libevaluation.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (target_graph_SOURCES, target_graph_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
evaluate_coverage_pattern_SOURCES = evaluate_coverage_pattern.t.cpp
# add additional objects required to build evaluate_coverage_pattern here, like
# evaluate_coverage_pattern_LDADD = ../base/libbase.la libbla.la
evaluate_coverage_pattern_LDADD = libevaluation.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (evaluate_coverage_pattern_SOURCES, evaluate_coverage_pattern_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
predicate_instrumentation_SOURCES = predicate_instrumentation.t.cpp
# add additional objects required to build predicate_instrumentation here, like
# predicate_instrumentation_LDADD = ../base/libbase.la libbla.la
predicate_instrumentation_LDADD = libevaluation.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (predicate_instrumentation_SOURCES, predicate_instrumentation_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = automaton_inserter evaluate_coverage_pattern compute_test_goals evaluate_filter evaluate_path_pattern predicate_instrumentation
