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

#ifndef INCLUDED_common_trie_hh
#define INCLUDED_common_trie_hh

#include "buildconfig.h"

#include <cassert>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include "common/either.hh"

namespace sesh {
namespace common {

/**
 * A trie is a recursively structured map. The value type of the map includes
 * (a pointer type to) the trie type itself. The entire recursively-connected
 * nodes are regarded as a map from strings of the original key type to the
 * original value type.
 *
 * @tparam KeyDigit The key type of the inner maps. The entire trie is then
 * regarded as a map from strings from @c KeyDigit to @c Value.
 * @tparam Value The value type. Must be containable in maybe.
 * @tparam KeyComparator The comparator of key units. It must have a function
 * call operator that takes two (constant references to) key units and returns
 * true iff the first is less than the second.
 */
template<
        typename KeyDigit,
        typename Value,
        typename KeyComparator = std::less<KeyDigit>>
class trie {

private:

    /**
     * Type of the map containing child nodes.
     *
     * It would be more desirable to have the children directly, but we need to
     * avoid recursion of incomplete types.
     */
    using child_map = std::map<KeyDigit, std::unique_ptr<trie>, KeyComparator>;

public:

    using key_type = KeyDigit;
    using mapped_type = Value;
    using value_type = trie;
    using size_type = typename child_map::size_type;
    using difference_type = typename child_map::difference_type;
    using key_compare = KeyComparator;
    // XXX trie is not (yet) allocator-aware.
    // using allocator_type = ?;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    // XXX trie is not (yet) allocator-aware.
    // using pointer = std::allocator_traits<Allocator>::pointer
    // using const_pointer = std::allocator_traits<Allocator>::const_pointer

private:

    /** Pointer to the parent node. Null if this node is the root node. */
    trie *m_parent;

    child_map m_children;

    maybe<Value> m_value;

    /**
     * The number of values that exist under this node, that is, the number of
     * descendant nodes (including this node itself) that have a value.
     */
    size_type m_size;

    trie(trie *parent, const key_compare &comp) :
            m_parent(parent), m_children(comp), m_value(), m_size(0) { }

    child_map &children() noexcept { return m_children; }
    const child_map &children() const noexcept { return m_children; }

public:

    /** Creates (the root node of) a new trie. */
    explicit trie(const key_compare &comp = key_compare()) :
            trie(nullptr, comp) { }

    // Copy constructor is not supported now.
    trie(const trie &) = delete;

    /** Move constructor */
    trie(trie &&t) :
            m_parent(std::move(t.m_parent)),
            m_children(std::move(t.m_children)),
            m_value(std::move(t.m_value)),
            m_size(std::move(t.m_size)) {
        using C = std::pair<const KeyDigit, std::unique_ptr<trie>>;
        for (C &c : m_children) {
            c.second->m_parent = this;
            t.m_size -= c.second->m_size;
        }
    }

    // Assignments are not supported now.
    trie &operator=(const trie &) = delete;
    trie &operator=(trie &&) = delete;

    /** Returns a copy of the key comparator. */
    key_compare key_comp() { return m_children.key_comp(); }

    size_type max_size() const noexcept { return children().max_size(); }

    /** Returns the number of values in the entire trie. */
    size_type size() const noexcept { return m_size; }

    /** Checks if this node has no values. */
    bool empty() const noexcept { return size() == 0; }

private:

    void increment_size(difference_type count = 1) noexcept {
        m_size += count;
        if (m_parent != nullptr)
            m_parent->increment_size(count);
    }

public:

    /** Checks if this node has a value. */
    bool has_value() const noexcept { return !!m_value; }

    /**
     * Returns a reference to the value of this node. The behavior is undefined
     * if this node has no value.
     */
    Value &value() { return *m_value; }
    /**
     * Returns a reference to the value of this node. The behavior is undefined
     * if this node has no value.
     */
    const Value &value() const { return *m_value; }

    /**
     * Returns a reference to the maybe object that may contain the value of
     * this node. The maybe is empty if this node has no value.
     *
     * There is no non-const version of this method because the trie
     * implementation has to keep track of the number of values.
     */
    const maybe<Value> &maybe_value() const noexcept { return m_value; }

    /**
     * If this node contains no value, creates one by emplacement. All the
     * arguments are forwarded to the constructor of the new value.
     *
     * If this node already has a value, this function has no side effect.
     *
     * If the constructor throws, the node remains without value.
     *
     * @return a pair of a reference to the value and a Boolean. The Boolean is
     * true if a new value was created, and false if the node already had a
     * value.
     */
    template<typename... Arg>
    std::pair<Value &, bool> emplace_value(Arg &&... arg)
            noexcept(std::is_nothrow_constructible<Value, Arg...>::value) {
        if (has_value())
            return std::pair<Value &, bool>(value(), false);

        m_value.try_emplace(std::forward<Arg>(arg)...);
        increment_size();
        return std::pair<Value &, bool>(value(), true);
    }

