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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#  include <diagnostics/basic_exceptions/not_implemented.hpp>
#endif

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_pattern.hpp>
#include <fshell2/fql/evaluation/evaluate_coverage_pattern.hpp>

#include <sstream>
#include <limits>

#include <cbmc/src/util/std_code.h>
#include <cbmc/src/util/std_expr.h>
#include <cbmc/src/util/arith_tools.h>
#include <cbmc/src/util/expr_util.h>
#include <cbmc/src/langapi/language_ui.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Automaton_Inserter::Automaton_Inserter(::language_uit & manager) :
	m_manager(manager),
	m_cp_eval(m_manager),
	m_pp_eval(m_cp_eval.get_pp_eval())
{
}

Automaton_Inserter::instrumentation_points_t const& Automaton_Inserter::get_test_goal_instrumentation(
			ta_state_t const& state) const {
	instrumentation_map_t::const_iterator entry(m_tg_instrumentation_map.find(state));
	FSHELL2_DEBUG_CHECK(::diagnostics::Invalid_Protocol,
			entry != m_tg_instrumentation_map.end());
	return entry->second;
}

void Automaton_Inserter::insert(::goto_functionst & gf,
		::fshell2::instrumentation::CFG const& cfg, char const * suffix,
		trace_automaton_t const& aut, ::exprt & final_cond, bool const map_tg) {
	
	ta_state_map_t state_map, reverse_state_map;
	ta_state_set_t compact_final;
	compact_state_numbers(aut, state_map, reverse_state_map, compact_final);

	// create a symbol requiring the least sufficient number of bits
	::fshell2::instrumentation::GOTO_Transformation inserter(m_manager, gf);
	
	int log_2_rounded(1);
	int num_states(aut.state_count());
	while (num_states >>= 1) ++log_2_rounded;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, ::std::numeric_limits< ta_state_t >::min() >= 0);
	::symbolt const& state_symb(inserter.new_var(::std::string("$fshell2$state$") + suffix,
				::unsignedbv_typet(log_2_rounded), true));
	
	::goto_programt defs;
	::goto_programt::targett state_init(defs.add_instruction(ASSIGN));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == aut.initial().size());
	state_init->code = ::code_assignt(::symbol_expr(state_symb), ::from_integer(
				state_map.find(*(aut.initial().begin()))->second, state_symb.type));
	inserter.insert("c::__CPROVER_initialize", ::fshell2::instrumentation::GOTO_Transformation::BEFORE,
			::END_FUNCTION, defs);

	// traverse the automaton and collect relevant target graphs; we only need a
	// subset of the target graphs and we should better know which ones
	target_graph_to_int_t local_target_graph_map;

	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		trace_automaton_t::edges_type const out_edges(aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter) {
			// skip ID
			if (m_pp_eval.id_index() == e_iter->first) continue;
			target_graph_t const& tgg(m_pp_eval.lookup_index(e_iter->first));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tgg.get_E().empty() ||
					tgg.get_disconnected_nodes().empty());
			if (local_target_graph_map.end() == local_target_graph_map.find(&tgg)) {
				local_target_graph_map[ &tgg ] = e_iter->first;
				// store the mapping from edges to target graphs
				for (target_graph_t::edges_t::const_iterator tg_iter(tgg.get_E().begin());
						tg_iter != tgg.get_E().end(); ++tg_iter)
					m_loc_to_edge_target_graphs_map[ tg_iter->first.second ][ *tg_iter ].insert(&tgg);
				// store the mapping from nodes to target graphs
				for (target_graph_t::nodes_t::const_iterator tg_iter(tgg.get_disconnected_nodes().begin());
						tg_iter != tgg.get_disconnected_nodes().end(); ++tg_iter)
					m_loc_to_node_target_graphs_map[ tg_iter->second ][ *tg_iter ].insert(&tgg);
			}
		}
	}

	// insert function calls
	for (::goto_functionst::function_mapt::iterator iter(gf.function_map.begin());
			iter != gf.function_map.end(); ++iter) {
		if (Evaluate_Filter::ignore_function(iter->second)) continue;
		::goto_programt::instructionst::iterator end_func_iter(iter->second.body.instructions.end());
		--end_func_iter;
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, end_func_iter->is_end_function());
		// function may have been added by earlier instrumentation
		if (::fshell2::instrumentation::GOTO_Transformation::is_instrumented(end_func_iter)) continue;
		// maybe this function is never called, then there is no successor,
		// and we can skip this function altogether
		::fshell2::instrumentation::CFG::entries_t::const_iterator
			cfg_node_for_end_func(cfg.find(end_func_iter));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg.end() != cfg_node_for_end_func);
		if (cfg_node_for_end_func->second.successors.empty()) continue;

		insert_function_calls(gf, iter->second.body, cfg, suffix, map_tg, local_target_graph_map);
	}
	
	transition_map_t transitions;
	// make sure transitions has an entry for ID
	transitions.insert(::std::make_pair(m_pp_eval.id_index(),
				::std::map< ta_state_t, ta_state_set_t >()));
	
	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		ta_state_t const mapped_state(state_map.find(*iter)->second);
		trace_automaton_t::edges_type const out_edges(aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter)
			transitions[ e_iter->first ][ mapped_state ].insert(
					state_map.find(e_iter->second)->second);
	}
	
	// for each map entry add a function later on filter_trans$i with lots of
	// transitions, some non-det as induced by map; add else assume(false)
	for (transition_map_t::const_iterator iter(transitions.begin());
			iter != transitions.end(); ++iter) {
		::std::ostringstream fct_name;
		fct_name << "c::filter_trans$" << suffix << "$" << iter->first;
		add_transition_function(gf, fct_name.str(), iter->second, state_symb,
				reverse_state_map, map_tg);
	}
	
	prepare_assertion(compact_final, final_cond, state_symb);
}
	
