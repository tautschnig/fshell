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

#include <cbmc/src/solvers/sat/satcheck_minisat2.h>
// #include <cbmc/src/goto-symex/build_goto_trace.h>
// #include <cbmc/src/solvers/sat/dimacs_cnf.h>

FSHELL2_NAMESPACE_BEGIN;

Constraint_Strengthening::Constraint_Strengthening(::fshell2::fql::CNF_Conversion & equation,
		::fshell2::statistics::Statistics & stats, ::optionst const& opts) :
	m_equation(equation), m_stats(stats), m_opts(opts)
{
}
	
static int find_sat_test_goals(::satcheck_minisatt const& minisat, ::std::list< ::bvt > & more_clauses,
		::fshell2::fql::test_goal_id_map_t const& goals, ::fshell2::fql::test_goal_ids_t & unsat_goals, ::bvt & fixed_literals,
		bool const has_internal_check, ::fshell2::fql::test_goal_ids_t & tgs,
		::fshell2::fql::test_goal_ids_t & tgs_done)
{
	// for (unsigned i(0); i != minisat.no_variables()-1; ++i)
	// 	::std::cerr << (i+1) << ": " << (minisat.l_get(::literalt(i+1,false)).is_false() ? "FALSE" : "TRUE") << ::std::endl;
	
	int cnt(0);

	for (::fshell2::fql::test_goal_ids_t::iterator iter(unsat_goals.begin());
			iter != unsat_goals.end(); ) {
		bool sat(true);
		for (::bvt::const_iterator v_iter(goals[*iter].begin());
				sat && v_iter != goals[*iter].end(); ++v_iter) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, minisat.l_get(*v_iter).is_known());
			sat = minisat.l_get(*v_iter).is_true();
		}
		if (!sat) {
			++iter;
			continue;
		}
		
		// test goal is done
		++cnt;
		// ::std::cerr << "Goal " << *iter << " sat" << ::std::endl;
		if (has_internal_check) {
			tgs.erase(*iter);
		}

		tgs_done.insert(*iter);
		more_clauses.push_back(goals[*iter]);
		for (::bvt::iterator v_iter(more_clauses.back().begin());
				v_iter != more_clauses.back().end(); ++v_iter)
			v_iter->invert();
		unsat_goals.erase(iter++);
	}

	FSHELL2_AUDIT_ASSERT_RELATION(::diagnostics::Violated_Invariance, 1, ==, cnt);
	return cnt;
}

