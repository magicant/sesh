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

#ifndef INCLUDED_language_syntax_conditional_pipeline_test_helper_hh
#define INCLUDED_language_syntax_conditional_pipeline_test_helper_hh

#include "buildconfig.h"

#include <utility>
#include "common/copy.hh"
#include "language/syntax/conditional_pipeline.hh"
#include "language/syntax/pipeline_test_helper.hh"

namespace sesh {
namespace language {
namespace syntax {

inline conditional_pipeline make_conditional_pipeline_stub(
        common::xstring &&s) {
    return conditional_pipeline(
            conditional_pipeline::condition_type::and_then,
            make_pipeline_stub(std::move(s)));
}

inline conditional_pipeline make_conditional_pipeline_stub(
        const common::xstring &s) {
    return make_conditional_pipeline_stub(common::copy(s));
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_conditional_pipeline_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