    /**
     * Returns a reference to the value in this node. If the node has no value,
     * a new value is created by value initialization.
     */
    Value &get_or_create_value()
            noexcept(std::is_nothrow_default_constructible<Value>::value) {
        return emplace_value().first;
    }

    /** Erases the value of this node. */
    void erase_value() noexcept {
        if (has_value()) {
            m_value.clear();
            increment_size(-1);
        }
    }

    /**
     * Constructs a key object from the given arguments and associates it with
     * a new child node.
     *
     * If this node already has an equivalent key, no child node will be added.
     *
     * If the constructor throws, the node remains unmodified.
     *
     * @return a pair of a reference to the new child node and a Boolean. The
     * Boolean is true if a new child was added, and false if this node already
     * had the child.
     */
    template<typename... Arg>
    std::pair<value_type &, bool> emplace_child(Arg &&... arg) {
        std::pair<typename child_map::iterator, bool> p = m_children.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<Arg>(arg)...),
                std::make_tuple(nullptr));
        std::unique_ptr<value_type> &child = p.first->second;
        if (p.second) {
            try {
                child.reset(new trie(this, key_comp()));
            } catch (...) {
                m_children.erase(p.first);
                throw;
            }
        }
        return std::pair<value_type &, bool>(*child, p.second);
    }

    /**
     * Adds descendant nodes from the iterator. The each iterated value is
     * passed to {@link #emplace_child} to create descendants.
     * @return reference to the last descendant.
     */
    template<typename InputIterator>
    value_type &emplace_descendants(InputIterator begin, InputIterator end) {
        value_type *node = this;
        while (begin != end) {
            node = std::addressof(node->emplace_child(*begin).first);
            ++begin;
        }
        return *node;
    }

    /**
     * Adds descendant nodes from the given key string. The key string must be
     * able to iterate key digits that can be passed to {@link #emplace_child}
     * to create descendants.
     * @return reference to the last descendant.
     */
    template<typename KeyString>
    value_type &emplace_descendants(const KeyString &ks) {
        return emplace_descendants(std::begin(ks), std::end(ks));
    }

    /**
     * Returns a reference to the child node associated with the given key.
     *
     * If this node has no child node for the key, a new empty child node is
     * created with a key copy-constructed from the argument.
     */
    value_type &operator[](const key_type &k) {
        return emplace_child(k).first;
    }
    /**
     * Returns a reference to the child node associated with the given key.
     *
     * If this node has no child node for the key, a new empty child node is
     * created with a key move-constructed from the argument.
     */
    value_type &operator[](key_type &&k) {
        return emplace_child(std::move(k)).first;
    }

    /**
     * Returns a reference to the child node associated with the given key.
     * @throws std::out_of_range if there is no such child node.
     */
    value_type &at(const key_type &k) {
        return *m_children.at(k);
    }
    /**
     * Returns a reference to the child node associated with the given key.
     * @throws std::out_of_range if there is no such child node.
     */
    const value_type &at(const key_type &k) const {
        return *m_children.at(k);
    }

    /**
     * Removes all descendants of this node.
     *
     * All references and iterators pointing to any descendants of this node
     * are invalidated.
     */
    void erase_descendants() noexcept {
        m_children.clear();
    }

    /** Removes the value of this node and all descendants. */
    void clear() noexcept {
        erase_value();
        erase_descendants();
    }

    // For friend declaration in non-const traverser
    template<typename>
    class const_traverser;

    /**
     * Traverses a trie. Iterates all nodes of the trie in pre-order.
     *
     * @tparam KS The type of strings of keys. It will be used to represent a
     * sequence of keys on the path from the root node to the current node.
     * This type must be default-constructible and have the back, push_back,
     * and pop_back functions.
     *
     * @see const_traverser
     */
    template<typename KS = std::basic_string<KeyDigit>>
    class traverser : public std::iterator<std::forward_iterator_tag, trie> {

        // For conversion to const-traverser
        friend class const_traverser<KS>;

    public:

        using key_string = KS;

    private:

        std::vector<typename child_map::iterator> m_path_nodes;
        key_string m_path_string;
        /**
         * If this traverser is dereferenceable, the current node is non-null.
         * Otherwise, the current node is either the root node of the trie or
         * null.
         */
        trie *m_current_node;
        /** A past-the-end traverser is not dereferenceable. */
        bool m_is_dereferenceable;

    public:

        traverser() :
                m_path_nodes(),
                m_path_string(),
                m_current_node(nullptr),
                m_is_dereferenceable(false) { }

