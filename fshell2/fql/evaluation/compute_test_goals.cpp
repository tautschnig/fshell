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
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>

#include <fshell2/exception/fshell2_error.hpp>

#include <fshell2/fql/evaluation/build_test_goal_automaton.hpp>
#include <fshell2/fql/evaluation/automaton_inserter.hpp>

#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>

#include <algorithm>
#include <iterator>

#include <cbmc/src/util/i2string.h>
#include <cbmc/src/util/message_stream.h>
#include <cbmc/src/goto-symex/slice.h>
#include <cbmc/src/goto-symex/slice_by_trace.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

CNF_Conversion::CNF_Conversion(::language_uit & manager, ::optionst const& opts) :
	::bmct(manager.context, manager.ui_message_handler),
	m_cnf(), m_bv(this->ns, m_cnf) {
	this->options = opts;
	this->options.set_option("dimacs", false);
	this->options.set_option("cvc", false);
	this->options.set_option("smt", false);
	this->options.set_option("refine", false);
	this->options.set_option("cvc", false);
	this->set_verbosity(manager.get_verbosity());
	this->set_ui(manager.ui_message_handler.get_ui());

	m_cnf.set_message_handler(*(this->message_handler));
	m_cnf.set_verbosity(this->get_verbosity());
	m_bv.set_message_handler(*(this->message_handler));
	m_bv.set_verbosity(this->get_verbosity());
}

CNF_Conversion::~CNF_Conversion() {
}
	
void CNF_Conversion::mark_as_test_goal(::literalt const& lit) {
	m_test_goals.insert(lit);
}

bool CNF_Conversion::decide_default() {
  if(this->options.get_option("arrays-uf")=="never")
    m_bv.unbounded_array=bv_cbmct::U_NONE;
  else if(this->options.get_option("arrays-uf")=="always")
    m_bv.unbounded_array=bv_cbmct::U_ALL;

  status("Passing problem to "+m_bv.decision_procedure_text());

  this->do_unwind_module(m_bv);
  this->do_cbmc(m_bv);

  return false;
}

void CNF_Conversion::convert(::goto_functionst const& gf) {
	// build the Boolean equation
	FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, !this->run(gf),
			"Failed to build Boolean program representation");
}
	

Compute_Test_Goals_From_Instrumentation::Compute_Test_Goals_From_Instrumentation(CNF_Conversion & equation,
		Build_Test_Goal_Automaton const& build_tg_aut, Automaton_Inserter const& a_i) :
	m_equation(equation), m_build_tg_aut(build_tg_aut), m_aut_insert(a_i) {
}

Compute_Test_Goals_From_Instrumentation::~Compute_Test_Goals_From_Instrumentation() {
}
	
