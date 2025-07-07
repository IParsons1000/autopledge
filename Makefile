#
# (c)2025 Ira Parsons
# autopledge - automatically sandbox syscalls
#

# default and optional build settings
CC ?= cc
CFLAGS = -Wall
CFLAGS += -Wextra
CFLAGS += -Werror
CFLAGS += -g
CFLAGS += -fPIC -shared
COPTS ?=
CFLAGS += $(COPTS)

# module build files (provides `test-all', `test-clean', and `test-spotless')
include test/build.mk

.PHONY: all test clean spotless

all: libautopledge.so

libautopledge.so: autopledge.o elf.o syscalls.o seccomp.o glibc.o
	$(CC) $(CFLAGS) -o libautopledge.so autopledge.o elf.o syscalls.o seccomp.o glibc.o

autopledge.o: autopledge.c
	$(CC) $(CFLAGS) -c autopledge.c

elf.o: elf.c
	$(CC) $(CFLAGS) -c elf.c

syscalls.o: syscalls.c
	$(CC) $(CFLAGS) -c syscalls.c

seccomp.o: seccomp.c
	$(CC) $(CFLAGS) -c seccomp.c

glibc.o: glibc.c
	$(CC) $(CFLAGS) -c glibc.c

test: all test-all

clean: test-clean
	-rm -f *.o

spotless: test-spotless
	-rm -f libautopledge.so *.o