        explicit traverser(trie *t, bool is_dereferenceable = true) :
                m_path_nodes(),
                m_path_string(),
                m_current_node(t),
                m_is_dereferenceable(is_dereferenceable) { }

        const key_string &path_string() const { return m_path_string; }

        trie &operator*() const {
            assert(m_current_node != nullptr);
            return *m_current_node;
        }
        trie *operator->() const {
            assert(m_current_node != nullptr);
            return m_current_node;
        }

    private:

        void down(typename child_map::iterator &&i) {
            m_path_string.push_back(i->first);
            m_current_node = i->second.get();
            m_path_nodes.push_back(std::move(i));
        }

        void forward() {
            assert(m_is_dereferenceable);
            if (!m_current_node->children().empty())
                return down(m_current_node->children().begin());
            for (;;) {
                if (m_path_nodes.empty()) {
                    m_is_dereferenceable = false;
                    return;
                }
                trie *parent = m_current_node->m_parent;
                ++m_path_nodes.back();
                if (m_path_nodes.back() != parent->children().end()) {
                    m_path_string.back() = m_path_nodes.back()->first;
                    m_current_node = m_path_nodes.back()->second.get();
                    return;
                }
                m_path_nodes.pop_back();
                m_path_string.pop_back();
                m_current_node = parent;
            }
        }

    public:

        traverser &operator++() {
            forward();
            return *this;
        }

        traverser operator++(int) {
            traverser old = *this;
            forward();
            return old;
        }

        /**
         * Looks up the given key in the current node. If the current node has
         * no child node associated with the key, this method has no effect.
         * @return true iff a child was found for the key.
         */
        bool down(const KeyDigit &k) {
            assert(m_is_dereferenceable);
            auto i = m_current_node->children().find(k);
            if (i == m_current_node->children().end())
                return false;
            down(std::move(i));
            return true;
        }

        size_type down(
                typename key_string::const_iterator begin,
                typename key_string::const_iterator end) {
            size_type count = 0;
            while (begin != end && down(*begin))
                ++begin, ++count;
            return count;
        }

        size_type down(const key_string &ks) {
            return down(std::begin(ks), std::end(ks));
        }

        /**
         * Goes up to the nth ancestor node. If the current node has no that
         * many ancestors, stops at the root node.
         * @return number of ancestors passed.
         */
        size_type up(size_type count = 1) {
            assert(m_is_dereferenceable);

            size_type actual_count = 0;
            while (actual_count < count && !m_path_nodes.empty()) {
                m_path_nodes.pop_back();
                m_path_string.pop_back();
                m_current_node = m_current_node->m_parent;
                ++actual_count;
            }
            return actual_count;
        }

        bool operator==(const traverser &other) const {
            return this->m_current_node == other.m_current_node &&
                    this->m_is_dereferenceable == other.m_is_dereferenceable;
        }
        bool operator!=(const traverser &other) const {
            return !(*this == other);
        }

    }; // template<typename KS> class traverser

