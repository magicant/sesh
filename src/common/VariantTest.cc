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
#include <type_traits>
#include <utility>
#include <vector>
#include "common/container_helper.hh"
#include "common/functional_initialize.hh"
#include "common/type_tag.hh"
#include "common/variant.hh"

namespace {

using sesh::common::contains;
using sesh::common::functional_initialize;
using sesh::common::type_tag;
using sesh::common::variant;

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
struct DefaultMayThrow {
    DefaultMayThrow() noexcept(false) { }
};
struct DefaultThrows {
    DefaultThrows() { throw Exception(); }
    ~DefaultThrows() noexcept(false) {
        FAIL("DefaultThrows destructor must not be called");
    }
};
struct CopyMayThrow {
    CopyMayThrow() = default;
    CopyMayThrow(const CopyMayThrow &) noexcept(false) { }
};
struct CopyThrows {
    CopyThrows() = default;
    CopyThrows(const CopyThrows &) { throw Exception(); }
    CopyThrows(CopyThrows &&) = default;
    CopyThrows &operator=(const CopyThrows &) = default;
};
struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable &) = delete;
};
struct MoveMayThrow {
    MoveMayThrow() = default;
    MoveMayThrow(MoveMayThrow &&) noexcept(false) { }
};
struct MoveThrows {
    MoveThrows() = default;
    MoveThrows(MoveThrows &&) { throw Exception(); }
    MoveThrows &operator=(const MoveThrows &) = default;
};
struct NonMovable {
    NonMovable() = default;
    NonMovable(const NonMovable &) = delete;
    NonMovable(NonMovable &&) = delete;
};
struct NonCopyAssignable {
    NonCopyAssignable &operator=(const NonCopyAssignable &) = delete;
};
struct NonMoveAssignable {
    NonMoveAssignable() = default;
    NonMoveAssignable(NonMoveAssignable &&) = default;
    NonMoveAssignable &operator=(NonMoveAssignable &&) = delete;
};
struct NonDestructible {
    ~NonDestructible() noexcept(false) { }
};

static_assert(
        sizeof(variant<>) > 0,
        "Empty variant is a valid type, even if not constructible");

TEST_CASE("Single variant construction & destruction") {
    variant<int>(type_tag<int>());
    variant<int>(type_tag<int>(), 123);
    variant<int>(123);
    variant<int>(functional_initialize(), [] { return 123; });

    static_assert(
            noexcept(variant<int>(type_tag<int>())),
            "int is no-throw constructible & destructible");
    static_assert(
            variant<int>::is_nothrow_destructible,
            "int is no-throw destructible");

    auto throwing = []() noexcept(false) { return 0; };
    auto nonthrowing = []() noexcept(true) { return 0; };
    static_assert(
            !noexcept(variant<int>(functional_initialize(), throwing)),
            "functional initialize propagates noexcept(false)");
    static_assert(
            noexcept(variant<int>(functional_initialize(), nonthrowing)),
            "functional initialize propagates noexcept(true)");

    std::vector<Action> actions;
    variant<Stub>(type_tag<Stub>(), actions);
    CHECK(actions.size() == 2);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::DESTRUCTION);

    CHECK_THROWS_AS(
            variant<DefaultThrows>(type_tag<DefaultThrows>()),
            Exception);
    CHECK_THROWS_AS(
            variant<DefaultThrows>(
                    functional_initialize(),
                    []() -> DefaultThrows { throw Exception(); }),
            Exception);
}

