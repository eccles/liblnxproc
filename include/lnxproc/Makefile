INCDIR = $(prefix)/include/lnxproc
HDRS = error.h interface.h lnxproc.h opt.h print.h util.h
all clean:

install:
	install -d $(INCDIR)
	install -m 0755 $(HDRS) $(INCDIR)

uninstall:
	-cd $(INCDIR) && rm -rf $(INCDIR)

.PHONY: all clean install uninstall

# vim: noexpandtab
