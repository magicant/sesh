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

#include <memory>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include "catch.hpp"
#include "common/trie.hh"
#include "common/xchar.hh"
#include "common/xstring.hh"

namespace {

using sesh::common::trie;
using sesh::common::xchar;
using sesh::common::xstring;

TEST_CASE("Trie, default construction") {
    trie<xchar, int> t;
}

TEST_CASE("Trie, construction with comparator and key_comp") {
    auto comp = [](xchar a, xchar b) { return a > b; };
    trie<xchar, int, decltype(comp)> t(comp);
    CHECK_FALSE(t.key_comp()(L('!'), L('!')));
    CHECK(t.key_comp()(L('\1'), L('\0')));

    t['0'].get_or_create_value() = 10;
    t['2'].get_or_create_value() = 12;
    t['1'].get_or_create_value() = 11;

    auto i = t.traverser_begin();
    auto end = t.traverser_end();
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
    trie<int, int> t1;
    t1.get_or_create_value() = 1;
    t1[1].get_or_create_value() = 10;
    t1[1][2].get_or_create_value() = 20;

    trie<int, int> t2 = std::move(t1);
    CHECK(t1.size() == 1);
    CHECK(t2.size() == 3);
    CHECK(t2.value() == 1);
    CHECK(t2[1].value() == 10);
    CHECK(t2[1][2].value() == 20);

    t2[1].erase_descendants();
    CHECK(t2.size() == 2);
    t2[1].erase_value();
    CHECK(t2.size() == 1);
    t2.erase_value();
    CHECK(t2.size() == 0);
}

TEST_CASE("Trie, construction from iterator range") {
    std::pair<std::string, int> values[] = {{"abc", 1}, {"aaa", 2}};
    trie<char, int> t(std::begin(values), std::end(values));
    CHECK(t.size() == 2);
    REQUIRE(t['a']['a']['a'].has_value());
    REQUIRE(t['a']['b']['c'].has_value());
    CHECK(t['a']['a']['a'].value() == 2);
    CHECK(t['a']['b']['c'].value() == 1);
}

TEST_CASE("Trie, construction from initializer list") {
    trie<char, int> t({{"abc", 1}, {"aaa", 2}});
    CHECK(t.size() == 2);
    REQUIRE(t['a']['a']['a'].has_value());
    REQUIRE(t['a']['b']['c'].has_value());
    CHECK(t['a']['a']['a'].value() == 2);
    CHECK(t['a']['b']['c'].value() == 1);
}

TEST_CASE("Trie, node value") {
    trie<char, std::pair<int, int>> t;
    CHECK_FALSE(t.maybe_value());
    CHECK_FALSE(t.has_value());

    auto e1 = t.emplace_value(1, 2);
    CHECK(e1.first.first == 1);
    CHECK(e1.first.second == 2);
    CHECK(e1.second);
    CHECK(t.maybe_value());
    CHECK(t.has_value());
    CHECK(std::addressof(e1.first) == std::addressof(t.maybe_value().value()));
    CHECK(std::addressof(e1.first) == std::addressof(t.value()));

    // emplace_value has no effect because the value already exists
    auto e2 = t.emplace_value(3, 4);
    CHECK(e2.first.first == 1);
    CHECK(e2.first.second == 2);
    CHECK_FALSE(e2.second);
    CHECK(t.maybe_value());
    CHECK(t.has_value());
    CHECK(std::addressof(e2.first) == std::addressof(t.maybe_value().value()));
    CHECK(std::addressof(e2.first) == std::addressof(t.value()));

    auto &int_pair1 = t.get_or_create_value(); // no effect again
    CHECK(int_pair1.first == 1);
    CHECK(int_pair1.second == 2);
    CHECK(t.has_value());
    CHECK(std::addressof(int_pair1) ==
            std::addressof(t.maybe_value().value()));
    CHECK(std::addressof(int_pair1) == std::addressof(t.value()));

    t.erase_value();
    CHECK_FALSE(t.maybe_value());
    CHECK_FALSE(t.has_value());

    t.erase_value(); // already erased; no effect
    CHECK_FALSE(t.maybe_value());
    CHECK_FALSE(t.has_value());

    auto &int_pair2 = t.get_or_create_value();
    CHECK(int_pair2.first == 0);
    CHECK(int_pair2.second == 0);
    CHECK(t.has_value());
    CHECK(std::addressof(int_pair2) ==
            std::addressof(t.maybe_value().value()));
    CHECK(std::addressof(int_pair2) == std::addressof(t.value()));
}

TEST_CASE("Trie, emplace child") {
    trie<xstring, int> t;

    auto a1 = t.emplace_child(3, L('a'));
    CHECK(a1.first.empty());
    CHECK(a1.second);

    auto b1 = t.emplace_child(L("b"));
    CHECK(b1.first.empty());
    CHECK(b1.second);

    auto a2 = t.emplace_child(L("aaa"));
    CHECK(std::addressof(a1.first) == std::addressof(a2.first));
    CHECK_FALSE(a2.second);
}

TEST_CASE("Trie, emplace descendants") {
    trie<char, int> t;
    const std::string s = "aaa";
    auto &t1 = t.emplace_descendants(s.begin(), s.end());
    auto &t2 = t.emplace_descendants(std::string("abc"));
    auto &t3 = t2.emplace_descendants(std::string(""));
    CHECK(std::addressof(t1) == std::addressof(t['a']['a']['a']));
    CHECK(std::addressof(t2) == std::addressof(t['a']['b']['c']));
    CHECK(std::addressof(t3) == std::addressof(t['a']['b']['c']));
}

TEST_CASE("Trie, operator[]") {
    trie<char, int> t;
    auto &a = t['a'];
    auto &b = t[std::move('b')];
    auto &c = t[std::move('c')];
    a.get_or_create_value() = 1;
    b.get_or_create_value() = 2;
    c.get_or_create_value() = 3;
    CHECK(t['a'].value() == 1);
    CHECK(t['b'].value() == 2);
    CHECK(t[std::move('c')].value() == 3);
}

TEST_CASE("Trie, at") {
    trie<char, int> t;
    t['a']['b'].get_or_create_value() = 10;
    CHECK(t.at('a').at('b').value() == 10);
    CHECK_THROWS_AS(t.at('b'), std::out_of_range);
    CHECK_THROWS_AS(t.at('a').at('a'), std::out_of_range);
}

TEST_CASE("Trie, size and empty") {
    trie<int, char> t;
    CHECK(t.empty());
    CHECK(t.size() == 0);

    t[0];
    CHECK(t.empty());
    CHECK(t.size() == 0);

    t[0].get_or_create_value();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 1);

