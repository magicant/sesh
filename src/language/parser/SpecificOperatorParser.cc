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
#include "SpecificOperatorParser.hh"

#include "language/parser/Environment.hh"
#include "language/parser/Operator.hh"
#include "language/parser/Parser.hh"

using sesh::language::parser::Environment;
using sesh::language::parser::Operator;
using sesh::language::parser::Parser;
using sesh::language::parser::createOperatorParser;

namespace sesh {
namespace language {
namespace parser {

SpecificOperatorParser::SpecificOperatorParser(
        Environment &e, Operator o, LineContinuationTreatment lct) :
        Parser(e),
        mAcceptedOperator(o),
        mAnyOperatorParser(createOperatorParser(e, lct)) { }

void SpecificOperatorParser::parseImpl() {
    const Operator *o = mAnyOperatorParser.parse();
    if (o != nullptr && *o == mAcceptedOperator)
        result() = o;
}

void SpecificOperatorParser::resetImpl() noexcept {
    mAnyOperatorParser.reset();
    Parser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
