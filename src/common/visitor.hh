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

#ifndef INCLUDED_common_visitor_hh
#define INCLUDED_common_visitor_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace sesh {
namespace common {

template<typename... T>
class visitor; // defined just below

template<typename T>
class visitor<T> {

public:

    virtual void operator()(const T &) const = 0;

}; // template<typename T> class visitor<T>

/**
 * A template for an abstract class that has methods that operate on their
 * argument.
 * @tparam T The types of objects this visitor operates on. They can be
 * incomplete when this class template is instantiated.
 */
template<typename... T>
class visitor : public visitor<T>... { };

namespace visitor_impl {

template<typename F, typename B, typename... T>
class visitor_value;

template<typename F, typename B>
class visitor_value<F, B> : public B {

public:

    using value_type = F;

    value_type value;

    template<
            typename... A,
            typename = typename std::enable_if<
                    std::is_constructible<value_type, A...>::value>::type>
    explicit visitor_value(A &&... a)
            noexcept(std::is_nothrow_constructible<value_type, A...>::value) :
            value(std::forward<A>(a)...) { }

}; // class visitor_value<F, B>

template<typename F, typename B, typename H, typename... T>
class visitor_value<F, B, H, T...> : public visitor_value<F, B, T...> {

public:

    using visitor_value<F, B, T...>::visitor_value;

    void operator()(const H &v) const final override { this->value(v); }

}; // class visitor_value<F, B, H, T...>

} // namespace visitor_impl

/**
 * A template for a concrete visitor class that wraps a callable value that
 * actually performs desired operation on the visited object.
 * @tparam F The type of the callable object that is wrapped in this class. It
 * must be callable with an argument of type <code>const T &</code> for all
 * <code>T</code>s.
 * @tparam T The types of objects this visitor operates on.
 */
template<typename F, typename... T>
class visitor_value :
        public visitor_impl::visitor_value<F, visitor<T...>, T...> {

public:

    using visitor_impl::visitor_value<F, visitor<T...>, T...>::visitor_value;

}; // template<typename F, typename... T> class visitor_value

template<typename, typename>
class visitor_value_type;

template<typename F, typename... T>
class visitor_value_type<F, visitor<T...>> {
public:
    using type = visitor_value<F, T...>;
};

/**
 * A template for an abstract class that is visited by a visitor.
 * @tparam Visitor The type of the visitor. The visitor type may be incomplete
 * when this class template is instantiated.
 */
template<typename Visitor>
class visitable {

public:

    /** The type of the visitor. */
    using visitor_type = Visitor;

    /**
     * Calls the argument visitor passing an appropriate argument (normally
     * <code>*this</code>).
     */
    virtual void apply(const visitor_type &) const = 0;

}; // template<typename Visitor> class visitable

/**
 * A template for a concrete visitable class that wraps a value that is passed
 * to the visitor.
 *
 * @tparam Visitor The type of the visitor. It must be callable with an
 * argument of type <code>const Value &</code>.
 * @tparam Value The type of the value that is wrapped in this class and passed
 * to the visitor.
 */
template<typename Visitor, typename Value>
class visitable_value : public visitable<Visitor> {

public:

    using value_type = Value;

    value_type value;

    /**
     * Constructs the wrapped value by passing all the arguments to its
     * constructor.
     */
    template<
            typename... A,
            typename = typename std::enable_if<
                    std::is_constructible<value_type, A...>::value>::type>
    explicit visitable_value(A &&... a)
            noexcept(std::is_nothrow_constructible<value_type, A...>::value) :
            value(std::forward<A>(a)...) { }

    void apply(const Visitor &v) const final override {
        const visitor<value_type> &v2 = v;
        v2(value);
    }

}; // template<typename Visitor, typename Value> class visitable_value

/**
 * Returns the unique shared pointer to a new visitable_value.
 * @tparam Visitable The visitable type.
 * @tparam Value The type of the argument.
 * @tparam Result The type of the new visitable_value object.
 * @param v An object from which the object wrapped in the new visitable_value
 * is constructed.
 */
template<
        typename Visitable,
        typename Value,
        typename Result = visitable_value<
                typename Visitable::visitor_type,
                typename std::decay<Value>::type>>
std::shared_ptr<Result> make_shared_visitable(Value &&v) {
    return std::make_shared<Result>(std::forward<Value>(v));
}

/**
 * Constructs a visitor_value that wraps a reference to the argument operator
 * and applies it to the visitable value.
 */
template<typename Visitor, typename Operator>
void visit(const visitable<Visitor> &value, const Operator &op) {
    using visitor_type = typename visitor_value_type<
            std::reference_wrapper<const Operator>, Visitor>::type;
    value.apply(visitor_type(op));
}

} // namespace common
} // namespace sesh

#endif // #ifndef INCLUDED_common_visitor_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
