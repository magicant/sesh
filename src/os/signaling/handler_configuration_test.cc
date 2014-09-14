/* Copyright (C) 2014 WATANABE Yuki
 *
 * This file is part of Sesh.
 *
 * Sesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Sesh is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Sesh.  If not, see <http://www.gnu.org/licenses/>.  */

#include "buildconfig.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <system_error>
#include "common/nop.hh"
#include "common/type_tag_test_helper.hh"
#include "os/signaling/handler_configuration.hh"
#include "os/signaling/handler_configuration_api_test_helper.hh"
#include "os/signaling/signal_error_code.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace {

using sesh::common::nop;
using sesh::os::signaling::handler_configuration;
using sesh::os::signaling::handler_configuration_api_dummy;
using sesh::os::signaling::handler_configuration_api_fake;
using sesh::os::signaling::signal_error_code;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;

using canceler_type = handler_configuration::canceler_type;

template<typename Api>
class fixture : protected Api {

protected:

    std::shared_ptr<handler_configuration> c =
            handler_configuration::create(*this);

}; // template<typename Api> class fixture

TEST_CASE_METHOD(
        fixture<handler_configuration_api_dummy>,
        "Handler configuration: construction") {
    CHECK(c != nullptr);
    CHECK(c.use_count() == 1);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_dummy>,
        "Handler configuration: empty configuration calls no handlers") {
    CHECK_NOTHROW(c->call_handlers());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_dummy>,
        "Handler configuration: empty configuration calls null mask") {
    CHECK(c->mask_for_pselect() == nullptr);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: simple handler and action") {
    SignalNumber v = 0;
    auto result = c->add_handler(5, [&v](SignalNumber n) { v += n; });
    CHECK(v == 0);

    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    CHECK(v == 0);
    c->call_handlers();
    CHECK(v == 5);

    a.value<sesh_osapi_signal_handler *>()(5);
    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();
    CHECK(v == 15);

    REQUIRE(result.tag() == result.tag<canceler_type>());
    CHECK(result.value<canceler_type>()().value() == 0);
    CHECK(result.value<canceler_type>()().value() == 0);
    CHECK(a.tag() == signal_action::tag<default_action>());

    c->call_handlers();
    CHECK(v == 15);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: many handlers and actions") {
    unsigned v1 = 0, v2 = 0, v3 = 0;

    auto result1 = c->add_handler(5, [&v1](SignalNumber) { ++v1; });
    auto result2 = c->add_handler(5, [&v2](SignalNumber) { ++v2; });
    auto result3 = c->add_handler(5, [&v3](SignalNumber) { ++v3; });

    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());
    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();

    CHECK(v1 == 1);
    CHECK(v2 == 1);
    CHECK(v3 == 1);

    result1.value<canceler_type>()();
    result2.value<canceler_type>()();

    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());
    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();

    CHECK(v1 == 1);
    CHECK(v2 == 1);
    CHECK(v3 == 2);

    result3.value<canceler_type>()();
    CHECK(a.tag() == signal_action::tag<default_action>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: handlers and actions for many signals") {
    SignalNumber s1 = 0, s2 = 0, s3 = 0;
    auto result1 = c->add_handler(1, [&s1](SignalNumber n) { s1 = n; });
    auto result2 = c->add_handler(2, [&s2](SignalNumber n) { s2 = n; });
    auto result3 = c->add_handler(3, [&s3](SignalNumber n) { s3 = n; });

    actions().at(1).value<sesh_osapi_signal_handler *>()(1);
    actions().at(2).value<sesh_osapi_signal_handler *>()(2);
    c->call_handlers();

    CHECK(s1 == 1);
    CHECK(s2 == 2);
    CHECK(s3 == 0);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: signal is blocked when handler is set") {
    auto result = c->add_handler(1, nop());
    CHECK(signal_mask().test(1));

    result.value<canceler_type>()();
    CHECK_FALSE(signal_mask().test(1));
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: mask is restored when handler is unset") {
    signal_mask().set(1);

    auto result = c->add_handler(1, nop());
    CHECK(signal_mask().test(1));

    result.value<canceler_type>()();
    CHECK(signal_mask().test(1));
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: handler for invalid signal") {
    auto result = c->add_handler(
            invalid_signal_number, [](SignalNumber) { FAIL(); });
    REQUIRE(result.tag() == result.tag<std::error_code>());
    CHECK(result.value<std::error_code>() == std::errc::invalid_argument);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: trap to default w/o handler") {
    std::error_code e = c->set_trap(
            5,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);
    CHECK(e.value() == 0);
    CHECK(actions().at(5).tag() == signal_action::tag<default_action>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: trap to default and add handler") {
    c->set_trap(
            5,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);
    SignalNumber v = 0;
    auto handler_result = c->add_handler(5, [&v](SignalNumber n) { v = n; });

    actions().at(5).value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();
    CHECK(v == 5);

    handler_result.value<canceler_type>()();
    CHECK(actions().at(5).tag() == signal_action::tag<default_action>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: add handler and trap to default") {
    SignalNumber v = 0;
    auto handlerResult = c->add_handler(5, [&v](SignalNumber n) { v += n; });
    std::error_code e = c->set_trap(
            5,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);

    CHECK(e.value() == 0);

    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();
    CHECK(v == 5);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: successful ignore w/o handler") {
    std::error_code e = c->set_trap(
            5,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::fail_if_ignored);
    CHECK(e.value() == 0);
    CHECK(actions().at(5).tag() == signal_action::tag<ignore>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: failed ignore w/o handler") {
    actions().emplace(5, ignore());

    std::error_code e = c->set_trap(
            5,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::fail_if_ignored);
    CHECK(e == signal_error_code::initially_ignored);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: failed ignore w/ handler") {
    actions().emplace(5, ignore());
    c->add_handler(5, nop());

    std::error_code e = c->set_trap(
            5,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::fail_if_ignored);
    CHECK(e == signal_error_code::initially_ignored);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: forced ignore w/o handler") {
    actions().emplace(5, ignore());

    std::error_code e = c->set_trap(
            5,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::force);
    CHECK(e.value() == 0);
    CHECK(actions().at(5).tag() == signal_action::tag<ignore>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: ignore and add handler") {
    c->set_trap(
            5,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::force);
    SignalNumber v = 0;
    auto handler_result = c->add_handler(5, [&v](SignalNumber n) { v = n; });

    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();
    CHECK(v == 5);

    handler_result.value<canceler_type>()();
    CHECK(a.tag() == signal_action::tag<ignore>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: add handler and ignore") {
    c->add_handler(5, nop());
    c->set_trap(
            5,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::force);
    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: trap w/o handler") {
    SignalNumber v = 0;
    handler_configuration::handler_type h = [&v](SignalNumber n) { v = n; };
    std::error_code e = c->set_trap(
            5, std::move(h), handler_configuration::setting_policy::force);
    CHECK(e.value() == 0);

    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();
    CHECK(v == 5);

    e = c->set_trap(
            5,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);
    CHECK(e.value() == 0);
    CHECK(a.tag() == signal_action::tag<default_action>());
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: trap and add another handler") {
    SignalNumber v1 = 0, v2 = 0;
    handler_configuration::handler_type h = [&v1](SignalNumber n) { v1 = n; };
    c->set_trap(5, std::move(h), handler_configuration::setting_policy::force);
    c->add_handler(5, [&v2](SignalNumber n) { v2 = n; });

    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();
    CHECK(v1 == 5);
    CHECK(v2 == 5);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: add handler and trap") {
    SignalNumber v1 = 0, v2 = 0;
    handler_configuration::handler_type h = [&v1](SignalNumber n) { v1 = n; };
    c->add_handler(5, [&v2](SignalNumber n) { v2 = n; });
    c->set_trap(5, std::move(h), handler_configuration::setting_policy::force);

    signal_action &a = actions().at(5);
    REQUIRE(a.tag() == signal_action::tag<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->call_handlers();
    CHECK(v1 == 5);
    CHECK(v2 == 5);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: signal is blocked when trap is set") {
    c->set_trap(
            5,
            handler_configuration::handler_type(nop()),
            handler_configuration::setting_policy::force);
    CHECK(signal_mask().test(5));

    c->set_trap(
            5,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);
    CHECK_FALSE(signal_mask().test(5));
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: signal is restored when trap is unset") {
    signal_mask().set(5);

    c->set_trap(
            5,
            handler_configuration::handler_type(nop()),
            handler_configuration::setting_policy::force);
    CHECK(signal_mask().test(5));

    c->set_trap(
            5,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);
    CHECK(signal_mask().test(5));
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: trapping invalid signal") {
    std::error_code e = c->set_trap(
            invalid_signal_number,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::force);
    CHECK(e == std::errc::invalid_argument);
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: mask for pselect with handlers") {
    signal_mask().set(1);
    signal_mask().set(2);
    auto action_result2 = c->add_handler(2, nop());
    auto action_result3 = c->add_handler(3, nop());

    const SignalNumberSet *set = c->mask_for_pselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK_FALSE(set->test(2));
    CHECK_FALSE(set->test(3));

    action_result2.value<canceler_type>()();
    action_result3.value<canceler_type>()();

    set = c->mask_for_pselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK(set->test(2));
    CHECK_FALSE(set->test(3));
}

TEST_CASE_METHOD(
        fixture<handler_configuration_api_fake>,
        "Handler configuration: mask for pselect with ignore") {
    signal_mask().set(1);
    signal_mask().set(2);
    c->set_trap(
            2,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::force);
    c->set_trap(
            3,
            handler_configuration::handler_type(),
            handler_configuration::setting_policy::force);

    const SignalNumberSet *set = c->mask_for_pselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK_FALSE(set->test(2));
    CHECK_FALSE(set->test(3));

    c->set_trap(
            2,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);
    c->set_trap(
            3,
            handler_configuration::default_action(),
            handler_configuration::setting_policy::force);

    set = c->mask_for_pselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK(set->test(2));
    CHECK_FALSE(set->test(3));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
