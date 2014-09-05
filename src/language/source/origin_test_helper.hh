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

#ifndef INCLUDED_language_source_origin_test_helper_hh
#define INCLUDED_language_source_origin_test_helper_hh

#include "buildconfig.h"

#include <memory>
#include "common/xchar.hh"
#include "language/source/origin.hh"

namespace sesh {
namespace language {
namespace source {

class origin_stub : public origin {

public:

    constexpr static const common::xchar
            *DUMMY_NAME = L("dummy origin"),
            *DUMMY_DESCRIPTION = L("dummy description");

    // XXX LLVM 3.4 libc++ seems to require a user-provided default constructor
    origin_stub() noexcept : origin() { }

    common::message<> name() const override {
        return common::message<>(DUMMY_NAME);
    }

    common::message<> description() const override {
        return common::message<>(DUMMY_DESCRIPTION);
    }

}; // class origin_stub

std::shared_ptr<const origin> dummy_origin() {
    return std::make_shared<const origin_stub>();
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_origin_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
