#! /usr/bin/perl
#
# configure.pl (bootstrap libtpt)
# Isaac W. Foraker (isaac@tazthecat.net)
# Use on *nix platforms.
#
# Copyright (C) 2002 Isaac W. Foraker (isaac@tazthecat.net)
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
# $Id$
#

#####
# Includes
use strict;
use Getopt::Long;
use Cwd qw(cwd chdir);

#####
# Global Variables
my $cwd = cwd();
my %clo;

my $mkmf	= "${cwd}/tools/mkmf";
my $cxxflags	= "${cwd}/tools/cxxflags";
my $mkmf_flags  = "--cxxflags '$cxxflags' --quiet ";

my $libname	= "tpt";
my $install_spec= "doc/install.spec";

my $includes	= "--include '${cwd}/inc' ";
my $libraries	= "--slinkwith '${cwd}/src/lib,$libname' ";

my @extra_compile = (
	"${cwd}/src/lib",
	"${cwd}/src/cli",
	"${cwd}/test"
);

#####
# Code Start
$|++;

GetOptions(
	\%clo,
	'help',
	'bundle',	# do not display build info
	'developer',
	'prefix=s',
	'bindir=s',
	'incdir=s',
	'libdir=s',
	'cxx=s'
) or usage();
$clo{'help'} && usage();

sub usage {
	print "Usage: $0 [options]\n", <<EOT;
  --developer        Turn on developer mode

  --prefix path      Set the install prefix to path  [/usr/local]
  --bindir path      Set the install bin dir to path [PREFIX/bin]
  --incdir path      Set the install inc dir to path [PREFIX/inc]
  --libdir path      Set the install lib dir to path [PREFIX/lib]
  --cxx    path      Specify the path to your C++ compiler (Required
                     if CXX environment variable is not set)
EOT
	exit;
}

if ($clo{'cxx'}) {
	$ENV{'CXX'} = $clo{'cxx'};
}

if (not defined $ENV{'CXX'}) {
	print STDERR <<EOT;
*** You must specify your compiler either with the --cxx command ***
*** line parameter or by setting the CXX environment variable.   ***
*** Please specify the path to your compiler when you re-run     ***
*** configure.pl.  Thanks.                                       ***
EOT
	exit 1;
}


$clo{'prefix'}	||= "/usr/local";
$clo{'bindir'}	||= "$clo{'prefix'}/bin";
$clo{'incdir'}	||= "$clo{'prefix'}/include";
$clo{'libdir'}	||= "$clo{'prefix'}/lib";
	
if ($clo{'developer'}) {
	$mkmf_flags .= "--developer ";
}

print "Generating libtpt Makefiles ";
generate_toplevel_makefile();
generate_library_makefile();
generate_cmdline_makefile();
generate_tests_makefile();
print "\n";
if (!$clo{'bundle'}) {
	print <<EOT;
+-------------------------------------------------------------+
| Okay, looks like you are ready to go.  To build, type:      |
|                                                             |
|       make                                                  |
|                                                             |
| If you would then like to run the tests, type:              |
|                                                             |
|       cd test/                                              |
|       ./test.sh                                             |
|                                                             |
| To install, type:                                           |
|                                                             |
|       make install                                          |
|                                                             |
| While you wait, why not drop a note to isaac\@tazthecat.net? |
+-------------------------------------------------------------+
EOT
}

sub generate_toplevel_makefile {
	unless (open(SPEC, ">$install_spec")) {
		print STDERR "\n$0: can't open $install_spec: $!\n";
		exit 1;
	}

	print SPEC "bindir=$clo{'bindir'}\n";
	print SPEC "binary src/cli/tpt\n";
	print SPEC "libdir=$clo{'libdir'}\n";
	print SPEC "static-lib src/lib tpt\n";
	print SPEC "includedir=$clo{'incdir'}\n";
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
