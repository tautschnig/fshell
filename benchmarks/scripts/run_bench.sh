#!/bin/bash

set -e

if [ -z "$debug" ] ; then
  debug=0
elif [ $debug -gt 0 ] ; then
  set -x
fi

[ -n "$fshell" ] 
[ -x "$fshell" ]
[ -n "$testenv" ] 
[ -x "$testenv" ]
[ -n "$logf" ]
  
benchmarks=$@
if which mktemp > /dev/null 2>&1 ; then
  results=`mktemp`
  testsuite=`mktemp`
else
  results="$logf.tmplog"
  testsuite="$logf.tmpts"
fi
trap 'rm -f $results $testsuite' EXIT

for b in $benchmarks ; do
  unset source extra_params prepare query tg unsat tc cputime mempeak exec_ts gcov gcov_opts
  . $b
  make -C code $source.prep
  rm -f .fshell_history
  rm -f $testsuite
  if [ x$exec_ts = xyes -o x$gcov = xyes ] ; then
    exec_ts="yes"
    extra_params="$extra_params --tco-location --outfile $testsuite"
  else
    extra_params="$extra_params --outfile /dev/null"
  fi
  eval set -- "$extra_params"
  echo "Running benchmark $b"
  if [ $debug -eq 1 ] ; then
    echo -e "$prepare\n$query\nquit\n" | $fshell $params "$@"
  elif [ $debug -eq 2 ] ; then
    echo -e "$prepare\n$query\nquit\n"
    gdb --args $fshell $params "$@"
  else
    ( time echo -e "$prepare\n$query\nquit\n" | $fshell $params "$@" ) > $results 2>&1
    mtg=`grep "Possibly feasible test goals:" $results | sed 's/.*Possibly feasible test goals: //'`
    munsat=`grep "Test goals not fulfilled:" $results | sed 's/.*Test goals not fulfilled: //'`
    mtc=`grep "Test cases:" $results | sed 's/.*Test cases: //'`
    mcputime=`grep "Query CPU time:" $results | sed 's/.*Query CPU time: //'`
    mmempeak=`grep "Peak memory usage:" $results | sed 's/.*Peak memory usage: //'`
    mmin=`grep "Test cases removed by minimization:" $results | sed 's/.*Test cases removed by minimization: //'`
    ommin=`grep "Minimization memory overhead:" $results | sed 's/.*Minimization memory overhead: //'`
    otmin=`grep "Minimization CPU time overhead:" $results | sed 's/.*Minimization CPU time overhead: //'`
    if [ -z "$mtg" -o -z "$munsat" -o -z "$mtc" -o -z "$mcputime" -o -z "$mmempeak" -o -z "$mmin" ] ; then
      cp $results failure.log
      exit 1
    fi
    echo "$b #tg=$mtg($tg) #iftg=$munsat($unsat) #tc=$mtc($tc) #min=$mmin($min) ${mcputime}($cputime) ${mmempeak}($mempeak) ommin=$ommin otmin=$otmin" >> $logf
    if [ x$exec_ts = xyes ] ; then
      echo "Trying to execute generated test suite"
      $testenv < $testsuite
      [ -f tester.mk ]
      make -f tester.mk clean > /dev/null
      if [ x$gcov = xyes ] ; then
        make -f tester.mk BUILD_FLAGS="-g -fprofile-arcs -ftest-coverage" > /dev/null
        for f in *.mod.gcno ; do
          gcov $gcov_opts "`echo $f | sed 's/mod.gcno/mod.c/'`"
        done
      else
        make -f tester.mk BUILD_FLAGS="-g" > /dev/null
      fi
      make -f tester.mk clean > /dev/null
      rm tester.mk tester.c
      rm -f *.gcov *.gcda *.gcno
      rm -rf tester.dSYM
    fi
  fi
done

