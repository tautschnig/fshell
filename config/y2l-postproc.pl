#!/usr/bin/perl -w

use strict;

my @inputs = ();

while (<>) {
  if (/^\s+(\S+.*)$/) {
    chomp $inputs[-1];
    $inputs[-1] .= " $1\n";
    next;
  }
  push @inputs, $_;
}

my $last_tok = "";

foreach (@inputs) {
  my $line = $_;
  my $line_bak = "";
  while ($line ne $line_bak) {
    $line_bak = $line;
    $line =~ s/(\s+|^)([a-zA-Z_]+)(\s+|$)/$1<$2>$3/g;
  }
  $line =~ s/"""/`\\"'/g;
  $line =~ s/"([^"`]+)"/`$1'/g;
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

