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
#include "OperatorParser.hh"

#include <memory>
#include "language/parser/Operator.hh"

namespace sesh {
namespace language {
namespace parser {

OperatorParser createOperatorParser(
        Environment &e, LineContinuationTreatment lct) {
    return OperatorParser(
            e,
            std::shared_ptr<OperatorParser::Trie>(
                    &Operator::TRIE, [](OperatorParser::Trie *) { }),
            lct);
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */