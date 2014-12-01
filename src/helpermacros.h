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

/**
 * @file
 * This file contains some macros that abstract compiler-specific features.
 */

#ifndef INCLUDED_helpermacros_h
#define INCLUDED_helpermacros_h

#include "buildconfig.h"

#include "config.h"

/**
 * Defined to be the GCC version. It will be 0x40803 on GCC 4.8.3, for example.
 * In non-GCC environments, it will be zero.
 */
#define GCC_VERSION \
        (__GNUC__ << 16 | __GNUC_MINOR__ << 8 | __GNUC_PATCHLEVEL__)

/**
 * Signifies unreachable code to help compiler optimization and dumb false
 * compiler warnings.
 */
#if __cplusplus ? HAVE___BUILTIN_UNREACHABLE : GCC_VERSION >= 0x40500
#define UNREACHABLE() (__builtin_unreachable())
#else
#define UNREACHABLE() ((void) 0)
#endif

#endif // #ifndef INCLUDED_helpermacros_h

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
