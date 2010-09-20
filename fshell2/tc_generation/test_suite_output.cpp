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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#  include <diagnostics/basic_exceptions/not_implemented.hpp>
#endif

#include <fshell2/instrumentation/goto_utils.hpp>
#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <cbmc/src/ansi-c/expr2c.h>
#include <cbmc/src/util/config.h>
#include <cbmc/src/util/std_code.h>
#include <cbmc/src/util/xml.h>
#include <cbmc/src/util/xml_irep.h>

FSHELL2_NAMESPACE_BEGIN;

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
			GLOBAL_STATIC,
			PARAMETER,
			UNKNOWN
		} variable_type_t;

		explicit Symbol_Identifier(::exprt const& sym);

		::std::string const& m_identifier;
		variable_type_t m_vt;
		::std::string m_var_name;
		::std::string m_level1, m_level2;
		int m_failed_object_level;
};

::std::ostream & operator<<(::std::ostream & os, Symbol_Identifier::variable_type_t const& vt) {
	switch (vt) {
		case Symbol_Identifier::CBMC_INTERNAL:
			os << "CBMC_INTERNAL";
			break;
		case Symbol_Identifier::CBMC_GUARD:
			os << "CBMC_GUARD";
			break;
		case Symbol_Identifier::CBMC_DYNAMIC_MEMORY:
			os << "CBMC_DYNAMIC_MEMORY";
			break;
		case Symbol_Identifier::CBMC_TMP_RETURN_VALUE:
			os << "CBMC_TMP_RETURN_VALUE";
			break;
		case Symbol_Identifier::FSHELL2_INTERNAL:
			os << "FSHELL2_INTERNAL";
			break;
		case Symbol_Identifier::LOCAL:
			os << "LOCAL";
			break;
		case Symbol_Identifier::LOCAL_STATIC:
			os << "LOCAL_STATIC";
			break;
		case Symbol_Identifier::GLOBAL:
			os << "GLOBAL";
			break;
		case Symbol_Identifier::GLOBAL_STATIC:
			os << "GLOBAL_STATIC";
			break;
		case Symbol_Identifier::PARAMETER:
			os << "PARAMETER";
			break;
		case Symbol_Identifier::UNKNOWN:
			os << "UNKNOWN";
			break;
	}
	return os;
}

Symbol_Identifier::Symbol_Identifier(::exprt const& sym) :
	m_identifier(sym.get(ID_identifier).as_string()),
	m_vt(UNKNOWN),
	m_var_name(m_identifier),
	m_level1("-1"),
	m_level2("-1"),
	m_failed_object_level(-1)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, sym.id() == ID_symbol);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, m_identifier.size() > 2);
	
	using ::std::string;

	// argc' and argv' used in the CBMC main routine
	if (string::npos != m_identifier.find('\'')) 
		m_vt = CBMC_INTERNAL;
	// other __CPROVER variables
	else if (0 == m_identifier.compare(0, string::traits_type::length("c::__CPROVER_"), "c::__CPROVER_")) 
		m_vt = CBMC_INTERNAL;
	// guards
	else if (0 == m_identifier.compare(0, string::traits_type::length("goto_symex::\\guard#"), "goto_symex::\\guard#")) 
		m_vt = CBMC_GUARD;
	// generated symbol
	else if (0 == m_identifier.compare(0, string::traits_type::length("c::$fshell2$"), "c::$fshell2$"))
		m_vt = FSHELL2_INTERNAL;
	// we don't care about the above
	if (UNKNOWN != m_vt) return;

	// first strip off #xx (level2 renaming)
	string::size_type const hash_start(m_identifier.rfind('#'));
	if (string::npos == hash_start) m_level2 = "";
	else {
		m_level2 = m_identifier.substr(hash_start + 1);
		m_var_name = m_identifier.substr(0, hash_start);
	}

	// strip off @xx (level1 renaming/frame numbers)
	string::size_type const at_start(m_identifier.rfind('@'));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			string::npos == at_start || 
			string::npos == hash_start ||
			at_start < hash_start);
	if (string::npos == at_start) m_level1 = "";
	else {
		m_level1 = m_var_name.substr(at_start + 1);
		m_var_name = m_var_name.substr(0, at_start);
	}

	// strip off $object (added to failed objects)
	string::size_type obj_start(string::npos);
	do {
		++m_failed_object_level;
		obj_start = m_var_name.rfind("$object");
		m_var_name = m_var_name.substr(0, obj_start);
	} while (string::npos != obj_start);

	// dynamically allocated memory
	if (0 == m_identifier.compare(0, string::traits_type::length("symex_dynamic::"), "symex_dynamic::")
			|| 0 == m_identifier.compare(0, string::traits_type::length("symex::invalid_object"), "symex::invalid_object"))
		m_vt = CBMC_DYNAMIC_MEMORY;
	// function call used outside assignment
	else if (string::npos != m_var_name.find("::$tmp::return_value_"))
		m_vt = CBMC_TMP_RETURN_VALUE;
	// global variables only have c:: and no other ::
	else if (string::npos == m_var_name.find("::", 3))
		m_vt = GLOBAL;
	// global static variables have one more ::
	else if (string::npos == m_var_name.find("::", m_var_name.find("::", 3) + 2))
		m_vt = GLOBAL_STATIC;
	// parameters have two more ::
	else if (string::npos == m_var_name.find("::", m_var_name.find("::", m_var_name.find("::", 3) + 2) + 2))
		m_vt = PARAMETER;
	// local static vars have more :: but no @ marking the frame
	else if (string::npos != m_var_name.find("::", m_var_name.find("::", m_var_name.find("::", 3) + 2) + 2))
		m_vt = (m_level1 != "") ? LOCAL : LOCAL_STATIC;
	
	FSHELL2_AUDIT_ASSERT1(::diagnostics::Not_Implemented, UNKNOWN != m_vt,
			::std::string("Cannot determine variable type of ") + m_identifier);
}
		
