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

#include "buildconfig.h"
#include "conditional_pipeline.hh"

#include <stdexcept>
#include <utility>
#include "common/xchar.hh"
#include "language/syntax/pipeline.hh"
#include "language/syntax/Printer.hh"

namespace sesh {
namespace language {
namespace syntax {

conditional_pipeline::conditional_pipeline(condition_type c) :
        m_condition(c), m_pipeline(new Pipeline) { }

conditional_pipeline::conditional_pipeline(
        condition_type c, pipeline_pointer &&p) :
        m_condition(c), m_pipeline(std::move(p)) {
    if (m_pipeline == nullptr)
        m_pipeline.reset(new Pipeline);
}

void conditional_pipeline::print(Printer &p) const {
    switch (condition()) {
    case condition_type::and_then:  p << L("&&");  break;
    case condition_type::or_else:   p << L("||");  break;
    }
    p.breakLine();
    p.printIndent();
    p << *m_pipeline;
}

} // namespace syntax
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
