# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

CBMC_OBJS = ../../cbmc/src/util/util.o ../../cbmc/src/big-int/bigint.o \
						../../cbmc/src/langapi/langapi.o \
						../../cbmc/src/ansi-c/ansi-c.o \
						../../cbmc/src/langapi/csp_dummy.o \
						../../cbmc/src/langapi/cpp_dummy.o \
						../../cbmc/src/langapi/specc_dummy.o \
						../../cbmc/src/langapi/php_dummy.o \
						../../cbmc/src/langapi/intrep_dummy.o \
						../../cbmc/src/langapi/pvs_dummy.o \
						../../cbmc/src/langapi/smvlang_dummy.o \
						../../cbmc/src/langapi/promela_dummy.o \
						../../cbmc/src/langapi/verilog_dummy.o \
						../../cbmc/src/langapi/vhdl_dummy.o \
						../../cbmc/src/langapi/smtlang_dummy.o \
						../../cbmc/src/langapi/xmllang_dummy.o \
						../../cbmc/src/langapi/netlist_dummy.o \
						../../cbmc/src/langapi/pascal_dummy.o \
						../../cbmc/src/langapi/bplang_dummy.o \
						../../cbmc/src/langapi/simplifylang_dummy.o \
						../../cbmc/src/langapi/csharp_dummy.o \
						../../cbmc/src/langapi/cvclang_dummy.o \
						../../cbmc/src/langapi/nsf_dummy.o \
						../../cbmc/src/langapi/mdllang_dummy.o

$(CBMC_OBJS):
	$(MAKE) -C ../../cbmc/src

include_fshell2_maindir = $(includedir)/fshell2/main
noinst_LTLIBRARIES = libmain.la
libmain_la_SOURCES = fshell2.cpp
libmain_la_LIBADD = ../exception/libexception.la ../command/libcommand.la ../macro/libmacro.la ../fql/parser/libparser.la
include_fshell2_main_HEADERS = fshell2.hpp
check_PROGRAMS = fshell2
fshell2_SOURCES = fshell2.t.cpp
# add additional objects required to build fshell2 here, like
# fshell2_LDADD = ../base/libbase.la libbla.la
fshell2_LDADD = libmain.la $(CBMC_OBJS)
# note that this entry (fshell2_SOURCES, fshell2_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
