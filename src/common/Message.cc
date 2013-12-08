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
#include "Message.hh"

#include <utility>
#include "boost/format.hpp"

namespace sesh {
namespace common {

namespace {

using Char = Message<>::Char;

const Char *defaultString(const Char *s) {
    return s == nullptr ? L("") : s;
}

Message<>::Format &id(Message<>::Format &f) {
    return f;
}

} // namespace

Message<>::Message(const Char *s) :
        mFormatString(defaultString(s)), mFeedArguments(id) { }

Message<>::Message(const String &s) :
        mFormatString(s), mFeedArguments(id) { }

Message<>::Message(String &&s) :
        mFormatString(std::move(s)), mFeedArguments(id) { }

auto Message<>::toFormat() const -> Format {
    Format f(mFormatString);
    mFeedArguments(f);
    return f;
}

auto Message<>::toString() const -> String {
    return toFormat().str();
}

} // namespace common
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
