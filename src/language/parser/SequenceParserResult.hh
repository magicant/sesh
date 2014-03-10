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

#ifndef INCLUDED_language_parser_SequenceParserResult_hh
#define INCLUDED_language_parser_SequenceParserResult_hh

#include "buildconfig.h"

#include <utility>
#include "common/Maybe.hh"
#include "language/parser/PositionedKeyword.hh"
#include "language/syntax/Sequence.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * The result type of the sequence parser. It is a pair of a sequence and an
 * optional keyword. Iff the sequence is followed by one of the following
 * keywords, the maybe object contains the keyword: do, done, elif, else, esac,
 * fi, then, }.
 */
using SequenceParserResult =
        std::pair<syntax::Sequence, common::Maybe<PositionedKeyword>>;

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SequenceParserResult_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
