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

#ifndef INCLUDED_language_source_stream_hh
#define INCLUDED_language_source_stream_hh

#include "buildconfig.h"

#include <functional>
#include <utility>
#include "async/shared_future.hh"
#include "async/shared_lazy.hh"
#include "language/source/fragment.hh"

namespace sesh {
namespace language {
namespace source {

// Defined just below
class stream_value;

using stream_value_future = async::shared_future<stream_value>;

/**
 * A stream is an abstract sequence of {@link async::shared_future}s of {@link
 * fragment_position}s.
 *
 * The stream abstracts how the shell reads source code lines, so the parser
 * can focus on parsing the source code fragment returned by the stream. The
 * fragment is returned in the future because the code cannot be read
 * synchronously.
 *
 * The stream is a key to the on-demand source code reading behavior of the
 * shell. When the shell reads the source code from the standard input, it
 * reads only one line at a time. If the line is found incomplete to make up a
 * command, the shell continues to read the next line. The shell is not allowed
 * to read more than the lines needed to make up a command because the
 * subsequent lines could be read by the command executed by the shell. To
 * achieve that on-demand reading behavior, the command parser requests the
 * source code of the stream only as much as needed. The stream determines
 * whether it needs to read the next line to return a fragment to the parser.
 *
 * The stream is defined as a lazily computed shared future of a pair of a
 * fragment position and the succeeding stream. The resultant fragment position
 * will be null if the stream has reached the end of input.
 *
 * Homomorphism requirement: Let @c s be an instance of stream and @c p the
 * pair presented in the shared future returned by calling @c s. Assume
 * <code>p.first</code> is incrementable and let <code>q</code> be the result
 * of <code>++p.first</code>. If <code>q.head</code> yields a non-null pointer,
 * the fragment position in the pair presented in the shared future returned by
 * calling <code>p.second</code> must be @c q.
 */
using stream = async::shared_lazy<stream_value_future>;

using stream_value_pair = std::pair<fragment_position, stream>;

class stream_value : public stream_value_pair {

    using stream_value_pair::stream_value_pair;

};

stream_value_future empty_stream_value_future();

inline stream empty_stream() noexcept {
    return stream(empty_stream_value_future);
}

/**
 * Creates a stream by prepending a fragment to another stream. The resultant
 * stream first iterates on the fragment and then continues to the other
 * stream.
 */
stream stream_of(const fragment_position &, const stream & = empty_stream());

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_stream_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
