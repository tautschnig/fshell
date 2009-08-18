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

#include <cbmc/src/util/std_expr.h>
#include <cbmc/src/util/expr_util.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

	
GOTO_Transformation::GOTO_Transformation(::goto_functionst & gf) :
	m_goto(gf), m_nondet_var_count(-1) {
}

void GOTO_Transformation::copy_annotations(::goto_programt::const_targett src, ::goto_programt & target) {
	for (::goto_programt::targett iter(target.instructions.begin());
			iter != target.instructions.end(); ++iter) {
		iter->function = src->function;
		iter->location = src->location;
		iter->add_local_variables(src->local_variables);
	}
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
			::goto_programt::targett next(iter);
			++next;
			insert(pos, ::std::make_pair(::std::make_pair(&(entry->second.body), iter),
						::std::make_pair(&(entry->second.body), next)), prg);
			collector.insert(collector.end(), m_inserted.begin(), m_inserted.end());
		}
	}

	m_inserted.swap(collector);
	return m_inserted;
}
	
GOTO_Transformation::inserted_t const& GOTO_Transformation::insert(position_t
		const pos, goto_edge_t const& edge, ::goto_programt const& prg) {
	::goto_programt tmp;
	tmp.copy_from(prg);
	m_inserted.clear();
	bool do_insert(!tmp.instructions.empty());
	
	switch (pos) {
		case BEFORE:
			copy_annotations(edge.first.second, tmp);
			edge.first.first->destructive_insert(edge.first.second, tmp);
			edge.first.first->update();
			if (do_insert) {
				::goto_programt::targett pred(edge.first.second);
				pred--;
				m_inserted.push_back(::std::make_pair(edge.first.first, pred));
			}
			break;
		case AFTER:
			copy_annotations(edge.second.second, tmp);
			edge.second.first->destructive_insert(edge.second.second, tmp);
			edge.second.first->update();
			if (do_insert) {
				::goto_programt::targett pred(edge.second.second);
				pred--;
				m_inserted.push_back(::std::make_pair(edge.second.first, pred));
			}
			break;
	}

	return m_inserted;
}
	
GOTO_Transformation::inserted_t & GOTO_Transformation::make_nondet_choice(::goto_programt & dest, int const num,
		::contextt context) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, num >= 1);
	m_inserted.clear();
	// count up until n-1, each bit sequence determines a boolean
	// combination, jump to end after call
	// compute log2(num); can be done faster, but never mind
	int log_2_rounded(0);
	int num_options(num);
	while (num_options >>= 1) ++log_2_rounded;

	::goto_programt tmp_target;
	::goto_programt::targett out_target(tmp_target.add_instruction(SKIP));
	
	if (num > 1) {
		++m_nondet_var_count;
		FSHELL2_PROD_CHECK1(Instrumentation_Error, m_nondet_var_count >= 0, "Too many nondet choices required");
	}

	for (int i(0); i < num - 1; ++i) {
		int bv(i);
		::exprt * full_cond(0);
		for (int j(0); j <= log_2_rounded; ++j) {
			bool pos(bv & 0x1);
			bv >>= 1;
			::std::string const var_name(::diagnostics::internal::to_string("c::nondet_choice_var", m_nondet_var_count, "$", j));
			if (0 == i) {
				::symbolt cond_symbol;
				cond_symbol.mode = "C";
				cond_symbol.name = var_name;
				cond_symbol.base_name = var_name.substr(3, ::std::string::npos);
				cond_symbol.type = ::typet("bool");
				cond_symbol.lvalue = false;
				cond_symbol.static_lifetime = false;
				context.move(cond_symbol);
			}
			::symbolst::const_iterator symb_entry(context.symbols.find(var_name));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symb_entry != context.symbols.end());
			::symbolt const& cond_symbol(symb_entry->second);
			if (0 == i) {
				::goto_programt::targett decl(dest.add_instruction());
				decl->make_other();
				decl->code = ::code_declt();
				decl->code.copy_to_operands(::symbol_expr(cond_symbol));
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

	dest.destructive_append(tmp_target);
	dest.update();

	return m_inserted;
}
	
FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

