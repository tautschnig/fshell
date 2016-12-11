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
#include <util/prefix.h>
#include <util/arith_tools.h>
#include <ansi-c/string_constant.h>

// goto-programs/goto_trace.cpp
::std::string counterexample_value_binary(const ::exprt &expr, const ::namespacet &ns);

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

void Symbol_Identifier::strip_SSA_renaming()
{
	// first strip off #xx (level2 renaming)
	string::size_type const hash_start(m_var_name.rfind('#'));
	if (string::npos == hash_start) m_level2 = "";
	else {
		m_level2 = m_var_name.substr(hash_start + 1);
		m_var_name.erase(hash_start);
	}

	// strip off @xx (level1 renaming/frame numbers)
	string::size_type const at_start(m_var_name.rfind('@'));
	if (string::npos == at_start) m_level1 = "";
	else {
		m_level1 = m_var_name.substr(at_start + 1);
		m_var_name.erase(at_start);
	}

	// strip off !xx (level0 renaming/threads)
	string::size_type const exc_start(m_var_name.rfind('!'));
	if (string::npos == exc_start) m_level0 = "";
	else {
		m_level0 = m_var_name.substr(exc_start + 1);
		m_var_name.erase(exc_start);
	}

}

Symbol_Identifier::Symbol_Identifier(::symbol_exprt const& sym) :
	m_identifier(sym.get_identifier()),
	m_vt(UNKNOWN),
	m_var_name(id2string(m_identifier)),
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
	else if (::has_prefix(m_var_name, "__CPROVER_"))
		m_vt = CBMC_INTERNAL;
	else if (::has_prefix(m_var_name, "symex::io::"))
		m_vt = CBMC_INTERNAL;
	// guards
	else if (::has_prefix(m_var_name, "goto_symex::\\guard#"))
		m_vt = CBMC_GUARD;
	// generated symbol
	else if (::has_prefix(m_var_name, "$fshell2$"))
		m_vt = FSHELL2_INTERNAL;

	// level2, level1, level0
	strip_SSA_renaming();

	// strip off $object (added to failed objects)
	string::size_type obj_start(m_var_name.rfind("$object"));
	while (string::npos != obj_start) {
		++m_failed_object_level;
		m_var_name.erase(obj_start);
		obj_start = m_var_name.rfind("$object");
	}

	// name de-mangling done, just work on those variables not yet identified
	if (UNKNOWN != m_vt) return;

	// dynamically allocated memory
	if (::has_prefix(m_var_name, "symex_dynamic::") ||
		::has_prefix(m_var_name, "symex::invalid_object"))
		m_vt = CBMC_DYNAMIC_MEMORY;
	// function call used outside assignment
	else if (string::npos != m_var_name.find("::$tmp::return_value_"))
		m_vt = CBMC_TMP_RETURN_VALUE;
	// parameters or other local variables
	else if (string::npos != m_var_name.find("::")) {
		// parameters have one more ::
		if (string::npos == m_var_name.find("::", m_var_name.find("::") + 2))
			m_vt = PARAMETER;
		// local vars have two more ::, static ones have no @ marking the frame
		else
			m_vt = (m_level1.empty()) ? LOCAL_STATIC : LOCAL;
	}
	// global variables only have no thread and no frame
	else if (m_level0.empty() && m_level1.empty())
		m_vt = GLOBAL;
	
	FSHELL2_AUDIT_ASSERT1(::diagnostics::Not_Implemented, UNKNOWN != m_vt,
			::std::string("Cannot determine variable type of ") + id2string(m_identifier));
}
		
Test_Suite_Output::Test_Input::Test_Input(::symbolt const& main_sym,
		::std::string const& main_str, ::locationt const& main_loc) :
	m_main_symbol(main_sym),
	m_main_symbol_str(main_str), 
	m_main_location(main_loc) {
}

