#!/bin/sh

top_srcdir=$1
DIAGNOSTICSLEVEL=$2
t=$3

mkdir -p $top_srcdir/test
check_path="`pwd`"
cd $top_srcdir
if test x`uname -o` = xMsys ; then
  if test -x $check_path/.libs/$t ; then
    $check_path/.libs/$t run "//*" $DIAGNOSTICSLEVEL || exit $?
  else
    $check_path/$t run "//*" $DIAGNOSTICSLEVEL || exit $?
  fi
else
  $check_path/$t run "/*" $DIAGNOSTICSLEVEL || exit $?
fi

exit 0

