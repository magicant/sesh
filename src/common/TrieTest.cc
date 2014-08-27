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

#include <memory>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include "common/Trie.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"

namespace {

using sesh::common::Trie;
using sesh::common::xchar;
using sesh::common::xstring;

TEST_CASE("Trie, default construction") {
    Trie<xchar, int> t;
}

TEST_CASE("Trie, construction with comparator and key_comp") {
    auto comp = [](xchar a, xchar b) { return a > b; };
    Trie<xchar, int, decltype(comp)> t(comp);
    CHECK_FALSE(t.key_comp()(L('!'), L('!')));
    CHECK(t.key_comp()(L('\1'), L('\0')));

    t['0'].getOrCreateValue() = 10;
    t['2'].getOrCreateValue() = 12;
    t['1'].getOrCreateValue() = 11;

    auto i = t.traverserBegin();
    auto end = t.traverserEnd();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    ++i;
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['2']));
    ++i;
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['1']));
    ++i;
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['0']));
    ++i;
    CHECK(i == end);
}

TEST_CASE("Trie, move constructor") {
    Trie<int, int> t1;
    t1.getOrCreateValue() = 1;
    t1[1].getOrCreateValue() = 10;
    t1[1][2].getOrCreateValue() = 20;

    Trie<int, int> t2 = std::move(t1);
    CHECK(t1.size() == 1);
    CHECK(t2.size() == 3);
    CHECK(t2.value() == 1);
    CHECK(t2[1].value() == 10);
    CHECK(t2[1][2].value() == 20);

    t2[1].eraseDescendants();
    CHECK(t2.size() == 2);
    t2[1].eraseValue();
    CHECK(t2.size() == 1);
    t2.eraseValue();
    CHECK(t2.size() == 0);
}

TEST_CASE("Trie, construction from iterator range") {
    std::pair<std::string, int> values[] = {{"abc", 1}, {"aaa", 2}};
    Trie<char, int> t(std::begin(values), std::end(values));
    CHECK(t.size() == 2);
    REQUIRE(t['a']['a']['a'].hasValue());
    REQUIRE(t['a']['b']['c'].hasValue());
    CHECK(t['a']['a']['a'].value() == 2);
    CHECK(t['a']['b']['c'].value() == 1);
}

TEST_CASE("Trie, construction from initializer list") {
    Trie<char, int> t({{"abc", 1}, {"aaa", 2}});
    CHECK(t.size() == 2);
    REQUIRE(t['a']['a']['a'].hasValue());
    REQUIRE(t['a']['b']['c'].hasValue());
    CHECK(t['a']['a']['a'].value() == 2);
    CHECK(t['a']['b']['c'].value() == 1);
}

TEST_CASE("Trie, node value") {
    Trie<char, std::pair<int, int>> t;
    CHECK_FALSE(t.maybeValue().hasValue());
    CHECK_FALSE(t.hasValue());

    auto e1 = t.emplaceValue(1, 2);
    CHECK(e1.first.first == 1);
    CHECK(e1.first.second == 2);
    CHECK(e1.second);
    CHECK(t.maybeValue().hasValue());
    CHECK(t.hasValue());
    CHECK(std::addressof(e1.first) == std::addressof(t.maybeValue().value()));
    CHECK(std::addressof(e1.first) == std::addressof(t.value()));

    // emplaceValue has no effect because the value already exists
    auto e2 = t.emplaceValue(3, 4);
    CHECK(e2.first.first == 1);
    CHECK(e2.first.second == 2);
    CHECK_FALSE(e2.second);
    CHECK(t.maybeValue().hasValue());
    CHECK(t.hasValue());
    CHECK(std::addressof(e2.first) == std::addressof(t.maybeValue().value()));
    CHECK(std::addressof(e2.first) == std::addressof(t.value()));

    auto &intPair1 = t.getOrCreateValue(); // no effect again
    CHECK(intPair1.first == 1);
    CHECK(intPair1.second == 2);
    CHECK(t.hasValue());
    CHECK(std::addressof(intPair1) == std::addressof(t.maybeValue().value()));
    CHECK(std::addressof(intPair1) == std::addressof(t.value()));

    t.eraseValue();
    CHECK_FALSE(t.maybeValue().hasValue());
    CHECK_FALSE(t.hasValue());

    t.eraseValue(); // already erased; no effect
    CHECK_FALSE(t.maybeValue().hasValue());
    CHECK_FALSE(t.hasValue());

    auto &intPair2 = t.getOrCreateValue();
    CHECK(intPair2.first == 0);
    CHECK(intPair2.second == 0);
    CHECK(t.hasValue());
    CHECK(std::addressof(intPair2) == std::addressof(t.maybeValue().value()));
    CHECK(std::addressof(intPair2) == std::addressof(t.value()));
}

