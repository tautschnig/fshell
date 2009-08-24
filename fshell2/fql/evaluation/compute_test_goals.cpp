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

/*! \file fshell2/fql/evaluation/compute_test_goals.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri May  1 21:45:40 CEST 2009 
*/

#include <fshell2/fql/evaluation/compute_test_goals.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/exception/fshell2_error.hpp>

#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>
#include <fshell2/fql/evaluation/automaton_inserter.hpp>

#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#include <algorithm>
#include <iterator>

#include <cbmc/src/util/i2string.h>
#include <cbmc/src/util/message_stream.h>
#include <cbmc/src/goto-symex/slice.h>
#include <cbmc/src/goto-symex/slice_by_trace.h>
#include <cbmc/src/cbmc/document_subgoals.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Compute_Test_Goals::Compute_Test_Goals(::language_uit & manager, ::optionst const& opts,
		::goto_functionst const& gf, Evaluate_Path_Monitor const& pm_eval,
		Automaton_Inserter const& a_i) :
	::bmct(manager.context, manager.ui_message_handler),
	m_is_initialized(false), m_gf(gf), m_pm_eval(pm_eval), m_aut_insert(a_i), m_cnf(),
	m_bv(m_cnf) {
	this->options = opts;
	this->options.set_option("dimacs", false);
	this->options.set_option("cvc", false);
	this->options.set_option("smt", false);
	this->options.set_option("refine", false);
	this->options.set_option("cvc", false);
	this->set_verbosity(manager.get_verbosity());

	m_cnf.set_message_handler(this->message_handler);
	m_cnf.set_verbosity(this->get_verbosity());
	m_bv.set_message_handler(this->message_handler);
	m_bv.set_verbosity(this->get_verbosity());
}

Compute_Test_Goals::~Compute_Test_Goals() {
}

bool Compute_Test_Goals::decide_default() {
  if(this->options.get_option("arrays-uf")=="never")
    m_bv.unbounded_array=bv_cbmct::U_NONE;
  else if(this->options.get_option("arrays-uf")=="always")
    m_bv.unbounded_array=bv_cbmct::U_ALL;

  status("Passing problem to "+m_bv.decision_procedure_text());

  this->do_unwind_module(m_bv);
  this->do_cbmc(m_bv);

  return false;
}

void Compute_Test_Goals::initialize() {
	if (m_is_initialized) return;

	// build the Boolean equation
	FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, !this->run(m_gf),
			"Failed to build Boolean program representation");
	// protected field, can't read here
	// FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, this->_symex.remaining_claims);

	// ::std::cerr << "Mapping program:" << ::std::endl;
	// ::std::cerr << this->_equation << ::std::endl;

	// build a map from GOTO instructions to guard literals by walking the
	// equation; group them by calling points
	::goto_programt::const_targett most_recent_caller;
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				_equation.SSA_steps.begin() ); iter != _equation.SSA_steps.end(); ++iter)
	{
		if (iter->source.pc->is_function_call()) most_recent_caller = iter->source.pc;
		/*::goto_programt tmp;
		tmp.output_instruction(this->ns, "", ::std::cerr, iter->source.pc);
		iter->output(this->ns, ::std::cerr);*/
		if (!iter->is_location()) continue;
		m_pc_to_guard[ iter->source.pc ][ most_recent_caller ].insert(iter->guard_literal);
	}
					
	m_is_initialized = true;
}
	
