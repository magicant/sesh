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
 * Sesh.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef INCLUDED_language_source_SourceBuffer_hh
#define INCLUDED_language_source_SourceBuffer_hh

#include "buildconfig.h"

#include <functional>
#include <iterator>
#include <memory>
#include <ostream>
#include <utility>
#include "language/source/Source.hh"
#include "language/source/SourceLocation.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * A source buffer wraps a Source object to provide extended functionality.
 *
 * Because the buffer contents may be dynamically modified during parsing, the
 * buffer provides iterators that are not invalidated by modification of buffer
 * contents (unlike normal string iterators). This class enforces use of
 * shared_ptr because iterators need safe pointers.
 */
class SourceBuffer : public std::enable_shared_from_this<SourceBuffer> {

public:

    using String = Source::String;
    using Char = Source::Char;
    using Size = Source::Size;
    using Difference = Source::Difference;
    using ConstReference = Source::ConstReference;

    using Pointer = std::shared_ptr<SourceBuffer>;
    using ConstPointer = std::shared_ptr<const SourceBuffer>;

    /**
     * Random access iterator for the buffer contents. This iterator remembers
     * the character position, so it is not invalidated by substitution of the
     * contents unless the contents are shortened and the position exceeds the
     * new contents length.
     */
    class ConstIterator :
            public std::iterator<std::random_access_iterator_tag, const Char> {

    private:

        ConstPointer mBuffer;
        Size mPosition;

    public:

        ConstIterator(const ConstPointer &, Size) noexcept;
        ConstIterator(ConstPointer &&, Size) noexcept;

        ConstIterator() = default;
        ConstIterator(const ConstIterator &) = default;
        ConstIterator(ConstIterator &&) = default;
        ConstIterator &operator=(const ConstIterator &) = default;
        ConstIterator &operator=(ConstIterator &&) = default;
        ~ConstIterator() = default;

        ConstPointer &bufferPointer() noexcept { return mBuffer; }
        const ConstPointer &bufferPointer() const noexcept { return mBuffer; }
        Size &position() noexcept { return mPosition; }
        Size position() const noexcept { return mPosition; }

        /** Precondition: the buffer pointer must not be null. */
        const SourceBuffer &buffer() const { return *bufferPointer(); }

        /** Not very useful... */
        inline pointer operator->() const;

        reference operator[](difference_type d) const {
            return buffer()[position() + d];
        }

    };

private:

    std::unique_ptr<const Source> mSource;

public:

    /** Don't call the default constructor directly. Use {@link #create()}. */
    SourceBuffer() = default;
    SourceBuffer(const SourceBuffer &) = delete;
    SourceBuffer(SourceBuffer &&) = delete;
    SourceBuffer &operator=(const SourceBuffer &) = delete;
    SourceBuffer &operator=(SourceBuffer &&) = delete;
    ~SourceBuffer() = default;

    static Pointer create();

    Size length() const noexcept;

    ConstReference at(Size) const;
    ConstReference operator[](Size) const;

    /**
     * Substitutes the contents of this source buffer with the return value of
     * the argument function. The function is called with the current source.
     */
    void substitute(
            const std::function<Source::Pointer(Source::Pointer &&)> &f) {
        mSource = f(std::move(mSource));
    }

    ConstIterator cbegin() const noexcept;
    ConstIterator cend() const noexcept;
    ConstIterator begin() const noexcept { return cbegin(); }
    ConstIterator end() const noexcept { return cend(); }

    SourceLocation location(Size position) const;

};

inline SourceBuffer::ConstIterator::reference operator*(
        const SourceBuffer::ConstIterator &i) {
    return i.buffer()[i.position()];
}

inline auto SourceBuffer::ConstIterator::operator->() const -> pointer {
    return &**this;
}

inline SourceBuffer::ConstIterator &operator++(
        SourceBuffer::ConstIterator &i) noexcept {
    ++i.position();
    return i;
}

inline SourceBuffer::ConstIterator operator++(
        SourceBuffer::ConstIterator &i, int) noexcept {
    auto orig = i;
    ++i;
    return orig;
}

inline SourceBuffer::ConstIterator &operator--(
        SourceBuffer::ConstIterator &i) noexcept {
    --i.position();
    return i;
}

