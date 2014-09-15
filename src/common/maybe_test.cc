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

#include <algorithm>
#include <exception>
#include <string>
#include <utility>
#include "catch.hpp"
#include "common/container_helper.hh"
#include "common/maybe.hh"
#include "common/type_tag.hh"

namespace {

using sesh::common::contains;
using sesh::common::maybe;
using sesh::common::type_tag;

enum class action {
    standard_construction,
    copy_construction,
    move_construction,
    copy_assignment,
    move_assignment,
    destruction,
};

class stub {

private:

    std::vector<action> &m_vector;

public:

    stub(std::vector<action> &v) noexcept : m_vector(v) {
        m_vector.emplace_back(action::standard_construction);
    }

    stub(const stub &s) noexcept : m_vector(s.m_vector) {
        m_vector.emplace_back(action::copy_construction);
    }

    stub(stub &&s) noexcept : m_vector(s.m_vector) {
        m_vector.emplace_back(action::move_construction);
    }

    stub &operator=(const stub &) {
        m_vector.emplace_back(action::copy_assignment);
        return *this;
    }

    stub &operator=(stub &&) {
        m_vector.emplace_back(action::move_assignment);
        return *this;
    }

    ~stub() {
        m_vector.emplace_back(action::destruction);
    }

};

struct exception : public std::exception {
    using std::exception::exception;
};
struct non_constructible {
    non_constructible() = delete;
};
struct default_throws {
    default_throws() { throw exception(); }
    default_throws(int) { }
};
struct copy_only {
    copy_only() = default;
    copy_only(const copy_only &) = default;
};
struct move_only {
    move_only() = default;
    move_only(const move_only &) = delete;
    move_only(move_only &&) = default;
};
struct destructor_throws {
    ~destructor_throws() noexcept(false) { throw exception(); }
};

void swap(move_only &, move_only &) noexcept { }

TEST_CASE("Maybe empty construction") {
    maybe<int> m1;
    const maybe<int> m2(m1);
    const maybe<int> m3(std::move(m2));

    maybe<non_constructible> m4;
    const maybe<non_constructible> m5(m4);
    const maybe<non_constructible> m6(std::move(m5));

    maybe<copy_only> m7;
    maybe<copy_only> m8(m7);
    maybe<copy_only> m9(std::move(m8));

    maybe<move_only> m10;
    maybe<move_only> m11(std::move(m10));
}

TEST_CASE("Maybe non-empty construction with tag") {
    maybe<int> m1((type_tag<int>()));
    maybe<int> m2(type_tag<int>(), 123);
    maybe<std::string> m3((type_tag<std::string>()));
    maybe<std::string> m4(type_tag<std::string>(), 5, '*');
}

TEST_CASE("Maybe non-empty construction by copy and move") {
    maybe<int> m1(123);

    copy_only c;
    maybe<copy_only> m2(c);

    maybe<move_only> m3((move_only()));
}

TEST_CASE("Maybe has value") {
    maybe<int> m1;
    CHECK(!m1.has_value());
    CHECK_FALSE(m1);

    maybe<int> m2(123);
    CHECK(m2.has_value());
    CHECK(m2);
}

TEST_CASE("Maybe value") {
    maybe<int> m1(123);
    CHECK(m1.value() == 123);
    m1.value() = 456;

    const maybe<int> m2(m1);
    CHECK(m2.value() == 456);

    maybe<std::string> m3(type_tag<std::string>(), 5, '*');
    maybe<std::string> m4(std::move(m3));
    CHECK(m4.value() == "*****");
}

TEST_CASE("Maybe *") {
    maybe<std::string> m1("");
    CHECK_NOTHROW((*m1).assign(3, '*'));

    const maybe<std::string> m2(m1);
    CHECK(*m1 == "***");
}

TEST_CASE("Maybe ->") {
    maybe<std::string> m1("");
    CHECK_NOTHROW(m1->assign(3, '*'));

    const maybe<std::string> m2(m1);
    CHECK(m1->size() == 3);
}

TEST_CASE("Maybe value or") {
    int i = 123;

    maybe<int> m1;
    CHECK(m1.value_or(123) == 123);
    m1.value_or(i) = 456;
    CHECK_FALSE(m1.has_value());
    CHECK(i == 456);

    const maybe<int> m1c(m1);
    CHECK(m1c.value_or(123) == 123);

    maybe<int> m2(456);
    CHECK(m2.value_or(123) == 456);
    m2.value_or(i) = 789;
    CHECK(m2.value() == 789);
    CHECK(i == 456);

    const maybe<int> m2c(m2);
    CHECK(m2c.value_or(123) == 789);
}

TEST_CASE("Maybe emplacement") {
    maybe<std::string> m;

    m.emplace("test");
    REQUIRE(m.has_value());
    CHECK(m.value() == "test");

    m.emplace(3, '!');
    REQUIRE(m.has_value());
    CHECK(m.value() == "!!!");
}

TEST_CASE("Maybe emplacement with exception from constructor") {
    maybe<default_throws> m;

    CHECK_FALSE(m.has_value());
    CHECK_THROWS_AS(m.emplace(), exception);
    CHECK_FALSE(m.has_value());

    CHECK_NOTHROW(m.emplace(0));
    CHECK(m.has_value());
    CHECK_THROWS_AS(m.emplace(), exception);
    CHECK_FALSE(m.has_value());
}

TEST_CASE("Maybe emplacement with exception from destructor") {
    maybe<destructor_throws> m;

    CHECK_NOTHROW(m.emplace());
    CHECK(m.has_value());

    CHECK_THROWS_AS(m.emplace(), exception);
    CHECK_FALSE(m.has_value());
}

TEST_CASE("Maybe clear") {
    maybe<int> m(0);
    m.clear();
    CHECK_FALSE(m.has_value());
    m.clear();
    CHECK_FALSE(m.has_value());
}

TEST_CASE("Maybe clear with exception from destructor") {
    maybe<destructor_throws> m;

    CHECK_NOTHROW(m.emplace());
    CHECK(m.has_value());

    CHECK_THROWS_AS(m.clear(), exception);
    CHECK_FALSE(m.has_value());
}

TEST_CASE("Maybe assignment") {
    maybe<long> m1;
    const maybe<long> m2(123L);

    m1 = m1;
    CHECK_FALSE(m1.has_value());

    m1 = m2;
    REQUIRE(m1.has_value());
    CHECK(m1.value() == 123L);

    m1 = maybe<long>(456L);
    REQUIRE(m1.has_value());
    CHECK(m1.value() == 456L);

    m1 = maybe<long>();
    CHECK_FALSE(m1.has_value());

    m1 = 789;
    REQUIRE(m1.has_value());
    CHECK(m1.value() == 789L);
}

TEST_CASE("Maybe assignment optimization") {
    std::vector<action> actions1, actions2;
    {
        maybe<stub> m;

        m = stub(actions1);
        CHECK(actions1.size() == 3);

        m = stub(actions2);
        CHECK(actions1.size() == 4);
        CHECK(actions2.size() == 2);
    }
    CHECK(actions1.size() == 5);
    CHECK(actions1.at(0) == action::standard_construction); // of temporary
    CHECK(actions1.at(1) == action::move_construction); // in maybe
    CHECK(actions1.at(2) == action::destruction); // of temporary
    CHECK(actions1.at(3) == action::move_assignment); // into maybe
    CHECK(actions1.at(4) == action::destruction); // of maybe
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == action::standard_construction);
    CHECK(actions2.at(1) == action::destruction);
}

