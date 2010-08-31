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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#endif

#include <fshell2/util/statistics.hpp>
#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <cbmc/src/util/config.h>
#include <cbmc/src/solvers/sat/satcheck_minisat2.h>
// #include <cbmc/src/goto-symex/build_goto_trace.h>
// #include <cbmc/src/solvers/sat/dimacs_cnf.h>

FSHELL2_NAMESPACE_BEGIN;

Constraint_Strengthening::Constraint_Strengthening(::fshell2::fql::CNF_Conversion & equation,
		::fshell2::statistics::Statistics & stats, ::optionst const& opts) :
	m_equation(equation), m_stats(stats), m_opts(opts)
{
}
	
typedef ::std::map< ::literalt, ::literalt > aux_var_map_t;

static int find_sat_test_goals(::satcheck_minisatt const& minisat,
		aux_var_map_t & aux_var_map, ::bvt & goals_done, ::bvt & fixed_literals,
		bool const has_internal_check, ::fshell2::fql::CNF_Conversion::test_goals_t & tgs)
{
	int cnt(0);

	for (aux_var_map_t::iterator iter(aux_var_map.begin());
			iter != aux_var_map.end();) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, minisat.l_get(iter->first).is_known());
		if (minisat.l_get(iter->first).is_false()) {
			++iter;
			continue;
		}
		// test goal is done
		++cnt;
		// ::std::cerr << "Goal " << iter->first.dimacs() << " sat" << ::std::endl;
		if (has_internal_check) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
					tgs.end() != tgs.find(iter->first));
			tgs.erase(iter->first);
		}

		goals_done.push_back(::neg(iter->second));
		fixed_literals.push_back(::neg(iter->second));
		aux_var_map.erase(iter++);
	}

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == cnt);
	return cnt;
}


void Constraint_Strengthening::generate(::fshell2::fql::Compute_Test_Goals const& ctg,
		test_cases_t & tcs)
{
	/*
	// find proper strategy
	::fshell2::fql::Strategy_Selection_Visitor strat;
	::fshell2::fql::Strategy_Selection_Visitor::strategy_t s(strat.select(*ast));
	*/

	m_equation.status("Starting incremental constraint strengthening");

	::fshell2::fql::CNF_Conversion::test_goals_t const& goal_set(m_equation.get_test_goal_literals());
	::cnf_clause_list_assignmentt & cnf(m_equation.get_cnf());

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
	
	::satcheck_minisatt minisat;
	minisat.set_message_handler(cnf.get_message_handler());
	minisat.set_verbosity(cnf.get_verbosity());
	cnf.copy_to(minisat);

	::bvt goals_done;
	bool max_tcs_reached(false);
	bool const use_sat(m_opts.get_bool_option("sat-subsumption"));
	while (!aux_var_map.empty()) {
		if (::config.fshell.max_test_cases > 0 && tcs.size() == ::config.fshell.max_test_cases) {
			max_tcs_reached = true;
			break;
		}
		
		minisat.set_assumptions(goals_done);
		if (::propt::P_UNSATISFIABLE == minisat.prop_solve()) break;
		
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
		/*
		for (unsigned i(0); i != cnf.no_variables()-1; ++i)
			::std::cerr << (i+1) << ": " << (minisat.l_get(::literalt(i+1,false)).is_false() ? "FALSE" : "TRUE") << ::std::endl;
		*/

		// deactivate test goals, if implemented
		::fshell2::statistics::Statistics i_stats;
		NEW_STAT(i_stats, CPU_Timer, timer1, "Internal subsumption analysis");
		timer1.start_timer();
		::fshell2::fql::CNF_Conversion::test_goals_t test_goal_set;
		bool const has_internal_check(ctg.get_satisfied_test_goals(tcs.back(), test_goal_set));
		if (has_internal_check) {
			::fshell2::fql::CNF_Conversion::test_goals_t test_goal_set_bak;
			test_goal_set_bak.swap(test_goal_set);
			for (::fshell2::fql::CNF_Conversion::test_goals_t::const_iterator iter(
						test_goal_set_bak.begin()); iter != test_goal_set_bak.end(); ++iter) {
				aux_var_map_t::iterator tg(aux_var_map.find(*iter));
				if (aux_var_map.end() == tg) continue;
					
				test_goal_set.insert(*iter);
				// ::std::cerr << "Goal " << tg->first.dimacs() << " sat" << ::std::endl;
				if (!use_sat) {
					goals_done.push_back(::neg(tg->second));
					aux_var_map.erase(tg);
				}
			}
			::std::ostringstream status;
			status << "Satisfies " << test_goal_set.size() << " additional test goals";
			m_equation.status(status.str());
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !test_goal_set.empty());
		}
		timer1.stop_timer();
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, has_internal_check || use_sat);
		if (!use_sat) continue;
		
		NEW_STAT(i_stats, CPU_Timer, timer2, "SAT subsumption analysis");
		timer2.start_timer();
		int num_sat(0);
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
		
		num_sat += find_sat_test_goals(minisat, aux_var_map, goals_done,
				fixed_literals, has_internal_check, test_goal_set);
		
		::satcheck_minisatt tmp_minisat;
		tmp_minisat.set_message_handler(cnf.get_message_handler());
		tmp_minisat.set_verbosity(cnf.get_verbosity());
		cnf.copy_to(tmp_minisat);
		while (!aux_var_map.empty()) {
			tmp_minisat.set_assumptions(fixed_literals);
			if (::propt::P_UNSATISFIABLE == tmp_minisat.prop_solve()) break;
			
			num_sat += find_sat_test_goals(tmp_minisat, aux_var_map, goals_done,
					fixed_literals, has_internal_check, test_goal_set);
		}

		timer2.stop_timer();
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, num_sat > 0);
		::std::ostringstream status;
		status << "Satisfies " << num_sat << " test goals";
		m_equation.status(status.str());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, test_goal_set.empty());
		if (use_sat && has_internal_check) i_stats.print(m_equation);
	}
	
	NEW_STAT(m_stats, Counter< test_cases_t::size_type >, tcs_cnt, "Test cases");
	tcs_cnt.inc(tcs.size());
	NEW_STAT(m_stats, Counter< aux_var_map_t::size_type >, missing_cnt, "Test goals not fulfilled");
	missing_cnt.inc(aux_var_map.size());
	
	if (max_tcs_reached) {
		::std::ostringstream warn;
		warn << "Stopped after computing " << ::config.fshell.max_test_cases
			<< " test cases as requested";
		m_equation.warning(warn.str());
	}
	
	if (!aux_var_map.empty()) {
		::std::ostringstream oss;
		oss << "Unsatisfied test goal ids:";
		for (aux_var_map_t::const_iterator iter(aux_var_map.begin());
				iter != aux_var_map.end(); ++iter)
			oss << " " << iter->first.dimacs();
		if (m_opts.get_bool_option("show-test-goals"))
			m_equation.print(0, oss.str());
		else
			m_equation.warning(oss.str());
	}
}
		
FSHELL2_NAMESPACE_END;

