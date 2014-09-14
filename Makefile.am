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
	src/common/function_helper_test \
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
	src/language/syntax/assignment_test \
	src/language/syntax/command_test \
	src/language/syntax/conditional_pipeline_test \
	src/language/syntax/pipeline_test \
	src/language/syntax/printer_test \
	src/language/syntax/raw_string_test \
	src/language/syntax/sequence_test \
	src/language/syntax/simple_command_test \
	src/language/syntax/word_test \
	src/os/event/awaiter_error_file_descriptor_test \
	src/os/event/awaiter_file_descriptor_test \
	src/os/event/awaiter_readable_file_descriptor_test \
	src/os/event/awaiter_signal_test \
	src/os/event/awaiter_test \
	src/os/event/awaiter_timeout_test \
	src/os/event/awaiter_user_provided_trigger_test \
	src/os/event/awaiter_writable_file_descriptor_test \
	src/os/io/file_descriptor_test \
	src/os/io/non_blocking_file_descriptor_test \
	src/os/io/reader_test \
	src/os/io/writer_test \
	src/os/signaling/handler_configuration_test

TESTSCRIPTS = \
	src/checkheaderinclude.sh \
	src/checkincludebuildconfig.sh \
	src/checklinelength.sh

TESTS = $(TESTPROGRAMS) $(TESTSCRIPTS)

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)

src/checkheaderinclude.log: src/checkheaderinclude.sh \
	$(SOURCES) $(TESTHEADERS)
src/checkincludebuildconfig.log: src/checkincludebuildconfig.sh \
	$(SOURCES) $(TESTHEADERS)
src/checklinelength.log: src/checklinelength.sh \
	$(SOURCES) $(TESTHEADERS) $(TESTSCRIPTS)

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
	src/common/function_helper.hh \
	src/common/functional_initialize.hh \
	src/common/identity.hh \
	src/common/integer_sequence.hh \
	src/common/logic_helper.hh \
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
	src/language/syntax/assignment.cc \
	src/language/syntax/assignment.hh \
	src/language/syntax/command.hh \
	src/language/syntax/conditional_pipeline.cc \
	src/language/syntax/conditional_pipeline.hh \
	src/language/syntax/pipeline.cc \
	src/language/syntax/pipeline.hh \
	src/language/syntax/printable.hh \
	src/language/syntax/printer.cc \
	src/language/syntax/printer.hh \
	src/language/syntax/raw_string.cc \
	src/language/syntax/raw_string.hh \
	src/language/syntax/sequence.cc \
	src/language/syntax/sequence.hh \
	src/language/syntax/simple_command.cc \
	src/language/syntax/simple_command.hh \
	src/language/syntax/word.cc \
	src/language/syntax/word.hh \
	src/language/syntax/word_component.hh \
	src/main.cc \
	src/os/api.cc \
	src/os/api.hh \
	src/os/capi.c \
	src/os/capi.h \
	src/os/capitypes.h \
	src/os/event/awaiter.cc \
	src/os/event/awaiter.hh \
	src/os/event/error_file_descriptor.hh \
	src/os/event/file_descriptor_condition.hh \
	src/os/event/proactor.hh \
	src/os/event/pselect_api.hh \
	src/os/event/readable_file_descriptor.hh \
	src/os/event/signal.hh \
	src/os/event/timeout.hh \
	src/os/event/trigger.hh \
	src/os/event/user_provided_trigger.hh \
	src/os/event/writable_file_descriptor.hh \
	src/os/io/file_description_access_mode.hh \
	src/os/io/file_description_api.hh \
	src/os/io/file_description_attribute.hh \
	src/os/io/file_description_status.hh \
	src/os/io/file_descriptor.hh \
	src/os/io/file_descriptor_api.hh \
	src/os/io/file_descriptor_open_mode.hh \
	src/os/io/file_descriptor_set.hh \
	src/os/io/file_mode.hh \
	src/os/io/non_blocking_file_descriptor.cc \
	src/os/io/non_blocking_file_descriptor.hh \
	src/os/io/reader.cc \
	src/os/io/reader.hh \
	src/os/io/reader_api.hh \
	src/os/io/writer.cc \
	src/os/io/writer.hh \
	src/os/io/writer_api.hh \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration.hh \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/handler_configuration_api.hh \
	src/os/signaling/signal_error_category.cc \
	src/os/signaling/signal_error_category.hh \
	src/os/signaling/signal_error_code.hh \
	src/os/signaling/signal_number.hh \
	src/os/signaling/signal_number_set.hh \
	src/os/time_api.hh

