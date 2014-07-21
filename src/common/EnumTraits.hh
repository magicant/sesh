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

#ifndef INCLUDED_common_EnumTraits_hh
#define INCLUDED_common_EnumTraits_hh

#include "buildconfig.h"

namespace sesh {
namespace common {

/**
 * The enum traits template is a placeholder where characteristics of an
 * enumeration are defined.
 *
 * A specialization of this template must have the following public members:
@code
template<> class EnumTraits<SomeEnumType> {
public:
    // The maximum value of the SomeEnumType enumerators.
    constexpr static SomeEnumType max = ...;
};
@endcode
 *
 * The non-specialized version of this template is not defined.
 *
 * @tparam E An enumeration type.
 */
template<typename E>
class EnumTraits;

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_EnumTraits_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
