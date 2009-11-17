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

#include <list>

class cnf_clause_list_assignmentt;

FSHELL2_NAMESPACE_BEGIN;

FSHELL2_FQL_NAMESPACE_BEGIN;

class Query;
class Compute_Test_Goals_From_Instrumentation;

FSHELL2_FQL_NAMESPACE_END;

/*! \brief TODO
*/
class Constraint_Strengthening
{
	/*! \copydoc doc_self
	*/
	typedef Constraint_Strengthening Self;

	public:
	typedef ::std::list< ::cnf_clause_list_assignmentt > test_cases_t;
	
	explicit Constraint_Strengthening(::fshell2::fql::Compute_Test_Goals_From_Instrumentation & goals);

	void generate(::fshell2::fql::Query const& query, test_cases_t & tcs);

	private:
	::fshell2::fql::Compute_Test_Goals_From_Instrumentation & m_goals;

	/*! \copydoc copy_constructor
	*/
	Constraint_Strengthening( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__TC_GENERATION__CONSTRAINT_STRENGTHENING_HPP */
