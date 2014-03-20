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

#ifndef INCLUDED_language_parser_DiagnosticEnvironmentTestHelper_hh
#define INCLUDED_language_parser_DiagnosticEnvironmentTestHelper_hh

#include "buildconfig.h"

#include <algorithm>
#include "catch.hpp"
#include "common/ErrorLevel.hh"
#include "common/Message.hh"
#include "language/parser/DiagnosticEnvironment.hh"
#include "language/source/DiagnosticMessage.hh"
#include "language/source/DiagnosticMessageTestHelper.hh"

namespace sesh {
namespace language {
namespace parser {

class DiagnosticTestEnvironment : public DiagnosticEnvironment {

public:

    struct SimpleDiagnosticMessage {
        Size position;
        common::Message<> message;
        common::ErrorLevel errorLevel;
    };

    void checkMessages(
            const std::vector<source::DiagnosticMessage> &expectedMessages)
            const {
        CHECK(diagnosticMessages().size() == expectedMessages.size());

        auto n = std::min(
                diagnosticMessages().size(), expectedMessages.size());
        for (decltype(n) i = 0; i < n; ++i) {
            INFO(i << "/" << n);
            checkEqual(diagnosticMessages()[i], expectedMessages[i]);
        }
    }

    void checkMessages(
            const std::vector<SimpleDiagnosticMessage> &expectedMessages)
            const {
        std::vector<source::DiagnosticMessage> dms;
        for (const SimpleDiagnosticMessage &sdm : expectedMessages)
            dms.emplace_back(
                    source::DiagnosticMessage::Position(
                            buffer().shared_from_this(), sdm.position),
                    common::Message<>(sdm.message),
                    sdm.errorLevel);
        checkMessages(dms);
    }

    bool hasMessage(common::ErrorLevel el) const {
        return std::any_of(
                diagnosticMessages().cbegin(),
                diagnosticMessages().cend(),
                [el](const source::DiagnosticMessage &m) {
                    return m.errorLevel() == el;
                });
    }

}; // class DiagnosticTestEnvironment

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_DiagnosticEnvironmentTestHelper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
