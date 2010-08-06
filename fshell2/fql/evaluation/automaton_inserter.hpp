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

#ifndef FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP
#define FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP

/*! \file fshell2/fql/evaluation/automaton_inserter.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:32:01 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/concepts/trace_automaton.hpp>
#include <fshell2/fql/evaluation/evaluate_coverage_pattern.hpp>

#include <list>
#include <map>

#include <cbmc/src/goto-programs/goto_functions.h>

class language_uit;

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

class CFG;

FSHELL2_INSTRUMENTATION_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

class Query;
class Evaluate_Path_Pattern;

/*! \brief TODO
*/
class Automaton_Inserter
{
	/*! \copydoc doc_self
	*/
	typedef Automaton_Inserter Self;

	public:
	typedef ::std::list< ::fshell2::instrumentation::GOTO_Transformation::goto_node_t > instrumentation_points_t;
	typedef ::std::map< ta_state_t, instrumentation_points_t > instrumentation_map_t;
	
	explicit Automaton_Inserter(::language_uit & manager);

	Evaluate_Coverage_Pattern::Test_Goal_States const& do_query(
			::goto_functionst & gf, ::fshell2::instrumentation::CFG & cfg, Query const& query);

	instrumentation_points_t const& get_test_goal_instrumentation(ta_state_t const& state) const;

	inline bool is_test_goal_state(ta_state_t const& s) const;
	inline trace_automaton_t const& get_tg_aut() const;

	private:
	typedef ::std::map< target_graph_t::edge_t, ::std::set< target_graph_t const* > > edge_to_target_graphs_t;
	typedef ::std::map< target_graph_t::node_t, ::std::set< target_graph_t const* > > node_to_target_graphs_t;
	typedef ::std::map< ::goto_programt::const_targett, edge_to_target_graphs_t > loc_to_edge_target_graphs_t; 
	typedef ::std::map< ::goto_programt::const_targett, node_to_target_graphs_t > loc_to_node_target_graphs_t; 
	typedef ::std::map< ta_state_t, ta_state_set_t > symbol_transition_map_t;
	typedef ::std::map< int, symbol_transition_map_t > transition_map_t;
	typedef ::std::map< target_graph_t const*, int > target_graph_to_int_t;

	::language_uit & m_manager;
	Evaluate_Coverage_Pattern m_cp_eval;
	Evaluate_Path_Pattern const& m_pp_eval;
	instrumentation_map_t m_tg_instrumentation_map;
	::std::map< ::goto_programt::const_targett, CFA::edge_t > m_target_edge_map;
	loc_to_node_target_graphs_t m_loc_to_node_target_graphs_map;
	loc_to_edge_target_graphs_t m_loc_to_edge_target_graphs_map;

	void insert(::goto_functionst & gf, ::fshell2::instrumentation::CFG const&
			cfg, char const * suffix, trace_automaton_t const& aut,
			::exprt & final_cond, bool const map_tg);

	void insert_function_calls(::goto_functionst & gf, ::goto_programt & body,
			::fshell2::instrumentation::CFG const& cfg, char const * suffix,
			bool const map_tg, target_graph_to_int_t const& local_target_graph_map);

	static void prepare_assertion(ta_state_set_t const& compact_final,
			::exprt & final_cond, ::symbolt const& state_symb);

	void add_transition_function(::goto_functionst & gf, ::std::string const& func_name,
			symbol_transition_map_t const& map, ::symbolt const& state_symb,
			ta_state_map_t const& reverse_state_map, bool const map_tg);

	/*! \copydoc copy_constructor
	*/
	Automaton_Inserter( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
inline bool Automaton_Inserter::is_test_goal_state(ta_state_t const& s) const {
	return m_cp_eval.is_test_goal_state(s);
}

inline trace_automaton_t const& Automaton_Inserter::get_tg_aut() const {
	return m_cp_eval.get();
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP */
