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

/*! \file fshell2/fql/evaluation/evaluate_filter.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:31:27 CEST 2009 
*/

#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/not_implemented.hpp>
#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <fshell2/fql/ast/abstraction.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/primitive_filter.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/restriction_automaton.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/test_goal_set.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#include <algorithm>
#include <iterator>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
	
Evaluate_Filter::Evaluate_Filter(::goto_functionst const& ts) :
	m_ts(ts) {
}

Evaluate_Filter::~Evaluate_Filter() {
}

Evaluate_Filter::value_t const& Evaluate_Filter::evaluate(Filter const& f) {
	f.accept(this);
	filter_value_t::const_iterator entry(m_filter_map.find(&f));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			entry != m_filter_map.end());
	return entry->second;
}

void Evaluate_Filter::visit(Query const* n) {
	if (n->get_prefix()) {
		n->get_prefix()->accept(this);
	}

	n->get_cover()->accept(this);
	
	if (n->get_passing()) {
		n->get_passing()->accept(this);
	}
}

void Evaluate_Filter::visit(Test_Goal_Sequence const* n) {
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

void Evaluate_Filter::visit(Restriction_Automaton const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Filter::visit(Abstraction const* n) {
	for (Abstraction::preds_t::const_iterator iter(n->get_predicates().begin());
			iter != n->get_predicates().end(); ++iter) {
		(*iter)->accept(this);
	}
}

void Evaluate_Filter::visit(Predicate const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Filter::visit(TGS_Union const* n) {
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);
}

void Evaluate_Filter::visit(TGS_Intersection const* n) {
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);
}

void Evaluate_Filter::visit(TGS_Setminus const* n) {
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);
}

void Evaluate_Filter::visit(Edgecov const* n) {
	n->get_abstraction()->accept(this);
	n->get_filter()->accept(this);
}

void Evaluate_Filter::visit(Pathcov const* n) {
	n->get_abstraction()->accept(this);
	n->get_filter()->accept(this);
}

void Evaluate_Filter::visit(Filter_Identity const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;

	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Filter::visit(Filter_Complement const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter()->accept(this);
	Filter_Identity * id(Filter_Identity::Factory::get_instance().create());
	id->accept(this);

	filter_value_t::const_iterator id_set(m_filter_map.find(id));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, id_set != m_filter_map.end());
	filter_value_t::const_iterator f_set(m_filter_map.find(n->get_filter()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, f_set != m_filter_map.end());
	::std::set_difference(id_set->second.begin(), id_set->second.end(),
			f_set->second.begin(), f_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(Filter_Union const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());
	::std::set_union(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(Filter_Intersection const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());
	::std::set_intersection(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(Filter_Setminus const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());
	::std::set_difference(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(Filter_Enclosing_Scopes const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter()->accept(this);

	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Filter::visit(Primitive_Filter const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	switch (n->get_filter_type()) {
		case F_FILE:
			n->get_string_arg<F_FILE>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_LINE:
			n->get_int_arg<F_LINE>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_COLUMN:
			n->get_int_arg<F_COLUMN>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_FUNC:
			n->get_string_arg<F_FUNC>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_LABEL:
			n->get_string_arg<F_LABEL>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_CALL:
			n->get_string_arg<F_CALL>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_CALLS:
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_ENTRY:
			n->get_string_arg<F_ENTRY>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_EXIT:
			n->get_string_arg<F_EXIT>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_EXPR:
			n->get_string_arg<F_EXPR>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_REGEXP:
			n->get_string_arg<F_REGEXP>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_BASICBLOCKENTRY:
			for (::goto_functionst::function_mapt::const_iterator iter(m_ts.function_map.begin());
					iter != m_ts.function_map.end(); ++iter) {
				if (iter->second.body.empty()) continue;

				bool take_next(true);
				for (::goto_programt::instructionst::const_iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					::goto_programt::const_targetst succ;
					iter->second.body.get_successors(f_iter, succ);
					if (!succ.empty() && (take_next || succ.size() > 1)) {
						for (::goto_programt::const_targetst::const_iterator s_iter(succ.begin());
								s_iter != succ.end(); ++s_iter)
							entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
					}
					take_next = f_iter->is_function_call();
				}
			}
			break;
		case F_CONDITIONEDGE:
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_DECISIONEDGE:
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_CONDITIONGRAPH:
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
	}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