TEST_CASE("Trie, emplace child") {
    Trie<xstring, int> t;

    auto a1 = t.emplaceChild(3, L('a'));
    CHECK(a1.first.empty());
    CHECK(a1.second);

    auto b1 = t.emplaceChild(L("b"));
    CHECK(b1.first.empty());
    CHECK(b1.second);

    auto a2 = t.emplaceChild(L("aaa"));
    CHECK(std::addressof(a1.first) == std::addressof(a2.first));
    CHECK_FALSE(a2.second);
}

TEST_CASE("Trie, emplace descendants") {
    Trie<char, int> t;
    const std::string s = "aaa";
    auto &t1 = t.emplaceDescendants(s.begin(), s.end());
    auto &t2 = t.emplaceDescendants(std::string("abc"));
    auto &t3 = t2.emplaceDescendants(std::string(""));
    CHECK(std::addressof(t1) == std::addressof(t['a']['a']['a']));
    CHECK(std::addressof(t2) == std::addressof(t['a']['b']['c']));
    CHECK(std::addressof(t3) == std::addressof(t['a']['b']['c']));
}

TEST_CASE("Trie, operator[]") {
    Trie<char, int> t;
    auto &a = t['a'];
    auto &b = t[std::move('b')];
    auto &c = t[std::move('c')];
    a.getOrCreateValue() = 1;
    b.getOrCreateValue() = 2;
    c.getOrCreateValue() = 3;
    CHECK(t['a'].value() == 1);
    CHECK(t['b'].value() == 2);
    CHECK(t[std::move('c')].value() == 3);
}

TEST_CASE("Trie, at") {
    Trie<char, int> t;
    t['a']['b'].getOrCreateValue() = 10;
    CHECK(t.at('a').at('b').value() == 10);
    CHECK_THROWS_AS(t.at('b'), std::out_of_range);
    CHECK_THROWS_AS(t.at('a').at('a'), std::out_of_range);
}

TEST_CASE("Trie, size and empty") {
    Trie<int, char> t;
    CHECK(t.empty());
    CHECK(t.size() == 0);

    t[0];
    CHECK(t.empty());
    CHECK(t.size() == 0);

    t[0].getOrCreateValue();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 1);

    t[1].getOrCreateValue();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 2);

    t[1][0].getOrCreateValue();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 3);

    t[0].getOrCreateValue();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 3);

    t[0].eraseValue();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 2);

    t[1].eraseValue();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 1);

    t[1][0].eraseValue();
    CHECK(t.empty());
    CHECK(t.size() == 0);

    CHECK(t.max_size() > 0);
}