# Header files that are contained in sesh_SOURCES are not listed below.
src_async_delay_holder_test_SOURCES = \
	src/async/delay_holder_test.cc \
	src/catch_main.cc
src_async_delay_test_SOURCES = \
	src/async/delay_test.cc \
	src/catch_main.cc
src_async_future_test_SOURCES = \
	src/async/future_test.cc \
	src/catch_main.cc
src_async_promise_test_SOURCES = \
	src/async/promise_test.cc \
	src/catch_main.cc
src_async_shared_future_test_SOURCES = \
	src/async/shared_future_test.cc \
	src/catch_main.cc
src_common_container_helper_test_SOURCES = \
	src/catch_main.cc \
	src/common/container_helper_test.cc
src_common_enum_iterator_test_SOURCES = \
	src/catch_main.cc \
	src/common/enum_iterator_test.cc
src_common_enum_set_test_SOURCES = \
	src/catch_main.cc \
	src/common/enum_set_test.cc
src_common_function_helper_test_SOURCES = \
	src/catch_main.cc \
	src/common/function_helper_test.cc
src_common_maybe_test_SOURCES = \
	src/catch_main.cc \
	src/common/maybe_test.cc
src_common_message_test_SOURCES = \
	src/catch_main.cc \
	src/common/message.cc \
	src/common/message_test.cc
src_common_reference_test_SOURCES = \
	src/catch_main.cc \
	src/common/reference_test.cc
src_common_shared_function_test_SOURCES = \
	src/catch_main.cc \
	src/common/shared_function_test.cc
src_common_trial_test_SOURCES = \
	src/catch_main.cc \
	src/common/trial_test.cc
src_common_trie_test_SOURCES = \
	src/catch_main.cc \
	src/common/trie_test.cc
src_common_type_tag_test_SOURCES = \
	src/catch_main.cc \
	src/common/type_tag.hh \
	src/common/type_tag_test.cc
src_common_variant_test_SOURCES = \
	src/catch_main.cc \
	src/common/variant_test.cc
src_language_source_buffer_test_SOURCES = \
	src/catch_main.cc \
	src/common/message.cc \
	src/language/source/buffer.cc \
	src/language/source/buffer_test.cc \
	src/language/source/source.cc
src_language_source_line_appended_source_test_SOURCES = \
	src/catch_main.cc \
	src/common/message.cc \
	src/language/source/line_appended_source.cc \
	src/language/source/line_appended_source_test.cc \
	src/language/source/location.cc \
	src/language/source/source.cc
src_language_source_line_continued_source_test_SOURCES = \
	src/catch_main.cc \
	src/common/message.cc \
	src/language/source/line_continued_source.cc \
	src/language/source/line_continued_source_test.cc \
	src/language/source/location.cc \
	src/language/source/source.cc
src_language_source_location_test_SOURCES = \
	src/catch_main.cc \
	src/common/message.cc \
	src/language/source/location.cc \
	src/language/source/location_test.cc
src_language_source_source_test_SOURCES = \
	src/catch_main.cc \
	src/common/message.cc \
	src/language/source/source.cc \
	src/language/source/source_test.cc
src_language_syntax_and_or_list_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/location.cc \
	src/language/syntax/and_or_list.cc \
	src/language/syntax/and_or_list_test.cc \
	src/language/syntax/conditional_pipeline.cc \
	src/language/syntax/pipeline.cc \
	src/language/syntax/printer.cc
src_language_syntax_assignment_test_SOURCES = \
	src/catch_main.cc \
	src/language/syntax/assignment.cc \
	src/language/syntax/assignment_test.cc \
	src/language/syntax/printer.cc \
	src/language/syntax/raw_string.cc \
	src/language/syntax/word.cc
src_language_syntax_command_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/location.cc \
	src/language/syntax/command_test.cc
