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

/*! \file fshell2/fql/evaluation/build_test_goal_automaton.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:34:05 CET 2009 
*/

#include <fshell2/fql/evaluation/build_test_goal_automaton.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_postcondition.hpp>
#include <fshell2/fql/ast/tgs_precondition.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>
#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
	
Build_Test_Goal_Automaton::Build_Test_Goal_Automaton(Evaluate_Filter const& eval_filter,
			Evaluate_Path_Monitor & pm_eval,
			Predicate_Instrumentation const& pred_instr,
			::fshell2::instrumentation::CFG const& cfg) :
	m_eval_filter(eval_filter),
	m_pm_eval(pm_eval),
	m_pred_instr(pred_instr),
	m_cfg(cfg),
	m_test_goal_map_entry(m_test_goal_map.end())
{
}

trace_automaton_t const& Build_Test_Goal_Automaton::get() const {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			m_test_goal_automaton.state_count() > 0);
	return m_test_goal_automaton;
}

Build_Test_Goal_Automaton::test_goal_states_t const& Build_Test_Goal_Automaton::get_test_goal_states(
		Test_Goal_Sequence::seq_entry_t const& s) const {
	test_goal_map_t::const_iterator entry(m_test_goal_map.find(&s));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_test_goal_map.end());
	return entry->second;
}
			
void Build_Test_Goal_Automaton::visit(Edgecov const* n) {
	if (n->get_predicates()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	}

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());

	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
	m_current_final.insert(m_test_goal_automaton.new_state());

	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter)
		m_test_goal_automaton.set_trans(*iter, m_pm_eval.to_index(m_eval_filter.get(*(n->get_filter_expr()))),
				*m_current_final.begin());
	
	m_test_goal_map_entry->second.insert(*m_current_final.begin());
	m_reverse_test_goal_map.insert(::std::make_pair(*m_current_final.begin(), m_test_goal_map_entry));
}
			
void Build_Test_Goal_Automaton::dfs_build(ta_state_t const& state, target_graph_t::node_t const& root, int const bound,
		node_counts_t const& nc, target_graph_t const& tgg) {
	::fshell2::instrumentation::CFG::entries_t::const_iterator cfg_node(m_cfg.find(root.second));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.end());
	// the end? that's ok as well
	if (cfg_node->second.successors.empty()) {
		m_current_final.insert(state);
		return;
	}

	for (::fshell2::instrumentation::CFG::successors_t::const_iterator s_iter(
				cfg_node->second.successors.begin());
			s_iter != cfg_node->second.successors.end(); ++s_iter) {
		// maybe the root statement is to be ignored, then just keep going but
		// don't add anything
		if (Evaluate_Filter::ignore_instruction(*(root.second))) {
			// maybe this is the end, maybe we'll find some more; if we do,
			// we'll pass this state anyway, but just be sure we have something.
			m_current_final.insert(state);
			dfs_build(state, s_iter->first, bound, nc, tgg);
			continue;
		}
		node_counts_t::const_iterator nc_iter(nc.find(s_iter->first));
		// test the bound
		if (nc.end() != nc_iter && nc_iter->second == bound) {
			m_current_final.insert(state);
			continue;
		}
		target_graph_t::edge_t new_edge(::std::make_pair(root, s_iter->first));
		// check target graph
		if (tgg.get_edges().end() == tgg.get_edges().find(new_edge)) {
			m_current_final.insert(state);
			continue;
		}
		// keep going, we're within the limits
		// create a new target graph
		m_more_target_graphs.push_back(target_graph_t());
		target_graph_t::edges_t e;
		e.insert(new_edge);
		m_more_target_graphs.back().set_edges(e);
		// make new node count map
		node_counts_t nc_next(nc);
		if (nc.end() == nc_iter)
			nc_next[ s_iter->first ] = 1;
		else
			++(nc_next[ s_iter->first ]);
		// next automaton state
		ta_state_t const new_state(m_test_goal_automaton.new_state());
		m_test_goal_automaton.set_trans(state, m_pm_eval.to_index(m_more_target_graphs.back()), new_state);
		// DFS recursion
		dfs_build(new_state, s_iter->first, bound, nc_next, tgg);
	}
}

void Build_Test_Goal_Automaton::visit(Pathcov const* n) {
	if (n->get_predicates()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	}

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());

	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);

	// for each initial state to a depth-first traversal of the CFG as long as
	// the edge remains within the given target graph; each edge yields a new
	// target graph that only has a single edge; set reverse lookups (move into
	// this class, do after collecting relevant tggs)

	int const bound(n->get_bound());
	target_graph_t const& tgg(m_eval_filter.get(*(n->get_filter_expr())));
	target_graph_t::initial_states_t const& i_states(tgg.get_initial_states());
	
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter) {
		for (target_graph_t::initial_states_t::const_iterator i_iter(i_states.begin());
				i_iter != i_states.end(); ++i_iter) {
			ta_state_t const i_state(m_test_goal_automaton.new_state());
			m_test_goal_automaton.set_trans(*iter, m_pm_eval.epsilon_index(), i_state);
			node_counts_t node_counts;
			node_counts[ *i_iter ] = 1;
			dfs_build(i_state, *i_iter, bound, node_counts, tgg);
		}
		// we currently don't properly deal with empty path sets
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented,
				m_current_final.end() == m_current_final.find(*iter));
	}

	for (ta_state_set_t::const_iterator iter(m_current_final.begin());
			iter != m_current_final.end(); ++iter) {
		m_test_goal_map_entry->second.insert(*iter);
		m_reverse_test_goal_map.insert(::std::make_pair(*iter, m_test_goal_map_entry));
	}
}

