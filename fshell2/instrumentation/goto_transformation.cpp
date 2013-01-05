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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#endif

#include <fshell2/exception/instrumentation_error.hpp>
#include <fshell2/instrumentation/goto_utils.hpp>

#include <util/std_expr.h>
#include <util/expr_util.h>
#include <langapi/language_ui.h>
#include <ansi-c/ansi_c_typecheck.h>
#include <ansi-c/expr2c.h>
#include <goto-programs/goto_convert.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

	
GOTO_Transformation::GOTO_Transformation(::language_uit & manager, ::goto_functionst & gf) :
	m_manager(manager), m_goto(gf) {
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

bool GOTO_Transformation::is_goto_edge(goto_edge_t const& edge) {
	if (!edge.first.second->is_goto()) return false;
	if (edge.first.second->guard.is_true()) return true;
	// if it is a backward edge then it must be the goto edge; check
	// the other case
	if (edge.first.second->location_number >
			edge.second.second->location_number) return true;
	// a forward goto; if there are only newly instrumented edges
	// between source and target of this edge, it cannot be the goto
	// edge (given that the guard is not trivially true)
	bool instrumentation_only(true);
	::goto_programt::const_targett iter(edge.first.second);
	for (++iter; iter != edge.second.second && instrumentation_only; ++iter)
		instrumentation_only = is_instrumented(iter);
	return !instrumentation_only;
}
	
void GOTO_Transformation::make_function_call(::goto_programt::targett ins,
			::std::string const& func_name) {
	ins->type = FUNCTION_CALL;
	::code_function_callt fct;
	fct.function() = ::symbol_exprt(func_name);
	ins->code = fct;
}
	
::symbolt const& GOTO_Transformation::new_var(::std::string const& name,
		::typet const& type, bool global) {
	::std::string const var_name("c::" + name);
	::contextt::symbolst::const_iterator symb_entry(m_manager.context.symbols.find(var_name));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symb_entry == m_manager.context.symbols.end());
	
	::symbolt symbol;
	symbol.mode = ID_C;
	symbol.name = var_name;
	symbol.base_name = name;
	symbol.type = type;
	symbol.is_lvalue = true;
	symbol.is_static_lifetime = global;
	m_manager.context.move(symbol);
	
	symb_entry = m_manager.context.symbols.find(var_name);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symb_entry != m_manager.context.symbols.end());
	return symb_entry->second;
}
	
::symbolt const& GOTO_Transformation::new_bool_var(char const* name) {
	static int var_count(-1);
	++var_count;
	FSHELL2_PROD_CHECK1(Instrumentation_Error, var_count >= 0, "Too many nondet choices required");
	::std::ostringstream var_name;
	var_name << "$fshell2$" << name << var_count;
	return new_var(var_name.str(), ::bool_typet(), false);
}

GOTO_Transformation::inserted_t const& GOTO_Transformation::insert(::irep_idt const& f,
		GOTO_Transformation::position_t const pos,
		::goto_program_instruction_typet const stmt_type, ::goto_programt const& prg) {
	::goto_functionst::function_mapt::iterator entry(m_goto.function_map.find(f));
	FSHELL2_DEBUG_CHECK1(::fshell2::Instrumentation_Error, entry != m_goto.function_map.end(),
			"Function " + id2string(f) + " not found in goto program");
	FSHELL2_DEBUG_CHECK1(::fshell2::Instrumentation_Error, !entry->second.body.empty(),
			"Body of function " + id2string(f) + " not available");
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
	// run update on all goto functions for consistent location numbers
	m_goto.update();

	return m_inserted;
}
	
