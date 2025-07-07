#
# (c)2025 Ira Parsons
# autopledge test suite - test-elf makefile
#

.PHONY: test-elf test-elf-clean test-elf-spotless

test-elf: $(TESTDIR)/test-elf/dummy $(TESTDIR)/test-elf/test-elf
	$(TESTDIR)/test-elf/test-elf.sh $(TESTFILE) $(TESTDIR)/test-elf/test-elf.log $(TESTDIR)/test-elf $(TOP)

$(TESTDIR)/test-elf/dummy: $(TESTDIR)/test-elf/dummy.c
	cd $(TESTDIR)/test-elf && \
	cc -o dummy dummy.c

$(TESTDIR)/test-elf/test-elf: $(TESTDIR)/test-elf/test-elf.c
	cd $(TESTDIR)/test-elf && \
	cc -o test-elf -L$(TOP) -l:$(AUTOPLEDGE) test-elf.c

test-elf-clean:
	-rm -f $(TESTDIR)/test-elf/dummy $(TESTDIR)/test-elf/test-elf

test-elf-spotless: test-elf-clean
	-rm -f $(TESTDIR)/test-elf/test-elf.log
