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

#ifndef INCLUDED_language_parser_Keyword_hh
#define INCLUDED_language_parser_Keyword_hh

#include "buildconfig.h"

#include <functional>
#include "common/Maybe.hh"
#include "common/String.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A wrapper that holds a reference to a keyword string. The referred-to string
 * is one of the static constant member strings of this class.
 */
class Keyword : public std::reference_wrapper<const common::String> {

public:

    static const common::String CASE;
    static const common::String DO;
    static const common::String DONE;
    static const common::String ELIF;
    static const common::String ELSE;
    static const common::String ESAC;
    static const common::String EXCLAMATION;
    static const common::String FI;
    static const common::String FOR;
    static const common::String FUNCTION;
    static const common::String IF;
    static const common::String IN;
    static const common::String LEFT_BRACE;
    static const common::String LEFT_BRACKET_BRACKET;
    static const common::String RIGHT_BRACE;
    static const common::String RIGHT_BRACKET_BRACKET;
    static const common::String SELECT;
    static const common::String THEN;
    static const common::String UNTIL;
    static const common::String WHILE;

private:

    explicit Keyword(const common::String &value) noexcept :
            reference_wrapper(value) { }

public:

    /**
     * Parses the argument string. If it is a keyword, a non-null maybe object
     * is returned that contains the argument keyword. Otherwise, an empty
     * maybe is returned.
     */
    static common::Maybe<Keyword> parse(const common::String &);

}; // class Keyword

inline bool operator==(const Keyword &k1, const Keyword &k2) noexcept {
    return &k1.get() == &k2.get();
}

inline bool operator!=(const Keyword &k1, const Keyword &k2) noexcept {
    return !(k1 == k2);
}

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_Keyword_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
