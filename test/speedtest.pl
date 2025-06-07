#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# speedtest.pl - test speed reduction from autopledge
#

my $AUTOPLEDGE = './autopledge.pl';
my $PLEDGE = '../pledge.com';
system('cc test.c -o test');

my @speeds = (`{ /usr/bin/time -p ./test ; } 2>&1`, `{ /usr/bin/time -p $PLEDGE -qVp \'stdio rpath\' ./test ; } 2>&1`, `{ cd .. ; /usr/bin/time -p $AUTOPLEDGE test/test ; } 2>&1`);

system('rm test');

my @times = ();
foreach $string (@speeds) {
    push(@times, split("\n", $string));
};

my @final = ();
foreach $string (@times) {
    if($string =~ /^sys (.*)$/) {
        push(@final, $1);
    };
};

print "RUN    TIME\n";
print "-----------\n";
print "normal " . $final[0] . "\n";
print "pledge " . $final[1] . "\n";
print "auto   " . $final[2] . "\n";
