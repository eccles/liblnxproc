#if USE_VALGRIND
#	TESTS_ENVIRONMENT=./valgrind-wrapper
#endif
EXTRA_DIST = proc sys var testdata test.sh
check_SCRIPTS = check-test.sh
TESTS = check-test.sh

check-test.sh:
	echo '$(srcdir)/test.sh $(srcdir)' > check-test.sh
	chmod +x check-test.sh

CLEANFILES = check-test.sh testoutput

# vim: noexpandtab

