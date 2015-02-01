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

#ifndef INCLUDED_environment_world_hh
#define INCLUDED_environment_world_hh

#include "buildconfig.h"

namespace sesh {
namespace environment {

/**
 * World is an abstract interface to a shell execution environment. It provides
 * all functions to probe and modify the environment and to access OS API.
 */
class world {

public:

}; // class world

} // namespace environment
} // namespace sesh

#endif // #ifndef INCLUDED_environment_world_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
