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

include_fshell2_maindir = $(includedir)/fshell2/main
noinst_LTLIBRARIES = libmain.la
libmain_la_SOURCES = fshell2.cpp
include_fshell2_main_HEADERS = fshell2.hpp
check_PROGRAMS = fshell2
fshell2_SOURCES = fshell2.t.cpp
# add additional objects required to build fshell2 here, like
# fshell2_LDADD = ../base/libbase.la libbla.la
fshell2_LDADD = libmain.la \
								../command/libcommand.la \
								../macro/libmacro.la \
								../tc_generation/libtc_generation.la \
								../fql/evaluation/libevaluation.la \
								../fql/normalize/libnormalize.la \
								../fql/parser/libparser.la \
								../fql/ast/libast.la \
								../instrumentation/libinstrumentation.la \
								../exception/libexception.la \
								$(CBMC_OBJS)
# note that this entry (fshell2_SOURCES, fshell2_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
