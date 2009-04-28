#!/usr/bin/perl -w

use strict;

# update all Makefile.am in $@ and their subdirectories, if given in the SUBDIRS
# variable. Recursive operation is performed in the background

use Getopt::Long;

my $print_usage = 0;
my $not_really = 0;
my $install_headers = 0;
my $diagnostics = 0;
my $verbose = 0;

GetOptions( 
  "help" => \$print_usage,
  "not-really" => \$not_really,
  "install-headers" => \$install_headers,
  "diagnostics" => \$diagnostics,
  "verbose" => \$verbose
);

#
# Message about this program and how to use it
#
( 1 == $print_usage ) and print STDERR << "EOF";

This program does...

usage: $0 [-hnid] directories ...

 -h        : this (help) message
 -n        : not-really, exits immediately
 -i        : get header files installed
 -d        : use diagnostics
 -v        : verbose

example: $0 -i -d directory1 directory2

EOF

( ( 1 == $print_usage ) or ( 1 == $not_really ) or ( ! $ARGV[ 0 ] ) ) and exit 0;

my @opts = ();
( 1 == $install_headers ) and push @opts, "--install-headers";
( 1 == $diagnostics ) and push @opts, "--diagnostics";


use Cwd qw/ cwd abs_path /;
use File::Spec;
use File::Basename;

my $self = abs_path( $0 );
my $short_name = basename( $self );

# call ourselves recursively, does nothing in case of an empty list of
# arguments
while( scalar( @ARGV ) > 1 )
{
  my $dir = shift @ARGV;
  my $pid = fork();
  defined( $pid ) or die "Failed to fork!\n";
  ( 0 == $pid ) and ( exec $self, @opts, $dir  or die "Failed to run $self\n" );
}

# make sure that $ARGV[ 0 ] is indeed a directory
( -d $ARGV[ 0 ] ) or die $ARGV[ 0 ] . " is not a directory\n";
chdir $ARGV[ 0 ] or die "Failed to chdir to " . $ARGV[ 0 ] . "\n";

#in case there is no Makefile.am in $ARGV[ 0 ], generate one
if( ! -f "Makefile.am" )
{
  warn "No Makefile.am found in " . $ARGV[ 0 ] . ", generating one\n";
  open( MF, ">Makefile.am" );
  print MF <<"EOF";
# This Makefile.am is maintained by config/$short_name, modifications may
# get lost. You need only change and maintain the "SUBDIRS =" line and possibly
# some LDADD for test cases, as directed by comments in case a test case is
# detected.
include \$(top_srcdir)/model.am

# Add all desired subdirectories here, like
# SUBDIRS = dir1 dir2 dirX
SUBDIRS =

EOF
  close( MF );
}

# get the full pathname of configure.ac
my $configure_ac = File::Spec->catfile(
  abs_path( 
    File::Spec->catdir( dirname( $self ), File::Spec->updir ) ),
  "configure.ac" );
( -f $configure_ac ) or die "configure.ac not found as $configure_ac\n";

# get the name of the Makefile relative to $configure_ac
my $makefile = File::Spec->abs2rel( abs_path( "Makefile" ), dirname( $configure_ac ) );

# check whether configure.ac resp. AC_CONFIG_FILES already includes $makefile
# do we have AC_CONFIG_FILES at all?
# do we have AC_OUTPUT at all?
`grep -q AC_OUTPUT $configure_ac`;
if( $? >> 8 )
{
  `echo "AC_OUTPUT" >> $configure_ac`;
  warn "Added missing AC_OUTPUT to $configure_ac\n";
}
`grep -q AC_CONFIG_FILES $configure_ac`;
if( $? >> 8 )
{
  `perl -p -i -e 's/AC_OUTPUT/AC_CONFIG_FILES([])\\nAC_OUTPUT/' $configure_ac`;
  warn "Added missing AC_CONFIG_FILES to $configure_ac\n";
}
# now go and check
use Fcntl ':flock';
`cat $configure_ac | sed -n '/AC_CONFIG_FILES(\\[/,/\\])/p' | grep -q '$makefile'`;
if( $? >> 8 )
{
  open(CONFIGURE_AC, "<$configure_ac");
  flock(CONFIGURE_AC, LOCK_EX);
  `cp $configure_ac $configure_ac.edit ; \\
    perl -p -i -e 's{(AC_CONFIG_FILES\\(\\[)}{\$1$makefile\\n                 }' $configure_ac.edit ; \\
    mv $configure_ac.edit $configure_ac`;
  flock(CONFIGURE_AC, LOCK_UN);
  close(CONFIGURE_AC);
  warn "Added $makefile to AC_CONFIG_FILES\n";
}