void Test_Suite_Output::update_call_stack(
  ::symex_target_equationt::SSA_stept const& step,
  called_functions_t &calls,
  call_stack_t &call_stack) const
{
	if(::fshell2::instrumentation::GOTO_Transformation::is_instrumented( step.source.pc) ||
	   step.is_function_return() || /* we figure these out ourselves */
	   step.source.pc->is_end_function() ||
	   (step.is_assignment() && ::symex_targett::STATE != step.assignment_type))
		return;

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !step.source.pc->function.empty());

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 step.source.pc->function == ID__start || !call_stack.empty());
	if (!call_stack.empty() && step.source.pc != call_stack.back()->first.first) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
							 call_stack.back()->first.first->is_function_call() ||
							 call_stack.back()->first.first->is_location());

		::irep_idt const fname(call_stack.back()->first.second);
		if (step.source.pc->function != fname) {
			FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
				"POP ", call_stack.back()->first.second));
			call_stack.pop_back();
			if (step.source.pc->function != ID__start) {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !call_stack.empty());
				// handle sequences of functions without explicit return
				while (step.source.pc->function != call_stack.back()->first.second) {
					FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
						"Want POP-more because of ", call_stack.back()->first.second,
						" vs. current pc: ", step.source.pc->function));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !call_stack.empty());
					call_stack.pop_back();
					FSHELL2_AUDIT_TRACE("POP-more");
				}
			}
		}
	}

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 !step.source.pc->is_end_function());
	::goto_programt::const_targett next(step.source.pc);
	++next;
	FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
		"this func: ", step.source.pc->function, " next func: ", next->function));
	// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, next);
	if (step.is_location() && step.source.pc->is_function_call()) {
		calls.push_back(::std::make_pair< ::std::pair< ::goto_programt::const_targett,
						::irep_idt const >, ::exprt const* >(::std::make_pair(step.source.pc,
																			  ::to_code_function_call(step.source.pc->code).function().get(ID_identifier)), 0));
		call_stack.push_back(calls.end());
		--call_stack.back();
		FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
			"PUSH ", call_stack.back()->first.second));
	} else if (step.is_location() &&
			   step.source.pc->function != next->function) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
							 !call_stack.back()->first.first->is_location() || call_stack.size() >= 2);
		// inlined function
		if (call_stack.back()->first.first->is_location() &&
			(*--(--call_stack.end()))->first.second == next->function) {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == call_stack.back()->second);
			call_stack.pop_back();
			FSHELL2_AUDIT_TRACE("POP-inline");
		} else {
			calls.push_back(::std::make_pair< ::std::pair< ::goto_programt::const_targett,
							::irep_idt const >, ::exprt const* >(::std::make_pair(step.source.pc,
																				  next->function), 0));
			call_stack.push_back(calls.end());
			--call_stack.back();
			FSHELL2_AUDIT_TRACE("PUSH-inline");
		}
	} else if (step.is_assignment() && step.source.pc->is_return()) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !call_stack.empty());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == call_stack.back()->second);
		call_stack.back()->second = &(step.ssa_lhs);
		// required for recursive functions
		call_stack.pop_back();
		FSHELL2_AUDIT_TRACE("POP-return");
	}
}

