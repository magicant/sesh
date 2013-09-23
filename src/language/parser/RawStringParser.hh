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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef INCLUDED_language_parser_RawStringParser_hh
#define INCLUDED_language_parser_RawStringParser_hh

#include "buildconfig.h"

#include <functional>
#include "common/Char.hh"
#include "language/parser/LineContinuationTreatment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/Predicate.hh"
#include "language/parser/Skipper.hh"
#include "language/parser/WordComponentParser.hh"

namespace sesh {
namespace language {

namespace syntax {

class RawString;

} // namespace syntax

namespace parser {

/** Parser for raw string. */
class RawStringParser : public Parser, public WordComponentParser {

private:

    Iterator mBegin;
    Skipper mSkipper;

public:

    /**
     * Constructs a raw string parser that works in the specified environment.
     * The raw string is parsed from the environment's current iterator
     * position up to (but not including) the first encountered delimiter (or
     * the end of source if no delimiter found). The argument predicate
     * determines if a character is a delimiter.
     */
    RawStringParser(
            Environment &,
            Predicate<common::Char> &&isDelimiter,
            LineContinuationTreatment = LineContinuationTreatment::REMOVE);

    RawStringParser(const RawStringParser &) = default;
    RawStringParser(RawStringParser &&) = default;
    RawStringParser &operator=(const RawStringParser &) = delete;
    RawStringParser &operator=(RawStringParser &&) = delete;
    ~RawStringParser() override = default;

public:

    /** Same as {@link #parse()}, but returns RawString type. */
    std::unique_ptr<syntax::RawString> parseRawString();

    std::unique_ptr<syntax::WordComponent> parse() override;

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_RawStringParser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
