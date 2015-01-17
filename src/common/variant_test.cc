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
#include <type_traits>
#include <utility>
#include <vector>
#include "catch.hpp"
#include "common/container_helper.hh"
#include "common/direct_initialize.hh"
#include "common/type_tag.hh"
#include "common/variant.hh"

namespace {

using sesh::common::contains;
using sesh::common::direct_initialize;
using sesh::common::type_tag;
using sesh::common::variant;

constexpr direct_initialize DI{};

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
struct default_may_throw {
    default_may_throw() noexcept(false) { }
};
struct default_throws {
    default_throws() { throw exception(); }
    ~default_throws() noexcept(false) {
        FAIL("default_throws destructor must not be called");
    }
};
struct copy_may_throw {
    copy_may_throw() = default;
    copy_may_throw(const copy_may_throw &) noexcept(false) { }
};
struct copy_throws {
    copy_throws() = default;
    copy_throws(const copy_throws &) { throw exception(); }
    copy_throws(copy_throws &&) = default;
    copy_throws &operator=(const copy_throws &) = default;
};
struct non_copyable {
    non_copyable() = default;
    non_copyable(const non_copyable &) = delete;
};
struct move_may_throw {
    move_may_throw() = default;
    move_may_throw(move_may_throw &&) noexcept(false) { }
};
struct move_throws {
    move_throws() = default;
    move_throws(move_throws &&) { throw exception(); }
    move_throws &operator=(const move_throws &) = default;
};
struct non_movable {
    non_movable() = default;
    non_movable(const non_movable &) = delete;
    non_movable(non_movable &&) = delete;
};
struct non_copy_assignable {
    non_copy_assignable &operator=(const non_copy_assignable &) = delete;
};
struct non_move_assignable {
    non_move_assignable() = default;
    non_move_assignable(non_move_assignable &&) = default;
    non_move_assignable &operator=(non_move_assignable &&) = delete;
};
struct move_only_int {
    int value;
    move_only_int(int i) noexcept : value(i) { }
    move_only_int(move_only_int &&) = default;
};
struct move_only_double {
    double value;
    move_only_double(double d) noexcept : value(d) { }
    move_only_double(move_only_double &&) = default;
};

bool operator==(const move_only_int &l, const move_only_int &r) {
    return l.value == r.value;
}
bool operator==(const move_only_double &l, const move_only_double &r) {
    return l.value == r.value;
}

static_assert(
        sizeof(variant<>) > 0,
        "Empty variant is a valid type, even if not constructible");

TEST_CASE("Single variant construction & destruction") {
    variant<int>(DI, type_tag<int>());
    variant<int>(DI, type_tag<int>(), 123);
    variant<int>(123);

    static_assert(
            noexcept(variant<int>(DI, type_tag<int>())),
            "int is no-throw constructible & destructible");

    std::vector<action> actions;
    variant<stub>(DI, type_tag<stub>(), actions);
    CHECK(actions.size() == 2);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::destruction);

    CHECK_THROWS_AS(
            variant<default_throws>(DI, type_tag<default_throws>()),
            exception);
}

//TEST_CASE("Single variant throwing destructor") {
    static_assert(
            !noexcept(variant<default_may_throw>(
                    DI, type_tag<default_may_throw>())),
            "default_may_throw constructor may throw");
//}

TEST_CASE("Double variant construction & destruction") {
    struct A {
    };
    struct B {
        B(int, double) noexcept(false) { }
    };

    variant<A, B>(DI, type_tag<A>());
    variant<A, B>(DI, type_tag<B>(), 0, 0.0);
    variant<A, B>{A()};
    variant<A, B>{B(0, 0.0)};

    static_assert(
            noexcept(variant<A, B>(DI, type_tag<A>())),
            "A is no-throw constructible & destructible");
    static_assert(
            !noexcept(variant<A, B>(DI, type_tag<B>(), 0, 0.0)),
            "B is not no-throw constructible");
}

//TEST_CASE("Double variant throwing constructor") {
    static_assert(
            !noexcept(variant<default_may_throw, int>(
                    DI, type_tag<default_may_throw>())),
            "default_may_throw constructor may throw");
    static_assert(
            !noexcept(variant<int, default_may_throw>(
                    DI, type_tag<default_may_throw>())),
            "default_may_throw constructor may throw");
    static_assert(
            !noexcept(variant<default_may_throw, default_may_throw>(
                    DI, type_tag<default_may_throw>())),
            "default_may_throw constructor may throw");
//}