void Compute_Test_Goals_From_Instrumentation::compute(Query const& query) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, !m_equation.get_equation().SSA_steps.empty());
	
	m_pc_to_guard.clear();
	// build a map from GOTO instructions to guard literals by walking the
	// equation; group them by calling points
	::goto_programt::const_targett most_recent_caller;
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				m_equation.get_equation().SSA_steps.begin() );
			iter != m_equation.get_equation().SSA_steps.end(); ++iter)
	{
		if (iter->source.pc->is_function_call()) most_recent_caller = iter->source.pc;
		/*::goto_programt tmp;
		tmp.output_instruction(this->ns, "", ::std::cerr, iter->source.pc);
		iter->output(this->ns, ::std::cerr);*/
		//if (!iter->is_location()) continue;
		if (!iter->is_assignment() || iter->assignment_type == ::symex_targett::HIDDEN) continue;
		m_pc_to_guard[ iter->source.pc ][ most_recent_caller ].insert(iter->guard_literal);
	}

	
	::std::set< ::literalt > test_goals;
	
	typedef ::std::map< ::goto_programt::const_targett,
				::std::set< ::goto_programt::const_targett > > context_to_pcs_t;

	for (Test_Goal_Sequence::seq_t::const_iterator iter(query.get_cover()->get_sequence().begin());
			iter != query.get_cover()->get_sequence().end(); ++iter) {

		context_to_pcs_t context_to_pcs;
		Build_Test_Goal_Automaton::test_goal_states_t const& states(
				m_build_tg_aut.get_test_goal_states(*iter));
		for (Build_Test_Goal_Automaton::test_goal_states_t::const_iterator s_iter(
					states.begin()); s_iter != states.end(); ++s_iter) {
			// ::std::cerr << "Test goal state: " << *s_iter << ::std::endl;
			Automaton_Inserter::instrumentation_points_t const& nodes(
					m_aut_insert.get_test_goal_instrumentation(*s_iter));
			// ::std::cerr << "#instrumentation points: " << nodes.size() << ::std::endl;
			for (Automaton_Inserter::instrumentation_points_t::const_iterator n_iter(
						nodes.begin()); n_iter != nodes.end(); ++n_iter) {
				//FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n_iter->second->is_location());
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n_iter->second->is_assign());
				pc_to_context_and_guards_t::const_iterator guards_entry(m_pc_to_guard.find(n_iter->second));
				if (m_pc_to_guard.end() == guards_entry) {
					m_equation.warning(::diagnostics::internal::to_string("transition to test goal state ",
								*s_iter, " in ", n_iter->second->function, " is unreachable"));
					continue;
				}

				// collect all possible function calls that may cause transitions to test goal state
				for (::std::map< ::goto_programt::const_targett, ::std::set< ::literalt > >::const_iterator
						c_iter(guards_entry->second.begin()); c_iter != guards_entry->second.end(); ++c_iter)
					context_to_pcs[ c_iter->first ].insert(n_iter->second);
			}
		}

		::std::set< ::literalt > subgoals;
		// forall calling contexts
		for (context_to_pcs_t::const_iterator c_iter(context_to_pcs.begin()); c_iter != context_to_pcs.end();
				++c_iter) {
			// ::std::cerr << "Next context." << ::std::endl;
			::bvt set;
			// forall transitions to test goal states in this context
			for (::std::set< ::goto_programt::const_targett >::const_iterator t_iter(c_iter->second.begin());
					t_iter != c_iter->second.end(); ++t_iter) {
				pc_to_context_and_guards_t::const_iterator guards_entry(m_pc_to_guard.find(*t_iter));
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_pc_to_guard.end() != guards_entry);
				::std::map< ::goto_programt::const_targett, ::std::set< ::literalt > >::const_iterator
					inner_guards_entry(guards_entry->second.find(c_iter->first));
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						guards_entry->second.end() != inner_guards_entry);

				// forall equation entries in the given context for this
				// transition
				for (::std::set< ::literalt >::const_iterator l_iter(
							inner_guards_entry->second.begin()); l_iter != inner_guards_entry->second.end();
						++l_iter) {
					// unreachable
					if (l_iter->is_false() || l_iter->var_no() == ::literalt::unused_var_no()) continue;
					// we will always take this edge, a trivial goal
					if (l_iter->is_true()) {
						set.clear();
						// just make sure we have a single entry in there to get at
						// least one test case
						// ::std::cerr << "Adding true guard " << l_iter->var_no() << ::std::endl;
						set.push_back(*l_iter);
						break;
					}
					// ::std::cerr << "Adding guard " << l_iter->var_no() << ::std::endl;
					set.push_back(*l_iter);
				}
			}
			if (!set.empty()) {
				// ::std::cerr << "Adding subgoal composed of " << set.size() << " guards" << ::std::endl;
				::literalt const tg(m_equation.get_cnf().lor(set));
				// ::std::cerr << "Subgoal " << tg.var_no() << " for ctx " << c_iter->first->location << ::std::endl;
				subgoals.insert(tg);
				/*for (Evaluate_Path_Monitor::test_goal_states_t::const_iterator s_iter(
							states.begin()); s_iter != states.end(); ++s_iter)
					m_state_context_tg_map[ *s_iter ].insert(::std::make_pair(c_iter->first, tg));
					*/
			}
		}
		
		if (iter != query.get_cover()->get_sequence().begin()) {
			::std::set< ::literalt > backup;
			backup.swap(test_goals);
			for (::std::set< ::literalt >::const_iterator tgi(backup.begin()); tgi != backup.end(); ++tgi)
				for (::std::set< ::literalt >::const_iterator sgi(subgoals.begin());
						sgi != subgoals.end(); ++sgi) {
					::literalt const sg(m_equation.get_cnf().land(*tgi, *sgi));
					// ::std::cerr << "Subgoal' " << sg.var_no() << " == " << tgi->var_no() << " AND " << sgi->var_no() << ::std::endl;
					test_goals.insert(sg);
				}
		} else {
			test_goals.swap(subgoals);
		}
	}

	for (::std::set< ::literalt >::const_iterator iter(test_goals.begin());
			iter != test_goals.end(); ++iter)
		m_equation.mark_as_test_goal(*iter);
}

