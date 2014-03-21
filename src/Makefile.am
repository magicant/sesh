# Copyright (C) 2013 WATANABE Yuki

# This Makefile.am is free software; the author gives unlimited permission to
# copy and/or distribute it, with or without modifications, as long as this
# notice is preserved.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY, to the extent permitted by law; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

bin_PROGRAMS = sesh

TESTPROGRAMS = \
	common/EnumSetTest \
	common/MaybeTest \
	common/MessageTest \
	common/ReferenceTest \
	common/TrieTest \
	common/VariantTest \
	language/parser/AndOrListParserTest \
	language/parser/AssignedWordParserTest \
	language/parser/AssignmentParserTest \
	language/parser/BlankAndCommentParserTest \
	language/parser/BlankCommentNewlineParserTest \
	language/parser/CharFilterTest \
	language/parser/CharParserTest \
	language/parser/CommandParserTest \
	language/parser/CommentParserTest \
	language/parser/ConverterTest \
	language/parser/EnvironmentHelperTest \
	language/parser/LineContinuationEnvironmentTest \
	language/parser/LineSequenceParserTest \
	language/parser/NewlineParserTest \
	language/parser/ParserTest \
	language/parser/PipelineParserTest \
	language/parser/RawStringParserTest \
	language/parser/RepeatTest \
	language/parser/SequenceParserTest \
	language/parser/SimpleCommandParserTest \
	language/parser/SpecificOperatorParserTest \
	language/parser/TokenParserTest \
	language/parser/TrieParserTest \
	language/parser/WordComponentParserTest \
	language/source/BufferTest \
	language/source/LineAppendedSourceTest \
	language/source/LineContinuedSourceTest \
	language/source/LocationTest \
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

TESTS = $(TESTPROGRAMS) $(TESTSCRIPTS)

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)

AM_CPPFLAGS = -I$(top_srcdir)/external/include

sesh_SOURCES = buildconfig.h main.cc \
	common/Char.hh \
	common/Empty.hh \
	common/EnumSet.hh \
	common/EnumTraits.hh \
	common/ErrorLevel.hh \
	common/Maybe.hh \
	common/Message.cc \
	common/Message.hh \
	common/OutputStream.hh \
	common/Reference.hh \
	common/String.hh \
	common/StringStream.hh \
	common/Trie.hh \
	common/Variant.hh \
	i18n/M.h \
	language/parser/AndOrListParser.cc \
	language/parser/AndOrListParser.hh \
	language/parser/AssignedWordParser.cc \
	language/parser/AssignedWordParser.hh \
	language/parser/AssignmentParser.cc \
	language/parser/AssignmentParser.hh \
	language/parser/AssignmentParserImpl.cc \
	language/parser/AssignmentParserImpl.hh \
	language/parser/BlankAndCommentParser.cc \
	language/parser/BlankAndCommentParser.hh \
	language/parser/BlankCommentNewlineParser.cc \
	language/parser/BlankCommentNewlineParser.hh \
	language/parser/CharFilter.cc \
	language/parser/CharFilter.hh \
	language/parser/CharParser.cc \
	language/parser/CharParser.hh \
	language/parser/CharPredicates.cc \
	language/parser/CharPredicates.hh \
	language/parser/CommandParser.cc \
	language/parser/CommandParser.hh \
	language/parser/CommandParserImpl.cc \
	language/parser/CommandParserImpl.hh \
	language/parser/CommentParser.cc \
	language/parser/CommentParser.hh \
	language/parser/Converter.hh \
	language/parser/DiagnosticEnvironment.cc \
	language/parser/DiagnosticEnvironment.hh \
	language/parser/Environment.hh \
	language/parser/EnvironmentHelper.cc \
	language/parser/EnvironmentHelper.hh \
	language/parser/EofEnvironment.hh \
	language/parser/FailingParser.hh \
	language/parser/IncompleteParse.cc \
	language/parser/IncompleteParse.hh \
	language/parser/Keyword.cc \
	language/parser/Keyword.hh \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/LineContinuationEnvironment.hh \
	language/parser/LineContinuationTreatment.hh \
	language/parser/LineSequenceParser.cc \
	language/parser/LineSequenceParser.hh \
	language/parser/LinebreakParser.cc \
	language/parser/LinebreakParser.hh \
	language/parser/NewlineParser.cc \
	language/parser/NewlineParser.hh \
	language/parser/Operator.cc \
	language/parser/Operator.hh \
	language/parser/OperatorParser.cc \
	language/parser/OperatorParser.hh \
	language/parser/Parser.hh \
	language/parser/ParserBase.cc \
	language/parser/ParserBase.hh \
	language/parser/PipelineParser.cc \
	language/parser/PipelineParser.hh \
	language/parser/PipelineParserImpl.cc \
	language/parser/PipelineParserImpl.hh \
	language/parser/PositionedKeyword.hh \
	language/parser/Predicate.hh \
	language/parser/RawStringParser.cc \
	language/parser/RawStringParser.hh \
	language/parser/Reparse.cc \
	language/parser/Reparse.hh \
	language/parser/Repeat.hh \
	language/parser/SequenceParser.cc \
	language/parser/SequenceParser.hh \
	language/parser/SequenceParserImpl.cc \
	language/parser/SequenceParserImpl.hh \
	language/parser/SequenceParserResult.hh \
	language/parser/SimpleCommandParser.cc \
	language/parser/SimpleCommandParser.hh \
	language/parser/SourceEnvironment.cc \
	language/parser/SourceEnvironment.hh \
	language/parser/SpecificOperatorParser.cc \
	language/parser/SpecificOperatorParser.hh \
	language/parser/StringParser.hh \
	language/parser/Token.hh \
	language/parser/TokenParser.cc \
	language/parser/TokenParser.hh \
	language/parser/TokenParserImpl.cc \
	language/parser/TokenParserImpl.hh \
	language/parser/TrieParser.hh \
	language/parser/WordComponentParser.cc \
	language/parser/WordComponentParser.hh \
	language/parser/WordComponentParserImpl.cc \
	language/parser/WordComponentParserImpl.hh \
	language/parser/WordParser.cc \
	language/parser/WordParser.hh \
	language/source/Buffer.cc \
	language/source/Buffer.hh \
	language/source/DiagnosticMessage.cc \
	language/source/DiagnosticMessage.hh \
	language/source/LineAppendedSource.cc \
	language/source/LineAppendedSource.hh \
	language/source/LineContinuedSource.cc \
	language/source/LineContinuedSource.hh \
	language/source/Location.cc \
	language/source/Location.hh \
	language/source/Origin.hh \
	language/source/Source.cc \
	language/source/Source.hh \
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
common_EnumSetTest_SOURCES = \
	common/EnumSetTest.cc
common_MaybeTest_SOURCES = \
	common/MaybeTest.cc
common_MessageTest_SOURCES = \
	common/Message.cc \
	common/MessageTest.cc
common_ReferenceTest_SOURCES = \
	common/ReferenceTest.cc
common_TrieTest_SOURCES = \
	common/TrieTest.cc
common_VariantTest_SOURCES = \
	common/VariantTest.cc
language_parser_AndOrListParserTest_SOURCES = \
	common/Message.cc \
	language/parser/AndOrListParser.cc \
	language/parser/AndOrListParserTest.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/BlankCommentNewlineParser.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommentParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/LinebreakParser.cc \
	language/parser/NewlineParser.cc \
	language/parser/Operator.cc \
	language/parser/OperatorParser.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/AndOrList.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc
language_parser_AssignedWordParserTest_SOURCES = \
	common/Message.cc \
	language/parser/AssignedWordParser.cc \
	language/parser/AssignedWordParserTest.cc \
	language/parser/CharParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
language_parser_AssignmentParserTest_SOURCES = \
	common/Message.cc \
	language/parser/AssignmentParser.cc \
	language/parser/AssignmentParserTest.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/Word.cc
language_parser_BlankAndCommentParserTest_SOURCES = \
	common/Message.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/BlankAndCommentParserTest.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommentParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_BlankCommentNewlineParserTest_SOURCES = \
	common/Message.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/BlankCommentNewlineParser.cc \
	language/parser/BlankCommentNewlineParserTest.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommentParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/NewlineParser.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_CharFilterTest_SOURCES = \
	common/Message.cc \
	language/parser/CharFilter.cc \
	language/parser/CharFilterTest.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_CharParserTest_SOURCES = \
	common/Message.cc \
	language/parser/CharParser.cc \
	language/parser/CharParserTest.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_CommandParserTest_SOURCES = \
	common/Message.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommandParser.cc \
	language/parser/CommandParserTest.cc \
	language/parser/CommentParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/Keyword.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/TokenParser.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
language_parser_CommentParserTest_SOURCES = \
	common/Message.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CommentParser.cc \
	language/parser/CommentParserTest.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_ConverterTest_SOURCES = \
	common/Message.cc \
	language/parser/ConverterTest.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/Source.cc
language_parser_EnvironmentHelperTest_SOURCES = \
	common/Message.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/EnvironmentHelperTest.cc \
	language/parser/IncompleteParse.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/Source.cc
language_parser_LineContinuationEnvironmentTest_SOURCES = \
	common/Message.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/LineContinuationEnvironmentTest.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_LineSequenceParserTest_SOURCES = \
	common/Message.cc \
	language/parser/CharParser.cc \
	language/parser/DiagnosticEnvironment.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/Keyword.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/LineSequenceParser.cc \
	language/parser/LineSequenceParserTest.cc \
	language/parser/Operator.cc \
	language/parser/OperatorParser.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/DiagnosticMessage.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Printer.cc \
	language/syntax/Sequence.cc
language_parser_NewlineParserTest_SOURCES = \
	common/Message.cc \
	language/parser/CharParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/NewlineParser.cc \
	language/parser/NewlineParserTest.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_ParserTest_SOURCES = \
	common/Message.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/ParserBase.cc \
	language/parser/ParserTest.cc \
	language/parser/Reparse.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/Source.cc
language_parser_PipelineParserTest_SOURCES = \
	common/Message.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/BlankCommentNewlineParser.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommentParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/Keyword.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/LinebreakParser.cc \
	language/parser/NewlineParser.cc \
	language/parser/Operator.cc \
	language/parser/OperatorParser.cc \
	language/parser/ParserBase.cc \
	language/parser/PipelineParser.cc \
	language/parser/PipelineParserTest.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/SpecificOperatorParser.cc \
	language/parser/TokenParser.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Assignment.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
language_parser_RawStringParserTest_SOURCES = \
	common/Message.cc \
	language/parser/CharParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/RawStringParser.cc \
	language/parser/RawStringParserTest.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc
language_parser_RepeatTest_SOURCES = \
	common/Message.cc \
	language/parser/CharParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/RepeatTest.cc \
	language/parser/SourceEnvironment.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_SequenceParserTest_SOURCES = \
	common/Message.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommentParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/Keyword.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SequenceParser.cc \
	language/parser/SequenceParserTest.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/TokenParser.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/AndOrList.cc \
	language/syntax/Assignment.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Sequence.cc \
	language/syntax/Word.cc
language_parser_SimpleCommandParserTest_SOURCES = \
	common/Message.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommentParser.cc \
	language/parser/DiagnosticEnvironment.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/Keyword.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SimpleCommandParser.cc \
	language/parser/SimpleCommandParserTest.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/TokenParser.cc \
	language/source/Buffer.cc \
	language/source/DiagnosticMessage.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/SimpleCommand.cc \
	language/syntax/Word.cc
