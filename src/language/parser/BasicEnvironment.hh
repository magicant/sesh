/* Copyright (C) 2013 WATANABE Yuki
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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef INCLUDED_language_parser_BasicEnvironment_hh
#define INCLUDED_language_parser_BasicEnvironment_hh

#include "language/parser/Environment.hh"
#include "language/source/Source.hh"
#include "language/source/SourceBuffer.hh"

namespace sesh {
namespace language {
namespace parser {

/**
 * This class implements basic portion of parser environment. It holds an
 * actual source buffer that can be manipulated by a subclass.
 */
class BasicEnvironment : public Environment {

private:

    source::SourceBuffer::Pointer mBuffer = source::SourceBuffer::create();
    source::SourceBuffer::ConstIterator mCurrent = mBuffer->begin();
    source::SourceBuffer::ConstIterator mEnd = mBuffer->end();

public:

    BasicEnvironment();
    BasicEnvironment(const BasicEnvironment &) = default;
    BasicEnvironment(BasicEnvironment &&) = default;
    BasicEnvironment &operator=(const BasicEnvironment &) = default;
    BasicEnvironment &operator=(BasicEnvironment &&) = default;
    ~BasicEnvironment() override = default;

protected:

    const source::SourceBuffer &sourceBuffer() const noexcept {
        return *mBuffer;
    }

    /**
     * This method is the only way to modify the source buffer and update the
     * cached end iterator.
     */
    void substituteSource(const std::function<
            source::Source::Pointer(source::Source::Pointer &&)> &f) {
        mBuffer->substitute(f);
        mEnd = mBuffer->end();
    }

public:

    source::SourceBuffer::ConstIterator &current()
            noexcept final override {
        return mCurrent;
    }

    const source::SourceBuffer::ConstIterator &end()
            const noexcept final override {
        return mEnd;
    }

    bool removeLineContinuation(
            const source::SourceBuffer::ConstIterator &)
            final override;

};

} // namespace parser
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parser_BasicEnvironment_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
