#!/bin/bash

set -e

if [ -z "$debug" ] ; then
  debug=0
elif [ $debug -gt 0 ] ; then
  set -x
fi

[ -n "$fshell" ] 
[ -x "$fshell" ]
[ -n "$logf" ]
  
benchmarks=$@
if which mktemp > /dev/null 2>&1 ; then
  results=`mktemp`
else
  results="$logf.tmplog"
fi
trap 'rm -f $results' EXIT

for b in $benchmarks ; do
  unset source extra_params prepare query tg unsat tc cputime
  . $b
  make -C code $source.prep
  rm -f .fshell_history
  echo "Running benchmark $b"
  if [ $debug -eq 1 ] ; then
    echo -e "$prepare\n$query\nquit\n" | $fshell $params $extra_params
  elif [ $debug -eq 2 ] ; then
    echo -e "$prepare\n$query\nquit\n"
    gdb --args $fshell $params $extra_params
  else
    ( time echo -e "$prepare\n$query\nquit\n" | $fshell $params $extra_params ) > $results 2>&1
    mtg=`grep "Possibly feasible test goals:" $results | sed 's/.*Possibly feasible test goals: //'`
    munsat=`grep "Test goals not fulfilled:" $results | sed 's/.*Test goals not fulfilled: //'`
    mtc=`grep "Test cases:" $results | sed 's/.*Test cases: //'`
    mcputime=`grep "Query CPU time:" $results | sed 's/.*Query CPU time: //'`
    if [ -z "$mtg" -o -z "$munsat" -o -z "$mtc" -o -z "$mcputime" ] ; then
      cp $results failure.log
      exit 1
    fi
    echo "$b #tg=$mtg($tg) #iftg=$munsat($unsat) #tc=$mtc($tc) ${mcputime}($cputime)" >> $logf
  fi
done