src_language_syntax_conditional_pipeline_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/location.cc \
	src/language/syntax/conditional_pipeline.cc \
	src/language/syntax/conditional_pipeline_test.cc \
	src/language/syntax/pipeline.cc \
	src/language/syntax/printer.cc
src_language_syntax_pipeline_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/location.cc \
	src/language/syntax/pipeline.cc \
	src/language/syntax/pipeline_test.cc \
	src/language/syntax/printer.cc
src_language_syntax_printer_test_SOURCES = \
	src/catch_main.cc \
	src/language/syntax/printer.cc \
	src/language/syntax/printer_test.cc
src_language_syntax_raw_string_test_SOURCES = \
	src/catch_main.cc \
	src/language/syntax/printer.cc \
	src/language/syntax/raw_string.cc \
	src/language/syntax/raw_string_test.cc
src_language_syntax_sequence_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/location.cc \
	src/language/syntax/and_or_list.cc \
	src/language/syntax/conditional_pipeline.cc \
	src/language/syntax/pipeline.cc \
	src/language/syntax/printer.cc \
	src/language/syntax/sequence.cc \
	src/language/syntax/sequence_test.cc
src_language_syntax_simple_command_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/location.cc \
	src/language/syntax/assignment.cc \
	src/language/syntax/printer.cc \
	src/language/syntax/raw_string.cc \
	src/language/syntax/simple_command.cc \
	src/language/syntax/simple_command_test.cc \
	src/language/syntax/word.cc
src_language_syntax_word_test_SOURCES = \
	src/catch_main.cc \
	src/language/syntax/printer.cc \
	src/language/syntax/raw_string.cc \
	src/language/syntax/word.cc \
	src/language/syntax/word_test.cc
src_os_event_awaiter_error_file_descriptor_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_error_file_descriptor_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_event_awaiter_file_descriptor_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_file_descriptor_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_event_awaiter_readable_file_descriptor_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_readable_file_descriptor_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_event_awaiter_signal_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_signal_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_event_awaiter_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_event_awaiter_timeout_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_timeout_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_event_awaiter_user_provided_trigger_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_user_provided_trigger_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_event_awaiter_writable_file_descriptor_test_SOURCES = \
	src/catch_main.cc \
	src/os/event/awaiter.cc \
	src/os/event/awaiter_writable_file_descriptor_test.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/signal_error_category.cc
src_os_io_file_descriptor_test_SOURCES = \
	src/catch_main.cc \
	src/os/io/file_descriptor_test.cc
src_os_io_non_blocking_file_descriptor_test_SOURCES = \
	src/catch_main.cc \
	src/os/io/non_blocking_file_descriptor.cc \
	src/os/io/non_blocking_file_descriptor_test.cc
src_os_io_reader_test_SOURCES = \
	src/catch_main.cc \
	src/os/io/non_blocking_file_descriptor.cc \
	src/os/io/reader.cc \
	src/os/io/reader_test.cc
src_os_io_writer_test_SOURCES = \
	src/catch_main.cc \
	src/os/io/non_blocking_file_descriptor.cc \
	src/os/io/writer.cc \
	src/os/io/writer_test.cc
src_os_signaling_handler_configuration_test_SOURCES = \
	src/catch_main.cc \
	src/os/signaling/handler_configuration.cc \
	src/os/signaling/handler_configuration_api.cc \
	src/os/signaling/handler_configuration_test.cc \
	src/os/signaling/signal_error_category.cc

TESTHEADERS = \
	src/common/type_tag_test_helper.hh \
	src/language/source/diagnostic_message_test_helper.hh \
	src/language/source/location_test_helper.hh \
	src/language/source/origin_test_helper.hh \
	src/language/source/source_test_helper.hh \
	src/language/syntax/printer_test_helper.hh \
	src/os/event/awaiter_test_helper.hh \
	src/os/event/pselect_api_test_helper.hh \
	src/os/io/file_description_api_test_helper.hh \
	src/os/io/file_descriptor_set_test_helper.hh \
	src/os/io/non_blocking_file_descriptor_test_helper.hh \
	src/os/signaling/handler_configuration_api_test_helper.hh \
	src/os/signaling/signal_number_set_test_helper.hh \
	src/os/time_api_test_helper.hh

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
