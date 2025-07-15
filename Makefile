#
# (c)2025 Ira Parsons
# autopledge - automatically sandbox syscalls
#

# custom config
AUTOPLEDGE ?= ./libautopledge.so
TOP ?= $(PWD)
PREFIX ?= /usr
LIBDIR ?= $(PREFIX)/lib64

# default and optional build settings
CC ?= cc
CFLAGS = -Wall
CFLAGS += -Wextra
CFLAGS += -Werror
CFLAGS += -g
CFLAGS += -fPIC
COPTS ?=
ifeq ($(LENIENT),1)
COPTS += -DLENIENT
endif
ifeq ($(USE_SYSLOG),0)
else ifeq ($(USE_STDERR),1)
COPTS += -DUSE_STDERR
else ifeq ($(USE_NOLOGS),1)
else
COPTS += -DUSE_SYSLOG
endif
CFLAGS += $(COPTS)

.PHONY: all test tools install uninstall clean spotless

all: $(AUTOPLEDGE)

$(AUTOPLEDGE): autopledge.o elf.o syscalls.o seccomp.o glibc.o glibcxx.o
	$(CC) $(CFLAGS) -shared -o $(AUTOPLEDGE) autopledge.o elf.o syscalls.o seccomp.o glibc.o glibcxx.o

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

glibcxx.o: glibcxx.c
	$(CC) $(CFLAGS) -c glibcxx.c

# module build files (provides `*-all', `*-clean', and `*-spotless')
include test/build.mk
include tools/build.mk

test: all test-all

tools: tools-all

install: all
	install $(AUTOPLEDGE) $(LIBDIR)

uninstall:
	rm -f $(LIBDIR)/$(AUTOPLEDGE)

clean: test-clean tools-clean
	-rm -f *.o

spotless: test-spotless tools-spotless
	-rm -f $(AUTOPLEDGE) *.o