void Test_Suite_Output::analyze_symbol(
  ::symex_target_equationt::SSA_stept const& step,
  ::symbol_exprt const& sym,
  ::exprt const* parent,
  nondet_syms_in_rhs_t const& nondet_syms_in_rhs,
  seen_vars_t & vars,
  Test_Input & ti,
  assignments_t & global_assign) const
{
	bool is_lhs(&sym == &(step.ssa_lhs));

	Test_Input::program_variable_t iv;
	::exprt const * nondet_expr(0);

	::boolbvt const& bv(m_equation.get_bv());

	Symbol_Identifier var(sym);
	const std::string l1_name=var.m_var_name+"!"+var.m_level0+"@"+var.m_level1;
	std::set< ::irep_idt > &vars_l2=
		vars.insert(std::make_pair(l1_name, std::set< ::irep_idt >())).first->second;
	//// ::std::cerr << "Analyzing " << var.m_identifier << "(" << var.m_var_name << ") [" << var.m_vt << "]" << ::std::endl;

	switch (var.m_vt)
	{
		case Symbol_Identifier::UNKNOWN:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
			return;
		case Symbol_Identifier::CBMC_INTERNAL: // we don't care
		case Symbol_Identifier::CBMC_GUARD: // we don't care
		case Symbol_Identifier::FSHELL2_INTERNAL: // we don't care
			vars_l2.insert(var.m_level2);
			return;
		case Symbol_Identifier::CBMC_DYNAMIC_MEMORY: // ignore in RHS
		case Symbol_Identifier::CBMC_TMP_RETURN_VALUE: // ignore in RHS
			if (!is_lhs)
			{
				vars_l2.insert(var.m_level2);
				return;
			}
		case Symbol_Identifier::PARAMETER:
		case Symbol_Identifier::LOCAL:
		case Symbol_Identifier::LOCAL_STATIC:
		case Symbol_Identifier::GLOBAL:
			if (is_lhs &&
				::symex_targett::STATE == step.assignment_type &&
				Symbol_Identifier::GLOBAL == var.m_vt)
				global_assign.push_back(&step);

			if (is_lhs &&
				::symex_targett::STATE == step.assignment_type &&
				step.ssa_rhs.id() == ID_with &&
				!nondet_syms_in_rhs.empty())
			{
				// assignment to array element with nondet components
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
									 vars_l2.end() == vars_l2.find(var.m_level2));
				vars_l2.insert(var.m_level2);
				// only a single one of them
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						1 == nondet_syms_in_rhs.size());
				nondet_expr = nondet_syms_in_rhs.front()->first;
			}
			else if (is_lhs &&
					 ::symex_targett::STATE == step.assignment_type &&
					 step.ssa_rhs.id() == ID_struct &&
					 !nondet_syms_in_rhs.empty())
			{
				// assignment to struct variable with nondet components
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
									 vars_l2.end() == vars_l2.find(var.m_level2));
				vars_l2.insert(var.m_level2);
				// only a single one of them
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						1 == nondet_syms_in_rhs.size());
				nondet_expr = nondet_syms_in_rhs.front()->first;
			}
			else if (is_lhs &&
					 ::symex_targett::STATE == step.assignment_type &&
					 step.ssa_rhs.id() == ID_nondet_symbol)
			{
				// assignment to simple variable using undef function
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
									 vars_l2.end() == vars_l2.find(var.m_level2));
				vars_l2.insert(var.m_level2);
				nondet_expr = &sym;
				// ::std::cerr << "Added LHS var " << var.m_identifier << " [" << var.m_vt << "]" << ::std::endl;
				/*if (Symbol_Identifier::CBMC_TMP_RETURN_VALUE != var.m_vt)
					vars.insert(var.m_var_name + "@" + var.m_level1);*/
				// ::std::cerr << "Also added var " << (var.m_var_name + "@" + var.m_level1) << " [" << var.m_vt << "]" << ::std::endl;
			}
			else if (!is_lhs &&
					 var.m_level2 == "1" &&
					 vars_l2.empty())
			{
				// reading use of undefined variable
				/*if (!vars.insert(var.m_identifier).second)
					return;*/
				// ::std::cerr << "Added #0 var " << var.m_identifier << " [" << var.m_vt << "]" << ::std::endl;
				//vars.insert(var.m_var_name + "@" + var.m_level1);
				vars_l2.insert(var.m_level2);
				// ::std::cerr << "Also added var " << (var.m_var_name + "@" + var.m_level1) << " [" << var.m_vt << "]" << ::std::endl;
			}
			else
			{
				const bool first=vars_l2.empty();
				vars_l2.insert(var.m_level2);
				if(!first)
					return;
				// ::std::cerr << "Added var " << (var.m_var_name + "@" + var.m_level1) << " [" << var.m_vt << "]" << ::std::endl;
			}
			break;
	}

	// select the init procedure chosen by the user
	::std::string const start_proc_prefix(config.main + "::");

	if (Symbol_Identifier::PARAMETER != var.m_vt &&
		nondet_expr &&
		step.source.pc->is_function_call())
	{
		::code_function_callt const& fct(::to_code_function_call(step.source.pc->code));
		iv.m_name = &(fct.function());
		iv.m_pretty_name = fct.function().get(ID_identifier);
		iv.m_value = nondet_expr;
		iv.m_symbol = 0;
		m_equation.get_ns().lookup(iv.m_pretty_name, iv.m_symbol);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iv.m_symbol);
		iv.m_location = &(step.source.pc->location);
		// remove return values of defined (but inlined) functions
		if (Symbol_Identifier::CBMC_TMP_RETURN_VALUE == var.m_vt && !iv.m_symbol->value.is_nil())
			return;
		/* we assume malloc doesn't return NULL, should check for
		 * invalid PTR
		 } else if (step.source.pc->is_assign() &&
		 ::to_code_assign(step.source.pc->code).ssa_rhs().get("statement") == "malloc") {
		 iv_in_use = true;
		 iv.m_name = &(step.ssh_full_lhs);
		 iv.m_pretty_name = "malloc()";
		 iv.m_location = &(step.source.pc->location);
		 iv.m_value = &(step.ssa_rhs);
		 ::std::cerr << "LHS: " << step.ssa_lhs << ::std::endl;
		 ::std::cerr << "ORIG_LHS: " << step.original_full_lhs << ::std::endl;
		 ::std::cerr << "RHS: " << step.ssa_rhs << ::std::endl; */
	}
	else if (!is_lhs || Symbol_Identifier::PARAMETER == var.m_vt)
	{
		if (Symbol_Identifier::PARAMETER == var.m_vt &&
			!::has_prefix(var.m_var_name, start_proc_prefix))
			return;
		// it may happen that the value is not even known to the
		// mapping, which happens for the following code (and
		// variable s):
		//   char * buffer;
		//   unsigned s;
		//   buffer=(char*)malloc(s);
		if (!is_lhs && step.original_full_lhs.get(ID_identifier) == "__CPROVER_alloc_size" &&
			bv.get(sym).is_nil())
			return;
		iv.m_name = &sym;
		// @ comes before #, also strip off possible $object
		// (failed symbols)
		iv.m_pretty_name = var.m_var_name;
		iv.m_value = &sym;
		iv.m_symbol = 0;
		m_equation.get_ns().lookup(iv.m_pretty_name, iv.m_symbol);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, iv.m_symbol);
		iv.m_location = &(iv.m_symbol->location);
	}
	else
		return;

	//// ::std::cerr << "Test input: " << iv.m_pretty_name << ::std::endl;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !iv.m_location->is_nil());
	iv.m_type_str = ::from_type(m_equation.get_ns(), iv.m_name->get(ID_identifier), iv.m_symbol->type);

	// obtain the value; if variable was used in pointer context, it will
	// lack #0; _Bool is handled by prop_convt, no entry in mapping
	//// ::std::cerr << "Fetching value for " << iv.m_value->pretty() << ::std::endl;
	::exprt val_copy(*iv.m_value);
	if (ID_bool != iv.m_symbol->type.id() &&
		val_copy.id() == ID_symbol)
	{
		Symbol_Identifier vaL_sym(to_symbol_expr(val_copy));
		if(vaL_sym.m_level2=="-1")
		{
			seen_vars_t::const_iterator entry=
				vars.find(vaL_sym.m_var_name+"!"+vaL_sym.m_level0+"@"+vaL_sym.m_level1);
			if(entry!=vars.end() &&
			   !entry->second.empty())
				val_copy.set(
				  ID_identifier,
				  val_copy.get_string(ID_identifier) + "#" + id2string(*(entry->second.rbegin())));
		// not sure whether this is needed
		// if (bv.get(val_copy).is_nil())
		//   val_copy.set(ID_identifier, ::diagnostics::internal::to_string(val_copy.get(ID_identifier), "$object#0"));
		}
	}
	
	iv.m_value_expr=bv.get(val_copy);
	FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, iv.m_value_expr.is_not_nil(),
			::diagnostics::internal::to_string("Failed to lookup ", val_copy.get(ID_identifier)));
	if(::from_expr(m_equation.get_ns(), iv.m_name->get(ID_identifier), iv.m_value_expr)=="argv'")
		beautify_argv(iv.m_value_expr, iv.m_name->get(ID_identifier), vars);
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
		
	ti.m_test_inputs.push_back(iv);
}

