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

#ifndef INCLUDED_language_parser_NormalParser_hh
#define INCLUDED_language_parser_NormalParser_hh

#include "buildconfig.h"

#include "common/Maybe.hh"
#include "language/parser/Parser.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Extension of {@link Parser} that contains the result maybe object as an
 * instance member.
 *
 * @tparam Result_ Type of the result of this parser.
 */
template<typename Result_>
class NormalParser : public Parser<Result_> {

    using Parser<Result_>::Parser;

private:

    common::Maybe<Result_> mResult;

protected:

    /**
     * Returns a reference to the parse result. This method can be called any
     * time. By default the result maybe object is empty. Typically, a subclass
     * implementation of {@link ParserBase#parseImpl} calls this method to set
     * a result.
     */
    common::Maybe<Result_> &result() noexcept final override {
        return mResult;
    }

    /** This default implementation simply clears the {@link #result}. */
    void resetImpl() noexcept override {
        result().clear();
    }

}; // template<typename Result_> class NormalParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_NormalParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
