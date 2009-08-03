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

#ifndef FSHELL2__TC_GENERATION__TEST_SUITE_MINIMIZATION_HPP
#define FSHELL2__TC_GENERATION__TEST_SUITE_MINIMIZATION_HPP

/*! \file fshell2/tc_generation/test_suite_minimization.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Jul 31 23:18:36 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/tc_generation/constraint_strengthening.hpp>

FSHELL2_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Test_Suite_Minimization
{
	/*! \copydoc doc_self
	*/
	typedef Test_Suite_Minimization Self;

	public:
	Test_Suite_Minimization();

	void minimize(Constraint_Strengthening::test_cases_t & test_suite) const;

	private:
	/*! \copydoc copy_constructor
	*/
	Test_Suite_Minimization( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__TC_GENERATION__TEST_SUITE_MINIMIZATION_HPP */
