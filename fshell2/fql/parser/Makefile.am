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

MAINTAINERCLEANFILES += grammar.hpp

lexer.txt: lexer.ll
	cat $^ | @GREP@ ^TOK | @SED@ 's/\\//g' | @EGREP@ -v '(TOK_C_IDENT|TOK_QUOTED_STRING|TOK_NAT_NUMBER)' > $@

grammar.hpp: parser.yy lexer.txt
	echo "#define FQL_HELP \\" > $@
	@AWK@ -f $(top_srcdir)/config/y2l -- -p -tlexer.txt $< | $(top_srcdir)/config/y2l-postproc.pl >> $@

include_fshell2_fql_parserdir = $(includedir)/fshell2/fql/parser
noinst_LTLIBRARIES = libparser.la
libparser_la_SOURCES = lexer.ll parser.yy query_processing.cpp
libparser_la_DEPENDENCIES = grammar.hpp
include_fshell2_fql_parser_HEADERS =   query_processing.hpp grammar.hpp parser.h
check_PROGRAMS = query_processing
query_processing_SOURCES = query_processing.t.cpp
# add additional objects required to build query_processing here, like
# query_processing_LDADD = ../base/libbase.la libbla.la
query_processing_LDADD = ../../exception/libexception.la libparser.la
# note that this entry (query_processing_SOURCES, query_processing_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
