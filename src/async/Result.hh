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

#ifndef INCLUDED_async_Result_hh
#define INCLUDED_async_Result_hh

#include "buildconfig.h"

#include <exception>
#include <memory>
#include "helpermacros.h"
#include "common/Variant.hh"

namespace sesh {
namespace async {

/**
 * Result of an asynchronous computation. It is either a value of the parameter
 * type or an exception.
 */
template<typename T>
class Result : public common::Variant<T, std::exception_ptr> {

public:

    using common::Variant<T, std::exception_ptr>::Variant;

    /** Checks if this value has an actual result rather than an exception. */
    bool hasValue() const noexcept {
        switch (this->index()) {
        case Result::template index<T>():
            return true;
        case Result::template index<std::exception_ptr>():
            return false;
        }
        UNREACHABLE();
    }

    operator bool() const noexcept {
        return hasValue();
    }

    /** Returns a reference to the result value or throws the exception. */
    T &operator*() {
        switch (this->index()) {
        case Result::template index<T>():
            return this->template value<T>();
        case Result::template index<std::exception_ptr>():
            std::rethrow_exception(this->template value<std::exception_ptr>());
        }
        UNREACHABLE();
    }

    /** Returns a reference to the result value or throws the exception. */
    const T &operator*() const {
        switch (this->index()) {
        case Result::template index<T>():
            return this->template value<T>();
        case Result::template index<std::exception_ptr>():
            std::rethrow_exception(this->template value<std::exception_ptr>());
        }
        UNREACHABLE();
    }

    /** Returns a pointer to the result value or throws the exception. */
    T *operator->() {
        return std::addressof(**this);
    }

    /** Returns a pointer to the result value or throws the exception. */
    const T *operator->() const {
        return std::addressof(**this);
    }

}; // template<typename T> class Result

} // namespace async
} // namespace sesh

#endif // #ifndef INCLUDED_async_Result_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