TEST_CASE("Trie, erase children") {
    Trie<int, int> t;
    t.emplaceValue(1);
    t[0].emplaceValue(2);
    t[0][0].emplaceValue(3);
    t[0][1][0].emplaceValue(4);
    t[1][0].emplaceValue(5);
    t[1][1][0].emplaceValue(6);
    CHECK(t.size() == 6);

    t[0].eraseDescendants();
    CHECK(t.size() == 4);
    REQUIRE(t.hasValue());
    CHECK(t.value() == 1);
    REQUIRE(t[0].hasValue());
    CHECK(t[0].value() == 2);
    CHECK_THROWS_AS(t[0].at(0), std::out_of_range);
    CHECK_THROWS_AS(t[0].at(1), std::out_of_range);
    REQUIRE(t[1][0].hasValue());
    CHECK(t[1][0].value() == 5);
    REQUIRE(t[1][1][0].hasValue());
    CHECK(t[1][1][0].value() == 6);

    t.eraseDescendants();
    CHECK(t.size() == 1);
    REQUIRE(t.hasValue());
    CHECK(t.value() == 1);
    CHECK_THROWS_AS(t.at(0), std::out_of_range);
    CHECK_THROWS_AS(t.at(1), std::out_of_range);
}

TEST_CASE("Trie, clear") {
    Trie<int, int> t;
    t.emplaceValue(1);
    t[0].emplaceValue(2);
    t[0][0].emplaceValue(3);
    t[0][1][0].emplaceValue(4);
    t[1][0].emplaceValue(5);
    t[1][1][0].emplaceValue(6);
    CHECK(t.size() == 6);

    t[0].clear();
    CHECK(t.size() == 3);
    REQUIRE(t.hasValue());
    CHECK(t.value() == 1);
    CHECK_FALSE(t.at(0).hasValue());
    CHECK_THROWS_AS(t.at(0).at(0), std::out_of_range);
    CHECK_THROWS_AS(t.at(0).at(1), std::out_of_range);
    REQUIRE(t[1][0].hasValue());
    CHECK(t[1][0].value() == 5);
    REQUIRE(t[1][1][0].hasValue());
    CHECK(t[1][1][0].value() == 6);

    t.clear();
    CHECK(t.size() == 0);
    CHECK_FALSE(t.hasValue());
    CHECK_THROWS_AS(t.at(0), std::out_of_range);
    CHECK_THROWS_AS(t.at(1), std::out_of_range);
}

TEST_CASE("Trie non-const traverser, basics") {
    Trie<char, int> t;
    Trie<char, int>::Traverser<> i = t.traverserBegin();
    Trie<char, int>::Traverser<> end = t.traverserEnd();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.pathString() == "");
    CHECK(i->size() == 0);
    i = end;
    CHECK(i == end);
    CHECK(decltype(i)() == decltype(i)());
}

TEST_CASE("Trie const traverser, basics") {
    const Trie<char, int> t;
    Trie<char, int>::ConstTraverser<> i = t.traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t.traverserEnd();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.pathString() == "");
    CHECK(i->size() == 0);
    i = end;
    CHECK(i == end);
    CHECK(decltype(i)() == decltype(i)());
}

TEST_CASE("Trie traverser, conversion") {
    using V = std::vector<char>;

    Trie<char, int> t;
    Trie<char, int>::Traverser<V> ib1 = t.traverserBegin<V>();
    Trie<char, int>::Traverser<V> ie1 = t.traverserEnd<V>();
    Trie<char, int>::ConstTraverser<V> ib2 = t.traverserBegin<V>();
    Trie<char, int>::ConstTraverser<V> ie2 = t.traverserEnd<V>();
    Trie<char, int>::ConstTraverser<V> ib3 = ib1;
    Trie<char, int>::ConstTraverser<V> ie3 = ie1;
    Trie<char, int>::ConstTraverser<V> ib4 = t.constTraverserBegin<V>();
    Trie<char, int>::ConstTraverser<V> ie4 = t.constTraverserEnd<V>();

    const Trie<char, int> ct;
    Trie<char, int>::ConstTraverser<V> cib1 = t.traverserBegin<V>();
    Trie<char, int>::ConstTraverser<V> cie1 = t.traverserEnd<V>();
    Trie<char, int>::ConstTraverser<V> cib2 = t.constTraverserBegin<V>();
    Trie<char, int>::ConstTraverser<V> cie2 = t.constTraverserEnd<V>();
}

