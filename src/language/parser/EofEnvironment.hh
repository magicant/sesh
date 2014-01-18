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

#ifndef INCLUDED_language_parser_EofEnvironment_hh
#define INCLUDED_language_parser_EofEnvironment_hh

#include "buildconfig.h"

#include "language/parser/SourceEnvironment.hh"

namespace sesh {
namespace language {
namespace parser {

/** An environment that supports {@link #isEof}. */
class EofEnvironment : public virtual SourceEnvironment {

private:

    bool mIsEof = false;

public:

    bool isEof() const noexcept final override { return mIsEof; }

    void setIsEof(bool isEof = true) noexcept { mIsEof = isEof; }

}; // class EofEnvironment

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_EofEnvironment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
