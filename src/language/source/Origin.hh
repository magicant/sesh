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

#ifndef INCLUDED_language_source_Origin_hh
#define INCLUDED_language_source_Origin_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include "common/Message.hh"

namespace sesh {
namespace language {
namespace source {

/** This abstract class identifies the origin of a source. */
class Origin {

public:

    Origin() = default;
    Origin(const Origin &) = default;
    Origin(Origin &&) = default;
    Origin &operator=(const Origin &) = default;
    Origin &operator=(Origin &&) = default;
    virtual ~Origin() = default;

    /**
     * Returns the name of this origin. It should be a pathname or a
     * (localizable) short description surrounded by a pair of triangle
     * brackets.
     */
    virtual common::Message<> name() const = 0;

    /**
     * Describes this origin.
     *
     * Examples of possible return values include:
     *  - in alias substitution ll='ls -l'
     *  - in the file included by the dot built-in
     */
    virtual common::Message<> description() const = 0;

}; // class Origin

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_Origin_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