void Test_Suite_Output::beautify_argv(
  exprt &val,
  irep_idt const& identifier,
  seen_vars_t const& vars) const
{
	if(val.id() == ID_constant &&
	   val.has_operands() &&
	   val.type().id() == ID_pointer &&
	   val.op0().id() == ID_address_of &&
	   to_address_of_expr(val.op0()).object().id() == ID_index &&
	   to_index_expr(to_address_of_expr(val.op0()).object()).index().is_constant() &&
	   to_index_expr(to_address_of_expr(val.op0()).object()).array().id()==ID_symbol)
	{
		index_exprt val_copy=to_index_expr(to_address_of_expr(val.op0()).object());
		exprt val_a=val_copy.array();
		Symbol_Identifier val_sym(to_symbol_expr(val_a));
		seen_vars_t::const_iterator entry=
			vars.find(val_sym.m_var_name+"!"+val_sym.m_level0+"@"+val_sym.m_level1);
		if(entry!=vars.end() &&
		   !entry->second.empty())
		{
			val_a.set(
			  ID_identifier,
			  val_a.get_string(ID_identifier) + "#" + id2string(*(entry->second.rbegin())));

			val=m_equation.get_bv().get(val_a);
			if(val==val_a)
			{
				val=constant_exprt(ID_NULL, val_copy.type());
				return;
			}

			mp_integer offset=-1;
			if(val.id()==ID_array)
			{
				Forall_operands(it, val)
					beautify_argv(*it, identifier, vars);

				to_integer(val_copy.index(), offset);
				while(offset > 0 && offset < val.operands().size())
				{
					val.operands().erase(
					  val.operands().begin());
					--offset;
				}

				string_constantt str;
				if(!str.from_array_expr(to_array_expr(val)))
				{
					// use the shortest zero-terminated substring
					std::string str_val=id2string(str.get_value());
					str.set_value(std::string(str_val, 0, str_val.find((char)0)));
					val=str;
				}
			}

			if(val.id()==ID_array && offset != 0)
				val=index_exprt(val, val_copy.index());
		}
	}
}

