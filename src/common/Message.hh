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

#ifndef INCLUDED_common_Message_hh
#define INCLUDED_common_Message_hh

#include "buildconfig.h"

#include <functional>
#include "boost/format.hpp"
#include "common/OutputStream.hh"
#include "common/String.hh"

namespace sesh {
namespace common {

/**
 * A message is a pair of a format string and a value list that can be used to
 * form a complete message to be presented to a human.
 *
 * The format string must obey the syntax of boost::format, which is similar to
 * but different from that of printf. This class template provides a
 * functionality similar to that of boost::format, but the former defers
 * application of the format string to argument values so that the format
 * string can be recalled and altered at any time while keeping already
 * provided argument values intact.
 *
 * The template parameters specify the types of argument values that have to be
 * supplied to form a complete message. A message of type <code>Message&lt;int,
 * double></code>, for example, needs an int and double value. The
 * <code>Message&lt;></code> type represents a complete message.
 *
 * @tparam Arg Types of arguments required to complete the message.
 */
template<typename... Arg>
class Message;

/**
 * Complete message.
 *
 * Implementation note: This template specialization may be used as a base
 * class of an incomplete message class. Therefore, this class may also
 * represent an incomplete message. However, this trick is hidden behind the
 * public API of this class.
 */
template<>
class Message<> {

public:

    using String = common::String;
    using Char = String::value_type;
    using CharTraits = String::traits_type;
    using Allocator = String::allocator_type;
    using Format = boost::basic_format<Char, CharTraits, Allocator>;

protected:

    using ArgumentFeeder = std::function<Format &(Format &)>;

private:

    String mFormatString;

    /** Never null. */
    ArgumentFeeder mFeedArguments;

public:

    /** @see Message<>::Message(String &&) */
    explicit Message(const Char * = nullptr);

    /** @see Message<>::Message(String &&) */
    explicit Message(const String &);

    /**
     * Constructs a message object with the specified format string.
     *
     * The validity of the format string is not checked in this constructor, so
     * the caller must make sure to match the argument types required by the
     * format string with the class template parameters.
     */
    explicit Message(String &&);

    /**
     * Returns a boost::format object.
     *
     * The remaining argument count of the return value matches the number of
     * template parameters of the Message template class. For a complete
     * message, it is zero. The number of expected, bound, and fed arguments of
     * the return value is unspecified.
     */
    Format toFormat() const;

    /** Converts the complete message to a string. */
    String toString() const;

    /**
     * The format string for this message.
     *
     * The validity of the format string is not checked when you modify it,
     * since this function returns a direct reference to the string object. The
     * caller is responsible to keep the format valid.
     */
    String &formatString() noexcept {
        return mFormatString;
    }
    const String &formatString() const noexcept {
        return mFormatString;
    }

protected:

    ArgumentFeeder &argumentFeeder() noexcept {
        return mFeedArguments;
    }
    const ArgumentFeeder &argumentFeeder() const noexcept {
        return mFeedArguments;
    }

};

/**
 * Incomplete message.
 *
 * Implementation note: The recursive derivation is a trick to implement the %
 * operator without copying the message object.
 */
template<typename Head, typename... Tail>
class Message<Head, Tail...> : private Message<Tail...> {

public:

    using String = typename Message<Tail...>::String;
    using Char = typename Message<Tail...>::Char;
    using CharTraits = typename Message<Tail...>::CharTraits;
    using Allocator = typename Message<Tail...>::Allocator;
    using Format = typename Message<Tail...>::Format;

    using Message<Tail...>::Message;

    using Message<Tail...>::toFormat;

protected:

    using Message<Tail...>::argumentFeeder;

public:

    /** Binds an argument value to this message object. */
    Message<Tail...> &&operator%(const Head &value) && {
        auto &old = argumentFeeder();
        argumentFeeder() = [old, value](Format &f) -> Format & {
            return old(f) % value;
        };
        return std::move(*this);
    }

};

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_Message_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
