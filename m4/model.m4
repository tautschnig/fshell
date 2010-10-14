# copied from /usr/share/autoconf/m4sugar/m4sh.m4
AC_DEFUN([model_VAR_SET],[
  AS_LITERAL_IF([$1],
         [$1=$2],
         [eval "$1=AS_ESCAPE([$2])"])
])

# copied from /usr/share/autoconf/autoconf/libs.m4
AC_DEFUN([model_CHECK_CPLUSPLUS_LIB],[
  AC_LANG([C++])
  m4_ifval([$4], , [AH_CHECK_LIB([$1])])dnl
  AS_LITERAL_IF([$1],
                [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1_$3])],
                [AS_VAR_PUSHDEF([ac_Lib], [ac_cv_lib_$1''_$3])])dnl
  AC_CACHE_CHECK([for $3 in -l$1], ac_Lib,
  [ac_check_lib_save_LIBS=$LIBS
  LIBS="-l$1 $6 $LIBS"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([$2],[$3])],
                 [model_VAR_SET(ac_Lib, yes)],
                 [model_VAR_SET(ac_Lib, no)])
  LIBS=$ac_check_lib_save_LIBS])
  AS_IF([test AS_VAR_GET(ac_Lib) = yes],
        [m4_default([$4], [AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_LIB$1), [], 
        [$1 found])
    LIBS="-l$1 $LIBS"
  ])],
        [$5])dnl
  AS_VAR_POPDEF([ac_Lib])dnl
])

# a shell-based portable implementation of realpath; replaces the value of the
# variable named $1
AC_DEFUN([model_REALPATH],[
  if test -d "[$]$1" ; then
    cd "[$]$1" > /dev/null
    $1="`pwd`"
    cd "$ac_pwd" > /dev/null
  elif test -r "[$]$1" ; then
    cd "`dirname "[$]$1"`" > /dev/null
    $1="`pwd`"
    $1="[$]$1/`basename "[$]$1"`"
    cd "$ac_pwd" > /dev/null
  fi
])

