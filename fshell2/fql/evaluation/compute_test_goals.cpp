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

#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>

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
	
Compute_Test_Goals::test_goals_t const& Compute_Test_Goals::compute(Query const& query) {
	initialize();
	m_test_goals.clear();

	for (Test_Goal_Sequence::seq_t::const_iterator iter(query.get_cover()->get_sequence().begin());
			iter != query.get_cover()->get_sequence().end(); ++iter) {

		Automaton_Inserter::instrumentation_points_t subgoal_nodes;
		Evaluate_Path_Monitor::test_goal_states_t const& states(
				m_pm_eval.get_test_goal_states(*iter));
		for (Evaluate_Path_Monitor::test_goal_states_t::const_iterator s_iter(
					states.begin()); s_iter != states.end(); ++s_iter) {
			Automaton_Inserter::instrumentation_points_t const& nodes(
					m_aut_insert.get_test_goal_instrumentation(*s_iter));
			subgoal_nodes.insert(subgoal_nodes.end(), nodes.begin(), nodes.end());
		}

		test_goals_t subgoals;
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
				if (!set.empty()) subgoals.insert(m_cnf.lor(set));
			}
		}
		
		if (iter != query.get_cover()->get_sequence().begin()) {
			test_goals_t backup;
			backup.swap(m_test_goals);
			for (test_goals_t::const_iterator tgi(backup.begin()); tgi != backup.end(); ++tgi)
				for (test_goals_t::const_iterator sgi(subgoals.begin());
						sgi != subgoals.end(); ++sgi)
					m_test_goals.insert(m_cnf.land(*tgi, *sgi));
		} else {
			m_test_goals.swap(subgoals);
		}
	}
	
	return m_test_goals;
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