void Constraint_Strengthening::generate(::fshell2::fql::Compute_Test_Goals const& ctg,
		test_cases_t & tcs, unsigned const limit)
{
	m_equation.status("Starting groupwise constraint strengthening");
	
	NEW_STAT(m_stats, CPU_Timer, sat_timer, "Time spent in main SAT solver");
	NEW_STAT(m_stats, CPU_Timer, sub_timer, "Time spent in subsumption analysis");

	::cnf_clause_list_assignmentt & cnf(m_equation.get_cnf());
	
	::fshell2::fql::test_goal_id_map_t const& goals(m_equation.get_test_goal_id_map());
	NEW_STAT(m_stats, Counter< ::fshell2::fql::test_goal_id_t >, feasible_cnt, "Possibly feasible test goals");
	feasible_cnt.inc(goals.size());
	::fshell2::fql::test_goal_ids_t unsat_goals;
	::fshell2::fql::test_goal_id_t id(0);
	for (::fshell2::fql::test_goal_id_map_t::const_iterator iter(goals.begin());
			iter != goals.end(); ++iter, ++id)
		unsat_goals.insert(unsat_goals.end(), id);
	
	/*
	::dimacs_cnft dimacs;
	cnf.copy_to(dimacs);
	dimacs.write_dimacs_cnf(::std::cerr);
	*/
	
	::satcheck_minisatt minisat;
	minisat.set_message_handler(cnf.get_message_handler());
	minisat.set_verbosity(cnf.get_verbosity());
	cnf.copy_to(minisat);
		
	bool const use_sat(m_opts.get_bool_option("sat-subsumption"));
	while (!unsat_goals.empty() && (limit == 0 || tcs.size() < limit)) {
		::std::ostringstream status;
		status << unsat_goals.size() << " test goals remain to be covered";
		m_equation.status(status.str());
		
		sat_timer.start_timer();
		if (::propt::P_UNSATISFIABLE == minisat.prop_solve()) break;
		sat_timer.stop_timer();
		
		/*
		cnf.copy_assignment_from(minisat);
		::goto_tracet trace;
		::build_goto_trace(m_equation.get_equation(), m_equation.get_bv(), trace);
		::show_goto_trace(::std::cerr, m_equation.get_ns(), trace);
		*/

		// keep all test cases
		tcs.push_back(::std::make_pair(::fshell2::fql::test_goal_ids_t(),
					::cnf_clause_list_assignmentt()));
		tcs.back().second.set_no_variables(cnf.no_variables());
		tcs.back().second.copy_assignment_from(minisat);

		// deactivate test goals, if implemented
		::fshell2::statistics::Statistics i_stats;
		NEW_STAT(i_stats, CPU_Timer, timer1, "Internal subsumption analysis");
		timer1.start_timer();
		::fshell2::fql::test_goal_ids_t test_goal_set;
		sub_timer.start_timer();
		bool const has_internal_check(ctg.get_satisfied_test_goals(tcs.back().second, test_goal_set));
		sub_timer.stop_timer();
		if (has_internal_check) {
			tcs.back().first.swap(test_goal_set);
			for (::fshell2::fql::test_goal_ids_t::const_iterator iter(
						tcs.back().first.begin()); iter != tcs.back().first.end(); ++iter) {
				::fshell2::fql::test_goal_ids_t::iterator tg(unsat_goals.find(*iter));
				if (unsat_goals.end() == tg) continue;
					
				test_goal_set.insert(*iter);
				// ::std::cerr << "Goal " << *tg << " sat" << ::std::endl;
				if (!use_sat) {
					::bvt cl(goals[*iter]);
					for (::bvt::iterator v_iter(cl.begin()); v_iter != cl.end(); ++v_iter)
						v_iter->invert();
					minisat.lcnf(cl);
					unsat_goals.erase(tg);
				}
			}
			status.str("");
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
		
		::std::list< ::bvt > more_cl;
		num_sat += find_sat_test_goals(minisat, more_cl, goals, unsat_goals,
				fixed_literals, has_internal_check, test_goal_set, tcs.back().first);
		while (!more_cl.empty()) {
			minisat.lcnf(more_cl.front());
			cnf.lcnf(more_cl.front());
			more_cl.pop_front();
		}
		
		::satcheck_minisatt tmp_minisat;
		tmp_minisat.set_message_handler(cnf.get_message_handler());
		tmp_minisat.set_verbosity(cnf.get_verbosity());
		cnf.copy_to(tmp_minisat);
		while (!unsat_goals.empty()) {
			tmp_minisat.set_assumptions(fixed_literals);
			if (::propt::P_UNSATISFIABLE == tmp_minisat.prop_solve()) break;
			
			num_sat += find_sat_test_goals(tmp_minisat, more_cl, goals, unsat_goals,
					fixed_literals, has_internal_check, test_goal_set, tcs.back().first);
			while (!more_cl.empty()) {
				minisat.lcnf(more_cl.front());
				tmp_minisat.lcnf(more_cl.front());
				cnf.lcnf(more_cl.front());
				more_cl.pop_front();
			}
		}

		timer2.stop_timer();
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, num_sat > 0);
		status.str("");
		status << "Satisfies " << num_sat << " test goals";
		m_equation.status(status.str());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, test_goal_set.empty());
		if (use_sat && has_internal_check) i_stats.print(m_equation);
	}
	if (!unsat_goals.empty()) sat_timer.stop_timer();
	
	NEW_STAT(m_stats, Counter< ::fshell2::fql::test_goal_id_t >, missing_cnt, "Test goals not fulfilled");
	missing_cnt.inc(unsat_goals.size());
	
	if (limit > 0 && tcs.size() == limit) {
		::std::ostringstream warn;
		warn << "Stopped after computing " << limit << " test cases as requested";
		m_equation.warning(warn.str());
	}
	
	if (!unsat_goals.empty()) {
		::std::ostringstream oss;
		oss << "Unsatisfied test goal ids:";
		for (::fshell2::fql::test_goal_ids_t::const_iterator iter(unsat_goals.begin());
				iter != unsat_goals.end(); ++iter)
			oss << " " << *iter;
		if (m_opts.get_bool_option("show-test-goals"))
			m_equation.print(0, oss.str());
		else
			m_equation.warning(oss.str());
	}
}
		
FSHELL2_NAMESPACE_END;

