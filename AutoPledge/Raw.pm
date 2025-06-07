#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# AutoPledge::Raw - detect direct linux syscalls from elf binary
#

package AutoPledge::Raw;

use AutoPledge::Core ':promises';

# enumerate promises by syscall number
my @syscalls = (
              STDIO  # read = 0
);

sub detect {

    # get .text section of binary and disassemble
    my $raw = `objdump -Dj .text $_[0]`;

    # form lines into array
    my @lines = split("\n", $raw);

    # filter out lint
    my $i = 0;
    foreach $string (@lines) {
        if($string =~ /^\s*$/) {
            splice(@lines, $i, 1);
        }
        elsif($string !~ /^\s/) {
	    splice(@lines, $i, 1);
        }
        $i++;
    }

print $lines[0];

    my $promises = 0;

    return $promises;

}

1;
