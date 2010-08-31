/* -*- Mode: C++; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 noexpandtab: */

/*******************************************************************************
 * FShell 2
 * Copyright 2009 Michael Tautschnig, tautschnig@forsyte.de
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

/*! \file fshell2/fql/evaluation/predicate_instrumentation.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:33:19 CET 2009 
*/

#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>
#include <fshell2/config/annotations.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#endif

#include <fshell2/fql/ast/predicate.hpp>

#include <cbmc/src/langapi/language_ui.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Predicate_Instrumentation::Predicate_Instrumentation(::language_uit & manager) :
	m_manager(manager)
{
}

target_graph_t::edge_t const& Predicate_Instrumentation::get(target_graph_t::node_t const& node,
		Predicate const* pred) const {
	node_to_pred_instrumentation_map_t::const_iterator entry1(m_node_to_pred_instr.find(node));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry1 != m_node_to_pred_instr.end());
	pred_instrumentation_map_t::const_iterator entry2(entry1->second.find(pred));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry2 != entry1->second.end());
	return entry2->second;
}

void Predicate_Instrumentation::insert_predicate(::goto_functionst & gf,
		node_set_t const& nodes, Predicate const* pred, CFA::edges_t & pred_edges) {
	using ::fshell2::instrumentation::GOTO_Transformation;
	
	::fshell2::instrumentation::GOTO_Transformation inserter(m_manager, gf);

	for (::std::set< target_graph_t::node_t >::iterator n_iter(nodes.begin());
			n_iter != nodes.end(); ++n_iter)
		if (m_node_to_pred_instr[ *n_iter ].end() ==
				m_node_to_pred_instr[ *n_iter ].find(pred)) {
			GOTO_Transformation::inserted_t const & res(inserter.insert_predicate_at(
						*n_iter, pred->get_expr()));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == res.size());
			goto_programt::targett next(res.front().second);
			++next;
			m_node_to_pred_instr[ *n_iter ][ pred ] = 
				*pred_edges.insert(::std::make_pair(res.front(), ::std::make_pair(n_iter->first, next))).first;
		}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

