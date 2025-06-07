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
    my $entry = -1;
    foreach $string (@lines) {
        $string =~ /^\s+(.*):.{23}(.*)$/;
        $mem{$1} = $2;
        if ($entry == -1) {
            $entry = $1;
        };
    }

    # search for direct syscalls
    my @dsys = grep { $mem{$_} =~ /syscall/ } keys %mem;

    # determine syscall number for each syscall
    my @calls;
    foreach $string (@dsys) {
        my $addr = $string;
        do {
            $addr = sprintf("%x", hex($addr) - 1);
            ### TODO: ONLY WORKS FOR MOV WITH A LITERAL ###
            if($mem{$addr} =~ /\$(.*),%eax/){
                push(@calls, hex($1));
                break;
            };
        } while(hex($addr) > hex($entry));
    };

    # assign promises by syscall number
    my $promises = 0;
    foreach $num (@calls) {
        if($num <= $#syscalls){
            $promises |= $syscalls[$num];
        };
    };

    return $promises;

}

1;
