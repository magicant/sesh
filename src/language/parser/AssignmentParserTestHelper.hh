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

#ifndef INCLUDED_language_parser_AssignmentParserTestHelper_hh
#define INCLUDED_language_parser_AssignmentParserTestHelper_hh

#include "buildconfig.h"

#include <memory>
#include "common/Char.hh"
#include "common/String.hh"
#include "language/parser/Parser.hh"
#include "language/syntax/Assignment.hh"
#include "language/syntax/Word.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * A parser stub that succeeds if the current character is an equal sign ('=').
 * The resultant assignment has "N" as the variable name and an empty assigned
 * value.
 */
class AssignmentParserStub :
            public Parser<std::unique_ptr<syntax::Assignment>> {

    using Parser<std::unique_ptr<syntax::Assignment>>::Parser;

    std::unique_ptr<syntax::Assignment> mResultAssignment;

    void parseImpl() override {
        using sesh::common::CharTraits;

        auto ci = currentCharInt();
        if (!CharTraits::eq_int_type(ci, CharTraits::to_int_type(L('='))))
            return;

        mResultAssignment.reset(new syntax::Assignment(
                L("N"), std::unique_ptr<syntax::Word>(new syntax::Word)));
        result() = &mResultAssignment;
        environment().setPosition(environment().position() + 1);
    }

    void resetImpl() noexcept override {
        mResultAssignment.reset();
        Parser<std::unique_ptr<syntax::Assignment>>::resetImpl();
    }

}; // class AssignmentParserStub

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_AssignmentParserTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