AC_DEFUN([model_DIAGNOSTICS],[
  # http://www.openismus.com/documents/linux/using_libraries/using_libraries.shtml
  AC_ARG_WITH(diagnostics,
              [--with-diagnostics=<path>,<diag-level> path to diagnostics library
              and diagnosis level (can be audit, debug, prod or off)],
              [DIAGNOSTICSDIR=${with_diagnostics/,*/}
              model_REALPATH([DIAGNOSTICSDIR])
              diag_lib_check_string="-L${DIAGNOSTICSDIR}/lib"
              DIAGNOSTICSLEVEL=${with_diagnostics/*,/}
              if test "x$DIAGNOSTICSLEVEL" = xno; then
                DIAGNOSTICSLEVEL=-1
              elif test "x$DIAGNOSTICSLEVEL" = xoff; then
                DIAGNOSTICSLEVEL=-1
              elif test "x$DIAGNOSTICSLEVEL" = xprod; then
                DIAGNOSTICSLEVEL=0
              elif test "x$DIAGNOSTICSLEVEL" = xdebug; then
                DIAGNOSTICSLEVEL=1
              elif test "x$DIAGNOSTICSLEVEL" = xaudit; then
                DIAGNOSTICSLEVEL=2
              else
                AC_MSG_ERROR([diagnosis level must be one of audit, debug, prod or off!])
              fi
              ],
              DIAGNOSTICSLEVEL=0
              )
  if test "x$DIAGNOSTICSLEVEL" != "x-1" ; then
  model_CHECK_CPLUSPLUS_LIB([diagnostics], [
#include <vector>
namespace diagnostics
{
class Logger;
char const * diagnostics_library_version(bool const);
void set_initial_loggers(::std::vector<Logger *> & loggers) {}
}
                ], [diagnostics::diagnostics_library_version(true)], 
                [:],
                [DIAGNOSTICSLEVEL=-1],
                [${diag_lib_check_string}])
  fi
  AM_CONDITIONAL(DIAGNOSTICS, test $DIAGNOSTICSLEVEL -gt -1)
  AM_CONDITIONAL(DIAGNOSTICSDIR_DEFINED, test "x$DIAGNOSTICSDIR" != x)
  AC_SUBST(DIAGNOSTICSDIR)
  AC_SUBST(DIAGNOSTICSLEVEL)
])

# macros to control update-makefiles.pl, disabled by default
AC_SUBST(disable_update_makefiles,[--not-really])
AC_DEFUN([model_UPDATE_MAKEFILES],[
  AC_ARG_ENABLE([install-headers],[  --disable-install-headers           Don't install header files during make install],
                [if test "x$enableval" != "xno" ; then
                  AC_SUBST(enable_install_headers,[--install-headers])
                else
                  AC_SUBST(enable_install_headers,[""])
                fi],
                AC_SUBST(enable_install_headers,[--install-headers])
                )

  AC_ARG_ENABLE([update-makefiles],[  --disable-update-makefiles        Do not update any Makefile.am],
                [if test "x$enableval" = "xno" ; then
                  AC_SUBST(disable_update_makefiles,[--not-really])
                else
                  AC_SUBST(disable_update_makefiles,[""])
                fi],
                AC_SUBST(disable_update_makefiles,[""])
                )
])

# C++ capable versions of lex, yacc
AC_DEFUN([model_LEX_YACC_CPLUSPLUS],[
  AC_REQUIRE([AC_PROG_AWK])
  AC_REQUIRE([AM_PROG_LEX])
  AC_REQUIRE([AC_PROG_YACC])
  # make sure the version of flex is >= 2.5.31
  if ! test -x "`which $LEX`" ; then
    AC_MSG_ERROR([lex/flex not found. Please install it first.])
  else
    flexv=`$LEX --version | sed 's/.* //'`
    flexv1=`echo $flexv | $AWK -F. '{ print [$]1 }'`
    flexv2=`echo $flexv | $AWK -F. '{ print [$]2 }'`
    flexv3=`echo $flexv | $AWK -F. '{ print [$]3 }' | sed 's/[[a-z]]*$//'`
    test $flexv1 -gt 2 || test $flexv1 -eq 2 -a $flexv2 -gt 5 || \
      test $flexv1 -eq 2 -a $flexv2 -eq 5 -a $flexv3 -gt 30 || \
     AC_MSG_ERROR([A lex/flex version greater than 2.5.30 is required, version $flexv has been found.])
  fi
  # make sure the version of bison is >= 2.0
  YACC_BINARY=`echo $YACC | sed 's/ .*//g'`
  if ! test -x "`which $YACC_BINARY`" ; then
    AC_MSG_ERROR([yacc/bison not found. Please install it first.])
  else
    bisonv=`$YACC --version | sed 's/.* //'`
    bisonv1=`echo $bisonv | $AWK -F. '{ print [$]1 }'`
    test $bisonv1 -ge 2 || \
     AC_MSG_ERROR([A yacc/bison version greater than 2.0 is required, version $bisonv has been found.])
  fi
])

# taken from rtorrent, http://libtorrent.rakshasa.no/browser/trunk/rtorrent/scripts/checks.m4
AC_DEFUN([model_WITH_XMLRPC_C], [
  AC_MSG_CHECKING([for XMLRPC-C, features $1])

  AC_ARG_WITH(xmlrpc-c,
  [  --with-xmlrpc-c=PATH     Enable XMLRPC-C support.],
  [
    if test "$withval" = "no"; then
      AC_MSG_RESULT(no)
    else
      if test "$withval" = "yes"; then
        xmlrpc_cc_prg="xmlrpc-c-config"
      else
        xmlrpc_cc_prg="$withval"
      fi
     
      if eval $xmlrpc_cc_prg --version 2>/dev/null >/dev/null; then
        ac_check_lib_save_CXXFLAGS="$CXXFLAGS"
        ac_check_lib_save_LIBS="$LIBS"
        CXXFLAGS="$CXXFLAGS `$xmlrpc_cc_prg --cflags $1`"
        LIBS="$LIBS `$xmlrpc_cc_prg $1 --ldadd`"
        AC_TRY_LINK(
        [ #include <xmlrpc-c/server.h>
        ],[ xmlrpc_registry_new(NULL); ],
        [
          AC_DEFINE(HAVE_XMLRPC_C, 1, Support for XMLRPC-C.)
          AC_MSG_RESULT(yes)
        ], [
          CXXFLAGS="$ac_check_lib_save_CXXFLAGS"
          LIBS="$ac_check_lib_save_LIBS"
          AC_MSG_RESULT(no)
          AC_MSG_WARN(Could not compile XMLRPC-C test.)
        ])
      else
        AC_MSG_RESULT(no)
        AC_MSG_WARN(Could not compile XMLRPC-C test.)
      fi
    fi

  ],[
    xmlrpc_cc_prg="xmlrpc-c-config"
    if eval $xmlrpc_cc_prg --version 2>/dev/null >/dev/null; then
      ac_check_lib_save_CXXFLAGS="$CXXFLAGS"
      ac_check_lib_save_LIBS="$LIBS"
      CXXFLAGS="$CXXFLAGS `$xmlrpc_cc_prg --cflags $1`"
      LIBS="$LIBS `$xmlrpc_cc_prg $1 --ldadd`"
      AC_TRY_LINK(
      [ #include <xmlrpc-c/server.h>
      ],[ xmlrpc_registry_new(NULL); ],
      [
        AC_DEFINE(HAVE_XMLRPC_C, 1, Support for XMLRPC-C.)
        AC_MSG_RESULT(yes)
      ], [
        CXXFLAGS="$ac_check_lib_save_CXXFLAGS"
        LIBS="$ac_check_lib_save_LIBS"
        AC_MSG_RESULT(no)
        AC_MSG_WARN(Could not compile XMLRPC-C test.)
      ])
    else
      AC_MSG_RESULT(no)
      AC_MSG_WARN(Could not compile XMLRPC-C test.)
    fi
  ])
])

