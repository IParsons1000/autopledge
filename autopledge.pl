#!/usr/bin/perl -I.
#
# (c)2025 Ira Parsons
# autopledge.pl - automatically pledge syscall promises
#

use Getopt::Std;

use AutoPledge::Core ':promises';
use AutoPledge::Glibc;
use AutoPledge::Syscall;

my $arg = 0;

# process command line arguments
getopts('hvp:', \my %opts) or die HELP_MESSAGE();
if ($opts{h}) {
    HELP_MESSAGE();
}
elsif ($opts{v}) {
    VERSION_MESSAGE();
}
elsif ($opts{p} != 1) {
    $arg = $opts{p};
}
elsif (@ARGV == 0) {
    HELP_MESSAGE();
};
my $program = $ARGV[0];
@ARGV = @ARGV[ 1 .. $#ARGV ];

# detect syscalls needed
my $promises = 0;
$promises |= AutoPledge::Glibc::detect($program);     # glibc calls
$promises |= AutoPledge::Syscall::detect($program);   # direct syscalls

# necessary to get working because pledge implementation is buggy:
$promises |= RPATH;

# convert promises into appropriate string for pledge
if ($arg) {
    $arg = $arg . ' ';
}
else {
    $arg = '';
};
$arg = $arg . AutoPledge::Core::stringify($promises);

# run linux pledge port without warnings (-q) and without unveiling (-V)
exec('./pledge.com', '-qVp', $arg, $program, @ARGV);

exit;

sub HELP_MESSAGE {
    print 'Usage: ./autopledge.pl [ -hv ] [ -p <promises> ] </path/to/binary>' . "\n";
    exit;
    return;
};

sub VERSION_MESSAGE {
    print 'autopledge 0.2' . "\n";
    exit;
    return;
};
