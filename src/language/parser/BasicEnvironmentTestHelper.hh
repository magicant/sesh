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

#ifndef INCLUDED_language_parser_BasicEnvironmentTestHelper_hh
#define INCLUDED_language_parser_BasicEnvironmentTestHelper_hh

#include "buildconfig.h"

#include "common/ErrorLevel.hh"
#include "common/String.hh"
#include "language/parser/BasicEnvironment.hh"
#include "language/parser/Environment.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"
#include "language/source/SourceTestHelper.hh"

namespace sesh {
namespace language {
namespace parser {

class BasicEnvironmentStub : public BasicEnvironment {

public:

    using Iterator = sesh::language::source::SourceBuffer::ConstIterator;

private:

    bool mIsEof = false;

    using BasicEnvironment::BasicEnvironment;

    bool isEof() const noexcept override {
        return mIsEof;
    }

    bool substituteAlias(const Iterator &, const Iterator &) override {
        throw "unexpected substituteAlias";
    }

    void addDiagnosticMessage(
            const Iterator &, common::String &&, common::ErrorLevel) override {
        throw "unexpected addDiagnosticMessage";
    }

public:

    void setIsEof(bool isEof = true) {
        mIsEof = isEof;
    }

    Iterator begin() const {
        return sourceBuffer().begin();
    }

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
            CHECK(sourceBuffer().at(i) == string.at(i));
        CHECK_THROWS_AS(sourceBuffer().at(string.length()), std::out_of_range);
    }

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_BasicEnvironmentTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