void Build_Test_Goal_Automaton::visit(Query const* n) {
	m_current_final.insert(m_test_goal_automaton.new_state());
	m_test_goal_automaton.initial().insert(*m_current_final.begin());
	
	n->get_cover()->accept(this);

	for (ta_state_set_t::const_iterator iter(m_current_final.begin());
			iter != m_current_final.end(); ++iter)
		m_test_goal_automaton.final(*iter) = 1;

	FSHELL2_AUDIT_TRACE("Finalizing test goal automaton");
	simplify_automaton(m_test_goal_automaton, false);
}

void Build_Test_Goal_Automaton::visit(Statecov const* n) {
	if (n->get_predicates()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	}

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());
	
	FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, false);
}

void Build_Test_Goal_Automaton::visit(TGS_Intersection const* n) {
	FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, false);

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());
}

void Build_Test_Goal_Automaton::visit(TGS_Postcondition const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());
	
	n->get_tgs()->accept(this);

	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
	m_current_final.insert(m_test_goal_automaton.new_state());
	
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter) {
		target_graph_t::edges_t pred_edges;
		::std::set< int > tgg_indices;
		find_non_eps_pred(m_test_goal_automaton, *iter, tgg_indices);
		for (::std::set< int >::const_iterator i_iter(tgg_indices.begin());
				i_iter != tgg_indices.end(); ++i_iter) {
			target_graph_t const& tgg(m_pm_eval.lookup_index(*i_iter));
			for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
					e_iter != tgg.get_edges().end(); ++e_iter)
				pred_edges.insert(m_pred_instr.get(e_iter->second, n->get_predicate()));
		}
		m_more_target_graphs.push_back(target_graph_t());
		m_more_target_graphs.back().set_edges(pred_edges);
		m_test_goal_automaton.set_trans(*iter,
				m_pm_eval.to_index(m_more_target_graphs.back()), *m_current_final.begin());
	}
}

void Build_Test_Goal_Automaton::visit(TGS_Precondition const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());

	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
	ta_state_t const tmp_final(m_test_goal_automaton.new_state());
	m_current_final.insert(tmp_final);
	
	n->get_tgs()->accept(this);
	
	target_graph_t::edges_t pred_edges;
	::std::set< int > tgg_indices;
	find_non_eps_succ(m_test_goal_automaton, tmp_final, tgg_indices);
	for (::std::set< int >::const_iterator iter(tgg_indices.begin());
			iter != tgg_indices.end(); ++iter) {
		target_graph_t const& tgg(m_pm_eval.lookup_index(*iter));
		for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
				e_iter != tgg.get_edges().end(); ++e_iter)
			pred_edges.insert(m_pred_instr.get(e_iter->first, n->get_predicate()));
	}
	m_more_target_graphs.push_back(target_graph_t());
	m_more_target_graphs.back().set_edges(pred_edges);
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter)
		m_test_goal_automaton.set_trans(*iter,
				m_pm_eval.to_index(m_more_target_graphs.back()), tmp_final);
}

void Build_Test_Goal_Automaton::visit(TGS_Setminus const* n) {
	FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, false);

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());
}

void Build_Test_Goal_Automaton::visit(TGS_Union const* n) {
	FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, false);

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());
}

void Build_Test_Goal_Automaton::visit(Test_Goal_Sequence const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !m_current_final.empty());
	
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		::std::pair< ta_state_set_t, ta_state_set_t > pm_init_final(
				copy_automaton(m_pm_eval.get(iter->first), m_test_goal_automaton));
		for (ta_state_set_t::const_iterator iter1(m_current_final.begin());
				iter1 != m_current_final.end(); ++iter1)
			for (ta_state_set_t::const_iterator iter2(pm_init_final.first.begin());
					iter2 != pm_init_final.first.end(); ++iter2)
				m_test_goal_automaton.set_trans(*iter1, m_pm_eval.epsilon_index(), *iter2);
		m_current_final.swap(pm_init_final.second);

		if (m_current_final.size() > 1) {
			ta_state_t new_final(m_test_goal_automaton.new_state());
			for (ta_state_set_t::const_iterator iter1(m_current_final.begin());
					iter1 != m_current_final.end(); ++iter1)
				m_test_goal_automaton.set_trans(*iter1, m_pm_eval.epsilon_index(), new_final);
			m_current_final.clear();
			m_current_final.insert(new_final);
		}

		m_test_goal_map_entry = m_test_goal_map.insert(::std::make_pair(
					&(*iter), test_goal_states_t())).first;
		iter->second->accept(this);
	}
		
	::std::pair< ta_state_set_t, ta_state_set_t > pm_init_final(
			copy_automaton(m_pm_eval.get(n->get_suffix_monitor()), m_test_goal_automaton));
	for (ta_state_set_t::const_iterator iter1(m_current_final.begin());
			iter1 != m_current_final.end(); ++iter1)
		for (ta_state_set_t::const_iterator iter2(pm_init_final.first.begin());
				iter2 != pm_init_final.first.end(); ++iter2)
			m_test_goal_automaton.set_trans(*iter1, m_pm_eval.epsilon_index(), *iter2);
	m_current_final.swap(pm_init_final.second);
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

