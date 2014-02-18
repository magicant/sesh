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
#include "PipelineParserImpl.hh"

#include <utility>
#include "common/EnumSet.hh"
#include "language/parser/CommandParserImpl.hh"
#include "language/parser/Token.hh"
#include "language/parser/TokenParserImpl.hh"

using sesh::common::EnumSet;
using sesh::language::parser::CommandParserImpl;
using sesh::language::parser::TokenType;
using sesh::language::parser::TokenParserImpl;

namespace sesh {
namespace language {
namespace parser {

auto PipelineParserImpl::createTokenParser() const -> TokenParserPointer {
    return TokenParserPointer(new TokenParserImpl(
            environment(), EnumSet<TokenType>().set()));
}

auto PipelineParserImpl::createCommandParser(TokenParserPointer tp) const
        -> CommandParserPointer {
    return CommandParserPointer(new CommandParserImpl(
            environment(), std::move(tp)));
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
