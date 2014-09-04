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

#ifndef INCLUDED_language_source_buffer_hh
#define INCLUDED_language_source_buffer_hh

#include "buildconfig.h"

#include <functional>
#include <iterator>
#include <memory>
#include <ostream>
#include <utility>
#include "language/source/location.hh"
#include "language/source/source.hh"

namespace sesh {
namespace language {
namespace source {

/**
 * A buffer wraps a source object to provide extended functionality.
 *
 * Because the buffer contents may be dynamically modified during parsing, the
 * buffer provides iterators that are not invalidated by modification of buffer
 * contents (unlike normal string iterators). This class enforces use of
 * shared_ptr because iterators need safe pointers.
 */
class buffer : public std::enable_shared_from_this<buffer> {

public:

    using string_type = source::string_type;
    using value_type = source::value_type;
    using size_type = source::size_type;
    using difference_type = source::difference_type;
    using const_reference = source::const_reference;

    /**
     * Random access iterator for the buffer contents. This iterator remembers
     * the character position, so it is not invalidated by substitution of the
     * contents unless the contents are shortened and the position exceeds the
     * new contents length.
     */
    class const_iterator :
            public std::iterator<
                    std::random_access_iterator_tag, const value_type> {

    private:

        std::shared_ptr<const class buffer> m_buffer;
        size_type m_position;

    public:

        const_iterator(const std::shared_ptr<const class buffer> &, size_type)
                noexcept;
        const_iterator(std::shared_ptr<const class buffer> &&, size_type)
                noexcept;

        const_iterator() = default;
        const_iterator(const const_iterator &) = default;
        const_iterator(const_iterator &&) = default;
        const_iterator &operator=(const const_iterator &) = default;
        const_iterator &operator=(const_iterator &&) = default;
        ~const_iterator() = default;

        std::shared_ptr<const class buffer> &buffer_pointer() noexcept {
            return m_buffer;
        }
        const std::shared_ptr<const class buffer> &buffer_pointer() const
                noexcept {
            return m_buffer;
        }
        size_type &position() noexcept { return m_position; }
        size_type position() const noexcept { return m_position; }

        /** Precondition: the buffer pointer must not be null. */
        const class buffer &buffer() const { return *buffer_pointer(); }
        /*
         * XXX This declaration of the buffer method hides the buffer outer
         * class, so the class must be referred to as "class buffer" within the
         * const_iterator class.
         */

        /** Not very useful... */
        inline pointer operator->() const;

        reference operator[](difference_type d) const {
            return buffer()[position() + d];
        }

    };

private:

    std::unique_ptr<const source> m_source;

public:

    /** Don't call the default constructor directly. Use {@link #create()}. */
    buffer() = default;
    buffer(const buffer &) = delete;
    buffer(buffer &&) = delete;
    buffer &operator=(const buffer &) = delete;
    buffer &operator=(buffer &&) = delete;
    ~buffer() = default;

    static std::shared_ptr<buffer> create();

    size_type length() const noexcept;

    const_reference at(size_type) const;
    const_reference operator[](size_type) const;

    /**
     * Substitutes the contents of this source buffer with the return value of
     * the argument function. The function is called with the current source.
     */
    void substitute(
            const std::function<
                    source::source_pointer(source::source_pointer &&)> &f) {
        m_source = f(std::move(m_source));
    }

    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator end() const noexcept { return cend(); }

    Location location(size_type position) const;

};

inline buffer::const_iterator::reference operator*(
        const buffer::const_iterator &i) {
    return i.buffer()[i.position()];
}

inline auto buffer::const_iterator::operator->() const -> pointer {
    return &**this;
}

inline buffer::const_iterator &operator++(buffer::const_iterator &i) noexcept {
    ++i.position();
    return i;
}

inline buffer::const_iterator operator++(buffer::const_iterator &i, int)
        noexcept {
    auto orig = i;
    ++i;
    return orig;
}

inline buffer::const_iterator &operator--(buffer::const_iterator &i) noexcept {
    --i.position();
    return i;
}

inline buffer::const_iterator operator--(buffer::const_iterator &i, int)
        noexcept {
    auto orig = i;
    --i;
    return orig;
}

inline buffer::const_iterator &operator+=(
        buffer::const_iterator &i, buffer::const_iterator::difference_type d)
        noexcept {
    i.position() += d;
    return i;
}

inline buffer::const_iterator &operator-=(
        buffer::const_iterator &i, buffer::const_iterator::difference_type d)
        noexcept {
    i.position() -= d;
    return i;
}

inline buffer::const_iterator operator+(
        const buffer::const_iterator &i,
        buffer::const_iterator::difference_type d) noexcept {
    return buffer::const_iterator(i.buffer_pointer(), i.position() + d);
}

inline buffer::const_iterator operator+(
        buffer::const_iterator &&i, buffer::const_iterator::difference_type d)
        noexcept {
    return buffer::const_iterator(
            std::move(i.buffer_pointer()), i.position() + d);
}

inline buffer::const_iterator operator+(
        buffer::const_iterator::difference_type d,
        const buffer::const_iterator &i) noexcept {
    return i + d;
}

inline buffer::const_iterator operator+(
        buffer::const_iterator::difference_type d,
        buffer::const_iterator &&i) noexcept {
    return std::move(i) + d;
}

inline buffer::const_iterator operator-(
        const buffer::const_iterator &i,
        buffer::const_iterator::difference_type d) noexcept {
    return buffer::const_iterator(i.buffer_pointer(), i.position() - d);
}

inline buffer::const_iterator operator-(
        buffer::const_iterator &&i, buffer::const_iterator::difference_type d)
        noexcept {
    return buffer::const_iterator(
            std::move(i.buffer_pointer()), i.position() - d);
}

inline buffer::const_iterator::difference_type operator-(
        const buffer::const_iterator &i1, const buffer::const_iterator &i2)
        noexcept {
    return i1.position() - i2.position();
}

inline bool operator==(
        const buffer::const_iterator &i1, const buffer::const_iterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() == i2.position();
}

inline bool operator!=(
        const buffer::const_iterator &i1, const buffer::const_iterator &i2)
        noexcept {
    return !(i1 == i2);
}

inline bool operator<(
        const buffer::const_iterator &i1, const buffer::const_iterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() < i2.position();
}

inline bool operator<=(
        const buffer::const_iterator &i1, const buffer::const_iterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() <= i2.position();
}

inline bool operator>(
        const buffer::const_iterator &i1, const buffer::const_iterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() > i2.position();
}

inline bool operator>=(
        const buffer::const_iterator &i1, const buffer::const_iterator &i2)
        noexcept {
    // Comparison of iterators on different buffers is undefined, so only the
    // positions are compared.
    return i1.position() >= i2.position();
}

buffer::string_type to_string(
        const buffer::const_iterator &begin,
        const buffer::const_iterator &end);

inline Location to_location(const buffer::const_iterator &i) {
    return i.buffer().location(i.position());
}

/** For debugging only. */
template<typename Char, typename Traits>
std::basic_ostream<Char, Traits> &operator<<(
        std::basic_ostream<Char, Traits> &os,
        const buffer::const_iterator &i) {
    return os << i.buffer_pointer() << '@' << i.position();
}

} // namespace source
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_source_buffer_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
