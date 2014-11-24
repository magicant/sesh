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

#ifndef INCLUDED_common_type_tag_set_hh
#define INCLUDED_common_type_tag_set_hh

#include "buildconfig.h"

#include <cstddef>
#include <initializer_list>
#include "common/enum_set.hh"
#include "common/enum_traits.hh"
#include "common/type_tag.hh"

namespace sesh {
namespace common {

/**
 * Type tag set is an enum set of type tags.
 */
template<typename... T>
class type_tag_set : public enum_set<typename type_tag<T...>::value_type> {

public:

    using tag_type = type_tag<T...>;
    using set_type = enum_set<typename tag_type::value_type>;

    using set_type::set_type;

    type_tag_set() = default;

    type_tag_set(std::initializer_list<tag_type> tags) : set_type() {
        for (tag_type tag : tags)
            set(tag);
    }

    constexpr static auto to_enum_value(tag_type t) noexcept
            -> typename tag_type::value_type {
        return t;
    }

    using set_type::operator[];

    constexpr bool operator[](tag_type t) const {
        return (*this)[to_enum_value(t)];
    }
    typename set_type::reference operator[](tag_type t) {
        return (*this)[to_enum_value(t)];
    }

    using set_type::set;

    type_tag_set &set(tag_type t, bool value = true) {
        this->set(to_enum_value(t), value);
        return *this;
    }

    using set_type::reset;

    type_tag_set &reset(tag_type t) {
        return set(t, false);
    }

    using set_type::flip;

    type_tag_set &flip(tag_type t) {
        this->flip(to_enum_value(t));
        return *this;
    }

}; // template<typename... T> class type_tag_set

template<>
class enum_traits<type_tag_impl::type_tag_value<0>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<0>::type max =
            (type_tag_impl::type_tag_value<0>::type) 0;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<1>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<1>::type max =
            type_tag_impl::type_tag_value<1>::type::v0;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<2>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<2>::type max =
            type_tag_impl::type_tag_value<2>::type::v1;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<3>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<3>::type max =
            type_tag_impl::type_tag_value<3>::type::v2;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<4>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<4>::type max =
            type_tag_impl::type_tag_value<4>::type::v3;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<5>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<5>::type max =
            type_tag_impl::type_tag_value<5>::type::v4;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<6>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<6>::type max =
            type_tag_impl::type_tag_value<6>::type::v5;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<7>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<7>::type max =
            type_tag_impl::type_tag_value<7>::type::v6;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<8>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<8>::type max =
            type_tag_impl::type_tag_value<8>::type::v7;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<9>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<9>::type max =
            type_tag_impl::type_tag_value<9>::type::v8;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<10>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<10>::type max =
            type_tag_impl::type_tag_value<10>::type::v9;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<11>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<11>::type max =
            type_tag_impl::type_tag_value<11>::type::v10;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<12>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<12>::type max =
            type_tag_impl::type_tag_value<12>::type::v11;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<13>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<13>::type max =
            type_tag_impl::type_tag_value<13>::type::v12;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<14>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<14>::type max =
            type_tag_impl::type_tag_value<14>::type::v13;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<15>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<15>::type max =
            type_tag_impl::type_tag_value<15>::type::v14;
};

template<>
class enum_traits<type_tag_impl::type_tag_value<16>::type> {
public:
    constexpr static type_tag_impl::type_tag_value<16>::type max =
            type_tag_impl::type_tag_value<16>::type::v15;
};

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_type_tag_set_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
