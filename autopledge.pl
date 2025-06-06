#!/usr/bin/perl -I.
#
# (c)2025 Ira Parsons
# autopledge.pl - automatically pledge syscall promises
#

use AutoPledge::Core ':promises';
use AutoPledge::Glibc;

# process command line arguments
my $program = $ARGV[0];

# detect syscalls needed
my $promises = AutoPledge::Glibc::detect($program);

# necessary to get working because pledge implementation is buggy:
$promises |= RPATH;

# convert promises into appropriate string for pledge
my $arg = AutoPledge::Core::stringify($promises);

# run linux pledge port without warnings (-q) and without unveiling (-V)
exec('./pledge.com', '-qVp', $arg, $program);

exit;
