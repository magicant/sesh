bin_PROGRAMS = sesh

check_PROGRAMS = \
	language/AssignmentTest \
	language/CommandTest \
	language/PrinterTest \
	language/RawStringTest \
	language/SourceLocationTest \
	language/WordTest

TESTS = $(check_PROGRAMS)

catch_include = -I$(top_srcdir)/external/Catch/single_include
AM_CPPFLAGS = $(catch_include)

sesh_SOURCES = common.hh main.cc \
	language/Assignment.cc language/Assignment.hh \
	language/Command.hh \
	language/Printable.hh \
	language/Printer.cc language/Printer.hh \
	language/RawString.cc language/RawString.hh \
	language/SourceLocation.cc language/SourceLocation.hh \
	language/Word.cc language/Word.hh \
	language/WordComponent.hh

language_AssignmentTest_SOURCES = language/AssignmentTest.cc \
	language/Assignment.cc \
	language/Printer.cc \
	language/RawString.cc \
	language/Word.cc
language_CommandTest_SOURCES = language/CommandTest.cc \
	language/SourceLocation.cc
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
language_SourceLocationTest_SOURCES = language/SourceLocationTest.cc \
	language/SourceLocation.cc \
	language/SourceLocationTestHelper.hh
