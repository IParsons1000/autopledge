#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# AutoPledge::Glibc - detect glibc calls from elf binary
#

package AutoPledge::Glibc;

use AutoPledge::Core ':promises';

# enumerate promises by glibc function
my %funcs = (
              printf => STDIO,
              __isoc23_scanf  => STDIO | TTY
);

sub detect {

    # dump symbols in .dynsym section
    my $raw = `objdump -T $_[0]`;

    # form lines into array
    my @allsyms = split("\n", $raw);

    # pull glibc calls from formatted output
    my @syms = ();
    foreach $string (@allsyms) {
        if ($string =~ /.*\(GLIBC.*\)\s*(.*)$/) {
        push(@syms, $1);
        }
    }

    # assign appropriate promises
    my $promises = 0;
    foreach $sym (@syms) {
        $promises |= $funcs{$sym} if exists $funcs{$sym};
    }

    return $promises;

}

1;
