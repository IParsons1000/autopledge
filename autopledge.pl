#!/usr/bin/perl -I.
#
# (c)2025 Ira Parsons
# autopledge.pl - automatically pledge syscall promises
#

use AutoPledge::Core;
use AutoPledge::Glibc;

# process command line arguments
my $program = $ARGV[0];

# detect syscalls needed
my $promises = AutoPledge::Glibc::detect($program);

# convert promises into appropriate string for pledge
my $arg = AutoPledge::Core::stringify($promises);

exec('./pledge.com', '-p', $arg, $program);

exit;
