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
#include <string>

#include <fshell2/tc_generation/constraint_strengthening.hpp>

#include <util/ui_message.h>
#include <goto-programs/goto_program.h>
#include <goto-symex/symex_target_equation.h>

class optionst;
class symbol_exprt;

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
	Test_Suite_Output(::fshell2::fql::CNF_Conversion & equation, ::optionst const& opts);
	
	::std::ostream & print_ts(Constraint_Strengthening::test_cases_t & test_suite,
			unsigned const index, ::std::ostream & os, ::ui_message_handlert::uit const ui);

	private:
	class Test_Input
	{
		public:
		typedef struct {
			::exprt const * m_name;
			::irep_idt m_pretty_name;
			::exprt const * m_value;
			::symbolt const * m_symbol;
			::source_locationt const * m_location;
			::exprt m_value_expr;
			::std::string m_type_str;
		} program_variable_t;
		typedef ::std::list< program_variable_t > test_inputs_t;

		Test_Input(::symbolt const& main_sym, 
				::std::string const& main_str, ::source_locationt const& main_loc);

		::symbolt const& m_main_symbol;
		::std::string const m_main_symbol_str;
		::source_locationt const& m_main_location;
		test_inputs_t m_test_inputs;
	};
	typedef ::std::list< ::std::pair< ::std::pair< ::goto_programt::const_targett,
			::irep_idt const >, ::exprt const * > > called_functions_t;
	typedef ::std::list< called_functions_t::iterator > call_stack_t;
	typedef ::std::list< ::symex_target_equationt::SSA_stept const* > assignments_t;
	typedef ::std::map< ::irep_idt, std::set< ::irep_idt > > seen_vars_t;
	typedef ::std::map< ::exprt const*, ::exprt const* > stmt_vars_and_parents_t;
	typedef ::std::list< stmt_vars_and_parents_t::const_iterator > nondet_syms_in_rhs_t;

	void get_test_case(Test_Input & ti, called_functions_t & calls,
			assignments_t & global_assign) const;
	void analyze_symbols(
	  ::symex_target_equationt::SSA_stept const& step,
	  seen_vars_t & vars,
	  Test_Input & ti,
	  assignments_t & global_assign) const;
	void analyze_symbol(
	  ::symex_target_equationt::SSA_stept const& step,
	  ::symbol_exprt const& sym,
	  ::exprt const* parent,
	  nondet_syms_in_rhs_t const& nondet_syms_in_rhs,
	  seen_vars_t & vars,
	  Test_Input & ti,
	  assignments_t & global_assign) const;
	void beautify_argv(
	  exprt &val,
	  irep_idt const& identifier,
	  seen_vars_t const& vars) const;
	void update_call_stack(
	  ::symex_target_equationt::SSA_stept const& step,
	  called_functions_t &calls,
	  call_stack_t &call_stack) const;
	void do_step(
	  ::symex_target_equationt::SSA_stept const& step,
	  seen_vars_t &vars,
	  called_functions_t &calls,
	  call_stack_t &call_stack,
	  bool &most_recent_non_hidden_is_true,
	  Test_Input & ti,
	  assignments_t & global_assign) const;

	::std::ostream & print_test_inputs_plain(::std::ostream & os, Test_Input const& ti) const;
	::std::ostream & print_function_calls(::std::ostream & os, called_functions_t const& cf) const;
	::std::ostream & print_assignments_to_globals(::std::ostream & os, assignments_t const& as) const;
	
	::std::ostream & print_test_case_xml(::std::ostream & os, Test_Input const& ti,
			called_functions_t const& cf, assignments_t const& as) const;
	
	::fshell2::fql::CNF_Conversion & m_equation;
	::optionst const& m_opts;
	
	/*! \copydoc copy_constructor
	*/
	Test_Suite_Output( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

/*! \brief TODO
*/
class Symbol_Identifier
{
	public:
		typedef enum {
			CBMC_INTERNAL,
			CBMC_GUARD,
			CBMC_DYNAMIC_MEMORY,
			CBMC_TMP_RETURN_VALUE,
			FSHELL2_INTERNAL,
			LOCAL,
			LOCAL_STATIC,
			GLOBAL,
			PARAMETER,
			UNKNOWN
		} variable_type_t;

		explicit Symbol_Identifier(::symbol_exprt const& sym);

		void strip_SSA_renaming();

		::irep_idt const& m_identifier;
		variable_type_t m_vt;
		::std::string m_var_name;
		::std::string m_level0, m_level1, m_level2;
		int m_failed_object_level;
};

::std::ostream & operator<<(::std::ostream & os, Symbol_Identifier::variable_type_t const& vt);

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__TC_GENERATION__TEST_SUITE_OUTPUT_HPP */
