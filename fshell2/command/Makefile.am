# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

if DIAGNOSTICS
BUILT_SOURCES += parser.h
else
BUILT_SOURCES = parser.h
endif

#debug bison-parser
#AM_YFLAGS = -d -t -v
AM_YFLAGS = -d
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

CBMC_OBJS = @CBMCDIR@/src/goto-programs/goto-programs.a \
						@CBMCDIR@/src/assembler/assembler.a \
						@CBMCDIR@/src/pointer-analysis/pointer-analysis.a \
						@CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/bigint.o \
						@CBMCDIR@/src/langapi/langapi.a \
						@CBMCDIR@/src/ansi-c/ansi-c.a \
						@CBMCDIR@/src/linking/linking.a

noinst_LTLIBRARIES = libcommand.la
libcommand_la_SOURCES = command_processing.cpp lexer.ll parser.yy
include_fshell2_commanddir = $(includedir)/fshell2/command
include_fshell2_command_HEADERS = command_processing.hpp
check_PROGRAMS = command_processing
command_processing_SOURCES = command_processing.t.cpp
# add additional objects required to build command_processing here, like
# command_processing_LDADD = ../base/libbase.la libbla.la
command_processing_LDADD = libcommand.la ../exception/libexception.la $(CBMC_OBJS)
# note that this entry (command_processing_SOURCES, command_processing_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = command_processing
