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

#ifndef INCLUDED_language_parser_EnvironmentTestHelper_hh
#define INCLUDED_language_parser_EnvironmentTestHelper_hh

#include "buildconfig.h"

#include "catch.hpp"

#include <locale>
#include <stdexcept>
#include <utility>
#include "common/String.hh"
#include "language/parser/SourceEnvironment.hh"
#include "language/source/Buffer.hh"
#include "language/source/Source.hh"
#include "language/source/SourceTestHelper.hh"

namespace sesh {
namespace language {
namespace parser {

/** A stub of source environment that allows manipulating source code. */
class SourceTestEnvironment : public virtual SourceEnvironment {

public:

    void setSource(common::String &&s) {
        substituteSource([&s](source::Source::Pointer &&)
                -> source::Source::Pointer {
            return source::Source::Pointer(
                    new source::SourceStub(nullptr, 0, 0, std::move(s)));
        });
    }

    void appendSource(common::String &&s) {
        substituteSource([&s](source::Source::Pointer &&orig)
                -> source::Source::Pointer {
            auto length = (orig == nullptr) ? 0 : orig->length();
            return source::Source::Pointer(new source::SourceStub(
                    std::move(orig), length, length, std::move(s)));
        });
    }

    void checkSource(const common::String &string) {
        for (common::String::size_type i = 0; i < string.length(); ++i)
            CHECK(buffer().at(i) == string.at(i));
        CHECK_THROWS_AS(buffer().at(string.length()), std::out_of_range);
    }

}; // class SourceTestEnvironment

class CLocaleEnvironment : public virtual SourceEnvironment {

    const std::locale &locale() const noexcept override {
        return std::locale::classic();
    }

}; // class CLocaleEnvironment

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_EnvironmentTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
