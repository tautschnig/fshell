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
		::optionst const& opts, Evaluate_Filter const& eval) :
	::bmct(manager.context, manager.ui_message_handler),
	m_is_initialized(false), m_eval_filter(eval), m_cnf(),
	m_bv(m_cnf) {
	this->options = opts;
	this->options.set_option("dimacs", false);
	this->options.set_option("cvc", false);
	this->options.set_option("smt", false);
	this->options.set_option("refine", false);
	this->options.set_option("cvc", false);
	this->set_verbosity(manager.get_verbosity());

	m_cnf.set_message_handler(this->message_handler);
	m_cnf.set_verbosity(this->get_verbosity());
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
	// protected field, can't read here
	// FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, this->_symex.remaining_claims);

	// ::std::cerr << "Mapping program:" << ::std::endl;
	// ::std::cerr << this->_equation << ::std::endl;

	// build a map from GOTO instructions to variable numbers by walking the
	// equation; this is a bit more complicated than just taking the
	// guard_literal of the state as the original guards have been shifted
	// to to build dominating expressions
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				_equation.SSA_steps.begin() ); iter != _equation.SSA_steps.end(); ++iter)
	{
		// don't consider hidden assignments
		if (iter->is_assignment() && iter->assignment_type == ::symex_targett::HIDDEN) continue;
		// goto deserves special care
		if (!iter->source.pc->is_goto()) {
			m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(iter->guard_literal,
							(iter->is_assume() || iter->is_assert())?iter->cond_literal : ::const_literal(true))));
			/* if (iter->source.pc->is_assert()) {
				::std::cerr << "Instr: ASSERT " << ::from_expr(this->ns, "", iter->source.pc->guard) << ::std::endl;
			} else {
				::std::cerr << "Instr: " << ::from_expr(this->ns, "", iter->source.pc->code) << ::std::endl;
			}
			::std::cerr << "Guard: " << iter->guard_literal.dimacs() << ::std::endl;
			*/
			continue;
		}

		if (!iter->is_location()) continue;
		// first step is location, carries the reachability-guard
		FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, iter->is_location());
		// unguarded goto
		if (iter->source.pc->guard.is_true() || iter->source.pc->guard.is_false()) {
			// ::std::cerr << "(GOTO) Trivial guard: " << ::const_literal(iter->source.pc->guard.is_true()).dimacs() << ::std::endl;
			m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(iter->guard_literal,
							::const_literal(iter->source.pc->guard.is_true()))));
			continue;
		}
		// loop-bound-exceeded handling is missing
		// we might need to distinguish forward vs. backwards goto here
		::goto_programt::const_targett succ(iter->source.pc);
		++succ;
        
		// now there are two possibilites - either the next state is a hidden
		// assignment (with the same program counter), then its lhs is the
		// desired guard; otherwise the guard is the last one in a long list
		::symex_target_equationt::SSA_stepst::const_iterator next_state(iter); ++next_state;
		FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, next_state != this->_equation.SSA_steps.end());
		::exprt const * real_guard(0);
		// skip this statement if it seems to be without effect
		if (next_state->source.pc == succ &&
				next_state->is_assignment() && next_state->assignment_type == ::symex_targett::HIDDEN )
		{
			real_guard = &(next_state->lhs);
		}
		else
		{
			// now we know that *next_state has a non-trivial guard; but this
			// guard may be a conjunction of several guards, pick the last one
			real_guard = next_state->guard_expr.has_operands() ? 
				&(next_state->guard_expr.operands().back()) : &(next_state->guard_expr);
		}
		// the resulting guard may be trivial
		if (real_guard->is_true() || real_guard->is_false()) {
			m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(iter->guard_literal,
							::const_literal(real_guard->is_true()))));
			continue;
		}
		// ... or a negated symbol
		bool neg(false);
		/*
		this would be more beautiful, no new variables
		if (real_guard->id() == "not") {
			neg = true;
			real_guard = &(real_guard->op0());
		}
		::std::cerr << "real guard: " << ::from_expr(this->ns, "", *real_guard) << ::std::endl;
		// lookup the name in the symbol map
		::literalt dest;
		::prop_convt const& prop(m_bv); // terrible overload of literal with different signature
		FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, !prop.literal(*real_guard, dest));
		*/
		::literalt dest(m_bv.convert(*real_guard));
		// ::std::cerr << "(GOTO) Found mapping for " << real_guard->get("identifier") << ": " << dest.dimacs() << ::std::endl;
		// entry has been found, store it in the map
		m_pc_to_bool_var_and_guard.insert(::std::make_pair(iter->source.pc, ::std::make_pair(iter->guard_literal, neg?::neg(dest):dest)));
	}
					
	m_is_initialized = true;
}
	
