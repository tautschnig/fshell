# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = $(top_builddir)/cbmc/src/goto-programs/goto-programs.a \
						$(top_builddir)/cbmc/src/pointer-analysis/pointer-analysis.a \
						$(top_builddir)/cbmc/src/util/util.a \
						$(top_builddir)/cbmc/src/big-int/bigint.o \
						$(top_builddir)/cbmc/src/goto-symex/goto-symex.a \
						$(top_builddir)/cbmc/src/cbmc/cover.o \
						$(top_builddir)/cbmc/src/cbmc/bmc.o \
						$(top_builddir)/cbmc/src/cbmc/show_vcc.o \
						$(top_builddir)/cbmc/src/cbmc/cbmc_solvers.o \
						$(top_builddir)/cbmc/src/solvers/solvers.a \
						$(top_builddir)/cbmc/src/cbmc/dimacs.o \
						$(top_builddir)/cbmc/src/cbmc/bv_cbmc.o \
						$(top_builddir)/cbmc/src/cbmc/counterexample_beautification_greedy.o \
						$(top_builddir)/cbmc/src/cbmc/counterexample_beautification.o \
						$(top_builddir)/cbmc/src/cbmc/symex_bmc.o \
						$(top_builddir)/cbmc/src/langapi/langapi.a \
						$(top_builddir)/cbmc/src/ansi-c/ansi-c.a

include_fshell2_instrumentationdir = $(includedir)/fshell2/instrumentation
noinst_LTLIBRARIES = libinstrumentation.la
libinstrumentation_la_SOURCES = goto_transformation.cpp cfg.cpp goto_utils.cpp goto_lt_compare.cpp
include_fshell2_instrumentation_HEADERS = goto_transformation.hpp cfg.hpp goto_utils.hpp goto_lt_compare.hpp
check_PROGRAMS = goto_transformation goto_lt_compare
goto_transformation_SOURCES = goto_transformation.t.cpp
# add additional objects required to build goto_transformation here, like
# goto_transformation_LDADD = ../base/libbase.la libbla.la
goto_transformation_LDADD = libinstrumentation.la ../exception/libexception.la $(CBMC_OBJS)
# note that this entry (goto_transformation_SOURCES, goto_transformation_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = goto_transformation goto_lt_compare
goto_lt_compare_SOURCES = goto_lt_compare.t.cpp
# add additional objects required to build goto_lt_compare here, like
# goto_lt_compare_LDADD = ../base/libbase.la libbla.la
goto_lt_compare_LDADD = libinstrumentation.la $(CBMC_OBJS)
# note that this entry (goto_lt_compare_SOURCES, goto_lt_compare_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
