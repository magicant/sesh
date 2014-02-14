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

#ifndef INCLUDED_common_Trie_hh
#define INCLUDED_common_Trie_hh

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
#include "common/Maybe.hh"

namespace sesh {
namespace common {

/**
 * A trie is a recursively structured map. The value type of the map includes
 * (a pointer type to) the trie type itself. The entire recursively-connected
 * nodes are regarded as a map from strings of the original key type to the
 * original value type.
 *
 * @tparam KeyDigit The key type of the inner maps. The entire trie is then
 * regarded as a map from strings from {@code KeyDigit} to {@code Value}.
 * @tparam Value The value type. Must be containable in Maybe.
 * @tparam KeyComparator The comparator of key units. It must have a function
 * call operator that takes two (constant references to) key units and returns
 * true iff the first is less than the second.
 */
template<
        typename KeyDigit,
        typename Value,
        typename KeyComparator = std::less<KeyDigit>>
class Trie {

private:

    /**
     * Type of the map containing child nodes.
     *
     * It would be more desirable to have the children directly, but we need to
     * avoid recursion of incomplete types.
     */
    using ChildMap = std::map<KeyDigit, std::unique_ptr<Trie>, KeyComparator>;

public:

    using key_type = KeyDigit;
    using mapped_type = Value;
    using value_type = Trie;
    using size_type = typename ChildMap::size_type;
    using difference_type = typename ChildMap::difference_type;
    using key_compare = KeyComparator;
    // XXX Trie is not (yet) allocator-aware.
    // using allocator_type = ?;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    // XXX Trie is not (yet) allocator-aware.
    // using pointer = std::allocator_traits<Allocator>::pointer
    // using const_pointer = std::allocator_traits<Allocator>::const_pointer

private:

    /** Pointer to the parent node. Null if this node is the root node. */
    Trie *mParent;

    ChildMap mChildren;

    Maybe<Value> mValue;

    /**
     * The number of values that exist under this node, that is, the number of
     * descendant nodes (including this node itself) that have a value.
     */
    size_type mSize;

    Trie(Trie *parent, const key_compare &comp) :
            mParent(parent), mChildren(comp), mValue(), mSize(0) { }

    ChildMap &children() noexcept { return mChildren; }
    const ChildMap &children() const noexcept { return mChildren; }

public:

    /** Creates (the root node of) a new trie. */
    explicit Trie(const key_compare &comp = key_compare()) :
            Trie(nullptr, comp) { }

    // Copy constructor is not supported now.
    Trie(const Trie &) = delete;

    /** Move constructor */
    Trie(Trie &&t) :
            mParent(std::move(t.mParent)),
            mChildren(std::move(t.mChildren)),
            mValue(std::move(t.mValue)),
            mSize(std::move(t.mSize)) {
        for (std::pair<const KeyDigit, std::unique_ptr<Trie>> &c : mChildren) {
            c.second->mParent = this;
            t.mSize -= c.second->mSize;
        }
    }

    // Assignments are not supported now.
    Trie &operator=(const Trie &) = delete;
    Trie &operator=(Trie &&) = delete;

    /** Returns a copy of the key comparator. */
    key_compare key_comp() { return mChildren.key_comp(); }

    size_type max_size() const noexcept { return children().max_size(); }

    /** Returns the number of values in the entire trie. */
    size_type size() const noexcept { return mSize; }

    /** Checks if this node has no values. */
    bool empty() const noexcept { return size() == 0; }

private:

    void incrementSize(difference_type count = 1) noexcept {
        mSize += count;
        if (mParent != nullptr)
            mParent->incrementSize(count);
    }

public:

    /** Checks if this node has a value. */
    bool hasValue() const noexcept { return mValue.hasValue(); }

    /**
     * Returns a reference to the value of this node. The behavior is undefined
     * if this node has no value.
     */
    Value &value() { return mValue.value(); }
    /**
     * Returns a reference to the value of this node. The behavior is undefined
     * if this node has no value.
     */
    const Value &value() const { return mValue.value(); }

    /**
     * Returns a reference to the maybe object that may contain the value of
     * this node. The maybe is empty if this node has no value.
     *
     * There is no non-const version of this method because the trie
     * implementation has to keep track of the number of values.
     */
    const Maybe<Value> &maybeValue() const noexcept { return mValue; }

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
    std::pair<Value &, bool> emplaceValue(Arg &&... arg)
            noexcept(std::is_nothrow_constructible<Value, Arg...>::value) {
        if (hasValue())
            return std::pair<Value &, bool>(value(), false);

        mValue.emplace(std::forward<Arg>(arg)...);
        incrementSize();
        return std::pair<Value &, bool>(value(), true);
    }

    /**
     * Returns a reference to the value in this node. If the node has no value,
     * a new value is created by value initialization.
     */
    Value &getOrCreateValue()
            noexcept(std::is_nothrow_default_constructible<Value>::value) {
        return emplaceValue().first;
    }

    /** Erases the value of this node. */
    void eraseValue() noexcept {
        if (hasValue()) {
            mValue.clear();
            incrementSize(-1);
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
    std::pair<value_type &, bool> emplaceChild(Arg &&... arg) {
        std::pair<typename ChildMap::iterator, bool> p = mChildren.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(std::forward<Arg>(arg)...),
                std::make_tuple(nullptr));
        std::unique_ptr<value_type> &child = p.first->second;
        if (p.second) {
            try {
                child.reset(new Trie(this, key_comp()));
            } catch (...) {
                mChildren.erase(p.first);
                throw;
            }
        }
        return std::pair<value_type &, bool>(*child, p.second);
    }

    /**
     * Adds descendant nodes from the iterator. The each iterated value is
     * passed to {@link #emplaceChild} to create descendants.
     * @return reference to the last descendant.
     */
    template<typename InputIterator>
    value_type &emplaceDescendants(InputIterator begin, InputIterator end) {
        value_type *node = this;
        while (begin != end) {
            node = std::addressof(node->emplaceChild(*begin).first);
            ++begin;
        }
        return *node;
    }

    /**
     * Adds descendant nodes from the given key string. The key string must be
     * able to iterate key digits that can be passed to {@link #emplaceChild}
     * to create descendants.
     * @return reference to the last descendant.
     */
    template<typename KeyString>
    value_type &emplaceDescendants(const KeyString &ks) {
        return emplaceDescendants(std::begin(ks), std::end(ks));
    }

    /**
     * Returns a reference to the child node associated with the given key.
     *
     * If this node has no child node for the key, a new empty child node is
     * created with a key copy-constructed from the argument.
     */
    value_type &operator[](const key_type &k) {
        return emplaceChild(k).first;
    }
    /**
     * Returns a reference to the child node associated with the given key.
     *
     * If this node has no child node for the key, a new empty child node is
     * created with a key move-constructed from the argument.
     */
    value_type &operator[](key_type &&k) {
        return emplaceChild(std::move(k)).first;
    }

    /**
     * Returns a reference to the child node associated with the given key.
     * @throws std::out_of_range if there is no such child node.
     */
    value_type &at(const key_type &k) {
        return *mChildren.at(k);
    }
    /**
     * Returns a reference to the child node associated with the given key.
     * @throws std::out_of_range if there is no such child node.
     */
    const value_type &at(const key_type &k) const {
        return *mChildren.at(k);
    }

    /**
     * Removes all descendants of this node.
     *
     * All references and iterators pointing to any descendants of this node
     * are invalidated.
     */
    void eraseDescendants() noexcept {
        mChildren.clear();
    }

    /** Removes the value of this node and all descendants. */
    void clear() noexcept {
        eraseValue();
        eraseDescendants();
    }

    // For friend declaration in non-const traverser
    template<typename>
    class ConstTraverser;

    /**
     * Traverses a trie. Iterates all nodes of the trie in pre-order.
     *
     * @tparam KS The type of strings of keys. It will be used to represent a
     * sequence of keys on the path from the root node to the current node.
     * This type must be default-constructible and have the back, push_back,
     * and pop_back functions.
     *
     * @see ConstTraverser
     */
    template<typename KS = std::basic_string<KeyDigit>>
    class Traverser : public std::iterator<std::forward_iterator_tag, Trie> {

        // For conversion to const-traverser
        friend class ConstTraverser<KS>;

    public:

        using KeyString = KS;

    private:

        std::vector<typename ChildMap::iterator> mPathNodes;
        KeyString mPathString;
        /**
         * If this traverser is dereferenceable, the current node is non-null.
         * Otherwise, the current node is either the root node of the trie or
         * null.
         */
        Trie *mCurrentNode;
        /** A past-the-end traverser is not dereferenceable. */
        bool mIsDereferenceable;

    public:

        Traverser() :
                mPathNodes(),
                mPathString(),
                mCurrentNode(nullptr),
                mIsDereferenceable(false) { }

        explicit Traverser(Trie *t, bool isDereferenceable = true) :
                mPathNodes(),
                mPathString(),
                mCurrentNode(t),
                mIsDereferenceable(isDereferenceable) { }

        const KeyString &pathString() const { return mPathString; }

        Trie &operator*() const {
            assert(mCurrentNode != nullptr);
            return *mCurrentNode;
        }
        Trie *operator->() const {
            assert(mCurrentNode != nullptr);
            return mCurrentNode;
        }

    private:

        void down(typename ChildMap::iterator &&i) {
            mPathString.push_back(i->first);
            mCurrentNode = i->second.get();
            mPathNodes.push_back(std::move(i));
        }

        void forward() {
            assert(mIsDereferenceable);
            if (!mCurrentNode->children().empty())
                return down(mCurrentNode->children().begin());
            for (;;) {
                if (mPathNodes.empty()) {
                    mIsDereferenceable = false;
                    return;
                }
                Trie *parent = mCurrentNode->mParent;
                ++mPathNodes.back();
                if (mPathNodes.back() != parent->children().end()) {
                    mPathString.back() = mPathNodes.back()->first;
                    mCurrentNode = mPathNodes.back()->second.get();
                    return;
                }
                mPathNodes.pop_back();
                mPathString.pop_back();
                mCurrentNode = parent;
            }
        }

    public:

        Traverser &operator++() {
            forward();
            return *this;
        }

        Traverser operator++(int) {
            Traverser old = *this;
            forward();
            return old;
        }

        /**
         * Looks up the given key in the current node. If the current node has
         * no child node associated with the key, this method has no effect.
         * @return true iff a child was found for the key.
         */
        bool down(const KeyDigit &k) {
            assert(mIsDereferenceable);
            auto i = mCurrentNode->children().find(k);
            if (i == mCurrentNode->children().end())
                return false;
            down(std::move(i));
            return true;
        }

        size_type down(
                typename KeyString::const_iterator begin,
                typename KeyString::const_iterator end) {
            size_type count = 0;
            while (begin != end && down(*begin))
                ++begin, ++count;
            return count;
        }

        size_type down(const KeyString &ks) {
            return down(std::begin(ks), std::end(ks));
        }

        /**
         * Goes up to the nth ancestor node. If the current node has no that
         * many ancestors, stops at the root node.
         * @return number of ancestors passed.
         */
        size_type up(size_type count = 1) {
            assert(mIsDereferenceable);

            size_type actualCount = 0;
            while (actualCount < count && !mPathNodes.empty()) {
                mPathNodes.pop_back();
                mPathString.pop_back();
                mCurrentNode = mCurrentNode->mParent;
                ++actualCount;
            }
            return actualCount;
        }

        bool operator==(const Traverser &other) const {
            return this->mCurrentNode == other.mCurrentNode &&
                    this->mIsDereferenceable == other.mIsDereferenceable;
        }
        bool operator!=(const Traverser &other) const {
            return !(*this == other);
        }

    }; // template<typename KS> class Traverser

    /**
     * Traverses a trie. Iterates all nodes of the trie in pre-order.
     *
     * @tparam KS The type of strings of keys. It will be used to represent a
     * sequence of keys on the path from the root node to the current node.
     * This type must be default-constructible and have the back, push_back,
     * and pop_back functions.
     *
     * @see Traverser
     */
    template<typename KS = std::basic_string<KeyDigit>>
    class ConstTraverser :
            public std::iterator<std::forward_iterator_tag, const Trie> {

    public:

        using KeyString = KS;

    private:

        std::vector<typename ChildMap::const_iterator> mPathNodes;
        KeyString mPathString;
        /**
         * If this traverser is dereferenceable, the current node is non-null.
         * Otherwise, the current node is either the root node of the trie or
         * null.
         */
        const Trie *mCurrentNode;
        /** A past-the-end traverser is not dereferenceable. */
        bool mIsDereferenceable;

    public:

        ConstTraverser() :
                mPathNodes(),
                mPathString(),
                mCurrentNode(nullptr),
                mIsDereferenceable(false) { }

        explicit ConstTraverser(const Trie *t, bool isDereferenceable = true) :
                mPathNodes(),
                mPathString(),
                mCurrentNode(t),
                mIsDereferenceable(isDereferenceable) { }

        ConstTraverser(const Traverser<KS> &t) :
                mPathNodes(t.mPathNodes.begin(), t.mPathNodes.end()),
                mPathString(t.mPathString),
                mCurrentNode(t.mCurrentNode),
                mIsDereferenceable(t.mIsDereferenceable) { }

        ConstTraverser(Traverser<KS> &&t) :
                mPathNodes(
                        std::make_move_iterator(t.mPathNodes.begin()),
                        std::make_move_iterator(t.mPathNodes.end())),
                mPathString(std::move(t.mPathString)),
                mCurrentNode(std::move(t.mCurrentNode)),
                mIsDereferenceable(std::move(t.mIsDereferenceable)) { }

        const KeyString &pathString() const { return mPathString; }

        const Trie &operator*() const {
            assert(mCurrentNode != nullptr);
            return *mCurrentNode;
        }
        const Trie *operator->() const {
            assert(mCurrentNode != nullptr);
            return mCurrentNode;
        }

    private:

        void down(typename ChildMap::const_iterator &&i) {
            mPathString.push_back(i->first);
            mCurrentNode = i->second.get();
            mPathNodes.push_back(std::move(i));
        }

        void forward() {
            assert(mIsDereferenceable);
            if (!mCurrentNode->children().empty())
                return down(mCurrentNode->children().begin());
            for (;;) {
                if (mPathNodes.empty()) {
                    mIsDereferenceable = false;
                    return;
                }
                const Trie *parent = mCurrentNode->mParent;
                ++mPathNodes.back();
                if (mPathNodes.back() != parent->children().end()) {
                    mPathString.back() = mPathNodes.back()->first;
                    mCurrentNode = mPathNodes.back()->second.get();
                    return;
                }
                mPathNodes.pop_back();
                mPathString.pop_back();
                mCurrentNode = parent;
            }
        }

    public:

        ConstTraverser &operator++() {
            forward();
            return *this;
        }

        ConstTraverser operator++(int) {
            ConstTraverser old = *this;
            forward();
            return old;
        }

        /**
         * Looks up the given key in the current node. If the current node has
         * no child node associated with the key, this method has no effect.
         * @return true iff a child was found for the key.
         */
        bool down(const KeyDigit &k) {
            assert(mIsDereferenceable);
            auto i = mCurrentNode->children().find(k);
            if (i == mCurrentNode->children().end())
                return false;
            down(std::move(i));
            return true;
        }

        size_type down(
                typename KeyString::const_iterator begin,
                typename KeyString::const_iterator end) {
            size_type count = 0;
            while (begin != end && down(*begin))
                ++begin, ++count;
            return count;
        }

        size_type down(const KeyString &ks) {
            return down(std::begin(ks), std::end(ks));
        }

        /**
         * Goes up to the nth ancestor node. If the current node has no that
         * many ancestors, stops at the root node.
         * @return number of ancestors passed.
         */
        size_type up(size_type count = 1) {
            assert(mIsDereferenceable);

            size_type actualCount = 0;
            while (actualCount < count && !mPathNodes.empty()) {
                mPathNodes.pop_back();
                mPathString.pop_back();
                mCurrentNode = mCurrentNode->mParent;
                ++actualCount;
            }
            return actualCount;
        }

        bool operator==(const ConstTraverser &other) const {
            return this->mCurrentNode == other.mCurrentNode &&
                    this->mIsDereferenceable == other.mIsDereferenceable;
        }
        bool operator!=(const ConstTraverser &other) const {
            return !(*this == other);
        }

    }; // template<typename KS> class ConstTraverser

    template<typename KeyString = std::basic_string<KeyDigit>>
    Traverser<KeyString> traverserBegin() {
        return Traverser<KeyString>(this);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    Traverser<KeyString> traverserEnd() {
        return Traverser<KeyString>(this, false);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    ConstTraverser<KeyString> traverserBegin() const {
        return ConstTraverser<KeyString>(this);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    ConstTraverser<KeyString> traverserEnd() const {
        return ConstTraverser<KeyString>(this, false);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    ConstTraverser<KeyString> constTraverserBegin() const {
        return ConstTraverser<KeyString>(this);
    }
    template<typename KeyString = std::basic_string<KeyDigit>>
    ConstTraverser<KeyString> constTraverserEnd() const {
        return ConstTraverser<KeyString>(this, false);
    }

    /*
    using iterator = TrieTraverser<Trie>;
    using const_iterator = TrieTraverser<const Trie>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    // iterator: {,c}{,r}{begin,end}
    */

    /**
     * Creates (the root node of) a new trie and add values from the iterator.
     * The value type of the iterator must be {@code std::pair<key_string,
     * Value>}, where the key string type must be a container that iterates
     * KeyDigit.
     */
    template<typename InputIterator>
    Trie(
            InputIterator begin,
            InputIterator end,
            const key_compare &comp = key_compare()) :
            Trie(comp) {
        while (begin != end) {
            emplaceDescendants(begin->first).emplaceValue(begin->second);
            ++begin;
        }
    }

    /**
     * Creates (the root node of) a new trie and add values from the
     * initializer list.
     */
    Trie(
            std::initializer_list<
                    std::pair<const std::basic_string<KeyDigit>, Value>> init,
            const key_compare &comp = key_compare()) :
            Trie(std::begin(init), std::end(init), comp) { }

    ~Trie() noexcept {
        eraseValue(); // decrement size if has value
    }

};

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_Trie_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
