
if HAVE_DOXYGEN
#directory = $(top_srcdir)/docs/man/man3/

#dist_man_MANS = $(directory)/libtopiary.3 $(directory)/libtopiary_free.3 \
#$(directory)/libtopiary_new.3 $(directory)/libtopiary_performance.3 \
#$(directory)/libtopiary_print.3 $(directory)/libtopiary_set.3 
#
#$(directory)/libtopiary.3: doxyfile.stamp
#$(directory)/libtopiary_free.3: doxyfile.stamp
#$(directory)/libtopiary_new.3: doxyfile.stamp
#$(directory)/libtopiary_performance.3: doxyfile.stamp
#$(directory)/libtopiary_print.3: doxyfile.stamp
#$(directory)/libtopiary_set.3: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp

clean-local:
	rm -rf html
	rm -rf latex

endif

# vim: noexpandtab

