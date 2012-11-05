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

#include <ansi-c/expr2c.h>
#include <util/config.h>
#include <util/std_code.h>
#include <util/std_expr.h>
#include <util/xml.h>
#include <util/xml_irep.h>

FSHELL2_NAMESPACE_BEGIN;

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
		case Symbol_Identifier::PARAMETER:
			os << "PARAMETER";
			break;
		case Symbol_Identifier::UNKNOWN:
			os << "UNKNOWN";
			break;
	}
	return os;
}

Symbol_Identifier::Symbol_Identifier(::symbol_exprt const& sym) :
	m_identifier(sym.get_identifier()),
	m_vt(UNKNOWN),
	m_var_name(m_identifier.as_string()),
	m_level0("-1"),
	m_level1("-1"),
	m_level2("-1"),
	m_failed_object_level(-1)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, m_var_name.size() > 2);

	using ::std::string;

	// argc' and argv' used in the CBMC main routine
	if (string::npos != m_var_name.find('\''))
		m_vt = CBMC_INTERNAL;
	// other __CPROVER variables
	else if (0 == m_var_name.compare(0,
				string::traits_type::length("c::__CPROVER_"),
				"c::__CPROVER_"))
		m_vt = CBMC_INTERNAL;
	// guards
	else if (0 == m_var_name.compare(0,
				string::traits_type::length("goto_symex::\\guard#"),
				"goto_symex::\\guard#"))
		m_vt = CBMC_GUARD;
	// generated symbol
	else if (0 == m_var_name.compare(0,
				string::traits_type::length("c::$fshell2$"),
				"c::$fshell2$"))
		m_vt = FSHELL2_INTERNAL;
	// we don't care about the above
	if (UNKNOWN != m_vt) return;

	// first strip off #xx (level2 renaming)
	string::size_type const hash_start(m_var_name.rfind('#'));
	if (string::npos == hash_start) m_level2 = "";
	else {
		m_level2 = m_var_name.substr(hash_start + 1);
		m_var_name.erase(hash_start);
	}

	// strip off @xx (level1 renaming/frame numbers)
	string::size_type const at_start(m_var_name.rfind('@'));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			string::npos == at_start ||
			string::npos == hash_start ||
			at_start < hash_start);
	if (string::npos == at_start) m_level1 = "";
	else {
		m_level1 = m_var_name.substr(at_start + 1);
		m_var_name.erase(at_start);
	}

	// strip off !xx (level0 renaming/threads)
	string::size_type const exc_start(m_var_name.rfind('!'));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			(string::npos == exc_start ||
			 string::npos != at_start) &&
			(string::npos != exc_start ||
			 string::npos == at_start));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			string::npos == exc_start ||
			string::npos == at_start ||
			exc_start < at_start);
	if (string::npos == exc_start) m_level0 = "";
	else {
		m_level0 = m_var_name.substr(exc_start + 1);
		m_var_name.erase(exc_start);
	}

	// strip off $object (added to failed objects)
	string::size_type obj_start(m_var_name.rfind("$object"));
	while (string::npos != obj_start) {
		++m_failed_object_level;
		m_var_name.erase(obj_start);
		obj_start = m_var_name.rfind("$object");
	}

	// dynamically allocated memory
	if (0 == m_var_name.compare(0,
				string::traits_type::length("symex_dynamic::"),
				"symex_dynamic::") ||
			0 == m_var_name.compare(0,
				string::traits_type::length("symex::invalid_object"),
				"symex::invalid_object"))
		m_vt = CBMC_DYNAMIC_MEMORY;
	// function call used outside assignment
	else if (string::npos != m_var_name.find("::$tmp::return_value_"))
		m_vt = CBMC_TMP_RETURN_VALUE;
	// parameters or other local variables
	else if (string::npos != m_var_name.find("::", 3)) {
		// parameters have one more ::
		if (string::npos == m_var_name.find("::", m_var_name.find("::", 3) + 2))
			m_vt = PARAMETER;
		// local vars have two more ::, static ones have no @ marking the frame
		else
			m_vt = (m_level1.empty()) ? LOCAL_STATIC : LOCAL;
	}
	// global variables only have no thread and no frame
	else if (m_level0.empty() && m_level1.empty())
		m_vt = GLOBAL;
	
	FSHELL2_AUDIT_ASSERT1(::diagnostics::Not_Implemented, UNKNOWN != m_vt,
			::std::string("Cannot determine variable type of ") + m_identifier.as_string());
}
		
Test_Suite_Output::Test_Input::Test_Input(::symbolt const& main_sym,
		::std::string const& main_str, ::locationt const& main_loc) :
	m_main_symbol(main_sym),
	m_main_symbol_str(main_str), 
	m_main_location(main_loc) {
}

void Test_Suite_Output::get_test_case(Test_Input & ti, called_functions_t & calls,
			assignments_t & global_assign) const
{
	::symex_target_equationt::SSA_stepst const& equation(m_equation.get_equation().SSA_steps);
	::cnf_clause_list_assignmentt const& cnf(m_equation.get_cnf());

	// beautify pointers
	// ::std::map< ::std::string, ::std::string > beautified_name;
	
	::boolbvt const& bv(m_equation.get_bv());

	//// ::boolbv_mapt const& map(bv.get_map());
	//// for (::boolbv_mapt::mappingt::const_iterator iter(map.mapping.begin());
	//// 		iter != map.mapping.end(); ++iter) {
	//// 	::symbol_exprt sym(iter->first);
	//// 	Symbol_Identifier v(sym);
	//// 	if (Symbol_Identifier::FSHELL2_INTERNAL != v.m_vt)
	//// 		::std::cerr << "SYMB: " << iter->first << " [" << v.m_vt << "] VALUE: " << ::expr2c(bv.get(sym), m_equation.get_ns()) << ::std::endl;
	//// }

	// collect variables that are used or defined, skipping level2 counters
	// (unless the variable is a CBMC_TMP_RETURN_VALUE)
	typedef ::std::set< ::irep_idt > seen_vars_t;
	seen_vars_t vars;
	
	// select the init procedure chosen by the user
	::std::string const start_proc_prefix(::std::string("c::") + config.main + "::");

	// track calls and returns
	::std::list< called_functions_t::iterator > call_stack;

	bool most_recent_non_hidden_is_true(false);
	// does a DEF-USE + call stack analysis
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				equation.begin() ); iter != equation.end(); ++iter)
	{
		//// ::goto_programt tmp;
		//// ::std::cerr << "#########################################################" << ::std::endl;
		//// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, iter->source.pc);
		//// iter->output(m_equation.get_ns(), ::std::cerr);
		//// ::std::cerr << "LHS: " << iter->ssa_lhs << ::std::endl;
		//// ::std::cerr << "ORIG_LHS: " << iter->original_full_lhs << ::std::endl;
		//// ::std::cerr << "RHS: " << iter->ssa_rhs << ::std::endl;
	
		bool const instr_enabled(iter->guard_expr.is_true() || cnf.l_get(iter->guard_literal) == ::tvt(true));

		if (instr_enabled && !::fshell2::instrumentation::GOTO_Transformation::is_instrumented(
					iter->source.pc) && 
				(!iter->is_assignment() || ::symex_targett::HIDDEN != iter->assignment_type)) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !iter->source.pc->function.empty());
			
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
					iter->source.pc->function == ID_main || !call_stack.empty());
			if (!call_stack.empty() && iter->source.pc != call_stack.back()->first.first) {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						call_stack.back()->first.first->is_function_call() ||
						call_stack.back()->first.first->is_location());
					
				::irep_idt const fname(call_stack.back()->first.second);
				if (iter->source.pc->function != fname) {
					// ::std::cerr << "POP " << call_stack.back()->first.second << ::std::endl;
					call_stack.pop_back();
					if (iter->source.pc->function != ID_main) {
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !call_stack.empty());
						// handle sequences of functions without explicit return
						while (iter->source.pc->function != call_stack.back()->first.second) {
							// ::std::cerr << "Want POP-more because of " << call_stack.back()->first.second << " vs. current pc: " << iter->source.pc->function << ::std::endl;
							FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !call_stack.empty());
							call_stack.pop_back();
							// ::std::cerr << "POP-more" << ::std::endl;
						}
					}
				}
			}
		
			::goto_programt::const_targett next(iter->source.pc);
			++next;
			// ::std::cerr << "this func: " << iter->source.pc->function << " next func: " << next->function << ::std::endl;
			// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, next);
			if (iter->is_location() && iter->source.pc->is_function_call()) {
				calls.push_back(::std::make_pair< ::std::pair< ::goto_programt::const_targett,
						::irep_idt const >, ::exprt const* >(::std::make_pair(iter->source.pc,
								::to_code_function_call(iter->source.pc->code).function().get(ID_identifier)), 0));
				call_stack.push_back(calls.end());
				--call_stack.back();
				// ::std::cerr << "PUSH " << call_stack.back()->first.second << ::std::endl;
			} else if (iter->is_location() &&
					iter->source.pc->function != next->function) {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						!call_stack.back()->first.first->is_location() || call_stack.size() >= 2);
				// inlined function
				if (call_stack.back()->first.first->is_location() &&
						(*--(--call_stack.end()))->first.second == next->function) {
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == call_stack.back()->second);
					call_stack.pop_back();
					// ::std::cerr << "POP-inline" << ::std::endl;
				} else {
					calls.push_back(::std::make_pair< ::std::pair< ::goto_programt::const_targett,
							::irep_idt const >, ::exprt const* >(::std::make_pair(iter->source.pc,
									next->function), 0));
					call_stack.push_back(calls.end());
					--call_stack.back();
					// ::std::cerr << "PUSH-inline" << ::std::endl;
				}
			} else if (iter->is_assignment() && iter->source.pc->is_return()) {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !call_stack.empty());
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == call_stack.back()->second);
				call_stack.back()->second = &(iter->ssa_lhs);
				// required for recursive functions
				call_stack.pop_back();
				// ::std::cerr << "POP-return" << ::std::endl;
			} 
		}
		
		if (!iter->is_assignment() || ::symex_targett::HIDDEN != iter->assignment_type)
			most_recent_non_hidden_is_true = instr_enabled;
		if (!most_recent_non_hidden_is_true || !iter->is_assignment()) continue;
		
		typedef ::std::map< ::exprt const*, ::exprt const* > stmt_vars_and_parents_t;
		stmt_vars_and_parents_t stmt_vars_and_parents;
		::fshell2::instrumentation::collect_expr_with_parents(iter->ssa_rhs, stmt_vars_and_parents);
		::std::list< stmt_vars_and_parents_t::const_iterator > nondet_syms_in_rhs;
		for (stmt_vars_and_parents_t::const_iterator
				v_iter(stmt_vars_and_parents.begin());
				v_iter != stmt_vars_and_parents.end(); ++v_iter)
			if (ID_nondet_symbol == v_iter->first->id()) nondet_syms_in_rhs.push_back(v_iter);

		stmt_vars_and_parents.insert(::std::make_pair< ::exprt const*, ::exprt const* >(&(iter->ssa_lhs), 0));
			
		for (stmt_vars_and_parents_t::const_iterator
				v_iter(stmt_vars_and_parents.begin());
				v_iter != stmt_vars_and_parents.end(); ++v_iter) {
			if (v_iter->first->id() != ID_symbol) continue;
			
			// we don't care about variables where only the address of some other
			// object is taken
			bool has_address_of(false);
			stmt_vars_and_parents_t::const_iterator p_iter(v_iter);
			while (p_iter != stmt_vars_and_parents.end() && p_iter->second && !has_address_of) {
				has_address_of = (ID_address_of == p_iter->second->id());
				p_iter = stmt_vars_and_parents.find(p_iter->second);
			}
			if (has_address_of) continue;

			bool is_lhs(v_iter->first == &(iter->ssa_lhs));

			Test_Input::program_variable_t iv;
			bool iv_in_use(false);
			::exprt const * nondet_expr(0);
		
			Symbol_Identifier var(::to_symbol_expr(*(v_iter->first)));
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
					if (is_lhs && ::symex_targett::HIDDEN != iter->assignment_type &&
							Symbol_Identifier::GLOBAL == var.m_vt)
						global_assign.push_back(iter);
					
					if (v_iter->second && ID_with == v_iter->second->id() && v_iter->second->op2().id() == ID_nondet_symbol) {
						// array/struct element assignment using undefined
						// function; we do not add the array/struct variable at
						// this point as reads to other indices may require the
						// entire array/struct to be added
						nondet_expr = &(v_iter->second->op2());
					} else if (is_lhs && iter->ssa_rhs.id() == ID_struct && ::symex_targett::HIDDEN != iter->assignment_type &&
							!nondet_syms_in_rhs.empty()) {
						// assignment to struct variable with nondet components;
						// again, it seems safer not to add the entire array
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
								1 == nondet_syms_in_rhs.size());
						nondet_expr = nondet_syms_in_rhs.front()->first;
					} else if (is_lhs && iter->ssa_rhs.id() == ID_nondet_symbol && ::symex_targett::HIDDEN != iter->assignment_type) {
						// assignment to simple variable using undef function
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
								vars.end() == vars.find(var.m_identifier));
						vars.insert(var.m_identifier);
						nondet_expr = v_iter->first;
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
						iv.m_pretty_name = fct.function().get(ID_identifier);
						iv.m_value = nondet_expr;
						iv.m_symbol = 0;
						m_equation.get_ns().lookup(iv.m_pretty_name, iv.m_symbol);
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iv.m_symbol);
						iv.m_location = &(iter->source.pc->location);
						// remove return values of defined (but inlined) functions
						if (Symbol_Identifier::CBMC_TMP_RETURN_VALUE == var.m_vt && !iv.m_symbol->value.is_nil()) iv_in_use = false;
						/* we assume malloc doesn't return NULL, should check for
						 * invalid PTR
						 } else if (iter->source.pc->is_assign() &&
						 ::to_code_assign(iter->source.pc->code).ssa_rhs().get("statement") == "malloc") {
						 iv_in_use = true;
						 iv.m_name = &(iter->ssh_full_lhs);
						 iv.m_pretty_name = "malloc()";
						 iv.m_location = &(iter->source.pc->location);
						 iv.m_value = &(iter->ssa_rhs);
						 ::std::cerr << "LHS: " << iter->ssa_lhs << ::std::endl;
						 ::std::cerr << "ORIG_LHS: " << iter->original_full_lhs << ::std::endl;
						 ::std::cerr << "RHS: " << iter->ssa_rhs << ::std::endl; */
					} else if (!is_lhs || Symbol_Identifier::PARAMETER == var.m_vt) {
						if (Symbol_Identifier::PARAMETER == var.m_vt &&
								0 != var.m_var_name.compare(0, start_proc_prefix.size(), start_proc_prefix)) break;
						// it may happen that the value is not even known to the
						// mapping, which happens for the following code (and
						// variable s):
						//   char * buffer;
						//   unsigned s;
						//   buffer=(char*)malloc(s);
						if (!is_lhs && iter->original_full_lhs.get(ID_identifier) == "c::__CPROVER_alloc_size" &&
								bv.get(*(v_iter->first)).is_nil()) break;
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
				iv.m_type_str = ::from_type(m_equation.get_ns(), iv.m_name->get(ID_identifier), iv.m_symbol->type);
		
				// obtain the value; if variable was used in pointer context, it will
				// lack #0; _Bool is handled by prop_convt, no entry in mapping
				//// ::std::cerr << "Fetching value for " << iv.m_value->pretty() << ::std::endl;
				::exprt val_copy(*iv.m_value);
				if (ID_bool != iv.m_symbol->type.id() && val_copy.id() != ID_nondet_symbol &&
						::std::string::npos == val_copy.get(ID_identifier).as_string().rfind("#")) {
					val_copy.set(ID_identifier, val_copy.get(ID_identifier).as_string() + "#0");
					// not sure whether this is needed
					// if (bv.get(val_copy).is_nil())
					//   val_copy.set(ID_identifier, ::diagnostics::internal::to_string(val_copy.get(ID_identifier), "$object#0"));
				}
				
				::exprt val(bv.get(val_copy));
				FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, !val.is_nil(),
						::diagnostics::internal::to_string("Failed to lookup ", val_copy.get(ID_identifier)));
				iv.m_value_str = ::from_expr(m_equation.get_ns(), iv.m_name->get(ID_identifier), val);
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