TEST_CASE("Quad variant construction & destruction") {
    class A { };
    class B { };
    class C { };
    class D { };
    variant<A, B, C, D>(DI, type_tag<A>());
    variant<A, B, C, D>(DI, type_tag<B>());
    variant<A, B, C, D>(DI, type_tag<C>());
    variant<A, B, C, D>(DI, type_tag<D>());
    variant<A, B, C, D>{A()};
    variant<A, B, C, D>{B()};
    variant<A, B, C, D>{C()};
    variant<A, B, C, D>{D()};
}

TEST_CASE("Double variant copy initialization") {
    variant<int, type_tag<int>> vi = 0;
    CHECK(vi.tag() == vi.tag<int>());

    variant<int, type_tag<int>> vt = type_tag<int>();
    CHECK(vt.tag() == vt.tag<type_tag<int>>());
}

TEST_CASE("Double variant direct initialization") {
    variant<int, type_tag<int>> vi(DI, type_tag<int>());
    CHECK(vi.tag() == vi.tag<int>());

    variant<int, type_tag<int>> vt(DI, type_tag<type_tag<int>>());
    CHECK(vt.tag() == vt.tag<type_tag<int>>());
}

TEST_CASE("Double variant value") {
    const int I1 = 123, I2 = 234;
    const float F1 = 456.0f, F2 = 567.0f;

    variant<int, float> i(DI, type_tag<int>(), I1);
    variant<int, float> f(DI, type_tag<float>(), F1);

    CHECK(i.value<int>() == I1);
    CHECK(f.value<float>() == F1);

    i.value<int>() = I2;
    f.value<float>() = F2;

    CHECK(i.value<int>() == I2);
    CHECK(f.value<float>() == F2);
}

TEST_CASE("Double variant constant value") {
    const int I = 123;
    const float F = 456.0;

    const variant<int, float> i(DI, type_tag<int>(), I);
    const variant<int, float> f(DI, type_tag<float>(), F);

    CHECK(i.value<int>() == I);
    CHECK(f.value<float>() == F);
}

TEST_CASE("Double variant r-value") {
    std::vector<action> actions;
    {
        variant<int, stub> v(DI, type_tag<stub>(), actions);
        CHECK(actions.size() == 1);

        stub stub_copy(v.value<stub>());
        CHECK(actions.size() == 2);

        stub stub_move(std::move(v).value<stub>());
        CHECK(actions.size() == 3);
    }
    CHECK(actions.size() == 6);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::copy_construction);
    CHECK(actions.at(2) == action::move_construction);
    CHECK(actions.at(3) == action::destruction);
    CHECK(actions.at(4) == action::destruction);
    CHECK(actions.at(5) == action::destruction);
}

struct template_visitor {
    template<typename T>
    std::string operator()(T v) { return std::to_string(v); }
    // A local class cannot have a template member.
};

TEST_CASE("Double variant apply with templated visitor") {
    variant<int, double> vi = 3;
    std::string si = vi.apply(template_visitor());
    CHECK(si == "3");

    variant<int, double> vd = 1.5;
    std::string sd = vd.apply(template_visitor());
    CHECK(sd == "1.500000");
}

TEST_CASE("Double variant apply, contained value l/r-value") {
    struct visitor {
        std::string operator()(int &) { return "int &"; }
        std::string operator()(const int &) { return "const int &"; }
        std::string operator()(int &&) { return "int &&"; }
        std::string operator()(double &) { return "double &"; }
        std::string operator()(const double &) { return "const double &"; }
        std::string operator()(double &&) { return "double &&"; }
    };

    auto vi = variant<int, double>::create<int>();
    CHECK(vi.apply(visitor()) == "int &");
    CHECK(std::move(vi).apply(visitor()) == "int &&");

    const auto cvi = variant<int, double>::create<int>();
    CHECK(cvi.apply(visitor()) == "const int &");
    CHECK(std::move(cvi).apply(visitor()) == "const int &");

    auto vd = variant<int, double>::create<double>();
    CHECK(vd.apply(visitor()) == "double &");
    CHECK(std::move(vd).apply(visitor()) == "double &&");

    const auto cvd = variant<int, double>::create<double>();
    CHECK(cvd.apply(visitor()) == "const double &");
    CHECK(std::move(cvd).apply(visitor()) == "const double &");
}

