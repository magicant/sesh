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
	src/async/continuation_test \
	src/async/delay_holder_test \
	src/async/delay_test \
	src/async/future_test \
	src/async/lazy_test \
	src/async/promise_test \
	src/async/shared_future_test \
	src/async/shared_lazy_test \
	src/common/container_helper_test \
	src/common/either_test \
	src/common/enum_iterator_test \
	src/common/enum_set_test \
	src/common/function_helper_test \
	src/common/reference_test \
	src/common/shared_function_test \
	src/common/tagged_union_test \
	src/common/trie_test \
	src/common/type_tag_set_test \
	src/common/type_tag_test \
	src/common/variant_test \
	src/common/visitor_test \
	src/language/executing/raw_string_test \
	src/language/executing/word_test \
	src/language/parsing/and_or_list_test \
	src/language/parsing/char_test \
	src/language/parsing/command_test \
	src/language/parsing/comment_test \
	src/language/parsing/eof_test \
	src/language/parsing/line_continuation_test \
	src/language/parsing/line_continued_char_test \
	src/language/parsing/pipeline_test \
	src/language/parsing/raw_string_test \
	src/language/parsing/repeat_test \
	src/language/parsing/sequence_test \
	src/language/parsing/simple_command_test \
	src/language/parsing/token_test \
	src/language/parsing/whitespace_test \
	src/language/parsing/word_component_test \
	src/language/parsing/word_test \
	src/language/printing/and_or_list_test \
	src/language/printing/buffer_test \
	src/language/printing/conditional_pipeline_test \
	src/language/printing/pipeline_test \
	src/language/printing/raw_string_test \
	src/language/printing/sequence_test \
	src/language/printing/simple_command_test \
	src/language/printing/word_test \
	src/language/source/fragment_test \
	src/language/source/stream_test \
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
	src/os/signaling/handler_configuration_test \
	src/ui/message/format_test

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

src_sesh_SOURCES_PARSER = \
	src/language/parsing/and_or_list.cc \
	src/language/parsing/and_or_list.hh \
	src/language/parsing/blackhole.hh \
	src/language/parsing/char.cc \
	src/language/parsing/char.hh \
	src/language/parsing/char_predicate.cc \
	src/language/parsing/char_predicate.hh \
	src/language/parsing/command.cc \
	src/language/parsing/command.hh \
	src/language/parsing/comment.cc \
	src/language/parsing/comment.hh \
	src/language/parsing/constant_parser.hh \
	src/language/parsing/eof.cc \
	src/language/parsing/eof.hh \
	src/language/parsing/joiner.hh \
	src/language/parsing/line_continuation.cc \
	src/language/parsing/line_continuation.hh \
	src/language/parsing/line_continued_char.cc \
	src/language/parsing/line_continued_char.hh \
	src/language/parsing/mapper.hh \
	src/language/parsing/option.hh \
	src/language/parsing/parser.hh \
	src/language/parsing/pipeline.cc \
	src/language/parsing/pipeline.hh \
	src/language/parsing/raw_string.cc \
	src/language/parsing/raw_string.hh \
	src/language/parsing/repeat.hh \
	src/language/parsing/report_helper.hh \
	src/language/parsing/sequence.cc \
	src/language/parsing/sequence.hh \
	src/language/parsing/simple_command.cc \
	src/language/parsing/simple_command.hh \
	src/language/parsing/token.cc \
	src/language/parsing/token.hh \
	src/language/parsing/whitespace.cc \
	src/language/parsing/whitespace.hh \
	src/language/parsing/word.cc \
	src/language/parsing/word.hh \
	src/language/parsing/word_component.cc \
	src/language/parsing/word_component.hh

src_sesh_SOURCES_EXECUTOR = \
	src/language/executing/expansion.hh \
	src/language/executing/expansion_result.hh \
	src/language/executing/field.hh \
	src/language/executing/multiple_field_result.hh \
	src/language/executing/raw_string.cc \
	src/language/executing/raw_string.hh \
	src/language/executing/result.hh \
	src/language/executing/string.cc \
	src/language/executing/string.hh \
	src/language/executing/word.cc \
	src/language/executing/word.hh \
	src/language/executing/word_char.hh \
	src/language/executing/word_component.cc \
	src/language/executing/word_component.hh

