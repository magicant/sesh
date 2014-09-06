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
	src/async/delay_holder_test \
	src/async/delay_test \
	src/async/future_test \
	src/async/promise_test \
	src/async/shared_future_test \
	src/common/container_helper_test \
	src/common/enum_iterator_test \
	src/common/enum_set_test \
	src/common/maybe_test \
	src/common/message_test \
	src/common/reference_test \
	src/common/shared_function_test \
	src/common/trial_test \
	src/common/trie_test \
	src/common/type_tag_test \
	src/common/variant_test \
	src/language/source/buffer_test \
	src/language/source/line_appended_source_test \
	src/language/source/line_continued_source_test \
	src/language/source/location_test \
	src/language/source/source_test \
	src/language/syntax/and_or_list_test \
	src/language/syntax/AssignmentTest \
	src/language/syntax/CommandTest \
	src/language/syntax/ConditionalPipelineTest \
	src/language/syntax/PipelineTest \
	src/language/syntax/PrinterTest \
	src/language/syntax/RawStringTest \
	src/language/syntax/SequenceTest \
	src/language/syntax/SimpleCommandTest \
	src/language/syntax/WordTest \
	src/os/event/AwaiterErrorFileDescriptorTest \
	src/os/event/AwaiterFileDescriptorTest \
	src/os/event/AwaiterReadableFileDescriptorTest \
	src/os/event/AwaiterSignalTest \
	src/os/event/AwaiterTest \
	src/os/event/AwaiterTimeoutTest \
	src/os/event/AwaiterUserProvidedTriggerTest \
	src/os/event/AwaiterWritableFileDescriptorTest \
	src/os/io/FileDescriptorTest \
	src/os/io/NonBlockingFileDescriptorTest \
	src/os/io/ReaderTest \
	src/os/io/WriterTest \
	src/os/signaling/HandlerConfigurationTest

TESTSCRIPTS = \
	src/checkheaderinclude.sh \
	src/checkincludebuildconfig.sh \
	src/checklinelength.sh

TESTS = $(TESTPROGRAMS) $(TESTSCRIPTS)

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)

src/checkheaderinclude.log: src/checkheaderinclude.sh $(SOURCES)
src/checkincludebuildconfig.log: src/checkincludebuildconfig.sh $(SOURCES)
src/checklinelength.log: src/checklinelength.sh $(SOURCES) $(TESTSCRIPTS)

### Source files

