#!/usr/bin/perl -w

use strict;

my $last_tok = "";

while (<>) {
  my $line = $_;
  $line =~ s/(\s+|^)([a-zA-Z_]+)(\s+|$)/$1<$2>$3/g;
  $line =~ s/"([^"]+)"/`$1'/g;
  ($line =~ /^(<[a-zA-Z_]+>\s+::)(=)/) or die "$line was unexpected\n";
  if ($1 eq $last_tok) {
    my $blanks = $1;
    $blanks =~ s/./ /g;
    $line =~ s/$last_tok=/$blanks|/;
  } else {
    $last_tok = $1;
  }

  $line =~ s/^/"/;
  $line =~ s/$/" << ::std::endl << \\/;
  print $line;
}

print "\"\"\n";

