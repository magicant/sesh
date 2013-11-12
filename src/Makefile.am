# Copyright (C) 2013 WATANABE Yuki

# This Makefile.am is free software; the author gives unlimited permission to
# copy and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY, to the extent permitted by law; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = sesh

check_PROGRAMS = \
	common/MaybeTest \
	common/VariantTest \
	language/parser/AssignmentParserImplTest \
	language/parser/BasicEnvironmentTest \
	language/parser/CommentSkipperTest \
	language/parser/EnvironmentTest \
	language/parser/PipelineParserTest \
	language/parser/RawStringParserTest \
	language/parser/SimpleCommandParserImplTest \
	language/parser/SkipperTest \
	language/parser/StringParserTest \
	language/parser/WordParserImplTest \
	language/parser/tokenTest \
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

TESTSCRIPTS = \
	checkheaderinclude.sh \
	checkincludebuildconfig.sh

TESTS = $(check_PROGRAMS) $(TESTSCRIPTS)

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)

catch_include = -I$(top_srcdir)/external/Catch/single_include
AM_CPPFLAGS = $(catch_include)

sesh_SOURCES = buildconfig.h main.cc \
	common/Char.hh \
	common/ErrorLevel.hh \
	common/Maybe.hh \
	common/OutputStream.hh \
	common/String.hh \
	common/StringStream.hh \
	common/Variant.hh \
	language/parser/AssignmentParser.cc \
	language/parser/AssignmentParser.hh \
	language/parser/AssignmentParserImpl.hh \
	language/parser/AssignmentParserImpl.tcc \
	language/parser/BasicEnvironment.cc \
	language/parser/BasicEnvironment.hh \
	language/parser/CommandParser.hh \
	language/parser/CommentSkipper.cc \
	language/parser/CommentSkipper.hh \
	language/parser/Environment.cc \
	language/parser/Environment.hh \
	language/parser/LineContinuationTreatment.hh \
	language/parser/NeedMoreSource.hh \
	language/parser/Parser.hh \
	language/parser/PipelineParser.cc \
	language/parser/PipelineParser.hh \
	language/parser/Predicate.cc \
	language/parser/Predicate.hh \
	language/parser/RawStringParser.cc \
	language/parser/RawStringParser.hh \
	language/parser/Reparse.hh \
	language/parser/SimpleCommandParser.cc \
	language/parser/SimpleCommandParser.hh \
	language/parser/SimpleCommandParserImpl.hh \
	language/parser/SimpleCommandParserImpl.tcc \
	language/parser/Skipper.cc \
	language/parser/Skipper.hh \
	language/parser/StringParser.cc \
	language/parser/StringParser.hh \
	language/parser/WordComponentParser.hh \
	language/parser/WordParser.cc \
	language/parser/WordParser.hh \
	language/parser/WordParserImpl.hh \
	language/parser/WordParserImpl.tcc \
	language/parser/token.cc \
	language/parser/token.hh \
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
common_MaybeTest_SOURCES = \
	common/MaybeTest.cc
common_VariantTest_SOURCES = \
	common/VariantTest.cc
language_parser_AssignmentParserImplTest_SOURCES = \
	language/parser/AssignmentParserImplTest.cc \
	language/parser/BasicEnvironment.cc \
	language/parser/Environment.cc \
	language/parser/Predicate.cc \
	language/parser/Skipper.cc \
	language/parser/StringParser.cc \
	language/parser/WordParserTestHelper.hh \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
language_parser_BasicEnvironmentTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/BasicEnvironmentTest.cc \
	language/parser/BasicEnvironmentTestHelper.hh \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_CommentSkipperTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/CommentSkipper.cc \
	language/parser/CommentSkipperTest.cc \
	language/parser/Environment.cc \
	language/parser/Predicate.cc \
	language/parser/Skipper.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_EnvironmentTest_SOURCES = \
	language/parser/Environment.cc \
	language/parser/EnvironmentTest.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_PipelineParserTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/CommentSkipper.cc \
	language/parser/Environment.cc \
	language/parser/PipelineParser.cc \
	language/parser/PipelineParserTest.cc \
	language/parser/Predicate.cc \
	language/parser/Skipper.cc \
	language/parser/StringParser.cc \
	language/parser/token.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc
language_parser_RawStringParserTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/Environment.cc \
	language/parser/RawStringParser.cc \
	language/parser/RawStringParserTest.cc \
	language/parser/Skipper.cc \
	language/parser/StringParser.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc
language_parser_SimpleCommandParserImplTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/CommentSkipper.cc \
	language/parser/Environment.cc \
	language/parser/Predicate.cc \
	language/parser/SimpleCommandParserImplTest.cc \
	language/parser/Skipper.cc \
	language/parser/WordParserTestHelper.hh \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/SimpleCommand.cc \
	language/syntax/Word.cc
language_parser_SkipperTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/Environment.cc \
	language/parser/Skipper.cc \
	language/parser/SkipperTest.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_StringParserTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/Environment.cc \
	language/parser/Skipper.cc \
	language/parser/StringParser.cc \
	language/parser/StringParserTest.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_WordParserImplTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/Environment.cc \
	language/parser/WordParserImplTest.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc \
	language/syntax/Word.cc
language_parser_tokenTest_SOURCES = \
	language/parser/BasicEnvironment.cc \
	language/parser/Environment.cc \
	language/parser/Predicate.cc \
	language/parser/token.cc \
	language/parser/tokenTest.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
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

checkheaderinclude.log: checkheaderinclude.sh $(SOURCES)
checkincludebuildconfig.log: checkincludebuildconfig.sh $(SOURCES)

EXTRA_DIST = $(TESTSCRIPTS)
