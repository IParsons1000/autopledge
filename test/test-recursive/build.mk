#
# (c)2025 Ira Parsons
# autopledge test suite - test-recursive makefile
#

.PHONY: test-recursive test-recursive-clean test-recursive-spotless

test-recursive: $(TESTDIR)/test-recursive/c.so $(TESTDIR)/test-recursive/b.so $(TESTDIR)/test-recursive/test-recursive
	AUTOPLEDGE=$(AUTOPLEDGE) $(TESTDIR)/test-recursive/test-recursive.sh $(TESTFILE) $(TESTDIR)/test-recursive/test-recursive.log $(TESTDIR)/test-recursive $(TOP)

$(TESTDIR)/test-recursive/c.so: $(TESTDIR)/test-recursive/c.c
	cd $(TESTDIR)/test-recursive && \
	cc -o c.so c.c -fPIC -shared

$(TESTDIR)/test-recursive/b.so: $(TESTDIR)/test-recursive/b.c
	cd $(TESTDIR)/test-recursive && \
	cc -o b.so b.c -fPIC -shared -L. -l:c.so 

$(TESTDIR)/test-recursive/test-recursive: $(TESTDIR)/test-recursive/test-recursive.c
	cd $(TESTDIR)/test-recursive && \
	LD_LIBRARY_PATH=. cc -o test-recursive test-recursive.c -L. -l:b.so

test-recursive-clean:
	-rm -f $(TESTDIR)/test-recursive/test-recursive $(TESTDIR)/test-recursive/b.so $(TESTDIR)/test-recursive/c.so

test-recursive-spotless: test-recursive-clean
	-rm -f $(TESTDIR)/test-recursive/test-recursive.log
