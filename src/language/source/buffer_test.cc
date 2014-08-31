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

#include <utility>
#include "language/source/buffer.hh"
#include "language/source/Source.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::language::source::Location;
using sesh::language::source::Source;
using sesh::language::source::buffer;
using sesh::language::source::checkSourceString;

using buffer_pointer = std::shared_ptr<buffer>;
using SBCI = sesh::language::source::buffer::const_iterator;
using source_pointer = sesh::language::source::Source::Pointer;
using string = sesh::language::source::buffer::string_type;

class string_prepended_source : public Source {
public:
    string_prepended_source(Pointer &&p, string &&s) :
            Source(std::move(p), 0, 0, std::move(s)) { }
    Location locationInAlternate(Size) const override {
        throw "unexpected";
    }
};

source_pointer prepend(source_pointer &&p, string &&s) {
    return source_pointer(
            new string_prepended_source(std::move(p), std::move(s)));
}

buffer_pointer create_buffer(string &&s) {
    buffer_pointer b = buffer::create();

    b->substitute([&s](source_pointer &&p) -> source_pointer {
        return prepend(std::move(p), std::move(s));
    });
    return b;
}

void check_buffer_string(const buffer &sb, const string &s) {
    for (buffer::size_type i = 0; i < s.length(); ++i) {
        CHECK(sb.at(i) == s.at(i));
        CHECK(sb[i] == s[i]);
    }
    CHECK_THROWS_AS(sb.at(s.length()), std::out_of_range);
    CHECK(sb[s.length()] == buffer::value_type());
}

} // namespace

TEST_CASE("Buffer, construction and creation") {
    CHECK_NOTHROW(buffer());
    CHECK_NOTHROW(buffer::create());
}

TEST_CASE("Buffer, length") {
    CHECK(buffer().length() == 0);
}

TEST_CASE("Buffer, substitution, at and operator[]") {
    buffer sb;

    check_buffer_string(sb, L(""));
    sb.substitute([](source_pointer &&p) -> source_pointer {
        CHECK(p == nullptr);
        return prepend(std::move(p), L("Test"));
    });
    check_buffer_string(sb, L("Test"));
    sb.substitute([](source_pointer &&p) -> source_pointer {
        return std::move(p);
    });
    check_buffer_string(sb, L("Test"));
    sb.substitute([](source_pointer &&p) -> source_pointer {
        REQUIRE(p != nullptr);
        CHECK(dynamic_cast<const string_prepended_source *>(p.get()) !=
                nullptr);
        checkSourceString(*p, L("Test"));
        return std::move(p);
    });
    check_buffer_string(sb, L("Test"));
}

TEST_CASE("Buffer, begin and end") {
    buffer_pointer b = buffer::create();
    CHECK_NOTHROW(b->cbegin());
    CHECK_NOTHROW(b->cend());
    CHECK_NOTHROW(b->begin());
    CHECK_NOTHROW(b->end());

    b->substitute([](source_pointer &&p) -> source_pointer {
        return prepend(std::move(p), L("Test"));
    });
    CHECK_NOTHROW(b->cbegin());
    CHECK_NOTHROW(b->cend());
    CHECK_NOTHROW(b->begin());
    CHECK_NOTHROW(b->end());
}

TEST_CASE("Buffer iterator, normal construction") {
    buffer_pointer b = create_buffer(L("X"));
    CHECK_NOTHROW(SBCI(b, 0));
    CHECK_NOTHROW(SBCI(b, 1));
}

TEST_CASE("Buffer iterator, special construction") {
    // default constructible
    SBCI i1;
    // copy constructible
    SBCI i2(i1);
    // move constructible
    SBCI i2move(std::move(i2));
}

TEST_CASE("Buffer iterator, assignment") {
    SBCI i1, i2;
    // copy assignable
    i1 = i2;
    // move assignable
    i1 = std::move(i2);
}

TEST_CASE("Buffer iterator, dereferencing") {
    buffer_pointer b = create_buffer(L("ABC"));
    CHECK(*b->cbegin() == L('A'));

    SBCI ci = b->cbegin(), i = b->begin();
    // ci == i
    CHECK(&*ci == &*i);
    ++ci, ++i;
    CHECK(&*ci == &*i);
    ++ci, ++i;
    CHECK(&*ci == &*i);
}