Compute_Test_Goals::value_t const& Compute_Test_Goals::compute(Query const& query) {
	initialize();

	query.accept(this);

	// a hack, to be reconsidered TODO
	tgs_value_t::const_iterator entry(m_tgs_map.find(query.get_cover()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, entry != m_tgs_map.end());
	return entry->second;
}


void Compute_Test_Goals::visit(Edgecov const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
/*

	if (n->get_predicates()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	}
	
	// for now, we completely ignore the abstraction!
	target_graph_t::edges_t const& filter_val(m_eval_filter.get(*(n->get_filter())).get_edges());
	::std::cerr << filter_val.size() << " edges must be covered" << ::std::endl;
	for (CFA::edges_t::const_iterator iter(filter_val.begin());
			iter != filter_val.end(); ++iter) {
		::std::pair< pc_to_bool_var_t::const_iterator, pc_to_bool_var_t::const_iterator >
			guards(m_pc_to_bool_var_and_guard.equal_range(iter->first.second));
		// FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, guards.first != guards.second);
		
		if (guards.first == guards.second) {
			::std::cerr << "WARNING: no guards for expr ";
			if (iter->first.second->is_goto()) {
				::std::cerr << "FILTER Instr: IF " << ::from_expr(this->ns, "", iter->first.second->guard);
			} else if (iter->first.second->is_assert()) {
				::std::cerr << "FILTER Instr: ASSERT " << ::from_expr(this->ns, "", iter->first.second->guard);
			} else {
				::std::cerr << "FILTER Instr: " << ::from_expr(this->ns, "", iter->first.second->code);
			}
			::std::cerr << " @" << iter->first.second->location << ::std::endl;
			continue;
		}

		// this is an edge in the CFG, therefore it is taken if
		// - the first statement is not a goto and its guard is true (test goal
		// is making this guard true)
		// - otherwise, iff it is a goto, the guard_literal of the goto must be
		// true if the second statement is the jump target (and not the skip
		// target)
		if (!iter->first.second->is_goto()) {
			::bvt set;
			for (; guards.first != guards.second; ++(guards.first)) {
				if (guards.first->second.first.is_false()) continue;
				// we will always take this edge, a trivial goal
				if (guards.first->second.first.is_true()) {
					set.clear();
					// set.push_back(m_cnf.new_variable());
					break;
				}
				// ::std::cerr << "Adding non-trivial guard " << guards.first->second.first.dimacs() << ::std::endl;
				set.push_back(guards.first->second.first);
			}
			if (!set.empty()) entry.first->second.insert(m_cnf.lor(set));
		} else {
			::bvt set;
			for (; guards.first != guards.second; ++(guards.first)) {
				if (guards.first->second.first.is_false()) continue;
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == iter->first.second->targets.size());
				bool is_goto_target(iter->first.second->targets.front() == iter->second.second);
				// ::std::cerr << "Real goto: " << (char const*) (is_goto_target?"yes":"no") << ::std::endl;
				if (guards.first->second.first.is_true() && (
							(is_goto_target && guards.first->second.second.is_true()) ||
							(!is_goto_target && guards.first->second.second.is_false()))) {
					// we will always take this edge, a trivial goal
					set.clear();
					// set.push_back(m_cnf.new_variable());
					break;
				}
				::literalt guard(is_goto_target? ::neg(guards.first->second.second) : guards.first->second.second);
				if (guards.first->second.first.is_true()) {
					// ::std::cerr << "Adding goto-guard " << guard.dimacs() << ::std::endl;
					set.push_back(guard);
				} else {
					set.push_back(m_cnf.land(guards.first->second.first, guard));
					// ::std::cerr << "Adding goto-guard " << set.back().dimacs() << ::std::endl;
				}
			}
			if (!set.empty()) entry.first->second.insert(m_cnf.lor(set));
		}
	}
*/
}

void Compute_Test_Goals::visit(Pathcov const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Compute_Test_Goals::visit(Predicate const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Compute_Test_Goals::visit(Query const* n) {
	n->get_cover()->accept(this);
}

void Compute_Test_Goals::visit(Statecov const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Compute_Test_Goals::visit(TGS_Intersection const* n) {
	// does not match what we wrote in the paper
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);

	tgs_value_t::const_iterator a_set(m_tgs_map.find(n->get_tgs_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_tgs_map.end());
	tgs_value_t::const_iterator b_set(m_tgs_map.find(n->get_tgs_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_tgs_map.end());
	::std::set_intersection(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Compute_Test_Goals::visit(TGS_Setminus const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);

	tgs_value_t::const_iterator a_set(m_tgs_map.find(n->get_tgs_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_tgs_map.end());
	tgs_value_t::const_iterator b_set(m_tgs_map.find(n->get_tgs_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_tgs_map.end());
	::std::set_difference(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Compute_Test_Goals::visit(TGS_Union const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);

	tgs_value_t::const_iterator a_set(m_tgs_map.find(n->get_tgs_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_tgs_map.end());
	tgs_value_t::const_iterator b_set(m_tgs_map.find(n->get_tgs_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_tgs_map.end());
	::std::set_union(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Compute_Test_Goals::visit(Test_Goal_Sequence const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {

		Automaton_Inserter::instrumentation_points_t subgoal_nodes;
		Evaluate_Path_Monitor::test_goal_states_t const& states(
				m_pm_eval.get_test_goal_states(*iter));
		for (Evaluate_Path_Monitor::test_goal_states_t::const_iterator s_iter(
					states.begin()); s_iter != states.end(); ++s_iter) {
			Automaton_Inserter::instrumentation_points_t const& nodes(
					m_aut_insert.get_test_goal_instrumentation(*s_iter));
			subgoal_nodes.insert(subgoal_nodes.end(), nodes.begin(), nodes.end());
		}

		for (Automaton_Inserter::instrumentation_points_t::const_iterator n_iter(
					subgoal_nodes.begin()); n_iter != subgoal_nodes.end(); ++n_iter) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n_iter->second->is_location());
			pc_to_context_and_guards_t::const_iterator guards_entry(m_pc_to_guard.find(n_iter->second));

			if (m_pc_to_guard.end() == guards_entry) {
				::std::cerr << "WARNING: no guards for expr "
					<< ::from_expr(this->ns, "", n_iter->second->code);
				::std::cerr << " @" << n_iter->second->location << ::std::endl;
				continue;
			}

			for (::std::map< ::goto_programt::const_targett, ::std::set< ::literalt > >::const_iterator
					c_iter(guards_entry->second.begin()); c_iter != guards_entry->second.end(); ++c_iter) {
				::bvt set;
				for (::std::set< ::literalt >::const_iterator l_iter(c_iter->second.begin());
						l_iter != c_iter->second.end(); ++l_iter) {
					// I think this does not even happen
					if (l_iter->is_false()) continue;
					// we will always take this edge, a trivial goal
					if (l_iter->is_true()) {
						set.clear();
						// just make sure we have a single entry in there to get at
						// least one test case
						set.push_back(*l_iter);
						break;
					}
					// ::std::cerr << "Adding non-trivial guard " << guards.first->second.first.dimacs() << ::std::endl;
					set.push_back(*l_iter);
				}
				if (!set.empty()) m_tgs_map[ iter->second ].insert(m_cnf.lor(set));
			}
		}
		
		//iter->second->accept(this);
		tgs_value_t::const_iterator subgoals(m_tgs_map.find(iter->second));
		// this will happen, if all subgoals are infeasible, so deal with it
		// properly!
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, subgoals != m_tgs_map.end());

		if (iter != n->get_sequence().begin()) {
			value_t const backup(entry.first->second);
			entry.first->second.clear();
			for (value_t::const_iterator tgi(backup.begin()); tgi != backup.end(); ++tgi)
				for (value_t::const_iterator sgi(subgoals->second.begin());
						sgi != subgoals->second.end(); ++sgi)
					entry.first->second.insert(m_cnf.land(*tgi, *sgi));
		} else {
			::std::copy(subgoals->second.begin(), subgoals->second.end(),
				::std::inserter(entry.first->second, entry.first->second.begin()));
		}
	}
	
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

