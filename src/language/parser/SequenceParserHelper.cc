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
#include "SequenceParserHelper.hh"

#include <memory>
#include "language/parser/AndOrListParser.hh"
#include "language/parser/CommandParser.hh"
#include "language/parser/Environment.hh"
#include "language/parser/Parser.hh"
#include "language/parser/PipelineParser.hh"
#include "language/parser/SequenceParser.hh"
#include "language/syntax/AndOrList.hh"
#include "language/syntax/Command.hh"
#include "language/syntax/Pipeline.hh"
#include "language/syntax/Sequence.hh"

using sesh::language::syntax::AndOrList;
using sesh::language::syntax::Command;
using sesh::language::syntax::Pipeline;
using sesh::language::syntax::Sequence;

namespace sesh {
namespace language {
namespace parser {

namespace {

template<typename V>
using ParserPointer = std::unique_ptr<Parser<std::unique_ptr<V>>>;

ParserPointer<Command> commandParser(Environment &e) {
    return ParserPointer<Command>(new CommandParser(e));
}

ParserPointer<Pipeline> pipelineParser(Environment &e) {
    return ParserPointer<Pipeline>(new PipelineParser(e, commandParser));
}

ParserPointer<AndOrList> andOrListParser(Environment &e) {
    return ParserPointer<AndOrList>(new AndOrListParser(e, pipelineParser));
}

} // namespace

ParserPointer<Sequence> sequenceParser(Environment &e, LineMode lm) {
    return ParserPointer<Sequence>(new SequenceParser(e, andOrListParser, lm));
}

} // namespace parser
} // namespace language
} // namespace sesh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
