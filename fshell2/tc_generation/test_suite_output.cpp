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

/*! \file fshell2/tc_generation/test_suite_output.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Jul 31 23:18:53 CEST 2009 
*/

#include <fshell2/tc_generation/test_suite_output.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/invalid_argument.hpp>

#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <cbmc/src/ansi-c/expr2c.h>
#include <cbmc/src/util/config.h>

FSHELL2_NAMESPACE_BEGIN;

typedef enum {
	TEMPORARY,
	ARGC_ARGV_CBMC_MAIN,
	GLOBAL_STDIO,
	LOCAL_INIT,
	LOCAL,
	GLOBAL_INIT,
	GLOBAL,
	PARAMETER_INIT,
	PARAMETER,
	UNKNOWN
} variable_type_t;

::std::ostream & operator<<(::std::ostream & os, variable_type_t const& vt) {
	switch (vt) {
		case TEMPORARY:
			os << "TEMPORARY";
			break;
		case ARGC_ARGV_CBMC_MAIN:
			os << "ARGC_ARGV_CBMC_MAIN";
			break;
		case GLOBAL_STDIO:
			os << "GLOBAL_STDIO";
			break;
		case LOCAL_INIT:
			os << "LOCAL_INIT";
			break;
		case LOCAL:
			os << "LOCAL";
			break;
		case GLOBAL_INIT:
			os << "GLOBAL_INIT";
			break;
		case GLOBAL:
			os << "GLOBAL";
			break;
		case PARAMETER_INIT:
			os << "PARAMETER_INIT";
			break;
		case PARAMETER:
			os << "PARAMETER";
			break;
		case UNKNOWN:
			os << "UNKNOWN";
			break;
	}
	return os;
}

variable_type_t get_variable_type(::std::string const& v)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, v.size() > 2 );
	// first check for temporaries introduced by CBMC
	if( ::std::string::npos != v.find( "$tmp" ) ) return TEMPORARY;
	// argc' and argv' used in the CBMC main routine
	else if( ::std::string::npos != v.find( '\'' ) ) return ARGC_ARGV_CBMC_MAIN;
	// global variables only have c:: and end in #1
	if( ::std::string::npos == v.find( "::", 3 ) )
	{
		if( 0 == v.find( "c::stderr#" ) || 0 == v.find( "c::stdin#" ) 
			|| 0 == v.find( "c::stdout#" ) ) return GLOBAL_STDIO;
		else if( '#' == v[ v.size() - 2 ] && '1' == v[ v.size() - 1 ] ) return GLOBAL_INIT;
		else return GLOBAL;
	}
	// parameters have two more ::
	else if( ::std::string::npos == v.find( "::", v.find( "::", v.find( "::", 3 ) + 2 ) + 2 ) )
		if( '#' == v[ v.size() - 2 ] && '1' == v[ v.size() - 1 ] ) return PARAMETER_INIT;
		else return PARAMETER;
	// must be a local variable
	else
	{
		// must have a frame, otherwise I have no idea...
		if( ::std::string::npos == v.find( '@' ) ) return UNKNOWN;
		else if( '#' == v[ v.size() - 2 ] && '0' == v[ v.size() - 1 ] ) return LOCAL_INIT;
		else return LOCAL;
	}
}

::std::ostream & Test_Suite_Output::print_test_case(::std::ostream & os) const {
	// select the init procedure chosen by the user
	::std::string start_proc_name( "::" );
	start_proc_name += config.main;
	start_proc_name += "::";
	// beautify pointers
	::std::map< ::std::string, ::std::string > beautified_name;
	
	::boolbvt const& bv(m_goals.get_bv());

	for (::boolbv_mapt::mappingt::const_iterator iter(bv.map.mapping.begin());
			iter != bv.map.mapping.end(); ++iter) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, iter->first.size() > 2);
		// keep the name for further manipulation
		::std::string var_name( iter->first.c_str() );
		// only consider the initial values
		variable_type_t const vt( get_variable_type( var_name ) );
		if( vt != GLOBAL_INIT && vt != PARAMETER_INIT && vt != LOCAL_INIT ) continue; 
		// discard input parameters that don't belong to the user-defined init proc
		if( PARAMETER_INIT == vt && ::std::string::npos == var_name.find( start_proc_name ) ) continue;
		// ::std::cerr << "Examining " << iter->first << ::std::endl;
		// cut off any #...
		var_name = var_name.substr( 0, var_name.rfind( '#' ) );
		// split at an @..., need not exist
		::std::string::size_type const suf_start( var_name.rfind( '@' ) );
		::std::string suffix;
		if( ::std::string::npos != suf_start ) suffix = var_name.substr( suf_start );
		var_name = var_name.substr( 0, suf_start );
		// try to obtain the value
		::exprt tmp("symbol");
		tmp.set("identifier", iter->first);
		::std::string val(::expr2c(bv.get(tmp), m_goals.get_ns()));
		// ignore empty valuations
		if( val.empty() ) continue;
		// beautify dynamic_X_name
		if( 0 == val.find( "&dynamic_" ) )
		{
			::std::map< ::std::string, ::std::string >::const_iterator entry(
					beautified_name.find( val ) );
			if( beautified_name.end() == entry )
			{
				::std::string new_name( ::diagnostics::internal::to_string( "LOC",
							beautified_name.size() + 1 ) );
				beautified_name[ val ] = new_name;
				val.swap( new_name );
			}
			else val = entry->second;
		}
		// ok, we're still there, try to get a nicer name from the namespace
		const symbolt *symbol( 0 );
		if( !m_goals.get_ns().lookup( var_name, symbol ) )
		{
			// for parameters, take the base_name, for others take the
			// pretty_name
			if( PARAMETER_INIT == vt && symbol->base_name != "" )
				var_name = symbol->base_name.as_string();
			else if( symbol->pretty_name != "" )
				var_name = symbol->pretty_name.as_string();
		}
		// in case the lookup fails the variable can't be part of the input
		// program (but rather must have been auto-generated)
		FSHELL2_AUDIT_ASSERT( ::diagnostics::Violated_Invariance, symbol );
		// don't store data twice, append the suffix if necessary
		// if( dest.end() != dest.find( var_name ) ) var_name += suffix;
		// FSHELL2_DEBUG_ASSERT( ::diagnostics::Violated_Invariance, dest.end() == dest.find( var_name ) );
		// store the value
		// dest[ var_name ] = val;
		os << " " << var_name << "=" << val;
	}

	return os;
}


Test_Suite_Output::Test_Suite_Output(::fshell2::fql::Compute_Test_Goals & goals) :
	m_goals(goals) {
}
	
::std::ostream & Test_Suite_Output::print_ts(
		Constraint_Strengthening::test_cases_t & test_suite,
		::std::ostream & os) {
	
	::cnf_clause_list_assignmentt & cnf(m_goals.get_cnf());
	
	for (::fshell2::Constraint_Strengthening::test_cases_t::const_iterator iter(
				test_suite.begin()); iter != test_suite.end(); ++iter) {
		cnf.copy_assignment_from(*iter);
		os << "IN: ";
		print_test_case(os) << ::std::endl;
	}

	return os;
}


FSHELL2_NAMESPACE_END;

