EXTRA_DIST = topiary.sh proc sys var
check_SCRIPTS = check-topiary.sh

check-topiary.sh:
	echo './topiary.sh test nodbg' > check-topiary.sh
	chmod +x check-topiary.sh
	./check-topiary.sh

CLEANFILES = check-topiary.sh

# vim: noexpandtab

