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

#ifndef INCLUDED_language_executing_multiple_field_result_hh
#define INCLUDED_language_executing_multiple_field_result_hh

#include "buildconfig.h"

#include <vector>
#include "common/either.hh"
#include "language/executing/field.hh"
#include "language/executing/result.hh"

namespace sesh {
namespace language {
namespace executing {

/**
 * Result of field expansion that may possibly have failed.
 *
 * The result parent class subobject contains the results of command
 * substitutions that happened during expansion.
 */
class multiple_field_result : public result {

public:

    /**
     * Optional field expansion result. The maybe object is empty if expansion
     * failed. Otherwise, the vector contains any number (possibly zero) of
     * fields.
     */
    common::maybe<std::vector<field>> fields;

}; // class multiple_field_result

} // namespace executing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_executing_multiple_field_result_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