inline SourceBuffer::ConstIterator operator--(
        SourceBuffer::ConstIterator &i, int) noexcept {
    auto orig = i;
    --i;
    return orig;
}

inline SourceBuffer::ConstIterator &operator+=(
        SourceBuffer::ConstIterator &i,
        SourceBuffer::ConstIterator::difference_type d) noexcept {
    i.position() += d;
    return i;
}

inline SourceBuffer::ConstIterator &operator-=(
        SourceBuffer::ConstIterator &i,
        SourceBuffer::ConstIterator::difference_type d) noexcept {
    i.position() -= d;
    return i;
}

inline SourceBuffer::ConstIterator operator+(
        const SourceBuffer::ConstIterator &i,
        SourceBuffer::ConstIterator::difference_type d) noexcept {
    return SourceBuffer::ConstIterator(i.bufferPointer(), i.position() + d);
}

inline SourceBuffer::ConstIterator operator+(
        SourceBuffer::ConstIterator &&i,
        SourceBuffer::ConstIterator::difference_type d) noexcept {
    return SourceBuffer::ConstIterator(
            std::move(i.bufferPointer()), i.position() + d);
}

inline SourceBuffer::ConstIterator operator+(
        SourceBuffer::ConstIterator::difference_type d,
        const SourceBuffer::ConstIterator &i) noexcept {
    return i + d;
}

inline SourceBuffer::ConstIterator operator+(
        SourceBuffer::ConstIterator::difference_type d,
        SourceBuffer::ConstIterator &&i) noexcept {
    return std::move(i) + d;
}

inline SourceBuffer::ConstIterator operator-(
        const SourceBuffer::ConstIterator &i,
        SourceBuffer::ConstIterator::difference_type d) noexcept {
    return SourceBuffer::ConstIterator(i.bufferPointer(), i.position() - d);
}

inline SourceBuffer::ConstIterator operator-(
        SourceBuffer::ConstIterator &&i,
        SourceBuffer::ConstIterator::difference_type d) noexcept {
    return SourceBuffer::ConstIterator(
            std::move(i.bufferPointer()), i.position() - d);
}

inline SourceBuffer::ConstIterator::difference_type operator-(
        const SourceBuffer::ConstIterator &i1,
        const SourceBuffer::ConstIterator &i2) noexcept {
    return i1.position() - i2.position();
}

inline bool operator==(
        const SourceBuffer::ConstIterator &i1,
        const SourceBuffer::ConstIterator &i2) noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() == i2.position();
}

inline bool operator!=(
        const SourceBuffer::ConstIterator &i1,
        const SourceBuffer::ConstIterator &i2) noexcept {
    return !(i1 == i2);
}

inline bool operator<(
        const SourceBuffer::ConstIterator &i1,
        const SourceBuffer::ConstIterator &i2) noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() < i2.position();
}

inline bool operator<=(
        const SourceBuffer::ConstIterator &i1,
        const SourceBuffer::ConstIterator &i2) noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() <= i2.position();
}

inline bool operator>(
        const SourceBuffer::ConstIterator &i1,
        const SourceBuffer::ConstIterator &i2) noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() > i2.position();
}

inline bool operator>=(
        const SourceBuffer::ConstIterator &i1,
        const SourceBuffer::ConstIterator &i2) noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() >= i2.position();
}

SourceBuffer::String toString(
        const SourceBuffer::ConstIterator &begin,
        const SourceBuffer::ConstIterator &end);

inline SourceLocation toLocation(const SourceBuffer::ConstIterator &i) {
    return i.buffer().location(i.position());
}

/** For debugging only. */
template<typename Char, typename Traits>
std::basic_ostream<Char, Traits> &operator<<(
        std::basic_ostream<Char, Traits> &os,
        const SourceBuffer::ConstIterator &i) {
    return os << i.bufferPointer() << '@' << i.position();
}

} // namespace source
} // namespace language
} // namespace sesh

namespace std {

template<>
void swap<sesh::language::source::SourceBuffer::ConstIterator>(
        sesh::language::source::SourceBuffer::ConstIterator &i1,
        sesh::language::source::SourceBuffer::ConstIterator &i2) noexcept;

} // namespace std

#endif // #ifndef INCLUDED_language_source_SourceBuffer_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
