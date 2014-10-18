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

#ifndef INCLUDED_language_syntax_conditional_pipeline_hh
#define INCLUDED_language_syntax_conditional_pipeline_hh

#include "buildconfig.h"

#include <memory>
#include "language/syntax/pipeline.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A conditional pipeline is a pipeline that is executed conditionally
 * depending on the exit status of the previous pipeline.
 */
class conditional_pipeline {

public:

    using pipeline_pointer = std::unique_ptr<class pipeline>;

    enum class condition_type {
        /**
         * A conditional pipeline that has the and-then condition is executed
         * if the exit status of the previous pipeline is zero.
         */
        and_then,
        /**
         * A conditional pipeline that has the or-else condition is executed
         * if the exit status of the previous pipeline is non-zero.
         */
        or_else,
    };

private:

    condition_type m_condition;
    pipeline_pointer m_pipeline;

public:

    explicit conditional_pipeline(condition_type c);
    conditional_pipeline(condition_type c, pipeline_pointer &&p);

    conditional_pipeline(const conditional_pipeline &) = delete;
    conditional_pipeline(conditional_pipeline &&) = default;
    conditional_pipeline &operator=(const conditional_pipeline &) = delete;
    conditional_pipeline &operator=(conditional_pipeline &&) = default;
    ~conditional_pipeline() = default;

    condition_type &condition() noexcept { return m_condition; }
    condition_type condition() const noexcept { return m_condition; }

    class pipeline &pipeline() { return *m_pipeline; }
    const class pipeline &pipeline() const { return *m_pipeline; }

}; // class conditional_pipeline

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_conditional_pipeline_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
