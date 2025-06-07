#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# AutoPledge::Syscall - detect direct linux syscalls from elf binary
#

package AutoPledge::Syscall;

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
    # int 0x80 is deprecated and syscall numbers are inconsistent for code old enough to use it
    # sysenter throws illegal instruction on 64 bit
    my @dsys = grep { $mem{$_} =~ /syscall/} keys %mem;

    # determine syscall number for each syscall
    my @calls;
    foreach $string (@dsys) {
        my $addr = $string;
        do {
            $addr = sprintf("%x", hex($addr) - 1);
            ### Works for constant mov, xor zeroing ###
            if($mem{$addr} =~ /^mov\s*\$(.*),%.ax/) {
                push(@calls, hex($1));
                break;
            }
            elsif($mem{$addr} =~ /^xor\s*%.ax,%.ax/) {
                push(@calls, 0);
                break;
            }
            elsif($mem{$addr} =~ /,%.ax/) {
                # something else is happening to the register (poisoned)
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