src_sesh_SOURCES = \
	$(src_sesh_SOURCES_EXECUTOR) \
	$(src_sesh_SOURCES_PARSER) \
	src/async/continuation.hh \
	src/async/delay.hh \
	src/async/delay_holder.hh \
	src/async/future.hh \
	src/async/future.tcc \
	src/async/lazy.hh \
	src/async/promise.hh \
	src/async/shared_future.hh \
	src/async/shared_future.tcc \
	src/async/shared_lazy.hh \
	src/buildconfig.h \
	src/common/constant_function.hh \
	src/common/container_helper.hh \
	src/common/copy.hh \
	src/common/direct_initialize.hh \
	src/common/either.hh \
	src/common/empty.hh \
	src/common/enum_iterator.hh \
	src/common/enum_set.hh \
	src/common/enum_traits.hh \
	src/common/errno_helper.hh \
	src/common/function_helper.hh \
	src/common/identity.hh \
	src/common/integer_sequence.hh \
	src/common/logic_helper.hh \
	src/common/nop.hh \
	src/common/reference.hh \
	src/common/shared_function.hh \
	src/common/static_cast_function.hh \
	src/common/tagged_union.hh \
	src/common/trie.hh \
	src/common/type_tag.hh \
	src/common/type_tag_set.hh \
	src/common/variant.hh \
	src/common/visitor.hh \
	src/common/xchar.hh \
	src/common/xostream.hh \
	src/common/xsstream.hh \
	src/common/xstring.hh \
	src/environment/world.hh \
	src/helpermacros.h \
	src/i18n/M.h \
	src/language/printing/and_or_list.cc \
	src/language/printing/and_or_list.hh \
	src/language/printing/buffer.cc \
	src/language/printing/buffer.hh \
	src/language/printing/command.cc \
	src/language/printing/command.hh \
	src/language/printing/conditional_pipeline.cc \
	src/language/printing/conditional_pipeline.hh \
	src/language/printing/pipeline.cc \
	src/language/printing/pipeline.hh \
	src/language/printing/raw_string.hh \
	src/language/printing/sequence.cc \
	src/language/printing/sequence.hh \
	src/language/printing/simple_command.cc \
	src/language/printing/simple_command.hh \
	src/language/printing/word.cc \
	src/language/printing/word.hh \
	src/language/printing/word_component.cc \
	src/language/printing/word_component.hh \
	src/language/source/fragment.cc \
	src/language/source/fragment.hh \
	src/language/source/stream.cc \
	src/language/source/stream.hh \
	src/language/syntax/and_or_list.hh \
	src/language/syntax/command.hh \
	src/language/syntax/conditional_pipeline.hh \
	src/language/syntax/pipeline.hh \
	src/language/syntax/raw_string.hh \
	src/language/syntax/sequence.hh \
	src/language/syntax/simple_command.hh \
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
	src/os/time_api.hh \
	src/ui/message/category.hh \
	src/ui/message/format.cc \
	src/ui/message/format.hh \
	src/ui/message/report.hh

# Header files that are contained in sesh_SOURCES are not listed below.
src_async_continuation_test_SOURCES = \
	src/async/continuation_test.cc \
	src/catch_main.cc
src_async_delay_holder_test_SOURCES = \
	src/async/delay_holder_test.cc \
	src/catch_main.cc
src_async_delay_test_SOURCES = \
	src/async/delay_test.cc \
	src/catch_main.cc
src_async_future_test_SOURCES = \
	src/async/future_test.cc \
	src/catch_main.cc
src_async_lazy_test_SOURCES = \
	src/catch_main.cc \
	src/async/lazy_test.cc
src_async_promise_test_SOURCES = \
	src/async/promise_test.cc \
	src/catch_main.cc
src_async_shared_future_test_SOURCES = \
	src/async/shared_future_test.cc \
	src/catch_main.cc
src_async_shared_lazy_test_SOURCES = \
	src/async/shared_lazy_test.cc \
	src/catch_main.cc
src_common_container_helper_test_SOURCES = \
	src/catch_main.cc \
	src/common/container_helper_test.cc
src_common_either_test_SOURCES = \
	src/catch_main.cc \
	src/common/either_test.cc
src_common_enum_iterator_test_SOURCES = \
	src/catch_main.cc \
	src/common/enum_iterator_test.cc
src_common_enum_set_test_SOURCES = \
	src/catch_main.cc \
	src/common/enum_set_test.cc
src_common_function_helper_test_SOURCES = \
	src/catch_main.cc \
	src/common/function_helper_test.cc
src_common_reference_test_SOURCES = \
	src/catch_main.cc \
	src/common/reference_test.cc
src_common_shared_function_test_SOURCES = \
	src/catch_main.cc \
	src/common/shared_function_test.cc
src_common_tagged_union_test_SOURCES = \
	src/catch_main.cc \
	src/common/tagged_union_test.cc
src_common_trie_test_SOURCES = \
	src/catch_main.cc \
	src/common/trie_test.cc
src_common_type_tag_set_test_SOURCES = \
	src/catch_main.cc \
	src/common/type_tag_set_test.cc
src_common_type_tag_test_SOURCES = \
	src/catch_main.cc \
	src/common/type_tag.hh \
	src/common/type_tag_test.cc
src_common_variant_test_SOURCES = \
	src/catch_main.cc \
	src/common/variant_test.cc
src_common_visitor_test_SOURCES = \
	src/catch_main.cc \
	src/common/visitor_test.cc
src_language_executing_raw_string_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_EXECUTOR) \
	src/language/executing/raw_string_test.cc
src_language_executing_word_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_EXECUTOR) \
	src/language/executing/word_test.cc
