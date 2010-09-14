#!/bin/bash

if [ -z "$1" ] ; then
  echo "Usage: $0 <config-name>" 1>&2
  exit 1
fi

conf="$1"

if [ ! -d conf/ ] ; then
  echo "Directory conf/ does not exist" 1>&2
  exit 2
fi

if [ -e "conf/$conf" ] ; then
  echo "Configuration $conf already exists" 1>&2
  exit 3
fi

shift
cat > "conf/$conf" <<EOF
# vi: set ft=sh:

source="`echo $conf | cut -f1 -d_`"
extra_params="$@"
prepare=""
query="cover no-such-query"
tg=0
unsat=0
tc=0
cputime=0.000
EOF

