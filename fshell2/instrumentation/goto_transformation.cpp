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

/*! \file fshell2/instrumentation/goto_transformation.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:50:49 CEST 2009 
*/

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/invalid_argument.hpp>

#include <fshell2/exception/instrumentation_error.hpp>
#include <fshell2/instrumentation/goto_utils.hpp>

#include <cbmc/src/util/std_expr.h>
#include <cbmc/src/util/expr_util.h>
#include <cbmc/src/ansi-c/ansi_c_typecheck.h>
#include <cbmc/src/ansi-c/expr2c.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

	
GOTO_Transformation::GOTO_Transformation(::goto_functionst & gf) :
	m_goto(gf), m_nondet_var_count(-1) {
}

void GOTO_Transformation::set_annotations(::goto_programt::const_targett src, ::goto_programt & target) {
	for (::goto_programt::targett iter(target.instructions.begin());
			iter != target.instructions.end(); ++iter) {
		iter->function = src->function;
		iter->location = src->location;
	}
	mark_instrumented(target);
}

void GOTO_Transformation::mark_instrumented(::goto_programt::targett inst) {
	inst->location.set_property("fshell2_instrumentation");
}

void GOTO_Transformation::mark_instrumented(::goto_programt & target) {
	for (::goto_programt::targett iter(target.instructions.begin());
			iter != target.instructions.end(); ++iter)
		mark_instrumented(iter);
}

bool GOTO_Transformation::is_instrumented(::goto_programt::const_targett inst) {
	return (inst->location.get_property() == "fshell2_instrumentation");
}

GOTO_Transformation::inserted_t const& GOTO_Transformation::insert(::std::string const& f,
		GOTO_Transformation::position_t const pos,
		::goto_program_instruction_typet const stmt_type, ::goto_programt const& prg) {
	::goto_functionst::function_mapt::iterator entry(m_goto.function_map.find(f));
	FSHELL2_DEBUG_CHECK1(::fshell2::Instrumentation_Error, entry != m_goto.function_map.end(),
			"Function " + f + " not found in goto program");
	FSHELL2_DEBUG_CHECK1(::fshell2::Instrumentation_Error, !entry->second.body.empty(),
			"Body of function " + f + " not available");
	FSHELL2_DEBUG_CHECK1(::fshell2::Instrumentation_Error, !entry->second.is_inlined(),
			"Cannot instrument inlined functions");
	
	inserted_t collector;
	for (::goto_programt::targett iter(entry->second.body.instructions.begin());
			iter != entry->second.body.instructions.end(); ++iter) {
		if (iter->type == stmt_type) {
			::goto_programt tmp;
			tmp.copy_from(prg);
			switch (pos) {
				case BEFORE:
					insert_at(::std::make_pair(&(entry->second.body), iter), tmp);
					if (!m_inserted.empty()) {
						iter = m_inserted.back().second;
						++iter;
					}
					break;
				case AFTER:
					{
						::goto_programt::targett next(iter);
						++next;
						insert_at(::std::make_pair(&(entry->second.body), next), tmp);
						if (!m_inserted.empty())
							iter = m_inserted.back().second;
					}
					break;
			}
			collector.insert(collector.end(), m_inserted.begin(), m_inserted.end());
		}
	}

	m_inserted.swap(collector);
	return m_inserted;
}
	
GOTO_Transformation::inserted_t const& GOTO_Transformation::insert_at(
		goto_node_t const& node, ::goto_programt & prg) {
	m_inserted.clear();
	if (prg.instructions.empty()) return m_inserted;
	
	if (node.second->is_target()) {
		for (::std::set< ::goto_programt::targett >::iterator iter(node.second->incoming_edges.begin());
				iter != node.second->incoming_edges.end(); ++iter) {
			for (::goto_programt::instructiont::targetst::iterator t_iter((*iter)->targets.begin());
					t_iter != (*iter)->targets.end(); ) {
				if (*t_iter == node.second) {
					(*iter)->targets.insert(t_iter, prg.instructions.begin());
					::goto_programt::instructiont::targetst::iterator t_iter_bak(t_iter);
					++t_iter;
					(*iter)->targets.erase(t_iter_bak);
				} else {
					++t_iter;
				}
			}
		}
	}
	set_annotations(node.second, prg);
	node.first->destructive_insert(node.second, prg);
	::goto_programt::targett pred(node.second);
	pred--;
	m_inserted.push_back(::std::make_pair(node.first, pred));
	node.first->update();

	return m_inserted;
}
	
