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

#ifndef INCLUDED_language_parser_Converter_hh
#define INCLUDED_language_parser_Converter_hh

#include "buildconfig.h"

#include <type_traits>
#include <utility>
#include "language/parser/Environment.hh"
#include "language/parser/NormalParser.hh"

namespace sesh {
namespace language {
namespace parser {

/** Converts the result of another parser into something else. */
template<typename FromParser, typename ToResult_>
class Converter : public NormalParser<ToResult_> {

public:

    using FromResult = typename FromParser::Result;
    using ToResult = ToResult_;

private:

    FromParser mFromParser;

public:

    /**
     * Constructs a new converter.
     * @tparam Arg argument types to the constructor of from-parser.
     * @param e environment
     * @param arg arguments forwarded to the constructor of from-parser.
     */
    template<typename... Arg>
    explicit Converter(Environment &e, Arg &&... arg)
            noexcept(std::is_nothrow_constructible<
                    FromParser, Arg &&...>::value) :
            NormalParser<ToResult>(e),
            mFromParser(std::forward<Arg>(arg)...) { }

private:

    /**
     * Converts the result of the from-parser.
     *
     * This method is called (only) after the from-parser succeeded. If the
     * conversion succeeds, this method must set the {@link
     * NormalParser#result} to a non-empty value, which will be
     * the result of this parser. Otherwise, if the conversion fails,
     * the result must be left empty and this parser will fail.
     *
     * @param from the result of the from-parser.
     */
    virtual void convert(FromResult &&from) = 0;

    void parseImpl() final override {
        if (auto &from = mFromParser.parse())
            convert(std::move(from.value()));
    }

    /**
     * Resets the from-parser as well as the intermediate result of this
     * parser. If a subclass overrides this function, it should call
     * Converter::resetImpl.
     */
    void resetImpl() noexcept override {
        mFromParser.reset();
        NormalParser<ToResult>::resetImpl();
    }

}; // class Converter

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Converter_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
