# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS = config exception command macro fql main

include_fshell2dir = $(includedir)/fshell2

# NO_UPDATE_MAKEFILES

lib_LTLIBRARIES = libfshell2.la
libfshell2_la_SOURCES =
libfshell2_la_LIBADD = exception/libexception.la \
											 command/libcommand.la \
											 macro/libmacro.la \
											 fql/ast/libast.la \
											 fql/parser/libparser.la \
											 main/libmain.la
libfshell2_la_LDFLAGS = -version-info 0:0:0
