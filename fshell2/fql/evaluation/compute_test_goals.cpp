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

/*! \file fshell2/fql/evaluation/compute_test_goals.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri May  1 21:45:40 CEST 2009 
*/

#include <fshell2/fql/evaluation/compute_test_goals.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/exception/fshell2_error.hpp>

#include <fshell2/fql/evaluation/evaluate_filter.hpp>

#include <fshell2/fql/ast/abstraction.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/restriction_automaton.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/test_goal_set.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#include <algorithm>
#include <iterator>

#include <cbmc/src/util/i2string.h>
#include <cbmc/src/util/message_stream.h>
#include <cbmc/src/goto-symex/slice.h>
#include <cbmc/src/goto-symex/slice_by_trace.h>
#include <cbmc/src/cbmc/document_subgoals.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Compute_Test_Goals::Compute_Test_Goals(::language_uit & manager,
		::optionst const& opts, Evaluate_Filter & eval) :
	::bmct(manager.context, manager.ui_message_handler),
	m_is_initialized(false), m_eval_filter(eval), m_solver(),
	m_bv(m_solver) {
	this->options = opts;
	this->options.set_option("dimacs", false);
	this->options.set_option("cvc", false);
	this->options.set_option("smt", false);
	this->options.set_option("bv-refine", false);
	this->options.set_option("cvc", false);
	this->set_verbosity(manager.get_verbosity());

	m_solver.set_message_handler(this->message_handler);
	m_solver.set_verbosity(this->get_verbosity());
	m_bv.set_message_handler(this->message_handler);
	m_bv.set_verbosity(this->get_verbosity());
}

Compute_Test_Goals::~Compute_Test_Goals() {
}

bool Compute_Test_Goals::decide_default() {
  if(this->options.get_option("arrays-uf")=="never")
    m_bv.unbounded_array=bv_cbmct::U_NONE;
  else if(this->options.get_option("arrays-uf")=="always")
    m_bv.unbounded_array=bv_cbmct::U_ALL;

  status("Passing problem to "+m_bv.decision_procedure_text());

  this->do_unwind_module(m_bv);
  this->do_cbmc(m_bv);

  return false;
}

void Compute_Test_Goals::initialize() {
	if (m_is_initialized) return;

	// build the Boolean equation
	FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, !this->run(m_eval_filter.get_ts()),
			"Failed to build Boolean program representation");
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, this->_symex.get_remaining_claims());

	// ::std::cerr << "Mapping program:" << ::std::endl;
	// ::std::cerr << this->_equation << ::std::endl;

	// build a map from GOTO instructions to variable numbers by walking the
	// equation; this is a bit more complicated than just taking the
	// guard_literal of the state as the original guards have been shifted
	// to to build dominating expressions
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				_equation.SSA_steps.begin() ); iter != _equation.SSA_steps.end(); ++iter)
	{
		// ::std::cerr << "Checking "; iter->output(this->ns, ::std::cerr); ::std::cerr << ::std::endl;
		// don't consider hidden assignments
		if (iter->is_assignment() && iter->assignment_type == ::symex_targett::HIDDEN) continue;
		if (iter->source.pc->is_goto() && !(iter->source.pc->guard.is_true() || iter->source.pc->guard.is_false())) {
			// now there are two possibilites - either the next state is a
			// hidden assignment (that has a TRUE guard itself), then the
			// next but one state must carry the desired guard; such a guard may
			// be one of many, but in all cases it is the last one.
			::symex_target_equationt::SSA_stepst::const_iterator next_state(iter); ++next_state;
			FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, next_state != this->_equation.SSA_steps.end());
			::exprt const * real_guard(0);
			// skip this statement if it seems to be without effect
			if (next_state->guard.is_true() &&
					next_state->is_assignment() && next_state->assignment_type == ::symex_targett::HIDDEN )
			{
				real_guard = &(next_state->lhs);
			}
			else if(iter->guard == next_state->guard)
			{
				FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance,
						!(next_state->is_assignment() && next_state->assignment_type == ::symex_targett::HIDDEN) );
				continue;
			}
			else
			{
				// now we know that *next_state has a non-trivial guard; but this
				// guard may be a conjunction of several guards, pick the last one
				real_guard = next_state->guard.has_operands() ? 
					&(next_state->guard.operands().back()) : &(next_state->guard);
			}
			// lookup the name in the symbol map
			::literalt dest;
			::prop_convt const& prop(m_bv); // terrible overload of literal with different signature
			FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !prop.literal(*real_guard, dest));
			// ::std::cerr << "(GOTO) Found mapping for " << real_guard->get("identifier") << ": " << dest.dimacs() << ::std::endl;
			// entry has been found, store it in the map
			m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(dest.dimacs(),
							(iter->is_assume() || iter->is_assert())?iter->cond_literal.dimacs():iter->guard_literal.dimacs())));
		} else if (iter->guard.is_true() || iter->guard.is_false()) {
			// Boolean variable corresponding to the guard will be 0 or 1
			m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(!iter->source.pc->guard.is_false(),
							(iter->is_assume() || iter->is_assert())?iter->cond_literal.dimacs():iter->guard_literal.dimacs())));
		} else if (iter->guard.id() == "not") {
			// lookup the name in the symbol map
			::literalt dest;
			::prop_convt const& prop(m_bv); // terrible overload of literal with different signature
			FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !prop.literal(iter->guard.op0(), dest));
			// ::std::cerr << "Found mapping for " << iter->guard.op0().get("identifier") << ": " << -dest.dimacs() << ::std::endl;
			// entry has been found, store it in the map
			m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(-dest.dimacs(),
							(iter->is_assume() || iter->is_assert())?iter->cond_literal.dimacs():iter->guard_literal.dimacs())));
		} else {
			FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, iter->guard.id() == "symbol");
			// lookup the name in the symbol map
			::literalt dest;
			::prop_convt const& prop(m_bv); // terrible overload of literal with different signature
			FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !prop.literal(iter->guard, dest));
			// ::std::cerr << "Found mapping for " << iter->guard.get("identifier") << ": " << dest.dimacs() << ::std::endl;
			// entry has been found, store it in the map
			m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(dest.dimacs(),
							(iter->is_assume() || iter->is_assert())?iter->cond_literal.dimacs():iter->guard_literal.dimacs())));
		}
	}
					
	m_is_initialized = true;
}
	
