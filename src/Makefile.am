bin_PROGRAMS = sesh

check_PROGRAMS = \
	language/syntax/AndOrListTest \
	language/syntax/AssignmentTest \
	language/syntax/CommandTest \
	language/syntax/ConditionalPipelineTest \
	language/syntax/PipelineTest \
	language/syntax/PrinterTest \
	language/syntax/RawStringTest \
	language/syntax/SequenceTest \
	language/syntax/SimpleCommandTest \
	language/syntax/WordTest \
	parser/LineAppendedSourceTest \
	parser/LineContinuedSourceTest \
	parser/SourceLineLocationTest \
	parser/SourceLocationTest \
	parser/SourceTest

TESTS = $(check_PROGRAMS)

catch_include = -I$(top_srcdir)/external/Catch/single_include
AM_CPPFLAGS = $(catch_include)

sesh_SOURCES = common.hh main.cc \
	language/syntax/AndOrList.cc language/syntax/AndOrList.hh \
	language/syntax/Assignment.cc language/syntax/Assignment.hh \
	language/syntax/Command.hh \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/ConditionalPipeline.hh \
	language/syntax/Pipeline.cc language/syntax/Pipeline.hh \
	language/syntax/Printable.hh \
	language/syntax/Printer.cc language/syntax/Printer.hh \
	language/syntax/RawString.cc language/syntax/RawString.hh \
	language/syntax/Sequence.cc language/syntax/Sequence.hh \
	language/syntax/SimpleCommand.cc language/syntax/SimpleCommand.hh \
	language/syntax/Word.cc language/syntax/Word.hh \
	language/syntax/WordComponent.hh \
	parser/LineAppendedSource.cc parser/LineAppendedSource.hh \
	parser/LineContinuedSource.cc parser/LineContinuedSource.hh \
	parser/Source.cc parser/Source.hh \
	parser/SourceLineLocation.cc parser/SourceLineLocation.hh \
	parser/SourceLocation.cc parser/SourceLocation.hh

language_syntax_AndOrListTest_SOURCES = language/syntax/AndOrListTest.cc \
	language/syntax/AndOrList.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_syntax_AssignmentTest_SOURCES = language/syntax/AssignmentTest.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
language_syntax_CommandTest_SOURCES = language/syntax/CommandTest.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_syntax_ConditionalPipelineTest_SOURCES = \
	language/syntax/ConditionalPipelineTest.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_syntax_PipelineTest_SOURCES = language/syntax/PipelineTest.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_syntax_PrinterTest_SOURCES = language/syntax/PrinterTest.cc \
	language/syntax/Printer.cc \
	language/syntax/PrinterTestHelper.hh
language_syntax_RawStringTest_SOURCES = language/syntax/RawStringTest.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc
language_syntax_SequenceTest_SOURCES = language/syntax/SequenceTest.cc \
	language/syntax/AndOrList.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	language/syntax/Sequence.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_syntax_SimpleCommandTest_SOURCES = \
	language/syntax/SimpleCommandTest.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/SimpleCommand.cc \
	language/syntax/Word.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
language_syntax_WordTest_SOURCES = language/syntax/WordTest.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
parser_LineAppendedSourceTest_SOURCES = parser/LineAppendedSourceTest.cc \
	parser/LineAppendedSource.cc \
	parser/Source.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
parser_LineContinuedSourceTest_SOURCES = parser/LineContinuedSourceTest.cc \
	parser/LineContinuedSource.cc \
	parser/Source.cc \
	parser/SourceLineLocation.cc \
	parser/SourceLocation.cc
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