Test_Suite_Output::Test_Input::Test_Input(::symbolt const& main_sym,
		::std::string const& main_str, ::locationt const& main_loc) :
	m_main_symbol(main_sym),
	m_main_symbol_str(main_str), 
	m_main_location(main_loc) {
}

void Test_Suite_Output::get_test_case(Test_Suite_Output::Test_Input & ti) const {
	::symex_target_equationt::SSA_stepst const& equation(m_equation.get_equation().SSA_steps);
	::cnf_clause_list_assignmentt const& cnf(m_equation.get_cnf());

	// beautify pointers
	// ::std::map< ::std::string, ::std::string > beautified_name;
	
	::boolbvt const& bv(m_equation.get_bv());

	//// for (::boolbv_mapt::mappingt::const_iterator iter(bv.map.mapping.begin()); iter != bv.map.mapping.end(); ++iter) {
	//// 	::exprt sym("symbol");
	//// 	sym.set("identifier", iter->first);
	//// 	Symbol_Identifier v(sym);
	//// 	if (Symbol_Identifier::FSHELL2_INTERNAL != v.m_vt)
	//// 		::std::cerr << "SYMB: " << iter->first << " VALUE: " << ::expr2c(bv.get(sym), m_equation.get_ns()) << ::std::endl;
	//// }

	// collect variables that are used or defined, skipping level2 counters
	// (unless the variable is a CBMC_TMP_RETURN_VALUE)
	typedef ::std::set< ::std::string > seen_vars_t;
	seen_vars_t vars;
	
	// select the init procedure chosen by the user
	::std::string const start_proc_prefix(::std::string("c::") +
			ti.m_main_symbol.module.as_string() + "::" + config.main + "::");

	bool most_recent_non_hidden_is_true(false);
	// does a DEF-USE analysis
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				equation.begin() ); iter != equation.end(); ++iter)
	{
		if (!iter->is_assignment() || ::symex_targett::HIDDEN != iter->assignment_type)
			most_recent_non_hidden_is_true = (iter->guard_expr.is_true() || cnf.l_get(iter->guard_literal) == ::tvt(true));
		if (!most_recent_non_hidden_is_true || !iter->is_assignment()) continue;

		//// ::goto_programt tmp;
		//// ::std::cerr << "#########################################################" << ::std::endl;
		//// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, iter->source.pc);
		//// iter->output(m_equation.get_ns(), ::std::cerr);
		//// ::std::cerr << "LHS: " << iter->lhs << ::std::endl;
		//// ::std::cerr << "ORIG_LHS: " << iter->original_lhs << ::std::endl;
		//// ::std::cerr << "RHS: " << iter->rhs << ::std::endl;
		
		::std::map< ::exprt const*, ::exprt const* > stmt_vars_and_parents;
		::fshell2::instrumentation::collect_expr_with_parents(iter->rhs, stmt_vars_and_parents);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iter->lhs.id() == "symbol");
		stmt_vars_and_parents.insert(::std::make_pair< ::exprt const*, ::exprt const* >(&(iter->lhs), 0));
			
		for (::std::map< ::exprt const*, ::exprt const* >::const_iterator
				v_iter(stmt_vars_and_parents.begin());
				v_iter != stmt_vars_and_parents.end(); ++v_iter) {
			if (v_iter->first->id() != ID_symbol) continue;
			
			// we don't care about variables where only the address of some other
			// object is taken
			bool has_address_of(false);
			::std::map< ::exprt const*, ::exprt const* >::const_iterator p_iter(v_iter);
			while (p_iter != stmt_vars_and_parents.end() && p_iter->second && !has_address_of) {
				has_address_of = (ID_address_of == p_iter->second->id());
				p_iter = stmt_vars_and_parents.find(p_iter->second);
			}
			if (has_address_of) continue;

			bool is_lhs(v_iter->first == &(iter->lhs));

			Test_Input::program_variable_t iv;
			bool iv_in_use(false);
			::exprt const * nondet_expr(0);
		
			Symbol_Identifier var(*(v_iter->first));
			// ::std::cerr << "Analyzing " << var.m_identifier << "(" << var.m_var_name << ") [" << var.m_vt << "]" << ::std::endl;
		
			switch (var.m_vt) {
				case Symbol_Identifier::CBMC_INTERNAL: // we don't care
				case Symbol_Identifier::CBMC_GUARD: // we don't care
				case Symbol_Identifier::FSHELL2_INTERNAL: // we don't care
					break;
				case Symbol_Identifier::CBMC_DYNAMIC_MEMORY: // ignore in RHS
				case Symbol_Identifier::CBMC_TMP_RETURN_VALUE: // ignore in RHS
					if (!is_lhs) break;
				case Symbol_Identifier::PARAMETER:
				case Symbol_Identifier::LOCAL:
				case Symbol_Identifier::LOCAL_STATIC:
				case Symbol_Identifier::GLOBAL:
				case Symbol_Identifier::GLOBAL_STATIC:
					if (v_iter->second && ID_with == v_iter->second->id() && v_iter->second->op2().id() == "nondet_symbol") {
						// array/struct element assignment using undefined
						// function; we do not add the array/struct variable at
						// this point as reads to other indices may require the
						// entire array/struct to be added
						nondet_expr = &(v_iter->second->op2());
					} else if (is_lhs && iter->rhs.id() == "nondet_symbol" && ::symex_targett::HIDDEN != iter->assignment_type) {
						// assignment to simple variable using undef function
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
								vars.end() == vars.find(var.m_identifier));
						vars.insert(var.m_identifier);
						nondet_expr = &(iter->rhs);
						// ::std::cerr << "Added LHS var " << var.m_identifier << " [" << var.m_vt << "]" << ::std::endl;
						if (Symbol_Identifier::CBMC_TMP_RETURN_VALUE != var.m_vt) vars.insert(var.m_var_name + "@" + var.m_level1);
						// ::std::cerr << "Also added var " << (var.m_var_name + "@" + var.m_level1) << " [" << var.m_vt << "]" << ::std::endl;
					} else if (!is_lhs && var.m_level2 == "0" && vars.end() == vars.find(var.m_var_name + "@" + var.m_level1)) {
						// reading use of undefined variable
						if (!vars.insert(var.m_identifier).second) break;
						// ::std::cerr << "Added #0 var " << var.m_identifier << " [" << var.m_vt << "]" << ::std::endl;
						vars.insert(var.m_var_name + "@" + var.m_level1);
						// ::std::cerr << "Also added var " << (var.m_var_name + "@" + var.m_level1) << " [" << var.m_vt << "]" << ::std::endl;
					} else {
						if (!vars.insert(var.m_var_name + "@" + var.m_level1).second) break;
						// ::std::cerr << "Added var " << (var.m_var_name + "@" + var.m_level1) << " [" << var.m_vt << "]" << ::std::endl;
					}
					
					if (Symbol_Identifier::PARAMETER != var.m_vt &&
							nondet_expr && iter->source.pc->is_function_call()) {
						iv_in_use = true;
						::code_function_callt const& fct(::to_code_function_call(iter->source.pc->code));
						iv.m_name = &(fct.function());
						iv.m_pretty_name = fct.function().get("identifier").as_string();
						iv.m_value = is_lhs ? v_iter->first : nondet_expr;
						iv.m_symbol = 0;
						m_equation.get_ns().lookup(iv.m_pretty_name, iv.m_symbol);
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iv.m_symbol);
						iv.m_location = &(iter->source.pc->location);
						// remove return values of defined (but inlined) functions
						if (Symbol_Identifier::CBMC_TMP_RETURN_VALUE == var.m_vt && !iv.m_symbol->value.is_nil()) iv_in_use = false;
						/* we assume malloc doesn't return NULL, should check for
						 * invalid PTR
						 } else if (iter->source.pc->is_assign() &&
						 ::to_code_assign(iter->source.pc->code).rhs().get("statement") == "malloc") {
						 iv_in_use = true;
						 iv.m_name = &(iter->lhs);
						 iv.m_pretty_name = "malloc()";
						 iv.m_location = &(iter->source.pc->location);
						 iv.m_value = &(iter->rhs);
						 ::std::cerr << "LHS: " << iter->lhs << ::std::endl;
						 ::std::cerr << "ORIG_LHS: " << iter->original_lhs << ::std::endl;
						 ::std::cerr << "RHS: " << iter->rhs << ::std::endl; */
					} else if (!is_lhs || Symbol_Identifier::PARAMETER == var.m_vt) {
						if (Symbol_Identifier::PARAMETER == var.m_vt &&
								0 != var.m_var_name.compare(0, start_proc_prefix.size(), start_proc_prefix)) break;
						// it may happen that the value is not even known to the
						// mapping, which happens for the following code (and
						// variable s):
						//   char * buffer;
						//   unsigned s;
						//   buffer=(char*)malloc(s);
						if (!is_lhs && iter->original_lhs.get(ID_identifier) == "c::__CPROVER_alloc_size" &&
								bv.map.mapping.end() == bv.map.mapping.find(v_iter->first->get(ID_identifier))) break;
						iv_in_use = true;
						iv.m_name = v_iter->first;
						// @ comes before #, also strip off possible $object
						// (failed symbols)
						iv.m_pretty_name = var.m_var_name;
						iv.m_value = v_iter->first;
						iv.m_symbol = 0;
						m_equation.get_ns().lookup(iv.m_pretty_name, iv.m_symbol);
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iv.m_symbol);
						iv.m_location = &(iv.m_symbol->location);
					}
					break;
				case Symbol_Identifier::UNKNOWN:
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
					break;
			}
		
			if (iv_in_use) {
				//// ::std::cerr << "Test input: " << iv.m_pretty_name << ::std::endl;
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !iv.m_location->is_nil());
				iv.m_type_str = ::from_type(m_equation.get_ns(), iv.m_name->get("identifier"), iv.m_symbol->type);
		
				// obtain the value; if variable was used in pointer context, it will
				// lack #0; _Bool is handled by prop_convt, no entry in mapping
				//// ::std::cerr << "Fetching value for " << iv.m_value->pretty() << ::std::endl;
				if (ID_bool == iv.m_symbol->type.id()) {
					iv.m_value_str = ::from_expr(m_equation.get_ns(), iv.m_name->get("identifier"), bv.get(*iv.m_value));
				} else if (iv.m_value->id() != "nondet_symbol" &&
						::std::string::npos == iv.m_value->get("identifier").as_string().rfind("#")) {
					::exprt new_sym("symbol");
					new_sym.set("identifier", iv.m_value->get("identifier").as_string() + "#0");
					// not sure whether this is needed
					// if (bv.map.mapping.end() == bv.map.mapping.find(new_sym.get("identifier"))) {
					//   new_sym.set("identifier", ::diagnostics::internal::to_string(iv.m_value->get("identifier"), "$object#0"));
					// }
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, bv.map.mapping.end() !=
							bv.map.mapping.find(new_sym.get("identifier")));
					iv.m_value_str = ::from_expr(m_equation.get_ns(), iv.m_name->get("identifier"), bv.get(new_sym));
				} else {
					FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, bv.map.mapping.end() !=
							bv.map.mapping.find(iv.m_value->get("identifier")),
							::diagnostics::internal::to_string("Failed to lookup ", iv.m_value->get("identifier")));
					iv.m_value_str = ::from_expr(m_equation.get_ns(), iv.m_name->get("identifier"), bv.get(*iv.m_value));
				}
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
					
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !iv.m_value_str.empty());
				ti.m_test_inputs.push_back(iv);
			}
		}
	}

}

