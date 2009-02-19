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

include_fshell2_fql_parserdir = $(includedir)/fshell2/fql/parser
noinst_LTLIBRARIES = libparser.la
libparser_la_SOURCES = lexer.ll parser.yy