src_sesh_SOURCES = \
	src/async/delay.hh \
	src/async/delay_holder.hh \
	src/async/future.hh \
	src/async/future.tcc \
	src/async/promise.hh \
	src/async/shared_future.hh \
	src/async/shared_future.tcc \
	src/buildconfig.h \
	src/common/common_result.hh \
	src/common/container_helper.hh \
	src/common/copy.hh \
	src/common/direct_initialize.hh \
	src/common/empty.hh \
	src/common/enum_iterator.hh \
	src/common/enum_set.hh \
	src/common/enum_traits.hh \
	src/common/errno_helper.hh \
	src/common/error_level.hh \
	src/common/functional_initialize.hh \
	src/common/identity.hh \
	src/common/integer_sequence.hh \
	src/common/maybe.hh \
	src/common/message.cc \
	src/common/message.hh \
	src/common/nop.hh \
	src/common/reference.hh \
	src/common/shared_function.hh \
	src/common/static_cast_function.hh \
	src/common/trial.hh \
	src/common/trie.hh \
	src/common/type_tag.hh \
	src/common/variant.hh \
	src/common/xchar.hh \
	src/common/xostream.hh \
	src/common/xsstream.hh \
	src/common/xstring.hh \
	src/helpermacros.h \
	src/i18n/M.h \
	src/language/source/buffer.cc \
	src/language/source/buffer.hh \
	src/language/source/diagnostic_message.cc \
	src/language/source/diagnostic_message.hh \
	src/language/source/line_appended_source.cc \
	src/language/source/line_appended_source.hh \
	src/language/source/line_continued_source.cc \
	src/language/source/line_continued_source.hh \
	src/language/source/location.cc \
	src/language/source/location.hh \
	src/language/source/origin.hh \
	src/language/source/source.cc \
	src/language/source/source.hh \
	src/language/syntax/and_or_list.cc \
	src/language/syntax/and_or_list.hh \
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
	src/main.cc \
	src/os/Api.cc \
	src/os/Api.hh \
	src/os/TimeApi.hh \
	src/os/capi.c \
	src/os/capi.h \
	src/os/capitypes.h \
	src/os/event/Awaiter.cc \
	src/os/event/Awaiter.hh \
	src/os/event/ErrorFileDescriptor.hh \
	src/os/event/FileDescriptorCondition.hh \
	src/os/event/Proactor.hh \
	src/os/event/PselectApi.hh \
	src/os/event/ReadableFileDescriptor.hh \
	src/os/event/Signal.hh \
	src/os/event/Timeout.hh \
	src/os/event/Trigger.hh \
	src/os/event/UserProvidedTrigger.hh \
	src/os/event/WritableFileDescriptor.hh \
	src/os/io/FileDescriptionAccessMode.hh \
	src/os/io/FileDescriptionApi.hh \
	src/os/io/FileDescriptionAttribute.hh \
	src/os/io/FileDescriptionStatus.hh \
	src/os/io/FileDescriptor.hh \
	src/os/io/FileDescriptorApi.hh \
	src/os/io/FileDescriptorOpenMode.hh \
	src/os/io/FileDescriptorSet.hh \
	src/os/io/FileMode.hh \
	src/os/io/NonBlockingFileDescriptor.cc \
	src/os/io/NonBlockingFileDescriptor.hh \
	src/os/io/Reader.cc \
	src/os/io/Reader.hh \
	src/os/io/ReaderApi.hh \
	src/os/io/Writer.cc \
	src/os/io/Writer.hh \
	src/os/io/WriterApi.hh \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfiguration.hh \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/HandlerConfigurationApi.hh \
	src/os/signaling/SignalErrorCategory.cc \
	src/os/signaling/SignalErrorCategory.hh \
	src/os/signaling/SignalErrorCode.hh \
	src/os/signaling/SignalNumber.hh \
	src/os/signaling/SignalNumberSet.hh

# Header files that are contained in sesh_SOURCES are not listed below.
src_async_delay_holder_test_SOURCES = \
	src/async/delay_holder_test.cc
src_async_delay_test_SOURCES = \
	src/async/delay_test.cc
src_async_future_test_SOURCES = \
	src/async/future_test.cc
src_async_promise_test_SOURCES = \
	src/async/promise_test.cc
src_async_shared_future_test_SOURCES = \
	src/async/shared_future_test.cc
src_common_container_helper_test_SOURCES = \
	src/common/container_helper_test.cc
src_common_enum_iterator_test_SOURCES = \
	src/common/enum_iterator_test.cc
src_common_enum_set_test_SOURCES = \
	src/common/enum_set_test.cc
src_common_maybe_test_SOURCES = \
	src/common/maybe_test.cc
src_common_message_test_SOURCES = \
	src/common/message.cc \
	src/common/message_test.cc
src_common_reference_test_SOURCES = \
	src/common/reference_test.cc
src_common_shared_function_test_SOURCES = \
	src/common/shared_function_test.cc
src_common_trial_test_SOURCES = \
	src/common/trial_test.cc
src_common_trie_test_SOURCES = \
	src/common/trie_test.cc
src_common_type_tag_test_SOURCES = \
	src/common/type_tag.hh \
	src/common/type_tag_test.cc
src_common_variant_test_SOURCES = \
	src/common/variant_test.cc
src_language_source_buffer_test_SOURCES = \
	src/common/message.cc \
	src/language/source/buffer.cc \
	src/language/source/buffer_test.cc \
	src/language/source/source.cc
src_language_source_line_appended_source_test_SOURCES = \
	src/common/message.cc \
	src/language/source/line_appended_source.cc \
	src/language/source/line_appended_source_test.cc \
	src/language/source/location.cc \
	src/language/source/source.cc
src_language_source_line_continued_source_test_SOURCES = \
	src/common/message.cc \
	src/language/source/line_continued_source.cc \
	src/language/source/line_continued_source_test.cc \
	src/language/source/location.cc \
	src/language/source/source.cc
src_language_source_location_test_SOURCES = \
	src/common/message.cc \
	src/language/source/location.cc \
	src/language/source/location_test.cc
src_language_source_source_test_SOURCES = \
	src/common/message.cc \
	src/language/source/source.cc \
	src/language/source/source_test.cc
src_language_syntax_and_or_list_test_SOURCES = \
	src/language/source/location.cc \
	src/language/syntax/and_or_list.cc \
	src/language/syntax/and_or_list_test.cc \
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
	src/language/source/location.cc \
	src/language/syntax/CommandTest.cc
src_language_syntax_ConditionalPipelineTest_SOURCES = \
	src/language/source/location.cc \
	src/language/syntax/ConditionalPipeline.cc \
	src/language/syntax/ConditionalPipelineTest.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc
src_language_syntax_PipelineTest_SOURCES = \
	src/language/source/location.cc \
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
	src/language/source/location.cc \
	src/language/syntax/and_or_list.cc \
	src/language/syntax/ConditionalPipeline.cc \
	src/language/syntax/Pipeline.cc \
	src/language/syntax/Printer.cc \
	src/language/syntax/Sequence.cc \
	src/language/syntax/SequenceTest.cc
src_language_syntax_SimpleCommandTest_SOURCES = \
	src/language/source/location.cc \
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
src_os_event_AwaiterErrorFileDescriptorTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterErrorFileDescriptorTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_event_AwaiterFileDescriptorTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterFileDescriptorTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_event_AwaiterReadableFileDescriptorTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterReadableFileDescriptorTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_event_AwaiterSignalTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterSignalTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_event_AwaiterTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_event_AwaiterTimeoutTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterTimeoutTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_event_AwaiterUserProvidedTriggerTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterUserProvidedTriggerTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_event_AwaiterWritableFileDescriptorTest_SOURCES = \
	src/os/event/Awaiter.cc \
	src/os/event/AwaiterWritableFileDescriptorTest.cc \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/SignalErrorCategory.cc
src_os_io_FileDescriptorTest_SOURCES = \
	src/os/io/FileDescriptorTest.cc
src_os_io_NonBlockingFileDescriptorTest_SOURCES = \
	src/os/io/NonBlockingFileDescriptor.cc \
	src/os/io/NonBlockingFileDescriptorTest.cc
src_os_io_ReaderTest_SOURCES = \
	src/os/io/NonBlockingFileDescriptor.cc \
	src/os/io/Reader.cc \
	src/os/io/ReaderTest.cc
src_os_io_WriterTest_SOURCES = \
	src/os/io/NonBlockingFileDescriptor.cc \
	src/os/io/Writer.cc \
	src/os/io/WriterTest.cc
src_os_signaling_HandlerConfigurationTest_SOURCES = \
	src/os/signaling/HandlerConfiguration.cc \
	src/os/signaling/HandlerConfigurationApi.cc \
	src/os/signaling/HandlerConfigurationTest.cc \
	src/os/signaling/SignalErrorCategory.cc

TESTHEADERS = \
	src/common/type_tag_test_helper.hh \
	src/language/source/diagnostic_message_test_helper.hh \
	src/language/source/location_test_helper.hh \
	src/language/source/origin_test_helper.hh \
	src/language/source/source_test_helper.hh \
	src/language/syntax/PrinterTestHelper.hh \
	src/os/TimeApiTestHelper.hh \
	src/os/event/AwaiterTestHelper.hh \
	src/os/event/PselectApiTestHelper.hh \
	src/os/io/FileDescriptionApiTestHelper.hh \
	src/os/io/FileDescriptorSetTestHelper.hh \
	src/os/io/NonBlockingFileDescriptorTestHelper.hh \
	src/os/signaling/HandlerConfigurationApiTestHelper.hh \
	src/os/signaling/SignalNumberSetTestHelper.hh

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
