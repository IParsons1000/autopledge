#
# (c)2025 Ira Parsons
# autopledge test suite - master makefile
#

# individual test names
TESTS = $(find . -maxdepth 1 -regex "test-.*$")

# individual test build files (provides test-* and poc-*)
include test/*/build.mk

.PHONY: test-all test-clean test-spotless

test-all: $(TESTS)

test-clean: $(wildcard $(addsuffix -clean,$(TESTS)))

test-spotless: $(wildcard $(addsuffix -spotless,$(TESTS)))