::std::ostream & Test_Suite_Output::print_test_case_plain(::std::ostream & os,
		Test_Suite_Output::Test_Input const& ti) const
{
	bool const do_full(!m_opts.get_bool_option("brief-test-inputs"));

	if (do_full)
		os << "  ENTRY " << ti.m_main_symbol_str << "@[" << ti.m_main_location << "]" << ::std::endl;

	for (Test_Suite_Output::Test_Input::test_inputs_t::const_iterator iter(ti.m_test_inputs.begin()); 
			iter != ti.m_test_inputs.end(); ++iter) {
		os << "  ";
		bool global(false);
		if (iter->m_symbol->type.id() == "code") {
			::std::string::size_type pos(iter->m_type_str.find('('));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, pos != ::std::string::npos);
			if (do_full) os << iter->m_type_str.substr(0, pos);
			os << iter->m_symbol->base_name << iter->m_type_str.substr(pos);
		} else {
			if (do_full) os << iter->m_type_str << " ";
			os << iter->m_symbol->base_name;
			Symbol_Identifier var(::symbol_exprt(iter->m_symbol->name));
			global = (Symbol_Identifier::GLOBAL == var.m_vt || Symbol_Identifier::GLOBAL_STATIC == var.m_vt);
		}
		if (do_full)
			os << "@[" << *(iter->m_location) << (global ? " #global":"") << "]";
		os << "=" << iter->m_value_str;
		os << ::std::endl;
	}
	
	return os;
}

