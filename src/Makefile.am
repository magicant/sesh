bin_PROGRAMS = sesh

check_PROGRAMS = \
	language/source/LineAppendedSourceTest \
	language/source/LineContinuedSourceTest \
	language/source/SourceBufferTest \
	language/source/SourceLineLocationTest \
	language/source/SourceLocationTest \
	language/source/SourceTest \
	language/syntax/AndOrListTest \
	language/syntax/AssignmentTest \
	language/syntax/CommandTest \
	language/syntax/ConditionalPipelineTest \
	language/syntax/PipelineTest \
	language/syntax/PrinterTest \
	language/syntax/RawStringTest \
	language/syntax/SequenceTest \
	language/syntax/SimpleCommandTest \
	language/syntax/WordTest

TESTS = $(check_PROGRAMS)

catch_include = -I$(top_srcdir)/external/Catch/single_include
AM_CPPFLAGS = $(catch_include)

sesh_SOURCES = main.cc \
	common/Char.hh \
	common/ErrorLevel.hh \
	common/OutputStream.hh \
	common/String.hh \
	common/StringStream.hh \
	language/parser/NeedMoreSource.hh \
	language/source/LineAppendedSource.cc \
	language/source/LineAppendedSource.hh \
	language/source/LineContinuedSource.cc \
	language/source/LineContinuedSource.hh \
	language/source/Source.cc \
	language/source/Source.hh \
	language/source/SourceBuffer.cc \
	language/source/SourceBuffer.hh \
	language/source/SourceLineLocation.cc \
	language/source/SourceLineLocation.hh \
	language/source/SourceLocation.cc \
	language/source/SourceLocation.hh \
	language/syntax/AndOrList.cc \
	language/syntax/AndOrList.hh \
	language/syntax/Assignment.cc \
	language/syntax/Assignment.hh \
	language/syntax/Command.hh \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/ConditionalPipeline.hh \
	language/syntax/Pipeline.cc \
	language/syntax/Pipeline.hh \
	language/syntax/Printable.hh \
	language/syntax/Printer.cc \
	language/syntax/Printer.hh \
	language/syntax/RawString.cc \
	language/syntax/RawString.hh \
	language/syntax/Sequence.cc \
	language/syntax/Sequence.hh \
	language/syntax/SimpleCommand.cc \
	language/syntax/SimpleCommand.hh \
	language/syntax/Word.cc \
	language/syntax/Word.hh \
	language/syntax/WordComponent.hh

# Header files that are contained in sesh_SOURCES are not listed below.
language_source_LineAppendedSourceTest_SOURCES = \
	language/source/LineAppendedSource.cc \
	language/source/LineAppendedSourceTest.cc \
	language/source/Source.cc \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc
language_source_LineContinuedSourceTest_SOURCES = \
	language/source/LineContinuedSource.cc \
	language/source/LineContinuedSourceTest.cc \
	language/source/Source.cc \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc
language_source_SourceBufferTest_SOURCES = \
	language/source/Source.cc \
	language/source/SourceBuffer.cc \
	language/source/SourceBufferTest.cc
language_source_SourceLineLocationTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLineLocationTest.cc \
	language/source/SourceLineLocationTestHelper.hh
language_source_SourceLocationTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/source/SourceLocationTest.cc \
	language/source/SourceLocationTestHelper.hh
language_source_SourceTest_SOURCES = \
	language/source/Source.cc \
	language/source/SourceTest.cc \
	language/source/SourceTestHelper.hh
language_syntax_AndOrListTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/syntax/AndOrList.cc \
	language/syntax/AndOrListTest.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc
language_syntax_AssignmentTest_SOURCES = \
	language/syntax/Assignment.cc \
	language/syntax/AssignmentTest.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
language_syntax_CommandTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/syntax/CommandTest.cc
language_syntax_ConditionalPipelineTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/ConditionalPipelineTest.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc
language_syntax_PipelineTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/syntax/Pipeline.cc \
	language/syntax/PipelineTest.cc \
	language/syntax/Printer.cc
language_syntax_PrinterTest_SOURCES = \
	language/syntax/Printer.cc \
	language/syntax/PrinterTest.cc \
	language/syntax/PrinterTestHelper.hh
language_syntax_RawStringTest_SOURCES = \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/RawStringTest.cc
language_syntax_SequenceTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/syntax/AndOrList.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	language/syntax/Sequence.cc \
	language/syntax/SequenceTest.cc
language_syntax_SimpleCommandTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/SimpleCommand.cc \
	language/syntax/SimpleCommandTest.cc \
	language/syntax/Word.cc
language_syntax_WordTest_SOURCES = \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc \
	language/syntax/WordTest.cc
