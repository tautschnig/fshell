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
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <cbmc/src/ansi-c/expr2c.h>
#include <cbmc/src/util/config.h>
#include <cbmc/src/util/std_code.h>
#include <cbmc/src/util/xml.h>
#include <cbmc/src/util/xml_irep.h>

FSHELL2_NAMESPACE_BEGIN;

typedef enum {
	CBMC_INTERNAL,
	CBMC_GUARD,
	CBMC_DYNAMIC_MEMORY,
	CBMC_TMP_RETURN_VALUE,
	FSHELL2_INTERNAL,
	LOCAL,
	GLOBAL,
	PARAMETER,
	UNKNOWN
} variable_type_t;

::std::ostream & operator<<(::std::ostream & os, variable_type_t const& vt) {
	switch (vt) {
		case CBMC_INTERNAL:
			os << "CBMC_INTERNAL";
			break;
		case CBMC_GUARD:
			os << "CBMC_GUARD";
			break;
		case CBMC_DYNAMIC_MEMORY:
			os << "CBMC_DYNAMIC_MEMORY";
			break;
		case CBMC_TMP_RETURN_VALUE:
			os << "CBMC_TMP_RETURN_VALUE";
			break;
		case FSHELL2_INTERNAL:
			os << "FSHELL2_INTERNAL";
			break;
		case LOCAL:
			os << "LOCAL";
			break;
		case GLOBAL:
			os << "GLOBAL";
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
	// argc' and argv' used in the CBMC main routine
	if (::std::string::npos != v.find('\'')) return CBMC_INTERNAL;
	// other __CPROVER variables
	else if (0 == v.find("c::__CPROVER_")) return CBMC_INTERNAL;
	// guards
	else if (0 == v.find("goto_symex::\\guard#")) return CBMC_GUARD;
	// dynamically allocated memory
	else if (0 == v.find("symex_dynamic::") || 0 == v.find("symex::invalid_object")) return CBMC_DYNAMIC_MEMORY;
	// function call used outside assignment
	else if (::std::string::npos != v.find("::$tmp::return_value_")) return CBMC_TMP_RETURN_VALUE;
	// generated symbol
	else if (0 == v.find("c::!fshell2!")) return FSHELL2_INTERNAL;
	// global variables only have c:: and not other ::
	else if (::std::string::npos == v.find("::", 3)) return GLOBAL;
	// parameters have two more ::
	else if (::std::string::npos == v.find("::", v.find("::", v.find("::", 3) + 2) + 2)) return PARAMETER;
	// must have a frame, otherwise I have no idea...
	else if (::std::string::npos != v.find('@')) return LOCAL;
	
	FSHELL2_PROD_CHECK1(::diagnostics::Not_Implemented, false,
			::diagnostics::internal::to_string("Cannot determine variable type of ", v));
	return UNKNOWN;
}
	
void find_symbols(::exprt const& expr, ::std::list< ::exprt const * > & symbols) {
	if (expr.id() == "symbol") {
		symbols.push_back(&expr);
	} else {
		forall_operands(iter, expr) find_symbols(*iter, symbols);
	}
}

void Test_Suite_Output::get_test_case(Test_Suite_Output::test_case_t & tc) const {
	tc.clear();
	::symex_target_equationt::SSA_stepst const& equation(m_goals.get_equation());
	::cnf_clause_list_assignmentt const& cnf(m_goals.get_cnf());

	// collect variables that are used or defined, skipping level2 counters
	typedef ::std::set< ::std::string > seen_vars_t;
	seen_vars_t vars;
	
	// select the init procedure chosen by the user
	::std::string main_symb_name("c::");
	main_symb_name += config.main;
	::symbolt const& main_symb(m_goals.get_ns().lookup(main_symb_name));
	::std::string const start_proc_prefix(::diagnostics::internal::to_string(
				"c::", main_symb.module, "::", config.main, "::"));

	// does a DEF-USE analysis
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				equation.begin() ); iter != equation.end(); ++iter)
	{
		if (!iter->guard_expr.is_true() && cnf.l_get(iter->guard_literal) != ::tvt(true)) continue;
		//// ::goto_programt tmp;
		//// ::std::cerr << "#########################################################" << ::std::endl;
		//// tmp.output_instruction(m_goals.get_ns(), "", ::std::cerr, iter->source.pc);
		//// iter->output(m_goals.get_ns(), ::std::cerr);
		if (!iter->is_assignment()) continue;
		//// ::std::cerr << "LHS: " << iter->lhs << ::std::endl;
		//// ::std::cerr << "ORIG_LHS: " << iter->original_lhs << ::std::endl;
		//// ::std::cerr << "RHS: " << iter->rhs << ::std::endl;
		
		::std::list< ::exprt const* > stmt_vars;
		find_symbols(iter->rhs, stmt_vars);
		for (::std::list< ::exprt const* >::const_iterator v_iter(stmt_vars.begin());
				v_iter != stmt_vars.end(); ++v_iter) {
			// keep the name for further manipulation
			::std::string const& var_name((*v_iter)->get("identifier").as_string());
			//// ::std::cerr << "Checking " << var_name << ::std::endl;
			if (!vars.insert(var_name.substr(0, var_name.rfind('#'))).second) continue;
			//// ::std::cerr << "Added var " << var_name << ::std::endl;
			switch (get_variable_type(var_name)) {
				case CBMC_INTERNAL: // we don't care
				case CBMC_GUARD: // we don't care
				case FSHELL2_INTERNAL: // we don't care
				case CBMC_DYNAMIC_MEMORY: // ignore in RHS
					break;
				case CBMC_TMP_RETURN_VALUE:
					// we must have seen these on the LHS first
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
					break;
				case PARAMETER:
				case LOCAL:
				case GLOBAL:
					tc.push_back(program_variable_t());
					tc.back().m_name = *v_iter;
					tc.back().m_pretty_name = var_name.substr(0, var_name.rfind('#')).substr(0, var_name.rfind('@')); // @ comes before #
					tc.back().m_value = *v_iter;
					tc.back().m_symbol = 0;
					m_goals.get_ns().lookup(tc.back().m_pretty_name, tc.back().m_symbol);
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tc.back().m_symbol);
					tc.back().m_location = &(tc.back().m_symbol->location);
					break;
				case UNKNOWN:
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
					break;
			}
		}
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iter->lhs.id() == "symbol");
		// keep the name for further manipulation
		::std::string const& var_name(iter->lhs.get("identifier").as_string());
		//// ::std::cerr << "Checking " << var_name << ::std::endl;
		if (!vars.insert(var_name.substr(0, var_name.rfind('#'))).second) continue;
		//// ::std::cerr << "Added var " << var_name << ::std::endl;
		switch (get_variable_type(var_name)) {
			case CBMC_INTERNAL: // we don't care
			case CBMC_GUARD: // we don't care
			case FSHELL2_INTERNAL: // we don't care
				break;
			case PARAMETER:
				if (0 == var_name.find(start_proc_prefix)) {
					tc.push_back(program_variable_t());
					tc.back().m_name = &(iter->lhs);
					tc.back().m_pretty_name = var_name.substr(0, var_name.rfind('#'));
					tc.back().m_value = &(iter->lhs);
					tc.back().m_symbol = 0;
					m_goals.get_ns().lookup(tc.back().m_pretty_name, tc.back().m_symbol);
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tc.back().m_symbol);
					tc.back().m_location = &(tc.back().m_symbol->location);
				}
				break;
			case CBMC_DYNAMIC_MEMORY:
			case CBMC_TMP_RETURN_VALUE:
			case LOCAL:
			case GLOBAL:
				if (iter->rhs.is_constant()) break;
				if (iter->source.pc->is_function_call()) {
					::code_function_callt const& fct(::to_code_function_call(iter->source.pc->code));
					tc.push_back(program_variable_t());
					tc.back().m_name = &(fct.function());
					tc.back().m_pretty_name = fct.function().get("identifier").as_string();
					tc.back().m_value = &(iter->lhs);
					tc.back().m_symbol = 0;
					m_goals.get_ns().lookup(tc.back().m_pretty_name, tc.back().m_symbol);
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tc.back().m_symbol);
					tc.back().m_location = &(iter->source.pc->location);
				/* we assume malloc doesn't return NULL, should check for
				 * invalid PTR
				} else if (iter->source.pc->is_assign() &&
						::to_code_assign(iter->source.pc->code).rhs().get("statement") == "malloc") {
					tc.push_back(program_variable_t());
					tc.back().m_name = &(iter->lhs);
					tc.back().m_pretty_name = "malloc()";
					tc.back().m_location = &(iter->source.pc->location);
					tc.back().m_value = &(iter->rhs);
					::std::cerr << "LHS: " << iter->lhs << ::std::endl;
					::std::cerr << "ORIG_LHS: " << iter->original_lhs << ::std::endl;
					::std::cerr << "RHS: " << iter->rhs << ::std::endl; */
				} else if (iter->rhs.id() == "nondet_symbol") {
					tc.push_back(program_variable_t());
					tc.back().m_name = &(iter->lhs);
					tc.back().m_pretty_name = var_name.substr(0, var_name.rfind('#')).substr(0, var_name.rfind('@')); // @ comes before #
					tc.back().m_value = &(iter->lhs);
					tc.back().m_symbol = 0;
					m_goals.get_ns().lookup(tc.back().m_pretty_name, tc.back().m_symbol);
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tc.back().m_symbol);
					tc.back().m_location = &(tc.back().m_symbol->location);
				} else {
					//// ::std::cerr << "Skipping:" << ::std::endl;
					//// ::goto_programt tmp;
					//// tmp.output_instruction(m_goals.get_ns(), "", ::std::cerr, iter->source.pc);
					//// iter->output(m_goals.get_ns(), ::std::cerr);
					//// ::std::cerr << "LHS: " << iter->lhs << ::std::endl;
					//// ::std::cerr << "ORIG_LHS: " << iter->original_lhs << ::std::endl;
					//// ::std::cerr << "RHS: " << iter->rhs << ::std::endl;
				}
				break;
			case UNKNOWN:
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
				break;
		}
	}
}

::std::ostream & Test_Suite_Output::print_test_case_plain(::std::ostream & os,
		Test_Suite_Output::test_case_t const& tc) const {
	// beautify pointers
	// ::std::map< ::std::string, ::std::string > beautified_name;
	
	::boolbvt const& bv(m_goals.get_bv());

	//// for (::boolbv_mapt::mappingt::const_iterator iter(bv.map.mapping.begin()); iter != bv.map.mapping.end(); ++iter) {
	//// 	::exprt sym("symbol");
	//// 	sym.set("identifier", iter->first);
	//// 	if (get_variable_type(iter->first.as_string()) != FSHELL2_INTERNAL)
	//// 		::std::cerr << "SYMB: " << iter->first << " VALUE: " << ::expr2c(bv.get(sym), m_goals.get_ns()) << ::std::endl;
	//// }
	
	// select the init procedure chosen by the user
	::std::string main_symb_name("c::");
	main_symb_name += config.main;
	::symbolt const& main_symb(m_goals.get_ns().lookup(main_symb_name));
	::code_typet::argumentst const &arguments(::to_code_type(main_symb.type).arguments());
	os << "  ENTRY " << main_symb.base_name << "(";
	for (::code_typet::argumentst::const_iterator iter(arguments.begin());
			iter != arguments.end(); ++iter) {
		if (iter != arguments.begin()) os << ",";
		os << iter->get("#base_name");
	}
	os << ")@[" << main_symb.location << "]" << ::std::endl;

	for (test_case_t::const_iterator iter(tc.begin()); iter != tc.end(); ++iter) {
		os << "  ";
		if (iter->m_symbol->type.id() == "code") {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iter->m_symbol->value.is_nil());
			::std::string const decl(::from_type(m_goals.get_ns(),
						iter->m_name->get("identifier"), iter->m_symbol->type));
			::std::string::size_type pos(decl.find('('));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, pos != ::std::string::npos);
			os << decl.substr(0, pos) << iter->m_symbol->base_name << decl.substr(pos);
		} else {
			os << ::from_type(m_goals.get_ns(), iter->m_name->get("identifier"), iter->m_symbol->type);
			os << " " << iter->m_symbol->base_name;
		}
		FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !iter->m_location->is_nil());
		os << "@[" << *(iter->m_location) << "]";
		// obtain the value
		::std::string const val(::from_expr(m_goals.get_ns(),
					iter->m_name->get("identifier"), bv.get(*(iter->m_value))));
		FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !val.empty());
		/* // beautify dynamic_X_name
		if(0 == val.find("&dynamic_")) {
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
		*/
		os << "=" << val << ::std::endl;
	}

	return os;
}

