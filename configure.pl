#! /usr/bin/perl
#
# configure.pl (bootstrap libtpt)
# Use on *nix platforms.
#
# Copyright (C) 2002-2006 Isaac W. Foraker (isaac@noscience.net)
# All Rights Reserved
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 
# 3. Neither the name of the Author nor the names of its contributors
#    may be used to endorse or promote products derived from this
#    software without specific prior written permission.
# 
# THIS SOFTWARE AND DOCUMENTATION IS PROVIDED BY THE AUTHOR AND
# CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
# FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
# AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# This tools uses elements of cxxtools, written by Peter J Jones.
#

#####
# Includes
use strict;
use Getopt::Long;
use Cwd qw(cwd chdir);

#####
# Global Variables
use vars qw{$opt_help $opt_bundle $opt_developer $opt_prefix $opt_bindir
			$opt_incdir $opt_incdir $opt_libdir $opt_cxx $opt_disable_shared};

# Possible names for the compiler
my @cxx_guess = qw(g++ c++ CC cl bcc32);

# Possible names for the library archiver
# For Solaris, look in
# /usr/ccs/bin/ar
# /usr/xpg4/bin/ar
# $^O == solaris

# Possible names for the linker
# For Solaris, look in
# /usr/ucb/ld
# /usr/ccs/bin/ld

# The current directory
my $cwd = cwd();

my $mkmf	= "${cwd}/tools/mkmf";
my $cxxflags	= "${cwd}/tools/cxxflags";
my $mkmf_flags  = "--cxxflags '$cxxflags' --quiet ";

my $libname	= "tpt";
my $install_spec= "doc/install.spec";

my $includes	= "--include '${cwd}/inc' ";
my $libraries	= "--slinkwith '${cwd}/src/lib,$libname' ";

# All the directories that get compiled
my @extra_compile = (
	"${cwd}/src/lib",
	"${cwd}/src/cli",
	"${cwd}/test"
);

#####
# Code Start
$|++;

GetOptions(
	'help',
	'bundle',			# do not display build info
	'disable-shared',	# For future use
	'developer',
	'prefix=s',
	'bindir=s',
	'incdir=s',
	'libdir=s',
	'cxx=s'
) or usage();
$opt_help && usage();

sub usage {
	print "Usage: $0 [options]\n", <<EOT;
  --developer        Turn on developer mode
  --bundle           Supress banners for bundling with other projects

  --prefix path      Set the install prefix to path  [/usr/local]
  --bindir path      Set the install bin dir to path [PREFIX/bin]
  --incdir path      Set the install inc dir to path [PREFIX/inc]
  --libdir path      Set the install lib dir to path [PREFIX/lib]
  --cxx    path      Specify the path to your C++ compiler (Required
                     if CXX environment variable is not set)
EOT
	exit;
}

$opt_prefix	||= "/usr/local";
$opt_bindir	||= "$opt_prefix/bin";
$opt_incdir	||= "$opt_prefix/include";
$opt_libdir	||= "$opt_prefix/lib";
$opt_disable_shared = 1 if $opt_bundle;

if ($opt_developer) {
	print "Developer extensions... enabled\n";
	$mkmf_flags.= "--developer ";
}

#####
# Determine C++ compiler settings
$opt_cxx ||= $ENV{'CXX'};
if (not $opt_cxx) {
	print "Checking C++ compiler... ";
	my $path;
	# search for a compiler
	foreach (@cxx_guess) {
		if ($path = search_path($_)) {
			$opt_cxx = "$path/$_";
			last;
		}
	}
	if ($opt_cxx) {
		print "$opt_cxx\n";
	} else {
		print <<EOT;
Not found.

You must specify your C++ compiler with the --cxx parameter or by setting the
CXX environment variable.
EOT
		exit;
	}
}
else {
	print "Using C++ compiler... $opt_cxx\n";
}
$ENV{'CXX'} = $opt_cxx;		# This will be passed into mkmf

print "Generating libtpt Makefiles ";
generate_toplevel_makefile();
generate_library_makefile();
generate_cmdline_makefile();
generate_tests_makefile();
print "\n";
if (!$opt_bundle) {
	print "\n";
	print "Install Prefix:          $opt_prefix\n";
	print "Binary Install Path:     $opt_bindir\n";
	print "Includes Install Path:   $opt_incdir\n";
	print "Libraries Install Path:  $opt_libdir\n";
	print "\n";

	print <<EOT;
===============================================================================

Configuration complete.  To build, type:

	make

To run the tests, type:

	cd test/
	./test.sh

To install, type:

	make install

===============================================================================
EOT
}

sub generate_toplevel_makefile {
	unless (open(SPEC, ">$install_spec")) {
		print STDERR "\n$0: can't open $install_spec: $!\n";
		exit 1;
	}

	print SPEC "bindir=$opt_bindir\n";
	print SPEC "binary src/cli/tpt\n";
	print SPEC "libdir=$opt_libdir\n";
	print SPEC "static-lib src/lib tpt\n";
	print SPEC "includedir=$opt_incdir\n";
	print SPEC "include-dir inc/libtpt libtpt\n";
	close SPEC;

	system("$^X $mkmf $mkmf_flags --install $install_spec --wrapper @extra_compile");
	print ".";
}


sub generate_library_makefile {
	if (not chdir("$cwd/src/lib")) {
		print STDERR "\n$0: can't chdir to src/lib: $!\n";
		exit 1;
	}

	system("$^X $mkmf $mkmf_flags $includes --static-lib $libname *.cxx");
	print ".";
	chdir $cwd;
}


sub generate_cmdline_makefile {
	if (not chdir("$cwd/src/cli")) {
		print STDERR "\n$0: can't chdir to src/cli: $!\n";
		exit 1;
	}

	system("$^X $mkmf $mkmf_flags $includes $libraries --one-exec tpt *.cxx");
	print ".";
	chdir $cwd;
}


sub generate_tests_makefile {
	if (not chdir("$cwd/test")) {
		print STDERR "\n$0: can't chdir to test: $!\n";
		exit 1;
	}

	system("$^X $mkmf $mkmf_flags $includes $libraries --quiet --many-exec *.cxx");
	print ".";
	chdir $cwd;
}

sub search_path {
	my $prog = shift;
	# Determine search paths
	my $path = $ENV{'PATH'} || $ENV{'Path'} || $ENV{'path'};
	my @paths = split /[;| |:]/, $path;

	my $ext = $^O =~ /win/i ? '.exe' : '';

	foreach (@paths) {
		if (-e "$_/$prog$ext") {
			return $_;
		}
	}
	return undef;
}

sub run_command {
	my $cmd = shift;
	my $output;

	# Note, INSTREAM is ignored.
	my $pid = open3(\*INSTREAM, \*OUTSTREAM, \*OUTSTREAM, $cmd);

	if (not $pid) {
		return undef;
	}
	while (<OUTSTREAM>) {
		$output.= $_;
	}
	waitpid($pid, 0);
	return $output;
}