TEST_CASE("Trie non-const traverser, forward, whole tree") {
    Trie<char, int> t;
    t['a'].emplaceValue(2);
    t['a']['a'];
    t['a']['b']['a'].emplaceValue(4);
    t['b']['a'].emplaceValue(5);
    t['b']['b']['a'];

    Trie<char, int>::Traverser<> i = t.traverserBegin();
    Trie<char, int>::Traverser<> end = t.traverserEnd();
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.pathString() == "aa");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.pathString() == "aba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']));
    CHECK(i.pathString() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['a']));
    CHECK(i.pathString() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']));
    CHECK(i.pathString() == "bb");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']['a']));
    CHECK(i.pathString() == "bba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie const traverser, forward, whole tree") {
    Trie<char, int> t;
    t['a'].emplaceValue(2);
    t['a']['a'];
    t['a']['b']['a'].emplaceValue(4);
    t['b']['a'].emplaceValue(5);
    t['b']['b']['a'];

    Trie<char, int>::ConstTraverser<> i = t.traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t.traverserEnd();
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.pathString() == "aa");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.pathString() == "aba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']));
    CHECK(i.pathString() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['a']));
    CHECK(i.pathString() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']));
    CHECK(i.pathString() == "bb");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']['a']));
    CHECK(i.pathString() == "bba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie non-const traverser, forward, subtree") {
    Trie<char, int> t;
    t['a'].emplaceValue(2);
    t['a']['a'];
    t['a']['b']['a'].emplaceValue(4);
    t['b']['a'].emplaceValue(5);
    t['b']['b']['a'];

    Trie<char, int>::Traverser<> i = t['a'].traverserBegin();
    Trie<char, int>::Traverser<> end = t['a'].traverserEnd();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.pathString() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.pathString() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie const traverser, forward, subtree") {
    Trie<char, int> t;
    t['a'].emplaceValue(2);
    t['a']['a'];
    t['a']['b']['a'].emplaceValue(4);
    t['b']['a'].emplaceValue(5);
    t['b']['b']['a'];

    Trie<char, int>::ConstTraverser<> i = t['a'].traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t['a'].traverserEnd();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.pathString() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.pathString() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie non-const traverser, forward, postfix operator") {
    Trie<char, int> t;
    t.emplaceValue(0);
    t['a'].emplaceValue(1);

    Trie<char, int>::Traverser<> i1 = t.traverserBegin();
    Trie<char, int>::Traverser<> end = t.traverserEnd();
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t));
    CHECK(i1.pathString() == "");

    Trie<char, int>::Traverser<> i2 = i1++;
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t['a']));
    CHECK(i1.pathString() == "a");
    CHECK(i2 != end);
    CHECK(std::addressof(*i2) == std::addressof(t));
    CHECK(i2.pathString() == "");

    Trie<char, int>::Traverser<> i3 = i1++;
    CHECK(i1 == end);
    CHECK(i3 != end);
    CHECK(std::addressof(*i3) == std::addressof(t['a']));
    CHECK(i3.pathString() == "a");
}

TEST_CASE("Trie const traverser, forward, postfix operator") {
    Trie<char, int> t;
    t.emplaceValue(0);
    t['a'].emplaceValue(1);

    Trie<char, int>::ConstTraverser<> i1 = t.traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t.traverserEnd();
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t));
    CHECK(i1.pathString() == "");

    Trie<char, int>::ConstTraverser<> i2 = i1++;
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t['a']));
    CHECK(i1.pathString() == "a");
    CHECK(i2 != end);
    CHECK(std::addressof(*i2) == std::addressof(t));
    CHECK(i2.pathString() == "");

    Trie<char, int>::ConstTraverser<> i3 = i1++;
    CHECK(i1 == end);
    CHECK(i3 != end);
    CHECK(std::addressof(*i3) == std::addressof(t['a']));
    CHECK(i3.pathString() == "a");
}