my @array_makefile_am = ();

# read entry lines for variable $entry and return them as seen
sub get_makefile_entry()
{
  my ( $entry ) = @_;
  my $start = -1;
  my $k = 0;
  my @val = ();

  foreach( @array_makefile_am )
  {
    my $line = $_;
    $start = $k if( ( $line =~ /^$entry\s*=/ ) || ( $line =~ /^$entry \s*\+=/ ));
    $k++;
    next if( -1 == $start );

    push @val, $line;

    $start = -1 unless( $line =~ /\\$/ );
  }

  return @val;
}

# just collect all the values of $entry
sub get_variable_values()
{
  my ( $entry ) = @_;
  my $start = -1;
  my $k = 0;
  my @val = ();

  foreach( @array_makefile_am )
  {
    my $line = $_;
    $start = $k if( ( $line =~ /^$entry\s*=/ ) || ( $line =~ /^$entry \s*\+=/ ));
    $k++;
    next if( -1 == $start );

    ( /^($entry\s*(\+)?=)?\s*(.*?)\s*(\\)?$/ ) or die "Internal error - $entry line does not match\n";
    push @val, $_  foreach (split(/\s+/, $3));

    $start = -1 unless( $line =~ /\\$/ );
  }

  return @val;
}


sub set_makefile_entry()
{
  my ( $entry, @val ) = @_;
  
  warn "Updating $makefile.am\n";
  
  for( my $k = 0; $k < scalar( @array_makefile_am ); $k++ )
  {
    my $line = $array_makefile_am[ $k ];
    my $sep = "";

    $sep = "=" if( $line =~ /^$entry\s*=/ );
    $sep = "+=" if( $line =~ /^$entry\s*\+=/ );
    next if( $sep eq "" );

    # remove the current entry
    splice( @array_makefile_am, $k, 1 ) while( $array_makefile_am[ $k ] =~ /\\$/);
    splice( @array_makefile_am, $k, 1 );
    # insert the new entry
    if( defined( $val[ 0 ] ) && $val[ 0 ] =~ /^$entry\s*\Q$sep\E/ )
    {
      while( $val[ 0 ] =~ /\\$/ )
      {
        splice( @array_makefile_am, $k, 0, shift @val );
        $k++;
      }
      # remove trailing whitespace
      $val[ 0 ] =~ s#\s*$##;
      splice( @array_makefile_am, $k, 0, shift @val );
    }
  }

  push @array_makefile_am, @val if( scalar( @val ) > 0 );
}

sub update_makefile_entry()
{
  my ( $entry, @val ) = @_;
  my $entry_mod = 0;

  if( scalar( @val ) > 0 )
  {
    my @e_entry = &get_makefile_entry( $entry );

    foreach( @e_entry )
    {
      # check, that only files from @val are part of $entry
      # and make sure all @val are provided in $entry
      ( /^($entry\s*(\+)?=)?\s*(.*?)\s*(\\)?$/ ) or die "Internal error - $entry line does not match\n";

      foreach my $y ( split( /\s+/, $3 ) )
      {
        my $num_wanted_files = scalar( @val );
        for( my $i = 0; $i < $num_wanted_files; $i++ )
        {
          my $f = shift @val;
          last if( $y eq $f );
          push @val, $f;
        }
        # file is not in list
        if( $num_wanted_files == scalar( @val ) )
        {
          $_ =~ s#(=|\s+)$y(\s+|\\|$)#$1$2#;
          $entry_mod = 1;
        }
      }
    }

    push @e_entry, "$entry =" if( 0 == scalar( @e_entry ) );
    foreach( @val )
    {
      $e_entry[ -1 ] .= " $_";
      $entry_mod = 1;
    }

    if( 1 == $entry_mod )
    {
      &set_makefile_entry( $entry, @e_entry );
    }
  }
  else
  {
    if( scalar( grep( /^$entry\s*(\+)?=/, @array_makefile_am ) ) > 0 )
    {
      &set_makefile_entry( $entry );
      $entry_mod = 1;
    }
  }

  return $entry_mod;
}

