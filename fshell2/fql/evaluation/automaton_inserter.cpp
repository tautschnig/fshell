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

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>
#include <fshell2/fql/ast/query.hpp>

#include <cbmc/src/goto-programs/extended_cfg.h>
#include <cbmc/src/util/std_code.h>
#include <cbmc/src/util/std_expr.h>
#include <cbmc/src/util/arith_tools.h>
#include <cbmc/src/util/expr_util.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
			
Automaton_Inserter::Automaton_Inserter(Evaluate_Path_Monitor const& pm_eval,
			Evaluate_Filter const& filter_eval, ::goto_functionst & gf,
			::contextt & context) :
	m_pm_eval(pm_eval), m_filter_eval(filter_eval), m_gf(gf),
	m_context(context),
	m_inserter(*(new ::fshell2::instrumentation::GOTO_Transformation(m_gf))) {
}

Automaton_Inserter::~Automaton_Inserter() {
	delete &m_inserter;
}

/*
void Automaton_Inserter::insert(Path_Monitor const* pm) {
	using ::fshell2::instrumentation::GOTO_Transformation;

	m_acc_edges.clear();
	++m_aut_index;
	::std::string const symbol_name(::diagnostics::internal::to_string("aut_state$", m_aut_index));
	Evaluate_Path_Monitor::trace_automaton_t const& aut(m_pm_eval.get(pm));

	::std::list< To_Process > processing_stack;
	
	for (::std::set< state_t >::const_iterator iter(aut.initial().begin());
			iter != aut.initial().end(); ++iter)
		processing_stack.push_back(To_Process(*iter));
				
	// DFS traversal
	::std::set< state_t > seen;
	while (!processing_stack.empty()) {
		To_Process stack_elem;
		stack_elem.take_over(processing_stack.front());
		if (!seen.insert(stack_elem.first).second) continue;

		Evaluate_Path_Monitor::trace_automaton_t::edges_type out_edges(aut.delta2(
					stack_elem.first));

		// TODO use smaller type, if possible
		::exprt state("symbol", ::unsignedbv_typet(32));
		// TODO declaration missing, must be global and of proper size
		// instrumentation should have function to add global decl
		state.set("#base_name", symbol_name);
		state.set("#identifier", symbol_name);
			
		::exprt cur_state("constant", state.type());
		cur_state.set("value", *iter);

		::binary_relation_exprt cond(state, "=", cur_state);
			
		if (2 == out_edges.size() && 0 == out_edges.begin()->first && 0 == (++(out_edges.begin()))->first) {
			// non-det
			state_t const& succ1(out_edges.begin()->second);
			state_t const& succ2((++(out_edges.begin()))->second);
			// must not get into final states -- really, why not? TODO
			FSHELL2_AUDIT_ASSERT(Violated_Invariance, 0 == aut.final(succ1));
			FSHELL2_AUDIT_ASSERT(Violated_Invariance, 0 == aut.final(succ2));
			// code symbol_name = (symbol_name$state && cond)?succ1:succ2;
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

			append goto program to m_remaining_statements

			CFA::edges_t recent_insert;
			if (stack_elem.m_preceding_edges.empty() && !stack_elem.m_is_any) {
				GOTO_Transformation::inserted_t const& nodes(m_inserter.insert("main",
							GOTO_Transformation::BEFORE, ::FUNCTION_CALL, tmp));
				for (GOTO_Transformation::inserted_t::const_iterator iter(nodes.begin());
						iter != nodes.end(); ++iter)
					recent_insert.insert(::std::make_pair(*iter, ::std::make_pair(
									iter->first, ++(iter->second))));
			} else if (stack_elem.m_preceding_edges.empty()) {
			} else {
				for (CFA::edges_t::const_iterator l_iter(stack_elem.second.first.begin());
						l_iter != stack_elem.second.first.end(); ++l_iter) {
					GOTO_Transformation::inserted_t const& nodes(
							m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER,
								*l_iter, tmp));
					for (GOTO_Transformation::inserted_t::const_iterator iter(nodes.begin());
							iter != nodes.end(); ++iter)
						recent_insert.insert(::std::make_pair(*iter, ::std::make_pair(
										iter->first, ++(iter->second))));
				}
			}
			processing_stack.push_back(other);
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
				Evaluate_Filter::CFA::edges_t to_insert;
				to_insert.swap(m_last_insert);
				for (Evaluate_Filter::CFA::edges_t::iterator l_iter(to_insert.begin());
						l_iter != to_insert.end(); ++l_iter) {
					::fshell2::instrumentation::GOTO_Transformation::CFA::edges_t const& recent(
							m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER, *l_iter, tmp));
					m_last_insert.insert(recent.begin(), recent.end());
				}
			}
			
			if (aut.final(out_edges.begin()->second)) {
				for (Evaluate_Filter::CFA::edges_t::iterator l_iter(m_last_insert.begin());
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
				Evaluate_Filter::CFA::edges_t to_insert;
				to_insert.swap(m_last_insert);
				for (Evaluate_Filter::CFA::edges_t::iterator l_iter(to_insert.begin());
						l_iter != to_insert.end(); ++l_iter) {
					::fshell2::instrumentation::GOTO_Transformation::CFA::edges_t const& recent(
							m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER, *l_iter, tmp));
					m_last_insert.insert(recent.begin(), recent.end());
				}
			}
			
			if (aut.final(out_edges.begin()->second)) {
				for (Evaluate_Filter::CFA::edges_t::iterator l_iter(m_last_insert.begin());
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
			Evaluate_Filter::CFA::edges_t const& filter_val(m_filter_eval.get(*(out_edges.begin()->first)));
			for (Evaluate_Filter::CFA::edges_t::const_iterator f_iter(filter_val.begin());
					f_iter != filter_val.end(); ++f_iter) {
				::fshell2::instrumentation::GOTO_Transformation::CFA::edges_t const& recent(
						m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::AFTER, *f_iter, tmp));
				m_last_insert.insert(recent.begin(), recent.end());
			}
			
			if (aut.final(out_edges.begin()->second)) {
				for (Evaluate_Filter::CFA::edges_t::iterator l_iter(m_last_insert.begin());
						l_iter != m_last_insert.end(); ++l_iter) {
					::goto_programt::targett pred(l_iter->first);
					pred--;
					m_acc_edges.insert(::std::make_pair(pred, l_iter->first));
				}
			}
		} else {
			FSHELL2_AUDIT_ASSERT(Violated_Invariance, out_edges.empty());
		}
		
		processing_stack.push_front(out_edges.begin()->second);
	}
}
*/
	
