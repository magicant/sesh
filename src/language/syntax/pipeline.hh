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

#ifndef INCLUDED_language_syntax_pipeline_hh
#define INCLUDED_language_syntax_pipeline_hh

#include "buildconfig.h"

#include <memory>
#include <vector>
#include <utility>
#include "language/syntax/command.hh"

namespace sesh {
namespace language {
namespace syntax {

/**
 * A pipeline is a sequence of one or more commands that are executed at a time
 * with their standard input/output connected with each other.
 *
 * Despite that definition, an instance of this class may not contain any
 * command. Users of this class must ensure that the instance contains at least
 * one command and that all the command pointers are non-null.
 */
class pipeline {

public:

    /** The type of pointers to a command. Pointers must not be null. */
    using command_pointer = std::shared_ptr<const command>;

    enum class exit_status_mode_type { straight, negated };

    std::vector<command_pointer> commands;
    exit_status_mode_type exit_status_mode;

    /**
     * Constructs a pipeline.
     * @param esm Exit status mode of the new pipeline.
     * @param a Arguments that are passed to the constructor of {@code
     * std::vector<command_pointer>} to initialize {@link #commands}.
     */
    template<typename... A>
    explicit pipeline(exit_status_mode_type esm, A &&... a) :
            commands(std::forward<A>(a)...), exit_status_mode(esm) { }

    /** Constructs a new straight, empty pipeline. */
    pipeline() : pipeline(exit_status_mode_type::straight) { }

}; // class pipeline

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_pipeline_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
