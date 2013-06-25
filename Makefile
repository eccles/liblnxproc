package = liblnxproc
version = 0.0.1
tarname = $(package)
distdir = $(tarname)-$(version)
#--------------------------------------------------------------------

FILES=CREDITS LICENSE.txt NOTES README README.md TODO
SCRIPTS=format.sh genchanges.sh splint.conf submit.sh
MANDIR=man
SRCDIR=src
TESTDIR=test
INCDIR=include
HDRDIR=include/lnxproc
DATADIR=proc sys var
DIRS=$(SRCDIR) $(TESTDIR) $(INCDIR)
all install:
	for d in $(DIRS); do (cd $$d; $(MAKE) $@ ); done

check: all
	cd $(TESTDIR) && ./testing.sh nodbg
	@echo "*** All tests passed ***"

clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) $@ ); done
	-rm -rf $(distdir) $(distdir).tar.gz x y z

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir)/$(SRCDIR):
	mkdir -p $@
	cp $(SRCDIR)/Makefile $@
	cp $(SRCDIR)/*.[ch] $@
	cp $(SRCDIR)/*.sh $@

$(distdir)/$(TESTDIR):
	mkdir -p $@
	cp $(TESTDIR)/Makefile $@
	cp $(TESTDIR)/*.c $@
	cp $(TESTDIR)/*.sh $@
	cp $(TESTDIR)/testdata $@
	cp $(TESTDIR)/testoutput $@
	for d in $(DATADIR); do (cp -r $(TESTDIR)/$$d $@ ); done

$(distdir)/$(INCDIR):
	mkdir -p $@
	cp $(INCDIR)/Makefile $@
	cp $(INCDIR)/*.h $@

$(distdir)/$(HDRDIR):
	mkdir -p $@
	cp $(HDRDIR)/Makefile $@
	cp $(HDRDIR)/*.h $@

$(distdir)/$(MANDIR):
	mkdir -p $@
	cp $(MANDIR)/* $@

$(distdir): FORCE \
            $(distdir)/$(SRCDIR)\
            $(distdir)/$(MANDIR)\
            $(distdir)/$(TESTDIR)\
            $(distdir)/$(INCDIR)\
            $(distdir)/$(HDRDIR)
	mkdir -p $@
	cp Makefile $(FILES) $(SCRIPTS) $@

FORCE:
	-rm $(distdir).tar.gz >/dev/null 2>&1
	-rm -rf $(distdir) >/dev/null 2>&1

distcheck: $(distdir).tar.gz
	gzip -cd $(distdir).tar.gz | tar xvf -
	cd $(distdir) && $(MAKE) all
	cd $(distdir) && $(MAKE) check
	cd $(distdir) && $(MAKE) clean
	rm -rf $(distdir)
	@echo "*** Package $(distdir).tar.gz is ready for distribution ***"

	
.PHONY: FORCE all check clean dist distcheck install

# vim: noexpandtab
