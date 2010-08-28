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

/*! \file fshell2/fql/evaluation/evaluate_path_pattern.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:29:47 CEST 2009 
*/

#include <fshell2/fql/evaluation/evaluate_path_pattern.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pp_alternative.hpp>
#include <fshell2/fql/ast/pp_concat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
		
Evaluate_Path_Pattern::Evaluate_Path_Pattern(::language_uit & manager) :
	m_eval_filter(manager),
	m_cfg(0),
	m_pp_map(),
	m_entry(m_pp_map.end(), false)
{
}

Evaluate_Path_Pattern::~Evaluate_Path_Pattern() {
}
	
void Evaluate_Path_Pattern::do_query(::goto_functionst & gf,
		::fshell2::instrumentation::CFG & cfg, Query const& query) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !m_cfg);
	m_cfg = &cfg;
	m_pp_map.clear();
	m_entry = ::std::make_pair(m_pp_map.end(), false);
	m_target_graph_index.clear();
	m_more_target_graphs.clear();
	
	// prepare filter evaluation
	// may create a predicated CFA and add predicate edges
	m_eval_filter.do_query(gf, cfg, query);
	
	m_target_graph_index.init(&(m_eval_filter.get(*(Filter_Function::Factory::get_instance().create<F_IDENTITY>()))));
	query.accept(this);

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			m_pp_map.end() != m_pp_map.find(query.get_passing()));
	m_cfg = 0;
}

trace_automaton_t const& Evaluate_Path_Pattern::get(Path_Pattern_Expr const* pm) const {
	pp_value_t::const_iterator entry(m_pp_map.find(pm));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_pp_map.end());
	return entry->second;
}

void Evaluate_Path_Pattern::dfs_build(trace_automaton_t & ta, ta_state_t const& state,
		target_graph_t::node_t const& root, int const bound,
		node_counts_t const& nc, target_graph_t const& tgg) {
	
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_cfg);
	::fshell2::instrumentation::CFG::entries_t::const_iterator cfg_node(m_cfg->find(root.second));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
	// the end? that's ok as well
	if (cfg_node->second.successors.empty()) {
		ta.final(state) = 1;
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
			ta.final(state) = 1;
			dfs_build(ta, state, s_iter->first, bound, nc, tgg);
			continue;
		}
		node_counts_t::const_iterator nc_iter(nc.find(s_iter->first));
		// test the bound
		if (nc.end() != nc_iter && nc_iter->second == bound) {
			ta.final(state) = 1;
			continue;
		}
		target_graph_t::edge_t new_edge(::std::make_pair(root, s_iter->first));
		// check target graph
		if (tgg.get_E().end() == tgg.get_E().find(new_edge)) {
			ta.final(state) = 1;
			continue;
		}
		// keep going, we're within the limits
		// create a new target graph
		m_more_target_graphs.push_back(target_graph_t());
		target_graph_t::edges_t e;
		e.insert(new_edge);
		m_more_target_graphs.back().set_E(e);
		// make new node count map
		node_counts_t nc_next(nc);
		if (nc.end() == nc_iter)
			nc_next[ s_iter->first ] = 1;
		else
			++(nc_next[ s_iter->first ]);
		// next automaton state
		ta_state_t const new_state(ta.new_state());
		ta.set_trans(state, m_target_graph_index.to_index(&(m_more_target_graphs.back())), new_state);
		// DFS recursion
		dfs_build(ta, new_state, s_iter->first, bound, nc_next, tgg);
	}
}

void Evaluate_Path_Pattern::visit(CP_Alternative const* n) {
	n->get_cp_a()->accept(this);
	n->get_cp_b()->accept(this);
}
	
void Evaluate_Path_Pattern::visit(CP_Concat const* n) {
	n->get_cp_a()->accept(this);
	n->get_cp_b()->accept(this);
}

