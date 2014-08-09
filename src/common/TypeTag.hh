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

#ifndef INCLUDED_common_TypeTag_hh
#define INCLUDED_common_TypeTag_hh

#include "buildconfig.h"

namespace sesh {
namespace common {

/**
 * The type tag is an enumeration-like type to render distinct values which
 * each corresponds to one of the template parameter types.
 *
 * Note that the specialization {@code TypeTag<>} cannot be instantiated since
 * it cannot represent any type.
 */
template<typename...>
class TypeTag;

/**
 * An object of this TypeTag class template specialization always represents
 * the template parameter type T.
 */
template<typename T>
class TypeTag<T> { };

/** Single-typed type tag objects always compare equal. */
template<typename T>
constexpr bool operator==(TypeTag<T>, TypeTag<T>) noexcept {
    return true;
}

/** Single-typed type tag objects always compare equal. */
template<typename T>
constexpr bool operator<(TypeTag<T>, TypeTag<T>) noexcept {
    return false;
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_TypeTag_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