GOTO_Transformation::inserted_t const& GOTO_Transformation::insert_predicate_at(
		goto_node_t const& node, ::exprt const* pred, ::contextt & context) {
	m_inserted.clear();

	::namespacet const ns(context);
	::exprt pred_copy(*pred);
	::std::list< ::exprt * > symbols;
	find_symbols(pred_copy, symbols);
	bool make_nondet(false);
	for (::std::list< ::exprt * >::iterator iter(symbols.begin()); iter != symbols.end(); ++iter) {
		::std::list< ::exprt > alt_names;
		// check globals first
		::goto_functionst::function_mapt::iterator main_entry(m_goto.function_map.find("main"));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, main_entry != m_goto.function_map.end());
		for (::goto_programt::instructionst::const_iterator f_iter(main_entry->second.body.instructions.begin());
				f_iter != main_entry->second.body.instructions.end(); ++f_iter) {
			if (f_iter->is_other() && ::to_code(f_iter->code).get_statement() == "decl") {
				::std::list< ::exprt const* > decl_symbols;
				find_symbols(f_iter->code, decl_symbols);
				for (::std::list< ::exprt const* >::const_iterator s_iter(decl_symbols.begin());
						s_iter != decl_symbols.end(); ++s_iter) {
					::symbolt const& symb(ns.lookup((*s_iter)->get("identifier")));
					if ((*iter)->get("identifier") == symb.base_name) alt_names.push_back(**s_iter);
				}
			}
		}
		// function arguments
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !node.first->instructions.empty());
		::goto_functionst::function_mapt::iterator fct_entry(m_goto.function_map.find(node.first->instructions.front().function));
		FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, fct_entry != m_goto.function_map.end(),
				::diagnostics::internal::to_string("Function ",
					node.first->instructions.front().function, " not found in map!"));
		::code_typet::argumentst const& argument_types(fct_entry->second.type.arguments());
		for (::code_typet::argumentst::const_iterator a_iter(argument_types.begin());
				a_iter != argument_types.end(); ++a_iter) {
			::symbolt const& symb(ns.lookup(a_iter->get_identifier()));
			if ((*iter)->get("identifier") == symb.base_name) alt_names.push_back(::symbol_expr(symb));
		}
		// local variables
		for (::goto_programt::instructionst::const_iterator f_iter(node.first->instructions.begin());
				f_iter != node.second; ++f_iter) {
			if (f_iter->is_other() && ::to_code(f_iter->code).get_statement() == "decl") {
				::std::list< ::exprt const* > decl_symbols;
				find_symbols(f_iter->code, decl_symbols);
				for (::std::list< ::exprt const* >::const_iterator s_iter(decl_symbols.begin());
						s_iter != decl_symbols.end(); ++s_iter) {
					::symbolt const& symb(ns.lookup((*s_iter)->get("identifier")));
					if ((*iter)->get("identifier") == symb.base_name) alt_names.push_back(**s_iter);
				}
			}
		}

		// rename, if possible
		if (alt_names.empty()) {
			make_nondet = true;
			break;
		}
		(*iter)->set("identifier", alt_names.back().get("identifier"));
	}
	::goto_programt tmp;

	// predicate may be invalid
	if (make_nondet) {
		++m_nondet_var_count; FSHELL2_PROD_CHECK1(Instrumentation_Error,
				m_nondet_var_count >= 0, "Too many nondet choices required");
		::std::string const var_name(::diagnostics::internal::to_string("c::!fshell2!nd_choice", m_nondet_var_count));
		{
			::symbolt cond_symbol;
			cond_symbol.mode = "C";
			cond_symbol.name = var_name;
			cond_symbol.base_name = var_name.substr(3, ::std::string::npos);
			cond_symbol.type = ::typet("bool");
			cond_symbol.lvalue = true;
			cond_symbol.static_lifetime = false;
			context.move(cond_symbol);
		}
		::symbolst::const_iterator symb_entry(context.symbols.find(var_name));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symb_entry != context.symbols.end());
		::symbolt const& cond_symbol(symb_entry->second);
		::goto_programt::targett decl(tmp.add_instruction());
		decl->make_other();
		decl->code = ::code_declt(::symbol_expr(cond_symbol));
		pred_copy = ::symbol_expr(cond_symbol);
	} else {
		FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Trying to typecheck: ", pred_copy.pretty()));
		::ansi_c_parse_treet ansi_c_parse_tree;
		::stream_message_handlert mh(::std::cerr);
		::ansi_c_typecheckt ct(ansi_c_parse_tree, context, "", mh);
		ct.typecheck_expr(pred_copy);
		FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Typecheck completed: ", pred_copy.pretty()));
		if (pred_copy.type().id() == "code" && 1 == pred_copy.operands().size()) {
			::exprt pred_bak(pred_copy.op0());
			pred_copy.swap(pred_bak);
		}
		FSHELL2_PROD_CHECK1(Instrumentation_Error, pred_copy.type().id() == "bool",
				::diagnostics::internal::to_string("Predicate ", ::expr2c(*pred, ns), " is not of Boolean type"));
	}


	::goto_programt::targett if_stmt(tmp.add_instruction());
	::goto_programt::targett loc(tmp.add_instruction(LOCATION));
	::goto_programt::targett dest(tmp.add_instruction(SKIP));
	if_stmt->make_goto(dest, pred_copy);
	if_stmt->guard.make_not();

	insert_at(node, tmp);
	m_inserted.clear();
	m_inserted.push_back(::std::make_pair(node.first, loc));

	return m_inserted;
}
	
