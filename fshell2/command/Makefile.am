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

EXTRA_DIST = grammar.hpp

MAINTAINERCLEANFILES += grammar.hpp grammar.txt lexer.txt

lexer.txt: lexer.ll
	@GREP@ ^TOK $^ | @SED@ 's/\\//g' | @EGREP@ -v '(TOK_C_IDENT|TOK_QUOTED_STRING|TOK_NAT_NUMBER)' > $@

grammar.txt: parser.yy lexer.txt
	@AWK@ -f $(top_srcdir)/config/y2l -- -p -tlexer.txt $< > $@

grammar.hpp: grammar.txt
	echo "#define COMMAND_HELP \\" > $@
	$(top_srcdir)/config/y2l-postproc.pl < $< >> $@

BUILT_SOURCES += grammar.hpp

CBMC_OBJS = @CBMCDIR@/src/goto-programs/goto-programs.a \
						@CBMCDIR@/src/assembler/assembler.a \
						@CBMCDIR@/src/pointer-analysis/value_set.o \
						@CBMCDIR@/src/pointer-analysis/value_set_dereference.o \
						@CBMCDIR@/src/pointer-analysis/dereference_callback.o \
						@CBMCDIR@/src/pointer-analysis/add_failed_symbols.o \
						@CBMCDIR@/src/pointer-analysis/rewrite_index.o \
						@CBMCDIR@/src/pointer-analysis/goto_program_dereference.o \
						@CBMCDIR@/src/analyses/analyses.a \
						@CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/big-int.a \
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