::std::ostream & Test_Suite_Output::print_test_case_xml(::std::ostream & os,
		Test_Suite_Output::Test_Input const& ti) const
{
	bool const do_full(!m_opts.get_bool_option("brief-test-inputs"));

	::xmlt xml_tc("test-case");

	if (do_full) {
		::xmlt xml_entry("entry");
		xml_entry.new_element("function").data = ti.m_main_symbol_str;
		::xmlt xml_main_loc("location");
		::convert(ti.m_main_location, xml_main_loc);
		xml_entry.new_element().swap(xml_main_loc);
		xml_tc.new_element().swap(xml_entry);
	}

	for (Test_Suite_Output::Test_Input::test_inputs_t::const_iterator iter(ti.m_test_inputs.begin()); 
			iter != ti.m_test_inputs.end(); ++iter) {
		::xmlt xml_obj("object");
		xml_obj.set_attribute("kind", (iter->m_symbol->type.id() == ID_code ? "undefined-function" : "variable"));

		if (do_full)
			xml_obj.new_element("identifier").data = ::xmlt::escape(::id2string(iter->m_name->get("identifier")));
		xml_obj.new_element("base_name").data = ::xmlt::escape(::id2string(iter->m_symbol->base_name));
	
		if (do_full) {
			::xmlt xml_loc("location");
			::convert(*(iter->m_location), xml_loc);
			xml_obj.new_element().swap(xml_loc);
		}
		
		xml_obj.new_element("value").data = ::xmlt::escape(iter->m_value_str);
		if (do_full)
			xml_obj.new_element("type").data = ::xmlt::escape(iter->m_type_str);
		
		xml_tc.new_element().swap(xml_obj);
	}

	xml_tc.output(os, 2);
	return os;
}

