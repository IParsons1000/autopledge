#
# (c)2025 Ira Parsons
# autopledge test suite - poc-llama makefile
#
# CVE-2025-49847
#

.PHONY: poc-llama poc-llama-clean poc-llama-spotless

poc-llama: $(TESTDIR)/poc-llama/llama.cpp $(TESTDIR)/poc-llama/shellcode.bin $(TESTDIR)/poc-llama/poc-llama
	AUTOPLEDGE=$(AUTOPLEDGE) $(TESTDIR)/poc-llama/poc-llama.sh $(TESTFILE) $(TESTDIR)/poc-llama/poc-llama.log $(TESTDIR)/poc-llama $(TOP)

$(TESTDIR)/poc-llama/llama.cpp:
	cd $(TESTDIR)/poc-llama && \
	git clone https://github.com/ggml-org/llama.cpp.git && \
	cd llama.cpp && \
	git reset --hard 80709b7 && \
	cmake -B build && \
	cmake --build build --config Release

$(TESTDIR)/poc-llama/shellcode.bin: $(TESTDIR)/poc-llama/shellcode.S
	cd $(TESTDIR)/poc-llama && \
	nasm -f bin -o shellcode.bin shellcode.S

$(TESTDIR)/poc-llama/poc-llama: $(TESTDIR)/poc-llama/poc-llama.c
	cd $(TESTDIR)/poc-llama && \
	cc -o poc-llama poc-llama.c

poc-llama-clean:
	-rm -f $(TESTDIR)/poc-llama/poc-llama
	-rm -rf $(TESTDIR)/poc-llama/llama.cpp
	-rm -f $(TESTDIR)/poc-llama/shellcode.bin

poc-llama-spotless: poc-llama-clean
	-rm -f $(TESTDIR)/poc-llama/poc-llama.log
