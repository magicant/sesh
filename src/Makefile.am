bin_PROGRAMS = sesh

check_PROGRAMS = \
	language/PrinterTest \
	language/RawStringTest \
	language/WordTest

TESTS = $(check_PROGRAMS)

catch_include = -I$(top_srcdir)/external/Catch/single_include
AM_CPPFLAGS = $(catch_include)

sesh_SOURCES = common.hh main.cc \
	language/Printable.hh \
	language/Printer.cc language/Printer.hh \
	language/RawString.cc language/RawString.hh \
	language/Word.cc language/Word.hh \
	language/WordComponent.hh

language_PrinterTest_SOURCES = language/PrinterTest.cc \
	language/Printer.cc \
	language/PrinterTestHelper.hh
language_RawStringTest_SOURCES = language/RawStringTest.cc \
	language/Printer.cc \
	language/RawString.cc
language_WordTest_SOURCES = language/WordTest.cc \
	language/Printer.cc \
	language/RawString.cc \
	language/Word.cc
