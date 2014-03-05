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

#ifndef INCLUDED_language_parser_RawStringParser_hh
#define INCLUDED_language_parser_RawStringParser_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "language/parser/Converter.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/StringParser.hh"
#include "language/syntax/WordComponent.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * Parser that converts a parsed string to a raw string as a word component.
 *
 * The result is the unique pointer to a non-empty raw string. If no characters
 * were accepted by the predicate, this parser fails.
 */
class RawStringParser : public Converter<
        StringParser, std::unique_ptr<syntax::WordComponent>> {

private:

    std::unique_ptr<syntax::WordComponent> mResultWordComponent;

public:

    /**
     * Constructs a raw string parser, passing the arguments to the internal
     * string parser.
     */
    RawStringParser(
            Environment &,
            Predicate<common::Char> &&isAcceptableChar,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

private:

    void convert(common::String &&) final override;

    void resetImpl() noexcept final override;

}; // class RawStringParser

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_RawStringParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