void Test_Suite_Output::analyze_symbols(
  ::symex_target_equationt::SSA_stept const& step,
  seen_vars_t & vars,
  Test_Input & ti,
  assignments_t & global_assign) const
{
	stmt_vars_and_parents_t stmt_vars_and_parents;
	nondet_syms_in_rhs_t nondet_syms_in_rhs;
	::fshell2::instrumentation::collect_expr_with_parents(step.ssa_rhs, stmt_vars_and_parents);

	for (stmt_vars_and_parents_t::const_iterator
		 v_iter(stmt_vars_and_parents.begin());
		 v_iter != stmt_vars_and_parents.end(); ++v_iter)
		if (ID_nondet_symbol == v_iter->first->id())
			nondet_syms_in_rhs.push_back(v_iter);

	// RHS first
	for (stmt_vars_and_parents_t::const_iterator
		 v_iter(stmt_vars_and_parents.begin());
		 v_iter != stmt_vars_and_parents.end(); ++v_iter)
	{
		if (v_iter->first->id() != ID_symbol)
			continue;

		// we don't care about variables where only the address of some other
		// object is taken
		bool has_address_of(false);
		stmt_vars_and_parents_t::const_iterator p_iter(v_iter);
		while (p_iter != stmt_vars_and_parents.end() && p_iter->second && !has_address_of) {
			has_address_of = (ID_address_of == p_iter->second->id());
			p_iter = stmt_vars_and_parents.find(p_iter->second);
		}

		if (!has_address_of)
			analyze_symbol(
			  step,
			  ::to_symbol_expr(*(v_iter->first)),
			  v_iter->second,
			  nondet_syms_in_rhs,
			  vars,
			  ti,
			  global_assign);
	}

	// LHS
	analyze_symbol(
	  step,
	  step.ssa_lhs,
	  0,
	  nondet_syms_in_rhs,
	  vars,
	  ti,
	  global_assign);
}

