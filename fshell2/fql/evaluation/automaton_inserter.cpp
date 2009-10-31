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
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>
#include <fshell2/fql/ast/query.hpp>

#include <limits>

#include <cbmc/src/util/std_code.h>
#include <cbmc/src/util/std_expr.h>
#include <cbmc/src/util/arith_tools.h>
#include <cbmc/src/util/expr_util.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
			
Automaton_Inserter::Automaton_Inserter(Evaluate_Path_Monitor const& pm_eval,
			Evaluate_Filter const& filter_eval, ::goto_functionst & gf,
			::fshell2::instrumentation::CFG & cfg,
			::contextt & context) :
	m_pm_eval(pm_eval), m_filter_eval(filter_eval), m_gf(gf),
	m_context(context),
	m_cfg(cfg),
	m_inserter(m_gf) {
}

Automaton_Inserter::instrumentation_points_t const& Automaton_Inserter::get_test_goal_instrumentation(
			Evaluate_Path_Monitor::trace_automaton_t::state_type const& state) const {
	instrumentation_map_t::const_iterator entry(m_tg_instrumentation_map.find(state));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_tg_instrumentation_map.end());
	return entry->second;
}
	
CFA::edge_t const& Automaton_Inserter::get_target_graph_edge(::goto_programt::const_targett const& node) const {
	::std::map< ::goto_programt::const_targett, CFA::edge_t >::const_iterator entry(
			m_target_edge_map.find(node));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_target_edge_map.end());
	return entry->second;
}

typedef Evaluate_Path_Monitor::trace_automaton_t trace_automaton_t;

