# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = $(top_builddir)/cbmc/src/util/util.o \
						$(top_builddir)/cbmc/src/big-int/bigint.o \
						$(top_builddir)/cbmc/src/ansi-c/ansi-c.o

$(CBMC_OBJS):
	$(MAKE) -C $(top_builddir)/cbmc/src util.dir big-int.dir ansi-c.dir

$(top_builddir)/cbmc/src/util/irep_ids.h:
	$(MAKE) -C $(top_builddir)/cbmc/src/util irep_ids.h

include_fshell2_fql_astdir = $(includedir)/fshell2/fql/ast
noinst_LTLIBRARIES = libast.la
libast_la_SOURCES = ast_visitor.cpp fql_node.cpp pathcov.cpp predicate.cpp  query.cpp        edgecov.cpp    filter_intersection.cpp filter_setminus.cpp filter_union.cpp filter_compose.cpp filter_function.cpp  filter_expr.cpp     nodecov.cpp path_pattern_expr.cpp  repeat.cpp transform_pred.cpp   coverage_pattern_expr.cpp depcov.cpp pp_alternative.cpp pp_concat.cpp cp_alternative.cpp cp_concat.cpp ecp_atom.cpp fql_ast_printer.cpp fql_node_lt_visitor.cpp quote.cpp
include_fshell2_fql_ast_HEADERS = ast_visitor.hpp fql_ast_printer.hpp fql_node_factory.hpp pathcov.hpp predicate.hpp  query.hpp        edgecov.hpp    filter_intersection.hpp filter_setminus.hpp filter_union.hpp fql_node.hpp standard_ast_visitor_aspect.hpp fql_node_lt_visitor.hpp filter_compose.hpp      filter_function.hpp   filter_expr.hpp      nodecov.hpp  depcov.hpp repeat.hpp  coverage_pattern_expr.hpp  transform_pred.hpp path_pattern_expr.hpp cp_concat.hpp cp_alternative.hpp pp_alternative.hpp pp_concat.hpp ecp_atom.hpp quote.hpp
check_PROGRAMS = fql_ast_printer fql_node_lt_visitor
fql_ast_printer_SOURCES = fql_ast_printer.t.cpp
# add additional objects required to build fql_ast_printer here, like
# fql_ast_printer_LDADD = ../base/libbase.la libbla.la
fql_ast_printer_LDADD = libast.la $(CBMC_OBJS)
# note that this entry (fql_ast_printer_SOURCES, fql_ast_printer_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
fql_node_lt_visitor_SOURCES = fql_node_lt_visitor.t.cpp
# add additional objects required to build fql_node_lt_visitor here, like
# fql_node_lt_visitor_LDADD = ../base/libbase.la libbla.la
fql_node_lt_visitor_LDADD = libast.la $(CBMC_OBJS)
# note that this entry (fql_node_lt_visitor_SOURCES, fql_node_lt_visitor_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
TESTS = fql_ast_printer fql_node_lt_visitor
