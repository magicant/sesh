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

#ifndef INCLUDED_common_reference_hh
#define INCLUDED_common_reference_hh

#include "buildconfig.h"

#include <functional>

namespace sesh {
namespace common {

/**
 * The reference class template is an extension of std::reference_wrapper that
 * is truly movable.
 *
 * The C++11 standard is too strict as to when a implicitly-declared move
 * constructor and assignment operator can be implicitly defined. They are
 * implicitly deleted  if the class's sub-object does not have a move
 * constructor or assignment operator and is not trivially copyable. The
 * standard does not require the reference_wrapper class template to be
 * trivially copyable nor have a move constructor or assignment operator. As a
 * result, a class that is directly derived from or has a non-static member of
 * reference_wrapper may have the implicitly declared move constructor and
 * assignment operator deleted. This class template works around the issue by
 * explicitly declaring and defining all the constructors and assignment
 * operators.
 */
template<typename T>
class reference : public std::reference_wrapper<T> {

private:

    using base = std::reference_wrapper<T>;

public:

    using base::base;

    reference(const reference &r) noexcept : base(r) { }
    reference(reference &&r) noexcept : base(r) { }

    reference(const base &r) noexcept : base(r) { }

    reference &operator=(const reference &r) noexcept {
        base::operator=(r);
        return *this;
    }
    reference &operator=(reference &&r) noexcept {
        base::operator=(r);
        return *this;
    }

}; // template<typename T> class reference

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_reference_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