TEST_CASE("Double variant apply, visitor l/r-value") {
    struct visitor {
        visitor() { }
        std::string operator()(int) & { return "int, &"; }
        std::string operator()(int) const & { return "int, const &"; }
        std::string operator()(int) && { return "int, &&"; }
        std::string operator()(double) & { return "double, &"; }
        std::string operator()(double) const & { return "double, const &"; }
        std::string operator()(double) && { return "double, &&"; }
    };

    auto vi = variant<int, double>::create<int>();
    auto vd = variant<int, double>::create<double>();

    visitor v;
    CHECK(vi.apply(v) == "int, &");
    CHECK(vd.apply(v) == "double, &");

    const visitor cv;
    CHECK(vi.apply(cv) == "int, const &");
    CHECK(vd.apply(cv) == "double, const &");

    CHECK(vi.apply(visitor()) == "int, &&");
    CHECK(vd.apply(visitor()) == "double, &&");
}

TEST_CASE("Double variant copy construction") {
    variant<int, double> v1(DI, type_tag<int>(), 7);
    variant<int, double> v2(v1);
    CHECK(v1.tag() == v1.tag<int>());
    CHECK(v2.tag() == v2.tag<int>());
    CHECK(v1.value<int>() == 7);
    CHECK(v2.value<int>() == 7);

    static_assert(
            noexcept(variant<int, double>::is_nothrow_copy_constructible),
            "int and double are no-throw copy constructible");

    std::vector<action> actions;
    {
        variant<int, stub> v3(DI, type_tag<stub>(), actions);
        CHECK(actions.size() == 1);

        variant<int, stub> v4(v3);
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::copy_construction);
    CHECK(actions.at(2) == action::destruction);
    CHECK(actions.at(3) == action::destruction);
}

TEST_CASE("Double variant throwing copy constructor") {
    variant<int, copy_may_throw> v1(DI, type_tag<copy_may_throw>());
    variant<int, copy_may_throw> v2(v1);

    static_assert(
            noexcept(variant<int, copy_may_throw>::
                    is_nothrow_copy_constructible),
            "copy_may_throw has a copy constructor that may throw");
}

TEST_CASE("Double variant deleted copy constructor") {
    variant<int, non_copyable> v1(DI, type_tag<non_copyable>());
    // variant<int, non_copyable> v2(v1);

    // static_assert(
    //         !std::is_copy_constructible<variant<int, non_copyable>>::value,
    //         "non_copyable has no copy constructor");
}

TEST_CASE("Double/quad variant copy construction to supertype") {
    const int I = 17;
    const double D = 2.5;

    const variant<int, double> v2i(DI, type_tag<int>(), I);
    const variant<int, double> v2d(DI, type_tag<double>(), D);

    const variant<double, float, int, long> v4i(v2i);
    const variant<double, float, int, long> v4d(v2d);

    REQUIRE(v4i.tag() == v4i.tag<int>());
    REQUIRE(v4d.tag() == v4d.tag<double>());
    CHECK(v4i.value<int>() == I);
    CHECK(v4d.value<double>() == D);
}

TEST_CASE("Double variant move construction") {
    variant<int, double> v1(DI, type_tag<int>(), 7);
    variant<int, double> v2(std::move(v1));
    REQUIRE(v1.tag() == v1.tag<int>());
    REQUIRE(v2.tag() == v2.tag<int>());
    CHECK(v2.value<int>() == 7);

    static_assert(
            noexcept(variant<int, double>::is_nothrow_move_constructible),
            "int and double are no-throw move constructible");

    std::vector<action> actions;
    {
        variant<int, stub> v3(DI, type_tag<stub>(), actions);
        CHECK(actions.size() == 1);

        variant<int, stub> v4(std::move(v3));
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::move_construction);
    CHECK(actions.at(2) == action::destruction);
    CHECK(actions.at(3) == action::destruction);
}

TEST_CASE("Double variant throwing move constructor") {
    variant<int, move_may_throw> v1(DI, type_tag<move_may_throw>());
    variant<int, move_may_throw> v2(std::move(v1));

    static_assert(
            noexcept(variant<int, move_may_throw>::
                    is_nothrow_move_constructible),
            "move_may_throw has a move constructor that may throw");
}

TEST_CASE("Double variant deleted move constructor") {
    variant<int, non_movable> v1(DI, type_tag<non_movable>());
    // variant<int, non_movable> v2(std::move(v1));

    // static_assert(
    //         !std::is_move_constructible<variant<int, non_movable>>::value,
    //         "non_movable has no move constructor");
}

TEST_CASE("Double/quad variant move construction to supertype") {
    const int I = 3;
    const double D = -7.0;

    using V2 = variant<move_only_int, move_only_double>;
    using V4 = variant<float, move_only_double, int, move_only_int>;

    V2 v2i(DI, type_tag<move_only_int>(), I);
    V2 v2d(DI, type_tag<move_only_double>(), D);

    V4 v4i(std::move(v2i));
    V4 v4d(std::move(v2d));

    REQUIRE(v4i.tag() == v4i.tag<move_only_int>());
    REQUIRE(v4d.tag() == v4d.tag<move_only_double>());
    CHECK(v4i.value<move_only_int>().value == I);
    CHECK(v4d.value<move_only_double>().value == D);
}

