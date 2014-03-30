# Copyright (C) 2013 WATANABE Yuki

# This Makefile.am is free software; the author gives unlimited permission to
# copy and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY, to the extent permitted by law; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

### Basic definitions

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/external/include

### Binaries

bin_PROGRAMS = src/sesh

### Tests

TESTPROGRAMS = \
	src/async/DelayHolderTest \
	src/async/DelayTest \
	src/async/FutureTest \
	src/async/PromiseTest \
	src/async/ResultTest \
	src/common/EnumSetTest \
	src/common/MaybeTest \
	src/common/MessageTest \
	src/common/ReferenceTest \
	src/common/SharedFunctionTest \
	src/common/TrieTest \
	src/common/VariantTest \
	src/language/parser/AndOrListParserTest \
	src/language/parser/AssignedWordParserTest \
	src/language/parser/AssignmentParserTest \
	src/language/parser/BlankAndCommentParserTest \
	src/language/parser/BlankCommentNewlineParserTest \
	src/language/parser/CharFilterTest \
	src/language/parser/CharParserTest \
	src/language/parser/CommandParserTest \
	src/language/parser/CommentParserTest \
	src/language/parser/ConverterTest \
	src/language/parser/EnvironmentHelperTest \
	src/language/parser/LineContinuationEnvironmentTest \
	src/language/parser/LineSequenceParserTest \
	src/language/parser/NewlineParserTest \
	src/language/parser/ParserTest \
	src/language/parser/PipelineParserTest \
	src/language/parser/RawStringParserTest \
	src/language/parser/RepeatTest \
	src/language/parser/SequenceParserTest \
	src/language/parser/SimpleCommandParserTest \
	src/language/parser/SpecificOperatorParserTest \
	src/language/parser/TokenParserTest \
	src/language/parser/TrieParserTest \
	src/language/parser/WordComponentParserTest \
	src/language/source/BufferTest \
	src/language/source/LineAppendedSourceTest \
	src/language/source/LineContinuedSourceTest \
	src/language/source/LocationTest \
	src/language/source/SourceTest \
	src/language/syntax/AndOrListTest \
	src/language/syntax/AssignmentTest \
	src/language/syntax/CommandTest \
	src/language/syntax/ConditionalPipelineTest \
	src/language/syntax/PipelineTest \
	src/language/syntax/PrinterTest \
	src/language/syntax/RawStringTest \
	src/language/syntax/SequenceTest \
	src/language/syntax/SimpleCommandTest \
	src/language/syntax/WordTest

TESTSCRIPTS = \
	src/checkheaderinclude.sh \
	src/checkincludebuildconfig.sh

TESTS = $(TESTPROGRAMS) $(TESTSCRIPTS)

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)

src/checkheaderinclude.log: src/checkheaderinclude.sh $(SOURCES)
src/checkincludebuildconfig.log: src/checkincludebuildconfig.sh $(SOURCES)

### Source files

