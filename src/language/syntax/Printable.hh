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

#ifndef INCLUDED_language_syntax_Printable_hh
#define INCLUDED_language_syntax_Printable_hh

namespace sesh {
namespace language {
namespace syntax {

class Printer;

/**
 * Printable objects represent (part of) abstract syntax trees that can be
 * converted to string representations using printers.
 *
 * A valid printable object must print a string that will be parsed to an
 * equivalent syntax tree. If an object is invalid (in the sense that no shell
 * command string is parsed to the invalid object), then the object is not
 * required to print a syntactically correct string.
 */
class Printable {

public:

    Printable() = default;
    Printable(const Printable &) = default;
    Printable(Printable &&) = default;
    Printable &operator=(const Printable &) = default;
    Printable &operator=(Printable &&) = default;
    virtual ~Printable() = default;

    virtual void print(Printer &) const = 0;

}; // class Printable

inline Printer &operator<<(Printer &printer, const Printable &printable) {
    printable.print(printer);
    return printer;
}

} // namespace syntax
} // namespace language
} // namespace sesh

#endif // #ifndef INCLUDED_language_syntax_Printable_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