Test_Suite_Output::Test_Suite_Output(::fshell2::fql::CNF_Conversion & equation,
		::optionst const& opts) :
	m_equation(equation),
	m_opts(opts)
{
}
	
::std::ostream & Test_Suite_Output::print_ts(
		Constraint_Strengthening::test_cases_t & test_suite,
		::std::ostream & os, ::ui_message_handlert::uit const ui) {
	
	::cnf_clause_list_assignmentt & cnf(m_equation.get_cnf());
	
	// select the init procedure chosen by the user
	::std::string main_symb_name("c::");
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !config.main.empty());
	main_symb_name += config.main;
	::symbolt const& main_symb(m_equation.get_ns().lookup(main_symb_name));
	::code_typet::argumentst const &arguments(::to_code_type(main_symb.type).arguments());
	::std::ostringstream main_symb_str;
	main_symb_str << main_symb.base_name << "(";
	for (::code_typet::argumentst::const_iterator iter(arguments.begin());
			iter != arguments.end(); ++iter) {
		if (iter != arguments.begin()) main_symb_str << ",";
		main_symb_str << iter->get(ID_C_base_name);
	}
	main_symb_str << ")";
	Test_Input ti(main_symb, main_symb_str.str(), main_symb.location);
	
	for (::fshell2::Constraint_Strengthening::test_cases_t::const_iterator iter(
				test_suite.begin()); iter != test_suite.end(); ++iter) {
		cnf.copy_assignment_from(iter->second);
		ti.m_test_inputs.clear();
		get_test_case(ti);
		switch (ui) {
			case ::ui_message_handlert::XML_UI:
				os << "<test-suite>" << ::std::endl;
				print_test_case_xml(os, ti);
				os << "</test-suite>" << ::std::endl;
				break;
			case ::ui_message_handlert::PLAIN:
			case ::ui_message_handlert::OLD_GUI:
				os << "IN:" << ::std::endl;
				print_test_case_plain(os, ti);
				os << ::std::endl;
				break;
		}
	}

	return os;
}


FSHELL2_NAMESPACE_END;

