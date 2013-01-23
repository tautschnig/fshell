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
						@CBMCDIR@/src/pointer-analysis/pointer-analysis.a \
						@CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/bigint.o \
						@CBMCDIR@/src/goto-symex/goto-symex.a \
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
