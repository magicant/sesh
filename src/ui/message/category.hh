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

#ifndef INCLUDED_ui_message_category_hh
#define INCLUDED_ui_message_category_hh

#include "buildconfig.h"

namespace sesh {
namespace ui {
namespace message {

/** Defines types of diagnostic messages. */
enum class category {

    /**
     * Result messages are printed during the normal (successful) operation of
     * a command.
     */
    result,

    /**
     * Errors are the most severe type of diagnostic messages that are always
     * presented to the user and make the command to fail.
     */
    error,

    /**
     * Warnings are non-critical type of diagnostic messages that inform the
     * user of possible errors.
     */
    warning,

    /**
     * Notes are auxiliary messages that provide more details on other errors
     * and warnings.
     */
    note,

}; // enum class category

} // namespace message
} // namespace ui
} // namespace sesh

#endif // #ifndef INCLUDED_ui_message_category_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
