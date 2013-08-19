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

#ifndef INCLUDED_language_AndOrList_hh
#define INCLUDED_language_AndOrList_hh

#include <memory>
#include <vector>
#include "language/ConditionalPipeline.hh"
#include "language/Pipeline.hh"
#include "language/Printable.hh"

namespace sesh {
namespace language {

class Printer;

/**
 * An and-or list is a pipeline possibly followed by any number of conditional
 * pipelines.
 */
class AndOrList : public Printable {

public:

    enum class Synchronicity {
        SEQUENTIAL,
        ASYNCHRONOUS,
    };

private:

    Pipeline mFirst;
    std::vector<ConditionalPipeline> mRest;
    Synchronicity mSynchronicity;

public:

    explicit AndOrList(
            Pipeline &&first,
            Synchronicity = Synchronicity::SEQUENTIAL);

    AndOrList(const AndOrList &) = delete;
    AndOrList(AndOrList &&) = default;
    AndOrList &operator=(const AndOrList &) = delete;
    AndOrList &operator=(AndOrList &&) = default;
    ~AndOrList() override = default;

    Pipeline &first() noexcept { return mFirst; }
    const Pipeline &first() const noexcept { return mFirst; }

    std::vector<ConditionalPipeline> &rest() noexcept {
        return mRest;
    }
    const std::vector<ConditionalPipeline> &rest() const noexcept {
        return mRest;
    }

    Synchronicity &synchronicity() noexcept { return mSynchronicity; }
    Synchronicity synchronicity() const noexcept { return mSynchronicity; }

    void print(Printer &) const override;

}; // class AndOrList

} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_AndOrList_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