GOTO_Transformation::inserted_t const& GOTO_Transformation::insert_predicate_at(
		goto_node_t const& node, ::exprt const* pred) {
	m_inserted.clear();

	::namespacet const ns(m_manager.context);
	::exprt pred_copy(*pred);
	::std::list< ::exprt * > symbols;
	find_symbols(pred_copy, symbols);
	bool make_nondet(false);
	::exprt * special_pred_symb(0);
	for (::std::list< ::exprt * >::iterator iter(symbols.begin()); iter != symbols.end(); ++iter) {
		if ((*iter)->get(ID_identifier) == "!PRED!") {
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == special_pred_symb);
			special_pred_symb = *iter;
			continue;
		}

		::std::list< ::exprt > alt_names;
		::goto_functionst::function_mapt::iterator fct_entry(m_goto.function_map.end());
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !node.first->instructions.empty());
		// walk the function map to check for matching function names
		for (::goto_functionst::function_mapt::iterator fmap_entry(m_goto.function_map.begin());
				fmap_entry != m_goto.function_map.end(); ++fmap_entry) {
			if (fmap_entry->first == node.first->instructions.front().function) fct_entry = fmap_entry;
			::symbolt const& symb(ns.lookup(fmap_entry->first));
			if ((*iter)->get(ID_identifier) == symb.base_name) alt_names.push_back(::symbol_expr(symb));
		}
		// check globals
		::contextt::symbolst::const_iterator global_symb(
				m_manager.context.symbols.find(::std::string("c::") + (*iter)->get_string(ID_identifier)));
		if (global_symb != m_manager.context.symbols.end() && global_symb->second.is_static_lifetime)
			alt_names.push_back(::symbol_expr(global_symb->second));
		// function arguments
		FSHELL2_AUDIT_ASSERT1(::diagnostics::Violated_Invariance, fct_entry != m_goto.function_map.end(),
				::diagnostics::internal::to_string("Function ",
					node.first->instructions.front().function, " not found in map!"));
		::code_typet::argumentst const& argument_types(fct_entry->second.type.arguments());
		for (::code_typet::argumentst::const_iterator a_iter(argument_types.begin());
				a_iter != argument_types.end(); ++a_iter) {
			::symbolt const& symb(ns.lookup(a_iter->get_identifier()));
			if ((*iter)->get(ID_identifier) == symb.base_name) alt_names.push_back(::symbol_expr(symb));
		}
		// local variables
		for (::goto_programt::instructionst::const_iterator f_iter(node.first->instructions.begin());
				f_iter != node.second; ++f_iter) {
			if (!f_iter->is_decl()) continue;
			::std::list< ::exprt const* > decl_symbols;
			find_symbols(f_iter->code, decl_symbols);
			for (::std::list< ::exprt const* >::const_iterator s_iter(decl_symbols.begin());
					s_iter != decl_symbols.end(); ++s_iter) {
				::symbolt const& symb(ns.lookup((*s_iter)->get(ID_identifier)));
				if ((*iter)->get(ID_identifier) == symb.base_name) alt_names.push_back(**s_iter);
			}
		}

		// rename, if possible
		if (alt_names.empty()) {
			FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Failed to resolve ", (*iter)->get(ID_identifier), " at ", node.second->location));
			make_nondet = true;
			break;
		}
		std::string a_n(alt_names.back().get_string(ID_identifier));
		if(a_n.size()>3 && a_n.substr(0,3)=="c::") a_n=a_n.substr(3);
		FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Renaming ", (*iter)->get(ID_identifier), " to ", a_n));
		(*iter)->set(ID_identifier, a_n);
	}
	::goto_programt tmp;

	if (make_nondet || special_pred_symb) {
		::symbolt const& cond_symbol(new_bool_var("pred"));
		::goto_programt::targett decl(tmp.add_instruction());
		decl->make_decl();
		decl->code = ::code_declt(::symbol_expr(cond_symbol));

		if (special_pred_symb && !make_nondet) {
			special_pred_symb->set(ID_identifier, id2string(cond_symbol.name).substr(3));
			
			FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Trying to typecheck: ", pred_copy.pretty()));
			bool const tc_failed(::ansi_c_typecheck(pred_copy, m_manager.get_message_handler(), ns));
			FSHELL2_PROD_CHECK1(Instrumentation_Error, !tc_failed,
					::std::string("Predicate ") + ::expr2c(pred_copy, ns) + " failed to typecheck");
			FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Typecheck completed: ", pred_copy.pretty()));

			::goto_programt dest;
			::goto_convert(::code_expressiont(pred_copy), m_manager.context,
					dest, m_manager.get_message_handler());
			tmp.destructive_append(dest);
		}

		pred_copy = ::symbol_expr(cond_symbol);
	} else {
		FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Trying to typecheck: ", pred_copy.pretty()));
		bool const tc_failed(::ansi_c_typecheck(pred_copy, m_manager.get_message_handler(), ns));
		FSHELL2_PROD_CHECK1(Instrumentation_Error, !tc_failed,
				::std::string("Predicate ") + ::expr2c(pred_copy, ns) + " failed to typecheck");
		FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Typecheck completed: ", pred_copy.pretty()));
	}

	FSHELL2_PROD_CHECK1(Instrumentation_Error, pred_copy.type().id() == ID_bool,
			::std::string("Predicate ") + ::expr2c(*pred, ns) + " is not of Boolean type");

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
	
GOTO_Transformation::inserted_t & GOTO_Transformation::make_nondet_choice(::goto_programt & dest, int const num) {
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
	::std::vector< ::symbolt const* > var_map(log_2_rounded, static_cast< ::symbolt const* >(0));
	
	for (int i(0); i < num - 1; ++i) {
		int bv(i);
		::exprt * full_cond(0);
		for (int j(0); j < log_2_rounded; ++j) {
			if (0 == i) {
				::symbolt const& cond_symbol(new_bool_var("nd_choice"));
				::goto_programt::targett decl(dest.add_instruction());
				decl->make_decl();
				decl->code = ::code_declt(::symbol_expr(cond_symbol));
				var_map[j] = &cond_symbol;
			}
			
			bool pos(bv & 0x1);
			bv >>= 1;
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, var_map.size() > static_cast<unsigned>(j) && var_map[j] != 0);
			::exprt cond(::symbol_expr(*(var_map[j])));
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

