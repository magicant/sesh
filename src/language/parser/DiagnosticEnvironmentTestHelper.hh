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

#ifndef INCLUDED_language_parser_DiagnosticEnvironmentTestHelper_hh
#define INCLUDED_language_parser_DiagnosticEnvironmentTestHelper_hh

#include "buildconfig.h"

#include <algorithm>
#include <utility>
#include <vector>
#include "common/Message.hh"
#include "language/parser/BasicEnvironmentTestHelper.hh"
#include "language/source/DiagnosticMessage.hh"
#include "language/source/DiagnosticMessageTestHelper.hh"

namespace sesh {
namespace language {
namespace parser {

class DiagnosticEnvironmentStub : public virtual BasicEnvironmentStub {

public:

    using MessageVector = std::vector<source::DiagnosticMessage>;

private:

    MessageVector mMessages;

public:

    void addDiagnosticMessage(
            const Iterator &i, common::Message<> &&m, common::ErrorLevel el)
            override {
        mMessages.emplace_back(i, std::move(m), el);
    }

    MessageVector &messages() noexcept { return mMessages; }
    const MessageVector &messages() const noexcept { return mMessages; }

    void checkMessages(const MessageVector &expectedMessages) const {
        CHECK(messages().size() == expectedMessages.size());

        auto n = std::min(messages().size(), expectedMessages.size());
        for (decltype(n) i = 0; i < n; ++i) {
            INFO(i << "/" << n);
            checkEqual(messages()[i], expectedMessages[i]);
        }
    }

    bool hasMessage(common::ErrorLevel el) const {
        return std::any_of(
                messages().cbegin(),
                messages().cend(),
                [el](const source::DiagnosticMessage &m) {
                    return m.errorLevel() == el;
                });
    }

}; // class DiagnosticEnvironmentStub

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_DiagnosticEnvironmentTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