void Evaluate_Path_Pattern::visit(Depcov const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Path_Pattern::visit(Edgecov const* n) {
	m_entry = m_pp_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	trace_automaton_t & current_aut(m_entry.first->second);
	
	ta_state_t const current_initial(current_aut.new_state());
	current_aut.initial().insert(current_initial);
	ta_state_t const current_final(current_aut.new_state());
	current_aut.final(current_final) = 1;
		
	target_graph_t const* tgg(&(m_eval_filter.get(*(n->get_filter_expr()))));
	// discard disconnected nodes, if any
	if (!tgg->get_disconnected_nodes().empty()) {
		m_more_target_graphs.push_back(target_graph_t());
		target_graph_t::edges_t e(tgg->get_E());
		target_graph_t::initial_states_t is(tgg->get_I());
		m_more_target_graphs.back().set_E(e);
		m_more_target_graphs.back().set_I(is);
		tgg = &(m_more_target_graphs.back());
	}

	current_aut.set_trans(current_initial, m_target_graph_index.to_index(tgg),
			current_final);
}
	
void Evaluate_Path_Pattern::visit(Nodecov const* n) {
	m_entry = m_pp_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	trace_automaton_t & current_aut(m_entry.first->second);
	
	ta_state_t const current_initial(current_aut.new_state());
	current_aut.initial().insert(current_initial);
	ta_state_t const current_final(current_aut.new_state());
	current_aut.final(current_final) = 1;
		
	m_more_target_graphs.push_back(target_graph_t());
	target_graph_t::nodes_t nodes(m_eval_filter.get(*(n->get_filter_expr())).get_L());
	m_more_target_graphs.back().set_disconnected_nodes(nodes);

	current_aut.set_trans(current_initial,
			m_target_graph_index.to_index(&(m_more_target_graphs.back())),
			current_final);
}
	
void Evaluate_Path_Pattern::visit(PP_Alternative const* n) {
	m_entry = m_pp_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pp_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_pp_a()->accept(this);
	::std::pair< pp_value_t::iterator, bool > entry_a(m_entry);
	n->get_pp_b()->accept(this);
	::std::pair< pp_value_t::iterator, bool > entry_b(m_entry);

	::std::pair< ta_state_set_t, ta_state_set_t > const a_init_final(
				copy_automaton(entry_a.first->second, current_aut));
	::std::pair< ta_state_set_t, ta_state_set_t > const b_init_final(
				copy_automaton(entry_b.first->second, current_aut));

	ta_state_t const current_initial(current_aut.new_state());
	current_aut.initial().insert(current_initial);
	ta_state_t const current_final(current_aut.new_state());
	current_aut.final(current_final) = 1;
	
	for (ta_state_set_t::const_iterator iter(a_init_final.first.begin());
			iter != a_init_final.first.end(); ++iter)
		current_aut.set_trans(current_initial, m_target_graph_index.epsilon_index(), *iter);
	for (ta_state_set_t::const_iterator iter(a_init_final.second.begin());
			iter != a_init_final.second.end(); ++iter)
		current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), current_final);
	for (ta_state_set_t::const_iterator iter(b_init_final.first.begin());
			iter != b_init_final.first.end(); ++iter)
		current_aut.set_trans(current_initial, m_target_graph_index.epsilon_index(), *iter);
	for (ta_state_set_t::const_iterator iter(b_init_final.second.begin());
			iter != b_init_final.second.end(); ++iter)
		current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), current_final);

	m_entry = entry;
}

void Evaluate_Path_Pattern::visit(PP_Concat const* n) {
	m_entry = m_pp_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pp_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_pp_a()->accept(this);
	::std::pair< pp_value_t::iterator, bool > entry_a(m_entry);
	n->get_pp_b()->accept(this);
	::std::pair< pp_value_t::iterator, bool > entry_b(m_entry);

	::std::pair< ta_state_set_t, ta_state_set_t > a_init_final(
				copy_automaton(entry_a.first->second, current_aut));
	::std::pair< ta_state_set_t, ta_state_set_t > const b_init_final(
				copy_automaton(entry_b.first->second, current_aut));
	
	current_aut.initial().swap(a_init_final.first);
	for (ta_state_set_t::const_iterator iter(b_init_final.second.begin());
			iter != b_init_final.second.end(); ++iter)
		current_aut.final(*iter) = 1;

	for (ta_state_set_t::const_iterator iter(a_init_final.second.begin());
			iter != a_init_final.second.end(); ++iter)
		for (ta_state_set_t::const_iterator iter2(b_init_final.first.begin());
				iter2 != b_init_final.first.end(); ++iter2)
			current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), *iter2);

	m_entry = entry;
}

void Evaluate_Path_Pattern::visit(Pathcov const* n) {
	m_entry = m_pp_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	trace_automaton_t & current_aut(m_entry.first->second);
	
	ta_state_t const current_initial(current_aut.new_state());
	current_aut.initial().insert(current_initial);
	
	// for each initial state do a depth-first traversal of the CFG as long as
	// the edge remains within the given target graph; each edge yields a new
	// target graph that only has a single edgei

	int const bound(n->get_bound());
	target_graph_t const& tgg(m_eval_filter.get(*(n->get_filter_expr())));
	target_graph_t::initial_states_t const& i_states(tgg.get_I());
	
	for (target_graph_t::initial_states_t::const_iterator iter(i_states.begin());
			iter != i_states.end(); ++iter) {
		ta_state_t const i_state(current_aut.new_state());
		current_aut.set_trans(current_initial, m_target_graph_index.epsilon_index(), i_state);
		node_counts_t node_counts;
		node_counts[ *iter ] = 1;
		dfs_build(current_aut, i_state, *iter, bound, node_counts, tgg);
		// if initial state has no successor then remove it from accepting
		// states
		if (current_aut.final(i_state)) current_aut.final(i_state) = 0;
	}
}
	
