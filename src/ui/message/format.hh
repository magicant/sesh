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

#ifndef INCLUDED_ui_message_format_hh
#define INCLUDED_ui_message_format_hh

#include "buildconfig.h"

#include <functional>
#include "boost/format.hpp"
#include "common/xstring.hh"

namespace sesh {
namespace ui {
namespace message {

/**
 * A format is a pair of a format string and a value list that can be used to
 * form a complete message to be presented to the user.
 *
 * The format string must obey the syntax of boost::format, which is similar to
 * but different from that of printf. This class template provides a
 * functionality similar to that of boost::format, but the former defers
 * application of the format string to argument values so that the format
 * string can be recalled and altered at any time while keeping already
 * provided argument values intact.
 *
 * The template parameters specify the types of argument values that have to be
 * supplied to form a complete message. A message of type <code>message&lt;int,
 * double></code>, for example, needs an int and double value. The
 * <code>message&lt;></code> type represents a complete message.
 *
 * @tparam Arg Types of arguments required to complete the message.
 */
template<typename... Arg>
class format;

/**
 * Complete message.
 *
 * Implementation note: This template specialization may be used as a base
 * class of an incomplete message class. Therefore, this class may also
 * represent an incomplete message. However, this trick is hidden behind the
 * public API of this class.
 */
template<>
class format<> {

public:

    using string_type = common::xstring;
    using char_type = string_type::value_type;
    using char_traits = string_type::traits_type;
    using allocator_type = string_type::allocator_type;
    using format_type =
            boost::basic_format<char_type, char_traits, allocator_type>;

protected:

    using argument_feeder_type = std::function<format_type &(format_type &)>;

private:

    string_type m_format_string;

    /** Never null. */
    argument_feeder_type m_feed_arguments;

public:

    /**
     * Constructs a message object with the specified format string.
     *
     * The validity of the format string is not checked in this constructor, so
     * the caller must make sure to match the argument types required by the
     * format string with the class template parameters.
     */
    explicit format(string_type &&);

    /** @see format<>::format(string_type &&) */
    explicit format(const string_type &);

    /** @see format<>::format(string_type &&) */
    explicit format(const char_type * = nullptr);

    /** Converts the complete message to a string. */
    string_type to_string() const;

    /**
     * The format string for this message.
     *
     * The validity of the format string is not checked when you modify it,
     * since this function returns a direct reference to the string object. The
     * caller is responsible to keep the format valid.
     */
    string_type &format_string() noexcept {
        return m_format_string;
    }
    const string_type &format_string() const noexcept {
        return m_format_string;
    }

protected:

    argument_feeder_type &argument_feeder() noexcept {
        return m_feed_arguments;
    }
    const argument_feeder_type &argument_feeder() const noexcept {
        return m_feed_arguments;
    }

}; // template<> class format<>

/**
 * Incomplete message.
 *
 * Implementation note: The recursive derivation is a trick to implement the %
 * operator without copying the format object.
 */
template<typename Head, typename... Tail>
class format<Head, Tail...> : private format<Tail...> {

public:

    using typename format<Tail...>::string_type;
    using typename format<Tail...>::char_type;
    using typename format<Tail...>::char_traits;
    using typename format<Tail...>::allocator_type;
    using typename format<Tail...>::format_type;

    using format<Tail...>::format;

protected:

    using format<Tail...>::argument_feeder;

public:

    /** Binds an argument value to this format object. */
    format<Tail...> &&operator%(const Head &value) && {
        auto &old = argument_feeder();
        argument_feeder() = [old, value](format_type &f) -> format_type & {
            return old(f) % value;
        };
        return std::move(*this);
    }

}; // template<typename Head, typename... Tail> class format<Head, Tail...>

} // namespace message
} // namespace ui
} // namespace sesh

#endif // #ifndef INCLUDED_ui_message_format_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