TEST_CASE("Trie non-const traverser, down, key digit") {
    Trie<char, int> t;
    t['a']['b'];

    Trie<char, int>::Traverser<> i = t.traverserBegin();
    Trie<char, int>::Traverser<> end = t.traverserEnd();
    CHECK_FALSE(i.down('z'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.pathString() == "");
    CHECK(i.down('a'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "a");
    CHECK_FALSE(i.down('y'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "a");
    CHECK(i.down('b'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
}

TEST_CASE("Trie const traverser, down, key digit") {
    Trie<char, int> t;
    t['a']['b'];

    Trie<char, int>::ConstTraverser<> i = t.traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t.traverserEnd();
    CHECK_FALSE(i.down('z'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.pathString() == "");
    CHECK(i.down('a'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "a");
    CHECK_FALSE(i.down('y'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "a");
    CHECK(i.down('b'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
}

TEST_CASE("Trie non-const traverser, down, key iterator") {
    Trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    std::string aba = "aba", cde = "cde";

    Trie<char, int>::Traverser<> i = t.traverserBegin();
    Trie<char, int>::Traverser<> end = t.traverserEnd();
    CHECK(i.down(aba.begin(), aba.end()) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(i.down(cde.begin(), cde.end()) == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.pathString() == "abcde");
}

TEST_CASE("Trie const traverser, down, key iterator") {
    Trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    std::string aba = "aba", cde = "cde";

    Trie<char, int>::ConstTraverser<> i = t.traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t.traverserEnd();
    CHECK(i.down(aba.begin(), aba.end()) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(i.down(cde.begin(), cde.end()) == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.pathString() == "abcde");
}

TEST_CASE("Trie non-const traverser, down, key string") {
    Trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    Trie<char, int>::Traverser<> i = t.traverserBegin();
    Trie<char, int>::Traverser<> end = t.traverserEnd();
    CHECK(i.down("aba") == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(i.down("cde") == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.pathString() == "abcde");
}

TEST_CASE("Trie const traverser, down, key string") {
    Trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    Trie<char, int>::ConstTraverser<> i = t.traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t.traverserEnd();
    CHECK(i.down("aba") == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(i.down("cde") == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.pathString() == "abcde");
}

TEST_CASE("Trie non-const traverser, up, whole tree") {
    Trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    Trie<char, int>::Traverser<> i = t.traverserBegin();
    Trie<char, int>::Traverser<> end = t.traverserEnd();
    i.down("abcde");
    CHECK(i.up() == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']));
    CHECK(i.pathString() == "abcd");
    CHECK(i.up(2) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(i.up(3) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.pathString() == "");
}

TEST_CASE("Trie const traverser, up, whole tree") {
    Trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    Trie<char, int>::ConstTraverser<> i = t.traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t.traverserEnd();
    i.down("abcde");
    CHECK(i.up() == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']));
    CHECK(i.pathString() == "abcd");
    CHECK(i.up(2) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.pathString() == "ab");
    CHECK(i.up(3) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.pathString() == "");
}

TEST_CASE("Trie non-const traverser, up, subtree") {
    Trie<char, int> t;
    t['a']['b'];

    Trie<char, int>::Traverser<> i = t['a'].traverserBegin();
    Trie<char, int>::Traverser<> end = t['a'].traverserEnd();
    i.down('b');
    CHECK(i.up(2) == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "");
}

TEST_CASE("Trie const traverser, up, subtree") {
    Trie<char, int> t;
    t['a']['b'];

    Trie<char, int>::ConstTraverser<> i = t['a'].traverserBegin();
    Trie<char, int>::ConstTraverser<> end = t['a'].traverserEnd();
    i.down('b');
    CHECK(i.up(2) == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "");
}

TEST_CASE("Trie non-const iterator, non-constness of value") {
    Trie<char, int> t;
    Trie<char, int>::Traverser<> i = t.traverserBegin();
    i->emplaceChild('a');
    ++i;
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.pathString() == "a");
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
