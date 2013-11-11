# Copyright (C) 2013 WATANABE Yuki

# This Makefile.am is free software; the author gives unlimited permission to
# copy and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY, to the extent permitted by law; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

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
