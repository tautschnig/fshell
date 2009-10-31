#!/usr/bin/perl -w

use strict;

my $in_input = 0;
my $tc_counter = 0;
my %test_suite = ();
my %inserts = ();
my %replaces = ();

sub prepare_main () {
  my ($tc_id, $input_line) = @_;
  defined $test_suite{$tc_id} or die "Test case $tc_id is not known\n";

  ($input_line =~ /^ENTRY\s+(\S+)\((.*)\)@\[([^\]]+)\]$/) 
    or die "Unexpected entry format in $input_line\n";
  my $sym = $1;
  my $args = $2;
  my $loc = $3;
    
  ($loc =~ /^file (\S+) line (\d+)(\s+.*)?$/)
    or die "Unexpected location format in $input_line\n";
  my $file = $1;
  my $line = $2;

  $test_suite{$tc_id}{MAIN} = {
    symbol => $sym,
    file => $file,
    line => $line,
    args => {}
  };
  $test_suite{$tc_id}{MAIN}{arg_vals} = ();

  foreach my $a (split(/,/, $args)) {
    $a =~ s/\s+//g;
    $test_suite{$tc_id}{MAIN}{args}{$a} = "";
  }

  $replaces{$file}{$line}{$sym} = "__orig__$sym";
}

sub process_input () {
  my ($tc_id, $input_line) = @_;
  # print "INPUT[$input_id]: $input_line\n";
  defined $test_suite{$tc_id} or die "Test case $tc_id is not known\n";

  ($input_line =~ /^(.+)@\[([^\]]+)\]=(.+)$/) 
    or die "Unexpected assigment format in $input_line\n";
  my $sym = $1;
  my $type = "";
  my $is_func = 0;
  my $loc = $2;
  my $val = $3;
  my $key = "$sym\@$loc";
    
  ($loc =~ /^file (\S+) line (\d+)(\s+.*)?$/)
    or die "Unexpected location format in $input_line\n";
  my $file = $1;
  my $line = $2;

  # is $sym an undefined function?
  # drop non-file info from $loc
  if ($sym =~ /\(.*\)/) {
    $is_func = 1;
    $key = "$sym\@$file";
  } else {
    $sym =~ s/^(.*)\s+(\S+)$/$2/;
    $type = $1;
  }

  if (defined($test_suite{$tc_id}{MAIN}{args}{$sym})) {
    push @{ $test_suite{$tc_id}{MAIN}{arg_vals} }, $val;
    return;
  }
    
  unless (defined($test_suite{$tc_id}{$key})) {
    $test_suite{$tc_id}{$key} = {
      symbol => $sym,
      type => $type,
      is_func => $is_func,
      file => $file,
      line => $line,
      code_done => 0
    };
    $test_suite{$tc_id}{$key}{vals} = ();
  }

  push @{ $test_suite{$tc_id}{$key}{vals} }, $val;
}

while (<>) {
  if (/^IN:/) {
    $in_input = 1;
    $tc_counter++;
    $test_suite{ $tc_counter } = {};
  } elsif (/^\s*$/) {
    $in_input = 0;
  } elsif ($in_input == 1) {
    chomp;
    s/^\s+//;
    if (/^ENTRY/) {
      chomp;
      s/^\s+//;
      &prepare_main($tc_counter, $_);
    } else {
      &process_input($tc_counter, $_);
    }
  }
}

foreach my $id (sort keys %test_suite) {
  foreach my $key (keys %{ $test_suite{$id} }) {
    next if ($key eq "MAIN");
    my $sym = \%{ $test_suite{$id}{$key} };
    next if ($sym->{code_done});
    if ($sym->{is_func}) {
      my $retval_type = $sym->{symbol};
      $retval_type =~ s/^(.+)\s+\S+\(.*$/$1/;
      (defined($inserts{ $sym->{file} })) or $inserts{ $sym->{file} } = ();
      push @{ $inserts{ $sym->{file} } }, "extern unsigned __fshell2__tc_selector;";
      my $decl = $sym->{symbol};
      if ($decl =~ /,/) {
        $decl =~ s/,/ ###,/g;
        $decl =~ s/\)/ ###)/;
        my $i = 0;
        while ($decl =~ / ###/) {
          $decl =~ s/ ###/ _$i/;
          $i++;
        }
      }
      push @{ $inserts{ $sym->{file} } }, "$decl\{";
      push @{ $inserts{ $sym->{file} } }, "  static unsigned idx = 0;";
      
      my @switch = ();
      push @switch, "  switch (__fshell2__tc_selector) {";
      foreach my $id2 (sort keys %test_suite) {
        defined($test_suite{$id2}{$key}) or next;
        my $sym2 = \%{ $test_suite{$id2}{$key} };
        push @{ $inserts{ $sym->{file} } }, "  $retval_type retval$id2\[" .
          scalar(@{ $sym2->{vals} }) . "] = { " . join(",", (@{ $sym2->{vals} })) . " };";
        my $vals = "";
        $vals =~ s/^, //;
        push @switch, "    case $id2: return retval$id2\[idx++];";
        $sym2->{code_done} = 1;
      }
      push @{ $inserts{ $sym->{file} } }, @switch;
      push @{ $inserts{ $sym->{file} } }, "  }";
      push @{ $inserts{ $sym->{file} } }, "}";
    } else {
      my $new_name = $sym->{symbol} . "_" . $sym->{file} . "_" . $sym->{line};
      $new_name =~ s/[\/\\:]/__/g;
      $new_name =~ s/\./_/g;
      push @{ $inserts{ $sym->{file} } }, "extern unsigned __fshell2__tc_selector;";
      $replaces{ $sym->{file} }{ $sym->{line} }{ $sym->{symbol} } =
        $sym->{symbol} . "=$new_name\[__fshell2__tc_selector][idx__$new_name++]";
      
      my $max_size = 0;
      foreach my $id2 (sort keys %test_suite) {
        next if (!defined($test_suite{$id2}{$key}));
        my $sym2 = \%{ $test_suite{$id2}{$key} };
        $max_size = scalar(@{ $sym2->{vals} }) if (scalar(@{ $sym2->{vals} }) > $max_size);
      }
      my @vals = ();
      for (my $i = 0; $i < $max_size; $i++) {
        push @vals, "0";
      }
      foreach my $id2 (sort keys %test_suite) {
        if (!defined($test_suite{$id2}{$key})) {
          for (my $i = 0; $i < $max_size; $i++) {
            push @vals, "0";
          }
          next;
        }
        my $sym2 = \%{ $test_suite{$id2}{$key} };
        push @vals, @{ $sym2->{vals} };
        for (my $i = scalar(@{ $sym2->{vals} }); $i < $max_size; $i++) {
          push @vals, "0";
        }
        $sym2->{code_done} = 1;
      }
      
      (defined($inserts{ $sym->{file} })) or $inserts{ $sym->{file} } = ();
      push @{ $inserts{ $sym->{file} } }, "unsigned idx__$new_name = 0;";
      push @{ $inserts{ $sym->{file} } }, $sym->{type} . " $new_name\[" . scalar(@vals) .
        "][$max_size] = { " . join(",", @vals) . " };";
    }
  }
}

open (MAKEFILE, ">tester.mk");

print MAKEFILE "run_tests: tester\n";
print MAKEFILE "\t./tester $_\n" foreach (sort keys %test_suite);

print MAKEFILE <<EOF;

%.bak:
\tcp % \$@

clean:
\trm -f tester
EOF
print MAKEFILE "\trm -f $_.mod.c\n" foreach(keys %replaces);
print MAKEFILE "\trm -f $_.mod.c\n" foreach(keys %inserts);

print MAKEFILE "\ntester: tester.c ";
print MAKEFILE join(".mod.c ", keys %replaces) .
  (scalar(keys %replaces)?".mod.c ":" ") . join(".mod.c ", keys %inserts) .
  (scalar(keys %inserts)?".mod.c\n":"\n");
print MAKEFILE "\tgcc -o \$@ \$(BUILD_FLAGS) \$^\n\n";

foreach my $f (keys %replaces) {
  print MAKEFILE "$f.mod.c: $f\n";
  print MAKEFILE "\tcp \$^ \$@\n";
  foreach my $l (keys %{ $replaces{$f} }) {
    foreach my $s (keys %{ $replaces{$f}{$l} }) {
      print MAKEFILE "\tmv \$\@ \$\@_\n";
      print MAKEFILE "\tsed '$l s/[[:space:]]$s/ $replaces{$f}{$l}{$s}/' \$\@_ > \$\@\n";
      print MAKEFILE "\trm \$\@_\n";
    }
  }
  
  if (defined($inserts{$f})) {
    print MAKEFILE "\tmv \$\@ \$\@_\n";
    print MAKEFILE "\techo '$_' >> \$\@\n" foreach(@{ $inserts{$f} });
    print MAKEFILE "\tcat \$\@_ >> \$\@\n";
    print MAKEFILE "\trm \$\@_\n";
  }
  
  print MAKEFILE "\n";
}

foreach my $f (keys %inserts) {
  next if (defined($replaces{$f}));
  print MAKEFILE "$f.mod.c: $f\n";
  print MAKEFILE "\techo '$_' >> \$\@\n" foreach(@{ $inserts{$f} });
  print MAKEFILE "\tcat \$^ >> \$\@\n";
  print MAKEFILE "\n";
}

print MAKEFILE <<EOF;
.PHONY: run_tests clean
EOF

open (TESTER, ">tester.c");

print TESTER <<EOF;
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

unsigned __fshell2__tc_selector;

int main(int argc, char* argv[]) {
  long tc = -1;
  assert(argc == 2);
  errno = 0;
  tc = strtol(argv[1], NULL, 10);
  assert(errno == 0);

  switch(tc) {
EOF

foreach my $id (sort keys %test_suite) {
  print TESTER "    case $id:\n";
  print TESTER "      __fshell2__tc_selector = $id;\n";
  print TESTER "      __orig__" . $test_suite{$id}{MAIN}{symbol} . "(";
  print TESTER (defined($test_suite{$id}{MAIN}{arg_vals})?join(",", @{ $test_suite{$id}{MAIN}{arg_vals} }):"") . ");\n";
  print TESTER "      break;\n";
}

print TESTER <<EOF;
    default:
      printf("No test case available for id %ld\\n", tc);
      return 1;
  }

  return 0;
}
EOF