TEST_CASE("Buffer iterator, incrementation") {
    buffer_pointer b = create_buffer(L("Test"));
    SBCI i = b->cbegin();
    CHECK_NOTHROW(++i);
    CHECK(*i == L('e'));
    CHECK(*++i == L('s'));
    CHECK(*i++ == L('s'));
    CHECK(*i == L('t'));

    const SBCI &iref = i++;
    CHECK(*iref == L('t'));
}

TEST_CASE("Buffer iterator, decrementation") {
    buffer_pointer b = create_buffer(L("Test"));
    SBCI i1 = b->cend();
    CHECK(*--i1 == L('t'));

    SBCI i2 = i1;
    CHECK(--++i2 == i1);
    CHECK(&i2 == &--i2);

    const SBCI &i1ref = i1--;
    CHECK(*i1ref == L('t'));
    CHECK(*i1 == L('s'));
    CHECK(*i1-- == L('s'));
    CHECK(*i1 == L('e'));
}

TEST_CASE("Buffer iterator, equality") {
    buffer_pointer b = create_buffer(L("ABC"));
    SBCI ci = b->cbegin(), i = b->begin();
    SBCI cend = b->cend(), end = b->end();

    CHECK(ci == ci);
    CHECK(ci == i);
    CHECK(i == ci);
    CHECK(i == i);
    CHECK(cend == cend);
    CHECK(cend == end);
    CHECK(end == cend);
    CHECK(end == end);
    CHECK(ci != cend);
    CHECK(i != end);

    CHECK_FALSE(ci != ci);
    CHECK_FALSE(ci != i);
    CHECK_FALSE(i != ci);
    CHECK_FALSE(i != i);
    CHECK_FALSE(cend != cend);
    CHECK_FALSE(cend != end);
    CHECK_FALSE(end != cend);
    CHECK_FALSE(end != end);
    CHECK_FALSE(ci == cend);
    CHECK_FALSE(i == end);

    ++ci;
    CHECK(ci != i);
    CHECK(ci != cend);
    CHECK_FALSE(ci == i);
    CHECK_FALSE(ci == cend);

    ++i;
    CHECK(ci == i);
    CHECK(i != cend);
    CHECK_FALSE(ci != i);
    CHECK_FALSE(i == cend);

    ++i;
    ++i;
    CHECK(ci != i);
    CHECK(i == cend);
    CHECK(i == end);
    CHECK_FALSE(ci == i);
    CHECK_FALSE(i != cend);
    CHECK_FALSE(i != end);

    ++ci;
    ++ci;
    CHECK(ci == i);
    CHECK(ci == cend);
    CHECK(ci == end);
    CHECK_FALSE(ci != i);
    CHECK_FALSE(ci != cend);
    CHECK_FALSE(ci != end);

    CHECK(SBCI() == SBCI());
    CHECK_FALSE(SBCI() != SBCI());
}

TEST_CASE("Buffer iterator, multi-pass guarantee 1") {
    buffer_pointer b = create_buffer(L("ABC"));
    SBCI ci = b->cbegin(), i = b->begin();
    // ci == i
    CHECK(++ci == ++i);
    CHECK(++ci == ++i);
    CHECK(++ci == ++i);
}

TEST_CASE("Buffer iterator, multi-pass guarantee 2") {
    buffer_pointer b = create_buffer(L("ABC"));
    SBCI i1 = b->begin();
    SBCI i2 = i1;
    CHECK(((void) ++i2, *i1) == *i1);
}