    /**
     * Traverses a trie. Iterates all nodes of the trie in pre-order.
     *
     * @tparam KS The type of strings of keys. It will be used to represent a
     * sequence of keys on the path from the root node to the current node.
     * This type must be default-constructible and have the back, push_back,
     * and pop_back functions.
     *
     * @see traverser
     */
    template<typename KS = std::basic_string<KeyDigit>>
    class const_traverser :
            public std::iterator<std::forward_iterator_tag, const trie> {

    public:

        using key_string = KS;

    private:

        std::vector<typename child_map::const_iterator> m_path_nodes;
        key_string m_path_string;
        /**
         * If this traverser is dereferenceable, the current node is non-null.
         * Otherwise, the current node is either the root node of the trie or
         * null.
         */
        const trie *m_current_node;
        /** A past-the-end traverser is not dereferenceable. */
        bool m_is_dereferenceable;

    public:

        const_traverser() :
                m_path_nodes(),
                m_path_string(),
                m_current_node(nullptr),
                m_is_dereferenceable(false) { }

        explicit const_traverser(
                const trie *t, bool is_dereferenceable = true) :
                m_path_nodes(),
                m_path_string(),
                m_current_node(t),
                m_is_dereferenceable(is_dereferenceable) { }

        const_traverser(const traverser<KS> &t) :
                m_path_nodes(t.m_path_nodes.begin(), t.m_path_nodes.end()),
                m_path_string(t.m_path_string),
                m_current_node(t.m_current_node),
                m_is_dereferenceable(t.m_is_dereferenceable) { }

        const_traverser(traverser<KS> &&t) :
                m_path_nodes(
                        std::make_move_iterator(t.m_path_nodes.begin()),
                        std::make_move_iterator(t.m_path_nodes.end())),
                m_path_string(std::move(t.m_path_string)),
                m_current_node(std::move(t.m_current_node)),
                m_is_dereferenceable(std::move(t.m_is_dereferenceable)) { }

        const key_string &path_string() const { return m_path_string; }

        const trie &operator*() const {
            assert(m_current_node != nullptr);
            return *m_current_node;
        }
        const trie *operator->() const {
            assert(m_current_node != nullptr);
            return m_current_node;
        }

    private:

        void down(typename child_map::const_iterator &&i) {
            m_path_string.push_back(i->first);
            m_current_node = i->second.get();
            m_path_nodes.push_back(std::move(i));
        }

        void forward() {
            assert(m_is_dereferenceable);
            if (!m_current_node->children().empty())
                return down(m_current_node->children().begin());
            for (;;) {
                if (m_path_nodes.empty()) {
                    m_is_dereferenceable = false;
                    return;
                }
                const trie *parent = m_current_node->m_parent;
                ++m_path_nodes.back();
                if (m_path_nodes.back() != parent->children().end()) {
                    m_path_string.back() = m_path_nodes.back()->first;
                    m_current_node = m_path_nodes.back()->second.get();
                    return;
                }
                m_path_nodes.pop_back();
                m_path_string.pop_back();
                m_current_node = parent;
            }
        }

    public:

        const_traverser &operator++() {
            forward();
            return *this;
        }

        const_traverser operator++(int) {
            const_traverser old = *this;
            forward();
            return old;
        }

        /**
         * Looks up the given key in the current node. If the current node has
         * no child node associated with the key, this method has no effect.
         * @return true iff a child was found for the key.
         */
        bool down(const KeyDigit &k) {
            assert(m_is_dereferenceable);
            auto i = m_current_node->children().find(k);
            if (i == m_current_node->children().end())
                return false;
            down(std::move(i));
            return true;
        }

        size_type down(
                typename key_string::const_iterator begin,
                typename key_string::const_iterator end) {
            size_type count = 0;
            while (begin != end && down(*begin))
                ++begin, ++count;
            return count;
        }

        size_type down(const key_string &ks) {
            return down(std::begin(ks), std::end(ks));
        }

        /**
         * Goes up to the nth ancestor node. If the current node has no that
         * many ancestors, stops at the root node.
         * @return number of ancestors passed.
         */
        size_type up(size_type count = 1) {
            assert(m_is_dereferenceable);

            size_type actual_count = 0;
            while (actual_count < count && !m_path_nodes.empty()) {
                m_path_nodes.pop_back();
                m_path_string.pop_back();
                m_current_node = m_current_node->m_parent;
                ++actual_count;
            }
            return actual_count;
        }

        bool operator==(const const_traverser &other) const {
            return this->m_current_node == other.m_current_node &&
                    this->m_is_dereferenceable == other.m_is_dereferenceable;
        }
        bool operator!=(const const_traverser &other) const {
            return !(*this == other);
        }

    }; // template<typename KS> class const_traverser

    template<typename KeyString = std::basic_string<KeyDigit>>
    traverser<KeyString> traverser_begin() {
        return traverser<KeyString>(this);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    traverser<KeyString> traverser_end() {
        return traverser<KeyString>(this, false);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    const_traverser<KeyString> traverser_begin() const {
        return const_traverser<KeyString>(this);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    const_traverser<KeyString> traverser_end() const {
        return const_traverser<KeyString>(this, false);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    const_traverser<KeyString> const_traverser_begin() const {
        return const_traverser<KeyString>(this);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    const_traverser<KeyString> const_traverser_end() const {
        return const_traverser<KeyString>(this, false);
    }

    /*
    using iterator = traverser<>;
    using const_iterator = const_traverser<>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    // iterator: {,c}{,r}{begin,end}
    */

    /**
     * Creates (the root node of) a new trie and add values from the iterator.
     * The value type of the iterator must be <code>std::pair&lt;key_string,
     * Value></code>, where the key string type must be a container that
     * iterates @c KeyDigit.
     */
    template<typename InputIterator>
    trie(
            InputIterator begin,
            InputIterator end,
            const key_compare &comp = key_compare()) :
            trie(comp) {
        while (begin != end) {
            emplace_descendants(begin->first).emplace_value(begin->second);
            ++begin;
        }
    }

    /**
     * Creates (the root node of) a new trie and add values from the
     * initializer list.
     */
    trie(
            std::initializer_list<
                    std::pair<const std::basic_string<KeyDigit>, Value>> init,
            const key_compare &comp = key_compare()) :
            trie(std::begin(init), std::end(init), comp) { }

    ~trie() noexcept {
        erase_value(); // decrement size if has value
    }

};

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_trie_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
