/* Copyright (C) 2013 WATANABE Yuki
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

#include <algorithm>
#include <exception>
#include <string>
#include <utility>
#include "common/container_helper.hh"
#include "common/Maybe.hh"
#include "common/TypeTag.hh"

namespace {

using sesh::common::Maybe;
using sesh::common::TypeTag;
using sesh::common::contains;

enum class Action {
    STANDARD_CONSTRUCTION,
    COPY_CONSTRUCTION,
    MOVE_CONSTRUCTION,
    COPY_ASSIGNMENT,
    MOVE_ASSIGNMENT,
    DESTRUCTION,
};

class Stub {

private:

    std::vector<Action> &mVector;

public:

    Stub(std::vector<Action> &v) noexcept : mVector(v) {
        mVector.emplace_back(Action::STANDARD_CONSTRUCTION);
    }

    Stub(const Stub &s) noexcept : mVector(s.mVector) {
        mVector.emplace_back(Action::COPY_CONSTRUCTION);
    }

    Stub(Stub &&s) noexcept : mVector(s.mVector) {
        mVector.emplace_back(Action::MOVE_CONSTRUCTION);
    }

    Stub &operator=(const Stub &) {
        mVector.emplace_back(Action::COPY_ASSIGNMENT);
        return *this;
    }

    Stub &operator=(Stub &&) {
        mVector.emplace_back(Action::MOVE_ASSIGNMENT);
        return *this;
    }

    ~Stub() {
        mVector.emplace_back(Action::DESTRUCTION);
    }

};

struct Exception : public std::exception {
    using std::exception::exception;
};
struct NonConstructible {
    NonConstructible() = delete;
};
struct DefaultThrows {
    DefaultThrows() { throw Exception(); }
    DefaultThrows(int) { }
};
struct CopyOnly {
    CopyOnly() = default;
    CopyOnly(const CopyOnly &) = default;
};
struct MoveOnly {
    MoveOnly() = default;
    MoveOnly(const MoveOnly &) = delete;
    MoveOnly(MoveOnly &&) = default;
};
struct DestructorThrows {
    ~DestructorThrows() noexcept(false) { throw Exception(); }
};

void swap(MoveOnly &, MoveOnly &) noexcept { }

TEST_CASE("Maybe empty construction") {
    Maybe<int> m1;
    const Maybe<int> m2(m1);
    const Maybe<int> m3(std::move(m2));

    Maybe<NonConstructible> m4;
    const Maybe<NonConstructible> m5(m4);
    const Maybe<NonConstructible> m6(std::move(m5));

    Maybe<CopyOnly> m7;
    Maybe<CopyOnly> m8(m7);
    Maybe<CopyOnly> m9(std::move(m8));

    Maybe<MoveOnly> m10;
    Maybe<MoveOnly> m11(std::move(m10));
}

TEST_CASE("Maybe non-empty construction with tag") {
    Maybe<int> m1((TypeTag<int>()));
    Maybe<int> m2(TypeTag<int>(), 123);
    Maybe<std::string> m3((TypeTag<std::string>()));
    Maybe<std::string> m4(TypeTag<std::string>(), 5, '*');
}

TEST_CASE("Maybe non-empty construction by copy and move") {
    Maybe<int> m1(123);

    CopyOnly c;
    Maybe<CopyOnly> m2(c);

    Maybe<MoveOnly> m3((MoveOnly()));
}

TEST_CASE("Maybe has value") {
    Maybe<int> m1;
    CHECK(!m1.hasValue());
    CHECK_FALSE(m1);

    Maybe<int> m2(123);
    CHECK(m2.hasValue());
    CHECK(m2);
}

TEST_CASE("Maybe value") {
    Maybe<int> m1(123);
    CHECK(m1.value() == 123);
    m1.value() = 456;

    const Maybe<int> m2(m1);
    CHECK(m2.value() == 456);

    Maybe<std::string> m3(TypeTag<std::string>(), 5, '*');
    Maybe<std::string> m4(std::move(m3));
    CHECK(m4.value() == "*****");
}

TEST_CASE("Maybe *") {
    Maybe<std::string> m1("");
    CHECK_NOTHROW((*m1).assign(3, '*'));

    const Maybe<std::string> m2(m1);
    CHECK(*m1 == "***");
}

TEST_CASE("Maybe ->") {
    Maybe<std::string> m1("");
    CHECK_NOTHROW(m1->assign(3, '*'));

    const Maybe<std::string> m2(m1);
    CHECK(m1->size() == 3);
}

TEST_CASE("Maybe value or") {
    int i = 123;

    Maybe<int> m1;
    CHECK(m1.valueOr(123) == 123);
    m1.valueOr(i) = 456;
    CHECK_FALSE(m1.hasValue());
    CHECK(i == 456);

    const Maybe<int> m1c(m1);
    CHECK(m1c.valueOr(123) == 123);

    Maybe<int> m2(456);
    CHECK(m2.valueOr(123) == 456);
    m2.valueOr(i) = 789;
    CHECK(m2.value() == 789);
    CHECK(i == 456);

    const Maybe<int> m2c(m2);
    CHECK(m2c.valueOr(123) == 789);
}

TEST_CASE("Maybe emplacement") {
    Maybe<std::string> m;

    m.emplace("test");
    REQUIRE(m.hasValue());
    CHECK(m.value() == "test");

    m.emplace(3, '!');
    REQUIRE(m.hasValue());
    CHECK(m.value() == "!!!");
}

TEST_CASE("Maybe emplacement with exception from constructor") {
    Maybe<DefaultThrows> m;

    CHECK_FALSE(m.hasValue());
    CHECK_THROWS_AS(m.emplace(), Exception);
    CHECK_FALSE(m.hasValue());

    CHECK_NOTHROW(m.emplace(0));
    CHECK(m.hasValue());
    CHECK_THROWS_AS(m.emplace(), Exception);
    CHECK_FALSE(m.hasValue());
}

TEST_CASE("Maybe emplacement with exception from destructor") {
    Maybe<DestructorThrows> m;

    CHECK_NOTHROW(m.emplace());
    CHECK(m.hasValue());

    CHECK_THROWS_AS(m.emplace(), Exception);
    CHECK_FALSE(m.hasValue());
}

TEST_CASE("Maybe clear") {
    Maybe<int> m(0);
    m.clear();
    CHECK_FALSE(m.hasValue());
    m.clear();
    CHECK_FALSE(m.hasValue());
}

TEST_CASE("Maybe clear with exception from destructor") {
    Maybe<DestructorThrows> m;

    CHECK_NOTHROW(m.emplace());
    CHECK(m.hasValue());

    CHECK_THROWS_AS(m.clear(), Exception);
    CHECK_FALSE(m.hasValue());
}

TEST_CASE("Maybe assignment") {
    Maybe<long> m1;
    const Maybe<long> m2(123L);

    m1 = m1;
    CHECK_FALSE(m1.hasValue());

    m1 = m2;
    REQUIRE(m1.hasValue());
    CHECK(m1.value() == 123L);

    m1 = Maybe<long>(456L);
    REQUIRE(m1.hasValue());
    CHECK(m1.value() == 456L);

    m1 = Maybe<long>();
    CHECK_FALSE(m1.hasValue());

    m1 = 789;
    REQUIRE(m1.hasValue());
    CHECK(m1.value() == 789L);
}

TEST_CASE("Maybe assignment optimization") {
    std::vector<Action> actions1, actions2;
    {
        Maybe<Stub> m;

        m = Stub(actions1);
        CHECK(actions1.size() == 3);

        m = Stub(actions2);
        CHECK(actions1.size() == 4);
        CHECK(actions2.size() == 2);
    }
    CHECK(actions1.size() == 5);
    CHECK(actions1.at(0) == Action::STANDARD_CONSTRUCTION); // of temporary
    CHECK(actions1.at(1) == Action::MOVE_CONSTRUCTION); // in maybe
    CHECK(actions1.at(2) == Action::DESTRUCTION); // of temporary
    CHECK(actions1.at(3) == Action::MOVE_ASSIGNMENT); // into maybe
    CHECK(actions1.at(4) == Action::DESTRUCTION); // of maybe
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions2.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Maybe swap, both non-empty") {
    Maybe<std::string> m1("ABC");
    Maybe<std::string> m2("XYZ");

    using std::swap;
    swap(m1, m2);
    REQUIRE(m1.hasValue());
    REQUIRE(m2.hasValue());
    CHECK(m1.value() == "XYZ");
    CHECK(m2.value() == "ABC");

    m2.swap(m1);
    REQUIRE(m1.hasValue());
    REQUIRE(m2.hasValue());
    CHECK(m1.value() == "ABC");
    CHECK(m2.value() == "XYZ");
}

TEST_CASE("Maybe swap, empty and non-empty") {
    Maybe<MoveOnly> m1;
    Maybe<MoveOnly> m2 = MoveOnly();

    m1.swap(m2);
    CHECK(m1.hasValue());
    CHECK_FALSE(m2.hasValue());

    m1.swap(m2);
    CHECK_FALSE(m1.hasValue());
    CHECK(m2.hasValue());

    using std::swap;
    swap(m1, m2);
    CHECK(m1.hasValue());
    CHECK_FALSE(m2.hasValue());

    swap(m1, m2);
    CHECK_FALSE(m1.hasValue());
    CHECK(m2.hasValue());
}

TEST_CASE("Maybe swap, both empty") {
    Maybe<int> m1, m2;
    m1.swap(m2);
    CHECK_FALSE(m1.hasValue());
    CHECK_FALSE(m2.hasValue());
}

TEST_CASE("Maybe ==") {
    Maybe<int> m1;
    Maybe<int> m2(0);
    Maybe<int> m3(1);

    CHECK(m1 == m1);
    CHECK_FALSE(m1 == m2);
    CHECK_FALSE(m1 == m3);
    CHECK_FALSE(m2 == m1);
    CHECK(m2 == m2);
    CHECK_FALSE(m2 == m3);
    CHECK_FALSE(m3 == m1);
    CHECK_FALSE(m3 == m2);
    CHECK(m3 == m3);
}

TEST_CASE("Maybe <") {
    Maybe<int> m1;
    Maybe<int> m2(0);
    Maybe<int> m3(1);

    CHECK_FALSE(m1 < m1);
    CHECK(m1 < m2);
    CHECK(m1 < m3);
    CHECK_FALSE(m2 < m1);
    CHECK_FALSE(m2 < m2);
    CHECK(m2 < m3);
    CHECK_FALSE(m3 < m1);
    CHECK_FALSE(m3 < m2);
    CHECK_FALSE(m3 < m3);
}

TEST_CASE("Maybe, create maybe") {
    using sesh::common::createMaybe;

    Maybe<int> m1 = createMaybe<int>(123);
    REQUIRE(m1.hasValue());
    CHECK(m1.value() == 123);

    Maybe<std::string> m2 = createMaybe<std::string>("ABC", 2);
    REQUIRE(m2.hasValue());
    CHECK(m2.value() == "AB");

    auto m3 = createMaybe<MoveOnly>();
    CHECK(m3.hasValue());

    std::vector<Action> actions;
    {
        auto m4 = createMaybe<Stub>(Stub(actions));
        CHECK(m4.hasValue());
    }
    CHECK_FALSE(contains(actions, Action::COPY_CONSTRUCTION));
    actions.clear();

    {
        Stub s(actions);
        auto m5 = createMaybe<Stub>(s);
        CHECK(m5.hasValue());
    }
    CHECK(contains(actions, Action::COPY_CONSTRUCTION));
}

TEST_CASE("Maybe, create maybe of") {
    using sesh::common::createMaybeOf;

    Maybe<int> m1 = createMaybeOf(123);
    REQUIRE(m1.hasValue());
    CHECK(m1.value() == 123);

    Maybe<std::string> m2 = createMaybeOf<std::string>("ABC");
    REQUIRE(m2.hasValue());
    CHECK(m2.value() == "ABC");

    auto m3 = createMaybeOf(MoveOnly());
    CHECK(m3.hasValue());

    std::vector<Action> actions;
    {
        auto m4 = createMaybeOf(Stub(actions));
        CHECK(m4.hasValue());
    }
    CHECK_FALSE(contains(actions, Action::COPY_CONSTRUCTION));
    actions.clear();

    {
        Stub s(actions);
        auto m5 = createMaybeOf(s);
        CHECK(m5.hasValue());
    }
    CHECK(contains(actions, Action::COPY_CONSTRUCTION));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