use Tie::File;
print "Working on " . File::Spec->catfile( cwd(), "Makefile.am" ) . "\n" if( 1 == $verbose );
tie( @array_makefile_am, "Tie::File", "Makefile.am" );

my @subdirs = ();

foreach( &get_makefile_entry( "SUBDIRS" ) )
{
  ( /^(SUBDIRS\s*(\+)?=)?\s*(.*?)\s*(\\)?$/ ) or die "Internal error - SUBDIRS line does not match\n";
  push @subdirs, split( /\s+/, $3 );
}

untie @array_makefile_am;
if( scalar( @subdirs ) > 0 )
{
  my $pid = fork();
  defined( $pid ) or die "Failed to fork!\n";
  ( 0 == $pid ) and ( exec $self, @opts, @subdirs  or die "Failed to run $self\n" );
}

@array_makefile_am = ();
tie( @array_makefile_am, "Tie::File", "Makefile.am" );
if( scalar( grep( /NO_UPDATE_MAKEFILES/, @array_makefile_am ) ) > 0 )
{
  untie @array_makefile_am;
  warn "Not touching $makefile.am\n";
  while( wait() != -1 ) {}
  exit 0;
}

# update all entries related to .c .cpp, .ll, .yy, but not .t.cpp
opendir( CWD, cwd() );
my @wanted_files = grep( !/\.t\.cpp$/, grep( /(\.ll|\.yy|\.cpp|\.c)$/, readdir( CWD ) ) );
closedir( CWD );

if( scalar( @wanted_files ) > 0 )
{
  my @lib_LT = &get_makefile_entry( "lib_LTLIBRARIES" );
  my @noinst_LT = &get_makefile_entry( "noinst_LTLIBRARIES" );
  my $libname = "lib" . basename( cwd() ) . ".la";

  if( 0 == scalar( @lib_LT ) && 0 == scalar( @noinst_LT ) )
  {
    &set_makefile_entry( "noinst_LTLIBRARIES", "noinst_LTLIBRARIES = $libname");
  }
  elsif( scalar( @noinst_LT ) > 0 && 
    0 == scalar( grep( /(=|\s*)\Q$libname\E(\s+|\\|$)/, @lib_LT ) ) )
  {
    if( 0 == scalar( grep( /(=|\s*)\Q$libname\E(\s+|\\|$)/, @noinst_LT ) ) )
    {
      $noinst_LT[ -1 ] .= " $libname";
      &set_makefile_entry( "noinst_LTLIBRARIES", @noinst_LT );
    }
  }
  else
  {
    if( 0 == scalar( grep( /(=|\s*)\Q$libname\E(\s+|\\|$)/, @lib_LT ) ) )
    {
      $lib_LT[ -1 ] .= " $libname";
      &set_makefile_entry( "lib_LTLIBRARIES", @lib_LT );
    }
  }
}
else
{
  my @lib_LT = &get_makefile_entry( "lib_LTLIBRARIES" );
  my @noinst_LT = &get_makefile_entry( "noinst_LTLIBRARIES" );
  my $libname = "lib" . basename( cwd() ) . ".la";

  if( scalar( @noinst_LT ) > 0 )
  {
    foreach( grep( /(=|\s*)\Q$libname\E(\s+|\\|$)/, @noinst_LT ) )
    {
      $_ =~ s#(=|\s+)\Q$libname\E(\s+|\\|$)#$1$2#;
    }
    &set_makefile_entry( "noinst_LTLIBRARIES", @noinst_LT );
  }
  
  if( scalar( @lib_LT ) > 0 )
  {
    foreach( grep( /(=|\s*)\Q$libname\E(\s+|\\|$)/, @lib_LT ) )
    {
      $_ =~ s#(=|\s+)\Q$libname\E(\s+|\\|$)#$1$2#;
    }
    &set_makefile_entry( "lib_LTLIBRARIES", @lib_LT );
  }

}
  