void Test_Suite_Output::do_step(
  ::symex_target_equationt::SSA_stept const& step,
  seen_vars_t &vars,
  called_functions_t &calls,
  call_stack_t &call_stack,
  bool &most_recent_non_hidden_is_true,
  Test_Input & ti,
  assignments_t & global_assign) const
{
	//// ::goto_programt tmp;
	//// ::std::cerr << "#########################################################" << ::std::endl;
	//// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, step.source.pc);
	//// step.output(m_equation.get_ns(), ::std::cerr);
	//// ::std::cerr << "LHS: " << step.ssa_lhs << ::std::endl;
	//// ::std::cerr << "ORIG_LHS: " << step.original_full_lhs << ::std::endl;
	//// ::std::cerr << "RHS: " << step.ssa_rhs << ::std::endl;

	::cnf_clause_list_assignmentt const& cnf(m_equation.get_cnf());
	bool const instr_enabled(step.guard.is_true() || cnf.l_get(step.guard_literal) == ::tvt(true));
	//// std::cerr << "ENABLED? " << instr_enabled << std::endl;

	if (instr_enabled)
		update_call_stack(step, calls, call_stack);

	if (!step.is_assignment() || ::symex_targett::STATE == step.assignment_type)
		most_recent_non_hidden_is_true = instr_enabled;
	if (most_recent_non_hidden_is_true && step.is_assignment())
		analyze_symbols(step, vars, ti, global_assign);
}

void Test_Suite_Output::get_test_case(Test_Input & ti, called_functions_t & calls,
			assignments_t & global_assign) const
{
	// beautify pointers
	// ::std::map< ::std::string, ::std::string > beautified_name;

	//// ::boolbvt const& bv(m_equation.get_bv());
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
	seen_vars_t vars;

	// track calls and returns
	call_stack_t call_stack;

	bool most_recent_non_hidden_is_true(false);
	// does a DEF-USE + call stack analysis
	::symex_target_equationt::SSA_stepst const& equation(m_equation.get_equation().SSA_steps);
	for (::symex_target_equationt::SSA_stepst::const_iterator iter(
				equation.begin() ); iter != equation.end(); ++iter)
		do_step(
		  *iter,
		  vars,
		  calls,
		  call_stack,
		  most_recent_non_hidden_is_true,
		  ti,
		  global_assign);
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
		os << "=" << ::from_expr(m_equation.get_ns(), iter->m_name->get(ID_identifier), iter->m_value_expr);
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
		
		xml_obj.new_element("value").data =
			::from_expr(m_equation.get_ns(), iter->m_name->get(ID_identifier), iter->m_value_expr);
		xml_obj.new_element("binary-value").data =
			::counterexample_value_binary(iter->m_value_expr, m_equation.get_ns());
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
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !config.main.empty());
	::symbolt const& main_symb(m_equation.get_ns().lookup(config.main));
	::code_typet::parameterst const &parameters(::to_code_type(main_symb.type).parameters());
	::std::ostringstream main_symb_str;
	main_symb_str << main_symb.base_name << "(";
	for (::code_typet::parameterst::const_iterator iter(parameters.begin());
			iter != parameters.end(); ++iter) {
		if (iter != parameters.begin()) main_symb_str << ",";
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
				cf.front().first.second == "__CPROVER_initialize");
		cf.pop_front();
		switch (ui) {
			case ::ui_message_handlert::XML_UI:
				print_test_case_xml(os, ti, cf, as);
				break;
			case ::ui_message_handlert::PLAIN:
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