//TEST_CASE("Single variant throwing destructor") {
    static_assert(
            !noexcept(variant<DefaultMayThrow>(type_tag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
//}

//TEST_CASE("Single variant throwing destructor") {
    static_assert(
            !noexcept(variant<NonDestructible>(type_tag<NonDestructible>())),
            "NonDestructible destructor may throw");
    static_assert(
            !variant<NonDestructible>::is_nothrow_destructible,
            "NonDestructible destructor may throw");
//}

TEST_CASE("Double variant construction & destruction") {
    struct A {
    };
    struct B {
        B(int, double) noexcept(false) { }
    };

    variant<A, B>(type_tag<A>());
    variant<A, B>(type_tag<B>(), 0, 0.0);
    variant<A, B>{A()};
    variant<A, B>{B(0, 0.0)};
    variant<A, B>(functional_initialize(), [] { return A(); });
    variant<A, B>(functional_initialize(), [] { return B(0, 0.0); });

    static_assert(
            noexcept(variant<A, B>(type_tag<A>())),
            "A is no-throw constructible & destructible");
    static_assert(
            !noexcept(variant<A, B>(type_tag<B>(), 0, 0.0)),
            "B is not no-throw constructible");
    static_assert(
            variant<A, B>::is_nothrow_destructible,
            "A and B are no-throw destructible");

    auto throwing = []() noexcept(false) { return 0; };
    auto nonthrowing = []() noexcept(true) { return 0.0; };
    static_assert(
            !noexcept(variant<int, double>(functional_initialize(), throwing)),
            "functional initialize propagates noexcept(false)");
    static_assert(
            noexcept(
                variant<int, double>(functional_initialize(), nonthrowing)),
            "functional initialize propagates noexcept(true)");
}

//TEST_CASE("Double variant throwing constructor") {
    static_assert(
            !noexcept(variant<DefaultMayThrow, int>(
                    type_tag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
    static_assert(
            !noexcept(variant<int, DefaultMayThrow>(
                    type_tag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
    static_assert(
            !noexcept(variant<DefaultMayThrow, DefaultMayThrow>(
                    type_tag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
//}

//TEST_CASE("Double variant throwing destructor") {
    static_assert(
            !noexcept(variant<int, NonDestructible>(
                    type_tag<NonDestructible>())),
            "NonDestructible destructor is throwing");
    static_assert(
            !variant<int, NonDestructible>::is_nothrow_destructible,
            "NonDestructible destructor is throwing");
    static_assert(
            !noexcept(variant<NonDestructible, int>(
                    type_tag<NonDestructible>())),
            "NonDestructible destructor is throwing");
    static_assert(
            !variant<NonDestructible, int>::is_nothrow_destructible,
            "NonDestructible destructor is throwing");
//}

TEST_CASE("Quad variant construction & destruction") {
    class A { };
    class B { };
    class C { };
    class D { };
    variant<A, B, C, D>(type_tag<A>());
    variant<A, B, C, D>(type_tag<B>());
    variant<A, B, C, D>(type_tag<C>());
    variant<A, B, C, D>(type_tag<D>());
    variant<A, B, C, D>{A()};
    variant<A, B, C, D>{B()};
    variant<A, B, C, D>{C()};
    variant<A, B, C, D>{D()};
    variant<A, B, C, D>(functional_initialize(), [] { return A(); });
    variant<A, B, C, D>(functional_initialize(), [] { return B(); });
    variant<A, B, C, D>(functional_initialize(), [] { return C(); });
    variant<A, B, C, D>(functional_initialize(), [] { return D(); });
}

TEST_CASE("Double variant copy initialization") {
    variant<int, type_tag<int>> vi = 0;
    CHECK(vi.tag() == vi.tag<int>());

    // Implicit conversion from type_tag is disabled to disambiguate overloads.
//    variant<int, type_tag<int>> vt = type_tag<int>();
//    CHECK(vt.tag() == vt.tag<type_tag<int>>());
}

TEST_CASE("Double variant direct initialization") {
    variant<int, type_tag<int>> vi((type_tag<int>()));
    CHECK(vi.tag() == vi.tag<int>());

    variant<int, type_tag<int>> vt((type_tag<type_tag<int>>()));
    CHECK(vt.tag() == vt.tag<type_tag<int>>());
}

TEST_CASE("Double variant value") {
    const int I1 = 123, I2 = 234;
    const float F1 = 456.0f, F2 = 567.0f;

    variant<int, float> i(type_tag<int>(), I1);
    variant<int, float> f(functional_initialize(), [=] { return F1; });

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

    const variant<int, float> i(type_tag<int>(), I);
    const variant<int, float> f(functional_initialize(), [=] { return F; });

    CHECK(i.value<int>() == I);
    CHECK(f.value<float>() == F);
}

TEST_CASE("Double variant r-value") {
    std::vector<Action> actions;
    {
        variant<int, Stub> v(type_tag<Stub>(), actions);
        CHECK(actions.size() == 1);

        Stub stubCopy(v.value<Stub>());
        CHECK(actions.size() == 2);

        Stub stubMove(std::move(v).value<Stub>());
        CHECK(actions.size() == 3);
    }
    CHECK(actions.size() == 6);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::COPY_CONSTRUCTION);
    CHECK(actions.at(2) == Action::MOVE_CONSTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
    CHECK(actions.at(4) == Action::DESTRUCTION);
    CHECK(actions.at(5) == Action::DESTRUCTION);
}

struct TemplateVisitor {
    template<typename T>
    std::string operator()(T v) { return std::to_string(v); }
    // A local class cannot have a template member.
};

TEST_CASE("Double variant apply with templated visitor") {
    variant<int, double> vi = 3;
    std::string si = vi.apply(TemplateVisitor());
    CHECK(si == "3");

    variant<int, double> vd = 1.5;
    std::string sd = vd.apply(TemplateVisitor());
    CHECK(sd == "1.500000");
}

TEST_CASE("Double variant apply, contained value l/r-value") {
    struct Visitor {
        std::string operator()(int &) { return "int &"; }
        std::string operator()(const int &) { return "const int &"; }
        std::string operator()(int &&) { return "int &&"; }
        std::string operator()(double &) { return "double &"; }
        std::string operator()(const double &) { return "const double &"; }
        std::string operator()(double &&) { return "double &&"; }
    };

    auto vi = variant<int, double>::create<int>();
    CHECK(vi.apply(Visitor()) == "int &");
    CHECK(std::move(vi).apply(Visitor()) == "int &&");

    const auto cvi = variant<int, double>::create<int>();
    CHECK(cvi.apply(Visitor()) == "const int &");
    CHECK(std::move(cvi).apply(Visitor()) == "const int &");

    auto vd = variant<int, double>::create<double>();
    CHECK(vd.apply(Visitor()) == "double &");
    CHECK(std::move(vd).apply(Visitor()) == "double &&");

    const auto cvd = variant<int, double>::create<double>();
    CHECK(cvd.apply(Visitor()) == "const double &");
    CHECK(std::move(cvd).apply(Visitor()) == "const double &");
}

TEST_CASE("Double variant apply, visitor l/r-value") {
    struct Visitor {
        Visitor() { }
        std::string operator()(int) & { return "int, &"; }
        std::string operator()(int) const & { return "int, const &"; }
        std::string operator()(int) && { return "int, &&"; }
        std::string operator()(double) & { return "double, &"; }
        std::string operator()(double) const & { return "double, const &"; }
        std::string operator()(double) && { return "double, &&"; }
    };

    auto vi = variant<int, double>::create<int>();
    auto vd = variant<int, double>::create<double>();

    Visitor visitor;
    CHECK(vi.apply(visitor) == "int, &");
    CHECK(vd.apply(visitor) == "double, &");

    const Visitor constVisitor;
    CHECK(vi.apply(constVisitor) == "int, const &");
    CHECK(vd.apply(constVisitor) == "double, const &");

    CHECK(vi.apply(Visitor()) == "int, &&");
    CHECK(vd.apply(Visitor()) == "double, &&");
}

TEST_CASE("Double variant copy construction") {
    variant<int, double> v1(type_tag<int>(), 7);
    variant<int, double> v2(v1);
    CHECK(v1.tag() == v1.tag<int>());
    CHECK(v2.tag() == v2.tag<int>());
    CHECK(v1.value<int>() == 7);
    CHECK(v2.value<int>() == 7);

    static_assert(
            noexcept(variant<int, double>::is_nothrow_copy_constructible),
            "int and double are no-throw copy constructible");

    std::vector<Action> actions;
    {
        variant<int, Stub> v3(type_tag<Stub>(), actions);
        CHECK(actions.size() == 1);

        variant<int, Stub> v4(v3);
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::COPY_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant throwing copy constructor") {
    variant<int, CopyMayThrow> v1((type_tag<CopyMayThrow>()));
    variant<int, CopyMayThrow> v2(v1);

    static_assert(
            noexcept(variant<int, CopyMayThrow>::
                    is_nothrow_copy_constructible),
            "CopyMayThrow has a copy constructor that may throw");
}

TEST_CASE("Double variant deleted copy constructor") {
    variant<int, NonCopyable> v1((type_tag<NonCopyable>()));
    // variant<int, NonCopyable> v2(v1);

    static_assert(
            !std::is_copy_constructible<variant<int, NonCopyable>>::value,
            "NonCopyable has no copy constructor");
}

TEST_CASE("Double/quad variant copy construction to supertype") {
    const int I = 17;
    const double D = 2.5;

    const variant<int, double> v2i(type_tag<int>(), I);
    const variant<int, double> v2d(type_tag<double>(), D);

    const variant<double, float, int, long> v4i(v2i);
    const variant<double, float, int, long> v4d(v2d);

    REQUIRE(v4i.tag() == v4i.tag<int>());
    REQUIRE(v4d.tag() == v4d.tag<double>());
    CHECK(v4i.value<int>() == I);
    CHECK(v4d.value<double>() == D);
}

TEST_CASE("Double variant move construction") {
    variant<int, double> v1(type_tag<int>(), 7);
    variant<int, double> v2(std::move(v1));
    REQUIRE(v1.tag() == v1.tag<int>());
    REQUIRE(v2.tag() == v2.tag<int>());
    CHECK(v2.value<int>() == 7);

    static_assert(
            noexcept(variant<int, double>::is_nothrow_move_constructible),
            "int and double are no-throw move constructible");

    std::vector<Action> actions;
    {
        variant<int, Stub> v3(type_tag<Stub>(), actions);
        CHECK(actions.size() == 1);

        variant<int, Stub> v4(std::move(v3));
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant throwing move constructor") {
    variant<int, MoveMayThrow> v1((type_tag<MoveMayThrow>()));
    variant<int, MoveMayThrow> v2(std::move(v1));

    static_assert(
            noexcept(variant<int, MoveMayThrow>::
                    is_nothrow_move_constructible),
            "MoveMayThrow has a move constructor that may throw");
}

TEST_CASE("Double variant deleted move constructor") {
    variant<int, NonMovable> v1((type_tag<NonMovable>()));
    // variant<int, NonMovable> v2(std::move(v1));

    static_assert(
            !std::is_move_constructible<variant<int, NonMovable>>::value,
            "NonMovable has no move constructor");
}

TEST_CASE("Double/quad variant move construction to supertype") {
    struct MoveOnlyInt {
        int mValue;
        MoveOnlyInt(int i) noexcept : mValue(i) { }
        MoveOnlyInt(MoveOnlyInt &&) = default;
    };
    struct MoveOnlyDouble {
        double mValue;
        MoveOnlyDouble(double d) noexcept : mValue(d) { }
        MoveOnlyDouble(MoveOnlyDouble &&) = default;
    };
    const int I = 3;
    const double D = -7.0;

    using V2 = variant<MoveOnlyInt, MoveOnlyDouble>;
    using V4 = variant<float, MoveOnlyDouble, int, MoveOnlyInt>;

    V2 v2i(type_tag<MoveOnlyInt>(), I);
    V2 v2d(type_tag<MoveOnlyDouble>(), D);

    V4 v4i(std::move(v2i));
    V4 v4d(std::move(v2d));

    REQUIRE(v4i.tag() == v4i.tag<MoveOnlyInt>());
    REQUIRE(v4d.tag() == v4d.tag<MoveOnlyDouble>());
    CHECK(v4i.value<MoveOnlyInt>().mValue == I);
    CHECK(v4d.value<MoveOnlyDouble>().mValue == D);
}

TEST_CASE("Double variant emplacement") {
    std::vector<Action> actions;
    {
        variant<int, Stub> v(type_tag<int>(), 1);
        CHECK(v.value<int>() == 1);

        CHECK_NOTHROW(v.emplace(type_tag<Stub>(), actions));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions.size() == 1);

        CHECK_NOTHROW(v.emplace(2));
        REQUIRE(v.tag() == v.tag<int>());
        CHECK(v.value<int>() == 2);
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 2);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Double variant emplacement with fallback") {
    variant<int, MoveThrows> v(type_tag<int>(), 1);
    CHECK_NOTHROW(v.emplace_with_fallback<int>(type_tag<MoveThrows>()));
    CHECK(v.tag() == v.tag<MoveThrows>());
    CHECK_NOTHROW(v.emplace_with_fallback<int>(2));
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 2);
    CHECK_THROWS_AS(v.emplace_with_fallback<int>(MoveThrows()), Exception);
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
    variant<int, DefaultMayThrow> v = 1;
    CHECK_NOTHROW(v.emplace_with_backup(type_tag<DefaultMayThrow>()));
    CHECK(v.tag() == v.tag<DefaultMayThrow>());
}

TEST_CASE("Double variant emplacement with backup; "
        "exception in new value construction") {
    variant<int, DefaultThrows> v = 1;
    CHECK_THROWS_AS(
            v.emplace_with_backup(type_tag<DefaultThrows>()), Exception);
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 1);
}

TEST_CASE("Double variant emplacement with backup; "
        "exception in backup construction") {
    using V = variant<MoveThrows, DefaultMayThrow>;
    V v((type_tag<MoveThrows>()));
    CHECK_THROWS_AS(
            v.emplace_with_backup(type_tag<DefaultMayThrow>()), Exception);
    CHECK(v.tag() == v.tag<MoveThrows>());
}

TEST_CASE("Double variant reset") {
    std::vector<Action> actions;
    {
        variant<int, Stub> v(type_tag<int>(), 1);
        CHECK(v.value<int>() == 1);

        CHECK_NOTHROW(v.reset(Stub(actions)));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions.size() == 3);

        CHECK_NOTHROW(v.reset(2));
        REQUIRE(v.tag() == v.tag<int>());
        CHECK(v.value<int>() == 2);
        CHECK(actions.size() == 4);

        Stub stub(actions);
        CHECK(actions.size() == 5);
        CHECK_NOTHROW(v.reset(stub));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions.size() == 6);
        CHECK_NOTHROW(v.reset(stub));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions.size() == 8);
    }
    CHECK(actions.size() == 10);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION); // of temporary
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION); // of variant value
    CHECK(actions.at(2) == Action::DESTRUCTION); // of temporary
    CHECK(actions.at(3) == Action::DESTRUCTION); // of variant value
    CHECK(actions.at(4) == Action::STANDARD_CONSTRUCTION); // of local
    CHECK(actions.at(5) == Action::COPY_CONSTRUCTION); // of variant value
    CHECK(actions.at(6) == Action::DESTRUCTION); // of variant value
    CHECK(actions.at(7) == Action::COPY_CONSTRUCTION); // of variant value
    CHECK(actions.at(8) == Action::DESTRUCTION); // of local
    CHECK(actions.at(9) == Action::DESTRUCTION); // of variant value
}

TEST_CASE("Double variant assignment with same type") {
    std::vector<Action> actions1, actions2;
    {
        variant<int, Stub> v(type_tag<Stub>(), actions1);
        Stub stub(actions2);
        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v.assign(stub));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions1.size() == 2);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v.assign(std::move(stub)));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions1.size() == 3);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 4);
    CHECK(actions1.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions1.at(1) == Action::COPY_ASSIGNMENT);
    CHECK(actions1.at(2) == Action::MOVE_ASSIGNMENT);
    CHECK(actions1.at(3) == Action::DESTRUCTION);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions2.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Double variant assignment with different types") {
    std::vector<Action> actions;
    {
        variant<int, Stub> v(type_tag<int>(), 1);

        CHECK_NOTHROW(v.assign(Stub(actions)));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions.size() == 3);

        CHECK_NOTHROW(v.assign(100));
        REQUIRE(v.tag() == v.tag<int>());
        CHECK(v.value<int>() == 100);
        CHECK(actions.size() == 4);

        Stub stub(actions);
        CHECK(actions.size() == 5);
        CHECK_NOTHROW(v.assign(stub));
        REQUIRE(v.tag() == v.tag<Stub>());
        CHECK(actions.size() == 6);
    }
    CHECK(actions.size() == 8);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION); // of temporary
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION); // of variant value
    CHECK(actions.at(2) == Action::DESTRUCTION); // of temporary
    CHECK(actions.at(3) == Action::DESTRUCTION); // of variant value
    CHECK(actions.at(4) == Action::STANDARD_CONSTRUCTION); // of local
    CHECK(actions.at(5) == Action::COPY_CONSTRUCTION); // of variant value
    CHECK(actions.at(6) == Action::DESTRUCTION); // of local
    CHECK(actions.at(7) == Action::DESTRUCTION); // of variant value
}

