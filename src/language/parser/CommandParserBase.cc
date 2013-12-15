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
#include "common/ErrorLevel.hh"
#include "common/Message.hh"
#include "common/String.hh"
#include "i18n/M.h"
#include "language/parser/Environment.hh"
#include "language/parser/token.hh"
#include "language/syntax/Command.hh"

using sesh::common::Char;
using sesh::common::ErrorLevel;
using sesh::common::Message;
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
        return false;
    switch (symbol[0]) {
    case L('&'):
    case L('|'):
    case L(';'):
    case L(')'):
        return false;
    default:
        return true;
    }
}

void reportInvalidSymbolError(Environment &e, const String &symbol) {
    auto message = L(M("unexpected symbol `%1%'; a command was expected"));
    e.addDiagnosticMessage(
            e.current(), Message<String>(message) % symbol, ErrorLevel::ERROR);
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
    if (symbol.empty()) {
        createActualParserFromKeyword();
        if (mActualParser != nullptr)
            return;
    } else {
        if (!isValidCommandSymbol(symbol)) {
            reportInvalidSymbolError(environment(), symbol);
            return;
        }
        if (symbol == LEFT_PARENTHESIS) {
            return; // TODO create a grouping parser
        }
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