TEST_CASE("Double variant emplacement") {
    std::vector<action> actions;
    {
        variant<int, stub> v(DI, type_tag<int>(), 1);
        CHECK(v.value<int>() == 1);

        CHECK_NOTHROW(v.emplace(DI, type_tag<stub>(), actions));
        CHECK(v.tag() == v.tag<stub>());
        CHECK(actions.size() == 1);

        CHECK_NOTHROW(v.emplace(2));
        REQUIRE(v.tag() == v.tag<int>());
        CHECK(v.value<int>() == 2);
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 2);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::destruction);
}

TEST_CASE("Double variant emplacement with fallback") {
    variant<int, move_throws> v(DI, type_tag<int>(), 1);
    CHECK_NOTHROW(v.emplace_with_fallback<int>(DI, type_tag<move_throws>()));
    CHECK(v.tag() == v.tag<move_throws>());
    CHECK_NOTHROW(v.emplace_with_fallback<int>(2));
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 2);
    CHECK_THROWS_AS(v.emplace_with_fallback<int>(move_throws()), exception);
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 0);
}

TEST_CASE("Double variant emplacement with backup, without actual backup") {
    variant<int, double> v = 1.0;
    static_assert(
            noexcept(v.emplace_with_backup(2)), "emplacement is noexcept");
    v.emplace_with_backup(2);
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 2);
}

TEST_CASE("Double variant emplacement with backup, with actual backup") {
    variant<int, default_may_throw> v = 1;
    CHECK_NOTHROW(v.emplace_with_backup(DI, type_tag<default_may_throw>()));
    CHECK(v.tag() == v.tag<default_may_throw>());
}

TEST_CASE("Double variant emplacement with backup; "
        "exception in new value construction") {
    variant<int, default_throws> v = 1;
    CHECK_THROWS_AS(
            v.emplace_with_backup(DI, type_tag<default_throws>()), exception);
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 1);
}

TEST_CASE("Double variant emplacement with backup; "
        "exception in backup construction") {
    using V = variant<move_throws, default_may_throw>;
    V v(DI, type_tag<move_throws>());
    CHECK_THROWS_AS(v.emplace_with_backup(
            DI, type_tag<default_may_throw>()), exception);
    CHECK(v.tag() == v.tag<move_throws>());
}

TEST_CASE("Double variant assignment with same type") {
    std::vector<action> actions1, actions2;
    {
        variant<int, stub> v(DI, type_tag<stub>(), actions1);
        stub s(actions2);
        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v.assign(s));
        CHECK(v.tag() == v.tag<stub>());
        CHECK(actions1.size() == 2);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v.assign(std::move(s)));
        CHECK(v.tag() == v.tag<stub>());
        CHECK(actions1.size() == 3);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 4);
    CHECK(actions1.at(0) == action::standard_construction);
    CHECK(actions1.at(1) == action::copy_assignment);
    CHECK(actions1.at(2) == action::move_assignment);
    CHECK(actions1.at(3) == action::destruction);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == action::standard_construction);
    CHECK(actions2.at(1) == action::destruction);
}

TEST_CASE("Double variant assignment with different types") {
    std::vector<action> actions;
    {
        variant<int, stub> v(DI, type_tag<int>(), 1);

        CHECK_NOTHROW(v.assign(stub(actions)));
        CHECK(v.tag() == v.tag<stub>());
        CHECK(actions.size() == 3);

        CHECK_NOTHROW(v.assign(100));
        REQUIRE(v.tag() == v.tag<int>());
        CHECK(v.value<int>() == 100);
        CHECK(actions.size() == 4);

        stub s(actions);
        CHECK(actions.size() == 5);
        CHECK_NOTHROW(v.assign(s));
        REQUIRE(v.tag() == v.tag<stub>());
        CHECK(actions.size() == 6);
    }
    CHECK(actions.size() == 8);
    CHECK(actions.at(0) == action::standard_construction); // of temporary
    CHECK(actions.at(1) == action::move_construction); // of variant value
    CHECK(actions.at(2) == action::destruction); // of temporary
    CHECK(actions.at(3) == action::destruction); // of variant value
    CHECK(actions.at(4) == action::standard_construction); // of local
    CHECK(actions.at(5) == action::copy_construction); // of variant value
    CHECK(actions.at(6) == action::destruction); // of local
    CHECK(actions.at(7) == action::destruction); // of variant value
}