GOTO_Transformation::inserted_t & GOTO_Transformation::make_nondet_choice(::goto_programt & dest, int const num,
		::contextt & context) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, num >= 1);
	m_inserted.clear();
	// count up until n-1, each bit sequence determines a boolean
	// combination, jump to end after call
	// compute round_up(log2(num)); can be done faster, but never mind
	int log_2_rounded(0);
	int num_options(num);
	while (num_options >>= 1) ++log_2_rounded;
	if (num > (1 << log_2_rounded)) ++log_2_rounded;

	::goto_programt tmp_target;
	::goto_programt::targett out_target(tmp_target.add_instruction(SKIP));
	
	if (num > 1) {
		++m_nondet_var_count;
		FSHELL2_PROD_CHECK1(Instrumentation_Error, m_nondet_var_count >= 0, "Too many nondet choices required");
	}

	for (int i(0); i < num - 1; ++i) {
		int bv(i);
		::exprt * full_cond(0);
		for (int j(0); j < log_2_rounded; ++j) {
			bool pos(bv & 0x1);
			bv >>= 1;
			::std::string const var_name(::diagnostics::internal::to_string("c::!fshell2!nd_choice", m_nondet_var_count, "$", j));
			if (0 == i) {
				::symbolt cond_symbol;
				cond_symbol.mode = "C";
				cond_symbol.name = var_name;
				cond_symbol.base_name = var_name.substr(3, ::std::string::npos);
				cond_symbol.type = ::typet("bool");
				cond_symbol.lvalue = true;
				cond_symbol.static_lifetime = false;
				context.move(cond_symbol);
			}
			::symbolst::const_iterator symb_entry(context.symbols.find(var_name));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symb_entry != context.symbols.end());
			::symbolt const& cond_symbol(symb_entry->second);
			if (0 == i) {
				::goto_programt::targett decl(dest.add_instruction());
				decl->make_other();
				decl->code = ::code_declt(::symbol_expr(cond_symbol));
			}
			::exprt cond(::symbol_expr(cond_symbol));
			if (!pos) cond.make_not();
			if (0 == j) {
				full_cond = new ::exprt(cond);
			} else {
				::exprt * full_cond_bak(full_cond);
				full_cond = new ::and_exprt(*full_cond, cond);
				delete full_cond_bak;
			}
		}
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, full_cond != 0);

		::goto_programt::targett if_stmt(dest.add_instruction());

		::goto_programt::targett insert_later(dest.add_instruction());
		m_inserted.push_back(::std::make_pair(&dest, insert_later));

		::goto_programt::targett goto_stmt(dest.add_instruction());
		goto_stmt->make_goto(out_target);
		goto_stmt->guard.make_true();

		::goto_programt::targett others_target(dest.add_instruction(SKIP));
		if_stmt->make_goto(others_target);
		if_stmt->guard = *full_cond;
		delete full_cond; full_cond = 0;
		if_stmt->guard.make_not();
	}

	::goto_programt::targett insert_later(dest.add_instruction());
	m_inserted.push_back(::std::make_pair(&dest, insert_later));

	if (num > 1) dest.destructive_append(tmp_target);
	
	dest.update();

	return m_inserted;
}
	
FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

