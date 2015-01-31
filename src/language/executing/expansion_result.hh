/* Copyright (C) 2015 WATANABE Yuki
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

#ifndef INCLUDED_language_executing_expansion_result_hh
#define INCLUDED_language_executing_expansion_result_hh

#include "buildconfig.h"

#include <vector>
#include "common/either.hh"
#include "language/executing/expansion.hh"
#include "ui/message/report.hh"

namespace sesh {
namespace language {
namespace executing {

/** Result of word expansion that may possibly have failed. */
class expansion_result {

public:

    /**
     * Optional expansion result. The maybe object is empty if expansion
     * failed. Otherwise, the vector contains any number (possibly zero) of
     * expansions.
     *
     * Typically, the number of resultant expansions is one, but other number
     * of expansions may result when an array is expanded.
     */
    common::maybe<std::vector<expansion>> words;

    /**
     * Reports from the expansion process. If expansion failed, there should be
     * at least one {@link ui::message::category::error error} report. Reports
     * of other categories can be included, regardless of whether the expansion
     * was successful or not. Note that the reports might not be presented to
     * the human user because the result may be superseded by another expansion
     * result.
     */
    std::vector<ui::message::report> reports;

}; // class expansion_result

} // namespace executing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_executing_expansion_result_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
