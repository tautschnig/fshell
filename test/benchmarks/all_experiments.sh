#!/bin/bash

cd `dirname $0`

CBMC=../../cbmc/src/cbmc/cbmc
query=`mktemp`
results=`mktemp`

run_query() {
  bm=$1
  shift
  opts=$@
  ( time $CBMC --fshell --query-file $query $opts ) > $results 2>&1
  echo -n "$bm"
  echo -n " #tg=`grep "#Test goals:" $results | cut -f3 -d" "`"
  echo -n " #iftg=`grep "#Infeasible test goals:" $results | cut -f4 -d" "`"
  echo -n " #tc=`grep "#Test cases:" $results | cut -f3 -d" "`"
  echo -n " "
  grep ^real $results | awk '{ print $2 }'
}

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/cat.c"
set entry cat_main
cover edges(@basicblockentry)
quit
EOF
run_query "coreutils/cat.c;BB" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/cat.c"
set entry cat_main
cover edges(@conditionedge)
quit
EOF
run_query "coreutils/cat.c;CC" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/cat.c"
set entry cat_main
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "coreutils/cat.c;BB^2" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/cat.c"
set entry cat_main
cover edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "coreutils/cat.c;BB^4" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions




cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/echo.c"
set entry echo_main
cover edges(@basicblockentry)
quit
EOF
run_query "coreutils/echo.c;BB" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/echo.c"
set entry echo_main
cover edges(@conditionedge)
quit
EOF
run_query "coreutils/echo.c;CC" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/echo.c"
set entry echo_main
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "coreutils/echo.c;BB^2" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions




cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/nohup.c"
set entry nohup_main
cover edges(@basicblockentry)
quit
EOF
run_query "coreutils/nohup.c;BB" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/nohup.c"
set entry nohup_main
cover edges(@conditionedge)
quit
EOF
run_query "coreutils/nohup.c;CC" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/nohup.c"
set entry nohup_main
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "coreutils/nohup.c;BB^2" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions





cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/seq.c"
set entry seq_main
cover edges(@basicblockentry)
quit
EOF
run_query "coreutils/seq.c;BB" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/seq.c"
set entry seq_main
cover edges(@conditionedge)
quit
EOF
run_query "coreutils/seq.c;CC" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/seq.c"
set entry seq_main
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "coreutils/seq.c;BB^2" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions





cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/tee.c"
set entry tee_main
cover edges(@basicblockentry)
quit
EOF
run_query "coreutils/tee.c;BB" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/tee.c"
set entry tee_main
cover edges(@conditionedge)
quit
EOF
run_query "coreutils/tee.c;CC" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "busybox-1.14.0/coreutils/tee.c"
set entry tee_main
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "coreutils/tee.c;BB^2" -Ibusybox-1.14.0/include --unwind 10 --no-unwinding-assertions




cat > $query <<EOF
add sourcecode "kbfiltr.i.cil.c"
cover edges(@basicblockentry)
quit
EOF
run_query "kbfiltr.c;BB" --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "kbfiltr.i.cil.c"
cover edges(@conditionedge)
quit
EOF
run_query "kbfiltr.c;CC" --unwind 10 --no-unwinding-assertions

cat > $query <<EOF
add sourcecode "kbfiltr.i.cil.c"
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "kbfiltr.c;BB^2" --unwind 10 --no-unwinding-assertions





cat > $query <<EOF
add sourcecode "vfs-examples/pseudo_main.c"
cover edges(@basicblockentry)
quit
EOF
run_query "pseudo-vfs.c;BB" -Ivfs-examples

cat > $query <<EOF
add sourcecode "vfs-examples/pseudo_main.c"
cover edges(@conditionedge)
quit
EOF
run_query "pseudo-vfs.c;CC" -Ivfs-examples

cat > $query <<EOF
add sourcecode "vfs-examples/pseudo_main.c"
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "pseudo-vfs.c;BB^2" -Ivfs-examples

cat > $query <<EOF
add sourcecode "vfs-examples/pseudo_main.c"
cover edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "pseudo-vfs.c;BB^4" -Ivfs-examples

cat > $query <<EOF
add sourcecode "vfs-examples/pseudo_main.c"
cover edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "pseudo-vfs.c;BB^6" -Ivfs-examples




cat > $query <<EOF
add sourcecode "AL_expanded_mod.c"
cover edges(@basicblockentry)
quit
EOF
run_query "matlab.c;BB"

cat > $query <<EOF
add sourcecode "AL_expanded_mod.c"
cover edges(@conditionedge)
quit
EOF
run_query "matlab.c;CC"

cat > $query <<EOF
add sourcecode "AL_expanded_mod.c"
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "matlab.c;BB^2"




cat > $query <<EOF
cover edges(@basicblockentry)
quit
EOF
run_query "memman.c;BB" -Imemman/inc memman/main.c memman/src/*.c --function memman --unwind 5 --no-unwinding-assertions

cat > $query <<EOF
cover edges(@conditionedge)
quit
EOF
run_query "memman.c;CC" -Imemman/inc memman/main.c memman/src/*.c --function memman --unwind 5 --no-unwinding-assertions

cat > $query <<EOF
cover edges(@basicblockentry)->edges(@basicblockentry)
quit
EOF
run_query "memman.c;BB^2" -Imemman/inc memman/main.c memman/src/*.c --function memman --unwind 5 --no-unwinding-assertions




rm -f $results $query

