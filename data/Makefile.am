if HAVE_GCOV
MYGCOV = 1
else
MYGCOV = 0
endif
if HAVE_LCOV
MYLCOV = 1
else
MYLCOV = 0
endif
if HAVE_VALGRIND
MYVALGRIND = 1
else
MYVALGRIND = 0
endif

EXTRA_DIST = proc sys var testdata test.sh
check_SCRIPTS = check-test.sh
TESTS = check-test.sh

check-test.sh:
	echo $(srcdir)/test.sh $(srcdir) $(top_srcdir) $(MYGCOV) $(MYLCOV) $(MYVALGRIND) "\"${CFLAGS}\"" > check-test.sh
	chmod +x check-test.sh

CLEANFILES = check-test.sh testoutput testoutput.diff core*

# vim: noexpandtab

