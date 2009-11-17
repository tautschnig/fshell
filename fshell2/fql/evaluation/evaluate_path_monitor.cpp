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
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_postcondition.hpp>
#include <fshell2/fql/ast/pm_precondition.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
		
Evaluate_Path_Monitor::Evaluate_Path_Monitor(Evaluate_Filter const& filter_eval,
		Predicate_Instrumentation const& pred_instr) :
	m_eval_filter(filter_eval),
	m_pred_instr(pred_instr),
	m_target_graph_index(&(m_eval_filter.get(*(Filter_Function::Factory::get_instance().create<F_IDENTITY>())))) {
	// we always have the TRUE automaton in there
	m_entry = m_pm_map.insert(::std::make_pair(static_cast<Path_Monitor_Expr const*>(0), trace_automaton_t()));
	trace_automaton_t & current_aut(m_entry.first->second);
	ta_state_t const s(current_aut.new_state());
	current_aut.initial().insert(s);
	current_aut.final(s) = 1;
	current_aut.set_trans(s, m_target_graph_index.id_index(), s);
}

Evaluate_Path_Monitor::~Evaluate_Path_Monitor() {
}

trace_automaton_t const& Evaluate_Path_Monitor::get(Path_Monitor_Expr const* pm) const {
	pm_value_t::const_iterator entry(m_pm_map.find(pm));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_pm_map.end());
	return entry->second;
}
	
void Evaluate_Path_Monitor::visit(PM_Alternative const* n) {
	m_entry = m_pm_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pm_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_path_monitor_expr_a()->accept(this);
	::std::pair< pm_value_t::iterator, bool > entry_a(m_entry);
	n->get_path_monitor_expr_b()->accept(this);
	::std::pair< pm_value_t::iterator, bool > entry_b(m_entry);

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

void Evaluate_Path_Monitor::visit(PM_Concat const* n) {
	m_entry = m_pm_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pm_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_path_monitor_expr_a()->accept(this);
	::std::pair< pm_value_t::iterator, bool > entry_a(m_entry);
	n->get_path_monitor_expr_b()->accept(this);
	::std::pair< pm_value_t::iterator, bool > entry_b(m_entry);

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

void Evaluate_Path_Monitor::visit(PM_Filter_Adapter const* n) {
	m_entry = m_pm_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	trace_automaton_t & current_aut(m_entry.first->second);
	
	ta_state_t const current_initial(current_aut.new_state());
	current_aut.initial().insert(current_initial);
	ta_state_t const current_final(current_aut.new_state());
	current_aut.final(current_final) = 1;

	current_aut.set_trans(current_initial,
			m_target_graph_index.to_index(&(m_eval_filter.get(*(n->get_filter_expr())))),
			current_final);
}

void Evaluate_Path_Monitor::visit(PM_Postcondition const* n) {
	m_entry = m_pm_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pm_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_path_monitor_expr()->accept(this);
	::std::pair< pm_value_t::iterator, bool > entry_sub(m_entry);

	::std::pair< ta_state_set_t, ta_state_set_t > sub_init_final(
				copy_automaton(entry_sub.first->second, current_aut));
	current_aut.initial().swap(sub_init_final.first);
	ta_state_t const new_final(current_aut.new_state());
	current_aut.final(new_final) = 1;
	
	for (ta_state_set_t::const_iterator iter(sub_init_final.second.begin());
			iter != sub_init_final.second.end(); ++iter) {
		target_graph_t::edges_t pred_edges;
		trace_automaton_t::edges_type in_edges(current_aut.delta2_backwards(*iter));
		for (trace_automaton_t::edges_type::const_iterator i_iter(in_edges.begin());
				i_iter != in_edges.end(); ++i_iter) {
			target_graph_t const& tgg(*(m_target_graph_index.lookup_index(i_iter->first)));
			for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
					e_iter != tgg.get_edges().end(); ++e_iter)
				pred_edges.insert(m_pred_instr.get(e_iter->second, n->get_predicate()));
		}
		m_more_target_graphs.push_back(target_graph_t());
		m_more_target_graphs.back().set_edges(pred_edges);
		current_aut.set_trans(*iter,
				m_target_graph_index.to_index(&(m_more_target_graphs.back())), new_final);
	}

	m_entry = entry;
}

void Evaluate_Path_Monitor::visit(PM_Precondition const* n) {
	m_entry = m_pm_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pm_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_path_monitor_expr()->accept(this);
	::std::pair< pm_value_t::iterator, bool > entry_sub(m_entry);

	::std::pair< ta_state_set_t, ta_state_set_t > sub_init_final(
				copy_automaton(entry_sub.first->second, current_aut));
	ta_state_t const new_initial(current_aut.new_state());
	current_aut.initial().insert(new_initial);
	for (ta_state_set_t::const_iterator iter(sub_init_final.second.begin());
			iter != sub_init_final.second.end(); ++iter)
		current_aut.final(*iter) = 1;
	
	for (ta_state_set_t::const_iterator iter(sub_init_final.first.begin());
			iter != sub_init_final.first.end(); ++iter) {
		target_graph_t::edges_t pred_edges;
		trace_automaton_t::edges_type out_edges(current_aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
				o_iter != out_edges.end(); ++o_iter) {
			target_graph_t const& tgg(*(m_target_graph_index.lookup_index(o_iter->first)));
			for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
					e_iter != tgg.get_edges().end(); ++e_iter)
				pred_edges.insert(m_pred_instr.get(e_iter->first, n->get_predicate()));
		}
		m_more_target_graphs.push_back(target_graph_t());
		m_more_target_graphs.back().set_edges(pred_edges);
		current_aut.set_trans(new_initial,
				m_target_graph_index.to_index(&(m_more_target_graphs.back())), *iter);
	}

	m_entry = entry;
}

void Evaluate_Path_Monitor::visit(PM_Repeat const* n) {
	m_entry = m_pm_map.insert(::std::make_pair(n, trace_automaton_t()));
	if (!m_entry.second) return;
	::std::pair< pm_value_t::iterator, bool > entry(m_entry);
	trace_automaton_t & current_aut(entry.first->second);
	
	n->get_path_monitor_expr()->accept(this);
	::std::pair< pm_value_t::iterator, bool > entry_sub(m_entry);
	
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

void Evaluate_Path_Monitor::visit(Query const* n) {
	n->get_cover()->accept(this);
	
	if (n->get_passing()) {
		n->get_passing()->accept(this);
		simplify_automaton(m_entry.first->second, true);
	}
}

void Evaluate_Path_Monitor::visit(Test_Goal_Sequence const* n) {
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		if (iter->first) {
			iter->first->accept(this);
			simplify_automaton(m_entry.first->second, true);
		}
	}
		
	if (n->get_suffix_monitor()) {
		n->get_suffix_monitor()->accept(this);
		simplify_automaton(m_entry.first->second, true);
	} 
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

