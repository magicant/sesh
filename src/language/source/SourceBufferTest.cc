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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <utility>
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"

namespace {

using sesh::language::source::Source;
using sesh::language::source::SourceBuffer;
using sesh::language::source::SourceLocation;
using sesh::language::source::checkSourceString;

using BufferPointer = sesh::language::source::SourceBuffer::Pointer;
using SBCI = sesh::language::source::SourceBuffer::ConstIterator;
using SourcePointer = sesh::language::source::Source::Pointer;
using String = sesh::language::source::SourceBuffer::String;

class StringPrependedSource : public Source {
public:
    StringPrependedSource(Pointer &&p, String &&s) :
            Source(std::move(p), 0, 0, std::move(s)) { }
    SourceLocation locationInAlternate(Size) const override {
        throw "unexpected";
    }
};

SourcePointer prepend(SourcePointer &&p, String &&s) {
    return SourcePointer(
            new StringPrependedSource(std::move(p), std::move(s)));
}

BufferPointer createBuffer(SourceBuffer::String &&string) {
    BufferPointer b = SourceBuffer::create();

    b->substitute([&string](SourcePointer &&p) -> SourcePointer {
        return prepend(std::move(p), std::move(string));
    });
    return b;
}

void checkSourceBufferString(
        const SourceBuffer &sb, const SourceBuffer::String &string) {
    for (SourceBuffer::Size i = 0; i < string.length(); ++i) {
        CHECK(sb.at(i) == string.at(i));
        CHECK(sb[i] == string[i]);
    }
    CHECK_THROWS_AS(sb.at(string.length()), std::out_of_range);
    CHECK(sb[string.length()] == SourceBuffer::Char());
}

} // namespace

TEST_CASE("Source buffer construction and creation") {
    CHECK_NOTHROW(SourceBuffer());
    CHECK_NOTHROW(SourceBuffer::create());
}

TEST_CASE("Source buffer length") {
    CHECK(SourceBuffer().length() == 0);
}

TEST_CASE("Source buffer substitution, at and operator[]") {
    SourceBuffer sb;

    checkSourceBufferString(sb, L(""));
    sb.substitute([](SourcePointer &&p) -> SourcePointer {
        CHECK((p == nullptr));
        return prepend(std::move(p), L("Test"));
    });
    checkSourceBufferString(sb, L("Test"));
    sb.substitute([](SourcePointer &&p) -> SourcePointer {
        // XXX: GCC 4.8.1 and Clang 3.3 want explicit move.
        return std::move(p);
    });
    checkSourceBufferString(sb, L("Test"));
    sb.substitute([](SourcePointer &&p) -> SourcePointer {
        REQUIRE((p != nullptr));
        CHECK((dynamic_cast<const StringPrependedSource *>(p.get()) !=
                nullptr));
        checkSourceString(*p, L("Test"));
        // XXX: GCC 4.8.1 and Clang 3.3 want explicit move.
        return std::move(p);
    });
    checkSourceBufferString(sb, L("Test"));
}

TEST_CASE("Source buffer begin and end") {
    BufferPointer b = SourceBuffer::create();
    CHECK_NOTHROW(b->cbegin());
    CHECK_NOTHROW(b->cend());
    CHECK_NOTHROW(b->begin());
    CHECK_NOTHROW(b->end());

    b->substitute([](SourcePointer &&p) -> SourcePointer {
        return prepend(std::move(p), L("Test"));
    });
    CHECK_NOTHROW(b->cbegin());
    CHECK_NOTHROW(b->cend());
    CHECK_NOTHROW(b->begin());
    CHECK_NOTHROW(b->end());
}

TEST_CASE("Source buffer iterator normal construction") {
    BufferPointer b = createBuffer(L("X"));
    CHECK_NOTHROW(SBCI(b, 0));
    CHECK_NOTHROW(SBCI(b, 1));
}

TEST_CASE("Source buffer iterator special construction") {
    // default constructible
    SBCI i1;
    // copy constructible
    SBCI i2(i1);
    // move constructible
    SBCI i2move(std::move(i2));
}