void Automaton_Inserter::insert_function_calls(::goto_functionst & gf,
		::goto_programt & body, ::fshell2::instrumentation::CFG const& cfg,
		char const * suffix, bool const map_tg,
		target_graph_to_int_t const& local_target_graph_map) {
	using ::fshell2::instrumentation::CFG;
	using ::fshell2::instrumentation::GOTO_Transformation;
	
	GOTO_Transformation inserter(m_manager, gf);
	
	//// int i(0);
	for (::goto_programt::instructionst::iterator iter(body.instructions.begin());
			iter != body.instructions.end(); ++iter) {
		if (Evaluate_Filter::ignore_instruction(*iter)) continue;
		//// ++i;
		//// iter->location.set_column(i);

		loc_to_node_target_graphs_t::const_iterator l_n_t_g_entry(m_loc_to_node_target_graphs_map.find(iter));
		if (m_loc_to_node_target_graphs_map.end() != l_n_t_g_entry) {
			node_to_target_graphs_t const& entry(l_n_t_g_entry->second);
			CFA::node_t node(::std::make_pair(&body, iter));
			node_to_target_graphs_t::const_iterator node_map_iter(entry.find(node));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, entry.end() != node_map_iter);
			for (::std::set< target_graph_t const* >::const_iterator f_iter(node_map_iter->second.begin());
					f_iter != node_map_iter->second.end(); ++f_iter) {
				target_graph_to_int_t::const_iterator int_entry(local_target_graph_map.find(*f_iter));
				if (local_target_graph_map.end() == int_entry) continue;
				
				::goto_programt tmp;
				GOTO_Transformation::inserted_t & targets(inserter.make_nondet_choice(tmp, 2));
				GOTO_Transformation::inserted_t::iterator t_iter(targets.begin());
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 2 == targets.size());
			
				::std::ostringstream fct_name;
				fct_name << "c::filter_trans$" << suffix << "$" << int_entry->second;
				GOTO_Transformation::make_function_call(t_iter->second, fct_name.str());
				++t_iter;
				// may be ignored, non-deterministically
				t_iter->second->type = SKIP;

				inserter.insert_at(node, tmp);
			}

		}

		loc_to_edge_target_graphs_t::const_iterator l_e_t_g_entry(m_loc_to_edge_target_graphs_map.find(iter));
		if (m_loc_to_edge_target_graphs_map.end() == l_e_t_g_entry) {
			// statement may have been added by earlier instrumentation and
			// we don't care about it anyway
			if (GOTO_Transformation::is_instrumented(iter)) continue;

			::goto_programt tmp;
			::goto_programt::targett call(tmp.add_instruction(FUNCTION_CALL));
			::std::ostringstream fct_name;
			fct_name << "c::filter_trans$" << suffix << "$" << m_pp_eval.id_index();
			GOTO_Transformation::make_function_call(call, fct_name.str());

			inserter.insert_at(::std::make_pair(&body, iter), tmp);
		} else if (GOTO_Transformation::is_instrumented(iter)) {
			// statement has been added by earlier instrumentation, but
			// we care about it! It must be a predicate-edge
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iter->is_location());
			::goto_programt::targett next(iter);
			++next;
			edge_to_target_graphs_t const& entry(l_e_t_g_entry->second);
			CFA::edge_t edge(::std::make_pair(&body, iter),
					::std::make_pair(&body, next));
			edge_to_target_graphs_t::const_iterator edge_map_iter(entry.find(edge));
			::std::set< int > target_graphs;
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, entry.end() != edge_map_iter);
			for (::std::set< target_graph_t const* >::const_iterator f_iter(edge_map_iter->second.begin());
					f_iter != edge_map_iter->second.end(); ++f_iter) {
				target_graph_to_int_t::const_iterator int_entry(local_target_graph_map.find(*f_iter));
				// ID is not included and this automaton may not even make
				// use of this edge at all
				if (local_target_graph_map.end() != int_entry) target_graphs.insert(int_entry->second);
			}
			if (target_graphs.empty()) continue;

			::goto_programt tmp;
			GOTO_Transformation::inserted_t & targets(inserter.make_nondet_choice(tmp, target_graphs.size() + 1));
			GOTO_Transformation::inserted_t::iterator t_iter(targets.begin());
			for (::std::set< int >::const_iterator f_iter(target_graphs.begin()); f_iter != target_graphs.end(); 
					++f_iter, ++t_iter) {
				::std::ostringstream fct_name;
				fct_name << "c::filter_trans$" << suffix << "$" << *f_iter;
				GOTO_Transformation::make_function_call(t_iter->second, fct_name.str());
			}
			// may be ignored, non-deterministically
			t_iter->second->type = SKIP;

			inserter.insert_at(edge.first, tmp);
		} else {
			CFG::entries_t::const_iterator cfg_node(cfg.find(iter));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg.end() != cfg_node);
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !cfg_node->second.successors.empty());

			edge_to_target_graphs_t const& entry(l_e_t_g_entry->second);
			FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, (cfg_node->second.successors.size() == 1) ||
					iter->is_goto());
			for (CFG::successors_t::const_iterator s_iter(cfg_node->second.successors.begin());
					s_iter != cfg_node->second.successors.end(); ++s_iter) {
				CFA::edge_t edge(::std::make_pair(&body, iter), s_iter->first);
				edge_to_target_graphs_t::const_iterator edge_map_iter(entry.find(edge));
				::std::set< int > target_graphs;
				if (entry.end() != edge_map_iter) {
					for (::std::set< target_graph_t const* >::const_iterator f_iter(edge_map_iter->second.begin());
							f_iter != edge_map_iter->second.end(); ++f_iter) {
						target_graph_to_int_t::const_iterator int_entry(local_target_graph_map.find(*f_iter));
						if (local_target_graph_map.end() == int_entry) continue;
						target_graphs.insert(int_entry->second);
					}
				}

				//::goto_programt tg_record;
				::goto_programt tmp;
				GOTO_Transformation::inserted_t & targets(inserter.make_nondet_choice(tmp, target_graphs.size() + 1));
				GOTO_Transformation::inserted_t::iterator t_iter(targets.begin());
				for (::std::set< int >::const_iterator f_iter(target_graphs.begin()); f_iter != target_graphs.end(); 
						++f_iter, ++t_iter) {
					::std::ostringstream fct_name;
					fct_name << "c::filter_trans$" << suffix << "$" << *f_iter;
					GOTO_Transformation::make_function_call(t_iter->second, fct_name.str());
				}
				::std::ostringstream fct_name;
				fct_name << "c::filter_trans$" << suffix << "$" << m_pp_eval.id_index();
				GOTO_Transformation::make_function_call(t_iter->second, fct_name.str());

				if (iter->is_goto()) {
					// CBMC doesn't support non-det goto statements at the moment, but
					// just in case it ever does ...
					FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, (cfg_node->second.successors.size() == 1) ||
							(cfg_node->second.successors.size() == 2));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, 1 == iter->targets.size());

					::goto_programt::targett out_target(tmp.add_instruction(SKIP));
					::goto_programt if_prg;
					::goto_programt::targett if_stmt(if_prg.add_instruction());
					if_stmt->make_goto(out_target);
					if_stmt->guard = iter->guard;
					// s_iter->second is true for the effective goto-edge;
					// then invert the guard because we want tmp to be
					// executed if we take the goto edge (and therefore must
					// not jump here)
					if (s_iter->second) if_stmt->guard.make_not();

					tmp.destructive_insert(tmp.instructions.begin(), if_prg);
				}

				inserter.insert_at(edge.first, tmp);
			}
		}
	}
}

void Automaton_Inserter::add_transition_function(::goto_functionst & gf,
		::std::string const& func_name, symbol_transition_map_t const& map,
		::symbolt const& state_symb, ta_state_map_t const& reverse_state_map,
		bool const map_tg) {
	::fshell2::instrumentation::GOTO_Transformation inserter(m_manager, gf);
	
	::std::pair< ::goto_functionst::function_mapt::iterator, bool > entry(
			gf.function_map.insert(::std::make_pair(func_name, ::goto_functionst::goto_functiont())));
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
	m_manager.context.move(func_symb);

	::goto_programt & body(entry.first->second.body);
	::goto_programt tmp_target;
	::goto_programt::targett out_target(tmp_target.add_instruction(SKIP));

	for (symbol_transition_map_t::const_iterator iter(map.begin()); iter != map.end();
			++iter) {
		// code: if state == iter->first && non_det_expr state =
		// some t_iter->second
		::goto_programt::targett if_stmt(body.add_instruction());

		// compute_target_numbers (called by update in make_nondet_choice)
		// doesn't like inconsistent goto which happens because out_target
		// is not yet in body
		::goto_programt tmp_nd;
		::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
				inserter.make_nondet_choice(tmp_nd, iter->second.size()));
		body.destructive_append(tmp_nd);
		::fshell2::instrumentation::GOTO_Transformation::inserted_t::iterator t_iter(
				targets.begin());

		for (ta_state_set_t::const_iterator s_iter(iter->second.begin());
				s_iter != iter->second.end(); ++s_iter, ++t_iter) {
			if (map_tg) {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 
						reverse_state_map.end() != reverse_state_map.find(*s_iter));
				ta_state_t const unmapped_state(
						reverse_state_map.find(*s_iter)->second);
				if (m_cp_eval.is_test_goal_state(unmapped_state)) {
					// FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iter->first != *s_iter);
					m_tg_instrumentation_map[ unmapped_state ].push_back(*t_iter);
				}
			}
			// TODO - local loops are now possible, hence also disabled
			// assertion above
			if (false && iter->first == *s_iter) {
				t_iter->second->type = SKIP;
			} else {
				t_iter->second->type = ASSIGN;
				t_iter->second->code = ::code_assignt(::symbol_expr(state_symb), ::from_integer(
							*s_iter, state_symb.type));
			}
		}

		::goto_programt::targett goto_stmt(body.add_instruction());
		goto_stmt->make_goto(out_target);
		goto_stmt->guard.make_true();

		::goto_programt::targett others_target(body.add_instruction(SKIP));
		if_stmt->make_goto(others_target);
		if_stmt->guard = ::binary_relation_exprt(::symbol_expr(state_symb), "=",
				::from_integer(iter->first, state_symb.type));
		if_stmt->guard.make_not();
	}

	::goto_programt::targett else_target(body.add_instruction());
	else_target->make_assumption(::false_exprt());
	body.destructive_append(tmp_target);

	body.add_instruction(END_FUNCTION);
	Forall_goto_program_instructions(i_iter, body) i_iter->function = func_name;

	::fshell2::instrumentation::GOTO_Transformation::mark_instrumented(body);
	body.update();
}
	
