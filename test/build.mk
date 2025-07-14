#
# (c)2025 Ira Parsons
# autopledge test suite - master makefile
#

TESTDIR := ./test
TESTFILE ?= $(TESTDIR)/test-results

# individual test names
TESTS := $(shell find test -maxdepth 1 -type d -path "test/test-*" | sed 's|^test/||')
POCS := $(shell find test -maxdepth 1 -type d -path "test/poc-*" | sed 's|^test/||')
ALLTEST := $(TESTS) $(POCS)
TESTS-CLEAN := $(addsuffix -clean,$(ALLTEST))
TESTS-SPOTLESS := $(addsuffix -spotless,$(ALLTEST))

# individual test build files (provides test-* and poc-*)
include test/*/build.mk

.PHONY: test-all test-clean test-spotless $(TESTS-CLEAN) $(TESTS-SPOTLESS)

test-all: test-spotless $(TESTS)
	@cat $(TESTFILE)

test-clean: $(TESTS-CLEAN)

test-spotless: $(TESTS-SPOTLESS)
	-rm -f $(TESTFILE)
