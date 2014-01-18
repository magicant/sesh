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

#ifndef INCLUDED_language_parser_SourceEnvironment_hh
#define INCLUDED_language_parser_SourceEnvironment_hh

#include "buildconfig.h"

#include <functional>
#include "language/parser/Environment.hh"
#include "language/source/SourceBuffer.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * This environment class contains a source buffer that can be modified by
 * subclasses and has a position on the buffer.
 */
class SourceEnvironment : public Environment {

private:

    /** The source buffer */
    source::SourceBuffer::Pointer mBuffer;

    /** The current position */
    Size mPosition;
    /** The current length of the buffer */
    Size mLength;

protected:

    /**
     * This method is the only way to modify the source buffer and update the
     * cached end iterator.
     */
    void substituteSource(const std::function<
            source::Source::Pointer(source::Source::Pointer &&)> &f) {
        mBuffer->substitute(f);
        mLength = mBuffer->length();
    }

public:

    SourceEnvironment();

    const source::SourceBuffer &sourceBuffer() const noexcept final override;
    Size length() const noexcept final override;
    common::Char at(Size) const final override;
    Size position() const noexcept final override;
    void setPosition(Size) final override;

    /** Stub implementation: always aborts */
    bool isEof() const noexcept override;
    /** Stub implementation: always throws */
    bool removeLineContinuation(Size) override;
    /** Stub implementation: always throws */
    bool substituteAlias(Size, Size) override;
    /** Stub implementation: always throws */
    void addDiagnosticMessage(Size, common::Message<> &&, common::ErrorLevel)
            override;
    /** Stub implementation: always aborts */
    const std::locale &locale() const noexcept override;

}; // class SourceEnvironment

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_SourceEnvironment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
