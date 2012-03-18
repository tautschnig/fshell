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

/*! \file fshell2/tc_generation/test_suite_minimization.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Jul 31 23:18:32 CEST 2009 
*/

#include <fshell2/tc_generation/test_suite_minimization.hpp>
#include <fshell2/config/annotations.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#endif

#include <cbmc/src/util/namespace.h>
#include <cbmc/src/util/context.h>
#include <cbmc/src/solvers/flattening/bv_utils.h>
#include <cbmc/src/solvers/flattening/boolbv.h>
#include <cbmc/src/solvers/sat/cnf_clause_list.h>
#include <cbmc/src/solvers/sat/satcheck.h>

FSHELL2_NAMESPACE_BEGIN;
	
Test_Suite_Minimization::Test_Suite_Minimization(::messaget & msg) :
	m_message(msg)
{
}

void Test_Suite_Minimization::minimize(Constraint_Strengthening::test_cases_t & 
		test_suite)
{
	if (test_suite.size() <= 1) return;

	::cnf_clause_list_assignmentt cnf;
	cnf.set_message_handler(m_message.get_message_handler());
	cnf.set_verbosity(m_message.get_verbosity());
	::bv_utilst bv_utils(cnf);
	
	// one variable per test case
	// walk all sat test goals, add map entry OR-ing satisfying test cases
	// add fan-in adder over test cases
	::std::map< ::fshell2::fql::test_goal_id_t, ::literalt > set_cover;
	::std::map< ::literalt, Constraint_Strengthening::test_cases_t::iterator > tc_lit_map;
	::std::list< ::bvt > sum_operands;
	for (Constraint_Strengthening::test_cases_t::iterator iter(test_suite.begin());
			iter != test_suite.end(); ++iter) {
		::literalt const tc_var(cnf.new_variable());
		sum_operands.push_back(::bvt(1, tc_var));
		tc_lit_map.insert(::std::make_pair(tc_var, iter));
		for (::fshell2::fql::test_goal_ids_t::const_iterator tg_iter(iter->first.begin());
				tg_iter != iter->first.end(); ++tg_iter) {
			::std::pair< ::std::map< ::fshell2::fql::test_goal_id_t, ::literalt >::iterator, bool >
				entry(set_cover.insert(::std::make_pair(*tg_iter, tc_var)));
			if (!entry.second)
				entry.first->second = cnf.lor(entry.first->second, tc_var);
		}
	}
	// each test goal must be covered by at least one test case
	for (::std::map< ::fshell2::fql::test_goal_id_t, ::literalt >::const_iterator
			iter(set_cover.begin()); iter != set_cover.end(); ++iter)
		cnf.lcnf(::bvt(1, iter->second));

	// fan-in adder over test case variables
	while (sum_operands.size() > 1) {
		::std::list< ::bvt > next_sum_operands;
		unsigned const new_width(sum_operands.front().size() + 1);
		while (sum_operands.size() >= 2) {
			::bvt const& op0(sum_operands.front());
			::bvt const& op1(*(++sum_operands.begin()));
			next_sum_operands.push_back(bv_utils.add(
						bv_utils.zero_extension(op0, new_width),
						bv_utils.zero_extension(op1, new_width)));
			sum_operands.pop_front();
			sum_operands.pop_front();
		}
		if (!sum_operands.empty())
			next_sum_operands.push_back(bv_utils.zero_extension(sum_operands.front(), new_width));
		sum_operands.swap(next_sum_operands);
	}

	// add fresh variables to represent the upper bound
	::bvt upper_bound;
	upper_bound.reserve(sum_operands.front().size());
	for (::bvt::const_iterator iter(sum_operands.front().begin());
			iter != sum_operands.front().end(); ++iter)
		upper_bound.push_back(cnf.new_variable());
	// sum shall be less than upper bound
	cnf.lcnf(::bvt(1, bv_utils.unsigned_less_than(sum_operands.front(), upper_bound)));
	
	::satcheckt solver;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			solver.has_set_assumptions());
	
	solver.set_message_handler(m_message.get_message_handler());
	solver.set_verbosity(m_message.get_verbosity());
	cnf.copy_to(solver);
	boolbvt bv(::namespacet(::contextt()), solver);

	::mp_integer num_tcs(test_suite.size());
	while (num_tcs > 1) {
		// assume upper_bound is num_tcs
		::bvt bound_constant(bv_utils.build_constant(num_tcs, upper_bound.size()));
		::bvt::const_iterator c_iter(bound_constant.begin());
		for (::bvt::iterator iter(upper_bound.begin()); iter != upper_bound.end();
				++iter, ++c_iter)
			iter->cond_invert(iter->sign() != c_iter->is_false());

		solver.set_assumptions(upper_bound);
		if (::propt::P_UNSATISFIABLE == solver.prop_solve()) break;

		// backup assignment
		cnf.copy_assignment_from(solver);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				bv.get_value(sum_operands.front()) < num_tcs);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				bv.get_value(sum_operands.front()) > 0);
		num_tcs = bv.get_value(sum_operands.front());
	}

	if (num_tcs < test_suite.size())
		for (::std::map< ::literalt, Constraint_Strengthening::test_cases_t::iterator >::iterator iter(
					tc_lit_map.begin()); iter != tc_lit_map.end(); ++iter) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cnf.l_get(iter->first).is_known());
			if (cnf.l_get(iter->first).is_false())
				test_suite.erase(iter->second);
		}
}


FSHELL2_NAMESPACE_END;