typedef struct {
} empty_t;
typedef ::extended_cfgt< empty_t > cfg_t;

void Automaton_Inserter::insert() {
	typedef Evaluate_Path_Monitor::trace_automaton_t trace_automaton_t;
	
	trace_automaton_t const& c_s_aut(m_pm_eval.get_cov_seq_aut());
	trace_automaton_t const& p_aut(m_pm_eval.get_passing_aut());
	
	// traverse the automata and collect relevant filters; for each filter build
	// a map< state_t, set< state_t > >
	// add a map from states to new indices starting at 1, 0 is invalid state,
	// store accepting state for passing and cov_seq

	::std::map< trace_automaton_t::state_type, trace_automaton_t::state_type > c_s_state_map,
		p_state_map;
	trace_automaton_t::state_type c_s_initial(0), p_initial(0);
	::std::set< trace_automaton_t::state_type > c_s_final, p_final;

	trace_automaton_t::state_type idx(0);
	for (trace_automaton_t::const_iterator iter(c_s_aut.begin());
			iter != c_s_aut.end(); ++iter) {
		c_s_state_map.insert(::std::make_pair(*iter, ++idx));
		if (c_s_aut.final(*iter)) c_s_final.insert(idx);
	}
	c_s_initial = c_s_state_map.find(*(c_s_aut.initial().begin()))->second;

	idx = 0;
	for (trace_automaton_t::const_iterator iter(p_aut.begin());
			iter != p_aut.end(); ++iter) {
		p_state_map.insert(::std::make_pair(*iter, ++idx));
		if (p_aut.final(*iter)) p_final.insert(idx);
	}
	p_initial = p_state_map.find(*(p_aut.initial().begin()))->second;

	::goto_programt defs;

	{
		int log_2_rounded(1);
		int num_states(c_s_aut.state_count());
		while (num_states >>= 1) ++log_2_rounded;
		::symbolt c_s_symb;
		c_s_symb.mode = "C";
		c_s_symb.base_name = "state$c_s";
		c_s_symb.name = "c::state$c_s";
		c_s_symb.static_lifetime = true;
		c_s_symb.lvalue = true;
		c_s_symb.type = ::unsignedbv_typet(log_2_rounded);
		m_context.move(c_s_symb);
	}
	::symbolst::const_iterator c_s_symb_entry(m_context.symbols.find("c::state$c_s"));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, c_s_symb_entry != m_context.symbols.end());
	::symbolt const& c_s_symb(c_s_symb_entry->second);
	::goto_programt::targett c_s_init(defs.add_instruction(ASSIGN));
	c_s_init->code = ::code_assignt(::symbol_expr(c_s_symb), ::from_integer(
				c_s_initial, c_s_symb.type));

	{
		int log_2_rounded(1);
		int num_states(p_aut.state_count());
		while (num_states >>= 1) ++log_2_rounded;
		::symbolt p_symb;
		p_symb.mode = "C";
		p_symb.base_name = "state$p";
		p_symb.name = "c::state$p";
		p_symb.static_lifetime = true;
		p_symb.lvalue = true;
		p_symb.type = ::unsignedbv_typet(log_2_rounded);
		m_context.move(p_symb);
	}
	::symbolst::const_iterator p_symb_entry(m_context.symbols.find("c::state$p"));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, p_symb_entry != m_context.symbols.end());
	::symbolt const& p_symb(p_symb_entry->second);
	::goto_programt::targett p_init(defs.add_instruction(ASSIGN));
	p_init->code = ::code_assignt(::symbol_expr(p_symb), ::from_integer(
				p_initial, p_symb.type));
	m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::FUNCTION_CALL, defs);

	typedef ::std::map< int, Filter const* > index_to_filter_t;
	index_to_filter_t index_to_filter;
	typedef ::std::map< int, ::std::map< trace_automaton_t::state_type,
			::std::set< trace_automaton_t::state_type > > > transition_map_t;
	transition_map_t c_s_transitions, p_transitions;

	for (trace_automaton_t::const_iterator iter(c_s_aut.begin());
			iter != c_s_aut.end(); ++iter) {
		trace_automaton_t::state_type const mapped_state(c_s_state_map.find(*iter)->second);
		trace_automaton_t::edges_type const out_edges(c_s_aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter) {
			::std::pair< index_to_filter_t::iterator, bool > entry(index_to_filter.insert(
						::std::make_pair< int, Filter const* >(e_iter->first, 0)));
			if (entry.second) {
				entry.first->second = m_pm_eval.lookup_index(e_iter->first);
			}

			c_s_transitions[ e_iter->first ][ mapped_state ].insert(
					c_s_state_map.find(e_iter->second)->second);
		}
	}

	for (trace_automaton_t::const_iterator iter(p_aut.begin());
			iter != p_aut.end(); ++iter) {
		trace_automaton_t::state_type const mapped_state(p_state_map.find(*iter)->second);
		trace_automaton_t::edges_type const out_edges(p_aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter) {
			::std::pair< index_to_filter_t::iterator, bool > entry(index_to_filter.insert(
						::std::make_pair< int, Filter const* >(e_iter->first, 0)));
			if (entry.second) {
				entry.first->second = m_pm_eval.lookup_index(e_iter->first);
			}

			p_transitions[ e_iter->first ][ mapped_state ].insert(
					p_state_map.find(e_iter->second)->second);
		}
	}

	// map edges to filter evaluations
	typedef ::std::multimap< ::goto_programt::targett, ::std::pair< int, CFA::edge_t > > node_to_filter_t;
	node_to_filter_t node_to_filter;

	for (index_to_filter_t::const_iterator iter(index_to_filter.begin());
			iter != index_to_filter.end(); ++iter) {
		if (m_pm_eval.id_index() == iter->first) continue;
		// copying, we need non-const stuff in here
		CFA target_graph(m_filter_eval.get(*(iter->second)));
		for (CFA::edges_t::iterator e_iter(target_graph.get_edges().begin());
				e_iter != target_graph.get_edges().end(); ++e_iter) {
			node_to_filter.insert(::std::make_pair(e_iter->first.second,
						::std::make_pair(iter->first, *e_iter)));
		}
	}

	cfg_t cfg;
	cfg(m_gf);

	for (::goto_functionst::function_mapt::iterator iter(m_gf.function_map.begin());
			iter != m_gf.function_map.end(); ++iter) {
		if (!iter->second.body_available) continue;
		// skip CBMC main
		if (iter->first == "main") continue;
		for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
				f_iter != iter->second.body.instructions.end(); ++f_iter) {
			cfg_t::entriest::iterator cfg_node(cfg.entries.find(f_iter));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != cfg.entries.end());
			// maybe this function is never called, then there is no successor,
			// we could have skipped this altogether
			if (cfg_node->second.successors.empty() && f_iter->is_end_function()) continue;
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !cfg_node->second.successors.empty());
			::std::pair< node_to_filter_t::const_iterator, node_to_filter_t::const_iterator >
				nodes(node_to_filter.equal_range(f_iter));
			// TODO: check consistency and add condition, where necessary
			/*for (cfg_t::successorst::iterator s_iter(cfg_node->second.successors.begin());
					s_iter != cfg_node->second.successors.end(); ++s_iter) {
				initial.insert(::std::make_pair(&(iter->second.body), f_iter));
				edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), *s_iter));
			}*/
			node_to_filter_t::const_iterator count_iter(nodes.first);
			int num_options(1);
			for (;count_iter != nodes.second; ++count_iter) ++num_options;
			::goto_programt tmp;
			::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
					m_inserter.make_nondet_choice(tmp, num_options, m_context));
			::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
					targets.begin());
			for (; nodes.first != nodes.second; ++nodes.first, ++t_iter) {
				t_iter->second->type = FUNCTION_CALL;
				::code_function_callt fct;
				fct.function() = ::exprt("symbol");
				fct.function().set("identifier", ::diagnostics::internal::to_string(
							"c::filter_trans$", nodes.first->second.first));
				t_iter->second->code = fct;
			}
			t_iter->second->type = FUNCTION_CALL;
			::code_function_callt fct;
			fct.function() = ::exprt("symbol");
			fct.function().set("identifier", ::diagnostics::internal::to_string(
						"c::filter_trans$", m_pm_eval.id_index()));
			t_iter->second->code = fct;
			
			m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::std::make_pair(
						::std::make_pair(&(iter->second.body), f_iter),
						cfg_node->second.successors.front()), tmp);
		}
	}
	
	// for each map entry add a function later on filter_trans$i with lots of
	// transitions, some non-det as induced by map; add else = 0
	for (transition_map_t::const_iterator iter(c_s_transitions.begin());
			iter != c_s_transitions.end(); ++iter) {
		::std::string const func_name(::diagnostics::internal::to_string("c::filter_trans$",
					iter->first));
		::goto_functionst::function_mapt::iterator entry(m_gf.function_map.insert(::std::make_pair(
						func_name, ::goto_functionst::goto_functiont())).first);
		entry->second.body_available = true;
		entry->second.type.return_type() = ::empty_typet();
		::symbol_exprt func_expr(func_name, ::typet("code"));
		::symbolt func_symb;
		func_symb.from_irep(func_expr);
		func_symb.value = ::code_blockt();
		func_symb.mode = "C";
		func_symb.name = func_name;
		func_symb.base_name = func_name.substr(3, ::std::string::npos);
		m_context.move(func_symb);
		
		::goto_programt & body(entry->second.body);
		::goto_programt tmp_target;
		::goto_programt::targett out_target(tmp_target.add_instruction(SKIP));

		for (::std::map< trace_automaton_t::state_type, ::std::set< trace_automaton_t::state_type > >::
				const_iterator tr_iter(iter->second.begin()); tr_iter != iter->second.end();
				++tr_iter) {
			// code: if state$c_s == t_iter->first && non_det_expr state$c_s =
			// some t_iter->second
			::goto_programt::targett if_stmt(body.add_instruction());

			::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
					m_inserter.make_nondet_choice(body, tr_iter->second.size(), m_context));
			::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
					targets.begin());

			for (::std::set< trace_automaton_t::state_type >::const_iterator s_iter(tr_iter->second.begin());
					s_iter != tr_iter->second.end(); ++s_iter, ++t_iter) {
				t_iter->second->type = ASSIGN;
				t_iter->second->code = ::code_assignt(::symbol_expr(c_s_symb), ::from_integer(
							*s_iter, c_s_symb.type));
			}

			::goto_programt::targett goto_stmt(body.add_instruction());
			goto_stmt->make_goto(out_target);
			goto_stmt->guard.make_true();

			::goto_programt::targett others_target(body.add_instruction(SKIP));
			if_stmt->make_goto(others_target);
			if_stmt->guard = ::binary_relation_exprt(::symbol_expr(c_s_symb), "=",
					::from_integer(tr_iter->first, c_s_symb.type));
			if_stmt->guard.make_not();
		}

		::goto_programt::targett else_target(body.add_instruction(ASSIGN));
		else_target->code = ::code_assignt(::symbol_expr(c_s_symb), ::from_integer(0,
					c_s_symb.type));
		body.destructive_append(tmp_target);
	}
	
	// for each map entry add a function later on filter_trans$i with lots of
	// transitions, some non-det as induced by map; add else = 0
	for (transition_map_t::const_iterator iter(p_transitions.begin());
			iter != p_transitions.end(); ++iter) {
		::std::string const func_name(::diagnostics::internal::to_string("c::filter_trans$",
					iter->first));
		::std::pair< ::goto_functionst::function_mapt::iterator, bool > entry(
				m_gf.function_map.insert(::std::make_pair(func_name, ::goto_functionst::goto_functiont())));
		if (entry.second) {
			entry.first->second.body_available = true;
			entry.first->second.type.return_type() = ::empty_typet();
			::symbol_exprt func_expr(func_name, ::typet("code"));
			::symbolt func_symb;
			func_symb.from_irep(func_expr);
			func_symb.value = ::code_blockt();
			func_symb.mode = "C";
			func_symb.name = func_name;
			func_symb.base_name = func_name.substr(3, ::std::string::npos);
			m_context.move(func_symb);
		}
		
		::goto_programt & body(entry.first->second.body);
		::goto_programt tmp_target;
		::goto_programt::targett out_target(tmp_target.add_instruction(SKIP));

		for (::std::map< trace_automaton_t::state_type, ::std::set< trace_automaton_t::state_type > >::
				const_iterator tr_iter(iter->second.begin()); tr_iter != iter->second.end();
				++tr_iter) {
			// code: if state$p == t_iter->first && non_det_expr state$p =
			// some t_iter->second
			::goto_programt::targett if_stmt(body.add_instruction());

			::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
					m_inserter.make_nondet_choice(body, tr_iter->second.size(), m_context));
			::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
					targets.begin());

			for (::std::set< trace_automaton_t::state_type >::const_iterator s_iter(tr_iter->second.begin());
					s_iter != tr_iter->second.end(); ++s_iter, ++t_iter) {
				t_iter->second->type = ASSIGN;
				t_iter->second->code = ::code_assignt(::symbol_expr(p_symb), ::from_integer(
							*s_iter, p_symb.type));
			}

			::goto_programt::targett goto_stmt(body.add_instruction());
			goto_stmt->make_goto(out_target);
			goto_stmt->guard.make_true();

			::goto_programt::targett others_target(body.add_instruction(SKIP));
			if_stmt->make_goto(others_target);
			if_stmt->guard = ::binary_relation_exprt(::symbol_expr(p_symb), "=",
					::from_integer(tr_iter->first, p_symb.type));
			if_stmt->guard.make_not();
		}

		::goto_programt::targett else_target(body.add_instruction(ASSIGN));
		else_target->code = ::code_assignt(::symbol_expr(p_symb), ::from_integer(0,
					p_symb.type));
		body.destructive_append(tmp_target);
	}

	// complete functions
	for (index_to_filter_t::const_iterator iter(index_to_filter.begin());
			iter != index_to_filter.end(); ++iter) {
		::std::string const func_name(::diagnostics::internal::to_string("c::filter_trans$",
					iter->first));
		m_gf.function_map[func_name].body.add_instruction(END_FUNCTION);
		m_gf.function_map[func_name].body.update();
	}
	
	// final assertion
	::exprt * c_s_full_cond(0);
	for (::std::set< trace_automaton_t::state_type >::const_iterator iter(c_s_final.begin());
			iter != c_s_final.end(); ++iter) {
		if (!c_s_full_cond) {
			c_s_full_cond = new ::binary_relation_exprt(::symbol_expr(c_s_symb), "=",
						::from_integer(*iter, c_s_symb.type));
		} else {
			::exprt * c_s_full_cond_bak(c_s_full_cond);
			c_s_full_cond = new ::or_exprt(*c_s_full_cond, ::binary_relation_exprt(
						::symbol_expr(c_s_symb), "=", ::from_integer(*iter, c_s_symb.type)));
			delete c_s_full_cond_bak;
		}
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, c_s_full_cond != 0);
	
	::exprt * p_full_cond(0);
	for (::std::set< trace_automaton_t::state_type >::const_iterator iter(p_final.begin());
			iter != p_final.end(); ++iter) {
		if (!p_full_cond) {
			p_full_cond = new ::binary_relation_exprt(::symbol_expr(p_symb), "=",
						::from_integer(*iter, p_symb.type));
		} else {
			::exprt * p_full_cond_bak(p_full_cond);
			p_full_cond = new ::or_exprt(*p_full_cond, ::binary_relation_exprt(
						::symbol_expr(p_symb), "=", ::from_integer(*iter, p_symb.type)));
			delete p_full_cond_bak;
		}
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, p_full_cond != 0);
	
	::goto_programt tmp;
	::goto_programt::targett as(tmp.add_instruction(ASSERT));
	as->code = ::code_assertt();
	as->guard = ::and_exprt(*c_s_full_cond, *p_full_cond);
	as->guard.make_not();
	delete c_s_full_cond;
	delete p_full_cond;
	
	m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp);
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