Compute_Test_Goals::value_t const& Compute_Test_Goals::compute(Query const& query) {
	initialize();

	query.accept(this);

	// a hack, to be reconsidered TODO
	tgs_value_t::const_iterator entry(m_tgs_map.find(query.get_cover()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, entry != m_tgs_map.end());
	return entry->second;
}

void Compute_Test_Goals::visit(Query const* n) {
	if (n->get_prefix()) {
		FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
		n->get_prefix()->accept(this);
	}

	n->get_cover()->accept(this);

	if (n->get_passing()) {
		n->get_passing()->accept(this);
	}
}

void Compute_Test_Goals::visit(Test_Goal_Sequence const* n) {
	::std::pair< tgs_value_t::iterator, bool > entry(m_tgs_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		if (iter->first) {
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			iter->first->accept(this);
		}
		iter->second->accept(this);
		tgs_value_t::const_iterator subgoals(m_tgs_map.find(iter->second));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, subgoals != m_tgs_map.end());

		if (iter != n->get_sequence().begin()) {
			value_t const backup(entry.first->second);
			entry.first->second.clear();
			for (value_t::const_iterator tgi(backup.begin()); tgi != backup.end(); ++tgi)
				for (value_t::const_iterator sgi(subgoals->second.begin());
						sgi != subgoals->second.end(); ++sgi)
					entry.first->second.insert(m_cnf.land(*tgi, *sgi));
		} else {
			::std::copy(subgoals->second.begin(), subgoals->second.end(),
				::std::inserter(entry.first->second, entry.first->second.begin()));
		}
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
	Evaluate_Filter::value_t const& filter_val(m_eval_filter.get(*(n->get_filter())));
	::std::cerr << filter_val.size() << " edges must be covered" << ::std::endl;
	for (Evaluate_Filter::value_t::const_iterator iter(filter_val.begin());
			iter != filter_val.end(); ++iter) {
		::std::pair< pc_to_bool_var_t::const_iterator, pc_to_bool_var_t::const_iterator >
			guards(m_pc_to_bool_var_and_guard.equal_range(iter->first));
		// FSHELL2_PROD_ASSERT(::diagnostics::Violated_Invariance, guards.first != guards.second);
		
		if (guards.first == guards.second) {
			::std::cerr << "WARNING: no guards for expr ";
			if (iter->first->is_goto()) {
				::std::cerr << "FILTER Instr: IF " << ::from_expr(this->ns, "", iter->first->guard);
			} else if (iter->first->is_assert()) {
				::std::cerr << "FILTER Instr: ASSERT " << ::from_expr(this->ns, "", iter->first->guard);
			} else {
				::std::cerr << "FILTER Instr: " << ::from_expr(this->ns, "", iter->first->code);
			}
			::std::cerr << " @" << iter->first->location << ::std::endl;
			continue;
		}

		// this is an edge in the CFG, therefore it is taken if
		// - the first statement is not a goto and its guard is true (test goal
		// is making this guard true)
		// - otherwise, iff it is a goto, the guard_literal of the goto must be
		// true if the second statement is the jump target (and not the skip
		// target)
		if (!iter->first->is_goto()) {
			::bvt set;
			for (; guards.first != guards.second; ++(guards.first)) {
				if (guards.first->second.first.is_false()) continue;
				// we will always take this edge, a trivial goal
				if (guards.first->second.first.is_true()) {
					set.clear();
					// set.push_back(m_cnf.new_variable());
					break;
				}
				// ::std::cerr << "Adding non-trivial guard " << guards.first->second.first.dimacs() << ::std::endl;
				set.push_back(guards.first->second.first);
			}
			if (!set.empty()) entry.first->second.insert(m_cnf.lor(set));
		} else {
			::bvt set;
			for (; guards.first != guards.second; ++(guards.first)) {
				if (guards.first->second.first.is_false()) continue;
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == iter->first->targets.size());
				bool is_goto_target(iter->first->targets.front() == iter->second);
				// ::std::cerr << "Real goto: " << (char const*) (is_goto_target?"yes":"no") << ::std::endl;
				if (guards.first->second.first.is_true() && (
							(is_goto_target && guards.first->second.second.is_true()) ||
							(!is_goto_target && guards.first->second.second.is_false()))) {
					// we will always take this edge, a trivial goal
					set.clear();
					// set.push_back(m_cnf.new_variable());
					break;
				}
				::literalt guard(is_goto_target? ::neg(guards.first->second.second) : guards.first->second.second);
				if (guards.first->second.first.is_true()) {
					// ::std::cerr << "Adding goto-guard " << guard.dimacs() << ::std::endl;
					set.push_back(guard);
				} else {
					set.push_back(m_cnf.land(guards.first->second.first, guard));
					// ::std::cerr << "Adding goto-guard " << set.back().dimacs() << ::std::endl;
				}
			}
			if (!set.empty()) entry.first->second.insert(m_cnf.lor(set));
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

