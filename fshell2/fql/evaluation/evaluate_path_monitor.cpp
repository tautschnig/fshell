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

/*! \file fshell2/fql/evaluation/evaluate_path_monitor.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:29:47 CEST 2009 
*/

#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_next.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Evaluate_Path_Monitor::Evaluate_Path_Monitor() :
	m_current_aut(0) {
}

Evaluate_Path_Monitor::~Evaluate_Path_Monitor() {
}

Evaluate_Path_Monitor::automaton_t const& Evaluate_Path_Monitor::get(Path_Monitor const& pm) const {
	pm_aut_t::const_iterator entry(m_pm_aut_map.find(&pm));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			entry != m_pm_aut_map.end());
	return entry->second;
}

void Evaluate_Path_Monitor::visit(PM_Alternative const* n) {
	n->get_path_monitor_a()->accept(this);
	automaton_t::state_type const a_in(m_top_states.first);
	automaton_t::state_type const a_out(m_top_states.second);
	n->get_path_monitor_b()->accept(this);
	automaton_t::state_type const b_in(m_top_states.first);
	automaton_t::state_type const b_out(m_top_states.second);

	m_top_states.first = m_current_aut->new_state();
	m_top_states.second = m_current_aut->new_state();
	m_current_aut->set_trans(m_top_states.first, 0, a_in);
	m_current_aut->set_trans(m_top_states.first, 0, b_in);
	m_current_aut->set_trans(a_out, 0, m_top_states.second);
	m_current_aut->set_trans(b_out, 0, m_top_states.second);
}

void Evaluate_Path_Monitor::visit(PM_Concat const* n) {
	n->get_path_monitor_a()->accept(this);
	automaton_t::state_type const a_in(m_top_states.first);
	automaton_t::state_type const a_out(m_top_states.second);
	n->get_path_monitor_b()->accept(this);

	m_current_aut->set_trans(a_out, 0, m_top_states.second);
	m_top_states.first = a_in;
}

void Evaluate_Path_Monitor::visit(PM_Filter_Adapter const* n) {
	m_top_states.first = m_current_aut->new_state();
	m_top_states.second = m_current_aut->new_state();
	m_current_aut->set_trans(m_top_states.first, n->get_filter(),
			m_top_states.second);
}

void Evaluate_Path_Monitor::visit(PM_Next const* n) {
	n->get_path_monitor_a()->accept(this);
	automaton_t::state_type const a_in(m_top_states.first);
	automaton_t::state_type const a_out(m_top_states.second);
	n->get_path_monitor_b()->accept(this);

	m_current_aut->set_trans(a_out, (Filter const*) 1, m_top_states.second);
	m_top_states.first = a_in;
}

void Evaluate_Path_Monitor::visit(PM_Repeat const* n) {
	n->get_path_monitor()->accept(this);
	int lb(n->get_lower_bound());
	int ub(n->get_upper_bound());

	if (0 == lb && -1 == ub) {
		m_current_aut->set_trans(m_top_states.first, 0, m_top_states.second);
		m_current_aut->set_trans(m_top_states.second, 0, m_top_states.first);
	} else {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
		for (int lb(n->get_lower_bound()); lb > 1; --lb) {
			if (ub != -1) --ub;
			// clone, etc.
		}
	}
}

void Evaluate_Path_Monitor::visit(Query const* n) {
	n->get_cover()->accept(this);
	
	if (n->get_passing()) {
		::std::pair< pm_aut_t::iterator, bool > entry(m_pm_aut_map.insert(
					::std::make_pair(n->get_passing(), automaton_t())));
		if (!entry.second) return;
		m_current_aut = &(entry.first->second);
		
		n->get_passing()->accept(this);
		
		m_current_aut->initial().insert(m_top_states.first);
		m_current_aut->final(m_top_states.second) = 1;
	}
}

void Evaluate_Path_Monitor::visit(Test_Goal_Sequence const* n) {
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		if (iter->first) {
			::std::pair< pm_aut_t::iterator, bool > entry(m_pm_aut_map.insert(
						::std::make_pair(iter->first, automaton_t())));
			if (!entry.second) return;
			m_current_aut = &(entry.first->second);
			
			iter->first->accept(this);
			
			m_current_aut->initial().insert(m_top_states.first);
			m_current_aut->final(m_top_states.second) = 1;
		}
	}

	if (n->get_suffix_monitor()) {
		::std::pair< pm_aut_t::iterator, bool > entry(m_pm_aut_map.insert(
					::std::make_pair(n->get_suffix_monitor(), automaton_t())));
		if (!entry.second) return;
		m_current_aut = &(entry.first->second);
		
		n->get_suffix_monitor()->accept(this);
		
		m_current_aut->initial().insert(m_top_states.first);
		m_current_aut->final(m_top_states.second) = 1;
	}
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