void Automaton_Inserter::insert(char const * suffix, trace_automaton_t const& aut,
		::exprt & final_cond, bool map_tg) {
	using ::fshell2::instrumentation::CFG;

	// traverse the automaton and collect relevant filters; for each filter build
	// a map< state_t, set< state_t > >
	// add a map from states to new indices for efficiency
	// store accepting state for passing and cov_seq

	::std::map< trace_automaton_t::state_type, trace_automaton_t::state_type > state_map;
	::std::map< trace_automaton_t::state_type, trace_automaton_t::state_type > reverse_state_map;
	::std::set< trace_automaton_t::state_type > final;

	trace_automaton_t::state_type idx(0);
	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		trace_automaton_t::state_type const cur_idx(idx++);
		state_map.insert(::std::make_pair(*iter, cur_idx));
		reverse_state_map.insert(::std::make_pair(cur_idx, *iter));
		if (aut.final(*iter)) final.insert(cur_idx);
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !aut.initial().empty());
	trace_automaton_t::state_type initial(state_map.find(*(aut.initial().begin()))->second);

	::goto_programt defs;

	{
		int log_2_rounded(1);
		int num_states(aut.state_count());
		while (num_states >>= 1) ++log_2_rounded;
		::symbolt state_symb;
		state_symb.mode = "C";
		state_symb.base_name = ::diagnostics::internal::to_string("!fshell2!state$", suffix);
		state_symb.name = ::diagnostics::internal::to_string("c::!fshell2!state$", suffix);
		state_symb.static_lifetime = true;
		state_symb.lvalue = true;
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, ::std::numeric_limits<
				trace_automaton_t::state_type >::min() >= 0);
		state_symb.type = ::unsignedbv_typet(log_2_rounded);
		m_context.move(state_symb);
	}
	::symbolst::const_iterator state_symb_entry(m_context.symbols.find(
				::diagnostics::internal::to_string("c::!fshell2!state$", suffix)));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, state_symb_entry != m_context.symbols.end());
	::symbolt const& state_symb(state_symb_entry->second);
	::goto_programt::targett state_init(defs.add_instruction(ASSIGN));
	state_init->code = ::code_assignt(::symbol_expr(state_symb), ::from_integer(
				initial, state_symb.type));

	m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::FUNCTION_CALL, defs);

	// we only need a subset of the filters and we should better know which ones
	typedef ::std::map< Filter const*, int > filter_to_int_t;
	filter_to_int_t local_filter_map;
	typedef ::std::map< int, ::std::map< trace_automaton_t::state_type,
			::std::set< trace_automaton_t::state_type > > > transition_map_t;
	transition_map_t transitions;

	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		trace_automaton_t::state_type const mapped_state(state_map.find(*iter)->second);
		trace_automaton_t::edges_type const out_edges(aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter) {
			transitions[ e_iter->first ][ mapped_state ].insert(
					state_map.find(e_iter->second)->second);
			
			if (m_pm_eval.id_index() == e_iter->first) continue;
			Filter const* flt(m_pm_eval.lookup_index(e_iter->first));
			if (local_filter_map.end() == local_filter_map.find(flt))
				local_filter_map[ flt ] = e_iter->first;
		}
	}

	for (::goto_functionst::function_mapt::iterator iter(m_gf.function_map.begin());
			iter != m_gf.function_map.end(); ++iter) {
		if (Evaluate_Filter::skip_function(iter->second)) continue;
		::goto_programt::instructionst::iterator end_func_iter(iter->second.body.instructions.end());
		--end_func_iter;
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, end_func_iter->is_end_function());
		CFG::entries_t::iterator cfg_node_for_end_func(m_cfg.find(end_func_iter));
		// function may have been added by earlier instrumentation
		if (::fshell2::instrumentation::GOTO_Transformation::is_instrumented(end_func_iter)) continue;
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_cfg.end() != cfg_node_for_end_func);
		// maybe this function is never called, then there is no successor,
		// and we can skip this function altogether
		if (cfg_node_for_end_func->second.successors.empty()) continue;
		
		for (::goto_programt::instructionst::iterator i_iter(iter->second.body.instructions.begin());
				i_iter != iter->second.body.instructions.end(); ++i_iter) {
			CFG::entries_t::iterator cfg_node(m_cfg.find(i_iter));
			// statement may have been added by earlier instrumentation
			if (::fshell2::instrumentation::GOTO_Transformation::is_instrumented(i_iter)) continue;
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_cfg.end() != cfg_node);
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !cfg_node->second.successors.empty());
			
			Evaluate_Filter::edge_to_filters_t const& entry(m_filter_eval.get(i_iter));
			if (entry.empty()) {
				::goto_programt tmp;
				/*if (map_tg) {
					::goto_programt::targett call(tmp.add_instruction(FUNCTION_CALL));
					::code_function_callt fct;
					fct.function() = ::exprt("symbol");
					fct.function().set("identifier", ::diagnostics::internal::to_string(
								"c::tg_record$", suffix, "$", m_pm_eval.id_index()));
					call->code = fct;
				}*/
				::goto_programt::targett call(tmp.add_instruction(FUNCTION_CALL));
				::code_function_callt fct;
				fct.function() = ::exprt("symbol");
				fct.function().set("identifier", ::diagnostics::internal::to_string(
							"c::filter_trans$", suffix, "$", m_pm_eval.id_index()));
				call->code = fct;
				
				CFA::edge_t edge(::std::make_pair(&(iter->second.body), i_iter),
							cfg_node->second.successors.front());
				for (::goto_programt::const_targett tmp_iter(tmp.instructions.begin());
						tmp_iter != tmp.instructions.end(); ++tmp_iter)
					m_target_edge_map.insert(::std::make_pair(tmp_iter, edge)); 
			
				m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::BEFORE, edge, tmp);
			} else {
				FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, (cfg_node->second.successors.size() == 1) ||
						i_iter->is_goto());
				for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
						s_iter != cfg_node->second.successors.end(); ++s_iter) {
					Evaluate_Filter::edge_to_filters_t::const_iterator edge_map_iter(
							entry.find(::std::make_pair(::std::make_pair(&(iter->second.body), i_iter), *s_iter)));
					::std::set< int > filters;
					if (entry.end() != edge_map_iter) {
						for (::std::set< Filter const* >::const_iterator f_iter(edge_map_iter->second.begin());
							f_iter != edge_map_iter->second.end(); ++f_iter) {
							filter_to_int_t::const_iterator int_entry(local_filter_map.find(*f_iter));
							if (local_filter_map.end() == int_entry) continue;
							filters.insert(int_entry->second);
						}
					}
				
					//::goto_programt tg_record;
					::goto_programt tmp;
					::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
							m_inserter.make_nondet_choice(tmp, filters.size() + 1, m_context));
					::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
							targets.begin());
					for (::std::set< int >::const_iterator f_iter(filters.begin()); f_iter != filters.end(); 
							++f_iter, ++t_iter) {
						/*if (map_tg) {
							::goto_programt::targett call(tg_record.add_instruction(FUNCTION_CALL));
							::code_function_callt fct;
							fct.function() = ::exprt("symbol");
							fct.function().set("identifier", ::diagnostics::internal::to_string(
										"c::tg_record$", suffix, "$", *f_iter));
							call->code = fct;
						}*/
						t_iter->second->type = FUNCTION_CALL;
						::code_function_callt fct;
						fct.function() = ::exprt("symbol");
						fct.function().set("identifier", ::diagnostics::internal::to_string(
									"c::filter_trans$", suffix, "$", *f_iter));
						t_iter->second->code = fct;
					}
					/*if (map_tg) {
						::goto_programt::targett call(tg_record.add_instruction(FUNCTION_CALL));
						::code_function_callt fct;
						fct.function() = ::exprt("symbol");
						fct.function().set("identifier", ::diagnostics::internal::to_string(
									"c::tg_record$", suffix, "$", m_pm_eval.id_index()));
						call->code = fct;
					}*/
					t_iter->second->type = FUNCTION_CALL;
					::code_function_callt fct;
					fct.function() = ::exprt("symbol");
					fct.function().set("identifier", ::diagnostics::internal::to_string(
								"c::filter_trans$", suffix, "$", m_pm_eval.id_index()));
					t_iter->second->code = fct;

					//tmp.destructive_insert(tmp.instructions.begin(), tg_record);

					if (i_iter->is_goto()) {
						// CBMC doesn't support non-det goto statements at the moment, but
						// just in case it ever does ...
						FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, (cfg_node->second.successors.size() == 1) ||
								(cfg_node->second.successors.size() == 2));
						FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, 1 == i_iter->targets.size());

						::goto_programt::targett out_target(tmp.add_instruction(SKIP));
						::goto_programt if_prg;
						::goto_programt::targett if_stmt(if_prg.add_instruction());
						if_stmt->make_goto(out_target);
						if_stmt->guard = i_iter->guard;
						::goto_programt::targett succ(i_iter);
						++succ;
						if (s_iter->second == succ) if_stmt->guard.make_not();

						tmp.destructive_insert(tmp.instructions.begin(), if_prg);
					}
					
					CFA::edge_t edge(::std::make_pair(&(iter->second.body), i_iter), *s_iter);
					for (::goto_programt::const_targett tmp_iter(tmp.instructions.begin());
							tmp_iter != tmp.instructions.end(); ++tmp_iter)
						m_target_edge_map.insert(::std::make_pair(tmp_iter, edge));

					m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::BEFORE, edge, tmp);
				}
			}
		}
	}
	
	// for each map entry add a function later on filter_trans$i with lots of
	// transitions, some non-det as induced by map; add else assume(false)
	// make sure transitions has an entry for ID
	transitions.insert(::std::make_pair(m_pm_eval.id_index(),
				::std::map< trace_automaton_t::state_type,
				::std::set< trace_automaton_t::state_type > >()));
	for (transition_map_t::const_iterator iter(transitions.begin());
			iter != transitions.end(); ++iter) {
		::std::string const func_name(::diagnostics::internal::to_string("c::filter_trans$", suffix, "$",
					iter->first));
		::std::pair< ::goto_functionst::function_mapt::iterator, bool > entry(
				m_gf.function_map.insert(::std::make_pair(func_name, ::goto_functionst::goto_functiont())));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, entry.second);
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
		
		::goto_programt & body(entry.first->second.body);
		::goto_programt tmp_target;
		::goto_programt::targett out_target(tmp_target.add_instruction(SKIP));

		for (::std::map< trace_automaton_t::state_type, ::std::set< trace_automaton_t::state_type > >::
				const_iterator tr_iter(iter->second.begin()); tr_iter != iter->second.end();
				++tr_iter) {
			// code: if state == tr_iter->first && non_det_expr state =
			// some t_iter->second
			::goto_programt::targett if_stmt(body.add_instruction());

			// compute_target_numbers (called by update in make_nondet_choice)
			// doesn't like inconsistent goto which happens because out_target
			// is not yet in body
			::goto_programt tmp_nd;
			::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
					m_inserter.make_nondet_choice(tmp_nd, tr_iter->second.size(), m_context));
			body.destructive_append(tmp_nd);
			::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
					targets.begin());

			for (::std::set< trace_automaton_t::state_type >::const_iterator s_iter(tr_iter->second.begin());
					s_iter != tr_iter->second.end(); ++s_iter, ++t_iter) {
				if (tr_iter->first == *s_iter) {
					t_iter->second->type = SKIP;
				} else {
					t_iter->second->type = ASSIGN;
					t_iter->second->code = ::code_assignt(::symbol_expr(state_symb), ::from_integer(
								*s_iter, state_symb.type));
					if (map_tg) {
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 
								reverse_state_map.end() != reverse_state_map.find(*s_iter));
						trace_automaton_t::state_type const unmapped_state(
								reverse_state_map.find(*s_iter)->second);
						if (m_pm_eval.is_test_goal_state(unmapped_state)) {
							m_tg_instrumentation_map[ unmapped_state ].push_back(*t_iter);
						}
					}
				}
			}

			::goto_programt::targett goto_stmt(body.add_instruction());
			goto_stmt->make_goto(out_target);
			goto_stmt->guard.make_true();

			::goto_programt::targett others_target(body.add_instruction(SKIP));
			if_stmt->make_goto(others_target);
			if_stmt->guard = ::binary_relation_exprt(::symbol_expr(state_symb), "=",
					::from_integer(tr_iter->first, state_symb.type));
			if_stmt->guard.make_not();
		}

		::goto_programt::targett else_target(body.add_instruction());
		else_target->make_assumption(::false_exprt());
		body.destructive_append(tmp_target);

		body.add_instruction(END_FUNCTION);
		::fshell2::instrumentation::GOTO_Transformation::mark_instrumented(body);
		body.update();
	}

	/*
	if (map_tg) {
		for (transition_map_t::const_iterator iter(transitions.begin());
				iter != transitions.end(); ++iter) {
			::std::string const func_name(::diagnostics::internal::to_string("c::tg_record$", suffix, "$",
						iter->first));
			::std::pair< ::goto_functionst::function_mapt::iterator, bool > entry(
					m_gf.function_map.insert(::std::make_pair(func_name, ::goto_functionst::goto_functiont())));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, entry.second);
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
			
			::goto_programt & body(entry.first->second.body);

			for (::std::map< trace_automaton_t::state_type, ::std::set< trace_automaton_t::state_type > >::
					const_iterator tr_iter(iter->second.begin()); tr_iter != iter->second.end();
					++tr_iter) {
				bool may_do_tg(false);
				::goto_programt do_tg_prg;
				::goto_programt::targett do_tg(do_tg_prg.add_instruction(LOCATION));
				for (::std::set< trace_automaton_t::state_type >::const_iterator s_iter(tr_iter->second.begin());
						s_iter != tr_iter->second.end(); ++s_iter) {
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 
							reverse_state_map.end() != reverse_state_map.find(*s_iter));
					trace_automaton_t::state_type const unmapped_state(
							reverse_state_map.find(*s_iter)->second);
					if (m_pm_eval.is_test_goal_state(unmapped_state)) {
						may_do_tg = true;
						m_tg_instrumentation_map[ unmapped_state ].push_back(::std::make_pair(
									&body, do_tg));
					}
				}

				if (may_do_tg) {
					::goto_programt::targett if_stmt(body.add_instruction());
					body.destructive_append(do_tg_prg);
					::goto_programt::targett others_target(body.add_instruction(SKIP));
					if_stmt->make_goto(others_target);
					if_stmt->guard = ::binary_relation_exprt(::symbol_expr(state_symb), "=",
							::from_integer(tr_iter->first, state_symb.type));
					if_stmt->guard.make_not();
				}
			}

			body.add_instruction(END_FUNCTION);
			::fshell2::instrumentation::GOTO_Transformation::mark_instrumented(body);
			body.update();
		}
	}
	*/
	
	// final assertion
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !final.empty());
	for (::std::set< trace_automaton_t::state_type >::const_iterator iter(final.begin());
			iter != final.end(); ++iter) {
		if (final.begin() == iter) {
			final_cond = ::binary_relation_exprt(::symbol_expr(state_symb), "=",
					::from_integer(*iter, state_symb.type));
		} else {
			final_cond = ::or_exprt(final_cond, ::binary_relation_exprt(
						::symbol_expr(state_symb), "=", ::from_integer(*iter, state_symb.type)));
		}
	}
}

void Automaton_Inserter::insert() {
	::exprt c_s_final_cond, p_final_cond;
	insert("c_s", m_pm_eval.get_cov_seq_aut(), c_s_final_cond, true);
	insert("p", m_pm_eval.get_passing_aut(), p_final_cond, false);
	
	::goto_programt tmp;
	::goto_programt::targett as(tmp.add_instruction(ASSERT));
	as->make_assertion(::and_exprt(c_s_final_cond, p_final_cond));
	as->guard.make_not();
	
	m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp);
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

