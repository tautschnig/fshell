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
libast_la_SOURCES = ast_visitor.cpp fql_ast_printer.cpp fql_node.cpp pathcov.cpp predicate.cpp primitive_filter.cpp query.cpp restriction_automaton.cpp test_goal_sequence.cpp test_goal_set.cpp tgs_intersection.cpp tgs_setminus.cpp tgs_union.cpp abstraction.cpp edgecov.cpp filter.cpp filter_complement.cpp filter_enclosing_scopes.cpp filter_intersection.cpp filter_setminus.cpp filter_union.cpp fql_node_lt_visitor.cpp
include_fshell2_fql_ast_HEADERS = ast_visitor.hpp fql_ast_printer.hpp fql_node_factory.hpp pathcov.hpp predicate.hpp primitive_filter.hpp query.hpp restriction_automaton.hpp test_goal_sequence.hpp test_goal_set.hpp tgs_intersection.hpp tgs_setminus.hpp tgs_union.hpp abstraction.hpp edgecov.hpp filter.hpp filter_complement.hpp filter_enclosing_scopes.hpp filter_intersection.hpp filter_setminus.hpp filter_union.hpp fql_node.hpp standard_ast_visitor_aspect.hpp fql_node_lt_visitor.hpp
check_PROGRAMS = fql_ast_printer fql_node_lt_visitor
fql_ast_printer_SOURCES = fql_ast_printer.t.cpp
# add additional objects required to build fql_ast_printer here, like
# fql_ast_printer_LDADD = ../base/libbase.la libbla.la
fql_ast_printer_LDADD = libast.la
# note that this entry (fql_ast_printer_SOURCES, fql_ast_printer_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
fql_node_lt_visitor_SOURCES = fql_node_lt_visitor.t.cpp
# add additional objects required to build fql_node_lt_visitor here, like
# fql_node_lt_visitor_LDADD = ../base/libbase.la libbla.la
fql_node_lt_visitor_LDADD = libast.la
# note that this entry (fql_node_lt_visitor_SOURCES, fql_node_lt_visitor_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually