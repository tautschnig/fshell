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
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_pattern.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
	
Evaluate_Coverage_Pattern::Evaluate_Coverage_Pattern(Evaluate_Filter const& eval_filter,
			Evaluate_Path_Pattern & pp_eval) :
	m_eval_filter(eval_filter),
	m_pp_eval(pp_eval),
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
	m_test_goal_automaton.final(new_state) = 1;
	m_current_tg_states->m_tg_states.insert(new_state);
	int const idx(m_pp_eval.to_index(m_eval_filter.get(*(n->get_filter_expr()))));
			
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter) {
		m_test_goal_automaton.set_trans(*iter, idx, new_state);
		m_test_goal_automaton.final(*iter) = 0;
	}
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
	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
	
	trace_automaton_t const& ta(m_pp_eval.get(n));
	::std::pair< ta_state_set_t, ta_state_set_t > ta_init_final(
			copy_automaton(ta, m_test_goal_automaton));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == ta_init_final.first.size());
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter) {
		m_test_goal_automaton.set_trans(*iter, m_pp_eval.epsilon_index(), *ta_init_final.first.begin());
		m_test_goal_automaton.final(*iter) = 0;
	}
	
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			ta_init_final.second.end() == ta_init_final.second.find(*ta_init_final.first.begin()));
	for (ta_state_set_t::const_iterator iter(ta_init_final.second.begin());
			iter != ta_init_final.second.end(); ++iter)
		m_test_goal_automaton.final(*iter) = 1;
	simplify_automaton(m_test_goal_automaton, false);
	for (trace_automaton_t::const_iterator iter(m_test_goal_automaton.begin());
			iter != m_test_goal_automaton.end(); ++iter)
		if (m_test_goal_automaton.final(*iter)) m_current_final.insert(*iter);
	m_current_tg_states->m_tg_states = m_current_final;
}

void Evaluate_Coverage_Pattern::visit(Predicate const* n) {
	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
			
	ta_state_t const new_state(m_test_goal_automaton.new_state());
	m_current_final.insert(new_state);
	m_test_goal_automaton.final(new_state) = 1;
	m_current_tg_states->m_tg_states.insert(new_state);
	int const idx(m_pp_eval.to_index(m_eval_filter.get(*n)));
			
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter) {
		m_test_goal_automaton.set_trans(*iter, idx, new_state);
		m_test_goal_automaton.final(*iter) = 0;
	}
}

void Evaluate_Coverage_Pattern::visit(Query const* n) {
	m_current_final.insert(m_test_goal_automaton.new_state());
	m_test_goal_automaton.initial().insert(*m_current_final.begin());
	m_test_goal_automaton.final(*m_current_final.begin()) = 1;
	
	m_current_tg_states->m_cp = n->get_cover();
	n->get_cover()->accept(this);
}

void Evaluate_Coverage_Pattern::visit(Quote const* n) {
	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
	
	trace_automaton_t const& pp(m_pp_eval.get(n->get_pp()));
	::std::pair< ta_state_set_t, ta_state_set_t > pp_init_final(
			copy_automaton(pp, m_test_goal_automaton));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == pp_init_final.first.size());
	for (ta_state_set_t::const_iterator iter(prev_final.begin());
			iter != prev_final.end(); ++iter) {
		m_test_goal_automaton.set_trans(*iter, m_pp_eval.epsilon_index(), *pp_init_final.first.begin());
		m_test_goal_automaton.final(*iter) = 0;
	}
	
	for (ta_state_set_t::const_iterator iter(pp_init_final.second.begin());
			iter != pp_init_final.second.end(); ++iter)
		m_test_goal_automaton.final(*iter) = 1;
	simplify_automaton(m_test_goal_automaton, false);
	for (trace_automaton_t::const_iterator iter(m_test_goal_automaton.begin());
			iter != m_test_goal_automaton.end(); ++iter)
		if (m_test_goal_automaton.final(*iter)) m_current_final.insert(*iter);
	m_current_tg_states->m_tg_states = m_current_final;
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

