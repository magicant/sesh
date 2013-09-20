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

#ifndef INCLUDED_language_parser_CommentSkipper_hh
#define INCLUDED_language_parser_CommentSkipper_hh

#include "buildconfig.h"

#include "common/Char.hh"
#include "language/parser/Environment.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Skipper.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * An extended skipper that skips comments as well as blanks.
 *
 * A comment starts with a hash sign ('#') and continues up to (but not
 * including) the first newline.
 */
class CommentSkipper : public Parser {

private:

    enum class State {
        BLANK,
        COMMENT,
    } mState;

    Skipper mBlankSkipper;

    /** Skips up to a newline. */
    Skipper mCommentSkipper;

public:

    /**
     * The blank skipper defines blanks that are skipped in addition to
     * comments. It should not skip hash signs ('#') or comments are not
     * correctly recognized.
     */
    CommentSkipper(Environment &, Skipper &&blankSkipper);

    CommentSkipper(const CommentSkipper &) = default;
    CommentSkipper(CommentSkipper &&) = default;
    CommentSkipper &operator=(const CommentSkipper &) = delete;
    CommentSkipper &operator=(CommentSkipper &&) = delete;
    ~CommentSkipper() = default;

    /**
     * Increments the environment's iterator up to the character just after the
     * skipped characters.
     *
     * If more source is needed to find the first non-skipped character, this
     * function throws NeedMoreSource. In this case, the caller should set the
     * EOF flag or append to the source and then call this function again.
     *
     * @throws NeedMoreSource (see above)
     */
    void skip();

};

/** Creates a comment skipper that skips comments and blank characters. */
CommentSkipper normalCommentSkipper(Environment &);

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_CommentSkipper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
