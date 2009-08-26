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

#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <cbmc/src/util/config.h>
#include <cbmc/src/solvers/sat/satcheck_minisat.h>

FSHELL2_NAMESPACE_BEGIN;

Constraint_Strengthening::Constraint_Strengthening(::fshell2::fql::Compute_Test_Goals & goals) :
	m_goals(goals) {
}

void Constraint_Strengthening::generate(::fshell2::fql::Query const& query,
		test_cases_t & tcs) {
	/*
	// find proper strategy
	::fshell2::fql::Strategy_Selection_Visitor strat;
	::fshell2::fql::Strategy_Selection_Visitor::strategy_t s(strat.select(*ast));
	*/

	::fshell2::fql::Compute_Test_Goals::test_goals_t const& goal_set(m_goals.compute(query));
	::cnf_clause_list_assignmentt & cnf(m_goals.get_cnf());

	::std::map< ::literalt, ::literalt > aux_var_map;

	::bvt goal_cl;
	for (::fshell2::fql::Compute_Test_Goals::test_goals_t::const_iterator iter(goal_set.begin());
			iter != goal_set.end(); ++iter) {
		::literalt s(cnf.new_variable());
		aux_var_map.insert(::std::make_pair(*iter, s));
		goal_cl.push_back(cnf.land(*iter, s));
	}
	cnf.lcnf(goal_cl);
	
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

	::std::cerr << "#Possibly feasible test goals: " << aux_var_map.size() << ::std::endl;
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

		// store the Boolean variable values
		//cnf.copy_assignment_from(minisat);

		// keep all test cases
		tcs.push_back(::cnf_clause_list_assignmentt());
		tcs.back().set_no_variables(cnf.no_variables());
		tcs.back().copy_assignment_from(minisat);

		// deactivate test goals
		/*::fshell2::fql::Compute_Test_Goals::test_goals_t const& satisfied_tg(m_goals.get_satisfied_test_goals());
		::std::cerr << "NOT IMPLEMENTED" << ::std::endl;*/
		unsigned const size1(aux_var_map.size());
		::bvt fixed_literals;
		::boolbvt const& bv(m_goals.get_bv());
		for (::boolbv_mapt::mappingt::const_iterator iter(bv.map.mapping.begin());
			iter != bv.map.mapping.end(); ++iter) {
			if (iter->first.as_string().substr(0, 12) == "c::!fshell2!") continue;
			for (::boolbv_mapt::literal_mapt::const_iterator m_iter(iter->second.literal_map.begin());
					m_iter != iter->second.literal_map.end(); ++m_iter)
				if (!m_iter->l.is_constant()) fixed_literals.push_back(::literalt(m_iter->l.var_no(), 
							m_iter->l.sign() ? !minisat.l_get(m_iter->l).is_false() : minisat.l_get(m_iter->l).is_false()));
		}
		fixed_literals.insert(fixed_literals.end(), goals_done.begin(), goals_done.end());
		for (::std::map< ::literalt, ::literalt >::iterator iter(aux_var_map.begin());
				iter != aux_var_map.end();) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, minisat.l_get(iter->first).is_known());
			if (minisat.l_get(iter->first).is_false()) {
				++iter;
				continue;
			}
			// test goal is done
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
			
			for (::std::map< ::literalt, ::literalt >::iterator iter(aux_var_map.begin());
					iter != aux_var_map.end();) {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tmp_minisat.l_get(iter->first).is_known());
				if (tmp_minisat.l_get(iter->first).is_false()) {
					++iter;
					continue;
				}
				// test goal is done
				goals_done.push_back(::neg(iter->second));
				fixed_literals.push_back(::neg(iter->second));
				aux_var_map.erase(iter++);
			}
		}

		::std::cerr << "covers " << (size1 - aux_var_map.size()) << " test goals" << ::std::endl;
	}
	::std::cerr << "#Test cases: " << tcs.size() << ::std::endl;
	if (max_tcs_reached) {
		::std::cerr << "Stopped as requested" << ::std::endl;
	} else {
		::std::cerr << "#Infeasible test goals: " << aux_var_map.size() << ::std::endl;
	}
}

FSHELL2_NAMESPACE_END;

