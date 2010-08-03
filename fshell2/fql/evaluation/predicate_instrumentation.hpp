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

#ifndef FSHELL2__FQL__EVALUATION__PREDICATE_INSTRUMENTATION_HPP
#define FSHELL2__FQL__EVALUATION__PREDICATE_INSTRUMENTATION_HPP

/*! \file fshell2/fql/evaluation/predicate_instrumentation.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:33:15 CET 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/concepts/cfa.hpp>
#include <fshell2/fql/concepts/target_graph.hpp>

#include <cbmc/src/goto-programs/goto_functions.h>

#include <map>
#include <set>

class language_uit;

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Predicate;

/*! \brief TODO
*/
class Predicate_Instrumentation
{
	/*! \copydoc doc_self
	*/
	typedef Predicate_Instrumentation Self;

	public:
	typedef ::std::set< target_graph_t::node_t > node_set_t;
	typedef ::std::map< Predicate const*, target_graph_t::edge_t > pred_instrumentation_map_t;
	typedef ::std::map< target_graph_t::node_t, pred_instrumentation_map_t > node_to_pred_instrumentation_map_t;

	Predicate_Instrumentation(::goto_functionst & gf, ::language_uit & manager);

	target_graph_t::edge_t const& get(target_graph_t::node_t const& node, Predicate const* pred) const;

	void insert_predicate(node_set_t const& nodes, Predicate const* pred, CFA::edges_t & pred_edges);

	private:
	::goto_functionst & m_gf;
	::language_uit & m_manager;
	::fshell2::instrumentation::GOTO_Transformation m_inserter;
	node_to_pred_instrumentation_map_t m_node_to_pred_instr;

	/*! \copydoc copy_constructor
	*/
	Predicate_Instrumentation( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__PREDICATE_INSTRUMENTATION_HPP */
