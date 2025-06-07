#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# speedtest.pl - test speed reduction from autopledge
#

my $AUTOPLEDGE = '../autopledge.pl';
my $PLEDGE = '../pledge.com';
system("cc test.c -o test")
    or die "Building test failed";

my @speeds = (`/usr/bin/time ./test`, `time $PLEDGE -qVp \'stdio rpath\' ./test`, `time $AUTOPLEDGE ./test`);

my @times = ();
foreach $string (@speeds) {
    push(@times, split("\n", $string));
};

my @systimes = ();
foreach @string (@times) {
    push(@systimes, $string[2]);
};

my @final = ();
foreach $string (@systimes) {
    $string =~ /^sys .*$/;
    push(@final, $1);
};

print "RUN    TIME\n";
print "---------------\n";
print "normal " . $final[0] . "\n";
print "pledge " . $final[1] . "\n";
print "auto   " . $final[2] . "\n";
print "\n";
