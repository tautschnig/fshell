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

/*! \file fshell2/fql/evaluation/evaluate_coverage_pattern.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:34:05 CET 2009 
*/

#include <fshell2/fql/evaluation/evaluate_coverage_pattern.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>
#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_pattern.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
	
Evaluate_Coverage_Pattern::Evaluate_Coverage_Pattern(Evaluate_Filter const& eval_filter,
			Evaluate_Path_Pattern & pp_eval,
			::fshell2::instrumentation::CFG const& cfg) :
	m_eval_filter(eval_filter),
	m_pp_eval(pp_eval),
	m_cfg(cfg),
	m_test_goal_states(0),
	m_current_tg_states(&m_test_goal_states)
{
}
	
Evaluate_Coverage_Pattern::~Evaluate_Coverage_Pattern() {
}

trace_automaton_t const& Evaluate_Coverage_Pattern::get() const {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			m_test_goal_automaton.state_count() > 0);
	return m_test_goal_automaton;
}

Evaluate_Coverage_Pattern::Test_Goal_States const& Evaluate_Coverage_Pattern::get_test_goal_states() const {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, m_test_goal_states.m_cp);
	return m_test_goal_states;
}

bool Evaluate_Coverage_Pattern::is_test_goal_state(Test_Goal_States const& tgs, ta_state_t const& state) {
	if (tgs.m_tg_states.end() != tgs.m_tg_states.find(state)) return true;
	if (tgs.m_children.empty()) return false;
	
	for (::std::list< Test_Goal_States >::const_iterator iter(tgs.m_children.begin());
			iter != tgs.m_children.end(); ++iter)
		if (is_test_goal_state(*iter, state)) return true;
	
	return false;
}

void Evaluate_Coverage_Pattern::dfs_build(ta_state_t const& state, target_graph_t::node_t const& root, int const bound,
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
		m_test_goal_automaton.set_trans(state, m_pp_eval.to_index(m_more_target_graphs.back()), new_state);
		// DFS recursion
		dfs_build(new_state, s_iter->first, bound, nc_next, tgg);
	}
}

void Evaluate_Coverage_Pattern::visit(CP_Alternative const* n) {
	Test_Goal_States * m_current_tg_states_bak(m_current_tg_states);
	
	ta_state_set_t final_bak(m_current_final);
	m_current_tg_states_bak->m_children.push_back(Test_Goal_States(n->get_cp_a()));
	m_current_tg_states = &(m_current_tg_states_bak->m_children.back());
	n->get_cp_a()->accept(this);
	
	final_bak.swap(m_current_final);
	m_current_tg_states_bak->m_children.push_back(Test_Goal_States(n->get_cp_b()));
	m_current_tg_states = &(m_current_tg_states_bak->m_children.back());
	n->get_cp_b()->accept(this);
	m_current_final.insert(final_bak.begin(), final_bak.end());

	m_current_tg_states = m_current_tg_states_bak;
	m_current_tg_states->m_tg_states = m_current_final;
}

void Evaluate_Coverage_Pattern::visit(CP_Concat const* n) {
	Test_Goal_States * m_current_tg_states_bak(m_current_tg_states);
	
	m_current_tg_states_bak->m_children.push_back(Test_Goal_States(n->get_cp_a()));
	m_current_tg_states = &(m_current_tg_states_bak->m_children.back());
	n->get_cp_a()->accept(this);
	
	m_current_tg_states_bak->m_children.push_back(Test_Goal_States(n->get_cp_b()));
	m_current_tg_states = &(m_current_tg_states_bak->m_children.back());
	n->get_cp_b()->accept(this);
	
	m_current_tg_states = m_current_tg_states_bak;
	m_current_tg_states->m_tg_states = m_current_final;
}

void Evaluate_Coverage_Pattern::visit(Depcov const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}
			
