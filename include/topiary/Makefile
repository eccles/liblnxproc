includedir ?= $(prefix)/include
INCDIR = $(includedir)/$(package)
HDRS = error.h interface.h $(package).h opt.h print.h util.h
all clean:

install:
	install -d $(DESTDIR)$(INCDIR)
	install -m 0755 $(HDRS) $(DESTDIR)$(INCDIR)

uninstall:
	-cd $(DESTDIR)$(includedir) && rm -rf $(package)

.PHONY: all clean install uninstall

# vim: noexpandtab