src_sesh_SOURCES = \
	src/async/Delay.hh \
	src/async/DelayHolder.hh \
	src/async/Future.hh \
	src/async/Future.tcc \
	src/async/Promise.hh \
	src/async/Result.hh \
	src/buildconfig.h \
	src/common/Char.hh \
	src/common/DirectInitialize.hh \
	src/common/Empty.hh \
	src/common/EnumSet.hh \
	src/common/EnumTraits.hh \
	src/common/ErrorLevel.hh \
	src/common/IntegerSequence.hh \
	src/common/Maybe.hh \
	src/common/Message.cc \
	src/common/Message.hh \
	src/common/OutputStream.hh \
	src/common/Reference.hh \
	src/common/SharedFunction.hh \
	src/common/String.hh \
	src/common/StringStream.hh \
	src/common/Trie.hh \
	src/common/Variant.hh \
	src/helpermacros.h \
	src/i18n/M.h \
	src/language/parser/AndOrListParser.cc \
	src/language/parser/AndOrListParser.hh \
	src/language/parser/AssignedWordParser.cc \
	src/language/parser/AssignedWordParser.hh \
	src/language/parser/AssignmentParser.cc \
	src/language/parser/AssignmentParser.hh \
	src/language/parser/AssignmentParserImpl.cc \
	src/language/parser/AssignmentParserImpl.hh \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/BlankAndCommentParser.hh \
	src/language/parser/BlankCommentNewlineParser.cc \
	src/language/parser/BlankCommentNewlineParser.hh \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharFilter.hh \
	src/language/parser/CharParser.cc \
	src/language/parser/CharParser.hh \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CharPredicates.hh \
	src/language/parser/CommandParser.cc \
	src/language/parser/CommandParser.hh \
	src/language/parser/CommandParserImpl.cc \
	src/language/parser/CommandParserImpl.hh \
	src/language/parser/CommentParser.cc \
	src/language/parser/CommentParser.hh \
	src/language/parser/Converter.hh \
	src/language/parser/DiagnosticEnvironment.cc \
	src/language/parser/DiagnosticEnvironment.hh \
	src/language/parser/Environment.hh \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/EnvironmentHelper.hh \
	src/language/parser/EofEnvironment.hh \
	src/language/parser/FailingParser.hh \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/IncompleteParse.hh \
	src/language/parser/Keyword.cc \
	src/language/parser/Keyword.hh \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/LineContinuationEnvironment.hh \
	src/language/parser/LineContinuationTreatment.hh \
	src/language/parser/LineSequenceParser.cc \
	src/language/parser/LineSequenceParser.hh \
	src/language/parser/LinebreakParser.cc \
	src/language/parser/LinebreakParser.hh \
	src/language/parser/NewlineParser.cc \
	src/language/parser/NewlineParser.hh \
	src/language/parser/Operator.cc \
	src/language/parser/Operator.hh \
	src/language/parser/OperatorParser.cc \
	src/language/parser/OperatorParser.hh \
	src/language/parser/Parser.hh \
	src/language/parser/ParserBase.cc \
	src/language/parser/ParserBase.hh \
	src/language/parser/PipelineParser.cc \
	src/language/parser/PipelineParser.hh \
	src/language/parser/PipelineParserImpl.cc \
	src/language/parser/PipelineParserImpl.hh \
	src/language/parser/PositionedKeyword.hh \
	src/language/parser/Predicate.hh \
	src/language/parser/RawStringParser.cc \
	src/language/parser/RawStringParser.hh \
	src/language/parser/Reparse.cc \
	src/language/parser/Reparse.hh \
	src/language/parser/Repeat.hh \
	src/language/parser/SequenceParser.cc \
	src/language/parser/SequenceParser.hh \
	src/language/parser/SequenceParserImpl.cc \
	src/language/parser/SequenceParserImpl.hh \
	src/language/parser/SequenceParserResult.hh \
	src/language/parser/SimpleCommandParser.cc \
	src/language/parser/SimpleCommandParser.hh \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/SourceEnvironment.hh \
	src/language/parser/SpecificOperatorParser.cc \
	src/language/parser/SpecificOperatorParser.hh \
	src/language/parser/StringParser.hh \
	src/language/parser/Token.hh \
	src/language/parser/TokenParser.cc \
	src/language/parser/TokenParser.hh \
	src/language/parser/TokenParserImpl.cc \
	src/language/parser/TokenParserImpl.hh \
	src/language/parser/TrieParser.hh \
	src/language/parser/WordComponentParser.cc \
	src/language/parser/WordComponentParser.hh \
	src/language/parser/WordComponentParserImpl.cc \
	src/language/parser/WordComponentParserImpl.hh \
	src/language/parser/WordParser.cc \
	src/language/parser/WordParser.hh \
	src/language/source/Buffer.cc \
	src/language/source/Buffer.hh \
	src/language/source/DiagnosticMessage.cc \
	src/language/source/DiagnosticMessage.hh \
	src/language/source/LineAppendedSource.cc \
	src/language/source/LineAppendedSource.hh \
	src/language/source/LineContinuedSource.cc \
	src/language/source/LineContinuedSource.hh \
	src/language/source/Location.cc \
	src/language/source/Location.hh \
	src/language/source/Origin.hh \
	src/language/source/Source.cc \
	src/language/source/Source.hh \
	src/language/syntax/AndOrList.cc \
	src/language/syntax/AndOrList.hh \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Assignment.hh \
	src/language/syntax/Command.hh \
	src/language/syntax/ConditionalPipeline.cc \
	src/language/syntax/ConditionalPipeline.hh \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Pipeline.hh \
	src/language/syntax/Printable.hh \
	src/language/syntax/Printer.cc \
	src/language/syntax/Printer.hh \
	src/language/syntax/RawString.cc \
	src/language/syntax/RawString.hh \
	src/language/syntax/Sequence.cc \
	src/language/syntax/Sequence.hh \
	src/language/syntax/SimpleCommand.cc \
	src/language/syntax/SimpleCommand.hh \
	src/language/syntax/Word.cc \
	src/language/syntax/Word.hh \
	src/language/syntax/WordComponent.hh \
	src/main.cc

# Header files that are contained in sesh_SOURCES are not listed below.
src_async_DelayHolderTest_SOURCES = \
	src/async/DelayHolderTest.cc
src_async_DelayTest_SOURCES = \
	src/async/DelayTest.cc
