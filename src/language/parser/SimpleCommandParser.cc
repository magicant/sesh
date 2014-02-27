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
#include "SimpleCommandParser.hh"

#include <cassert>
#include <memory>
#include <utility>
#include "common/EnumSet.hh"
#include "common/ErrorLevel.hh"
#include "common/Maybe.hh"
#include "common/Message.hh"
#include "common/String.hh"
#include "i18n/M.h"
#include "language/parser/Environment.hh"
#include "language/parser/Keyword.hh"
#include "language/parser/Token.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/SimpleCommand.hh"
#include "language/syntax/Word.hh"

using sesh::common::EnumSet;
using sesh::common::ErrorLevel;
using sesh::common::Maybe;
using sesh::common::Message;
using sesh::common::String;
using sesh::common::enumSetOf;
using sesh::language::syntax::Assignment;
using sesh::language::syntax::SimpleCommand;
using sesh::language::syntax::Word;

namespace sesh {
namespace language {
namespace parser {

SimpleCommandParser::SimpleCommandParser(
        Environment &e, TokenParserPointer &&tokenParser) noexcept :
        NormalParser(e),
        mCommand(),
        mTokenParser(std::move(tokenParser)) {
    assert(mTokenParser != nullptr);
}

class SimpleCommandParser::TokenParserResultAcceptor {

private:

    SimpleCommandParser &mParser;

public:

    using Result = void;

    explicit TokenParserResultAcceptor(SimpleCommandParser &p) noexcept :
            mParser(p) { }

    void operator()(std::unique_ptr<Word> &w) {
        mParser.mCommand->words().push_back(std::move(w));
    }

    void operator()(std::unique_ptr<Assignment> &a) {
        mParser.mCommand->assignments().push_back(std::move(a));
    }

    void operator()(Keyword &k) {
        auto message = L(M("keyword `%1%' cannot be used as command name"));
        mParser.environment().addDiagnosticMessage(
                mParser.mTokenParser->begin(),
                Message<String>(message) % k,
                ErrorLevel::ERROR);
    }

}; // class SimpleCommandParser::TokenParserResultAcceptor

auto SimpleCommandParser::nextTokenTypes() const -> TokenTypeSet {
    TokenTypeSet types = enumSetOf(TokenType::WORD);
    // TODO alias, redirection
    if (mCommand == nullptr || mCommand->words().empty())
        types.set(TokenType::ASSIGNMENT);
    return types;
}

void SimpleCommandParser::parseImpl() {
    while (Maybe<Token> &t = mTokenParser->parse()) {
        if (mCommand == nullptr)
            mCommand.reset(new SimpleCommand);
        t->apply(TokenParserResultAcceptor(*this));
        mTokenParser->reset(nextTokenTypes());
    }

    if (mCommand != nullptr)
        result().emplace(mCommand.release());
}

void SimpleCommandParser::resetImpl() noexcept {
    mCommand = nullptr;
    mTokenParser->reset(EnumSet<TokenType>().set());
    NormalParser::resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