TEST_CASE("Double variant copy assignment with same tag") {
    std::vector<action> actions1, actions2;
    {
        variant<int, stub> v1(DI, type_tag<stub>(), actions1);
        const variant<int, stub> v2(DI, type_tag<stub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v1 = v2);
        CHECK(v1.tag() == v1.tag<stub>());
        CHECK(actions1.size() == 2);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 3);
    CHECK(actions1.at(0) == action::standard_construction);
    CHECK(actions1.at(1) == action::copy_assignment);
    CHECK(actions1.at(2) == action::destruction);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == action::standard_construction);
    CHECK(actions2.at(1) == action::destruction);
}

TEST_CASE("Double variant copy assignment with different tag") {
    std::vector<action> actions;
    {
        variant<int, stub> v1(DI, type_tag<int>(), 1);
        const variant<int, stub> v2(DI, type_tag<stub>(), actions);
        CHECK(actions.size() == 1);

        CHECK_NOTHROW(v1 = v2);
        CHECK(v1.tag() == v1.tag<stub>());
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::copy_construction);
    CHECK(actions.at(2) == action::destruction);
    CHECK(actions.at(3) == action::destruction);
}

TEST_CASE("Double variant copy assignment with same tag & exception") {
    struct throwing_stub : public stub {
        using stub::stub;
        throwing_stub &operator=(const throwing_stub &) { throw exception(); }
    };

    std::vector<action> actions1, actions2;
    {
        variant<int, throwing_stub> v1(
                DI, type_tag<throwing_stub>(), actions1);
        const variant<int, throwing_stub> v2(
                DI, type_tag<throwing_stub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_THROWS_AS(v1 = v2, exception);
        CHECK(v1.tag() == v1.tag<throwing_stub>());
        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 2);
    CHECK(actions1.at(0) == action::standard_construction);
    CHECK(actions1.at(1) == action::destruction);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == action::standard_construction);
    CHECK(actions2.at(1) == action::destruction);
}

TEST_CASE("Double variant copy assignment with different tag & exception") {
    static_assert(
            std::is_copy_assignable<variant<int, stub>>::value,
            "int and stub are copy-assignable");
    static_assert(
            std::is_nothrow_copy_assignable<variant<int, double>>::value,
            "int and double are no-throw copy-assignable");
    static_assert(
            !std::is_nothrow_copy_assignable<variant<copy_throws>>::value,
            "copy_throws is copy-assignable but throwing");

    std::vector<action> actions;
    variant<stub, copy_throws> v1(DI, type_tag<stub>(), actions);
    variant<stub, copy_throws> v2(DI, type_tag<copy_throws>());
    CHECK_THROWS_AS(v1 = v2, exception);
    CHECK(v1.tag() == v1.tag<stub>());
    CHECK(v2.tag() == v1.tag<copy_throws>());
    CHECK(actions.size() == 5);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::move_construction); // of backup
    CHECK(actions.at(2) == action::destruction); // of v1
    CHECK(actions.at(3) == action::move_construction); // of v1
    CHECK(actions.at(4) == action::destruction); // of backup
}

TEST_CASE("Double variant no copy assignment") {
    using V = variant<stub, non_copy_assignable>;

    V v(DI, type_tag<non_copy_assignable>());
    // v = v;

    static_assert(
            !std::is_copy_assignable<V>::value,
            "non_copy_assignable is not copy-assignable");
}

TEST_CASE("Double/quad variant copy assignment to supertype") {
    const int I = 17;
    const double D = 2.5;

    const variant<int, double> v2i(DI, type_tag<int>(), I);
    const variant<int, double> v2d(DI, type_tag<double>(), D);

    variant<double, float, int, long> v4fi(DI, type_tag<float>(), 1.23f);
    variant<double, float, int, long> v4ld(DI, type_tag<long>(), 92L);

    v4fi = v2i;
    v4ld = v2d;

    REQUIRE(v4fi.tag() == v4fi.tag<int>());
    REQUIRE(v4ld.tag() == v4ld.tag<double>());
    CHECK(v4fi.value<int>() == I);
    CHECK(v4ld.value<double>() == D);

    std::vector<action> actions;
    {
        variant<int, stub> v2s(DI, type_tag<int>(), I);
        variant<stub> v1s(DI, type_tag<stub>(), actions);
        v2s = v1s;
        CHECK(v2s.tag() == v2s.tag<stub>());
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::copy_construction);
    CHECK(actions.at(2) == action::destruction);
    CHECK(actions.at(3) == action::destruction);
}