    t[1].get_or_create_value();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 2);

    t[1][0].get_or_create_value();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 3);

    t[0].get_or_create_value();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 3);

    t[0].erase_value();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 2);

    t[1].erase_value();
    CHECK_FALSE(t.empty());
    CHECK(t.size() == 1);

    t[1][0].erase_value();
    CHECK(t.empty());
    CHECK(t.size() == 0);

    CHECK(t.max_size() > 0);
}

TEST_CASE("Trie, erase children") {
    trie<int, int> t;
    t.emplace_value(1);
    t[0].emplace_value(2);
    t[0][0].emplace_value(3);
    t[0][1][0].emplace_value(4);
    t[1][0].emplace_value(5);
    t[1][1][0].emplace_value(6);
    CHECK(t.size() == 6);

    t[0].erase_descendants();
    CHECK(t.size() == 4);
    REQUIRE(t.has_value());
    CHECK(t.value() == 1);
    REQUIRE(t[0].has_value());
    CHECK(t[0].value() == 2);
    CHECK_THROWS_AS(t[0].at(0), std::out_of_range);
    CHECK_THROWS_AS(t[0].at(1), std::out_of_range);
    REQUIRE(t[1][0].has_value());
    CHECK(t[1][0].value() == 5);
    REQUIRE(t[1][1][0].has_value());
    CHECK(t[1][1][0].value() == 6);

    t.erase_descendants();
    CHECK(t.size() == 1);
    REQUIRE(t.has_value());
    CHECK(t.value() == 1);
    CHECK_THROWS_AS(t.at(0), std::out_of_range);
    CHECK_THROWS_AS(t.at(1), std::out_of_range);
}

