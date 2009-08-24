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
#include <diagnostics/basic_exceptions/invalid_argument.hpp>
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_next.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#include <limits>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
		
Evaluate_Path_Monitor::Filter_Index::Filter_Index() :
	m_next_index(0) {
	to_index(Filter_Function::Factory::get_instance().create<F_IDENTITY>());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_next_index);
}

int Evaluate_Path_Monitor::Filter_Index::to_index(Filter const* f) {
	::std::map< Filter const*, int >::iterator entry(m_filter_to_int.find(f));
	if (m_filter_to_int.end() == entry) {
		FSHELL2_PROD_CHECK(::diagnostics::Violated_Invariance,
				m_next_index < ::std::numeric_limits<int>::max());
		entry = m_filter_to_int.insert(::std::make_pair(f, m_next_index++)).first;
		m_int_to_filter.insert(::std::make_pair(entry->second, f));
	}
	return entry->second;
}

Filter const* Evaluate_Path_Monitor::Filter_Index::lookup_index(int index) const {
	::std::map< int, Filter const* >::const_iterator entry(m_int_to_filter.find(index));
	FSHELL2_DEBUG_ASSERT1(::diagnostics::Invalid_Argument, m_int_to_filter.end() != entry,
			::diagnostics::internal::to_string("Lookup of index ", index, " failed"));
	return entry->second;
}

Evaluate_Path_Monitor::Evaluate_Path_Monitor() :
	m_current_aut(0) {
}

Evaluate_Path_Monitor::~Evaluate_Path_Monitor() {
}

Evaluate_Path_Monitor::trace_automaton_t const& Evaluate_Path_Monitor::get_cov_seq_aut() const {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, 0 != m_cov_seq_aut.state_count());
	return m_cov_seq_aut;
}

Evaluate_Path_Monitor::trace_automaton_t const& Evaluate_Path_Monitor::get_passing_aut() const {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, 0 != m_passing_aut.state_count());
	return m_passing_aut;
}
	
Evaluate_Path_Monitor::test_goal_states_t const& Evaluate_Path_Monitor::get_test_goal_states(
		Test_Goal_Sequence::seq_entry_t const& s) const {
	test_goal_map_t::const_iterator entry(m_test_goal_map.find(&s));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_test_goal_map.end());
	return entry->second;
}

void Evaluate_Path_Monitor::visit(Edgecov const* n) {
	if (n->get_predicates()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	}
	
	m_current_initial = m_current_aut->new_state();
	m_current_final = m_current_aut->new_state();
	m_current_aut->set_trans(m_current_initial, m_filter_index.to_index(n->get_filter()),
			m_current_final);
}

void Evaluate_Path_Monitor::visit(PM_Alternative const* n) {
	n->get_path_monitor_a()->accept(this);
	trace_automaton_t::state_type const a_in(m_current_initial);
	trace_automaton_t::state_type const a_out(m_current_final);
	n->get_path_monitor_b()->accept(this);
	trace_automaton_t::state_type const b_in(m_current_initial);
	trace_automaton_t::state_type const b_out(m_current_final);

	m_current_initial = m_current_aut->new_state();
	m_current_final = m_current_aut->new_state();
	m_current_aut->set_trans(m_current_initial, -1, a_in);
	m_current_aut->set_trans(m_current_initial, -1, b_in);
	m_current_aut->set_trans(a_out, -1, m_current_final);
	m_current_aut->set_trans(b_out, -1, m_current_final);
}

void Evaluate_Path_Monitor::visit(PM_Concat const* n) {
	n->get_path_monitor_a()->accept(this);
	trace_automaton_t::state_type const a_in(m_current_initial);
	trace_automaton_t::state_type const a_out(m_current_final);
	n->get_path_monitor_b()->accept(this);

	m_current_aut->set_trans(a_out, -1, m_current_final);
	m_current_initial = a_in;
}

void Evaluate_Path_Monitor::visit(PM_Filter_Adapter const* n) {
	m_current_initial = m_current_aut->new_state();
	m_current_final = m_current_aut->new_state();
	m_current_aut->set_trans(m_current_initial, m_filter_index.to_index(n->get_filter()),
			m_current_final);
}

void Evaluate_Path_Monitor::visit(PM_Next const* n) {
	n->get_path_monitor_a()->accept(this);
	trace_automaton_t::state_type const a_in(m_current_initial);
	trace_automaton_t::state_type const a_out(m_current_final);
	n->get_path_monitor_b()->accept(this);

	m_current_aut->set_trans(a_out, m_filter_index.id_index(), m_current_initial);
	m_current_aut->set_trans(a_out, -1, m_current_initial);
	m_current_aut->set_trans(m_current_initial, -1, a_out);
	m_current_initial = a_in;
}

void Evaluate_Path_Monitor::visit(PM_Repeat const* n) {
	n->get_path_monitor()->accept(this);
	int lb(n->get_lower_bound());
	int ub(n->get_upper_bound());

	if (0 == lb && -1 == ub) {
		m_current_aut->set_trans(m_current_initial, -1, m_current_final);
		m_current_aut->set_trans(m_current_final, -1, m_current_initial);
	} else {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
		for (int lb(n->get_lower_bound()); lb > 1; --lb) {
			if (ub != -1) --ub;
			// clone, etc.
		}
	}
}

void Evaluate_Path_Monitor::visit(Pathcov const* n) {
	if (n->get_predicates()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	}
	
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Path_Monitor::visit(Predicate const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Path_Monitor::visit(Query const* n) {
	m_current_aut = &m_cov_seq_aut;
	n->get_cover()->accept(this);
	
	m_current_aut = &m_passing_aut;
	if (n->get_passing()) {
		n->get_passing()->accept(this);
	} else {
		m_current_initial = m_current_aut->new_state();
		m_current_final = m_current_initial;
		m_current_aut->set_trans(m_current_initial, m_filter_index.id_index(),
				m_current_final);
	}
	m_current_aut->initial().insert(m_current_initial);
	m_current_aut->final(m_current_final) = 1;

	simplify(m_cov_seq_aut, true);
	simplify(m_passing_aut, false);
}

void Evaluate_Path_Monitor::visit(Statecov const* n) {
	if (n->get_predicates()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	}
	
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Path_Monitor::visit(TGS_Intersection const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Path_Monitor::visit(TGS_Setminus const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Path_Monitor::visit(TGS_Union const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Path_Monitor::visit(Test_Goal_Sequence const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !n->get_sequence().empty());
	if (n->get_sequence().front().first) {
		n->get_sequence().front().first->accept(this);
	} else {
		m_current_initial = m_current_aut->new_state();
		m_current_final = m_current_initial;
		m_current_aut->set_trans(m_current_initial, m_filter_index.id_index(),
				m_current_final);
	}

	trace_automaton_t::state_type initial(m_current_initial);
	trace_automaton_t::state_type final(m_current_final);

	n->get_sequence().front().second->accept(this);

	m_current_aut->set_trans(final, -1, m_current_initial);
	final = m_current_final;
	test_goal_map_t::iterator tg_entry(m_test_goal_map.insert(::std::make_pair(
					&(n->get_sequence().front()), test_goal_states_t())).first);
	tg_entry->second.insert(m_current_final);
	m_reverse_test_goal_map.insert(::std::make_pair(m_current_final, tg_entry));

	for (Test_Goal_Sequence::seq_t::const_iterator iter(++(n->get_sequence().begin()));
			iter != n->get_sequence().end(); ++iter) {
		if (iter->first) {
			iter->first->accept(this);
		} else {
			m_current_initial = m_current_aut->new_state();
			m_current_final = m_current_aut->new_state();
			m_current_aut->set_trans(m_current_initial, m_filter_index.id_index(),
					m_current_final);
			m_current_aut->set_trans(m_current_initial, -1, m_current_final);
			m_current_aut->set_trans(m_current_final, -1, m_current_initial);
		}

		m_current_aut->set_trans(final, -1, m_current_initial);
		final = m_current_final;

		iter->second->accept(this);

		m_current_aut->set_trans(final, -1, m_current_initial);
		final = m_current_final;
		test_goal_map_t::iterator tg_entry(m_test_goal_map.insert(::std::make_pair(
						&(*iter), test_goal_states_t())).first);
		tg_entry->second.insert(m_current_final);
		m_reverse_test_goal_map.insert(::std::make_pair(m_current_final, tg_entry));
	}
		
	if (n->get_suffix_monitor()) {
		n->get_suffix_monitor()->accept(this);
	} else {
		m_current_initial = m_current_aut->new_state();
		m_current_final = m_current_initial;
		m_current_aut->set_trans(m_current_initial, m_filter_index.id_index(),
				m_current_final);
	}
	m_current_aut->set_trans(final, -1, m_current_initial);
	final = m_current_final;
	
	m_current_aut->initial().insert(initial);
	m_current_aut->final(final) = 1;
}

void Evaluate_Path_Monitor::simplify(trace_automaton_t & aut, bool update_tg) {
	for (trace_automaton_t::const_iterator iter(aut.begin()); iter != aut.end(); ++iter) {
		trace_automaton_t::edges_type in_edges(aut.delta2_backwards(*iter));
		trace_automaton_t::edges_type out_edges(aut.delta2(*iter));
		// remove all incoming edges labeled with -1 and clone final state
		// markers, if necessary
		for (trace_automaton_t::edges_type::const_iterator i_iter(in_edges.begin());
				i_iter != in_edges.end();) {
			if (-1 == i_iter->first) {
				// self-loops on -1 can be discarded, others must be
				// re-routed
				if (i_iter->second != *iter)
					for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
							o_iter != out_edges.end(); ++o_iter)
						aut.set_trans(i_iter->second, o_iter->first, o_iter->second);
				// copy acceptance marker
				if (aut.final(*iter)) aut.final(i_iter->second) = 1;
				if (update_tg) {
					test_goal_reverse_map_t::const_iterator r_map_entry(m_reverse_test_goal_map.find(
								*iter));
					if (m_reverse_test_goal_map.end() != r_map_entry) {
						r_map_entry->second->second.insert(i_iter->second);
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
								m_reverse_test_goal_map.end() == m_reverse_test_goal_map.find(i_iter->second));
						m_reverse_test_goal_map.insert(::std::make_pair(i_iter->second,
									r_map_entry->second));
					}
				}
				// edge done, remove it
				trace_automaton_t::edges_type::const_iterator i_iter_bak(i_iter);
				++i_iter;
				aut.del_trans(i_iter_bak->second, i_iter_bak->first, *iter);
			} else {
				++i_iter;
			}
		}
	}

	// remove dead states
	::std::list< trace_automaton_t::state_type > queue;
	::std::copy(aut.begin(), aut.end(), ::std::back_inserter(queue));
	::std::set< trace_automaton_t::state_type > dead_states;

	while (!queue.empty()) {
		trace_automaton_t::state_type const state(queue.front());
		queue.pop_front();
		if (dead_states.end() != dead_states.find(state)) continue;
		trace_automaton_t::edges_type in_edges(aut.delta2_backwards(state));
		trace_automaton_t::edges_type out_edges(aut.delta2(state));
		if (aut.initial().end() != aut.initial().find(state)) continue;
		bool del_state(false);
		if (in_edges.empty()) {
			for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
					o_iter != out_edges.end();) {
				queue.push_back(o_iter->second);
				trace_automaton_t::edges_type::const_iterator o_iter_bak(o_iter);
				++o_iter;
				aut.del_trans(state, o_iter_bak->first, o_iter_bak->second);
			}
			del_state = true;
		} else if (out_edges.empty()) {
			if (aut.final(state)) continue;
			for (trace_automaton_t::edges_type::const_iterator i_iter(in_edges.begin());
					i_iter != in_edges.end();) {
				queue.push_back(i_iter->second);
				trace_automaton_t::edges_type::const_iterator i_iter_bak(i_iter);
				++i_iter;
				aut.del_trans(i_iter_bak->second, i_iter_bak->first, state);
			}
			del_state = true;
		}

		if (del_state) {
			dead_states.insert(state);
			aut.del_state(state);
			if (update_tg) {
				test_goal_reverse_map_t::iterator r_map_entry(m_reverse_test_goal_map.find(state));
				if (m_reverse_test_goal_map.end() != r_map_entry) {
					r_map_entry->second->second.erase(state);
					m_reverse_test_goal_map.erase(r_map_entry);
				}
			}
		}
	}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

