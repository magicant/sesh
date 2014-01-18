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
#include "LineContinuationEnvironment.hh"

#include <utility>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/EnvironmentHelper.hh"
#include "language/source/LineContinuedSource.hh"
#include "language/source/Source.hh"

using sesh::common::Char;
using sesh::common::CharTraits;
using sesh::language::source::LineContinuedSource;
using sesh::language::source::Source;

namespace sesh {
namespace language {
namespace parser {

namespace {

constexpr bool equal(CharTraits::int_type l, Char r) noexcept {
    return CharTraits::eq_int_type(l, CharTraits::to_int_type(r));
}

} // namespace

bool LineContinuationEnvironment::removeLineContinuation(Size position) {
    if (!equal(charIntAt(*this, position), L('\\')))
        return false;

    if (!equal(charIntAt(*this, position + 1), L('\n')))
        return false;

    substituteSource([position](Source::Pointer &&orig) -> Source::Pointer {
        return Source::Pointer(
                new LineContinuedSource(std::move(orig), position));
    });
    return true;
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