TEST_CASE("Double variant copy assignment with same tag") {
    std::vector<Action> actions1, actions2;
    {
        variant<int, Stub> v1(type_tag<Stub>(), actions1);
        const variant<int, Stub> v2(type_tag<Stub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v1 = v2);
        CHECK(v1.tag() == v1.tag<Stub>());
        CHECK(actions1.size() == 2);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 3);
    CHECK(actions1.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions1.at(1) == Action::COPY_ASSIGNMENT);
    CHECK(actions1.at(2) == Action::DESTRUCTION);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions2.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Double variant copy assignment with different tag") {
    std::vector<Action> actions;
    {
        variant<int, Stub> v1(type_tag<int>(), 1);
        const variant<int, Stub> v2(type_tag<Stub>(), actions);
        CHECK(actions.size() == 1);

        CHECK_NOTHROW(v1 = v2);
        CHECK(v1.tag() == v1.tag<Stub>());
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::COPY_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant copy assignment with same tag & exception") {
    struct ThrowingStub : public Stub {
        using Stub::Stub;
        ThrowingStub &operator=(const ThrowingStub &) { throw Exception(); }
    };

    std::vector<Action> actions1, actions2;
    {
        variant<int, ThrowingStub> v1(type_tag<ThrowingStub>(), actions1);
        const variant<int, ThrowingStub> v2(
                type_tag<ThrowingStub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_THROWS_AS(v1 = v2, Exception);
        CHECK(v1.tag() == v1.tag<ThrowingStub>());
        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 2);
    CHECK(actions1.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions1.at(1) == Action::DESTRUCTION);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions2.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Double variant copy assignment with different tag & exception") {
    static_assert(
            std::is_copy_assignable<variant<int, Stub>>::value,
            "int and Stub are copy-assignable");
    static_assert(
            std::is_nothrow_copy_assignable<variant<int, double>>::value,
            "int and double are no-throw copy-assignable");
    static_assert(
            !std::is_nothrow_copy_assignable<variant<CopyThrows>>::value,
            "MoveThrows is copy-assignable but throwing");

    std::vector<Action> actions;
    variant<Stub, CopyThrows> v1(type_tag<Stub>(), actions);
    variant<Stub, CopyThrows> v2((type_tag<CopyThrows>()));
    CHECK_THROWS_AS(v1 = v2, Exception);
    CHECK(v1.tag() == v1.tag<Stub>());
    CHECK(v2.tag() == v1.tag<CopyThrows>());
    CHECK(actions.size() == 5);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION); // of backup
    CHECK(actions.at(2) == Action::DESTRUCTION); // of v1
    CHECK(actions.at(3) == Action::MOVE_CONSTRUCTION); // of v1
    CHECK(actions.at(4) == Action::DESTRUCTION); // of backup
}

TEST_CASE("Double variant no copy assignment") {
    using V = variant<Stub, NonCopyAssignable>;

    V v((type_tag<NonCopyAssignable>()));
    // v = v;

    static_assert(
            !std::is_copy_assignable<V>::value,
            "NonCopyAssignable is not copy-assignable");
}

TEST_CASE("Double/quad variant copy assignment to supertype") {
    const int I = 17;
    const double D = 2.5;

    const variant<int, double> v2i(type_tag<int>(), I);
    const variant<int, double> v2d(type_tag<double>(), D);

    variant<double, float, int, long> v4fi(type_tag<float>(), 1.23f);
    variant<double, float, int, long> v4ld(type_tag<long>(), 92L);

    v4fi = v2i;
    v4ld = v2d;

    REQUIRE(v4fi.tag() == v4fi.tag<int>());
    REQUIRE(v4ld.tag() == v4ld.tag<double>());
    CHECK(v4fi.value<int>() == I);
    CHECK(v4ld.value<double>() == D);

    std::vector<Action> actions;
    {
        variant<int, Stub> v2s(type_tag<int>(), I);
        variant<Stub> v1s(type_tag<Stub>(), actions);
        v2s = v1s;
        CHECK(v2s.tag() == v2s.tag<Stub>());
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::COPY_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant move assignment with same tag") {
    std::vector<Action> actions1, actions2;
    {
        variant<int, Stub> v1(type_tag<Stub>(), actions1);
        variant<int, Stub> v2(type_tag<Stub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_NOTHROW(v1 = std::move(v2));
        CHECK(v1.tag() == v1.tag<Stub>());
        CHECK(actions1.size() == 2);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 3);
    CHECK(actions1.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions1.at(1) == Action::MOVE_ASSIGNMENT);
    CHECK(actions1.at(2) == Action::DESTRUCTION);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions2.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Double variant move assignment with different tag") {
    std::vector<Action> actions;
    {
        variant<int, Stub> v1(type_tag<int>(), 1);
        variant<int, Stub> v2(type_tag<Stub>(), actions);
        CHECK(actions.size() == 1);

        CHECK_NOTHROW(v1 = std::move(v2));
        CHECK(v1.tag() == v1.tag<Stub>());
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant move assignment with same tag & exception") {
    struct ThrowingStub : public Stub {
        using Stub::Stub;
        ThrowingStub(ThrowingStub &&) = default;
        ThrowingStub &operator=(ThrowingStub &&) { throw Exception(); }
    };

    std::vector<Action> actions1, actions2;
    {
        variant<int, ThrowingStub> v1(type_tag<ThrowingStub>(), actions1);
        variant<int, ThrowingStub> v2(type_tag<ThrowingStub>(), actions2);

        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);

        CHECK_THROWS_AS(v1 = std::move(v2), Exception);
        CHECK(v1.tag() == v1.tag<ThrowingStub>());
        CHECK(actions1.size() == 1);
        CHECK(actions2.size() == 1);
    }
    CHECK(actions1.size() == 2);
    CHECK(actions1.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions1.at(1) == Action::DESTRUCTION);
    CHECK(actions2.size() == 2);
    CHECK(actions2.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions2.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Double variant move assignment with different tag & exception") {
    static_assert(
            std::is_move_assignable<variant<int, Stub>>::value,
            "int and Stub are move-assignable");
    static_assert(
            std::is_nothrow_move_assignable<variant<int, double>>::value,
            "int and double are no-throw move-assignable");
    static_assert(
            !std::is_nothrow_move_assignable<variant<MoveThrows>>::value,
            "MoveThrows is move-assignable but throwing");

    std::vector<Action> actions;
    variant<Stub, MoveThrows> v1(type_tag<Stub>(), actions);
    variant<Stub, MoveThrows> v2((type_tag<MoveThrows>()));
    CHECK_THROWS_AS(v1 = std::move(v2), Exception);
    CHECK(v1.tag() == v1.tag<Stub>());
    CHECK(v2.tag() == v1.tag<MoveThrows>());
    CHECK(actions.size() == 5);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION); // of backup
    CHECK(actions.at(2) == Action::DESTRUCTION); // of v1
    CHECK(actions.at(3) == Action::MOVE_CONSTRUCTION); // of v1
    CHECK(actions.at(4) == Action::DESTRUCTION); // of backup
}

TEST_CASE("Double variant no move assignment") {
    using V = variant<Stub, NonMoveAssignable>;

    V v((type_tag<NonMoveAssignable>()));
//    v = std::move(v);

    static_assert(
            !std::is_move_assignable<V>::value,
            "NonMoveAssignable is not move-assignable");
}

TEST_CASE("Double/quad variant move assignment to supertype") {
    const int I = 17;
    const double D = 2.5;

    variant<double, float, int, long> v4fi(type_tag<float>(), 1.23f);
    variant<double, float, int, long> v4ld(type_tag<long>(), 92L);

    v4fi = variant<int, double>(type_tag<int>(), I);
    v4ld = variant<int, double>(type_tag<double>(), D);

    REQUIRE(v4fi.tag() == v4fi.tag<int>());
    REQUIRE(v4ld.tag() == v4ld.tag<double>());
    CHECK(v4fi.value<int>() == I);
    CHECK(v4ld.value<double>() == D);

    std::vector<Action> actions;
    {
        variant<int, Stub> v2s(type_tag<int>(), I);
        v2s = variant<Stub>(type_tag<Stub>(), actions);
        CHECK(v2s.tag() == v2s.tag<Stub>());
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
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
    int intArray[] = {19};

    variant<int *, const char *> v1 = intArray;
    variant<int *, const char *> v2 = "char array";

    REQUIRE(v1.tag() == v1.tag<int *>());
    CHECK(v1.value<int *>() == intArray);
    REQUIRE(v2.tag() == v2.tag<const char *>());
    CHECK(v2.value<const char *>()[0] == 'c');

    std::vector<Action> actions;
    {
        variant<int, Stub> v3 = Stub(actions);
        CHECK(v3.tag() == v3.tag<Stub>());
    }
    CHECK_FALSE(contains(actions, Action::COPY_CONSTRUCTION));
    actions.clear();

    {
        Stub s(actions);
        variant<int, Stub> v4 = s;
        CHECK(v4.tag() == v4.tag<Stub>());
    }
    CHECK(contains(actions, Action::COPY_CONSTRUCTION));
}

TEST_CASE("Double variant result of") {
    int i = 42;
    double d = 123.0;
    variant<int, double> vi =
            variant<int, double>::result_of([&i] { return i; });
    variant<int, double> vd =
            variant<int, double>::result_of([&d] { return d; });
    REQUIRE(vi.tag() == vi.tag<int>());
    CHECK(vi.value<int>() == 42);
    REQUIRE(vd.tag() == vd.tag<double>());
    CHECK(vd.value<double>() == 123.0);
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

    v1.reset(32.0);
    v2.reset(8.5);
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
//    variant<int, MoveThrows> v1(type_tag<int>(), 3);
//    variant<int, MoveThrows> v2((type_tag<MoveThrows>()));
//    swap(v1, v2);
//}

//TEST_CASE("Double variant throwing swapper") {
    static_assert(
            variant<int, double>::is_nothrow_swappable,
            "int swap never throws");
    static_assert(
            !variant<int, CopyMayThrow>::is_nothrow_swappable,
            "CopyMayThrow swap may throw");
//}

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

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