void Automaton_Inserter::prepare_assertion(ta_state_set_t const& compact_final,
		::exprt & final_cond, ::symbolt const& state_symb) {
	// final assertion
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !compact_final.empty());
	for (ta_state_set_t::const_iterator iter(compact_final.begin());
			iter != compact_final.end(); ++iter) {
		if (compact_final.begin() == iter) {
			final_cond = ::binary_relation_exprt(::symbol_expr(state_symb), "=",
					::from_integer(*iter, state_symb.type));
		} else {
			final_cond = ::or_exprt(final_cond, ::binary_relation_exprt(
						::symbol_expr(state_symb), "=", ::from_integer(*iter, state_symb.type)));
		}
	}
}

Evaluate_Coverage_Pattern::Test_Goal_States const& Automaton_Inserter::do_query(
		::goto_functionst & gf, ::fshell2::instrumentation::CFG & cfg, Query const& query) {
	m_tg_instrumentation_map.clear();
	m_target_edge_map.clear();
	m_loc_to_edge_target_graphs_map.clear();
	m_loc_to_node_target_graphs_map.clear();
	// compute trace automata for coverage and path patterns
	Evaluate_Coverage_Pattern::Test_Goal_States const& tgs(m_cp_eval.do_query(gf, cfg, query));

	::exprt t_g_final_cond, obs_final_cond;
	insert(gf, cfg, "t_g", m_cp_eval.get(), t_g_final_cond, true);
	insert(gf, cfg, "obs", m_pp_eval.get(query.get_passing()), obs_final_cond, false);
	
	::goto_programt tmp;
	::goto_programt::targett as(tmp.add_instruction(ASSERT));
	as->make_assertion(::and_exprt(t_g_final_cond, obs_final_cond));
	as->guard.make_not();
	
	::fshell2::instrumentation::GOTO_Transformation inserter(m_manager, gf);
	inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp);

	return tgs;
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

