#!/usr/bin/perl
#
# (c)2025 Ira Parsons
# AutoPledge::Core - shared definitions
#

package AutoPledge::Core;

use Exporter qw(import);

our @EXPORT_OK;

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

our @EXPORT_OK = ('STDIO', 'RPATH', 'WPATH', 'CPATH', 'DPATH', 'CHOWN', 'FLOCK', 'TTY', 'RECVFD', 'FATTR', 'INET', 'UNIX', 'DNS', 'PROC', 'THREAD', 'ID', 'EXEC', 'TMPPATH', 'VMINFO');
our %EXPORT_TAGS = (promises => [ 'STDIO', 'RPATH', 'WPATH', 'CPATH', 'DPATH', 'CHOWN', 'FLOCK', 'TTY', 'RECVFD', 'FATTR', 'INET', 'UNIX', 'DNS', 'PROC', 'THREAD', 'ID', 'EXEC', 'TMPPATH', 'VMINFO' ]);

sub stringify {

    my @pretty = ();

    push(@pretty, 'stdio') if ($_[0] & STDIO);
    push(@pretty, 'rpath') if ($_[0] & RPATH);
    push(@pretty, 'wpath') if ($_[0] & WPATH);
    push(@pretty, 'cpath') if ($_[0] & CPATH);
    push(@pretty, 'dpath') if ($_[0] & DPATH);
    push(@pretty, 'chown') if ($_[0] & CHOWN);
    push(@pretty, 'flock') if ($_[0] & FLOCK);
    push(@pretty, 'tty') if ($_[0] & TTY);
    push(@pretty, 'recvfd') if ($_[0] & RECVFD);
    push(@pretty, 'fattr') if ($_[0] & FATTR);
    push(@pretty, 'inet') if ($_[0] & INET);
    push(@pretty, 'unix') if ($_[0] & UNIX);
    push(@pretty, 'dns') if ($_[0] & DNS);
    push(@pretty, 'proc') if ($_[0] & PROC);
    push(@pretty, 'thread') if ($_[0] & THREAD);
    push(@pretty, 'id') if ($_[0] & ID);
    push(@pretty, 'exec') if ($_[0] & EXEC);
    push(@pretty, 'tmppath') if ($_[0] & TMPPATH);
    push(@pretty, 'vminfo') if ($_[0] & VMINFO);

    return join(' ', @pretty);

};

1;