void Evaluate_Path_Pattern::visit(Predicate const* n) {
	m_entry = m_pp_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	trace_automaton_t & current_aut(m_entry.first->second);
	
	ta_state_t const current_initial(current_aut.new_state());
	current_aut.initial().insert(current_initial);
	ta_state_t const current_final(current_aut.new_state());
	current_aut.final(current_final) = 1;

	current_aut.set_trans(current_initial,
			m_target_graph_index.to_index(&(m_eval_filter.get(*n))),
			current_final);
}
	
void Evaluate_Path_Pattern::visit(Query const* n) {
	n->get_cover()->accept(this);

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n->get_passing());
	n->get_passing()->accept(this);
	simplify_automaton(m_entry.first->second, true);
}

void Evaluate_Path_Pattern::visit(Quote const* n) {
	n->get_pp()->accept(this);
	simplify_automaton(m_entry.first->second, true);
}

void Evaluate_Path_Pattern::visit(Repeat const* n) {
	m_entry = m_pp_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pp_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_pp()->accept(this);
	::std::pair< pp_value_t::iterator, bool > entry_sub(m_entry);
	
	int lb(n->get_lower_bound());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, lb >= 0);
	int ub(n->get_upper_bound());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, -1 == ub || ub >= lb);

	ta_state_set_t initial, final;

	while (lb > 0) {
		--lb;
		if (ub != -1) --ub;

		::std::pair< ta_state_set_t, ta_state_set_t > sub_init_final(
				copy_automaton(entry_sub.first->second, current_aut));
		if (initial.empty())
			initial.swap(sub_init_final.first);
		else
			for (ta_state_set_t::const_iterator iter(final.begin());
					iter != final.end(); ++iter)
				for (ta_state_set_t::const_iterator iter2(sub_init_final.first.begin());
						iter2 != sub_init_final.first.end(); ++iter2)
					current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), *iter2);

		final.swap(sub_init_final.second);
	}
	
	if (-1 == ub) {
		::std::pair< ta_state_set_t, ta_state_set_t > sub_init_final(
				copy_automaton(entry_sub.first->second, current_aut));
		
		for (ta_state_set_t::const_iterator iter(sub_init_final.first.begin());
				iter != sub_init_final.first.end(); ++iter)
			for (ta_state_set_t::const_iterator iter2(sub_init_final.second.begin());
					iter2 != sub_init_final.second.end(); ++iter2) {
				current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), *iter2);
				current_aut.set_trans(*iter2, m_target_graph_index.epsilon_index(), *iter);
			}
		
		if (initial.empty())
			initial.swap(sub_init_final.first);
		else
			for (ta_state_set_t::const_iterator iter(final.begin());
					iter != final.end(); ++iter)
				for (ta_state_set_t::const_iterator iter2(sub_init_final.first.begin());
						iter2 != sub_init_final.first.end(); ++iter2)
					current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), *iter2);

		final.swap(sub_init_final.second);
	} else {
		while (ub > 0) {
			--ub;

			::std::pair< ta_state_set_t, ta_state_set_t > sub_init_final(
					copy_automaton(entry_sub.first->second, current_aut));
			
			for (ta_state_set_t::const_iterator iter(sub_init_final.first.begin());
					iter != sub_init_final.first.end(); ++iter)
				for (ta_state_set_t::const_iterator iter2(sub_init_final.second.begin());
						iter2 != sub_init_final.second.end(); ++iter2)
					current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), *iter2);
		
			if (initial.empty())
				initial.swap(sub_init_final.first);
			else
				for (ta_state_set_t::const_iterator iter(final.begin());
						iter != final.end(); ++iter)
					for (ta_state_set_t::const_iterator iter2(sub_init_final.first.begin());
							iter2 != sub_init_final.first.end(); ++iter2)
						current_aut.set_trans(*iter, m_target_graph_index.epsilon_index(), *iter2);

			final.swap(sub_init_final.second);
		}
	}
		
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			!initial.empty() && !final.empty());
	current_aut.initial().swap(initial);
	for (ta_state_set_t::const_iterator iter(final.begin());
			iter != final.end(); ++iter)
		current_aut.final(*iter) = 1;

	m_entry = entry;
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

