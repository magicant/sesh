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
#include "CommandParserBase.hh"

#include <memory>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/token.hh"
#include "language/syntax/Command.hh"

using sesh::common::Char;
using sesh::common::String;
using sesh::language::parser::Environment;
using sesh::language::parser::token::LEFT_PARENTHESIS;
using sesh::language::syntax::Command;

namespace sesh {
namespace language {
namespace parser {

CommandParserBase::CommandParserBase(Environment &e) noexcept :
        Parser(), ParserBase(e), mActualParser(nullptr) { }

namespace {

bool isValidCommandSymbol(const String &symbol) {
    if (symbol.empty())
        return true;
    switch (symbol[0u]) {
    case L('&'):
    case L('|'):
    case L(';'):
    case L(')'):
        return false;
    default:
        return true;
    }
}

} // namespace

void CommandParserBase::createActualParserFromKeyword() {
    const String &keyword = peekKeyword(environment());
    if (keyword.empty())
        return;
    // TODO support compound commands and function definition command
    // TODO report error for invalid keyword
}

void CommandParserBase::createActualParser() {
    const String &symbol = peekSymbol(environment());
    if (!isValidCommandSymbol(symbol)) {
        // TODO report error for invalid symbol
        return;
    }
    if (symbol == LEFT_PARENTHESIS) {
        return; // TODO create a grouping parser
    }

    if (symbol.empty()) {
        createActualParserFromKeyword();
        if (mActualParser != nullptr)
            return;
    }

    mActualParser = createSimpleCommandParser();
}

std::unique_ptr<Command> CommandParserBase::parse() {
    if (mActualParser == nullptr) {
        createActualParser();
        if (mActualParser == nullptr)
            return nullptr;
    }
    return mActualParser->parse();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