#if 0
Compute_Test_Goals_From_Instrumentation::test_goals_t const& Compute_Test_Goals_From_Instrumentation::get_satisfied_test_goals() {
	m_satisfied_goals.clear();
	Evaluate_Path_Monitor::trace_automaton_t const& aut(m_pm_eval.get_cov_seq_aut());
	typedef Evaluate_Path_Monitor::trace_automaton_t::state_type state_t;
	::std::list< ::std::list< state_t > > exec_sequences;
	::std::list< ::goto_programt::const_targett > path;
	for (::std::set< state_t >::const_iterator iter(aut.initial().begin());
			iter != aut.initial().end(); ++iter) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 
				!m_pm_eval.is_test_goal_state(*iter));
		exec_sequences.push_back(::std::list< state_t >(1, *iter));
	}
	// walk the steps and compare all non-instrumented and non-hidden PCs to
	// the cov seq automaton; do forall current states, try all transitions
	// and record new state-set; all test-goal-states must be recorded and
	// mapped back to literals
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				_equation.SSA_steps.begin() ); iter != _equation.SSA_steps.end(); ++iter)
	{
		if (m_cnf.l_get(iter->guard_literal) != ::tvt(true)) continue;
		if (iter->is_assignment() && iter->assignment_type == ::symex_targett::HIDDEN) continue;
		if (::fshell2::instrumentation::GOTO_Transformation::is_instrumented(iter->source.pc)) continue;
		path.push_back(iter->source.pc);
		Evaluate_Filter::edge_to_filters_t const& edge_map(m_eval_filter.get(iter->source.pc));
		for (::std::list< ::std::list< state_t > >::iterator s_iter(exec_sequences.begin());
				s_iter != exec_sequences.end();) {
			Evaluate_Path_Monitor::trace_automaton_t::edges_type const out_edges(aut.delta2(s_iter->back()));
			bool succ_found(false);
			for (Evaluate_Path_Monitor::trace_automaton_t::edges_type::const_iterator o_iter(
						out_edges.begin()); o_iter != out_edges.end(); ++o_iter) {
				if (m_pm_eval.id_index() == o_iter->first) {
					if (succ_found) {
						exec_sequences.insert(s_iter, *s_iter);
					} else {
						succ_found = true;
					}
					s_iter->push_back(o_iter->second);
				} else {
					for (Evaluate_Filter::edge_to_filters_t::const_iterator e_f_iter(
								edge_map.begin()); e_f_iter != edge_map.end(); ++e_f_iter) {
						// must handle goto here
						for (::std::set< Filter const* >::const_iterator f_iter(e_f_iter->second.begin());
								f_iter != e_f_iter->second.end(); ++f_iter) {
							if (m_pm_eval.lookup_filter(*f_iter) == o_iter->first) {
								if (succ_found) {
									exec_sequences.insert(s_iter, *s_iter);
								} else {
									succ_found = true;
								}
								s_iter->push_back(o_iter->second);
							}
						}
					}
				}
			}
			if (succ_found) {
				++s_iter;
			} else {
				::std::list< ::std::list< state_t > >::iterator s_iter_bak(s_iter);
				++s_iter;
				exec_sequences.erase(s_iter_bak);
			}
		}
	}

	/*
	for (::std::list< ::std::list< state_t > >::const_iterator s_iter(exec_sequences.begin());
			s_iter != exec_sequences.end(); ++s_iter) {
		if (!aut.final(s_iter->back())) continue;
		::std::cerr << "TG reached!" << ::std::endl;
		::std::list< ::goto_programt::const_targett >::const_iterator p_iter(path.begin());
		::std::vector< ::literalt > goals;
		for (::std::list< state_t >::const_iterator iter(++(s_iter->begin())); iter != s_iter->end();
				++iter, ++p_iter) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, path.end() != p_iter);
			if (m_pm_eval.is_test_goal_state(*iter)) {
				state_context_tg_t::const_iterator tg_map_entry(m_state_context_tg_map.find(*iter));
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_state_context_tg_map.end() != tg_map_entry);
				FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, !tg_map_entry->second.empty(),
						::diagnostics::internal::to_string("No contexts for state ", *iter));
				::std::map< ::goto_programt::const_targett, test_goal_t >::const_iterator entry(tg_map_entry->second.end());
				for (::std::map< ::goto_programt::const_targett, test_goal_t >::const_iterator c_iter(
							tg_map_entry->second.begin()); c_iter != tg_map_entry->second.end(); ++c_iter) {
					CFA::edge_t const& edge(m_aut_insert.get_target_graph_edge(c_iter->first));
					// handle goto!!
					::goto_programt tmp;
					::std::cerr << "Comparing" << ::std::endl;
					tmp.output_instruction(this->ns, "", ::std::cerr, edge.first.second);
					::std::cerr << "and" << ::std::endl;
					tmp.output_instruction(this->ns, "", ::std::cerr, *p_iter);
					if (edge.first.second == *p_iter) {
						entry = c_iter;
						break;
					}
				}
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tg_map_entry->second.end() != entry);
				goals.push_back(entry->second);
			}
		}
		// somehow lookup goals
	}
	*/

	return m_satisfied_goals;
}
#endif
	