src_async_FutureTest_SOURCES = \
	src/async/FutureTest.cc
src_async_PromiseTest_SOURCES = \
	src/async/PromiseTest.cc
src_async_ResultTest_SOURCES = \
	src/async/ResultTest.cc
src_common_EnumSetTest_SOURCES = \
	src/common/EnumSetTest.cc
src_common_MaybeTest_SOURCES = \
	src/common/MaybeTest.cc
src_common_MessageTest_SOURCES = \
	src/common/Message.cc \
	src/common/MessageTest.cc
src_common_ReferenceTest_SOURCES = \
	src/common/ReferenceTest.cc
src_common_SharedFunctionTest_SOURCES = \
	src/common/SharedFunctionTest.cc
src_common_TrieTest_SOURCES = \
	src/common/TrieTest.cc
src_common_VariantTest_SOURCES = \
	src/common/VariantTest.cc
src_language_parser_AndOrListParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/AndOrListParser.cc \
	src/language/parser/AndOrListParserTest.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/BlankCommentNewlineParser.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/LinebreakParser.cc \
	src/language/parser/NewlineParser.cc \
	src/language/parser/Operator.cc \
	src/language/parser/OperatorParser.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/AndOrList.cc \
	src/language/syntax/ConditionalPipeline.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc
src_language_parser_AssignedWordParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/AssignedWordParser.cc \
	src/language/parser/AssignedWordParserTest.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/Word.cc
src_language_parser_AssignmentParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/AssignmentParser.cc \
	src/language/parser/AssignmentParserTest.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/Word.cc
src_language_parser_BlankAndCommentParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/BlankAndCommentParserTest.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_BlankCommentNewlineParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/BlankCommentNewlineParser.cc \
	src/language/parser/BlankCommentNewlineParserTest.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/NewlineParser.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_CharFilterTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharFilterTest.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_CharParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharParserTest.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_CommandParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommandParser.cc \
	src/language/parser/CommandParserTest.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/Keyword.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/TokenParser.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/Word.cc
src_language_parser_CommentParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/CommentParserTest.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_ConverterTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/ConverterTest.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/Source.cc
src_language_parser_EnvironmentHelperTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/EnvironmentHelperTest.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/Source.cc
src_language_parser_LineContinuationEnvironmentTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/LineContinuationEnvironmentTest.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_LineSequenceParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/DiagnosticEnvironment.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/Keyword.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/LineSequenceParser.cc \
	src/language/parser/LineSequenceParserTest.cc \
	src/language/parser/Operator.cc \
	src/language/parser/OperatorParser.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/DiagnosticMessage.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/Sequence.cc
src_language_parser_NewlineParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/NewlineParser.cc \
	src/language/parser/NewlineParserTest.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_ParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/ParserTest.cc \
	src/language/parser/Reparse.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/Source.cc
src_language_parser_PipelineParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/BlankCommentNewlineParser.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/Keyword.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/LinebreakParser.cc \
	src/language/parser/NewlineParser.cc \
	src/language/parser/Operator.cc \
	src/language/parser/OperatorParser.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/PipelineParser.cc \
	src/language/parser/PipelineParserTest.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/SpecificOperatorParser.cc \
	src/language/parser/TokenParser.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/Word.cc
src_language_parser_RawStringParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/RawStringParser.cc \
	src/language/parser/RawStringParserTest.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc
src_language_parser_RepeatTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/RepeatTest.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_SequenceParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/Keyword.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SequenceParser.cc \
	src/language/parser/SequenceParserTest.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/TokenParser.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/AndOrList.cc \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/Sequence.cc \
	src/language/syntax/Word.cc
src_language_parser_SimpleCommandParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/DiagnosticEnvironment.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/Keyword.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SimpleCommandParser.cc \
	src/language/parser/SimpleCommandParserTest.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/TokenParser.cc \
	src/language/source/Buffer.cc \
	src/language/source/DiagnosticMessage.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/SimpleCommand.cc \
	src/language/syntax/Word.cc
src_language_parser_SpecificOperatorParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/Operator.cc \
	src/language/parser/OperatorParser.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/SpecificOperatorParser.cc \
	src/language/parser/SpecificOperatorParserTest.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_TokenParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/BlankAndCommentParser.cc \
	src/language/parser/CharFilter.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/CharPredicates.cc \
	src/language/parser/CommentParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/Keyword.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/TokenParser.cc \
	src/language/parser/TokenParserTest.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/Word.cc
src_language_parser_TrieParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/CharParser.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/LineContinuationEnvironment.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/TrieParserTest.cc \
	src/language/source/Buffer.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/Source.cc
