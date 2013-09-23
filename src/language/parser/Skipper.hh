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

#ifndef INCLUDED_language_parser_Skipper_hh
#define INCLUDED_language_parser_Skipper_hh

#include "buildconfig.h"

#include "common/Char.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"

namespace sesh {
namespace language {
namespace parser {

class Environment;

/** Skipper is a parser that skips blanks or other unmeaningful characters. */
class Skipper : public Parser {

private:

    Predicate<common::Char> mIsStopper;
    LineContinuationTreatment mLineContinuationTreatment;

public:

    /**
     * Constructs a new skipper that works in the specified environment. The
     * skipper increments the environment's iterator up to the first character
     * for which the specified predicate is true (or to the end of source if
     * all the remaining characters are skipped).
     */
    Skipper(
            Environment &,
            Predicate<common::Char> &&isStopper,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

    Skipper(const Skipper &) = default;
    Skipper(Skipper &&) = default;
    Skipper &operator=(const Skipper &) = delete;
    Skipper &operator=(Skipper &&) = delete;
    ~Skipper() = default;

private:

    /** @throws NeedMoreSource */
    void removeLineContinuation();
    /** @throws NeedMoreSource */
    bool currentIsStopper() const;

public:

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

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Skipper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
