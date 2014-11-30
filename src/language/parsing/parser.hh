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

/**
 * @file
 * This file contains definition of types that are used as input/output of
 * parser functions.
 */

#ifndef INCLUDED_language_parsing_parser_hh
#define INCLUDED_language_parsing_parser_hh

#include "buildconfig.h"

#include <locale>
#include <utility>
#include <vector>
#include "async/future.hh"
#include "common/empty.hh"
#include "common/function_helper.hh"
#include "language/source/stream.hh"
#include "ui/message/report.hh"

namespace sesh {
namespace language {
namespace parsing {

/** Data that may affect how source code is parsed. */
class context {

public:

    std::locale locale;

    /**
     * Dummy data. Will be replaced with pending_here_documents and aliases.
     */
    int dummy;

    // TODO pending_here_documents;

    // TODO aliases

}; // class context

/** State in syntax parsing. */
class state {

public:

    /** Rest of the source code to be parsed. */
    source::stream rest;

    class context context;

}; // class state

/** Result of a successful parse. */
template<typename T>
class product {

public:

    using result_type = T;

    result_type value;

    class state state;

}; // template<typename T> class product

/** Result of parsing. */
template<typename T>
class result {

public:

    using product_type = class product<T>;
    using result_type = typename product_type::result_type;

    /**
     * Optional result value. The maybe object is empty if parsing failed.
     * Otherwise, the product object contains the actual result and the updated
     * state that can be used to continue parsing the rest of the source code.
     */
    common::maybe<product_type> product;

    /**
     * Reports from the parser. If parsing failed with a fatal syntax error
     * that is not expected to be recovered by another parser, there should be
     * at least one {@link ui::message::category::error error} report. Reports
     * of other categories can be included, regardless of whether the parsing
     * was successful or not. Note that the reports might not be presented to
     * the human user because the result may be superseded by another result of
     * parse.
     */
    std::vector<ui::message::report> reports;

    /** Constructs a result without a product. */
    result() = default;

    /** Constructs a result with a product. */
    result(const product_type &p) : product(p), reports() { }

    /** Constructs a result with a product. */
    result(product_type &&p) : product(std::move(p)), reports() { }

    /** Constructs a result with a product and reports. */
    result(product_type &&p, decltype(reports) &&r) :
            product(std::move(p)), reports(std::move(r)) { }

    /** Constructs a result without a product but with reports. */
    result(common::empty, decltype(reports) &&r) :
            product(), reports(std::move(r)) { }

}; // template<typename T> class result

/**
 * Type of parsing functions.
 *
 * A parsing function takes a state as an argument, which may be modified by
 * the function, and returns a result in the future.
 */
template<typename T>
using parser = async::future<result<T>>(const state &);

// Just defined below
template<typename T>
class result_type_of;

template<typename T>
class result_type_of_result_of :
        public result_type_of<
                typename common::result_of<T(const state &)>::type> {
};

template<typename T>
class result_type_of_type {
public:
    using type = typename T::result_type;
};

template<typename T>
class result_type_of_type<async::future<T>> :
        public result_type_of_type<T> {
};

template<typename T>
class result_type_of :
        public std::conditional<
                common::is_callable<T(const state &)>::value,
                result_type_of_result_of<T>,
                result_type_of_type<typename std::decay<T>::type>
        >::type {
};

} // namespace parsing
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_parsing_parser_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