::std::ostream & Test_Suite_Output::print_test_inputs_plain(::std::ostream & os,
		Test_Suite_Output::Test_Input const& ti) const
{
	bool const print_loc(m_opts.get_bool_option("tco-location"));

	if (print_loc)
		os << "  ENTRY " << ti.m_main_symbol_str << "@[" << ti.m_main_location << "]" << ::std::endl;

	for (Test_Suite_Output::Test_Input::test_inputs_t::const_iterator iter(ti.m_test_inputs.begin()); 
			iter != ti.m_test_inputs.end(); ++iter) {
		os << "  ";
		bool global(false);
		if (iter->m_symbol->type.id() == ID_code) {
			::std::string::size_type pos(iter->m_type_str.find('('));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, pos != ::std::string::npos);
			if (print_loc) os << iter->m_type_str.substr(0, pos);
			os << iter->m_symbol->base_name << iter->m_type_str.substr(pos);
		} else {
			if (print_loc) os << iter->m_type_str << " ";
			os << iter->m_symbol->base_name;
			Symbol_Identifier var(::symbol_exprt(iter->m_symbol->name));
			global = Symbol_Identifier::GLOBAL == var.m_vt;
		}
		if (print_loc)
			os << "@[" << *(iter->m_location) << (global ? " #global":"") << "]";
		os << "=" << iter->m_value_str;
		os << ::std::endl;
	}
	
	return os;
}
	
