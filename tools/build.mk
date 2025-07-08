#
# (c)2025 Ira Parsons
# autopledge build tools - master makefile
#

TOOLDIR := ./tools

# individual tools names
TOOLS := glibc-get-syscalls
TOOLS-CLEAN := $(addsuffix -clean,$(TOOLS))
TOOLS-SPOTLESS := $(addsuffix -spotless,$(TOOLS))

.PHONY: tools-all tools-clean tools-spotless $(TOOLS-CLEAN) $(TOOLS-SPOTLESS)

tools-all: tools-spotless $(TOOLS)

glibc-get-syscalls: $(TOOLDIR)/glibc-get-syscalls.py
	python3 -m venv $(TOOLDIR)/angr
	. $(TOOLDIR)/angr/bin/activate && \
	python3 -m pip install angr && \
	$(TOOLDIR)/glibc-get-syscalls.py

tools-clean: $(TOOLS-CLEAN)

glibc-get-syscalls-clean:
	-rm -rf $(TOOLDIR)/angr

tools-spotless: tools-clean $(TOOLS-SPOTLESS)

glibc-get-syscalls-spotless:
