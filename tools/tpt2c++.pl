#! /usr/bin/perl
######################################################################
# Copyright (C) 2002 Peter J Jones (pjones at pmade dot org)
# All Rights Reserved
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name of the Author nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
################################################################################
#
# tpt2c++ (Convert a group of libtpt template files to one big char*)
# Peter J Jones (pjones at pmade dot org)
#
################################################################################
#
# Includes
#
################################################################################
use strict;
use Getopt::Long;
################################################################################
#
# Constants
#
################################################################################
use constant DATE		=> 'Wed Jan 16 14:41:46 2002';
################################################################################
#
# Global Variables
#
################################################################################
use vars qw($VERSION);
$VERSION = '0.0.1';

my %clo;
################################################################################
#
# Code Start
#
################################################################################
GetOptions(
    \%clo,

    'help|h!',
    'output|o=s',
    'varname|v=s',
) || usage();
$clo{'help'} && usage();
@ARGV || usage();

sub usage {
    print "Usage: $0 [options] <template file> ...\n", <<EOT;
  -h, --help         This message
  -o, --output file  File to store output
  -v, --varname name Name of variable to use
EOT
    exit;
}

$clo{'varname'} ||= "tptcode";

if ($clo{'output'}) {
    if (!open(OUT, ">$clo{'output'}")) {
	print STDERR "$0: can't open file $clo{'output'}: $!\n";
	exit 1;
    }

    select(OUT);
}

my @lines;
my $tptfile;

foreach $tptfile (@ARGV) {
    if (!open(FILE, $tptfile)) {
	print STDERR "$0: error opening $tptfile: $!\n";
	next;
    }

    push(@lines, <FILE>);
    close(FILE);
}

print "namespace {\n\tconst char $clo{'varname'}\[] =\n";

for (my $i=0; $i < @lines; ++$i) {
    # remove lines that tpt would remove anyway
    next if $lines[$i] =~ /^\@#/;

    # take off the newline
    chomp $lines[$i];

    # escape any reserved characters
    $lines[$i] =~ s/(["\\])/\\$1/g;

    # print the line wrapped in quotes
    print "\t\t\"$lines[$i]\\n\"\n";
}

print "\t\t;\n} // end anonymous namespace\n";
