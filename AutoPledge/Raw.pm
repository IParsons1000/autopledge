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
    splice(@lines, 0, 6);
    my $i = 0;
    foreach $string (@lines) {
        if($string =~ /^\s*$/) {
            splice(@lines, $i, 1);
        };
        if($string !~ /^\s/) {
	    splice(@lines, $i, 1);
        };
        $i++;
    }

    # format into address/instruction pairs
    my %mem;
    foreach $string (@lines) {
        $string =~ /^\s+(.*)\s+.*\s+(.*)$/;
        $mem{$1} = $2;
    }

print $_, "\n" for @lines;

    my $promises = 0;

    return $promises;

}

1;
