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

#ifndef INCLUDED_language_ConditionalPipeline_hh
#define INCLUDED_language_ConditionalPipeline_hh

#include <memory>
#include "language/Pipeline.hh"
#include "language/Printable.hh"

namespace sesh {
namespace language {

class Printer;

/**
 * A conditional pipeline is a pipeline that is executed conditionally
 * depending on the exit status of the previous pipeline.
 */
class ConditionalPipeline : public Printable {

public:

    using PipelinePointer = std::unique_ptr<Pipeline>;

    enum class Condition {
        /**
         * A conditional pipeline that has the and-then condition is executed
         * if the exit status of the previous pipeline is zero.
         */
        AND_THEN,
        /**
         * A conditional pipeline that has the or-else condition is executed
         * if the exit status of the previous pipeline is non-zero.
         */
        OR_ELSE,
    };

private:

    Condition mCondition;
    PipelinePointer mPipeline;

public:

    explicit ConditionalPipeline(Condition c);
    ConditionalPipeline(Condition c, PipelinePointer &&p);

    ConditionalPipeline(const ConditionalPipeline &) = delete;
    ConditionalPipeline(ConditionalPipeline &&) = default;
    ConditionalPipeline &operator=(const ConditionalPipeline &) = delete;
    ConditionalPipeline &operator=(ConditionalPipeline &&) = default;
    ~ConditionalPipeline() override = default;

    Condition &condition() noexcept { return mCondition; }
    Condition condition() const noexcept { return mCondition; }

    Pipeline &pipeline() { return *mPipeline; }
    const Pipeline &pipeline() const { return *mPipeline; }

    void print(Printer &) const override;

}; // class ConditionalPipeline

} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_ConditionalPipeline_hh

/* vim: set et sw=4 sts=4 tw=79: */
