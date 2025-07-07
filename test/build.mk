#
# (c)2025 Ira Parsons
# autopledge test suite - master makefile
#

TOP := $(PWD)
TESTDIR := ./test
TESTFILE ?= $(TESTDIR)/test-results

# individual test names
TESTS := $(shell find test -maxdepth 1 -type d -path "test/test-*" | sed 's|^test/||')
TESTS-CLEAN := $(addsuffix -clean,$(TESTS))
TESTS-SPOTLESS := $(addsuffix -spotless,$(TESTS))

# individual test build files (provides test-* and poc-*)
include test/*/build.mk

.PHONY: test-all test-clean test-spotless

test-all: test-spotless $(TESTS)
	@cat $(TESTFILE)

test-clean: $(TESTS-CLEAN)

test-spotless: $(TESTS-SPOTLESS)
	-rm -f $(TESTFILE)
