bin_PROGRAMS = sesh

check_PROGRAMS = \
	language/PrinterTest

TESTS = $(check_PROGRAMS)

catch_include = -I$(top_srcdir)/external/Catch/single_include
AM_CPPFLAGS = $(catch_include)

sesh_SOURCES = common.hh main.cc \
	language/Printable.hh \
	language/Printer.cc language/Printer.hh

language_PrinterTest_SOURCES = language/PrinterTest.cc \
	language/Printer.cc \
	language/PrinterTestHelper.hh