TEST_CASE("Double variant move assignment with same tag") {
    std::vector<action> actions1, actions2;
    {
        variant<int, stub> v1(DI, type_tag<stub>(), actions1);
        variant<int, stub> v2(DI, type_tag<stub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v1 = std::move(v2));
        CHECK(v1.tag() == v1.tag<stub>());
        CHECK(actions1.size() == 2);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 3);
    CHECK(actions1.at(0) == action::standard_construction);
    CHECK(actions1.at(1) == action::move_assignment);
    CHECK(actions1.at(2) == action::destruction);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == action::standard_construction);
    CHECK(actions2.at(1) == action::destruction);
}

TEST_CASE("Double variant move assignment with different tag") {
    std::vector<action> actions;
    {
        variant<int, stub> v1(DI, type_tag<int>(), 1);
        variant<int, stub> v2(DI, type_tag<stub>(), actions);
        CHECK(actions.size() == 1);

        CHECK_NOTHROW(v1 = std::move(v2));
        CHECK(v1.tag() == v1.tag<stub>());
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::move_construction);
    CHECK(actions.at(2) == action::destruction);
    CHECK(actions.at(3) == action::destruction);
}

TEST_CASE("Double variant move assignment with same tag & exception") {
    struct throwing_stub : public stub {
        using stub::stub;
        throwing_stub(throwing_stub &&) = default;
        throwing_stub &operator=(throwing_stub &&) { throw exception(); }
    };

    std::vector<action> actions1, actions2;
    {
        variant<int, throwing_stub> v1(
                DI, type_tag<throwing_stub>(), actions1);
        variant<int, throwing_stub> v2
                (DI, type_tag<throwing_stub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_THROWS_AS(v1 = std::move(v2), exception);
        CHECK(v1.tag() == v1.tag<throwing_stub>());
        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 2);
    CHECK(actions1.at(0) == action::standard_construction);
    CHECK(actions1.at(1) == action::destruction);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == action::standard_construction);
    CHECK(actions2.at(1) == action::destruction);
}

TEST_CASE("Double variant move assignment with different tag & exception") {
    static_assert(
            std::is_move_assignable<variant<int, stub>>::value,
            "int and stub are move-assignable");
    static_assert(
            std::is_nothrow_move_assignable<variant<int, double>>::value,
            "int and double are no-throw move-assignable");
    static_assert(
            !std::is_nothrow_move_assignable<variant<move_throws>>::value,
            "move_throws is move-assignable but throwing");

    std::vector<action> actions;
    variant<stub, move_throws> v1(DI, type_tag<stub>(), actions);
    variant<stub, move_throws> v2(DI, type_tag<move_throws>());
    CHECK_THROWS_AS(v1 = std::move(v2), exception);
    CHECK(v1.tag() == v1.tag<stub>());
    CHECK(v2.tag() == v1.tag<move_throws>());
    CHECK(actions.size() == 5);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::move_construction); // of backup
    CHECK(actions.at(2) == action::destruction); // of v1
    CHECK(actions.at(3) == action::move_construction); // of v1
    CHECK(actions.at(4) == action::destruction); // of backup
}

TEST_CASE("Double variant no move assignment") {
    using V = variant<stub, non_move_assignable>;

    V v(DI, type_tag<non_move_assignable>());
//    v = std::move(v);

    static_assert(
            !std::is_move_assignable<V>::value,
            "non_move_assignable is not move-assignable");
}

TEST_CASE("Double/quad variant move assignment to supertype") {
    const int I = 17;
    const double D = 2.5;

    variant<double, float, int, long> v4fi(DI, type_tag<float>(), 1.23f);
    variant<double, float, int, long> v4ld(DI, type_tag<long>(), 92L);

    v4fi = variant<int, double>(DI, type_tag<int>(), I);
    v4ld = variant<int, double>(DI, type_tag<double>(), D);

    REQUIRE(v4fi.tag() == v4fi.tag<int>());
    REQUIRE(v4ld.tag() == v4ld.tag<double>());
    CHECK(v4fi.value<int>() == I);
    CHECK(v4ld.value<double>() == D);

    std::vector<action> actions;
    {
        variant<int, stub> v2s(DI, type_tag<int>(), I);
        v2s = variant<stub>(DI, type_tag<stub>(), actions);
        CHECK(v2s.tag() == v2s.tag<stub>());
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == action::standard_construction);
    CHECK(actions.at(1) == action::move_construction);
    CHECK(actions.at(2) == action::destruction);
    CHECK(actions.at(3) == action::destruction);
}

