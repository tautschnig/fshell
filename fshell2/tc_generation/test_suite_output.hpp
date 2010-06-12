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

#include <cbmc/src/util/ui_message.h>

class symbolt;
class exprt;
class locationt;

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Compute_Test_Goals_From_Instrumentation;

FSHELL2_FQL_NAMESPACE_END;

/*! \brief TODO
*/
class Test_Suite_Output
{
	/*! \copydoc doc_self
	*/
	typedef Test_Suite_Output Self;

	public:
	explicit Test_Suite_Output(::fshell2::fql::CNF_Conversion & equation);
	
	::std::ostream & print_ts(Constraint_Strengthening::test_cases_t &
			test_suite, ::std::ostream & os, ::ui_message_handlert::uit const ui);

	private:
	class Test_Input
	{
		public:
		typedef struct {
			::exprt const * m_name;
			::std::string m_pretty_name;
			::exprt const * m_value;
			::symbolt const * m_symbol;
			::locationt const * m_location;
			::std::string m_value_str;
			::std::string m_type_str;
		} program_variable_t;
		typedef ::std::list< program_variable_t > test_inputs_t;

		Test_Input(::symbolt const& main_sym, 
				::std::string const& main_str, ::locationt const& main_loc);

		::symbolt const& m_main_symbol;
		::std::string const m_main_symbol_str;
		::locationt const& m_main_location;
		test_inputs_t m_test_inputs;
	};

	void get_test_case(Test_Input & ti) const;
	::std::ostream & print_test_case_plain(::std::ostream & os, Test_Input const& ti) const;
	::std::ostream & print_test_case_xml(::std::ostream & os, Test_Input const& ti) const;
	
	::fshell2::fql::CNF_Conversion & m_equation;
	
	/*! \copydoc copy_constructor
	*/
	Test_Suite_Output( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__TC_GENERATION__TEST_SUITE_OUTPUT_HPP */