language_parser_SpecificOperatorParserTest_SOURCES = \
	common/Message.cc \
	language/parser/CharParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/Operator.cc \
	language/parser/OperatorParser.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/SpecificOperatorParser.cc \
	language/parser/SpecificOperatorParserTest.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_TokenParserTest_SOURCES = \
	common/Message.cc \
	language/parser/BlankAndCommentParser.cc \
	language/parser/CharFilter.cc \
	language/parser/CharParser.cc \
	language/parser/CharPredicates.cc \
	language/parser/CommentParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/Keyword.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/TokenParser.cc \
	language/parser/TokenParserTest.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc \
	language/syntax/Assignment.cc \
	language/syntax/Printer.cc \
	language/syntax/RawString.cc \
	language/syntax/Word.cc
language_parser_TrieParserTest_SOURCES = \
	common/Message.cc \
	language/parser/CharParser.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/LineContinuationEnvironment.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/TrieParserTest.cc \
	language/source/Buffer.cc \
	language/source/LineContinuedSource.cc \
	language/source/Source.cc
language_parser_WordComponentParserTest_SOURCES = \
	common/Message.cc \
	language/parser/EnvironmentHelper.cc \
	language/parser/IncompleteParse.cc \
	language/parser/ParserBase.cc \
	language/parser/SourceEnvironment.cc \
	language/parser/WordComponentParser.cc \
	language/parser/WordComponentParserTest.cc \
	language/source/Buffer.cc \
	language/source/Source.cc
language_source_BufferTest_SOURCES = \
	common/Message.cc \
	language/source/Buffer.cc \
	language/source/BufferTest.cc \
	language/source/Source.cc
language_source_LineAppendedSourceTest_SOURCES = \
	common/Message.cc \
	language/source/LineAppendedSource.cc \
	language/source/LineAppendedSourceTest.cc \
	language/source/Location.cc \
	language/source/Source.cc
language_source_LineContinuedSourceTest_SOURCES = \
	common/Message.cc \
	language/source/LineContinuedSource.cc \
	language/source/LineContinuedSourceTest.cc \
	language/source/Location.cc \
	language/source/Source.cc
language_source_LocationTest_SOURCES = \
	common/Message.cc \
	language/source/Location.cc \
	language/source/LocationTest.cc
language_source_SourceTest_SOURCES = \
	common/Message.cc \
	language/source/Source.cc \
	language/source/SourceTest.cc
language_syntax_AndOrListTest_SOURCES = \
	language/source/Location.cc \
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
	language/source/Location.cc \
	language/syntax/CommandTest.cc
language_syntax_ConditionalPipelineTest_SOURCES = \
	language/source/Location.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/ConditionalPipelineTest.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc
language_syntax_PipelineTest_SOURCES = \
	language/source/Location.cc \
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
	language/source/Location.cc \
	language/syntax/AndOrList.cc \
	language/syntax/ConditionalPipeline.cc \
	language/syntax/Pipeline.cc \
	language/syntax/Printer.cc \
	language/syntax/Sequence.cc \
	language/syntax/SequenceTest.cc
language_syntax_SimpleCommandTest_SOURCES = \
	language/source/Location.cc \
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
	language/parser/AssignmentParserTestHelper.hh \
	language/parser/DiagnosticEnvironmentTestHelper.hh \
	language/parser/EnvironmentTestHelper.hh \
	language/parser/TokenParserTestHelper.hh \
	language/parser/WordParserTestHelper.hh \
	language/source/DiagnosticMessageTestHelper.hh \
	language/source/LocationTestHelper.hh \
	language/source/OriginTestHelper.hh \
	language/source/SourceTestHelper.hh \
	language/syntax/PrinterTestHelper.hh

checkheaderinclude.log: checkheaderinclude.sh $(SOURCES)
checkincludebuildconfig.log: checkincludebuildconfig.sh $(SOURCES)

EXTRA_DIST = $(TESTSCRIPTS) $(TESTHEADERS)
EXTRA_PROGRAMS = $(TESTPROGRAMS)

CLEANFILES = $(TESTPROGRAMS)