void Evaluate_Coverage_Pattern::visit(Edgecov const* n) {
	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
			
	ta_state_t const new_state(m_test_goal_automaton.new_state());
	m_current_final.insert(new_state);
	m_current_tg_states->m_tg_states.insert(new_state);
	int const idx(m_pp_eval.to_index(m_eval_filter.get(*(n->get_filter_expr()))));
			
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter)
		m_test_goal_automaton.set_trans(*iter, idx, new_state);
}

void Evaluate_Coverage_Pattern::visit(Nodecov const* n) {
	FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, false);

#if 0
	old crap from edgecov
	for (test_goal_states_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter) {
		ta_state_t pred(0);
		if (1 == iter->size()) pred = *(iter->begin());
		else {
			pred = m_test_goal_automaton.new_state();
		}
		for (::std::vector< int >::const_iterator v_iter(new_tggs.begin());
				v_iter != new_tggs.end(); ++v_iter) {
			ta_state_t const new_state(m_test_goal_automaton.new_state());
			m_test_goal_automaton.set_trans(pred, *v_iter, new_state);
			ta_state_set_t tmp;
			tmp.insert(new_state);
			m_current_final.insert(tmp);
		}
	}
	
	target_graph_t const& tgg(m_eval_filter.get(*(n->get_filter_expr())));
	::std::vector< int > new_tggs;
	new_tggs.reserve(tgg.get_edges().size());
	for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
			e_iter != tgg.get_edges().end(); ++e_iter) {
		m_more_target_graphs.push_back(target_graph_t());
		target_graph_t::edges_t e;
		e.insert(*e_iter);
		m_more_target_graphs.back().set_edges(e);
		new_tggs.push_back(m_pp_eval.to_index(m_more_target_graphs.back()));
	}
#endif
}
			
void Evaluate_Coverage_Pattern::visit(Pathcov const* n) {
	FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, false);

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
			// TODO must not do epsilon here
			m_test_goal_automaton.set_trans(*iter, m_pp_eval.epsilon_index(), i_state);
			node_counts_t node_counts;
			node_counts[ *i_iter ] = 1;
			dfs_build(i_state, *i_iter, bound, node_counts, tgg);
		}
		// we currently don't properly deal with empty path sets
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented,
				m_current_final.end() == m_current_final.find(*iter));
	}
}

void Evaluate_Coverage_Pattern::visit(Predicate const* n) {
	FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, false);
#if 0
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_current_final.size());
	
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
#endif
}

void Evaluate_Coverage_Pattern::visit(Query const* n) {
	m_current_final.insert(m_test_goal_automaton.new_state());
	m_test_goal_automaton.initial().insert(*m_current_final.begin());
	
	m_current_tg_states->m_cp = n->get_cover();
	n->get_cover()->accept(this);

	for (ta_state_set_t::const_iterator iter(m_current_final.begin());
			iter != m_current_final.end(); ++iter)
		m_test_goal_automaton.final(*iter) = 1;
}

void Evaluate_Coverage_Pattern::visit(Quote const* n) {
	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
	
	trace_automaton_t const& pp(m_pp_eval.get(n->get_pp()));
	::std::pair< ta_state_set_t, ta_state_set_t > pp_init_final(
			copy_automaton(pp, m_test_goal_automaton));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == pp_init_final.first.size());
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter)
		m_test_goal_automaton.set_trans(*iter, m_pp_eval.epsilon_index(), *pp_init_final.first.begin());
	
	unsigned long const sc(m_test_goal_automaton.state_count());
	simplify_automaton(m_test_goal_automaton, false);
	if (m_test_goal_automaton.state_count() < sc) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_automaton.state_count() + 1 == sc);
		if (pp_init_final.second.end() != pp_init_final.second.find(*pp_init_final.first.begin())) {
			pp_init_final.second.erase(*pp_init_final.first.begin());
			pp_init_final.second.insert(prev_final.begin(), prev_final.end());
		}
	} else {
		if (pp_init_final.second.end() != pp_init_final.second.find(*pp_init_final.first.begin())) {
			pp_init_final.second.insert(prev_final.begin(), prev_final.end());
		}
	}
	m_current_final.swap(pp_init_final.second);
	m_current_tg_states->m_tg_states = m_current_final;
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

