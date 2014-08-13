# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = @CBMCDIR@/src/util/util.a \
						@CBMCDIR@/src/big-int/big-int.a \
						@CBMCDIR@/src/ansi-c/ansi-c.a \
						@CBMCDIR@/src/langapi/langapi.a \
						@CBMCDIR@/src/linking/linking.a

if DIAGNOSTICS
BUILT_SOURCES += parser.hh
else
BUILT_SOURCES = parser.hh
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
	echo "#define FQL_HELP \\" > $@
	$(top_srcdir)/config/y2l-postproc.pl < $< >> $@

BUILT_SOURCES += grammar.hpp

include_fshell2_fql_parserdir = $(includedir)/fshell2/fql/parser
noinst_LTLIBRARIES = libparser.la
libparser_la_SOURCES = lexer.ll parser.yy  query_parser.cpp
include_fshell2_fql_parser_HEADERS =    query_parser.hpp
check_PROGRAMS =  query_parser
query_parser_SOURCES = query_parser.t.cpp
# add additional objects required to build query_parser here, like
# query_parser_LDADD = ../base/libbase.la libbla.la
query_parser_LDADD = libparser.la ../ast/libast.la ../../exception/libexception.la $(CBMC_OBJS)
# note that this entry (query_parser_SOURCES, query_parser_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = query_parser
