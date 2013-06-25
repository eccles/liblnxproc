package = liblnxproc
version = 0.0.1
tarname = $(package)
distdir = $(tarname)-$(version)

SRCDIR=src
TESTDIR=test
INCDIR=include
HDRDIR=include/lnxproc
DATADIR=proc sys var
DIRS=$(SRCDIR) $(TESTDIR) $(INCDIR)
all clean:
	for d in $(DIRS); do (cd $$d; $(MAKE) $@ ); done

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir):
	mkdir -p $(distdir)
	cp Makefile $(distdir)
	mkdir -p $(distdir)/$(SRCDIR)
	cp $(SRCDIR)/Makefile $(distdir)/$(SRCDIR)
	cp $(SRCDIR)/*.[ch] $(distdir)/$(SRCDIR)
	cp $(SRCDIR)/*.sh $(distdir)/$(SRCDIR)
	mkdir -p $(distdir)/$(TESTDIR)
	cp $(TESTDIR)/Makefile $(distdir)/$(TESTDIR)
	cp $(TESTDIR)/*.c $(distdir)/$(TESTDIR)
	cp $(TESTDIR)/*.sh $(distdir)/$(TESTDIR)
	for d in $(DATADIR); do (cp -r $(TESTDIR)/$$d $(distdir)/$(TESTDIR);\
                          ); done
	mkdir -p $(distdir)/$(INCDIR)
	cp $(INCDIR)/Makefile $(distdir)/$(INCDIR)
	mkdir -p $(distdir)/$(HDRDIR)
	cp $(HDRDIR)/Makefile $(distdir)/$(HDRDIR)
	cp $(HDRDIR)/*.h $(distdir)/$(HDRDIR)

.PHONY: all clean dist

# vim: noexpandtab
