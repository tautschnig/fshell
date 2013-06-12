# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

# NO_UPDATE_MAKEFILES
bin_PROGRAMS = fshell
fshell_SOURCES = main.cpp
fshell_LDADD = libmain.la \
							  ../util/libutil.la \
								../command/libcommand.la \
								../macro/libmacro.la \
								../tc_generation/libtc_generation.la \
								../fql/evaluation/libevaluation.la \
								../fql/normalize/libnormalize.la \
								../fql/parser/libparser.la \
								../fql/ast/libast.la \
								../fql/concepts/libconcepts.la \
								../instrumentation/libinstrumentation.la \
								../exception/libexception.la \
								$(LINENOISE_OBJS) $(CBMC_OBJS) $(MINISAT_OBJS)

CBMC_OBJS = @CBMCDIR@/src/goto-programs/goto-programs.a \
						@CBMCDIR@/src/goto-instrument/dump_c.o \
						@CBMCDIR@/src/cpp/cpp.a \
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
						@CBMCDIR@/src/cbmc/all_claims.o \
						@CBMCDIR@/src/langapi/langapi.a \
						@CBMCDIR@/src/ansi-c/ansi-c.a \
						@CBMCDIR@/src/linking/linking.a

MINISAT_OBJS = @MINISATDIR@/utils/System.o

if BUILD_LINENOISE
LINENOISE_OBJS = $(top_builddir)/linenoise/linenoise.o
else
LINENOISE_OBJS =
endif

include_fshell2_maindir = $(includedir)/fshell2/main
noinst_LTLIBRARIES = libmain.la
libmain_la_SOURCES = fshell2.cpp parseoptions.cpp
include_fshell2_main_HEADERS = fshell2.hpp parseoptions.hpp
check_PROGRAMS = fshell2
fshell2_SOURCES = fshell2.t.cpp
# add additional objects required to build fshell2 here, like
# fshell2_LDADD = ../base/libbase.la libbla.la
fshell2_LDADD = libmain.la \
							  ../util/libutil.la \
								../command/libcommand.la \
								../macro/libmacro.la \
								../tc_generation/libtc_generation.la \
								../fql/evaluation/libevaluation.la \
								../fql/normalize/libnormalize.la \
								../fql/parser/libparser.la \
								../fql/ast/libast.la \
								../fql/concepts/libconcepts.la \
								../instrumentation/libinstrumentation.la \
								../exception/libexception.la \
								$(LINENOISE_OBJS) $(CBMC_OBJS) $(MINISAT_OBJS)
# note that this entry (fshell2_SOURCES, fshell2_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = fshell2
