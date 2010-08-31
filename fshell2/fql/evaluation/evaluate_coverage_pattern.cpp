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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#  include <diagnostics/basic_exceptions/not_implemented.hpp>
#endif

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
	
Evaluate_Coverage_Pattern::Evaluate_Coverage_Pattern(::language_uit & manager) :
	m_pp_eval(manager), 
	m_test_goal_states(0),
	m_current_tg_states(&m_test_goal_states)
{
}
	
Evaluate_Coverage_Pattern::~Evaluate_Coverage_Pattern() {
}

bool Evaluate_Coverage_Pattern::is_test_goal_state_rec(Test_Goal_States const& tgs, ta_state_t const& state) {
	if (tgs.m_tg_states.end() != tgs.m_tg_states.find(state)) return true;
	if (tgs.m_children.empty()) return false;
	
	for (::std::list< Test_Goal_States >::const_iterator iter(tgs.m_children.begin());
			iter != tgs.m_children.end(); ++iter)
		if (is_test_goal_state_rec(*iter, state)) return true;
	
	return false;
}
	
#if FSHELL2_DEBUG__LEVEL__ >= 1
bool Evaluate_Coverage_Pattern::is_test_goal_state(ta_state_t const& state) const {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		is_test_goal_state_rec(m_test_goal_states, state) ==
		(m_all_test_goal_states.end() != m_all_test_goal_states.find(state)));
	return (m_all_test_goal_states.end() != m_all_test_goal_states.find(state));
}

trace_automaton_t const& Evaluate_Coverage_Pattern::get() const {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			m_test_goal_automaton.state_count() > 0);
	return m_test_goal_automaton;
}
#endif

Evaluate_Coverage_Pattern::Test_Goal_States const& Evaluate_Coverage_Pattern::do_query(
		::goto_functionst & gf, ::fshell2::instrumentation::CFG & cfg, Query const& query) {
	m_test_goal_states.m_cp = 0;
	m_test_goal_states.m_tg_states.clear();
	m_test_goal_states.m_children.clear();
	m_current_tg_states = &m_test_goal_states;
	for (trace_automaton_t::const_iterator iter(m_test_goal_automaton.begin());
			iter != m_test_goal_automaton.end(); ++iter)
		m_test_goal_automaton.del_state(*iter);
	m_current_final.clear();
	m_all_test_goal_states.clear();
	
	// build automata from path patterns
	m_pp_eval.do_query(gf, cfg, query);

	query.accept(this);
	
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states.m_cp);
	return m_test_goal_states;
}

void Evaluate_Coverage_Pattern::copy_from_path_pattern(Path_Pattern_Expr const* n) {
	ta_state_set_t prev_final;
	prev_final.swap(m_current_final);
	
	trace_automaton_t const& ta(m_pp_eval.get(n));
	
	// handle case of single edge automaton (edgecov, predicate, nodecov) more
	// efficiently
	if (2 == ta.state_count() && 1 == ta.trans_count()) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == ta.initial().size());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, *ta.begin() == *ta.initial().begin());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == ta.delta2(*ta.begin()).size());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !ta.final(*ta.initial().begin()));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, ta.final(*(++ta.begin())));
		
		ta_state_t const new_state(m_test_goal_automaton.new_state());
		m_current_final.insert(new_state);
		m_test_goal_automaton.final(new_state) = 1;
		m_current_tg_states->m_tg_states.insert(new_state);
		m_all_test_goal_states.insert(new_state);
		int const idx(ta.delta2(*ta.begin()).begin()->first);

		for (ta_state_set_t::const_iterator iter(prev_final.begin());
				iter != prev_final.end(); ++iter) {
			m_test_goal_automaton.set_trans(*iter, idx, new_state);
			m_test_goal_automaton.final(*iter) = 0;
		}
	} else {
		::std::pair< ta_state_set_t, ta_state_set_t > ta_init_final(
				copy_automaton(ta, m_test_goal_automaton));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == ta_init_final.first.size());
		for (ta_state_set_t::const_iterator iter(prev_final.begin());
				iter != prev_final.end(); ++iter) {
			m_test_goal_automaton.set_trans(*iter, m_pp_eval.epsilon_index(), *ta_init_final.first.begin());
			m_test_goal_automaton.final(*iter) = 0;
		}
		
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !dynamic_cast<ECP_Atom const*>(n) ||
				(ta_init_final.second.end() == ta_init_final.second.find(*ta_init_final.first.begin())));
		for (ta_state_set_t::const_iterator iter(ta_init_final.second.begin());
				iter != ta_init_final.second.end(); ++iter)
			m_test_goal_automaton.final(*iter) = 1;
		simplify_automaton(m_test_goal_automaton, false);
		for (trace_automaton_t::const_iterator iter(m_test_goal_automaton.begin());
				iter != m_test_goal_automaton.end(); ++iter)
			if (m_test_goal_automaton.final(*iter)) m_current_final.insert(*iter);
		m_current_tg_states->m_tg_states = m_current_final;
		m_all_test_goal_states.insert(m_current_final.begin(), m_current_final.end());
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
	m_all_test_goal_states.insert(m_current_final.begin(), m_current_final.end());
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
	m_all_test_goal_states.insert(m_current_final.begin(), m_current_final.end());
}

void Evaluate_Coverage_Pattern::visit(Depcov const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}
			
void Evaluate_Coverage_Pattern::visit(Edgecov const* n) {
	copy_from_path_pattern(n);
}

void Evaluate_Coverage_Pattern::visit(Nodecov const* n) {
	copy_from_path_pattern(n);
}
			
void Evaluate_Coverage_Pattern::visit(Pathcov const* n) {
	copy_from_path_pattern(n);
}

void Evaluate_Coverage_Pattern::visit(Predicate const* n) {
	copy_from_path_pattern(n);
}

void Evaluate_Coverage_Pattern::visit(Query const* n) {
	m_current_final.insert(m_test_goal_automaton.new_state());
	m_test_goal_automaton.initial().insert(*m_current_final.begin());
	m_test_goal_automaton.final(*m_current_final.begin()) = 1;
	
	m_current_tg_states->m_cp = n->get_cover();
	n->get_cover()->accept(this);
}

void Evaluate_Coverage_Pattern::visit(Quote const* n) {
	copy_from_path_pattern(n->get_pp());
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

