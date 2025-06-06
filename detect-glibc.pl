#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# detect-glibc.pl - detect glibc calls from elf binary
#

# define promises
use constant {
    STDIO   => 1 << 0,
    RPATH   => 1 << 1,
    WPATH   => 1 << 2,
    CPATH   => 1 << 3,
    DPATH   => 1 << 4,
    CHOWN   => 1 << 5,
    FLOCK   => 1 << 6,
    TTY     => 1 << 7,
    RECVFD  => 1 << 8,
    FATTR   => 1 << 9,
    INET    => 1 << 10,
    UNIX    => 1 << 11,
    DNS     => 1 << 12,
    PROC    => 1 << 13,
    THREAD  => 1 << 14,
    ID      => 1 << 15,
    EXEC    => 1 << 16,
    TMPPATH => 1 << 17,
    VMINFO  => 1 << 18
};

# enumerate promises by glibc function
my %funcs = (
              printf => STDIO,
              scanf  => STDIO
);

# dump symbols in .dynsym section
my $raw = `objdump -T $ARGV[0]`;

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

print $_, "\n" for @syms;
printf '%d', $promises;
print "\n";
exit;