TEST_CASE("Double variant create") {
    variant<int, double>::create<int>(4);
    variant<int, double>::create<double>(3.14);

    variant<int, double> v1(variant<int, double>::create<int>(2));
    variant<int, double> v2(variant<int, double>::create<double>(5.5));

    using V = variant<std::string, std::wstring, std::vector<char>>;
    V v3 = V::create<std::string>("Hello");
    V v4 = V::create<std::wstring>(7, '!');
    V v5 = V::create<std::vector<char>>({'?', '%'});

    REQUIRE(v3.tag() == v3.tag<std::string>());
    CHECK(v3.value<std::string>() == "Hello");
    REQUIRE(v4.tag() == v4.tag<std::wstring>());
    CHECK(v4.value<std::wstring>() == L"!!!!!!!");
    REQUIRE(v5.tag() == v5.tag<std::vector<char>>());
    CHECK(v5.value<std::vector<char>>().size() == 2u);
    CHECK(v5.value<std::vector<char>>().at(0) == '?');
    CHECK(v5.value<std::vector<char>>().at(1) == '%');
}

TEST_CASE("Double variant of") {
    int int_array[] = {19};

    variant<int *, const char *> v1 = int_array;
    variant<int *, const char *> v2 = "char array";

    REQUIRE(v1.tag() == v1.tag<int *>());
    CHECK(v1.value<int *>() == int_array);
    REQUIRE(v2.tag() == v2.tag<const char *>());
    CHECK(v2.value<const char *>()[0] == 'c');

    std::vector<action> actions;
    {
        variant<int, stub> v3 = stub(actions);
        CHECK(v3.tag() == v3.tag<stub>());
    }
    CHECK_FALSE(contains(actions, action::copy_construction));
    actions.clear();

    {
        stub s(actions);
        variant<int, stub> v4 = s;
        CHECK(v4.tag() == v4.tag<stub>());
    }
    CHECK(contains(actions, action::copy_construction));
}

TEST_CASE("Double variant swapping with same type") {
    using std::swap; // test against the swappable concept

    variant<int, double> v1 = 7;
    variant<int, double> v2 = 13;
    swap(v1, v2);
    REQUIRE(v1.tag() == v1.tag<int>());
    CHECK(v1.value<int>() == 13);
    REQUIRE(v2.tag() == v2.tag<int>());
    CHECK(v2.value<int>() == 7);

    v1.emplace(32.0);
    v2.emplace(8.5);
    v1.swap(v2);
    REQUIRE(v1.tag() == v1.tag<double>());
    CHECK(v1.value<double>() == 8.5);
    REQUIRE(v2.tag() == v2.tag<double>());
    CHECK(v2.value<double>() == 32.0);
}

TEST_CASE("Double variant swapping with different type") {
    using std::swap; // test against the swappable concept

    variant<int, double> v1 = 5;
    variant<int, double> v2 = 13.5;

    swap(v1, v2);
    REQUIRE(v1.tag() == v1.tag<double>());
    CHECK(v1.value<double>() == 13.5);
    REQUIRE(v2.tag() == v2.tag<int>());
    CHECK(v2.value<int>() == 5);

    v1.swap(v2);
    REQUIRE(v1.tag() == v1.tag<int>());
    CHECK(v1.value<int>() == 5);
    REQUIRE(v2.tag() == v2.tag<double>());
    CHECK(v2.value<double>() == 13.5);
}

//TEST_CASE("Double variant unswappable") {
//    variant<int, move_throws> v1(DI, type_tag<int>(), 3);
//    variant<int, move_throws> v2(DI, type_tag<move_throws>());
//    swap(v1, v2);
//}

//TEST_CASE("Double variant throwing swapper") {
    static_assert(
            variant<int, double>::is_nothrow_swappable,
            "int swap never throws");
    static_assert(
            !variant<int, copy_may_throw>::is_nothrow_swappable,
            "copy_may_throw swap may throw");
//}

TEST_CASE("Double variant flat map for matching constant value") {
    struct A { };
    struct B { operator int() const { return 1; } };

    const variant<A, B> v = A();
    int called = 0;
    int i = v.flat_map([&called](A) -> int {
        ++called;
        return 2;
    });
    CHECK(called == 1);
    CHECK(i == 2);
}

TEST_CASE("Double variant flat map for unmatched constant value") {
    struct A { operator int() const { return 1; } };
    struct B { };
    const variant<A, B> v = A();
    int i = v.flat_map([](B) -> int { FAIL(); return 0; });
    CHECK(i == 1);
}

