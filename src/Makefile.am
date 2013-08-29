bin_PROGRAMS = sesh

check_PROGRAMS = \
	language/AndOrListTest \
	language/AssignmentTest \
	language/CommandTest \
	language/ConditionalPipelineTest \
	language/PipelineTest \
	language/PrinterTest \
	language/RawStringTest \
	language/SequenceTest \
	language/SimpleCommandTest \
	language/WordTest \
	parser/SourceLineLocationTest \
	parser/SourceLocationTest \
	parser/SourceTest

TESTS = $(check_PROGRAMS)

catch_include = -I$(top_srcdir)/external/Catch/single_include
AM_CPPFLAGS = $(catch_include)

sesh_SOURCES = common.hh main.cc \
	language/AndOrList.cc language/AndOrList.hh \
	language/Assignment.cc language/Assignment.hh \
	language/Command.hh \
	language/ConditionalPipeline.cc language/ConditionalPipeline.hh \
	language/Pipeline.cc language/Pipeline.hh \
	language/Printable.hh \
	language/Printer.cc language/Printer.hh \
	language/RawString.cc language/RawString.hh \
	language/Sequence.cc language/Sequence.hh \
	language/SimpleCommand.cc language/SimpleCommand.hh \
	language/Word.cc language/Word.hh \
	language/WordComponent.hh \
	parser/Source.cc parser/Source.hh \
	parser/SourceLineLocation.cc parser/SourceLineLocation.hh \
	parser/SourceLocation.cc parser/SourceLocation.hh

language_AndOrListTest_SOURCES = language/AndOrListTest.cc \
	language/AndOrList.cc \
	language/ConditionalPipeline.cc \
	language/Pipeline.cc \
	language/Printer.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_AssignmentTest_SOURCES = language/AssignmentTest.cc \
	language/Assignment.cc \
	language/Printer.cc \
	language/RawString.cc \
	language/Word.cc
language_CommandTest_SOURCES = language/CommandTest.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_ConditionalPipelineTest_SOURCES = language/ConditionalPipelineTest.cc\
	language/ConditionalPipeline.cc \
	language/Pipeline.cc \
	language/Printer.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_PipelineTest_SOURCES = language/PipelineTest.cc \
	language/Pipeline.cc \
	language/Printer.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_PrinterTest_SOURCES = language/PrinterTest.cc \
	language/Printer.cc \
	language/PrinterTestHelper.hh
language_RawStringTest_SOURCES = language/RawStringTest.cc \
	language/Printer.cc \
	language/RawString.cc
language_SequenceTest_SOURCES = language/SequenceTest.cc \
	language/AndOrList.cc \
	language/ConditionalPipeline.cc \
	language/Pipeline.cc \
	language/Printer.cc \
	language/Sequence.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_SimpleCommandTest_SOURCES = language/SimpleCommandTest.cc \
	language/Assignment.cc \
	language/Printer.cc \
	language/RawString.cc \
	language/SimpleCommand.cc \
	language/Word.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_WordTest_SOURCES = language/WordTest.cc \
	language/Printer.cc \
	language/RawString.cc \
	language/Word.cc
parser_SourceLineLocationTest_SOURCES = parser/SourceLineLocationTest.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLineLocationTestHelper.hh
parser_SourceLocationTest_SOURCES = parser/SourceLocationTest.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc \
	parser/SourceLocationTestHelper.hh
parser_SourceTest_SOURCES = parser/SourceTest.cc \
	parser/Source.cc \
	parser/SourceTestHelper.hh
