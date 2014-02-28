/* Copyright (C) 2013-2014 WATANABE Yuki
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
#include "CommandParser.hh"

#include "common/Maybe.hh"
#include "language/parser/Environment.hh"

using sesh::common::Maybe;
using sesh::language::parser::Environment;

namespace sesh {
namespace language {
namespace parser {

CommandParser::CommandParser(Environment &e, TokenParserPointer &&p) noexcept :
        Parser(e), mTokenParser(std::move(p)), mActualParser() { }

void CommandParser::prepareActualParser() {
    if (mActualParser != nullptr)
        return;

    if (mTokenParser == nullptr)
        mTokenParser = createTokenParser();
    // TODO check if the token is a keyword to support compound commands
    mActualParser = createSimpleCommandParser(std::move(mTokenParser));
}

void CommandParser::parseImpl() {
    prepareActualParser();
    mActualParser->parse();
}

auto CommandParser::result() -> Maybe<CommandPointer> & {
    return mActualParser->parse();
}

void CommandParser::resetImpl() noexcept {
    mTokenParser = nullptr;
    mActualParser = nullptr;
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