utime undef, undef, $configure_ac 
  if( 1 == &update_makefile_entry( "lib" . basename( cwd() ) . "_la_SOURCES", @wanted_files ) );

# update _HEADERS entries to include all *.h *.hpp files, either noinst_HEADERS or
# include_<DIR>_HEADERS
my $e = "noinst_HEADERS";

if( 1 == $install_headers )
{
  my $rel_path = File::Spec->abs2rel( cwd(), dirname( $configure_ac ) );
  my $rel_path__ = "";
  if( "" ne $rel_path )
  {
    my $rel_path__v = "";
    my $rel_path__f = "";
    ( $rel_path__v,$rel_path__, $rel_path__f ) = File::Spec->splitpath( $rel_path );
    $rel_path__ = join( "_", ( "", 
        File::Spec->splitdir( File::Spec->catfile( $rel_path__, $rel_path__f ) ) ) );
    $rel_path__ =~ s#_$##;
    $rel_path__ =~ s#^__#_#;
    my $i = "include" . $rel_path__ . "dir";
    my $v = File::Spec->catdir( "\$(includedir)", "$rel_path" );
    &set_makefile_entry( $i, "$i = $v" ) 
      if( 0 == scalar( grep( /^$i\s*=\s*\Q$v\E/, @array_makefile_am ) ) );
  }
  $e = "include" . $rel_path__ . "_HEADERS";
}

opendir( CWD, cwd() );
@wanted_files = grep( !/\.t\.hpp$/, grep( /(\.hpp|\.h)$/, readdir( CWD ) ) );
closedir( CWD );
my %wanted_files_hash = ();
@wanted_files_hash{ @wanted_files } = ();
# remove BUILT_SOURCES from the list
delete $wanted_files_hash{$_} foreach (&get_variable_values("BUILT_SOURCES"));
&update_makefile_entry( $e, keys %wanted_files_hash );

# add tests, i.e., shell scripts and .t.cpp programs
opendir( CWD, cwd() );
@wanted_files = ();
foreach( readdir( CWD ) )
{
  open( F, $_ );
  push @wanted_files, $_ if( <F> =~ /^#!\/bin\/sh/ );
  close( F );
  unless( 1 == $diagnostics )
  {
    my $f = $_;
    next unless( /\.t\.cpp$/ );
    $f =~ s/\.t\.cpp$//;
    push @wanted_files, $f;
  }
}
closedir( CWD );
&update_makefile_entry( "TESTS", @wanted_files );

# update check_PROGRAMS and generate code to build the tests from *.t.cpp, if
# required
opendir( CWD, cwd() );
@wanted_files = ();
foreach( readdir( CWD ) )
{
  my $f = $_;
  next unless( /\.t\.cpp$/ );
  $f =~ s/\.t\.cpp$//;
  push @wanted_files, $f;
}
closedir( CWD );
&update_makefile_entry( "check_PROGRAMS", @wanted_files );

foreach( @wanted_files )
{
  my $n = $_ . "_SOURCES";
  my @fs = ( $_ . ".t.cpp" );
  push @fs, $_ . ".t.hpp" if( -f $_ . ".t.hpp" );
  &update_makefile_entry( $n, @fs );
  $n = $_ . "_LDADD";
  if( 0 == scalar( grep( /^$n\s*=/, @array_makefile_am ) ) )
  {
    warn "Updating $makefile.am\n";
    push @array_makefile_am, (
      "# add additional objects required to build $_ here, like",
      "# $n = ../base/libbase.la libbla.la",
      "$n =",
      "# note that this entry (" . $_ . "_SOURCES, $n) will not be removed",
      "# by $short_name, if the sources files are removed;",
      "# it must then be removed manually"
    );
  }

}

untie @array_makefile_am;
# print "Waited for " . wait() . "\n";
while( wait() != -1 ) {}
exit 0;

