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

#ifndef FSHELL2__FQL__EVALUATION__COMPUTE_TEST_GOALS_HPP
#define FSHELL2__FQL__EVALUATION__COMPUTE_TEST_GOALS_HPP

/*! \file fshell2/fql/evaluation/compute_test_goals.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri May  1 21:45:36 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>

#include <cbmc/src/cbmc/bmc.h>
#include <cbmc/src/cbmc/bv_cbmc.h>
#include <cbmc/src/solvers/sat/cnf_clause_list.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Query;
class Evaluate_Filter;
class Automaton_Inserter;

/*! \brief TODO
*/
class Compute_Test_Goals : public ::bmct
{
	/*! \copydoc doc_self
	*/
	typedef Compute_Test_Goals Self;

	public:
	typedef ::literalt test_goal_t;
	typedef ::std::set< test_goal_t > test_goals_t;
	
	Compute_Test_Goals(::language_uit & manager, ::optionst const& opts,
			::goto_functionst const& gf, Evaluate_Filter const& filter_eval,
			Evaluate_Path_Monitor const& pm_eval,
			Automaton_Inserter const& a_i);

	virtual ~Compute_Test_Goals();

	test_goals_t const& compute(Query const& query);

	//test_goals_t const& get_satisfied_test_goals();

	inline ::cnf_clause_list_assignmentt & get_cnf();

	inline ::boolbvt const& get_bv() const;
	inline ::symex_target_equationt::SSA_stepst const& get_equation() const;

	inline ::namespacet const& get_ns() const;

	private:
	virtual bool decide_default();

	void initialize();
	
	bool m_is_initialized;
	::goto_functionst const& m_gf;
	Evaluate_Filter const& m_filter_eval;
	Evaluate_Path_Monitor const& m_pm_eval;
	Automaton_Inserter const& m_aut_insert;
	::cnf_clause_list_assignmentt m_cnf;
	::bv_cbmct m_bv;
	/*typedef ::std::map< ta_state_t, 
		::std::map< ::goto_programt::const_targett, test_goal_t > > state_context_tg_t;
	state_context_tg_t m_state_context_tg_map;*/
	typedef ::std::map< ::goto_programt::const_targett,
				::std::map< ::goto_programt::const_targett, 
					::std::set< ::literalt > > > pc_to_context_and_guards_t;
	pc_to_context_and_guards_t m_pc_to_guard;
	test_goals_t m_test_goals;
	//test_goals_t m_satisfied_goals;

	/*! \copydoc copy_constructor
	*/
	Compute_Test_Goals( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
inline ::cnf_clause_list_assignmentt & Compute_Test_Goals::get_cnf() {
	return m_cnf;
}
	
inline ::boolbvt const& Compute_Test_Goals::get_bv() const {
	return m_bv;
}
	
inline ::symex_target_equationt::SSA_stepst const& Compute_Test_Goals::get_equation() const {
	return _equation.SSA_steps;
}

inline ::namespacet const& Compute_Test_Goals::get_ns() const {
	return this->ns;
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__COMPUTE_TEST_GOALS_HPP */
