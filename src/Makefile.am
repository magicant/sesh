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
	common/MessageTest \
	common/VariantTest \
	language/parser/CharFilterTest \
	language/parser/EnvironmentHelperTest \
	language/parser/LineContinuationEnvironmentTest \
	language/parser/ParserTest \
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

AM_CPPFLAGS = -I$(top_srcdir)/external/include

sesh_SOURCES = buildconfig.h main.cc \
	common/Char.hh \
	common/ErrorLevel.hh \
	common/Maybe.hh \
	common/Message.cc \
	common/Message.hh \
	common/OutputStream.hh \
	common/String.hh \
	common/StringStream.hh \
	common/Variant.hh \
	i18n/M.h \
	language/parser/CharFilter.cc \
	language/parser/CharFilter.hh \
	language/parser/Environment.hh \
	language/parser/EnvironmentHelper.cc \
	language/parser/EnvironmentHelper.hh \
	language/parser/EofEnvironment.hh \
	language/parser/IncompleteParse.cc \
	language/parser/IncompleteParse.hh \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/LineContinuationEnvironment.hh \
	language/parser/LineContinuationTreatment.hh \
	language/parser/NormalParser.hh \
	language/parser/Parser.hh \
	language/parser/ParserBase.cc \
	language/parser/ParserBase.hh \
	language/parser/Predicate.hh \
	language/parser/Reparse.cc \
	language/parser/Reparse.hh \
	language/parser/SourceEnvironment.cc \
	language/parser/SourceEnvironment.hh \
	language/source/DiagnosticMessage.cc \
	language/source/DiagnosticMessage.hh \
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
common_MessageTest_SOURCES = \
	common/Message.cc \
	common/MessageTest.cc
common_VariantTest_SOURCES = \
	common/VariantTest.cc
language_parser_CharFilterTest_SOURCES = \
	language/parser/CharFilter.cc \
	language/parser/CharFilterTest.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_EnvironmentHelperTest_SOURCES = \
	language/parser/EnvironmentHelper.cc \
	language/parser/EnvironmentHelperTest.cc \
	language/parser/IncompleteParse.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_LineContinuationEnvironmentTest_SOURCES = \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/LineContinuationEnvironmentTest.cc \
	language/parser/SourceEnvironment.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/source/SourceBuffer.cc
language_parser_ParserTest_SOURCES = \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/ParserBase.cc \
	language/parser/ParserTest.cc \
	language/parser/Reparse.cc \
	language/parser/SourceEnvironment.cc \
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
	language/source/SourceLineLocationTest.cc
language_source_SourceLocationTest_SOURCES = \
	language/source/SourceLineLocation.cc \
	language/source/SourceLocation.cc \
	language/source/SourceLocationTest.cc
language_source_SourceTest_SOURCES = \
	language/source/Source.cc \
	language/source/SourceTest.cc
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
	language/syntax/PrinterTest.cc
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

TESTHEADERS = \
	language/parser/EnvironmentTestHelper.hh \
	language/source/DiagnosticMessageTestHelper.hh \
	language/source/SourceLineLocationTestHelper.hh \
	language/source/SourceLocationTestHelper.hh \
	language/source/SourceTestHelper.hh \
	language/syntax/PrinterTestHelper.hh

checkheaderinclude.log: checkheaderinclude.sh $(SOURCES)
checkincludebuildconfig.log: checkincludebuildconfig.sh $(SOURCES)

EXTRA_DIST = $(TESTSCRIPTS) $(TESTHEADERS)
