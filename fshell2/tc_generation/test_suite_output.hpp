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

#ifndef FSHELL2__TC_GENERATION__TEST_SUITE_OUTPUT_HPP
#define FSHELL2__TC_GENERATION__TEST_SUITE_OUTPUT_HPP

/*! \file fshell2/tc_generation/test_suite_output.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Jul 31 23:18:50 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <iostream>

#include <fshell2/tc_generation/constraint_strengthening.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Compute_Test_Goals;

FSHELL2_FQL_NAMESPACE_END;

/*! \brief TODO
*/
class Test_Suite_Output
{
	/*! \copydoc doc_self
	*/
	typedef Test_Suite_Output Self;

	public:
	Test_Suite_Output(::fshell2::fql::Compute_Test_Goals & goals);
	
	::std::ostream & print_ts(Constraint_Strengthening::test_cases_t &
			test_suite, ::std::ostream & os);

	private:
	::std::ostream & print_test_case(::std::ostream & os) const;
	
	::fshell2::fql::Compute_Test_Goals & m_goals;
	
	/*! \copydoc copy_constructor
	*/
	Test_Suite_Output( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__TC_GENERATION__TEST_SUITE_OUTPUT_HPP */