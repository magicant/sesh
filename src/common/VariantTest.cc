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
#include "common/ContainerHelper.hh"
#include "common/FunctionalInitialize.hh"
#include "common/TypeTag.hh"
#include "common/Variant.hh"

namespace {

using sesh::common::FUNCTIONAL_INITIALIZE;
using sesh::common::TypeTag;
using sesh::common::Variant;
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
    MoveThrows(const MoveThrows &) = default;
    MoveThrows(MoveThrows &&) { throw Exception(); }
    MoveThrows &operator=(const MoveThrows &) = default;
    MoveThrows &operator=(MoveThrows &&) = default;
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
        sizeof(Variant<>) > 0,
        "Empty variant is a valid type, even if not constructible");

TEST_CASE("Single variant construction & destruction") {
    Variant<int>(TypeTag<int>());
    Variant<int>(TypeTag<int>(), 123);
    Variant<int>(123);
    Variant<int>(FUNCTIONAL_INITIALIZE, [] { return 123; });

    static_assert(
            noexcept(Variant<int>(TypeTag<int>())),
            "int is no-throw constructible & destructible");
    static_assert(
            Variant<int>::IS_NOTHROW_DESTRUCTIBLE,
            "int is no-throw destructible");

    std::vector<Action> actions;
    Variant<Stub>(TypeTag<Stub>(), actions);
    CHECK(actions.size() == 2);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::DESTRUCTION);

    CHECK_THROWS_AS(
            Variant<DefaultThrows>(TypeTag<DefaultThrows>()),
            Exception);
    CHECK_THROWS_AS(
            Variant<DefaultThrows>(
                    FUNCTIONAL_INITIALIZE,
                    []() -> DefaultThrows { throw Exception(); }),
            Exception);
}

