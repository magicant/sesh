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

#ifndef INCLUDED_language_source_Buffer_hh
#define INCLUDED_language_source_Buffer_hh

#include "buildconfig.h"

#include <functional>
#include <iterator>
#include <memory>
#include <ostream>
#include <utility>
#include "language/source/Location.hh"
#include "language/source/Source.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * A buffer wraps a Source object to provide extended functionality.
 *
 * Because the buffer contents may be dynamically modified during parsing, the
 * buffer provides iterators that are not invalidated by modification of buffer
 * contents (unlike normal string iterators). This class enforces use of
 * shared_ptr because iterators need safe pointers.
 */
class Buffer : public std::enable_shared_from_this<Buffer> {

public:

    using String = Source::String;
    using Char = Source::Char;
    using Size = Source::Size;
    using Difference = Source::Difference;
    using ConstReference = Source::ConstReference;

    using Pointer = std::shared_ptr<Buffer>;
    using ConstPointer = std::shared_ptr<const Buffer>;

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
        const Buffer &buffer() const { return *bufferPointer(); }

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
    Buffer() = default;
    Buffer(const Buffer &) = delete;
    Buffer(Buffer &&) = delete;
    Buffer &operator=(const Buffer &) = delete;
    Buffer &operator=(Buffer &&) = delete;
    ~Buffer() = default;

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

    Location location(Size position) const;

};

inline Buffer::ConstIterator::reference operator*(
        const Buffer::ConstIterator &i) {
    return i.buffer()[i.position()];
}

inline auto Buffer::ConstIterator::operator->() const -> pointer {
    return &**this;
}

inline Buffer::ConstIterator &operator++(Buffer::ConstIterator &i) noexcept {
    ++i.position();
    return i;
}

inline Buffer::ConstIterator operator++(Buffer::ConstIterator &i, int)
        noexcept {
    auto orig = i;
    ++i;
    return orig;
}

inline Buffer::ConstIterator &operator--(Buffer::ConstIterator &i) noexcept {
    --i.position();
    return i;
}

inline Buffer::ConstIterator operator--(Buffer::ConstIterator &i, int)
        noexcept {
    auto orig = i;
    --i;
    return orig;
}

inline Buffer::ConstIterator &operator+=(
        Buffer::ConstIterator &i, Buffer::ConstIterator::difference_type d)
        noexcept {
    i.position() += d;
    return i;
}

inline Buffer::ConstIterator &operator-=(
        Buffer::ConstIterator &i, Buffer::ConstIterator::difference_type d)
        noexcept {
    i.position() -= d;
    return i;
}

inline Buffer::ConstIterator operator+(
        const Buffer::ConstIterator &i,
        Buffer::ConstIterator::difference_type d) noexcept {
    return Buffer::ConstIterator(i.bufferPointer(), i.position() + d);
}

inline Buffer::ConstIterator operator+(
        Buffer::ConstIterator &&i, Buffer::ConstIterator::difference_type d)
        noexcept {
    return Buffer::ConstIterator(
            std::move(i.bufferPointer()), i.position() + d);
}

inline Buffer::ConstIterator operator+(
        Buffer::ConstIterator::difference_type d,
        const Buffer::ConstIterator &i) noexcept {
    return i + d;
}

inline Buffer::ConstIterator operator+(
        Buffer::ConstIterator::difference_type d,
        Buffer::ConstIterator &&i) noexcept {
    return std::move(i) + d;
}

inline Buffer::ConstIterator operator-(
        const Buffer::ConstIterator &i,
        Buffer::ConstIterator::difference_type d) noexcept {
    return Buffer::ConstIterator(i.bufferPointer(), i.position() - d);
}

inline Buffer::ConstIterator operator-(
        Buffer::ConstIterator &&i, Buffer::ConstIterator::difference_type d)
        noexcept {
    return Buffer::ConstIterator(
            std::move(i.bufferPointer()), i.position() - d);
}

inline Buffer::ConstIterator::difference_type operator-(
        const Buffer::ConstIterator &i1, const Buffer::ConstIterator &i2)
        noexcept {
    return i1.position() - i2.position();
}

inline bool operator==(
        const Buffer::ConstIterator &i1, const Buffer::ConstIterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() == i2.position();
}

inline bool operator!=(
        const Buffer::ConstIterator &i1, const Buffer::ConstIterator &i2)
        noexcept {
    return !(i1 == i2);
}

inline bool operator<(
        const Buffer::ConstIterator &i1, const Buffer::ConstIterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() < i2.position();
}

inline bool operator<=(
        const Buffer::ConstIterator &i1, const Buffer::ConstIterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() <= i2.position();
}

inline bool operator>(
        const Buffer::ConstIterator &i1, const Buffer::ConstIterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() > i2.position();
}

inline bool operator>=(
        const Buffer::ConstIterator &i1, const Buffer::ConstIterator &i2) noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() >= i2.position();
}

Buffer::String toString(
        const Buffer::ConstIterator &begin, const Buffer::ConstIterator &end);

inline Location toLocation(const Buffer::ConstIterator &i) {
    return i.buffer().location(i.position());
}

/** For debugging only. */
template<typename Char, typename Traits>
std::basic_ostream<Char, Traits> &operator<<(
        std::basic_ostream<Char, Traits> &os,
        const Buffer::ConstIterator &i) {
    return os << i.bufferPointer() << '@' << i.position();
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_Buffer_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
