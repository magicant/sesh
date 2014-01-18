/* Copyright (C) 2013-2014 WATANABE Yuki
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

#ifndef INCLUDED_language_parser_Repeat_hh
#define INCLUDED_language_parser_Repeat_hh

#include "buildconfig.h"

#include <type_traits>
#include <vector>
#include <utility>
#include "common/Maybe.hh"
#include "language/parser/Environment.hh"
#include "language/parser/NormalParser.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Repeat is a parser that collects results from another parser (sub-parser)
 * into a vector (or another kind of container). The reset and parse methods of
 * the sub-parser are repeatedly called until parsing fails.
 *
 * The entire repeat parser always succeeds, regardless of how many times the
 * sub-parser succeeds.
 *
 * @tparam Subparser The sub-parser type. The result type of the sub-parser
 * must be move-insertable and (no-throw) destructible.
 * @tparam ResultContainer The type of a container which collects the results
 * of repeated parsing. The container must (1) be default-constructible, (2)
 * support the push_back method with an argument of an r-value reference to the
 * result of the sub-parser, and (3) have the clear method that never throws.
 */
template<
        typename Subparser,
        typename ResultContainer = std::vector<typename Subparser::Result>>
class Repeat : public NormalParser<ResultContainer> {

private:

    Subparser mSubparser;

public:

    /**
     * Constructs a new repeat parser.
     * @param e The environment.
     * @param arg Arguments forwarded to the constructor of the sub-parser.
     */
    template<typename... Arg>
    explicit Repeat(Environment &e, Arg &&... arg)
            noexcept(
                    std::is_nothrow_constructible<
                        NormalParser<ResultContainer>, Environment &>::value &&
                    std::is_nothrow_constructible<
                        Subparser, Arg &&...>::value) :
            NormalParser<ResultContainer>(e),
            mSubparser(std::forward<Arg>(arg)...) { }

    /**
     * Constructs a new repeat parser. All arguments are forwarded to the
     * constructor of the sub-parser. The first argument must be the
     * environment.
     */
    template<typename... Arg>
    static Repeat create(Environment &e, Arg &&... arg)
            noexcept(noexcept(Repeat(e, e, std::forward<Arg>(arg)...))) {
        return Repeat(e, e, std::forward<Arg>(arg)...);
    }

private:

    void parseImpl() override {
        if (!this->result().hasValue())
            this->result().emplace();

        while (auto &subresult = mSubparser.parse()) {
            this->result().value().push_back(std::move(subresult.value()));
            mSubparser.reset();
        }
    }

    void resetImpl() noexcept override {
        mSubparser.reset();
        this->NormalParser<ResultContainer>::reset();
    }

}; // template<typename Subparser> class Repeat

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Repeat_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
