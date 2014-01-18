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
#include "ParserBase.hh"

#include "language/parser/EnvironmentHelper.hh"
#include "language/parser/Reparse.hh"

namespace sesh {
namespace language {
namespace parser {

ParserBase::ParserBase(Environment &e) noexcept :
        mEnvironment(e) { }

auto ParserBase::currentCharInt() const -> CharInt {
    return charIntAt(environment(), environment().position());
}

void ParserBase::parseIfNotFinished() {
    try {
        switch (state()) {
        case State::UNSTARTED:
            mState = State::PARSING;
            mBegin = environment().position();
            // fall-through
        case State::PARSING:
            parseImpl();
            mState = State::FINISHED;
            if (!isSuccessful())
                environment().setPosition(begin());
            // fall-through
        case State::FINISHED:
            break;
        }
    } catch (const Reparse &) {
        environment().setPosition(begin());
        reset();
        throw;
    }
}

void ParserBase::reset() noexcept {
    mState = State::UNSTARTED;
    resetImpl();
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
