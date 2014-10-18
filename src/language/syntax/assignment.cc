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
#include "assignment.hh"

#include <utility>
#include "common/xstring.hh"
#include "language/syntax/word.hh"

namespace sesh {
namespace language {
namespace syntax {

namespace {

using common::xstring;

void create_word_if_null(assignment::word_pointer &w) {
    if (w == nullptr)
        w.reset(new word);
}

} // namespace

assignment::assignment() : m_variable_name(), m_value(new word) { }

assignment::assignment(const xstring &variable_name, word_pointer &&value) :
        m_variable_name(variable_name), m_value(std::move(value)) {
    create_word_if_null(m_value);
}

assignment::assignment(xstring &&variable_name, word_pointer &&value) :
        m_variable_name(std::move(variable_name)), m_value(std::move(value)) {
    create_word_if_null(m_value);
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
