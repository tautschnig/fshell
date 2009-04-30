# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = ../../../cbmc/src/goto-programs/goto-programs.o \
						../../../cbmc/src/pointer-analysis/pointer-analysis.o \
						../../../cbmc/src/util/util.o ../../../cbmc/src/big-int/bigint.o \
						../../../cbmc/src/langapi/langapi.o \
						../../../cbmc/src/ansi-c/ansi-c.o \
						../../../cbmc/src/langapi/csp_dummy.o \
						../../../cbmc/src/langapi/cpp_dummy.o \
						../../../cbmc/src/langapi/specc_dummy.o \
						../../../cbmc/src/langapi/php_dummy.o \
						../../../cbmc/src/langapi/intrep_dummy.o \
						../../../cbmc/src/langapi/pvs_dummy.o \
						../../../cbmc/src/langapi/smvlang_dummy.o \
						../../../cbmc/src/langapi/promela_dummy.o \
						../../../cbmc/src/langapi/verilog_dummy.o \
						../../../cbmc/src/langapi/vhdl_dummy.o \
						../../../cbmc/src/langapi/smtlang_dummy.o \
						../../../cbmc/src/langapi/xmllang_dummy.o \
						../../../cbmc/src/langapi/netlist_dummy.o \
						../../../cbmc/src/langapi/pascal_dummy.o \
						../../../cbmc/src/langapi/bplang_dummy.o \
						../../../cbmc/src/langapi/simplifylang_dummy.o \
						../../../cbmc/src/langapi/csharp_dummy.o \
						../../../cbmc/src/langapi/cvclang_dummy.o \
						../../../cbmc/src/langapi/nsf_dummy.o \
						../../../cbmc/src/langapi/mdllang_dummy.o

$(CBMC_OBJS):
	$(MAKE) -C ../../../cbmc/src util.dir big-int.dir goto-programs.dir \
		pointer-analysis.dir langapi.dir ansi-c.dir

noinst_LTLIBRARIES = libevaluation.la
libevaluation_la_SOURCES = evaluate_filter.cpp
include_fshell2_fql_evaluationdir = $(includedir)/fshell2/fql/evaluation
include_fshell2_fql_evaluation_HEADERS = evaluate_filter.hpp
check_PROGRAMS = evaluate_filter
evaluate_filter_SOURCES = evaluate_filter.t.cpp
# add additional objects required to build evaluate_filter here, like
# evaluate_filter_LDADD = ../base/libbase.la libbla.la
evaluate_filter_LDADD = libevaluation.la ../ast/libast.la $(CBMC_OBJS)
# note that this entry (evaluate_filter_SOURCES, evaluate_filter_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