::std::ostream & Test_Suite_Output::print_function_calls(::std::ostream & os,
		called_functions_t const& cf) const
{
	::boolbvt const& bv(m_equation.get_bv());
	bool const print_loc(m_opts.get_bool_option("tco-location"));

	for (called_functions_t::const_iterator iter(cf.begin()); iter != cf.end(); ++iter) { 
		os << "  ";
		::symbolt const * sym(0);
		m_equation.get_ns().lookup(iter->first.second, sym);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, sym);
		os << sym->base_name << "()";
		if (print_loc)
			os << "@[" << iter->first.first->location << "]";
		if (iter->second) {
			::exprt val(bv.get(*(iter->second)));
			FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, !val.is_nil(),
					::diagnostics::internal::to_string("Failed to lookup ", iter->second->get(ID_identifier)));
			os << "=" << ::from_expr(m_equation.get_ns(), iter->second->get(ID_identifier), val);
		}
		os << ::std::endl;
	}
	
	return os;
}
	
::std::ostream & Test_Suite_Output::print_assignments_to_globals(::std::ostream & os,
		assignments_t const& as) const
{
	::boolbvt const& bv(m_equation.get_bv());
	bool const print_loc(m_opts.get_bool_option("tco-location"));

	for (assignments_t::const_iterator iter(as.begin()); iter != as.end(); ++iter) { 
		os << "  ";
		::symbol_exprt const& lhs((*iter)->ssa_lhs);
		Symbol_Identifier var(lhs);
		::symbolt const * sym(0);
		m_equation.get_ns().lookup(var.m_var_name, sym);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, sym);
		os << sym->base_name;
		if (print_loc)
			os << "@[" << (*iter)->source.pc->location << "]";
		::exprt val(bv.get(lhs));
		FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, !val.is_nil(),
				::diagnostics::internal::to_string("Failed to lookup ", lhs.get(ID_identifier)));
		os << "=" << ::from_expr(m_equation.get_ns(), lhs.get(ID_identifier), val);
		os << ::std::endl;
	}
	
	return os;
}
	
