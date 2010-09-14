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

#ifndef FSHELL2__TC_GENERATION__CONSTRAINT_STRENGTHENING_HPP
#define FSHELL2__TC_GENERATION__CONSTRAINT_STRENGTHENING_HPP

/*! \file fshell2/tc_generation/constraint_strengthening.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun May  3 21:42:20 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/concepts/test_goal.hpp>

#include <list>

class cnf_clause_list_assignmentt;
class optionst;

FSHELL2_NAMESPACE_BEGIN;

FSHELL2_STATISTICS_NAMESPACE_BEGIN;

class Statistics;

FSHELL2_STATISTICS_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

class CNF_Conversion;
class Compute_Test_Goals;

FSHELL2_FQL_NAMESPACE_END;

/*! \brief TODO
*/
class Constraint_Strengthening
{
	/*! \copydoc doc_self
	*/
	typedef Constraint_Strengthening Self;

	public:
	typedef ::std::list< ::std::pair< ::fshell2::fql::test_goal_ids_t,
				::cnf_clause_list_assignmentt > > test_cases_t;
	
	Constraint_Strengthening(::fshell2::fql::CNF_Conversion & equation,
			::fshell2::statistics::Statistics & stats, ::optionst const& opts);

	void generate(::fshell2::fql::Compute_Test_Goals const& ctg, test_cases_t & tcs,
			unsigned const limit);

	private:
	::fshell2::fql::CNF_Conversion & m_equation;
	::fshell2::statistics::Statistics & m_stats;
	::optionst const& m_opts;

	/*! \copydoc copy_constructor
	*/
	Constraint_Strengthening( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__TC_GENERATION__CONSTRAINT_STRENGTHENING_HPP */
