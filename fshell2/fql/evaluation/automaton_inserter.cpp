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

/*! \file fshell2/fql/evaluation/automaton_inserter.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:32:10 CEST 2009 
*/

#include <fshell2/fql/evaluation/automaton_inserter.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <fshell2/fql/ast/query.hpp>

#include <cbmc/src/util/std_code.h>
#include <cbmc/src/util/std_expr.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Automaton_Inserter::Automaton_Inserter(Evaluate_Path_Monitor const& pm_eval,
			Evaluate_Filter const& filter_eval, ::goto_functionst & gf) :
	m_pm_eval(pm_eval), m_filter_eval(filter_eval), m_gf(gf),
	m_inserter(m_gf), m_aut_index(-1) {
}

Automaton_Inserter::~Automaton_Inserter() {
}
	
Automaton_Inserter::value_t const& Automaton_Inserter::get(Test_Goal_Sequence::seq_entry_t const& seq) const {
	pm_value_t::const_iterator entry(m_pm_acc_edges_map.find(&seq));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			entry != m_pm_acc_edges_map.end());
	return entry->second;
}

void Automaton_Inserter::insert(Path_Monitor const& pm) {
	m_acc_edges.clear();
	++m_aut_index;
	::std::string const symbol_name(::diagnostics::internal::to_string("aut_state$", m_aut_index));
	Evaluate_Path_Monitor::automaton_t const& aut(m_pm_eval.get(pm));

	::std::list< Evaluate_Path_Monitor::automaton_t::state_type > processing_stack;
	for (::std::set< Evaluate_Path_Monitor::automaton_t::state_type >::const_iterator iter(
				aut.initial().begin()); iter != aut.initial().end(); ++iter)
		processing_stack.push_back(*iter);
				
	for (::std::list< Evaluate_Path_Monitor::automaton_t::state_type >::const_iterator iter(
				processing_stack.begin()); iter != processing_stack.end(); ++iter) {
		// TODO use smaller type, if possible
		::exprt state("symbol", ::unsignedbv_typet(32));
		// TODO declaration missing, must be global and of proper size
		// instrumentation should have function to add global decl
		state.set("#base_name", symbol_name);
		state.set("#identifier", symbol_name);
			
		::exprt cur_state("constant", state.type());
		cur_state.set("value", *iter);

		::binary_relation_exprt cond(state, "=", cur_state);

		Evaluate_Path_Monitor::automaton_t::edges_type out_edges(aut.delta2(*iter));
		processing_stack.push_back(out_edges.begin()->second);
			
		if (2 == out_edges.size() && 0 == out_edges.begin()->first && 0 == (++(out_edges.begin()))->first) {
			Evaluate_Path_Monitor::automaton_t::state_type const& other((++(out_edges.begin()))->second);
			processing_stack.push_back(other);
			
			// non-det
			FSHELL2_AUDIT_ASSERT(Violated_Invariance, 0 == aut.final(out_edges.begin()->second));
			FSHELL2_AUDIT_ASSERT(Violated_Invariance, 0 == aut.final(other));
			::exprt var("symbol", ::typet("bool"));
			::std::string const var_name(::diagnostics::internal::to_string(symbol_name, "$", *iter));
			var.set("#base_name", var_name);
			var.set("#identifier", var_name);
			::exprt rhs(::exprt("if", state.type()));
			rhs.operands().resize(3);
			rhs.op0() = ::and_exprt(var, cond);
			rhs.op1() = ::constant_exprt(state.type());
			rhs.op1().set("value", out_edges.begin()->second);
			rhs.op2() = ::constant_exprt(state.type());
			rhs.op2().set("value", other);
			
			::goto_programt tmp;
			::goto_programt::targett next_state(tmp.add_instruction(ASSIGN));
			next_state->code = ::code_assignt(state, rhs);

			if (m_last_insert.empty()) {
				m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::FUNCTION_CALL, tmp);
			} else {
				Evaluate_Filter::value_t to_insert;
				to_insert.swap(m_last_insert);
				for (Evaluate_Filter::value_t::iterator l_iter(to_insert.begin());
						l_iter != to_insert.end(); ++l_iter) {
					::fshell2::instrumentation::GOTO_Transformation::value_t const& recent(
							m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER, *l_iter, tmp));
					m_last_insert.insert(recent.begin(), recent.end());
				}
			}
		} else if (1 == out_edges.size() && 0 == out_edges.begin()->first) {
			// TODO not yet correct, does not enforce consecutive edges
			::goto_programt tmp_target;
			::goto_programt::targett goto_target(tmp_target.add_instruction(SKIP));

			::goto_programt tmp;
			::goto_programt::targett goto_stmt(tmp.add_instruction());
			goto_stmt->make_goto(goto_target);
			goto_stmt->guard = cond;
			goto_stmt->guard.make_not();

			::goto_programt::targett next_state(tmp.add_instruction(ASSIGN));
			::exprt next_state_num("constant", state.type());
			next_state_num.set("value", out_edges.begin()->second);
			next_state->code = ::code_assignt(state, next_state_num);

			tmp.destructive_append(tmp_target);

			if (m_last_insert.empty()) {
				m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::FUNCTION_CALL, tmp);
			} else {
				Evaluate_Filter::value_t to_insert;
				to_insert.swap(m_last_insert);
				for (Evaluate_Filter::value_t::iterator l_iter(to_insert.begin());
						l_iter != to_insert.end(); ++l_iter) {
					::fshell2::instrumentation::GOTO_Transformation::value_t const& recent(
							m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER, *l_iter, tmp));
					m_last_insert.insert(recent.begin(), recent.end());
				}
			}
			
			if (aut.final(out_edges.begin()->second)) {
				for (Evaluate_Filter::value_t::iterator l_iter(m_last_insert.begin());
						l_iter != m_last_insert.end(); ++l_iter) {
					::goto_programt::targett pred(l_iter->first);
					pred--;
					m_acc_edges.insert(::std::make_pair(pred, l_iter->first));
				}
			}
		} else if (1 == out_edges.size() && ((Filter const*) 1) == out_edges.begin()->first) {
			// .id*.
			::goto_programt tmp_target;
			::goto_programt::targett goto_target(tmp_target.add_instruction(SKIP));

			::goto_programt tmp;
			::goto_programt::targett goto_stmt(tmp.add_instruction());
			goto_stmt->make_goto(goto_target);
			goto_stmt->guard = cond;
			goto_stmt->guard.make_not();

			::goto_programt::targett next_state(tmp.add_instruction(ASSIGN));
			::exprt next_state_num("constant", state.type());
			next_state_num.set("value", out_edges.begin()->second);
			next_state->code = ::code_assignt(state, next_state_num);

			tmp.destructive_append(tmp_target);

			if (m_last_insert.empty()) {
				m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::FUNCTION_CALL, tmp);
			} else {
				Evaluate_Filter::value_t to_insert;
				to_insert.swap(m_last_insert);
				for (Evaluate_Filter::value_t::iterator l_iter(to_insert.begin());
						l_iter != to_insert.end(); ++l_iter) {
					::fshell2::instrumentation::GOTO_Transformation::value_t const& recent(
							m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER, *l_iter, tmp));
					m_last_insert.insert(recent.begin(), recent.end());
				}
			}
			
			if (aut.final(out_edges.begin()->second)) {
				for (Evaluate_Filter::value_t::iterator l_iter(m_last_insert.begin());
						l_iter != m_last_insert.end(); ++l_iter) {
					::goto_programt::targett pred(l_iter->first);
					pred--;
					m_acc_edges.insert(::std::make_pair(pred, l_iter->first));
				}
			}
		} else if (1 == out_edges.size()) {
			// some filter
			::goto_programt tmp_target;
			::goto_programt::targett goto_target(tmp_target.add_instruction(SKIP));

			::goto_programt tmp;
			::goto_programt::targett goto_stmt(tmp.add_instruction());
			goto_stmt->make_goto(goto_target);
			goto_stmt->guard = cond;
			goto_stmt->guard.make_not();

			::goto_programt::targett next_state(tmp.add_instruction(ASSIGN));
			::exprt next_state_num("constant", state.type());
			next_state_num.set("value", out_edges.begin()->second);
			next_state->code = ::code_assignt(state, next_state_num);

			tmp.destructive_append(tmp_target);

			m_last_insert.clear();
			Evaluate_Filter::value_t const& filter_val(m_filter_eval.get(*(out_edges.begin()->first)));
			for (Evaluate_Filter::value_t::const_iterator f_iter(filter_val.begin());
					f_iter != filter_val.end(); ++f_iter) {
				::fshell2::instrumentation::GOTO_Transformation::value_t const& recent(
						m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER, *f_iter, tmp));
				m_last_insert.insert(recent.begin(), recent.end());
			}
			
			if (aut.final(out_edges.begin()->second)) {
				for (Evaluate_Filter::value_t::iterator l_iter(m_last_insert.begin());
						l_iter != m_last_insert.end(); ++l_iter) {
					::goto_programt::targett pred(l_iter->first);
					pred--;
					m_acc_edges.insert(::std::make_pair(pred, l_iter->first));
				}
			}
		} else {
			FSHELL2_AUDIT_ASSERT(Violated_Invariance, out_edges.empty());
		}
	}
}

void Automaton_Inserter::insert(Query const& query) {
	for (Test_Goal_Sequence::seq_t::const_iterator iter(query.get_cover()->get_sequence().begin());
			iter != query.get_cover()->get_sequence().end(); ++iter) {
		if (iter->first) {
			insert(*(iter->first));
			m_pm_acc_edges_map.insert(::std::make_pair(&(*iter), value_t())).first->second.swap(m_acc_edges);
		}
	}
	
	::exprt * assertion_expr(0);
	if (query.get_cover()->get_suffix_monitor()) {
		insert(*(query.get_cover()->get_suffix_monitor()));

		FSHELL2_AUDIT_ASSERT(Violated_Invariance, !m_acc_edges.empty());
		assertion_expr = new ::binary_relation_exprt(
				::to_code_assign(m_acc_edges.begin()->first->code).lhs(),
				"=",
				::to_code_assign(m_acc_edges.begin()->first->code).rhs());
	}
	
	if (query.get_passing()) {
		insert(*(query.get_passing()));

		FSHELL2_AUDIT_ASSERT(Violated_Invariance, !m_acc_edges.empty());
		::exprt * passing(new ::binary_relation_exprt(
				::to_code_assign(m_acc_edges.begin()->first->code).lhs(),
				"=",
				::to_code_assign(m_acc_edges.begin()->first->code).rhs()));
		
		if (assertion_expr) {
			::exprt * and_expr(new ::exprt("code", ::typet("code")));
			and_expr->id("and");
			and_expr->reserve_operands(2);
			and_expr->move_to_operands(*assertion_expr);
			and_expr->move_to_operands(*passing);
			assertion_expr = and_expr;
		} else {
			assertion_expr = passing;
		}
	} 
	
	::goto_programt tmp;
	::goto_programt::targett as(tmp.add_instruction(ASSERT));
	as->code = ::code_assertt();
	
	if (assertion_expr) {
		as->guard = *assertion_expr;
	} else {
		// no restrictions, only insert a final assert(0)
		::exprt zero(::exprt("constant", ::typet("bool")));
		zero.make_false();
		as->guard = zero;
	}
	
	m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp);

	m_inserter.update_all();
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

