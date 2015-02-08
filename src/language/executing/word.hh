/* Copyright (C) 2015 WATANABE Yuki
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

#ifndef INCLUDED_language_executing_word_hh
#define INCLUDED_language_executing_word_hh

#include "buildconfig.h"

#include <memory>
#include "async/future.hh"
#include "environment/world.hh"
#include "language/executing/expansion_result.hh"
#include "language/executing/multiple_field_result.hh"
#include "language/syntax/word.hh"

namespace sesh {
namespace language {
namespace executing {

/**
 * Performs the four expansions, which are tilde expansion, parameter
 * expansion, command substitution, and arithmetic expansion.
 *
 * @pre The argument pointers must not be null.
 */
async::future<expansion_result> expand_four(
        const std::shared_ptr<environment::world> &,
        bool is_quoted,
        const std::shared_ptr<const syntax::word> &);

/**
 * Expands a word. All kinds of expansions are performed: the four expansions,
 * brace expansions, field splitting, and pathname expansion. The result may be
 * any number of fields.
 *
 * @pre The argument pointers must not be null.
 */
async::future<multiple_field_result> expand_to_multiple_fields(
        const std::shared_ptr<environment::world> &,
        const std::shared_ptr<const syntax::word> &);

} // namespace executing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_executing_word_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