TEST_CASE("Trie, clear") {
    trie<int, int> t;
    t.emplace_value(1);
    t[0].emplace_value(2);
    t[0][0].emplace_value(3);
    t[0][1][0].emplace_value(4);
    t[1][0].emplace_value(5);
    t[1][1][0].emplace_value(6);
    CHECK(t.size() == 6);

    t[0].clear();
    CHECK(t.size() == 3);
    REQUIRE(t.has_value());
    CHECK(t.value() == 1);
    CHECK_FALSE(t.at(0).has_value());
    CHECK_THROWS_AS(t.at(0).at(0), std::out_of_range);
    CHECK_THROWS_AS(t.at(0).at(1), std::out_of_range);
    REQUIRE(t[1][0].has_value());
    CHECK(t[1][0].value() == 5);
    REQUIRE(t[1][1][0].has_value());
    CHECK(t[1][1][0].value() == 6);

    t.clear();
    CHECK(t.size() == 0);
    CHECK_FALSE(t.has_value());
    CHECK_THROWS_AS(t.at(0), std::out_of_range);
    CHECK_THROWS_AS(t.at(1), std::out_of_range);
}

TEST_CASE("Trie non-const traverser, basics") {
    trie<char, int> t;
    trie<char, int>::traverser<> i = t.traverser_begin();
    trie<char, int>::traverser<> end = t.traverser_end();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.path_string() == "");
    CHECK(i->size() == 0);
    i = end;
    CHECK(i == end);
    CHECK(decltype(i)() == decltype(i)());
}

TEST_CASE("Trie const traverser, basics") {
    const trie<char, int> t;
    trie<char, int>::const_traverser<> i = t.traverser_begin();
    trie<char, int>::const_traverser<> end = t.traverser_end();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.path_string() == "");
    CHECK(i->size() == 0);
    i = end;
    CHECK(i == end);
    CHECK(decltype(i)() == decltype(i)());
}

TEST_CASE("Trie traverser, conversion") {
    using V = std::vector<char>;

    trie<char, int> t;
    trie<char, int>::traverser<V> ib1 = t.traverser_begin<V>();
    trie<char, int>::traverser<V> ie1 = t.traverser_end<V>();
    trie<char, int>::const_traverser<V> ib2 = t.traverser_begin<V>();
    trie<char, int>::const_traverser<V> ie2 = t.traverser_end<V>();
    trie<char, int>::const_traverser<V> ib3 = ib1;
    trie<char, int>::const_traverser<V> ie3 = ie1;
    trie<char, int>::const_traverser<V> ib4 = t.const_traverser_begin<V>();
    trie<char, int>::const_traverser<V> ie4 = t.const_traverser_end<V>();

    const trie<char, int> ct;
    trie<char, int>::const_traverser<V> cib1 = t.traverser_begin<V>();
    trie<char, int>::const_traverser<V> cie1 = t.traverser_end<V>();
    trie<char, int>::const_traverser<V> cib2 = t.const_traverser_begin<V>();
    trie<char, int>::const_traverser<V> cie2 = t.const_traverser_end<V>();
}

