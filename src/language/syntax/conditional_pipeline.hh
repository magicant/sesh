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

#ifndef INCLUDED_language_syntax_conditional_pipeline_hh
#define INCLUDED_language_syntax_conditional_pipeline_hh

#include "buildconfig.h"

#include <utility>
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

    /** Defines a condition on which the pipeline is executed. */
    enum class condition_type {
        /**
         * The pipeline is executed if the exit status of the previous pipeline
         * is zero.
         */
        and_then,
        /**
         * The pipeline is executed if the exit status of the previous pipeline
         * is non-zero.
         */
        or_else,
    };

    class pipeline pipeline;
    condition_type condition;

    /**
     * Constructs a conditional pipeline.
     * @param c Condition on which the pipeline is executed.
     * @param a Arguments that are passed to the constructor of {@link
     * pipeline} to initialize {@link #pipeline}.
     */
    template<typename... A>
    explicit conditional_pipeline(condition_type c, A &&... a) :
            pipeline(std::forward<A>(a)...), condition(c) { }

}; // class conditional_pipeline

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_conditional_pipeline_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