src_language_parser_WordComponentParserTest_SOURCES = \
	src/common/Message.cc \
	src/language/parser/EnvironmentHelper.cc \
	src/language/parser/IncompleteParse.cc \
	src/language/parser/ParserBase.cc \
	src/language/parser/SourceEnvironment.cc \
	src/language/parser/WordComponentParser.cc \
	src/language/parser/WordComponentParserTest.cc \
	src/language/source/Buffer.cc \
	src/language/source/Source.cc
src_language_source_BufferTest_SOURCES = \
	src/common/Message.cc \
	src/language/source/Buffer.cc \
	src/language/source/BufferTest.cc \
	src/language/source/Source.cc
src_language_source_LineAppendedSourceTest_SOURCES = \
	src/common/Message.cc \
	src/language/source/LineAppendedSource.cc \
	src/language/source/LineAppendedSourceTest.cc \
	src/language/source/Location.cc \
	src/language/source/Source.cc
src_language_source_LineContinuedSourceTest_SOURCES = \
	src/common/Message.cc \
	src/language/source/LineContinuedSource.cc \
	src/language/source/LineContinuedSourceTest.cc \
	src/language/source/Location.cc \
	src/language/source/Source.cc
src_language_source_LocationTest_SOURCES = \
	src/common/Message.cc \
	src/language/source/Location.cc \
	src/language/source/LocationTest.cc
src_language_source_SourceTest_SOURCES = \
	src/common/Message.cc \
	src/language/source/Source.cc \
	src/language/source/SourceTest.cc
src_language_syntax_AndOrListTest_SOURCES = \
	src/language/source/Location.cc \
	src/language/syntax/AndOrList.cc \
	src/language/syntax/AndOrListTest.cc \
	src/language/syntax/ConditionalPipeline.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc
src_language_syntax_AssignmentTest_SOURCES = \
	src/language/syntax/Assignment.cc \
	src/language/syntax/AssignmentTest.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/Word.cc
src_language_syntax_CommandTest_SOURCES = \
	src/language/source/Location.cc \
	src/language/syntax/CommandTest.cc
src_language_syntax_ConditionalPipelineTest_SOURCES = \
	src/language/source/Location.cc \
	src/language/syntax/ConditionalPipeline.cc \
	src/language/syntax/ConditionalPipelineTest.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc
src_language_syntax_PipelineTest_SOURCES = \
	src/language/source/Location.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/PipelineTest.cc \
	src/language/syntax/Printer.cc
src_language_syntax_PrinterTest_SOURCES = \
	src/language/syntax/Printer.cc \
	src/language/syntax/PrinterTest.cc
src_language_syntax_RawStringTest_SOURCES = \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/RawStringTest.cc
src_language_syntax_SequenceTest_SOURCES = \
	src/language/source/Location.cc \
	src/language/syntax/AndOrList.cc \
	src/language/syntax/ConditionalPipeline.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/Sequence.cc \
	src/language/syntax/SequenceTest.cc
src_language_syntax_SimpleCommandTest_SOURCES = \
	src/language/source/Location.cc \
	src/language/syntax/Assignment.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/SimpleCommand.cc \
	src/language/syntax/SimpleCommandTest.cc \
	src/language/syntax/Word.cc
src_language_syntax_WordTest_SOURCES = \
	src/language/syntax/Printer.cc \
	src/language/syntax/RawString.cc \
	src/language/syntax/Word.cc \
	src/language/syntax/WordTest.cc

TESTHEADERS = \
	src/language/parser/AssignmentParserTestHelper.hh \
	src/language/parser/DiagnosticEnvironmentTestHelper.hh \
	src/language/parser/EnvironmentTestHelper.hh \
	src/language/parser/TokenParserTestHelper.hh \
	src/language/parser/WordParserTestHelper.hh \
	src/language/source/DiagnosticMessageTestHelper.hh \
	src/language/source/LocationTestHelper.hh \
	src/language/source/OriginTestHelper.hh \
	src/language/source/SourceTestHelper.hh \
	src/language/syntax/PrinterTestHelper.hh

### Documentation

.PHONY: doxygen clean-local-doxygen
doxygen:
	@cd doc/doxygen && doxygen
clean-local-doxygen:
	-rm -fr doc/doxygen/html

### Distributions

dist_doc_DATA = LICENSE README

### Extras

EXTRA_DIST = \
	$(TESTHEADERS) \
	$(TESTSCRIPTS) \
	doc/doxygen/Doxyfile \
	external
EXTRA_PROGRAMS = \
	$(TESTPROGRAMS)

### Clean

CLEANFILES = $(TESTPROGRAMS)

clean-local: clean-local-doxygen
