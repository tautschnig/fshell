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
						$(top_builddir)/cbmc/src/cbmc/document_subgoals.o \
						$(top_builddir)/cbmc/src/cbmc/counterexample_beautification_greedy.o \
						$(top_builddir)/cbmc/src/cbmc/counterexample_beautification.o \
						$(top_builddir)/cbmc/src/cbmc/symex_bmc.o \
						$(top_builddir)/cbmc/src/langapi/langapi.o \
						$(top_builddir)/cbmc/src/ansi-c/ansi-c.o

$(CBMC_OBJS):
	-$(MAKE) -C $(top_builddir)/cbmc/src

noinst_LTLIBRARIES = libevaluation.la
libevaluation_la_SOURCES =  evaluate_filter.cpp predicate_instrumentation.cpp evaluate_path_monitor.cpp build_test_goal_automaton.cpp automaton_inserter.cpp compute_test_goals.cpp
include_fshell2_fql_evaluationdir = $(includedir)/fshell2/fql/evaluation
include_fshell2_fql_evaluation_HEADERS =  evaluate_filter.hpp evaluate_path_monitor.hpp automaton_inserter.hpp compute_test_goals.hpp build_test_goal_automaton.hpp predicate_instrumentation.hpp
check_PROGRAMS =  evaluate_filter predicate_instrumentation evaluate_path_monitor build_test_goal_automaton automaton_inserter compute_test_goals
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
compute_test_goals_LDADD = libevaluation.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (compute_test_goals_SOURCES, compute_test_goals_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
automaton_inserter_SOURCES = automaton_inserter.t.cpp
# add additional objects required to build automaton_inserter here, like
# automaton_inserter_LDADD = ../base/libbase.la libbla.la
automaton_inserter_LDADD =
# note that this entry (automaton_inserter_SOURCES, automaton_inserter_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
evaluate_path_monitor_SOURCES = evaluate_path_monitor.t.cpp
# add additional objects required to build evaluate_path_monitor here, like
# evaluate_path_monitor_LDADD = ../base/libbase.la libbla.la
evaluate_path_monitor_LDADD = libevaluation.la ../ast/libast.la ../concepts/libconcepts.la ../../instrumentation/libinstrumentation.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (target_graph_SOURCES, target_graph_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
build_test_goal_automaton_SOURCES = build_test_goal_automaton.t.cpp
# add additional objects required to build build_test_goal_automaton here, like
# build_test_goal_automaton_LDADD = ../base/libbase.la libbla.la
build_test_goal_automaton_LDADD =
# note that this entry (build_test_goal_automaton_SOURCES, build_test_goal_automaton_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
predicate_instrumentation_SOURCES = predicate_instrumentation.t.cpp
# add additional objects required to build predicate_instrumentation here, like
# predicate_instrumentation_LDADD = ../base/libbase.la libbla.la
predicate_instrumentation_LDADD =
# note that this entry (predicate_instrumentation_SOURCES, predicate_instrumentation_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