::std::ostream & Test_Suite_Output::print_test_case_xml(::std::ostream & os,
		Test_Suite_Output::test_case_t const& tc) const {
	::boolbvt const& bv(m_goals.get_bv());

	::xmlt xml_tc("test-case");

	// select the init procedure chosen by the user
	::std::string main_symb_name("c::");
	main_symb_name += config.main;
	::symbolt const& main_symb(m_goals.get_ns().lookup(main_symb_name));
	::code_typet::argumentst const &arguments(::to_code_type(main_symb.type).arguments());
	::std::ostringstream oss;
	oss << main_symb.base_name << "(";
	for (::code_typet::argumentst::const_iterator iter(arguments.begin());
			iter != arguments.end(); ++iter) {
		if (iter != arguments.begin()) oss << ",";
		oss << iter->get("#base_name");
	}
	oss << ")";
	::xmlt xml_entry("entry");
	xml_entry.new_element("function").data = oss.str();
	::xmlt xml_main_loc("location");
	::convert(main_symb.location, xml_main_loc);
	xml_entry.new_element().swap(xml_main_loc);
	xml_tc.new_element().swap(xml_entry);

	for (test_case_t::const_iterator iter(tc.begin()); iter != tc.end(); ++iter) {
		::xmlt xml_obj("object");
		if (iter->m_symbol->type.id() == "code") {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iter->m_symbol->value.is_nil());
			xml_obj.set_attribute("kind", "undefined-function");
		} else {
			xml_obj.set_attribute("kind", "variable");
		}
		xml_obj.new_element("identifier").data = ::xmlt::escape(::id2string(iter->m_name->get("identifier")));
		xml_obj.new_element("base_name").data = ::xmlt::escape(::id2string(iter->m_symbol->base_name));
		FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !iter->m_location->is_nil());
		::xmlt xml_loc("location");
		::convert(*(iter->m_location), xml_loc);
		xml_obj.new_element().swap(xml_loc);
		// obtain the value
		::std::string const val(::from_expr(m_goals.get_ns(),
					iter->m_name->get("identifier"), bv.get(*(iter->m_value))));
		FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !val.empty());
		xml_obj.new_element("value").data = ::xmlt::escape(val);
		xml_obj.new_element("type").data = ::xmlt::escape(::from_type(m_goals.get_ns(),
					iter->m_name->get("identifier"), iter->m_symbol->type));
		xml_tc.new_element().swap(xml_obj);
	}

	xml_tc.output(os, 2);
	return os;
}

Test_Suite_Output::Test_Suite_Output(::fshell2::fql::Compute_Test_Goals & goals) :
	m_goals(goals) {
}
	
::std::ostream & Test_Suite_Output::print_ts(
		Constraint_Strengthening::test_cases_t & test_suite,
		::std::ostream & os, ::ui_message_handlert::uit const ui) {
	
	::cnf_clause_list_assignmentt & cnf(m_goals.get_cnf());
	
	for (::fshell2::Constraint_Strengthening::test_cases_t::const_iterator iter(
				test_suite.begin()); iter != test_suite.end(); ++iter) {
		cnf.copy_assignment_from(*iter);
		test_case_t tc;
		get_test_case(tc);
		switch (ui) {
			case ::ui_message_handlert::XML_UI:
				os << "<test-suite>" << ::std::endl;
				print_test_case_xml(os, tc);
				os << "</test-suite>" << ::std::endl;
				break;
			case ::ui_message_handlert::PLAIN:
			case ::ui_message_handlert::OLD_GUI:
				os << "IN:" << ::std::endl;
				print_test_case_plain(os, tc);
				os << ::std::endl;
				break;
		}
	}

	return os;
}


FSHELL2_NAMESPACE_END;

