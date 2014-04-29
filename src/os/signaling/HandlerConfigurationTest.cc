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
#include "common/Nop.hh"
#include "os/signaling/HandlerConfiguration.hh"
#include "os/signaling/SignalErrorCode.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"
#include "os/test_helper/SigactionApiFake.hh"
#include "os/test_helper/SignalMaskApiFake.hh"
#include "os/test_helper/UnimplementedApi.hh"

namespace {

using sesh::common::Nop;
using sesh::os::signaling::HandlerConfiguration;
using sesh::os::signaling::SignalErrorCode;
using sesh::os::signaling::SignalNumber;
using sesh::os::signaling::SignalNumberSet;
using sesh::os::test_helper::SigactionApiFake;
using sesh::os::test_helper::SignalMaskApiFake;
using sesh::os::test_helper::UnimplementedApi;

using Canceler = HandlerConfiguration::Canceler;
using TrapAction = HandlerConfiguration::TrapAction;
using TrapDefault = HandlerConfiguration::Default;

template<typename Api>
class Fixture : protected Api {

protected:

    std::shared_ptr<HandlerConfiguration> c =
            HandlerConfiguration::create(*this);

}; // template<typename Api> class Fixture

class SignalApiFake : public SigactionApiFake, public SignalMaskApiFake {
};

TEST_CASE_METHOD(
        Fixture<UnimplementedApi>, "Handler configuration: construction") {
    CHECK(c != nullptr);
    CHECK(c.use_count() == 1);
}

TEST_CASE_METHOD(
        Fixture<UnimplementedApi>,
        "Handler configuration: empty configuration calls no handlers") {
    CHECK_NOTHROW(c->callHandlers());
}

TEST_CASE_METHOD(
        Fixture<UnimplementedApi>,
        "Handler configuration: empty configuration calls null mask") {
    CHECK(c->maskForPselect() == nullptr);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: simple handler and action") {
    SignalNumber v = 0;
    auto result = c->addHandler(5, [&v](SignalNumber n) { v += n; });
    CHECK(v == 0);

    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    CHECK(v == 0);
    c->callHandlers();
    CHECK(v == 5);

    a.value<sesh_osapi_signal_handler *>()(5);
    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();
    CHECK(v == 15);

    REQUIRE(result.index() == result.index<Canceler>());
    CHECK(result.value<Canceler>()().value() == 0);
    CHECK(result.value<Canceler>()().value() == 0);
    CHECK(a.index() == Action::index<Default>());

    c->callHandlers();
    CHECK(v == 15);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: many handlers and actions") {
    unsigned v1 = 0, v2 = 0, v3 = 0;

    auto result1 = c->addHandler(5, [&v1](SignalNumber) { ++v1; });
    auto result2 = c->addHandler(5, [&v2](SignalNumber) { ++v2; });
    auto result3 = c->addHandler(5, [&v3](SignalNumber) { ++v3; });

    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();

    CHECK(v1 == 1);
    CHECK(v2 == 1);
    CHECK(v3 == 1);

    result1.value<Canceler>()();
    result2.value<Canceler>()();

    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();

    CHECK(v1 == 1);
    CHECK(v2 == 1);
    CHECK(v3 == 2);

    result3.value<Canceler>()();
    CHECK(a.index() == Action::index<Default>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: handlers and actions for many signals") {
    SignalNumber s1 = 0, s2 = 0, s3 = 0;
    auto result1 = c->addHandler(1, [&s1](SignalNumber n) { s1 = n; });
    auto result2 = c->addHandler(2, [&s2](SignalNumber n) { s2 = n; });
    auto result3 = c->addHandler(3, [&s3](SignalNumber n) { s3 = n; });

    actions().at(1).value<sesh_osapi_signal_handler *>()(1);
    actions().at(2).value<sesh_osapi_signal_handler *>()(2);
    c->callHandlers();

    CHECK(s1 == 1);
    CHECK(s2 == 2);
    CHECK(s3 == 0);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: signal is blocked when handler is set") {
    auto result = c->addHandler(1, Nop());
    CHECK(signalMask().test(1));

    result.value<Canceler>()();
    CHECK_FALSE(signalMask().test(1));
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: mask is restored when handler is unset") {
    signalMask().set(1);

    auto result = c->addHandler(1, Nop());
    CHECK(signalMask().test(1));

    result.value<Canceler>()();
    CHECK(signalMask().test(1));
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: handler for invalid signal") {
    auto result =
            c->addHandler(INVALID_SIGNAL_NUMBER, [](SignalNumber) { FAIL(); });
    REQUIRE(result.index() == result.index<std::error_code>());
    CHECK(result.value<std::error_code>() == std::errc::invalid_argument);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: trap to default w/o handler") {
    std::error_code e = c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(e.value() == 0);
    CHECK(actions().at(5).index() == Action::index<Default>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: trap to default and add handler") {
    c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    SignalNumber v = 0;
    auto handlerResult = c->addHandler(5, [&v](SignalNumber n) { v = n; });

    actions().at(5).value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();
    CHECK(v == 5);

    handlerResult.value<Canceler>()();
    CHECK(actions().at(5).index() == Action::index<Default>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: add handler and trap to default") {
    SignalNumber v = 0;
    auto handlerResult = c->addHandler(5, [&v](SignalNumber n) { v += n; });
    std::error_code e = c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);

    CHECK(e.value() == 0);

    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();
    CHECK(v == 5);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: successful ignore w/o handler") {
    std::error_code e = c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FAIL_IF_IGNORED);
    CHECK(e.value() == 0);
    CHECK(actions().at(5).index() == Action::index<Ignore>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: failed ignore w/o handler") {
    actions().emplace(5, Action::create<Ignore>());

    std::error_code e = c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FAIL_IF_IGNORED);
    CHECK(e == SignalErrorCode::INITIALLY_IGNORED);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: failed ignore w/ handler") {
    actions().emplace(5, Action::create<Ignore>());
    c->addHandler(5, Nop());

    std::error_code e = c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FAIL_IF_IGNORED);
    CHECK(e == SignalErrorCode::INITIALLY_IGNORED);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: forced ignore w/o handler") {
    actions().emplace(5, Action::create<Ignore>());

    std::error_code e = c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(e.value() == 0);
    CHECK(actions().at(5).index() == Action::index<Ignore>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: ignore and add handler") {
    c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    SignalNumber v = 0;
    auto handlerResult = c->addHandler(5, [&v](SignalNumber n) { v = n; });

    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();
    CHECK(v == 5);

    handlerResult.value<Canceler>()();
    CHECK(a.index() == Action::index<Ignore>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: add handler and ignore") {
    c->addHandler(5, Nop());
    c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: trap w/o handler") {
    SignalNumber v = 0;
    HandlerConfiguration::Handler h = [&v](SignalNumber n) { v = n; };
    std::error_code e = c->setTrap(
            5,
            TrapAction::of(std::move(h)),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(e.value() == 0);

    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();
    CHECK(v == 5);

    e = c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(e.value() == 0);
    CHECK(a.index() == Action::index<Default>());
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: trap and add another handler") {
    SignalNumber v1 = 0, v2 = 0;
    HandlerConfiguration::Handler h = [&v1](SignalNumber n) { v1 = n; };
    c->setTrap(
            5,
            TrapAction::of(std::move(h)),
            HandlerConfiguration::SettingPolicy::FORCE);
    c->addHandler(5, [&v2](SignalNumber n) { v2 = n; });

    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();
    CHECK(v1 == 5);
    CHECK(v2 == 5);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: add handler and trap") {
    SignalNumber v1 = 0, v2 = 0;
    HandlerConfiguration::Handler h = [&v1](SignalNumber n) { v1 = n; };
    c->addHandler(5, [&v2](SignalNumber n) { v2 = n; });
    c->setTrap(
            5,
            TrapAction::of(std::move(h)),
            HandlerConfiguration::SettingPolicy::FORCE);

    Action &a = actions().at(5);
    REQUIRE(a.index() == Action::index<sesh_osapi_signal_handler *>());

    a.value<sesh_osapi_signal_handler *>()(5);
    c->callHandlers();
    CHECK(v1 == 5);
    CHECK(v2 == 5);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: signal is blocked when trap is set") {
    c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(Nop()),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(signalMask().test(5));

    c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK_FALSE(signalMask().test(5));
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: signal is restored when trap is unset") {
    signalMask().set(5);

    c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Handler>(Nop()),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(signalMask().test(5));

    c->setTrap(
            5,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(signalMask().test(5));
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: trapping invalid signal") {
    std::error_code e = c->setTrap(
            INVALID_SIGNAL_NUMBER,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    CHECK(e == std::errc::invalid_argument);
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: mask for pselect with handlers") {
    signalMask().set(1);
    signalMask().set(2);
    auto actionResult2 = c->addHandler(2, Nop());
    auto actionResult3 = c->addHandler(3, Nop());

    const SignalNumberSet *set = c->maskForPselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK_FALSE(set->test(2));
    CHECK_FALSE(set->test(3));

    actionResult2.value<Canceler>()();
    actionResult3.value<Canceler>()();

    set = c->maskForPselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK(set->test(2));
    CHECK_FALSE(set->test(3));
}

TEST_CASE_METHOD(
        Fixture<SignalApiFake>,
        "Handler configuration: mask for pselect with ignore") {
    signalMask().set(1);
    signalMask().set(2);
    c->setTrap(
            2,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    c->setTrap(
            3,
            TrapAction::create<HandlerConfiguration::Handler>(),
            HandlerConfiguration::SettingPolicy::FORCE);

    const SignalNumberSet *set = c->maskForPselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK_FALSE(set->test(2));
    CHECK_FALSE(set->test(3));

    c->setTrap(
            2,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);
    c->setTrap(
            3,
            TrapAction::create<HandlerConfiguration::Default>(),
            HandlerConfiguration::SettingPolicy::FORCE);

    set = c->maskForPselect();
    REQUIRE(set != nullptr);
    CHECK(set->test(1));
    CHECK(set->test(2));
    CHECK_FALSE(set->test(3));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