//TEST_CASE("Single variant throwing destructor") {
    static_assert(
            !noexcept(Variant<DefaultMayThrow>(TypeTag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
//}

//TEST_CASE("Single variant throwing destructor") {
    static_assert(
            !noexcept(Variant<NonDestructible>(TypeTag<NonDestructible>())),
            "NonDestructible destructor may throw");
    static_assert(
            !Variant<NonDestructible>::IS_NOTHROW_DESTRUCTIBLE,
            "NonDestructible destructor may throw");
//}

TEST_CASE("Double variant construction & destruction") {
    struct A {
    };
    struct B {
        B(int, double) noexcept(false) { }
    };

    Variant<A, B>(TypeTag<A>());
    Variant<A, B>(TypeTag<B>(), 0, 0.0);
    Variant<A, B>{A()};
    Variant<A, B>{B(0, 0.0)};
    Variant<A, B>(FUNCTIONAL_INITIALIZE, [] { return A(); });
    Variant<A, B>(FUNCTIONAL_INITIALIZE, [] { return B(0, 0.0); });

    static_assert(
            noexcept(Variant<A, B>(TypeTag<A>())),
            "A is no-throw constructible & destructible");
    static_assert(
            !noexcept(Variant<A, B>(TypeTag<B>(), 0, 0.0)),
            "B is not no-throw constructible");
    static_assert(
            Variant<A, B>::IS_NOTHROW_DESTRUCTIBLE,
            "A and B are no-throw destructible");
}

//TEST_CASE("Double variant throwing constructor") {
    static_assert(
            !noexcept(Variant<DefaultMayThrow, int>(
                    TypeTag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
    static_assert(
            !noexcept(Variant<int, DefaultMayThrow>(
                    TypeTag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
    static_assert(
            !noexcept(Variant<DefaultMayThrow, DefaultMayThrow>(
                    TypeTag<DefaultMayThrow>())),
            "DefaultMayThrow constructor may throw");
//}

//TEST_CASE("Double variant throwing destructor") {
    static_assert(
            !noexcept(Variant<int, NonDestructible>(
                    TypeTag<NonDestructible>())),
            "NonDestructible destructor is throwing");
    static_assert(
            !Variant<int, NonDestructible>::IS_NOTHROW_DESTRUCTIBLE,
            "NonDestructible destructor is throwing");
    static_assert(
            !noexcept(Variant<NonDestructible, int>(
                    TypeTag<NonDestructible>())),
            "NonDestructible destructor is throwing");
    static_assert(
            !Variant<NonDestructible, int>::IS_NOTHROW_DESTRUCTIBLE,
            "NonDestructible destructor is throwing");
//}

TEST_CASE("Quad variant construction & destruction") {
    class A { };
    class B { };
    class C { };
    class D { };
    Variant<A, B, C, D>(TypeTag<A>());
    Variant<A, B, C, D>(TypeTag<B>());
    Variant<A, B, C, D>(TypeTag<C>());
    Variant<A, B, C, D>(TypeTag<D>());
    Variant<A, B, C, D>{A()};
    Variant<A, B, C, D>{B()};
    Variant<A, B, C, D>{C()};
    Variant<A, B, C, D>{D()};
    Variant<A, B, C, D>(FUNCTIONAL_INITIALIZE, [] { return A(); });
    Variant<A, B, C, D>(FUNCTIONAL_INITIALIZE, [] { return B(); });
    Variant<A, B, C, D>(FUNCTIONAL_INITIALIZE, [] { return C(); });
    Variant<A, B, C, D>(FUNCTIONAL_INITIALIZE, [] { return D(); });
}

TEST_CASE("Double variant value") {
    const int I1 = 123, I2 = 234;
    const float F1 = 456.0f, F2 = 567.0f;

    Variant<int, float> i(TypeTag<int>(), I1);
    Variant<int, float> f(FUNCTIONAL_INITIALIZE, [=] { return F1; });

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

    const Variant<int, float> i(TypeTag<int>(), I);
    const Variant<int, float> f(FUNCTIONAL_INITIALIZE, [=] { return F; });

    CHECK(i.value<int>() == I);
    CHECK(f.value<float>() == F);
}

TEST_CASE("Double variant r-value") {
    std::vector<Action> actions;
    {
        Variant<int, Stub> v(TypeTag<Stub>(), actions);
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
    Variant<int, double> vi = 3;
    std::string si = vi.apply(TemplateVisitor());
    CHECK(si == "3");

    Variant<int, double> vd = 1.5;
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

    auto vi = Variant<int, double>::create<int>();
    CHECK(vi.apply(Visitor()) == "int &");
    CHECK(std::move(vi).apply(Visitor()) == "int &&");

    const auto cvi = Variant<int, double>::create<int>();
    CHECK(cvi.apply(Visitor()) == "const int &");
    CHECK(std::move(cvi).apply(Visitor()) == "const int &");

    auto vd = Variant<int, double>::create<double>();
    CHECK(vd.apply(Visitor()) == "double &");
    CHECK(std::move(vd).apply(Visitor()) == "double &&");

    const auto cvd = Variant<int, double>::create<double>();
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

    auto vi = Variant<int, double>::create<int>();
    auto vd = Variant<int, double>::create<double>();

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
    Variant<int, double> v1(TypeTag<int>(), 7);
    Variant<int, double> v2(v1);
    CHECK(v1.tag() == v1.tag<int>());
    CHECK(v2.tag() == v2.tag<int>());
    CHECK(v1.value<int>() == 7);
    CHECK(v2.value<int>() == 7);

    static_assert(
            noexcept(Variant<int, double>::IS_NOTHROW_COPY_CONSTRUCTIBLE),
            "int and double are no-throw copy constructible");

    std::vector<Action> actions;
    {
        Variant<int, Stub> v3(TypeTag<Stub>(), actions);
        CHECK(actions.size() == 1);

        Variant<int, Stub> v4(v3);
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::COPY_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant throwing copy constructor") {
    Variant<int, CopyMayThrow> v1((TypeTag<CopyMayThrow>()));
    Variant<int, CopyMayThrow> v2(v1);

    static_assert(
            noexcept(Variant<int, CopyMayThrow>::
                    IS_NOTHROW_COPY_CONSTRUCTIBLE),
            "CopyMayThrow has a copy constructor that may throw");
}

TEST_CASE("Double variant deleted copy constructor") {
    Variant<int, NonCopyable> v1((TypeTag<NonCopyable>()));
    // Variant<int, NonCopyable> v2(v1);

    // std::is_copy_constructible (wrongly) returns true due to lazy template
    // specialization.
//    static_assert(
//            !std::is_copy_constructible<Variant<int, NonCopyable>>::value,
//            "NonCopyable has no copy constructor");
}

TEST_CASE("Double/quad variant copy construction to supertype") {
    const int I = 17;
    const double D = 2.5;

    const Variant<int, double> v2i(TypeTag<int>(), I);
    const Variant<int, double> v2d(TypeTag<double>(), D);

    const Variant<double, float, int, long> v4i(v2i);
    const Variant<double, float, int, long> v4d(v2d);

    REQUIRE(v4i.tag() == v4i.tag<int>());
    REQUIRE(v4d.tag() == v4d.tag<double>());
    CHECK(v4i.value<int>() == I);
    CHECK(v4d.value<double>() == D);
}

TEST_CASE("Double variant move construction") {
    Variant<int, double> v1(TypeTag<int>(), 7);
    Variant<int, double> v2(std::move(v1));
    REQUIRE(v1.tag() == v1.tag<int>());
    REQUIRE(v2.tag() == v2.tag<int>());
    CHECK(v2.value<int>() == 7);

    static_assert(
            noexcept(Variant<int, double>::IS_NOTHROW_MOVE_CONSTRUCTIBLE),
            "int and double are no-throw move constructible");

    std::vector<Action> actions;
    {
        Variant<int, Stub> v3(TypeTag<Stub>(), actions);
        CHECK(actions.size() == 1);

        Variant<int, Stub> v4(std::move(v3));
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant throwing move constructor") {
    Variant<int, MoveMayThrow> v1((TypeTag<MoveMayThrow>()));
    Variant<int, MoveMayThrow> v2(std::move(v1));

    static_assert(
            noexcept(Variant<int, MoveMayThrow>::
                    IS_NOTHROW_MOVE_CONSTRUCTIBLE),
            "MoveMayThrow has a move constructor that may throw");
}

TEST_CASE("Double variant deleted move constructor") {
    Variant<int, NonMovable> v1((TypeTag<NonMovable>()));
    // Variant<int, NonMovable> v2(std::move(v1));

    // std::is_move_constructible (wrongly) returns true due to lazy template
    // specialization.
//    static_assert(
//            !std::is_move_constructible<Variant<int, NonMovable>>::value,
//            "NonMovable has no move constructor");
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

    using V2 = Variant<MoveOnlyInt, MoveOnlyDouble>;
    using V4 = Variant<float, MoveOnlyDouble, int, MoveOnlyInt>;

    V2 v2i(TypeTag<MoveOnlyInt>(), I);
    V2 v2d(TypeTag<MoveOnlyDouble>(), D);

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
        Variant<int, Stub> v(TypeTag<int>(), 1);
        CHECK(v.value<int>() == 1);

        CHECK_NOTHROW(v.emplace<Stub>(actions));
        CHECK(v.tag() == v.tag<Stub>());
        CHECK(actions.size() == 1);

        CHECK_NOTHROW(v.emplace<int>(2));
        REQUIRE(v.tag() == v.tag<int>());
        CHECK(v.value<int>() == 2);
        CHECK(actions.size() == 2);
    }
    CHECK(actions.size() == 2);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::DESTRUCTION);
}

TEST_CASE("Double variant emplacement with fallback") {
    Variant<int, MoveThrows> v(TypeTag<int>(), 1);
    CHECK_NOTHROW((v.emplaceWithFallback<MoveThrows, int>()));
    CHECK(v.tag() == v.tag<MoveThrows>());
    CHECK_NOTHROW((v.emplaceWithFallback<int, int>(2)));
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 2);
    CHECK_THROWS_AS(
            (v.emplaceWithFallback<MoveThrows, int>(MoveThrows())),
            Exception);
    REQUIRE(v.tag() == v.tag<int>());
    CHECK(v.value<int>() == 0);
}

TEST_CASE("Double variant reset") {
    std::vector<Action> actions;
    {
        Variant<int, Stub> v(TypeTag<int>(), 1);
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
        Variant<int, Stub> v(TypeTag<Stub>(), actions1);
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
        Variant<int, Stub> v(TypeTag<int>(), 1);

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
        Variant<int, Stub> v1(TypeTag<Stub>(), actions1);
        const Variant<int, Stub> v2(TypeTag<Stub>(), actions2);

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
        Variant<int, Stub> v1(TypeTag<int>(), 1);
        const Variant<int, Stub> v2(TypeTag<Stub>(), actions);
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
        Variant<int, ThrowingStub> v1(TypeTag<ThrowingStub>(), actions1);
        const Variant<int, ThrowingStub> v2(TypeTag<ThrowingStub>(), actions2);

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

// TEST_CASE(
//         "Double variant copy assignment with different tag & exception") {
    static_assert(
            std::is_copy_assignable<Variant<int, Stub>>::value,
            "int and Stub are copy-assignable");
    static_assert(
            Variant<int, double>::IS_NOTHROW_COPY_ASSIGNABLE,
            "int and double are no-throw copy-assignable");
// }

TEST_CASE("Double variant no copy assignment") {
    using V = Variant<Stub, NonCopyAssignable>;

    V v((TypeTag<NonCopyAssignable>()));
    // v = v;

    // std::is_copy_assignable (wrongly) returns true due to lazy template
    // specialization.
//    static_assert(
//            !std::is_copy_assignable<V>::value,
//            "NonCopyAssignable is not copy-assignable");
}

TEST_CASE("Double/quad variant copy assignment to supertype") {
    const int I = 17;
    const double D = 2.5;

    const Variant<int, double> v2i(TypeTag<int>(), I);
    const Variant<int, double> v2d(TypeTag<double>(), D);

    Variant<double, float, int, long> v4fi(TypeTag<float>(), 1.23f);
    Variant<double, float, int, long> v4ld(TypeTag<long>(), 92L);

    v4fi = v2i;
    v4ld = v2d;

    REQUIRE(v4fi.tag() == v4fi.tag<int>());
    REQUIRE(v4ld.tag() == v4ld.tag<double>());
    CHECK(v4fi.value<int>() == I);
    CHECK(v4ld.value<double>() == D);

    std::vector<Action> actions;
    {
        Variant<int, Stub> v2s(TypeTag<int>(), I);
        Variant<Stub> v1s(TypeTag<Stub>(), actions);
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
        Variant<int, Stub> v1(TypeTag<Stub>(), actions1);
        Variant<int, Stub> v2(TypeTag<Stub>(), actions2);

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
        Variant<int, Stub> v1(TypeTag<int>(), 1);
        Variant<int, Stub> v2(TypeTag<Stub>(), actions);
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
        Variant<int, ThrowingStub> v1(TypeTag<ThrowingStub>(), actions1);
        Variant<int, ThrowingStub> v2(TypeTag<ThrowingStub>(), actions2);

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

//TEST_CASE(
//      "Double variant move assignment with different tag & exception") {
    static_assert(
            std::is_move_assignable<Variant<int, Stub>>::value,
            "int and Stub are move-assignable");
    static_assert(
            Variant<int, double>::IS_NOTHROW_MOVE_ASSIGNABLE,
            "int and double are no-throw move-assignable");
//}

TEST_CASE("Double variant no move assignment") {
    using V = Variant<Stub, NonMoveAssignable>;

    V v((TypeTag<NonMoveAssignable>()));
//    v = std::move(v);

    // std::is_move_assignable (wrongly) returns true due to lazy template
    // specialization.
//    static_assert(
//            !std::is_move_assignable<V>::value,
//            "NonMoveAssignable is not move-assignable");

//    struct ThrowingMoveOnly : NonCopyable, MoveThrows { };
//    {
//        Variant<Stub, ThrowingMoveOnly> v1(TypeTag<Stub>(), actions);
//        Variant<Stub, ThrowingMoveOnly> v2((TypeTag<ThrowingMoveOnly>()));
//        CHECK_THROWS_AS(v1 = std::move(v2), Exception);
//    }

    // std::is_move_assignable (wrongly) returns true due to lazy template
    // specialization.
//    static_assert(
//            !std::is_move_assignable<Variant<Stub, ThrowingMoveOnly>>::value,
//            "ThrowingMoveOnly is not move-assignable");
}

TEST_CASE("Double/quad variant move assignment to supertype") {
    const int I = 17;
    const double D = 2.5;

    Variant<double, float, int, long> v4fi(TypeTag<float>(), 1.23f);
    Variant<double, float, int, long> v4ld(TypeTag<long>(), 92L);

    v4fi = Variant<int, double>(TypeTag<int>(), I);
    v4ld = Variant<int, double>(TypeTag<double>(), D);

    REQUIRE(v4fi.tag() == v4fi.tag<int>());
    REQUIRE(v4ld.tag() == v4ld.tag<double>());
    CHECK(v4fi.value<int>() == I);
    CHECK(v4ld.value<double>() == D);

    std::vector<Action> actions;
    {
        Variant<int, Stub> v2s(TypeTag<int>(), I);
        v2s = Variant<Stub>(TypeTag<Stub>(), actions);
        CHECK(v2s.tag() == v2s.tag<Stub>());
    }
    CHECK(actions.size() == 4);
    CHECK(actions.at(0) == Action::STANDARD_CONSTRUCTION);
    CHECK(actions.at(1) == Action::MOVE_CONSTRUCTION);
    CHECK(actions.at(2) == Action::DESTRUCTION);
    CHECK(actions.at(3) == Action::DESTRUCTION);
}

TEST_CASE("Double variant create") {
    Variant<int, double>::create<int>(4);
    Variant<int, double>::create<double>(3.14);

    Variant<int, double> v1(Variant<int, double>::create<int>(2));
    Variant<int, double> v2(Variant<int, double>::create<double>(5.5));

    using V = Variant<std::string, std::wstring, std::vector<char>>;
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

    Variant<int *, const char *> v1 = intArray;
    Variant<int *, const char *> v2 = "char array";

    REQUIRE(v1.tag() == v1.tag<int *>());
    CHECK(v1.value<int *>() == intArray);
    REQUIRE(v2.tag() == v2.tag<const char *>());
    CHECK(v2.value<const char *>()[0] == 'c');

    std::vector<Action> actions;
    {
        Variant<int, Stub> v3 = Stub(actions);
        CHECK(v3.tag() == v3.tag<Stub>());
    }
    CHECK_FALSE(contains(actions, Action::COPY_CONSTRUCTION));
    actions.clear();

    {
        Stub s(actions);
        Variant<int, Stub> v4 = s;
        CHECK(v4.tag() == v4.tag<Stub>());
    }
    CHECK(contains(actions, Action::COPY_CONSTRUCTION));
}

TEST_CASE("Double variant result of") {
    int i = 42;
    double d = 123.0;
    Variant<int, double> vi =
            Variant<int, double>::resultOf([&i] { return i; });
    Variant<int, double> vd =
            Variant<int, double>::resultOf([&d] { return d; });
    REQUIRE(vi.tag() == vi.tag<int>());
    CHECK(vi.value<int>() == 42);
    REQUIRE(vd.tag() == vd.tag<double>());
    CHECK(vd.value<double>() == 123.0);
}

TEST_CASE("Double variant swapping with same type") {
    using std::swap; // test against the swappable concept

    Variant<int, double> v1 = 7;
    Variant<int, double> v2 = 13;
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

    Variant<int, double> v1 = 5;
    Variant<int, double> v2 = 13.5;

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
//    Variant<int, MoveThrows> v1(TypeTag<int>(), 3);
//    Variant<int, MoveThrows> v2((TypeTag<MoveThrows>()));
//    swap(v1, v2);
//}

//TEST_CASE("Double variant throwing swapper") {
    static_assert(
            Variant<int, double>::IS_NOTHROW_SWAPPABLE,
            "int swap never throws");
    static_assert(
            !Variant<int, CopyMayThrow>::IS_NOTHROW_SWAPPABLE,
            "CopyMayThrow swap may throw");
//}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