TEST_CASE("Double variant flat map for matching move-only value") {
    variant<move_only_int, move_only_double> v = move_only_double(1.0);
    int called = 0;
    auto f = [&called](move_only_double d) -> move_only_int {
        CHECK(d.value == 1.0);
        ++called;
        return move_only_int(2);
    };
    move_only_int i = std::move(v).flat_map(f);
    CHECK(called == 1);
    CHECK(i.value == 2);
}

TEST_CASE("Double variant flat map for unmatched move-only value") {
    variant<move_only_int, move_only_double> v = move_only_int(1);
    auto f = [](move_only_double) -> move_only_int {
        FAIL();
        return move_only_int(0);
    };
    move_only_int i = std::move(v).flat_map(f);
    CHECK(i.value == 1);
}

TEST_CASE("Double variant flat map with overloaded function") {
    struct A { };
    struct B { };
    struct F {
        int operator()(const A &) { return 1; }
        int operator()(const B &) { return 2; }
        int operator()(A &&) { return 3; }
        int operator()(B &&) { return 4; }
    };

    const variant<A, B> ca = A(), cb = B();
    CHECK(ca.flat_map(F()) == 1);
    CHECK(cb.flat_map(F()) == 2);
    CHECK((variant<A, B>(A()).flat_map(F())) == 3);
    CHECK((variant<A, B>(B()).flat_map(F())) == 4);
}

TEST_CASE("Double variant map with uncallable function") {
    struct F { };
    using V = variant<int, double>;
    using MV = variant<move_only_int, move_only_double>;
    const V i = 1, d = 2.0;
    CHECK(i.map(F()) == V(1));
    CHECK(d.map(F()) == V(2.0));
    CHECK(MV(move_only_int(3)).map(F()) == MV(move_only_int(3)));
    CHECK(MV(move_only_double(4.0)).map(F()) == MV(move_only_double(4.0)));
}

TEST_CASE("Double variant map with function accepting one contained type") {
    struct X { };
    struct Y { };
    struct F {
        Y operator()(X) { return Y(); }
        int operator()(move_only_int i) { return i.value; }
    };
    using V = variant<X, Y>;
    using MV = variant<move_only_int, move_only_double>;
    using MV2 = variant<int, move_only_double>;
    const V x = X(), y = Y();
    CHECK(x.map(F()).tag() == type_tag<Y>());
    CHECK(y.map(F()).tag() == type_tag<Y>());
    CHECK(MV(move_only_int(1)).map(F()) == MV2(1));
    CHECK(MV(move_only_double(2.0)).map(F()) == MV2(move_only_double(2.0)));
}

class add1 {
public:
    template<typename T>
    T operator()(T t) const { return t + 1; }
};

TEST_CASE("Double variant map converting all values") {
    using V = variant<int, double>;
    CHECK(V(1).map(add1()) == V(2));
    CHECK(V(3.0).map(add1()) == V(4.0));
}

TEST_CASE("Double variant operator==") {
    variant<int, double> i1 = 42;
    variant<int, double> i2 = 123;
    variant<int, double> d1 = 42.0;
    variant<int, double> d2 = 123.0;

    CHECK(i1 == i1);
    CHECK(i2 == i2);
    CHECK(d1 == d1);
    CHECK(d2 == d2);

    CHECK_FALSE(i1 == i2);
    CHECK_FALSE(i1 == d1);
    CHECK_FALSE(i1 == d2);

    CHECK_FALSE(i2 == i1);
    CHECK_FALSE(i2 == d1);
    CHECK_FALSE(i2 == d2);

    CHECK_FALSE(d1 == i1);
    CHECK_FALSE(d1 == i2);
    CHECK_FALSE(d1 == d2);

    CHECK_FALSE(d2 == i1);
    CHECK_FALSE(d2 == i2);
    CHECK_FALSE(d2 == d1);
}

TEST_CASE("Double variant operator<") {
    variant<int, double> i1 = 42;
    variant<int, double> i2 = 123;
    variant<int, double> d1 = 42.0;
    variant<int, double> d2 = 123.0;

    CHECK_FALSE(i1 < i1);
    CHECK_FALSE(i2 < i2);
    CHECK_FALSE(d1 < d1);
    CHECK_FALSE(d2 < d2);

    CHECK(i1 < i2);
    CHECK(i1 < d1);
    CHECK(i1 < d2);

    CHECK_FALSE(i2 < i1);
    CHECK(i2 < d1);
    CHECK(i2 < d2);

    CHECK_FALSE(d1 < i1);
    CHECK_FALSE(d1 < i2);
    CHECK(d1 < d2);

    CHECK_FALSE(d2 < i1);
    CHECK_FALSE(d2 < i2);
    CHECK_FALSE(d2 < d1);
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
