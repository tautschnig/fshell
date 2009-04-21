# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

include_fshell2_fql_astdir = $(includedir)/fshell2/fql/ast
noinst_LTLIBRARIES = libast.la
libast_la_SOURCES = ast_visitor.cpp fql_ast_printer.cpp
include_fshell2_fql_ast_HEADERS = ast_visitor.hpp fql_ast_printer.hpp fql_node_factory.hpp
check_PROGRAMS = fql_ast_printer
fql_ast_printer_SOURCES = fql_ast_printer.t.cpp
# add additional objects required to build fql_ast_printer here, like
# fql_ast_printer_LDADD = ../base/libbase.la libbla.la
fql_ast_printer_LDADD =
# note that this entry (fql_ast_printer_SOURCES, fql_ast_printer_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
