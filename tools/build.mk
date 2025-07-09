#
# (c)2025 Ira Parsons
# autopledge build tools - master makefile
#

TOOLDIR := ./tools

# individual tools names
TOOLS := so-get-syscalls glibc-get-syscalls
TOOLS-CLEAN := $(addsuffix -clean,$(TOOLS))
TOOLS-SPOTLESS := $(addsuffix -spotless,$(TOOLS))

.PHONY: tools-all tools-clean tools-spotless $(TOOLS-CLEAN) $(TOOLS-SPOTLESS)

tools-all: tools-spotless $(TOOLS)

so-get-syscalls: $(TOOLDIR)/so-get-syscalls.py
	python3 -m venv $(TOOLDIR)/angr

glibc-get-syscalls: so-get-syscalls
	. $(TOOLDIR)/angr/bin/activate && \
	python3 -m pip install angr && \
	$(TOOLDIR)/so-get-syscalls.py /usr/lib64/libc.so.6 > $(TOOLDIR)/glibc-syscalls-per-function
	cat $(TOOLDIR)/glibc-syscalls-per-function | sed 's/$$/,/' > $(TOOLDIR)/glibc-syscalls-per-function

tools-clean: $(TOOLS-CLEAN)

so-get-syscalls-clean:
	-rm -rf $(TOOLDIR)/angr

glibc-get-syscalls-clean: so-get-syscalls-clean

tools-spotless: tools-clean $(TOOLS-SPOTLESS)

so-get-syscalls-spotless: so-get-syscalls-clean

glibc-get-syscalls-spotless: glibc-get-syscalls-clean so-get-syscalls-spotless
#	-rm -rf $(TOOLDIR)/glibc-syscalls-per-function
