# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

include_fshell2_instrumentationdir = $(includedir)/fshell2/instrumentation
noinst_LTLIBRARIES = libinstrumentation.la
libinstrumentation_la_SOURCES = goto_transformation.cpp
include_fshell2_instrumentation_HEADERS = goto_transformation.hpp
check_PROGRAMS = goto_transformation
goto_transformation_SOURCES = goto_transformation.t.cpp
# add additional objects required to build goto_transformation here, like
# goto_transformation_LDADD = ../base/libbase.la libbla.la
goto_transformation_LDADD =
# note that this entry (goto_transformation_SOURCES, goto_transformation_LDADD) will not be removed
# by update-makefiles.pl, if the sources files are removed;
# it must then be removed manually
