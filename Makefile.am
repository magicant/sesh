ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src

dist_doc_DATA = LICENSE README
EXTRA_DIST = \
	doc/doxygen/Doxyfile \
	external/Catch/LICENSE_1_0.txt \
	external/Catch/README.md \
	external/Catch/single_include/catch.hpp

.PHONY: doxygen clean-local-doxygen
doxygen:
	@cd doc/doxygen && doxygen
clean-local-doxygen:
	-rm -fr doc/doxygen/html

clean-local: clean-local-doxygen
