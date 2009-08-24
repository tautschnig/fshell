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
#include <diagnostics/basic_exceptions/not_implemented.hpp>

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
	m_inserter(*(new ::fshell2::instrumentation::GOTO_Transformation(m_gf))),
	m_cfg(*new cfg_t()) {
	m_cfg(m_gf);
}

Automaton_Inserter::~Automaton_Inserter() {
	delete &m_cfg;
	delete &m_inserter;
}

typedef Evaluate_Path_Monitor::trace_automaton_t trace_automaton_t;

void Automaton_Inserter::insert(char const * suffix, trace_automaton_t const& aut, ::exprt & final_cond) {
	// traverse the automaton and collect relevant filters; for each filter build
	// a map< state_t, set< state_t > >
	// add a map from states to new indices for efficiency
	// store accepting state for passing and cov_seq

	::std::map< trace_automaton_t::state_type, trace_automaton_t::state_type > state_map;
	::std::set< trace_automaton_t::state_type > final;

	trace_automaton_t::state_type idx(0);
	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		trace_automaton_t::state_type const cur_idx(idx++);
		state_map.insert(::std::make_pair(*iter, cur_idx));
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
		state_symb.base_name = ::diagnostics::internal::to_string("state$", suffix);
		state_symb.name = ::diagnostics::internal::to_string("c::state$", suffix);
		state_symb.static_lifetime = true;
		state_symb.lvalue = true;
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, ::std::numeric_limits<
				trace_automaton_t::state_type >::min() >= 0);
		state_symb.type = ::unsignedbv_typet(log_2_rounded);
		m_context.move(state_symb);
	}
	::symbolst::const_iterator state_symb_entry(m_context.symbols.find(
				::diagnostics::internal::to_string("c::state$", suffix)));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, state_symb_entry != m_context.symbols.end());
	::symbolt const& state_symb(state_symb_entry->second);
	::goto_programt::targett state_init(defs.add_instruction(ASSIGN));
	state_init->code = ::code_assignt(::symbol_expr(state_symb), ::from_integer(
				initial, state_symb.type));

	m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::FUNCTION_CALL, defs);

	typedef ::std::map< int, Filter const* > index_to_filter_t;
	index_to_filter_t index_to_filter;
	typedef ::std::map< int, ::std::map< trace_automaton_t::state_type,
			::std::set< trace_automaton_t::state_type > > > transition_map_t;
	transition_map_t transitions;

	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		trace_automaton_t::state_type const mapped_state(state_map.find(*iter)->second);
		trace_automaton_t::edges_type const out_edges(aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter) {
			::std::pair< index_to_filter_t::iterator, bool > entry(index_to_filter.insert(
						::std::make_pair< int, Filter const* >(e_iter->first, 0)));
			if (entry.second) {
				entry.first->second = m_pm_eval.lookup_index(e_iter->first);
			}

			transitions[ e_iter->first ][ mapped_state ].insert(
					state_map.find(e_iter->second)->second);
		}
	}

	// map edges to filter evaluations
	typedef ::std::map< ::goto_programt::targett, 
			::std::map< CFA::edge_t, ::std::set< int > > > node_to_filter_t;
	node_to_filter_t node_to_filter;

	for (index_to_filter_t::const_iterator iter(index_to_filter.begin());
			iter != index_to_filter.end(); ++iter) {
		if (m_pm_eval.id_index() == iter->first) continue;
		// copying, we need non-const stuff in here
		CFA target_graph(m_filter_eval.get(*(iter->second)));
		for (CFA::edges_t::iterator e_iter(target_graph.get_edges().begin());
				e_iter != target_graph.get_edges().end(); ++e_iter) {
			node_to_filter[ e_iter->first.second ][ *e_iter ].insert(iter->first);
		}
	}

	for (::goto_functionst::function_mapt::iterator iter(m_gf.function_map.begin());
			iter != m_gf.function_map.end(); ++iter) {
		if (!iter->second.body_available) continue;
		::goto_programt::instructionst::iterator end_func_iter(iter->second.body.instructions.end());
		--end_func_iter;
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, end_func_iter->is_end_function());
		cfg_t::entriest::iterator cfg_node_for_end_func(m_cfg.entries.find(end_func_iter));
		// function may have been added by earlier instrumentation
		if (m_cfg.entries.end() == cfg_node_for_end_func) continue;
		// maybe this function is never called, then there is no successor,
		// and we can skip this function altogether; thereby we also skip CBMC
		// main
		if (cfg_node_for_end_func->second.successors.empty()) continue;
		
		for (::goto_programt::instructionst::iterator i_iter(iter->second.body.instructions.begin());
				i_iter != iter->second.body.instructions.end(); ++i_iter) {
			cfg_t::entriest::iterator cfg_node(m_cfg.entries.find(i_iter));
			// statement may have been added by earlier instrumentation
			if (m_cfg.entries.end() == cfg_node) continue;
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !cfg_node->second.successors.empty());
			
			node_to_filter_t::const_iterator entry(node_to_filter.find(i_iter));
			if (node_to_filter.end() == entry) {
				::goto_programt tmp;
				::goto_programt::targett call(tmp.add_instruction(FUNCTION_CALL));
				::code_function_callt fct;
				fct.function() = ::exprt("symbol");
				fct.function().set("identifier", ::diagnostics::internal::to_string(
							"c::filter_trans$", suffix, "$", m_pm_eval.id_index()));
				call->code = fct;
				m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::std::make_pair(
							::std::make_pair(&(iter->second.body), i_iter),
							cfg_node->second.successors.front()), tmp);
			} else {
				FSHELL2_PROD_ASSERT(::diagnostics::Not_Implemented, (cfg_node->second.successors.size() == 1) ||
						i_iter->is_goto());
				// ensure that the symmetric difference between CFG successors
				// and filter entries is the same is the missing entries in
				// filter map
				int missing(0);

				for (cfg_t::successorst::iterator s_iter(cfg_node->second.successors.begin());
						s_iter != cfg_node->second.successors.end(); ++s_iter) {
					::std::map< CFA::edge_t, ::std::set< int > >::const_iterator edge_map_iter(
							entry->second.find(::std::make_pair(::std::make_pair(&(iter->second.body), i_iter), *s_iter)));
					::std::set< int >::size_type count(0);
					if (entry->second.end() != edge_map_iter) {
						count = edge_map_iter->second.size();
					} else {
						++missing;
					}
					
					::goto_programt tmp;
					::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
							m_inserter.make_nondet_choice(tmp, count + 1, m_context));
					::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
							targets.begin());
					if (entry->second.end() != edge_map_iter) {
						for (::std::set< int >::const_iterator f_iter(edge_map_iter->second.begin());
								f_iter != edge_map_iter->second.end(); ++f_iter, ++t_iter) {
							t_iter->second->type = FUNCTION_CALL;
							::code_function_callt fct;
							fct.function() = ::exprt("symbol");
							fct.function().set("identifier", ::diagnostics::internal::to_string(
										"c::filter_trans$", suffix, "$", *f_iter));
							t_iter->second->code = fct;
						}
					}
					t_iter->second->type = FUNCTION_CALL;
					::code_function_callt fct;
					fct.function() = ::exprt("symbol");
					fct.function().set("identifier", ::diagnostics::internal::to_string(
								"c::filter_trans$", suffix, "$", m_pm_eval.id_index()));
					t_iter->second->code = fct;

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
						if (s_iter->second != i_iter->targets.front()) if_stmt->guard.make_not();

						tmp.destructive_insert(tmp.instructions.begin(), if_prg);
					}
					
					m_inserter.insert(::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::std::make_pair(
								::std::make_pair(&(iter->second.body), i_iter), *s_iter), tmp);
				}

				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node->second.successors.size() - missing ==
						entry->second.size());
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
			// code: if state == t_iter->first && non_det_expr state =
			// some t_iter->second
			::goto_programt::targett if_stmt(body.add_instruction());

			::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
					m_inserter.make_nondet_choice(body, tr_iter->second.size(), m_context));
			::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
					targets.begin());

			for (::std::set< trace_automaton_t::state_type >::const_iterator s_iter(tr_iter->second.begin());
					s_iter != tr_iter->second.end(); ++s_iter, ++t_iter) {
				t_iter->second->type = ASSIGN;
				t_iter->second->code = ::code_assignt(::symbol_expr(state_symb), ::from_integer(
							*s_iter, state_symb.type));
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
		body.update();
	}
	
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
	insert("c_s", m_pm_eval.get_cov_seq_aut(), c_s_final_cond);
	insert("p", m_pm_eval.get_passing_aut(), p_final_cond);
	
	::goto_programt tmp;
	::goto_programt::targett as(tmp.add_instruction(ASSERT));
	as->make_assertion(::and_exprt(c_s_final_cond, p_final_cond));
	as->guard.make_not();
	
	m_inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp);
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

