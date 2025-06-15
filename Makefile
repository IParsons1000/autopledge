#
# (c)2025 Ira Parsons
# autopledge - automatically sandbox syscalls
#

CC ?= cc
CFLAGS = -Wall
CFLAGS += -Wextra
CFLAGS += -Werror
CFLAGS += -g

.PHONY: all clean spotless

all: autopledge

autopledge: autopledge.o
	$(CC) $(CFLAGS) -o autopledge autopledge.o

autopledge.o: autopledge.c
	$(CC) $(CFLAGS) -c autopledge.c

clean:
	-rm -f *.o

spotless:
	-rm -f autopledge *.o