src_language_parsing_and_or_list_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/and_or_list_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_char_test_SOURCES = \
	src/catch_main.cc \
	src/language/parsing/char.cc \
	src/language/parsing/char_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc
src_language_parsing_command_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/command_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_comment_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/comment_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_eof_test_SOURCES = \
	src/catch_main.cc \
	src/language/parsing/eof.cc \
	src/language/parsing/eof_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc
src_language_parsing_line_continuation_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/line_continuation_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_line_continued_char_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/line_continued_char_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_pipeline_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/pipeline_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_raw_string_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/raw_string_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_repeat_test_SOURCES = \
	src/catch_main.cc \
	src/language/parsing/char.cc \
	src/language/parsing/repeat_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_sequence_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/sequence_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_simple_command_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/simple_command_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_token_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/token_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_whitespace_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/whitespace_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_word_component_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/word_component_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_parsing_word_test_SOURCES = \
	src/catch_main.cc \
	$(src_sesh_SOURCES_PARSER) \
	src/language/parsing/word_test.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/ui/message/format.cc
src_language_printing_and_or_list_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/and_or_list.cc \
	src/language/printing/and_or_list_test.cc \
	src/language/printing/buffer.cc \
	src/language/printing/command.cc \
	src/language/printing/conditional_pipeline.cc \
	src/language/printing/pipeline.cc \
	src/language/printing/simple_command.cc \
	src/language/printing/word.cc \
	src/language/printing/word_component.cc
src_language_printing_buffer_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/buffer.cc \
	src/language/printing/buffer_test.cc
src_language_printing_conditional_pipeline_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/buffer.cc \
	src/language/printing/command.cc \
	src/language/printing/conditional_pipeline.cc \
	src/language/printing/conditional_pipeline_test.cc \
	src/language/printing/pipeline.cc \
	src/language/printing/simple_command.cc \
	src/language/printing/word.cc \
	src/language/printing/word_component.cc
src_language_printing_pipeline_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/buffer.cc \
	src/language/printing/command.cc \
	src/language/printing/pipeline.cc \
	src/language/printing/pipeline_test.cc \
	src/language/printing/simple_command.cc \
	src/language/printing/word.cc \
	src/language/printing/word_component.cc
src_language_printing_raw_string_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/buffer.cc \
	src/language/printing/raw_string_test.cc
src_language_printing_sequence_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/and_or_list.cc \
	src/language/printing/buffer.cc \
	src/language/printing/command.cc \
	src/language/printing/conditional_pipeline.cc \
	src/language/printing/pipeline.cc \
	src/language/printing/sequence.cc \
	src/language/printing/sequence_test.cc \
	src/language/printing/simple_command.cc \
	src/language/printing/word.cc \
	src/language/printing/word_component.cc
src_language_printing_simple_command_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/buffer.cc \
	src/language/printing/simple_command.cc \
	src/language/printing/simple_command_test.cc \
	src/language/printing/word.cc \
	src/language/printing/word_component.cc
src_language_printing_word_test_SOURCES = \
	src/catch_main.cc \
	src/language/printing/buffer.cc \
	src/language/printing/word.cc \
	src/language/printing/word_component.cc \
	src/language/printing/word_test.cc
src_language_source_fragment_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/fragment.cc \
	src/language/source/fragment_test.cc
src_language_source_stream_test_SOURCES = \
	src/catch_main.cc \
	src/language/source/fragment.cc \
	src/language/source/stream.cc \
	src/language/source/stream_test.cc
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
src_ui_message_format_test_SOURCES = \
	src/catch_main.cc \
	src/ui/message/format.cc \
	src/ui/message/format_test.cc

TESTHEADERS = \
	src/async/future_test_helper.hh \
	src/common/type_tag_test_helper.hh \
	src/common/visitor_test_helper.hh \
	src/language/executing/result_test_helper.hh \
	src/language/parsing/parser_test_helper.hh \
	src/language/source/fragment_test_helper.hh \
	src/language/syntax/and_or_list_test_helper.hh \
	src/language/syntax/command_test_helper.hh \
	src/language/syntax/conditional_pipeline_test_helper.hh \
	src/language/syntax/pipeline_test_helper.hh \
	src/language/syntax/sequence_test_helper.hh \
	src/language/syntax/simple_command_test_helper.hh \
	src/language/syntax/word_component_test_helper.hh \
	src/language/syntax/word_test_helper.hh \
	src/os/event/awaiter_test_helper.hh \
	src/os/event/pselect_api_test_helper.hh \
	src/os/io/file_description_api_test_helper.hh \
	src/os/io/file_descriptor_set_test_helper.hh \
	src/os/io/non_blocking_file_descriptor_test_helper.hh \
	src/os/signaling/handler_configuration_api_test_helper.hh \
	src/os/signaling/signal_number_set_test_helper.hh \
	src/os/time_api_test_helper.hh \
	src/ui/message/report_test_helper.hh

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