Compute_Test_Goals_Boolean::Compute_Test_Goals_Boolean(CNF_Conversion & equation,
		Build_Test_Goal_Automaton const& build_tg_aut) :
	m_equation(equation), m_build_tg_aut(build_tg_aut) {
}

Compute_Test_Goals_Boolean::~Compute_Test_Goals_Boolean() {
}
	
void Compute_Test_Goals_Boolean::compute(Query const& query) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol, !m_equation.get_equation().SSA_steps.empty());

#if 0
	m_pc_to_guard.clear();
	// build a map from GOTO instructions to guard literals by walking the
	// equation; group them by calling points
	::goto_programt::const_targett most_recent_caller;
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				m_equation.get_equation().SSA_steps.begin() );
			iter != m_equation.get_equation().SSA_steps.end(); ++iter)
	{
		if (iter->source.pc->is_function_call()) most_recent_caller = iter->source.pc;
		/*::goto_programt tmp;
		tmp.output_instruction(this->ns, "", ::std::cerr, iter->source.pc);
		iter->output(this->ns, ::std::cerr);*/
		//if (!iter->is_location()) continue;
		if (!iter->is_assignment() || iter->assignment_type == ::symex_targett::HIDDEN) continue;
		m_pc_to_guard[ iter->source.pc ][ most_recent_caller ].insert(iter->guard_literal);
	}

	
	::std::set< ::literalt > test_goals;
	
	typedef ::std::map< ::goto_programt::const_targett,
				::std::set< ::goto_programt::const_targett > > context_to_pcs_t;

	for (Test_Goal_Sequence::seq_t::const_iterator iter(query.get_cover()->get_sequence().begin());
			iter != query.get_cover()->get_sequence().end(); ++iter) {

		context_to_pcs_t context_to_pcs;
		Build_Test_Goal_Automaton::test_goal_states_t const& states(
				m_build_tg_aut.get_test_goal_states(*iter));
		for (Build_Test_Goal_Automaton::test_goal_states_t::const_iterator s_iter(
					states.begin()); s_iter != states.end(); ++s_iter) {
			// ::std::cerr << "Test goal state: " << *s_iter << ::std::endl;
			Automaton_Inserter::instrumentation_points_t const& nodes(
					m_aut_insert.get_test_goal_instrumentation(*s_iter));
			// ::std::cerr << "#instrumentation points: " << nodes.size() << ::std::endl;
			for (Automaton_Inserter::instrumentation_points_t::const_iterator n_iter(
						nodes.begin()); n_iter != nodes.end(); ++n_iter) {
				//FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n_iter->second->is_location());
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n_iter->second->is_assign());
				pc_to_context_and_guards_t::const_iterator guards_entry(m_pc_to_guard.find(n_iter->second));
				if (m_pc_to_guard.end() == guards_entry) {
					m_equation.warning(::diagnostics::internal::to_string("transition to test goal state ",
								*s_iter, " in ", n_iter->second->function, " is unreachable"));
					continue;
				}

				// collect all possible function calls that may cause transitions to test goal state
				for (::std::map< ::goto_programt::const_targett, ::std::set< ::literalt > >::const_iterator
						c_iter(guards_entry->second.begin()); c_iter != guards_entry->second.end(); ++c_iter)
					context_to_pcs[ c_iter->first ].insert(n_iter->second);
			}
		}

		::std::set< ::literalt > subgoals;
		// forall calling contexts
		for (context_to_pcs_t::const_iterator c_iter(context_to_pcs.begin()); c_iter != context_to_pcs.end();
				++c_iter) {
			// ::std::cerr << "Next context." << ::std::endl;
			::bvt set;
			// forall transitions to test goal states in this context
			for (::std::set< ::goto_programt::const_targett >::const_iterator t_iter(c_iter->second.begin());
					t_iter != c_iter->second.end(); ++t_iter) {
				pc_to_context_and_guards_t::const_iterator guards_entry(m_pc_to_guard.find(*t_iter));
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_pc_to_guard.end() != guards_entry);
				::std::map< ::goto_programt::const_targett, ::std::set< ::literalt > >::const_iterator
					inner_guards_entry(guards_entry->second.find(c_iter->first));
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						guards_entry->second.end() != inner_guards_entry);

				// forall equation entries in the given context for this
				// transition
				for (::std::set< ::literalt >::const_iterator l_iter(
							inner_guards_entry->second.begin()); l_iter != inner_guards_entry->second.end();
						++l_iter) {
					// unreachable
					if (l_iter->is_false() || l_iter->var_no() == ::literalt::unused_var_no()) continue;
					// we will always take this edge, a trivial goal
					if (l_iter->is_true()) {
						set.clear();
						// just make sure we have a single entry in there to get at
						// least one test case
						// ::std::cerr << "Adding true guard " << l_iter->var_no() << ::std::endl;
						set.push_back(*l_iter);
						break;
					}
					// ::std::cerr << "Adding guard " << l_iter->var_no() << ::std::endl;
					set.push_back(*l_iter);
				}
			}
			if (!set.empty()) {
				// ::std::cerr << "Adding subgoal composed of " << set.size() << " guards" << ::std::endl;
				::literalt const tg(m_equation.get_cnf().lor(set));
				// ::std::cerr << "Subgoal " << tg.var_no() << " for ctx " << c_iter->first->location << ::std::endl;
				subgoals.insert(tg);
				/*for (Evaluate_Path_Monitor::test_goal_states_t::const_iterator s_iter(
							states.begin()); s_iter != states.end(); ++s_iter)
					m_state_context_tg_map[ *s_iter ].insert(::std::make_pair(c_iter->first, tg));
					*/
			}
		}
		
		if (iter != query.get_cover()->get_sequence().begin()) {
			::std::set< ::literalt > backup;
			backup.swap(test_goals);
			for (::std::set< ::literalt >::const_iterator tgi(backup.begin()); tgi != backup.end(); ++tgi)
				for (::std::set< ::literalt >::const_iterator sgi(subgoals.begin());
						sgi != subgoals.end(); ++sgi) {
					::literalt const sg(m_equation.get_cnf().land(*tgi, *sgi));
					// ::std::cerr << "Subgoal' " << sg.var_no() << " == " << tgi->var_no() << " AND " << sgi->var_no() << ::std::endl;
					test_goals.insert(sg);
				}
		} else {
			test_goals.swap(subgoals);
		}
	}

	for (::std::set< ::literalt >::const_iterator iter(test_goals.begin());
			iter != test_goals.end(); ++iter)
		m_equation.mark_as_test_goal(*iter);
#endif
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