TEST_CASE("Trie non-const traverser, forward, whole tree") {
    trie<char, int> t;
    t['a'].emplace_value(2);
    t['a']['a'];
    t['a']['b']['a'].emplace_value(4);
    t['b']['a'].emplace_value(5);
    t['b']['b']['a'];

    trie<char, int>::traverser<> i = t.traverser_begin();
    trie<char, int>::traverser<> end = t.traverser_end();
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.path_string() == "aa");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.path_string() == "aba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']));
    CHECK(i.path_string() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['a']));
    CHECK(i.path_string() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']));
    CHECK(i.path_string() == "bb");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']['a']));
    CHECK(i.path_string() == "bba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie const traverser, forward, whole tree") {
    trie<char, int> t;
    t['a'].emplace_value(2);
    t['a']['a'];
    t['a']['b']['a'].emplace_value(4);
    t['b']['a'].emplace_value(5);
    t['b']['b']['a'];

    trie<char, int>::const_traverser<> i = t.traverser_begin();
    trie<char, int>::const_traverser<> end = t.traverser_end();
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.path_string() == "aa");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.path_string() == "aba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']));
    CHECK(i.path_string() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['a']));
    CHECK(i.path_string() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']));
    CHECK(i.path_string() == "bb");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['b']['b']['a']));
    CHECK(i.path_string() == "bba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie non-const traverser, forward, subtree") {
    trie<char, int> t;
    t['a'].emplace_value(2);
    t['a']['a'];
    t['a']['b']['a'].emplace_value(4);
    t['b']['a'].emplace_value(5);
    t['b']['b']['a'];

    trie<char, int>::traverser<> i = t['a'].traverser_begin();
    trie<char, int>::traverser<> end = t['a'].traverser_end();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.path_string() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.path_string() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie const traverser, forward, subtree") {
    trie<char, int> t;
    t['a'].emplace_value(2);
    t['a']['a'];
    t['a']['b']['a'].emplace_value(4);
    t['b']['a'].emplace_value(5);
    t['b']['b']['a'];

    trie<char, int>::const_traverser<> i = t['a'].traverser_begin();
    trie<char, int>::const_traverser<> end = t['a'].traverser_end();
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['a']));
    CHECK(i.path_string() == "a");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "b");
    CHECK(std::addressof(i) == std::addressof(++i));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['a']));
    CHECK(i.path_string() == "ba");
    CHECK(std::addressof(i) == std::addressof(++i));
    CHECK(i == end);
}

TEST_CASE("Trie non-const traverser, forward, postfix operator") {
    trie<char, int> t;
    t.emplace_value(0);
    t['a'].emplace_value(1);

    trie<char, int>::traverser<> i1 = t.traverser_begin();
    trie<char, int>::traverser<> end = t.traverser_end();
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t));
    CHECK(i1.path_string() == "");

    trie<char, int>::traverser<> i2 = i1++;
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t['a']));
    CHECK(i1.path_string() == "a");
    CHECK(i2 != end);
    CHECK(std::addressof(*i2) == std::addressof(t));
    CHECK(i2.path_string() == "");

    trie<char, int>::traverser<> i3 = i1++;
    CHECK(i1 == end);
    CHECK(i3 != end);
    CHECK(std::addressof(*i3) == std::addressof(t['a']));
    CHECK(i3.path_string() == "a");
}

TEST_CASE("Trie const traverser, forward, postfix operator") {
    trie<char, int> t;
    t.emplace_value(0);
    t['a'].emplace_value(1);

    trie<char, int>::const_traverser<> i1 = t.traverser_begin();
    trie<char, int>::const_traverser<> end = t.traverser_end();
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t));
    CHECK(i1.path_string() == "");

    trie<char, int>::const_traverser<> i2 = i1++;
    CHECK(i1 != end);
    CHECK(std::addressof(*i1) == std::addressof(t['a']));
    CHECK(i1.path_string() == "a");
    CHECK(i2 != end);
    CHECK(std::addressof(*i2) == std::addressof(t));
    CHECK(i2.path_string() == "");

    trie<char, int>::const_traverser<> i3 = i1++;
    CHECK(i1 == end);
    CHECK(i3 != end);
    CHECK(std::addressof(*i3) == std::addressof(t['a']));
    CHECK(i3.path_string() == "a");
}

