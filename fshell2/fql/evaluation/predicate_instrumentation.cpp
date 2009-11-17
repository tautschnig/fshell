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

/*! \file fshell2/fql/evaluation/predicate_instrumentation.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:33:19 CET 2009 
*/

#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>

#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_postcondition.hpp>
#include <fshell2/fql/ast/pm_precondition.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_postcondition.hpp>
#include <fshell2/fql/ast/tgs_precondition.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Predicate_Instrumentation::Predicate_Instrumentation(Evaluate_Filter const& eval_filter,
		::goto_functionst & gf, ::contextt & context) :
	m_eval_filter(eval_filter),
	m_gf(gf),
	m_context(context),
	m_inserter(m_gf)
{
}

Predicate_Instrumentation::~Predicate_Instrumentation() {
}

target_graph_t::edge_t const& Predicate_Instrumentation::get(target_graph_t::node_t const& node,
		Predicate const* pred) const {
	node_to_pred_instrumentation_map_t::const_iterator entry1(m_node_to_pred_instr.find(node));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry1 != m_node_to_pred_instr.end());
	pred_instrumentation_map_t::const_iterator entry2(entry1->second.find(pred));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry2 != entry1->second.end());
	return entry2->second;
}

void Predicate_Instrumentation::insert_predicate(Predicate const* pred) {
	using ::fshell2::instrumentation::GOTO_Transformation;

	for (::std::set< target_graph_t::node_t >::iterator n_iter(m_current_node_set.begin());
			n_iter != m_current_node_set.end(); ++n_iter)
		if (m_node_to_pred_instr[ *n_iter ].end() ==
				m_node_to_pred_instr[ *n_iter ].find(pred)) {
			GOTO_Transformation::inserted_t const & res(m_inserter.insert_predicate_at(
						*n_iter, pred->get_expr(), m_context));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == res.size());
			goto_programt::targett next(res.front().second);
			++next;
			m_node_to_pred_instr[ *n_iter ][ pred ] = ::std::make_pair(res.front(), ::std::make_pair(n_iter->first, next));
		}
}

void Predicate_Instrumentation::visit(Edgecov const* n) {
	target_graph_t const& tgg(m_eval_filter.get(*(n->get_filter_expr())));
	m_current_node_set.clear();
	for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
			e_iter != tgg.get_edges().end(); ++e_iter) {
		m_current_node_set.insert(e_iter->first);
		m_current_node_set.insert(e_iter->second);
	}
	if (n->get_predicates())
		for (Predicate::preds_t::const_iterator iter(n->get_predicates()->begin());
				iter != n->get_predicates()->end(); ++iter)
			insert_predicate(*iter);
}

void Predicate_Instrumentation::visit(PM_Alternative const* n) {
	n->get_path_monitor_expr_a()->accept(this);
	::std::set< target_graph_t::node_t > c_n_bak;
	c_n_bak.swap(m_current_node_set);
	n->get_path_monitor_expr_b()->accept(this);
	m_current_node_set.insert(c_n_bak.begin(), c_n_bak.end());
}

void Predicate_Instrumentation::visit(PM_Concat const* n) {
	n->get_path_monitor_expr_a()->accept(this);
	::std::set< target_graph_t::node_t > c_n_bak;
	c_n_bak.swap(m_current_node_set);
	n->get_path_monitor_expr_b()->accept(this);
	m_current_node_set.insert(c_n_bak.begin(), c_n_bak.end());
}

void Predicate_Instrumentation::visit(PM_Filter_Adapter const* n) {
	target_graph_t const& tgg(m_eval_filter.get(*(n->get_filter_expr())));
	m_current_node_set.clear();
	for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
			e_iter != tgg.get_edges().end(); ++e_iter) {
		m_current_node_set.insert(e_iter->first);
		m_current_node_set.insert(e_iter->second);
	}
}

void Predicate_Instrumentation::visit(PM_Postcondition const* n) {
	n->get_path_monitor_expr()->accept(this);
	insert_predicate(n->get_predicate());
}

void Predicate_Instrumentation::visit(PM_Precondition const* n) {
	n->get_path_monitor_expr()->accept(this);
	insert_predicate(n->get_predicate());
}

void Predicate_Instrumentation::visit(PM_Repeat const* n) {
	n->get_path_monitor_expr()->accept(this);
}

void Predicate_Instrumentation::visit(Pathcov const* n) {
	target_graph_t const& tgg(m_eval_filter.get(*(n->get_filter_expr())));
	m_current_node_set.clear();
	for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
			e_iter != tgg.get_edges().end(); ++e_iter) {
		m_current_node_set.insert(e_iter->first);
		m_current_node_set.insert(e_iter->second);
	}
	if (n->get_predicates())
		for (Predicate::preds_t::const_iterator iter(n->get_predicates()->begin());
				iter != n->get_predicates()->end(); ++iter)
			insert_predicate(*iter);
}

void Predicate_Instrumentation::visit(Query const* n) {
	n->get_cover()->accept(this);
	if (n->get_passing()) {
		n->get_passing()->accept(this);
	}
}

void Predicate_Instrumentation::visit(Statecov const* n) {
	target_graph_t const& tgg(m_eval_filter.get(*(n->get_filter_expr())));
	m_current_node_set.clear();
	for (target_graph_t::edges_t::const_iterator e_iter(tgg.get_edges().begin());
			e_iter != tgg.get_edges().end(); ++e_iter) {
		m_current_node_set.insert(e_iter->first);
		m_current_node_set.insert(e_iter->second);
	}
	if (n->get_predicates())
		for (Predicate::preds_t::const_iterator iter(n->get_predicates()->begin());
				iter != n->get_predicates()->end(); ++iter)
			insert_predicate(*iter);
}


void Predicate_Instrumentation::visit(TGS_Intersection const* n) {
	n->get_tgs_a()->accept(this);
	::std::set< target_graph_t::node_t > c_n_bak;
	c_n_bak.swap(m_current_node_set);
	n->get_tgs_b()->accept(this);
	m_current_node_set.insert(c_n_bak.begin(), c_n_bak.end());
}

void Predicate_Instrumentation::visit(TGS_Postcondition const* n) {
	n->get_tgs()->accept(this);
	insert_predicate(n->get_predicate());
}

void Predicate_Instrumentation::visit(TGS_Precondition const* n) {
	n->get_tgs()->accept(this);
	insert_predicate(n->get_predicate());
}

void Predicate_Instrumentation::visit(TGS_Setminus const* n) {
	n->get_tgs_a()->accept(this);
	::std::set< target_graph_t::node_t > c_n_bak;
	c_n_bak.swap(m_current_node_set);
	n->get_tgs_b()->accept(this);
	m_current_node_set.insert(c_n_bak.begin(), c_n_bak.end());
}

void Predicate_Instrumentation::visit(TGS_Union const* n) {
	n->get_tgs_a()->accept(this);
	::std::set< target_graph_t::node_t > c_n_bak;
	c_n_bak.swap(m_current_node_set);
	n->get_tgs_b()->accept(this);
	m_current_node_set.insert(c_n_bak.begin(), c_n_bak.end());
}

void Predicate_Instrumentation::visit(Test_Goal_Sequence const* n) {
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		if (iter->first) {
			iter->first->accept(this);
		}
		iter->second->accept(this);
	}

	if (n->get_suffix_monitor()) {
		n->get_suffix_monitor()->accept(this);
	}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

