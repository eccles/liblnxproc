datarootdir ?= $(prefix)/share
mandir ?= $(datarootdir)/man
man3dir ?= $(mandir)/man3

MANS = topiary.3 topiary_free.3 topiary_new.3 topiary_performance.3 topiary_print.3 topiary_set.3
all clean:

install:
	install -d $(man3dir)
	for d in $(MANS); do ( gzip -9 -c $$d > $(man3dir)/$$d.gz ); done;

uninstall:
	for d in $(MANS); do ( rm -rf $(man3dir)/$$d.gz ); done;

.PHONY: all clean install uninstall

# vim: noexpandtab
