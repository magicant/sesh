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

#include <utility>
#include "common/String.hh"
#include "language/parser/EofEnvironment.hh"
#include "language/parser/EnvironmentTestHelper.hh"
#include "language/parser/LineContinuationEnvironment.hh"
#include "language/parser/Operator.hh"
#include "language/parser/SpecificOperatorParser.hh"

namespace sesh {
namespace language {
namespace parser {

using sesh::common::String;
using sesh::language::parser::EofEnvironment;
using sesh::language::parser::LineContinuationEnvironment;
using sesh::language::parser::Operator;
using sesh::language::parser::SourceTestEnvironment;
using sesh::language::parser::SpecificOperatorParser;

class SpecificOperatorParserTestEnvironment :
        public SourceTestEnvironment,
        public EofEnvironment,
        public LineContinuationEnvironment {
};

TEST_CASE("Specific operator parser, construction") {
    SpecificOperatorParserTestEnvironment e;
    SpecificOperatorParser p1(e, Operator::operatorAnd());
    SpecificOperatorParser p2(p1);
    p1 = p2;
}

TEST_CASE("Specific operator parser, accepted operator") {
    SpecificOperatorParserTestEnvironment e;
    SpecificOperatorParser p(e, Operator::operatorAnd());
    e.appendSource(String(Operator::AND));
    e.setIsEof();
    REQUIRE(p.parse().hasValue());
    CHECK(p.parse().value() == Operator::operatorAnd());
    CHECK(e.position() == e.length());
}

TEST_CASE("Specific operator parser, unaccepted operator") {
    SpecificOperatorParserTestEnvironment e;
    SpecificOperatorParser p(e, Operator::operatorAnd());
    e.appendSource(String(Operator::AND_AND));
    e.setIsEof();
    CHECK_FALSE(p.parse().hasValue());
}

TEST_CASE("Specific operator parser, not a operator") {
    SpecificOperatorParserTestEnvironment e;
    SpecificOperatorParser p(e, Operator::operatorAnd());
    e.appendSource(L("-"));
    CHECK_FALSE(p.parse().hasValue());
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