TEST_CASE("Buffer iterator, random access") {
    buffer_pointer b = create_buffer(L("Test"));
    SBCI begin = b->cbegin();
    SBCI end = b->cend();
    SBCI i = begin;

    CHECK_NOTHROW(i += 1);
    CHECK(i == begin + 1);
    CHECK(i == 1 + begin);
    CHECK(i == end + -3);
    CHECK(i == -3 + end);
    CHECK(*i == L('e'));
    CHECK_NOTHROW(i += 2);
    CHECK(i == b->cbegin() + 3);
    CHECK(i == 3 + b->cbegin());
    CHECK(i == b->cend() + -1);
    CHECK(i == -1 + b->cend());
    CHECK(*i == L('t'));
    CHECK_NOTHROW(i += -3);
    CHECK(i == begin + 0);
    CHECK(i == 0 + begin);
    CHECK(i == end + -4);
    CHECK(i == -4 + end);
    CHECK(*i == L('T'));
    CHECK_NOTHROW(i += 4);
    CHECK(i == b->cbegin() + 4);
    CHECK(i == 4 + b->cbegin());
    CHECK(i == b->cend() + 0);
    CHECK(i == 0 + b->cend());
    CHECK_NOTHROW(i += -2);
    CHECK(i == begin + 2);
    CHECK(i == 2 + begin);
    CHECK(i == end + -2);
    CHECK(i == -2 + end);
    CHECK(*i == L('s'));

    CHECK_NOTHROW(i -= -2);
    CHECK(i == begin - -4);
    CHECK((i - begin == 4));
    CHECK((begin - i == -4));
    CHECK(i == end - 0);
    CHECK((i - end == 0));
    CHECK((end - i == 0));
    CHECK_NOTHROW(i -= 4);
    CHECK(i == b->cbegin() - 0);
    CHECK((i - b->cbegin() == 0));
    CHECK((b->cbegin() - i == 0));
    CHECK(i == b->cend() - 4);
    CHECK((i - b->cend() == -4));
    CHECK((b->cend() - i == 4));
    CHECK(*i == L('T'));
    CHECK_NOTHROW(i -= -3);
    CHECK(i == begin - -3);
    CHECK((i - begin == 3));
    CHECK((begin - i == -3));
    CHECK(i == end - 1);
    CHECK((i - end == -1));
    CHECK((end - i == 1));
    CHECK(*i == L('t'));
    CHECK_NOTHROW(i -= 2);
    CHECK(i == b->cbegin() - -1);
    CHECK((i - b->cbegin() == 1));
    CHECK((b->cbegin() - i == -1));
    CHECK(i == b->cend() - 3);
    CHECK((i - b->cend() == -3));
    CHECK((b->cend() - i == 3));
    CHECK(*i == L('e'));
    CHECK_NOTHROW(i -= 1);
    CHECK(i == begin - 0);
    CHECK((i - begin == 0));
    CHECK((begin - i == 0));
    CHECK(i == end - 4);
    CHECK((i - end == -4));
    CHECK((end - i == 4));
    CHECK(*i == L('T'));
}

TEST_CASE("Buffer iterator, inequality") {
    buffer_pointer b = create_buffer(L("ABC"));
    SBCI begin = b->cbegin();
    SBCI end = b->cend();
    SBCI i = begin + 1;

    CHECK_FALSE(i < i);
    CHECK(i <= i);
    CHECK_FALSE(i > i);
    CHECK(i >= i);

    CHECK(begin < end);
    CHECK(begin <= end);
    CHECK_FALSE(begin > end);
    CHECK_FALSE(begin >= end);

    CHECK(begin < i);
    CHECK(begin <= i);
    CHECK_FALSE(begin > i);
    CHECK_FALSE(begin >= i);

    CHECK_FALSE(i < begin);
    CHECK_FALSE(i <= begin);
    CHECK(i > begin);
    CHECK(i >= begin);

    CHECK(i < end);
    CHECK(i <= end);
    CHECK_FALSE(i > end);
    CHECK_FALSE(i >= end);

    CHECK_FALSE(end < begin);
    CHECK_FALSE(end <= begin);
    CHECK(end > begin);
    CHECK(end >= begin);

    CHECK_FALSE(end < i);
    CHECK_FALSE(end <= i);
    CHECK(end > i);
    CHECK(end >= i);
}

TEST_CASE("Buffer iterator, index") {
    buffer_pointer b = create_buffer(L("Test"));

    SBCI i = b->cbegin() + 1;
    CHECK(i[-1] == L('T'));
    CHECK(i[0] == L('e'));
    CHECK(i[1] == L('s'));
    CHECK(i[2] == L('t'));

    i = b->cend() - 2;
    CHECK(i[-2] == L('T'));
    CHECK(i[-1] == L('e'));
    CHECK(i[0] == L('s'));
    CHECK(i[1] == L('t'));
}

TEST_CASE("Buffer iterator, range to string") {
    buffer_pointer b = create_buffer(L("string"));

    b->substitute([](source_pointer &&p) -> source_pointer {
        return prepend(std::move(p), L("Test "));
    });

    string s = to_string(b->begin() + 2, b->end() - 2);
    CHECK(s == L("st stri"));
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
