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

/*! \file fshell2/tc_generation/constraint_strengthening.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun May  3 21:42:23 CEST 2009 
*/

#include <fshell2/tc_generation/constraint_strengthening.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <fshell2/util/statistics.hpp>
#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <cbmc/src/util/config.h>
#include <cbmc/src/solvers/sat/satcheck_minisat.h>
// #include <cbmc/src/goto-symex/build_goto_trace.h>
// #include <cbmc/src/solvers/sat/dimacs_cnf.h>

FSHELL2_NAMESPACE_BEGIN;

Constraint_Strengthening::Constraint_Strengthening(::fshell2::fql::CNF_Conversion & equation,
		::fshell2::statistics::Statistics & stats) :
	m_equation(equation), m_stats(stats)
{
}

void Constraint_Strengthening::generate(test_cases_t & tcs) {
	/*
	// find proper strategy
	::fshell2::fql::Strategy_Selection_Visitor strat;
	::fshell2::fql::Strategy_Selection_Visitor::strategy_t s(strat.select(*ast));
	*/

	::fshell2::fql::CNF_Conversion::test_goals_t const& goal_set(m_equation.get_test_goal_literals());
	::cnf_clause_list_assignmentt & cnf(m_equation.get_cnf());

	typedef ::std::map< ::literalt, ::literalt > aux_var_map_t;
	aux_var_map_t aux_var_map;

	::bvt goal_cl;
	for (::fshell2::fql::CNF_Conversion::test_goals_t::const_iterator iter(goal_set.begin());
			iter != goal_set.end(); ++iter) {
		::literalt s(cnf.new_variable());
		aux_var_map.insert(::std::make_pair(*iter, s));
		goal_cl.push_back(cnf.land(*iter, s));
	}
	cnf.lcnf(goal_cl);
	NEW_STAT(m_stats, Counter< aux_var_map_t::size_type >, feasible_cnt, "Possibly feasible test goals");
	feasible_cnt.inc(aux_var_map.size());

	/*
	::dimacs_cnft dimacs;
	cnf.copy_to(dimacs);
	dimacs.write_dimacs_cnf(::std::cerr);
	*/
	
	// MC/DC support:
	// check maximum cardinality required in path set predicates and duplicate
	// formula accordingly (k times)
	// add constraints over tuples of formula instances, if necessary, +proper
	// auxiliary variables
	// store mapping between variable names (k=3: 1..n  n+1..2n  2n+1..3n)

	::satcheck_minisatt minisat;
	minisat.set_message_handler(cnf.get_message_handler());
	minisat.set_verbosity(cnf.get_verbosity());
	
	cnf.copy_to(minisat);

	::bvt goals_done;
	bool max_tcs_reached(false);
	while (!aux_var_map.empty()) {
		if (::config.fshell.max_test_cases > 0 && tcs.size() == ::config.fshell.max_test_cases) {
			max_tcs_reached = true;
			break;
		}
		minisat.set_assumptions(goals_done);
		if (::propt::P_UNSATISFIABLE == minisat.prop_solve()) break;
		
		// solution has k paths!!!

		/*
		cnf.copy_assignment_from(minisat);
		::goto_tracet trace;
		::build_goto_trace(m_equation.get_equation(), m_equation.get_bv(), trace);
		::show_goto_trace(::std::cerr, m_equation.get_ns(), trace);
		*/

		// keep all test cases
		tcs.push_back(::cnf_clause_list_assignmentt());
		tcs.back().set_no_variables(cnf.no_variables());
		tcs.back().copy_assignment_from(minisat);

		// deactivate test goals
		/*::fshell2::fql::Compute_Test_Goals_From_Instrumentation::test_goals_t const& satisfied_tg(m_equation.get_satisfied_test_goals());
		::std::cerr << "NOT IMPLEMENTED" << ::std::endl;*/
		// unsigned const size1(aux_var_map.size());
		::bvt fixed_literals;
		::boolbvt const& bv(m_equation.get_bv());
		for (::boolbv_mapt::mappingt::const_iterator iter(bv.map.mapping.begin());
			iter != bv.map.mapping.end(); ++iter) {
			if (iter->first.as_string().substr(0, 12) == "c::$fshell2$") continue;
			for (::boolbv_mapt::literal_mapt::const_iterator m_iter(iter->second.literal_map.begin());
					m_iter != iter->second.literal_map.end(); ++m_iter)
				if (!m_iter->l.is_constant()) fixed_literals.push_back(::literalt(m_iter->l.var_no(), 
							m_iter->l.sign() ? !minisat.l_get(m_iter->l).is_false() : minisat.l_get(m_iter->l).is_false()));
		}
		fixed_literals.insert(fixed_literals.end(), goals_done.begin(), goals_done.end());
		for (aux_var_map_t::iterator iter(aux_var_map.begin());
				iter != aux_var_map.end();) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, minisat.l_get(iter->first).is_known());
			if (minisat.l_get(iter->first).is_false()) {
				++iter;
				continue;
			}
			// test goal is done
			// ::std::cerr << "Goal " << iter->first.var_no() << " sat" << ::std::endl;
			goals_done.push_back(::neg(iter->second));
			fixed_literals.push_back(::neg(iter->second));
			aux_var_map.erase(iter++);
		}
		::satcheck_minisatt tmp_minisat;
		tmp_minisat.set_message_handler(cnf.get_message_handler());
		tmp_minisat.set_verbosity(cnf.get_verbosity());
		cnf.copy_to(tmp_minisat);
		while (!aux_var_map.empty()) {
			tmp_minisat.set_assumptions(fixed_literals);
			if (::propt::P_UNSATISFIABLE == tmp_minisat.prop_solve()) break;
			
			for (aux_var_map_t::iterator iter(aux_var_map.begin());
					iter != aux_var_map.end();) {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tmp_minisat.l_get(iter->first).is_known());
				if (tmp_minisat.l_get(iter->first).is_false()) {
					++iter;
					continue;
				}
				// test goal is done
				// ::std::cerr << "Goal " << iter->first.var_no() << " sat" << ::std::endl;
				goals_done.push_back(::neg(iter->second));
				fixed_literals.push_back(::neg(iter->second));
				aux_var_map.erase(iter++);
			}
		}

		// ::std::cerr << "covers " << (size1 - aux_var_map.size()) << " test goals" << ::std::endl;
	}
	NEW_STAT(m_stats, Counter< test_cases_t::size_type >, tcs_cnt, "Test cases");
	tcs_cnt.inc(tcs.size());
	NEW_STAT(m_stats, Counter< aux_var_map_t::size_type >, missing_cnt, "Test goals not fulfilled");
	missing_cnt.inc(aux_var_map.size());
	
	if (max_tcs_reached)
		m_equation.warning(::diagnostics::internal::to_string("Stopped after ",
					::config.fshell.max_test_cases, " as requested"));
}

FSHELL2_NAMESPACE_END;

