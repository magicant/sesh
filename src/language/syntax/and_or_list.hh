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

#ifndef INCLUDED_language_syntax_and_or_list_hh
#define INCLUDED_language_syntax_and_or_list_hh

#include "buildconfig.h"

#include <vector>
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/pipeline.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * An and-or list is a pipeline possibly followed by any number of conditional
 * pipelines. An and-or list is executed either synchronously or
 * asynchronously.
 */
class and_or_list {

public:

    enum class synchronicity_type { sequential, asynchronous };

    pipeline first;
    std::vector<conditional_pipeline> rest;
    synchronicity_type synchronicity = synchronicity_type::sequential;

}; // class and_or_list

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_and_or_list_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
