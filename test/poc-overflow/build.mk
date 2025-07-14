#
# (c)2025 Ira Parsons
# autopledge test suite - poc-overflow makefile
#

.PHONY: poc-overflow poc-overflow-clean poc-overflow-spotless

poc-overflow: $(TESTDIR)/poc-overflow/shellcode.bin $(TESTDIR)/poc-overflow/poc-overflow
	AUTOPLEDGE=$(AUTOPLEDGE) $(TESTDIR)/poc-overflow/poc-overflow.sh $(TESTFILE) $(TESTDIR)/poc-overflow/poc-overflow.log $(TESTDIR)/poc-overflow $(TOP)

$(TESTDIR)/poc-overflow/shellcode.bin: $(TESTDIR)/poc-overflow/shellcode.S
	cd $(TESTDIR)/poc-overflow && \
	nasm -f bin -o shellcode.bin shellcode.S

$(TESTDIR)/poc-overflow/poc-overflow: $(TESTDIR)/poc-overflow/poc-overflow.c
	cd $(TESTDIR)/poc-overflow && \
	cc -fno-stack-protector -z execstack -Wno-implicit-function-declaration -o poc-overflow poc-overflow.c

poc-overflow-clean:
	-rm -f $(TESTDIR)/poc-overflow/poc-overflow
	-rm -f $(TESTDIR)/poc-overflow/shellcode.bin

poc-overflow-spotless: poc-overflow-clean
	-rm -f $(TESTDIR)/poc-overflow/poc-overflow.log
