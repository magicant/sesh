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

#ifndef INCLUDED_ui_message_report_hh
#define INCLUDED_ui_message_report_hh

#include "buildconfig.h"

#include <memory>
#include <utility>
#include <vector>
#include "language/source/fragment.hh"
#include "ui/message/category.hh"
#include "ui/message/format.hh"

namespace sesh {
namespace ui {
namespace message {

/**
 * A report is a message associated with a source fragment position and an
 * error level.
 */
class report {

public:

    enum category category;

    format<> text;

    /** Nullable pointer to a source fragment associated with this report. */
    language::source::fragment_position position;

    /**
     * List of non-null pointers to other reports that provide additional info
     * on this report.
     */
    std::vector<std::shared_ptr<const report>> subreports;

    report(
            enum category c,
            format<> &&text = format<>(),
            language::source::fragment_position &&p = {},
            std::vector<std::shared_ptr<const report>> subreports = {}) :
            category(c),
            text(std::move(text)),
            position(std::move(p)),
            subreports(std::move(subreports)) { }

}; // class report

} // namespace message
} // namespace ui
} // namespace sesh

#endif // #ifndef INCLUDED_ui_message_report_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