TEST_CASE("Trie non-const traverser, down, key digit") {
    trie<char, int> t;
    t['a']['b'];

    trie<char, int>::traverser<> i = t.traverser_begin();
    trie<char, int>::traverser<> end = t.traverser_end();
    CHECK_FALSE(i.down('z'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.path_string() == "");
    CHECK(i.down('a'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "a");
    CHECK_FALSE(i.down('y'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "a");
    CHECK(i.down('b'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
}

TEST_CASE("Trie const traverser, down, key digit") {
    trie<char, int> t;
    t['a']['b'];

    trie<char, int>::const_traverser<> i = t.traverser_begin();
    trie<char, int>::const_traverser<> end = t.traverser_end();
    CHECK_FALSE(i.down('z'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.path_string() == "");
    CHECK(i.down('a'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "a");
    CHECK_FALSE(i.down('y'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "a");
    CHECK(i.down('b'));
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
}

TEST_CASE("Trie non-const traverser, down, key iterator") {
    trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    std::string aba = "aba", cde = "cde";

    trie<char, int>::traverser<> i = t.traverser_begin();
    trie<char, int>::traverser<> end = t.traverser_end();
    CHECK(i.down(aba.begin(), aba.end()) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(i.down(cde.begin(), cde.end()) == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.path_string() == "abcde");
}

TEST_CASE("Trie const traverser, down, key iterator") {
    trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    std::string aba = "aba", cde = "cde";

    trie<char, int>::const_traverser<> i = t.traverser_begin();
    trie<char, int>::const_traverser<> end = t.traverser_end();
    CHECK(i.down(aba.begin(), aba.end()) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(i.down(cde.begin(), cde.end()) == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.path_string() == "abcde");
}

TEST_CASE("Trie non-const traverser, down, key string") {
    trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    trie<char, int>::traverser<> i = t.traverser_begin();
    trie<char, int>::traverser<> end = t.traverser_end();
    CHECK(i.down("aba") == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(i.down("cde") == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.path_string() == "abcde");
}

TEST_CASE("Trie const traverser, down, key string") {
    trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    trie<char, int>::const_traverser<> i = t.traverser_begin();
    trie<char, int>::const_traverser<> end = t.traverser_end();
    CHECK(i.down("aba") == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(i.down("cde") == 3);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']['e']));
    CHECK(i.path_string() == "abcde");
}

TEST_CASE("Trie non-const traverser, up, whole tree") {
    trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    trie<char, int>::traverser<> i = t.traverser_begin();
    trie<char, int>::traverser<> end = t.traverser_end();
    i.down("abcde");
    CHECK(i.up() == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']));
    CHECK(i.path_string() == "abcd");
    CHECK(i.up(2) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(i.up(3) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.path_string() == "");
}

TEST_CASE("Trie const traverser, up, whole tree") {
    trie<char, int> t;
    t['a']['b']['c']['d']['e'];

    trie<char, int>::const_traverser<> i = t.traverser_begin();
    trie<char, int>::const_traverser<> end = t.traverser_end();
    i.down("abcde");
    CHECK(i.up() == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']['c']['d']));
    CHECK(i.path_string() == "abcd");
    CHECK(i.up(2) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']['b']));
    CHECK(i.path_string() == "ab");
    CHECK(i.up(3) == 2);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t));
    CHECK(i.path_string() == "");
}

TEST_CASE("Trie non-const traverser, up, subtree") {
    trie<char, int> t;
    t['a']['b'];

    trie<char, int>::traverser<> i = t['a'].traverser_begin();
    trie<char, int>::traverser<> end = t['a'].traverser_end();
    i.down('b');
    CHECK(i.up(2) == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "");
}

TEST_CASE("Trie const traverser, up, subtree") {
    trie<char, int> t;
    t['a']['b'];

    trie<char, int>::const_traverser<> i = t['a'].traverser_begin();
    trie<char, int>::const_traverser<> end = t['a'].traverser_end();
    i.down('b');
    CHECK(i.up(2) == 1);
    REQUIRE(i != end);
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "");
}

TEST_CASE("Trie non-const iterator, non-constness of value") {
    trie<char, int> t;
    trie<char, int>::traverser<> i = t.traverser_begin();
    i->emplace_child('a');
    ++i;
    CHECK(std::addressof(*i) == std::addressof(t['a']));
    CHECK(i.path_string() == "a");
}

} // namespace

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
