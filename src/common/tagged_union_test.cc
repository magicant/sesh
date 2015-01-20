/* Copyright (C) 2015 WATANABE Yuki
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

#include <type_traits>
#include <utility>
#include "catch.hpp"
#include "common/function_helper.hh"
#include "common/tagged_union.hh"
#include "common/type_tag.hh"

namespace {

using sesh::common::move_if_noexcept;
using sesh::common::tagged_union;
using sesh::common::tagged_union_value;
using sesh::common::type_tag;

constexpr int I = 3;
constexpr float F = 5.5f;

template<typename T>
using INDC = std::is_nothrow_default_constructible<T>;

template<typename T>
using IND = std::is_nothrow_destructible<T>;

TEST_CASE("Tagged union values have no-throw special member functions") {
    struct non_copyable {
        non_copyable() = delete;
        non_copyable(const non_copyable &) = delete;
        non_copyable(non_copyable &&) = delete;
        non_copyable &operator=(const non_copyable &) = delete;
        non_copyable &operator=(non_copyable &&) = delete;
        ~non_copyable() noexcept { }
    };

    CHECK(INDC<tagged_union_value<>>::value);
    CHECK(INDC<tagged_union_value<short>>::value);
    CHECK((INDC<tagged_union_value<short, int>>::value));
    CHECK((INDC<tagged_union_value<short, int, int>>::value));
    CHECK((INDC<tagged_union_value<short, int, int, long>>::value));
    CHECK(INDC<tagged_union_value<non_copyable>>::value);

    CHECK(INDC<tagged_union_value<>>::value);
    CHECK(INDC<tagged_union_value<short>>::value);
    CHECK((INDC<tagged_union_value<short, int>>::value));
    CHECK((INDC<tagged_union_value<short, int, int>>::value));
    CHECK((INDC<tagged_union_value<short, int, int, long>>::value));
    CHECK(INDC<tagged_union_value<non_copyable>>::value);
}

TEST_CASE("Accessing element of tagged union value") {
    using sesh::common::get;

    tagged_union_value<short> s;
    CHECK((std::is_same<decltype(get<short>(s)), short &>::value));

    tagged_union_value<short, int> si;
    CHECK((std::is_same<decltype(get<short>(si)), short &>::value));
    CHECK((std::is_same<decltype(get<int>(si)), int &>::value));

    tagged_union_value<short, int, int> sii;
    CHECK((std::is_same<decltype(get<short>(sii)), short &>::value));
    CHECK((std::is_same<decltype(get<int>(sii)), int &>::value));

    tagged_union_value<short, int, int, long> siil;
    CHECK((std::is_same<decltype(get<short>(siil)), short &>::value));
    CHECK((std::is_same<decltype(get<int>(siil)), int &>::value));
    CHECK((std::is_same<decltype(get<long>(siil)), long &>::value));
}

TEST_CASE("Getting element of constant tagged union value") {
    using sesh::common::get;

    const tagged_union_value<short> s;
    CHECK((std::is_same<decltype(get<short>(s)), const short &>::value));

    const tagged_union_value<short, int> si;
    CHECK((std::is_same<decltype(get<short>(si)), const short &>::value));
    CHECK((std::is_same<decltype(get<int>(si)), const int &>::value));

    const tagged_union_value<short, int, int> sii;
    CHECK((std::is_same<decltype(get<short>(sii)), const short &>::value));
    CHECK((std::is_same<decltype(get<int>(sii)), const int &>::value));

    const tagged_union_value<short, int, int, long> siil;
    CHECK((std::is_same<decltype(get<short>(siil)), const short &>::value));
    CHECK((std::is_same<decltype(get<int>(siil)), const int &>::value));
    CHECK((std::is_same<decltype(get<long>(siil)), const long &>::value));
}

TEST_CASE("Getting element of r-value tagged union value") {
    using sesh::common::get;

    using S = tagged_union_value<short>;
    CHECK((std::is_same<
                decltype(get<short>(std::declval<S>())), short &&>::value));

    using SI = tagged_union_value<short, int>;
    CHECK((std::is_same<
                decltype(get<short>(std::declval<SI>())), short &&>::value));
    CHECK((std::is_same<
                decltype(get<int>(std::declval<SI>())), int &&>::value));

    using SII = tagged_union_value<short, int, int>;
    CHECK((std::is_same<
                decltype(get<short>(std::declval<SII>())), short &&>::value));
    CHECK((std::is_same<
                decltype(get<int>(std::declval<SII>())), int &&>::value));

    using SIIL = tagged_union_value<short, int, int, long>;
    CHECK((std::is_same<
                decltype(get<short>(std::declval<SIIL>())), short &&>::value));
    CHECK((std::is_same<
                decltype(get<int>(std::declval<SIIL>())), int &&>::value));
    CHECK((std::is_same<
                decltype(get<long>(std::declval<SIIL>())), long &&>::value));
}

TEST_CASE("Empty tagged union is complete type") {
    CHECK(sizeof(tagged_union<>) > 0);
}

namespace constructor_test {

TEST_CASE("Constructing tagged union (direct initialization)") {
    class constructee {
    public:
        constructee(bool &c, int i, float f) noexcept {
            c = true;
            CHECK(i == I);
            CHECK(f == F);
        }
    };

    bool constructed;

    constructed = false;
    (void) tagged_union<constructee>(
            type_tag<constructee>(), constructed, I, F);
    CHECK(constructed);

    constructed = false;
    (void) tagged_union<int, constructee>(
            type_tag<constructee>(), constructed, I, F);
    CHECK(constructed);

    constructed = false;
    (void) tagged_union<constructee, bool>(
            type_tag<constructee>(), constructed, I, F);
    CHECK(constructed);
}

TEST_CASE("Construction of non-copyable tagged union element") {
    struct non_copyable {
        non_copyable() = default;
        non_copyable(const non_copyable &) = delete;
    };
    (void) tagged_union<non_copyable>(type_tag<non_copyable>());
}

TEST_CASE("Construction of non-movable tagged union element") {
    struct non_movable {
        non_movable() = default;
        non_movable(non_movable &&) = delete;
    };
    (void) tagged_union<non_movable>(type_tag<non_movable>());
}

TEST_CASE("Constructing tagged union (copy)") {
    const tagged_union<int, float> i0(type_tag<int>(), I);
    const tagged_union<int, float> f0(type_tag<float>(), F);
    auto i1 = i0;
    auto f1 = f0;
    CHECK(i1.tag() == type_tag<int>());
    CHECK(f1.tag() == type_tag<float>());
    CHECK(i1.template value<int>() == I);
    CHECK(f1.template value<float>() == F);

    struct non_trivial_copy {
        non_trivial_copy() = default;
        non_trivial_copy(const non_trivial_copy &) noexcept { }
    };

    const tagged_union<non_trivial_copy> c0((type_tag<non_trivial_copy>()));
    auto c1 = c0;
    tagged_union<int, non_trivial_copy, float> c2 = c1;
    (void) c2;
}

TEST_CASE("Constructing tagged union (move)") {
    struct non_trivial_move {
        non_trivial_move() = default;
        non_trivial_move(non_trivial_move &&) noexcept { }
    };

    tagged_union<non_trivial_move> m0((type_tag<non_trivial_move>()));
    auto m1 = std::move(m0);
    tagged_union<int, non_trivial_move, float> m2 = std::move(m1);
    (void) m2;
}

TEST_CASE("Constructing tagged union (move if noexcept)") {
    struct noexcept_movable {
        noexcept_movable() = default;
        noexcept_movable(const noexcept_movable &) { FAIL("Copied"); }
        noexcept_movable(noexcept_movable &&) noexcept { }
    };
    struct move_only {
        move_only() = default;
        move_only(const move_only &) = delete;
        move_only(move_only &&) { }
    };
    struct throwing_movable {
        throwing_movable() = default;
        throwing_movable(const throwing_movable &) { }
        throwing_movable(throwing_movable &&) { FAIL("Moved"); }
    };
    using TU = tagged_union<noexcept_movable, move_only, throwing_movable>;

    TU n((type_tag<noexcept_movable>()));
    TU m((type_tag<move_only>()));
    TU t((type_tag<throwing_movable>()));
    (void) TU(move_if_noexcept(), n);
    (void) TU(move_if_noexcept(), m);
    (void) TU(move_if_noexcept(), t);
}

} // namespace constructor_test

TEST_CASE("Destructing tagged union") {
    class destructee {
    private:
        bool &m_destructed;
    public:
        explicit destructee(bool &d) noexcept : m_destructed(d) { }
        ~destructee() { m_destructed = true; }
    };

    bool destructed;

    destructed = false;
    (void) tagged_union<destructee>(type_tag<destructee>(), destructed);
    CHECK(destructed);

    destructed = false;
    (void) tagged_union<int, destructee>(type_tag<destructee>(), destructed);
    CHECK(destructed);

    destructed = false;
    (void) tagged_union<destructee, bool>(type_tag<destructee>(), destructed);
    CHECK(destructed);
}

TEST_CASE("Checking element type of tagged union") {
    const tagged_union<int> i((type_tag<int>()));
    const tagged_union<int, float> if_i((type_tag<int>()));
    const tagged_union<int, float> if_f((type_tag<float>()));
    CHECK(i.tag() == type_tag<int>());
    CHECK(if_i.tag() == type_tag<int>());
    CHECK(if_f.tag() == type_tag<float>());
}

TEST_CASE("Accessing element of tagged union (non-const l-value)") {
    using sesh::common::get;

    tagged_union<int, float> i((type_tag<int>()), I);
    tagged_union<int, float> f((type_tag<float>()), F);
    CHECK((std::is_same<decltype(get<int>(i)), int &>::value));
    CHECK((std::is_same<decltype(get<float>(f)), float &>::value));
    CHECK(get<int>(i) == I);
    CHECK(get<float>(f) == F);
}

TEST_CASE("Accessing element of tagged union (const l-value)") {
    using sesh::common::get;

    const tagged_union<int, float> i((type_tag<int>()), I);
    const tagged_union<int, float> f((type_tag<float>()), F);
    CHECK((std::is_same<decltype(get<int>(i)), const int &>::value));
    CHECK((std::is_same<decltype(get<float>(f)), const float &>::value));
    CHECK(get<int>(i) == I);
    CHECK(get<float>(f) == F);
}

TEST_CASE("Accessing element of tagged union (r-value)") {
    using sesh::common::get;

    tagged_union<int, float> i((type_tag<int>()), I);
    tagged_union<int, float> f((type_tag<float>()), F);
    CHECK((std::is_same<decltype(get<int>(std::move(i))), int &&>::value));
    CHECK((std::is_same<decltype(get<float>(std::move(f))), float &&>::value));
    CHECK(get<int>(std::move(i)) == I);
    CHECK(get<float>(std::move(f)) == F);
}

TEST_CASE("Applying visitor to tagged union (non-const l-value)") {
    using sesh::common::get;

    tagged_union<int, float> i((type_tag<int>()));
    tagged_union<int, float> f((type_tag<float>()));

    class visitor_i {
    public:
        int &expected;
        bool operator()(int &actual) const {
            CHECK(&actual == &expected);
            return true;
        }
        bool operator()(float &) const {
            return false;
        }
    };
    class visitor_f {
    public:
        float &expected;
        bool operator()(int &) const {
            return false;
        }
        bool operator()(float &actual) const {
            CHECK(&actual == &expected);
            return true;
        }
    };

    CHECK(i.apply(visitor_i{get<int>(i)}));
    CHECK(f.apply(visitor_f{get<float>(f)}));
}

TEST_CASE("Applying visitor to tagged union (const l-value)") {
    using sesh::common::get;

    const tagged_union<int, float> i((type_tag<int>()));
    const tagged_union<int, float> f((type_tag<float>()));

    class visitor_i {
    public:
        const int &expected;
        bool operator()(const int &actual) const {
            CHECK(&actual == &expected);
            return true;
        }
        bool operator()(const float &) const {
            return false;
        }
    };
    class visitor_f {
    public:
        const float &expected;
        bool operator()(const int &) const {
            return false;
        }
        bool operator()(const float &actual) const {
            CHECK(&actual == &expected);
            return true;
        }
    };

    CHECK(i.apply(visitor_i{get<int>(i)}));
    CHECK(f.apply(visitor_f{get<float>(f)}));
}

TEST_CASE("Applying visitor to tagged union (r-value)") {
    using sesh::common::get;

    tagged_union<int, float> i((type_tag<int>()));
    tagged_union<int, float> f((type_tag<float>()));

    class visitor_i {
    public:
        int &expected;
        bool operator()(int &&actual) const {
            CHECK(&actual == &expected);
            return true;
        }
        bool operator()(float &&) const {
            return false;
        }
    };
    class visitor_f {
    public:
        float &expected;
        bool operator()(int &&) const {
            return false;
        }
        bool operator()(float &&actual) const {
            CHECK(&actual == &expected);
            return true;
        }
    };

    CHECK(std::move(i).apply(visitor_i{get<int>(i)}));
    CHECK(std::move(f).apply(visitor_f{get<float>(f)}));
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
