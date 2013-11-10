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
#include "CommentSkipper.hh"

#include <functional>
#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NeedMoreSource.hh"
#include "language/parser/Predicate.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::language::parser::NeedMoreSource;

namespace sesh {
namespace language {
namespace parser {

CommentSkipper::CommentSkipper(Environment &e, Skipper &&blankSkipper) :
        Parser(e),
        mState(State::BLANK),
        mBlankSkipper(std::move(blankSkipper)),
        mCommentSkipper(e, isNewline, LineContinuationTreatment::LITERAL) { }

void CommentSkipper::skip() {
    for (;;) {
        switch (mState) {
        case State::BLANK:
            mBlankSkipper.skip();
            if (!CharTraits::eq_int_type(
                        currentCharInt(),
                        CharTraits::to_int_type(L('#'))))
                return;
            mState = State::COMMENT;
            // fall-through
        case State::COMMENT:
            mCommentSkipper.skip();
            mState = State::BLANK;
            break;
        }
    }
}

CommentSkipper normalCommentSkipper(Environment &e) {
    return CommentSkipper(e, Skipper(e, std::not2(Predicate<Char>(isBlank))));
}

CommentSkipper whitespaceSkipper(Environment &e) {
    return CommentSkipper(e,
            Skipper(e, std::not2(Predicate<Char>(isBlankOrNewline))));
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
