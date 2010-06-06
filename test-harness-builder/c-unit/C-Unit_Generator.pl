#!/usr/bin/perl -w

use strict;

my $in_input = 0;
my $tc_counter = 0;
my %test_suite = ();
my @idx_vars = ();
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
      my $idx_name = $sym->{symbol} . "_" . $sym->{file};
      $idx_name =~ s/[\s,\(\)\[\]\/\\:]/__/g;
      $idx_name =~ s/\./_/g;
      push @{ $inserts{ $sym->{file} } }, "extern unsigned $idx_name;";
      push @idx_vars, $idx_name;
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
      
      my @switch = ();
      push @switch, "  switch (__fshell2__tc_selector) {";
      foreach my $id2 (sort keys %test_suite) {
        defined($test_suite{$id2}{$key}) or next;
        my $sym2 = \%{ $test_suite{$id2}{$key} };
        push @{ $inserts{ $sym->{file} } }, "  $retval_type retval$id2\[" .
          scalar(@{ $sym2->{vals} }) . "] = { " . join(",", (@{ $sym2->{vals} })) . " };";
        my $vals = "";
        $vals =~ s/^, //;
        push @switch, "    case ".($id2-1).": return retval$id2\[idx++];";
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
      if ($sym->{type} =~ /\[\d+\]/) {
        $replaces{ $sym->{file} }{ $sym->{line} }{ "[[:space:]]" . $sym->{symbol} . "\\[.*\\]" } =
          "*" . $sym->{symbol} . "=$new_name\[__fshell2__tc_selector][idx__$new_name++]";
      } else {
        $replaces{ $sym->{file} }{ $sym->{line} }{ "[[:space:]]" . $sym->{symbol} } =
          $sym->{symbol} .  "=$new_name\[__fshell2__tc_selector][idx__$new_name++]";
      }
      
      my $max_size = 0;
      foreach my $id2 (sort keys %test_suite) {
        next if (!defined($test_suite{$id2}{$key}));
        my $sym2 = \%{ $test_suite{$id2}{$key} };
        $max_size = scalar(@{ $sym2->{vals} }) if (scalar(@{ $sym2->{vals} }) > $max_size);
      }
      my @vals = ();
      foreach my $id2 (sort keys %test_suite) {
        if (!defined($test_suite{$id2}{$key})) {
          my $val = "{";
          for (my $i = 0; $i < $max_size; $i++) { $val .= "0,"; }
          $val =~ s/,$//;
          $val .= "}";
          push @vals, $val;
          next;
        }
        my $sym2 = \%{ $test_suite{$id2}{$key} };
        my $val = "{" . join(",", @{ $sym2->{vals} });
        for (my $i = scalar(@{ $sym2->{vals} }); $i < $max_size; $i++) { $val .=",0"; }
        $val .= "}";
        push @vals, $val;
        $sym2->{code_done} = 1;
      }
      
      (defined($inserts{ $sym->{file} })) or $inserts{ $sym->{file} } = ();
      push @{ $inserts{ $sym->{file} } }, "unsigned idx__$new_name = 0;";
      if ($sym->{type} =~ /(\[\d+\])/) {
        my $dim = $1;
        my $type = $sym->{type};
        $type =~ s/\Q$dim\E//;
        push @{ $inserts{ $sym->{file} } }, $type . " $new_name\[" . scalar(@vals) .
          "][$max_size]$dim = { " . join(",", @vals) . " };";
      } else {
        push @{ $inserts{ $sym->{file} } }, $sym->{type} . " $new_name\[" . scalar(@vals) .
          "][$max_size] = { " . join(",", @vals) . " };";
      }
    }
  }
}

open (MAKEFILE, ">tester.mk");

print MAKEFILE <<EOF;
run_tests: tester
\t./tester

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
print MAKEFILE "\tgcc -lcunit -o \$@ \$(BUILD_FLAGS) \$^\n\n";

foreach my $f (keys %replaces) {
  print MAKEFILE "$f.mod.c: $f\n";
  print MAKEFILE "\tcp \$^ \$@\n";
  foreach my $l (keys %{ $replaces{$f} }) {
    foreach my $s (keys %{ $replaces{$f}{$l} }) {
      print MAKEFILE "\tmv \$\@ \$\@_\n";
      print MAKEFILE "\tsed '$l s/$s/ $replaces{$f}{$l}{$s}/' \$\@_ > \$\@\n";
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

print TESTER "#include <string.h>\n";
print TESTER "unsigned __fshell2__tc_selector;\n";
print TESTER "unsigned $_;\n" foreach(@idx_vars);
print TESTER "\n";

my @add_ts = ();
foreach my $id (sort keys %test_suite) {
  print TESTER "void tc$id() {\n";
  print TESTER "  __fshell2__tc_selector = " . ($id-1) . ";\n";
  print TESTER "  $_ = 0;\n" foreach(@idx_vars);
  print TESTER "  __orig__" . $test_suite{$id}{MAIN}{symbol} . "(";
  print TESTER (defined($test_suite{$id}{MAIN}{arg_vals})?join(",", @{ $test_suite{$id}{MAIN}{arg_vals} }):"") . ");\n";
  print TESTER "}\n\n";
  push @add_ts, "(NULL == CU_add_test(pSuite, \"tc$id\", tc$id))";
}


print TESTER <<EOF;
#include "CUnit/Basic.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite", init_suite, clean_suite);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if (
EOF

print TESTER join(" || ", @add_ts);

print TESTER <<EOF;
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}

EOF

