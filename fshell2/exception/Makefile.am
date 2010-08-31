# This Makefile.am is maintained by config/update-makefiles.pl, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include $(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

noinst_LTLIBRARIES = libexception.la
libexception_la_SOURCES = base_exception.cpp command_processing_error.cpp macro_processing_error.cpp query_processing_error.cpp instrumentation_error.cpp fshell2_error.cpp
include_fshell2_exceptiondir = $(includedir)/fshell2/exception
include_fshell2_exception_HEADERS = base_exception.hpp command_processing_error.hpp macro_processing_error.hpp query_processing_error.hpp instrumentation_error.hpp fshell2_error.hpp