Compute_Test_Goals::value_t const& Compute_Test_Goals::compute(Query const& query) {
	initialize();

	query.accept(this);
	
	// this is a hack!!!
	tgs_value_t::const_iterator entry(m_tgs_map.find(query.get_cover()->get_sequence().front().second));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, entry != m_tgs_map.end());
	return entry->second;
}

void Compute_Test_Goals::visit(Query const* n) {
	if (n->get_prefix()) {
		n->get_prefix()->accept(this);
	}

	n->get_cover()->accept(this);
}

void Compute_Test_Goals::visit(Test_Goal_Sequence const* n) {
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		if (iter->first) {
			iter->first->accept(this);
		}
		iter->second->accept(this);
	}

	if (n->get_suffix_automaton()) {
		n->get_suffix_automaton()->accept(this);
	}
}

void Compute_Test_Goals::visit(Restriction_Automaton const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Compute_Test_Goals::visit(Abstraction const* n) {
	for (Abstraction::preds_t::const_iterator iter(n->get_predicates().begin());
			iter != n->get_predicates().end(); ++iter) {
		(*iter)->accept(this);
	}
}

void Compute_Test_Goals::visit(Predicate const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Compute_Test_Goals::visit(TGS_Union const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);

	tgs_value_t::const_iterator a_set(m_tgs_map.find(n->get_tgs_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_tgs_map.end());
	tgs_value_t::const_iterator b_set(m_tgs_map.find(n->get_tgs_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_tgs_map.end());
	::std::set_union(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Compute_Test_Goals::visit(TGS_Intersection const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);

	tgs_value_t::const_iterator a_set(m_tgs_map.find(n->get_tgs_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_tgs_map.end());
	tgs_value_t::const_iterator b_set(m_tgs_map.find(n->get_tgs_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_tgs_map.end());
	::std::set_intersection(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Compute_Test_Goals::visit(TGS_Setminus const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);

	tgs_value_t::const_iterator a_set(m_tgs_map.find(n->get_tgs_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_tgs_map.end());
	tgs_value_t::const_iterator b_set(m_tgs_map.find(n->get_tgs_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_tgs_map.end());
	::std::set_difference(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Compute_Test_Goals::visit(Edgecov const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_abstraction()->accept(this);
	
	// for now, we completely ignore the abstraction!
	Evaluate_Filter::value_t const& filter_val(m_eval_filter.evaluate(*(n->get_filter())));
	for (Evaluate_Filter::value_t::const_iterator iter(filter_val.begin());
			iter != filter_val.end(); ++iter) {
		// this is an edge in the CFG, therefore it is taken if
		// - the first statement is not a goto and its guard is true (test goal
		// is making this guard true)
		// - otherwise, iff it is a goto, the guard_literal of the goto must be
		// true if the second statement is the jump target (and not the skip
		// target)
		::std::pair< pc_to_bool_var_t::const_iterator, pc_to_bool_var_t::const_iterator >
			guards(m_pc_to_bool_var_and_guard.equal_range(iter->first));
		// FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, guards.first != guards.second);
		// this is all very broken!!!!
		for (; guards.first != guards.second; ++(guards.first)) {
			if (!iter->first->is_goto()) {
				// ::std::cerr << "Adding guard " << guards.first->second.first << ::std::endl;
				entry.first->second.insert(guards.first->second.first);
			} else {
				// ::std::cerr << "Adding goto-guard " << guards.first->second.first << ::std::endl;
				entry.first->second.insert(guards.first->second.first);
			}
		}
	}

	n->get_filter()->accept(this);
}

void Compute_Test_Goals::visit(Pathcov const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
	
	n->get_abstraction()->accept(this);
	n->get_filter()->accept(this);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

