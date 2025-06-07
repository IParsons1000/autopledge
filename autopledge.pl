#!/usr/bin/perl -I.
#
# (c)2025 Ira Parsons
# autopledge.pl - automatically pledge syscall promises
#

use Getopt::Std;

use AutoPledge::Core ':promises';
use AutoPledge::Glibc;
use AutoPledge::Syscall;

# process command line arguments
getopts('') or die HELP_MESSAGE();
if (@ARGV == 0) {
    HELP_MESSAGE();
};
my $program = $ARGV[0];

# detect syscalls needed
my $promises = 0;
$promises |= AutoPledge::Glibc::detect($program);     # glibc calls
$promises |= AutoPledge::Syscall::detect($program);   # direct syscalls

# necessary to get working because pledge implementation is buggy:
$promises |= RPATH;

# convert promises into appropriate string for pledge
my $arg = AutoPledge::Core::stringify($promises);

# run linux pledge port without warnings (-q) and without unveiling (-V)
exec('./pledge.com', '-qVp', $arg, $program);

exit;

sub HELP_MESSAGE {
    print 'Usage: ./autopledge.pl </path/to/binary>' . "\n";
    exit;
    return;
};

sub VERSION_MESSAGE {
    print 'autopledge 0.2' . "\n";
    exit;
    return;
};