TEST_CASE("Maybe swap, both non-empty") {
    maybe<std::string> m1("ABC");
    maybe<std::string> m2("XYZ");

    using std::swap;
    swap(m1, m2);
    REQUIRE(m1.has_value());
    REQUIRE(m2.has_value());
    CHECK(m1.value() == "XYZ");
    CHECK(m2.value() == "ABC");

    m2.swap(m1);
    REQUIRE(m1.has_value());
    REQUIRE(m2.has_value());
    CHECK(m1.value() == "ABC");
    CHECK(m2.value() == "XYZ");
}

TEST_CASE("Maybe swap, empty and non-empty") {
    maybe<move_only> m1;
    maybe<move_only> m2 = move_only();

    m1.swap(m2);
    CHECK(m1.has_value());
    CHECK_FALSE(m2.has_value());

    m1.swap(m2);
    CHECK_FALSE(m1.has_value());
    CHECK(m2.has_value());

    using std::swap;
    swap(m1, m2);
    CHECK(m1.has_value());
    CHECK_FALSE(m2.has_value());

    swap(m1, m2);
    CHECK_FALSE(m1.has_value());
    CHECK(m2.has_value());
}

TEST_CASE("Maybe swap, both empty") {
    maybe<int> m1, m2;
    m1.swap(m2);
    CHECK_FALSE(m1.has_value());
    CHECK_FALSE(m2.has_value());
}

TEST_CASE("Maybe ==") {
    maybe<int> m1;
    maybe<int> m2(0);
    maybe<int> m3(1);

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
    maybe<int> m1;
    maybe<int> m2(0);
    maybe<int> m3(1);

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
    using sesh::common::make_maybe;

    maybe<int> m1 = make_maybe<int>(123);
    REQUIRE(m1.has_value());
    CHECK(m1.value() == 123);

    maybe<std::string> m2 = make_maybe<std::string>("ABC", 2);
    REQUIRE(m2.has_value());
    CHECK(m2.value() == "AB");

    auto m3 = make_maybe<move_only>();
    CHECK(m3.has_value());

    std::vector<action> actions;
    {
        auto m4 = make_maybe<stub>(stub(actions));
        CHECK(m4.has_value());
    }
    CHECK_FALSE(contains(actions, action::copy_construction));
    actions.clear();

    {
        stub s(actions);
        auto m5 = make_maybe<stub>(s);
        CHECK(m5.has_value());
    }
    CHECK(contains(actions, action::copy_construction));
}

TEST_CASE("Maybe, create maybe of") {
    using sesh::common::make_maybe_of;

    maybe<int> m1 = make_maybe_of(123);
    REQUIRE(m1.has_value());
    CHECK(m1.value() == 123);

    maybe<std::string> m2 = make_maybe_of<std::string>("ABC");
    REQUIRE(m2.has_value());
    CHECK(m2.value() == "ABC");

    auto m3 = make_maybe_of(move_only());
    CHECK(m3.has_value());

    std::vector<action> actions;
    {
        auto m4 = make_maybe_of(stub(actions));
        CHECK(m4.has_value());
    }
    CHECK_FALSE(contains(actions, action::copy_construction));
    actions.clear();

    {
        stub s(actions);
        auto m5 = make_maybe_of(s);
        CHECK(m5.has_value());
    }
    CHECK(contains(actions, action::copy_construction));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