TEST_CASE("Source buffer iterator assignment") {
    SBCI i1, i2;
    // copy assignable
    i1 = i2;
    // move assignable
    i1 = std::move(i2);
}

TEST_CASE("Source buffer iterator dereferencing") {
    BufferPointer b = createBuffer(L("ABC"));
    CHECK(*b->cbegin() == L('A'));

    SBCI ci = b->cbegin(), i = b->begin();
    // ci == i
    CHECK(&*ci == &*i);
    ++ci, ++i;
    CHECK(&*ci == &*i);
    ++ci, ++i;
    CHECK(&*ci == &*i);
}

TEST_CASE("Source buffer iterator incrementation") {
    BufferPointer b = createBuffer(L("Test"));
    SBCI i = b->cbegin();
    CHECK_NOTHROW(++i);
    CHECK(*i == L('e'));
    CHECK(*++i == L('s'));
    CHECK(*i++ == L('s'));
    CHECK(*i == L('t'));

    const SBCI &iref = i++;
    CHECK(*iref == L('t'));
}

TEST_CASE("Source buffer iterator decrementation") {
    BufferPointer b = createBuffer(L("Test"));
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

TEST_CASE("Source buffer iterator equality") {
    BufferPointer b = createBuffer(L("ABC"));
    SBCI ci = b->cbegin(), i = b->begin();
    SBCI cend = b->cend(), end = b->end();

    CHECK((ci == ci));
    CHECK((ci == i));
    CHECK((i == ci));
    CHECK((i == i));
    CHECK((cend == cend));
    CHECK((cend == end));
    CHECK((end == cend));
    CHECK((end == end));
    CHECK((ci != cend));
    CHECK((i != end));

    CHECK_FALSE((ci != ci));
    CHECK_FALSE((ci != i));
    CHECK_FALSE((i != ci));
    CHECK_FALSE((i != i));
    CHECK_FALSE((cend != cend));
    CHECK_FALSE((cend != end));
    CHECK_FALSE((end != cend));
    CHECK_FALSE((end != end));
    CHECK_FALSE((ci == cend));
    CHECK_FALSE((i == end));

    ++ci;
    CHECK((ci != i));
    CHECK((ci != cend));
    CHECK_FALSE((ci == i));
    CHECK_FALSE((ci == cend));

    ++i;
    CHECK((ci == i));
    CHECK((i != cend));
    CHECK_FALSE((ci != i));
    CHECK_FALSE((i == cend));

    ++i;
    ++i;
    CHECK((ci != i));
    CHECK((i == cend));
    CHECK((i == end));
    CHECK_FALSE((ci == i));
    CHECK_FALSE((i != cend));
    CHECK_FALSE((i != end));

    ++ci;
    ++ci;
    CHECK((ci == i));
    CHECK((ci == cend));
    CHECK((ci == end));
    CHECK_FALSE((ci != i));
    CHECK_FALSE((ci != cend));
    CHECK_FALSE((ci != end));

    CHECK((SBCI() == SBCI()));
    CHECK_FALSE((SBCI() != SBCI()));
}

TEST_CASE("Source buffer iterator multi-pass guarantee 1") {
    BufferPointer b = createBuffer(L("ABC"));
    SBCI ci = b->cbegin(), i = b->begin();
    // ci == i
    CHECK((++ci == ++i));
    CHECK((++ci == ++i));
    CHECK((++ci == ++i));
}

TEST_CASE("Source buffer iterator multi-pass guarantee 2") {
    BufferPointer b = createBuffer(L("ABC"));
    SBCI i1 = b->begin();
    SBCI i2 = i1;
    CHECK(((void) ++i2, *i1) == *i1);
}

TEST_CASE("Source buffer iterator random access") {
    BufferPointer b = createBuffer(L("Test"));
    SBCI begin = b->cbegin();
    SBCI end = b->cend();
    SBCI i = begin;

    CHECK_NOTHROW(i += 1);
    CHECK((i == begin + 1));
    CHECK((i == 1 + begin));
    CHECK((i == end + -3));
    CHECK((i == -3 + end));
    CHECK(*i == L('e'));
    CHECK_NOTHROW(i += 2);
    CHECK((i == b->cbegin() + 3));
    CHECK((i == 3 + b->cbegin()));
    CHECK((i == b->cend() + -1));
    CHECK((i == -1 + b->cend()));
    CHECK(*i == L('t'));
    CHECK_NOTHROW(i += -3);
    CHECK((i == begin + 0));
    CHECK((i == 0 + begin));
    CHECK((i == end + -4));
    CHECK((i == -4 + end));
    CHECK(*i == L('T'));
    CHECK_NOTHROW(i += 4);
    CHECK((i == b->cbegin() + 4));
    CHECK((i == 4 + b->cbegin()));
    CHECK((i == b->cend() + 0));
    CHECK((i == 0 + b->cend()));
    CHECK_NOTHROW(i += -2);
    CHECK((i == begin + 2));
    CHECK((i == 2 + begin));
    CHECK((i == end + -2));
    CHECK((i == -2 + end));
    CHECK(*i == L('s'));

    CHECK_NOTHROW(i -= -2);
    CHECK((i == begin - -4));
    CHECK((i - begin == 4));
    CHECK((begin - i == -4));
    CHECK((i == end - 0));
    CHECK((i - end == 0));
    CHECK((end - i == 0));
    CHECK_NOTHROW(i -= 4);
    CHECK((i == b->cbegin() - 0));
    CHECK((i - b->cbegin() == 0));
    CHECK((b->cbegin() - i == 0));
    CHECK((i == b->cend() - 4));
    CHECK((i - b->cend() == -4));
    CHECK((b->cend() - i == 4));
    CHECK(*i == L('T'));
    CHECK_NOTHROW(i -= -3);
    CHECK((i == begin - -3));
    CHECK((i - begin == 3));
    CHECK((begin - i == -3));
    CHECK((i == end - 1));
    CHECK((i - end == -1));
    CHECK((end - i == 1));
    CHECK(*i == L('t'));
    CHECK_NOTHROW(i -= 2);
    CHECK((i == b->cbegin() - -1));
    CHECK((i - b->cbegin() == 1));
    CHECK((b->cbegin() - i == -1));
    CHECK((i == b->cend() - 3));
    CHECK((i - b->cend() == -3));
    CHECK((b->cend() - i == 3));
    CHECK(*i == L('e'));
    CHECK_NOTHROW(i -= 1);
    CHECK((i == begin - 0));
    CHECK((i - begin == 0));
    CHECK((begin - i == 0));
    CHECK((i == end - 4));
    CHECK((i - end == -4));
    CHECK((end - i == 4));
    CHECK(*i == L('T'));
}

TEST_CASE("Source buffer iterator inequality") {
    BufferPointer b = createBuffer(L("ABC"));
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

TEST_CASE("Source buffer iterator index") {
    BufferPointer b = createBuffer(L("Test"));

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

TEST_CASE("Source buffer iterator swapping") {
    BufferPointer b1 = createBuffer(L("Test 1"));
    BufferPointer b2 = createBuffer(L("Test 2"));

    SBCI i1 = b1->cbegin() + 1;
    SBCI i2 = b2->cbegin() + 3;
    SBCI oldI1 = i1;
    SBCI oldI2 = i2;

    using namespace std;
    swap(i1, i2);
    CHECK(i1 == oldI2);
    CHECK(i2 == oldI1);
}

TEST_CASE("Source buffer iterator range to string") {
    BufferPointer b = createBuffer(L("string"));

    b->substitute([](SourcePointer &&p) -> SourcePointer {
        return prepend(std::move(p), L("Test "));
    });

    String s = toString(b->begin() + 2, b->end() - 2);
    CHECK(s == L("st stri"));
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