::std::ostream & Test_Suite_Output::print_test_case_xml(::std::ostream & os,
		Test_Suite_Output::Test_Input const& ti, called_functions_t const& cf,
		assignments_t const& as) const
{
	bool const print_loc(m_opts.get_bool_option("tco-location"));

	::xmlt xml_tc("test-case");

	if (print_loc) {
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

		if (print_loc)
			xml_obj.new_element("identifier").data = ::id2string(iter->m_name->get(ID_identifier));
		xml_obj.new_element("base_name").data = ::id2string(iter->m_symbol->base_name);
	
		if (print_loc) {
			::xmlt xml_loc("location");
			::convert(*(iter->m_location), xml_loc);
			xml_obj.new_element().swap(xml_loc);
		}
		
		xml_obj.new_element("value").data = iter->m_value_str;
		if (print_loc)
			xml_obj.new_element("type").data = iter->m_type_str;
		
		xml_tc.new_element().swap(xml_obj);
	}
	
	::boolbvt const& bv(m_equation.get_bv());
	if (m_opts.get_bool_option("tco-called-functions")) {
		for (called_functions_t::const_iterator iter(cf.begin()); iter != cf.end(); ++iter) { 
			::xmlt xml_obj("function-call");
			
			::symbolt const * sym(0);
			m_equation.get_ns().lookup(iter->first.second, sym);
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, sym);
			xml_obj.new_element("base_name").data = ::id2string(sym->base_name);

			if (print_loc) {
				::xmlt xml_loc("location");
				::convert(iter->first.first->location, xml_loc);
				xml_obj.new_element().swap(xml_loc);
			}

			if (iter->second) {
				::exprt val(bv.get(*(iter->second)));
				FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, !val.is_nil(),
						::diagnostics::internal::to_string("Failed to lookup ", iter->second->get(ID_identifier)));
				xml_obj.new_element("return-value").data =
						::from_expr(m_equation.get_ns(), iter->second->get(ID_identifier), val);
			}
		
			xml_tc.new_element().swap(xml_obj);
		}
	}
	
	if (m_opts.get_bool_option("tco-assign-globals")) {
		for (assignments_t::const_iterator iter(as.begin()); iter != as.end(); ++iter) { 
			::xmlt xml_obj("assign-global");
			
			::symbol_exprt const& lhs((*iter)->ssa_lhs);
			Symbol_Identifier var(lhs);
			::symbolt const * sym(0);
			m_equation.get_ns().lookup(var.m_var_name, sym);
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, sym);
			if (print_loc)
				xml_obj.new_element("identifier").data = ::id2string(lhs.get(ID_identifier));
			xml_obj.new_element("base_name").data = ::id2string(sym->base_name);

			if (print_loc) {
				::xmlt xml_loc("location");
				::convert((*iter)->source.pc->location, xml_loc);
				xml_obj.new_element().swap(xml_loc);
			}

			::exprt val(bv.get(lhs));
			FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, !val.is_nil(),
					::diagnostics::internal::to_string("Failed to lookup ", lhs.get(ID_identifier)));
			xml_obj.new_element("value").data =
					::from_expr(m_equation.get_ns(), lhs.get(ID_identifier), val);
		
			xml_tc.new_element().swap(xml_obj);
		}
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
		unsigned const index, ::std::ostream & os, ::ui_message_handlert::uit const ui)
{
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
	
	if (::ui_message_handlert::XML_UI == ui)
		os << "<test-suite index=\"" << index << "\">" << ::std::endl;
	else
		os << "Test Suite " << index << ":" << ::std::endl;
	for (::fshell2::Constraint_Strengthening::test_cases_t::const_iterator iter(
				test_suite.begin()); iter != test_suite.end(); ++iter) {
		cnf.copy_assignment_from(iter->second);
		ti.m_test_inputs.clear();
		called_functions_t cf;
		assignments_t as;
		get_test_case(ti, cf, as);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !cf.empty());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 
				cf.front().first.second == "c::__CPROVER_initialize");
		cf.pop_front();
		switch (ui) {
			case ::ui_message_handlert::XML_UI:
				print_test_case_xml(os, ti, cf, as);
				break;
			case ::ui_message_handlert::PLAIN:
			case ::ui_message_handlert::OLD_GUI:
				os << "IN:" << ::std::endl;
				print_test_inputs_plain(os, ti);
				if (m_opts.get_bool_option("tco-called-functions")) {
					os << "Function calls:" << ::std::endl;
					print_function_calls(os, cf);
				}
				if (m_opts.get_bool_option("tco-assign-globals")) {
					os << "Assignments to global variables:" << ::std::endl;
					print_assignments_to_globals(os, as);
				}
				os << ::std::endl;
				break;
		}
	}
	if (::ui_message_handlert::XML_UI == ui) os << "</test-suite>" << ::std::endl;

	return os;
}


FSHELL2_NAMESPACE_END;

