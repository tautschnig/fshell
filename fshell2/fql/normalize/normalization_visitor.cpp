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

/*! \file fshell2/fql/normalize/normalization_visitor.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 29 16:41:44 CEST 2009 
*/

#include <fshell2/fql/normalize/normalization_visitor.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
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

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

template <typename T>
Normalization_Visitor::Smart_FQL_Node_Ptr<T>::Smart_FQL_Node_Ptr()
	: m_ptr(0) {
}

template <typename T>
Normalization_Visitor::Smart_FQL_Node_Ptr<T>::Smart_FQL_Node_Ptr(Self const& rhs)
	: m_ptr(rhs.m_ptr) {
	if (0 != m_ptr) {
		m_ptr->incr_ref_count();
	}
}

template <typename T>
void Normalization_Visitor::Smart_FQL_Node_Ptr<T>::operator=(T * rhs) {
	if (m_ptr == rhs) return;
	if (0 != m_ptr) {
		m_ptr->decr_ref_count();
		m_ptr->destroy(); //if (m_ptr->destroy()) m_simplify.prune_cache(m_ptr);
	}
	m_ptr = rhs;
	if (0 != m_ptr) m_ptr->incr_ref_count();
}

template <typename T>
Normalization_Visitor::Smart_FQL_Node_Ptr<T>& Normalization_Visitor::Smart_FQL_Node_Ptr<T>::operator=(Self const& rhs) {
	if (m_ptr == rhs.m_ptr) return *this;
	if (0 != m_ptr) {
		m_ptr->decr_ref_count();
		m_ptr->destroy(); //if (m_ptr->destroy()) m_simplify.prune_cache(m_ptr);
	}
	m_ptr = rhs.m_ptr;
	if (0 != m_ptr) m_ptr->incr_ref_count();
	return *this;
}

template <typename T>
T* Normalization_Visitor::Smart_FQL_Node_Ptr<T>::get() {
	return m_ptr;
}

template <typename T>
Normalization_Visitor::Smart_FQL_Node_Ptr<T>::~Smart_FQL_Node_Ptr() {
	if (0 != m_ptr) {
		m_ptr->decr_ref_count();
		m_ptr->destroy(); //if (m_ptr->destroy()) m_simplify.prune_cache(m_ptr);
	}
	m_ptr = 0;
}

Normalization_Visitor::Normalization_Visitor() {
}

Normalization_Visitor::~Normalization_Visitor() {
}

void Normalization_Visitor::normalize(Query ** query) {
	FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Normalization input: ", **query));
	
	(*query)->accept(this);
	if (m_top_query.get() != *query) {
		(*query)->destroy();
		*query = m_top_query.get();
	}
	(*query)->incr_ref_count();
	m_top_query = 0;
	(*query)->decr_ref_count();

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !(*query)->get_prefix());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, (*query)->get_cover());
	
	FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string("Normalization result: ", **query));
}
	
void Normalization_Visitor::visit(Edgecov const* n) {
	n->get_filter_expr()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter(m_top_filter);

	Predicate::preds_t * preds(0);
	if (n->get_predicates()) {
		preds = new Predicate::preds_t;
		for (Predicate::preds_t::const_iterator iter(n->get_predicates()->begin());
				iter != n->get_predicates()->end(); ++iter) {
			(*iter)->accept(this);
			if (preds->insert(m_top_pred.get()).second) m_top_pred.get()->incr_ref_count();
		}
	
		for (Predicate::preds_t::const_iterator iter(preds->begin());
				iter != preds->end(); ++iter)
			(*iter)->decr_ref_count();
		m_top_pred = 0;
	}
	
	m_top_tgset = Edgecov::Factory::get_instance().create(
			m_prefix.get() ?
				Filter_Compose::Factory::get_instance().create(filter.get(), m_prefix.get()) :
				filter.get(), preds);
	m_top_filter = 0;
}

void Normalization_Visitor::visit(Filter_Complement const* n) {
	n->get_filter_expr()->accept(this);
	m_top_filter = Filter_Complement::Factory::get_instance().create(m_top_filter.get());
}

void Normalization_Visitor::visit(Filter_Compose const* n) {
	n->get_filter_expr_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_a(m_top_filter);
	n->get_filter_expr_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_b(m_top_filter);

	m_top_filter = Filter_Compose::Factory::get_instance().create(filter_a.get(), filter_b.get());
}

void Normalization_Visitor::visit(Filter_Enclosing_Scopes const* n) {
	n->get_filter_expr()->accept(this);
	m_top_filter = Filter_Enclosing_Scopes::Factory::get_instance().create(m_top_filter.get());
}

void Normalization_Visitor::visit(Filter_Function const* n) {
	switch (n->get_filter_type()) {
		case F_IDENTITY:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_IDENTITY>();
			break;
		case F_FILE:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_FILE>(
					n->get_string_arg<F_FILE>());
			break;
		case F_LINE:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_LINE>(
					n->get_int_arg<F_LINE>());
			break;
		case F_COLUMN:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_COLUMN>(
					n->get_int_arg<F_COLUMN>());
			break;
		case F_FUNC:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_FUNC>(
					n->get_string_arg<F_FUNC>());
			break;
		case F_LABEL:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_LABEL>(
					n->get_string_arg<F_LABEL>());
			break;
		case F_CALL:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_CALL>(
					n->get_string_arg<F_CALL>());
			break;
		case F_CALLS:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_CALLS>();
			break;
		case F_ENTRY:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_ENTRY>(
					n->get_string_arg<F_ENTRY>());
			break;
		case F_EXIT:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_EXIT>(
					n->get_string_arg<F_EXIT>());
			break;
		case F_EXPR:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_EXPR>(
					n->get_string_arg<F_EXPR>());
			break;
		case F_REGEXP:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_REGEXP>(
					n->get_string_arg<F_REGEXP>());
			break;
		case F_BASICBLOCKENTRY:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_BASICBLOCKENTRY>();
			break;
		case F_CONDITIONEDGE:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_CONDITIONEDGE>();
			break;
		case F_DECISIONEDGE:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_DECISIONEDGE>();
			break;
		case F_CONDITIONGRAPH:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_CONDITIONGRAPH>();
			break;
		case F_DEF:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_DEF>(
					n->get_string_arg<F_DEF>());
			break;
		case F_USE:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_USE>(
					n->get_string_arg<F_USE>());
			break;
		case F_STMTTYPE:
			m_top_filter = Filter_Function::Factory::get_instance().create<F_STMTTYPE>(
					n->get_int_arg<F_STMTTYPE>());
			break;
	}
}

void Normalization_Visitor::visit(Filter_Intersection const* n) {
	n->get_filter_expr_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_a(m_top_filter);
	n->get_filter_expr_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_b(m_top_filter);
	
	if (FQL_Node_Lt_Compare()(filter_a.get(), filter_b.get())) {
		m_top_filter = Filter_Intersection::Factory::get_instance().create(filter_a.get(), filter_b.get());
	} else if (FQL_Node_Lt_Compare()(filter_b.get(), filter_a.get())) {
		m_top_filter = Filter_Intersection::Factory::get_instance().create(filter_b.get(), filter_a.get());
	} else {
		m_top_filter = filter_a;
	}
}

void Normalization_Visitor::visit(Filter_Setminus const* n) {
	n->get_filter_expr_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_a(m_top_filter);
	n->get_filter_expr_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_b(m_top_filter);

	m_top_filter = Filter_Setminus::Factory::get_instance().create(filter_a.get(), filter_b.get());
}

void Normalization_Visitor::visit(Filter_Union const* n) {
	n->get_filter_expr_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_a(m_top_filter);
	n->get_filter_expr_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_b(m_top_filter);
	
	if (FQL_Node_Lt_Compare()(filter_a.get(), filter_b.get())) {
		m_top_filter = Filter_Union::Factory::get_instance().create(filter_a.get(), filter_b.get());
	} else if (FQL_Node_Lt_Compare()(filter_b.get(), filter_a.get())) {
		m_top_filter = Filter_Union::Factory::get_instance().create(filter_b.get(), filter_a.get());
	} else {
		m_top_filter = filter_a;
	}
}

void Normalization_Visitor::visit(PM_Alternative const* n) {
	n->get_path_monitor_expr_a()->accept(this);
	Smart_FQL_Node_Ptr<Path_Monitor_Expr> path_monitor_a(m_top_mon);
	n->get_path_monitor_expr_b()->accept(this);
	Smart_FQL_Node_Ptr<Path_Monitor_Expr> path_monitor_b(m_top_mon);
	
	if (FQL_Node_Lt_Compare()(path_monitor_a.get(), path_monitor_b.get())) {
		m_top_mon = PM_Alternative::Factory::get_instance().create(path_monitor_a.get(), path_monitor_b.get());
	} else if (FQL_Node_Lt_Compare()(path_monitor_b.get(), path_monitor_a.get())) {
		m_top_mon = PM_Alternative::Factory::get_instance().create(path_monitor_b.get(), path_monitor_a.get());
	} else {
		m_top_mon = path_monitor_a;
	}
}

void Normalization_Visitor::visit(PM_Concat const* n) {
	n->get_path_monitor_expr_a()->accept(this);
	Smart_FQL_Node_Ptr<Path_Monitor_Expr> path_monitor_a(m_top_mon);
	n->get_path_monitor_expr_b()->accept(this);
	Smart_FQL_Node_Ptr<Path_Monitor_Expr> path_monitor_b(m_top_mon);

	m_top_mon = PM_Concat::Factory::get_instance().create(path_monitor_a.get(), path_monitor_b.get());
}

void Normalization_Visitor::visit(PM_Filter_Adapter const* n) {
	n->get_filter_expr()->accept(this);
	m_top_mon = PM_Filter_Adapter::Factory::get_instance().create(
			m_prefix.get() ?
				Filter_Compose::Factory::get_instance().create(m_top_filter.get(), m_prefix.get()) :
				m_top_filter.get());
}

void Normalization_Visitor::visit(PM_Postcondition const* n) {
	n->get_path_monitor_expr()->accept(this);
	n->get_predicate()->accept(this);
	m_top_mon = PM_Postcondition::Factory::get_instance().create(m_top_mon.get(), m_top_pred.get());
}

void Normalization_Visitor::visit(PM_Precondition const* n) {
	n->get_predicate()->accept(this);
	n->get_path_monitor_expr()->accept(this);
	m_top_mon = PM_Precondition::Factory::get_instance().create(m_top_mon.get(), m_top_pred.get());
}

void Normalization_Visitor::visit(PM_Repeat const* n) {
	n->get_path_monitor_expr()->accept(this);
	m_top_mon = PM_Repeat::Factory::get_instance().create(m_top_mon.get(), n->get_lower_bound(),
			n->get_upper_bound());
}

void Normalization_Visitor::visit(Pathcov const* n) {
	n->get_filter_expr()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter(m_top_filter);

	Predicate::preds_t * preds(0);
	if (n->get_predicates()) {
		preds = new Predicate::preds_t;
		for (Predicate::preds_t::const_iterator iter(n->get_predicates()->begin());
				iter != n->get_predicates()->end(); ++iter) {
			(*iter)->accept(this);
			if (preds->insert(m_top_pred.get()).second) m_top_pred.get()->incr_ref_count();
		}
	
		for (Predicate::preds_t::const_iterator iter(preds->begin());
				iter != preds->end(); ++iter)
			(*iter)->decr_ref_count();
		m_top_pred = 0;
	}
	
	m_top_tgset = Pathcov::Factory::get_instance().create(
			m_prefix.get() ?
				Filter_Compose::Factory::get_instance().create(filter.get(), m_prefix.get()) :
				filter.get(), n->get_bound(), preds);
	m_top_filter = 0;
}

void Normalization_Visitor::visit(Predicate const* n) {
	m_top_pred = Predicate::Factory::get_instance().create(new ::exprt(*(n->get_expr())));
}
	
void Normalization_Visitor::visit(Query const* n) {
	if (n->get_prefix()) {
		n->get_prefix()->accept(this);
		m_prefix = m_top_filter;
	}

	n->get_cover()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Sequence> cover(m_top_tgseq);
	
	// IN ... prefix only applies to COVER clause
	m_prefix = 0;

	Smart_FQL_Node_Ptr<Path_Monitor_Expr> passing;
	if (n->get_passing()) {
		n->get_passing()->accept(this);
		passing = m_top_mon;
	}

	m_top_query = Query::Factory::get_instance().create(0, cover.get(), passing.get());
	m_top_filter = 0;
	m_top_tgseq = 0;
	m_top_mon = 0;
}

void Normalization_Visitor::visit(Statecov const* n) {
	n->get_filter_expr()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter(m_top_filter);

	Predicate::preds_t * preds(0);
	if (n->get_predicates()) {
		preds = new Predicate::preds_t;
		for (Predicate::preds_t::const_iterator iter(n->get_predicates()->begin());
				iter != n->get_predicates()->end(); ++iter) {
			(*iter)->accept(this);
			if (preds->insert(m_top_pred.get()).second) m_top_pred.get()->incr_ref_count();
		}
	
		for (Predicate::preds_t::const_iterator iter(preds->begin());
				iter != preds->end(); ++iter)
			(*iter)->decr_ref_count();
		m_top_pred = 0;
	}
	
	m_top_tgset = Statecov::Factory::get_instance().create(
			m_prefix.get() ?
				Filter_Compose::Factory::get_instance().create(filter.get(), m_prefix.get()) :
				filter.get(), preds);
	m_top_filter = 0;
}
	
void Normalization_Visitor::visit(TGS_Intersection const* n) {
	n->get_tgs_a()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_a(m_top_tgset);
	n->get_tgs_b()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_b(m_top_tgset);

	if (FQL_Node_Lt_Compare()(tgs_a.get(), tgs_b.get())) {
		m_top_tgset = TGS_Intersection::Factory::get_instance().create(tgs_a.get(), tgs_b.get());
	} else if (FQL_Node_Lt_Compare()(tgs_b.get(), tgs_a.get())) {
		m_top_tgset = TGS_Intersection::Factory::get_instance().create(tgs_b.get(), tgs_a.get());
	} else {
		m_top_tgset = tgs_a;
	}
}

void Normalization_Visitor::visit(TGS_Postcondition const* n) {
	n->get_tgs()->accept(this);
	n->get_predicate()->accept(this);
	m_top_tgset = TGS_Postcondition::Factory::get_instance().create(m_top_tgset.get(), m_top_pred.get());
}

void Normalization_Visitor::visit(TGS_Precondition const* n) {
	n->get_predicate()->accept(this);
	n->get_tgs()->accept(this);
	m_top_tgset = TGS_Precondition::Factory::get_instance().create(m_top_tgset.get(), m_top_pred.get());
}

void Normalization_Visitor::visit(TGS_Setminus const* n) {
	n->get_tgs_a()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_a(m_top_tgset);
	n->get_tgs_b()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_b(m_top_tgset);

	m_top_tgset = TGS_Setminus::Factory::get_instance().create(tgs_a.get(), tgs_b.get());
}

void Normalization_Visitor::visit(TGS_Union const* n) {
	n->get_tgs_a()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_a(m_top_tgset);
	n->get_tgs_b()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_b(m_top_tgset);

	if (FQL_Node_Lt_Compare()(tgs_a.get(), tgs_b.get())) {
		m_top_tgset = TGS_Union::Factory::get_instance().create(tgs_a.get(), tgs_b.get());
	} else if (FQL_Node_Lt_Compare()(tgs_b.get(), tgs_a.get())) {
		m_top_tgset = TGS_Union::Factory::get_instance().create(tgs_b.get(), tgs_a.get());
	} else {
		m_top_tgset = tgs_a;
	}
}

void Normalization_Visitor::visit(Test_Goal_Sequence const* n) {
	Test_Goal_Sequence::seq_t seq;
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		Smart_FQL_Node_Ptr<Path_Monitor_Expr> mon;
		if (iter->first) {
			iter->first->accept(this);
			mon = m_top_mon;
			mon.get()->incr_ref_count();
		} else if (m_prefix.get() && iter != n->get_sequence().begin()) {
			m_top_mon = PM_Repeat::Factory::get_instance().create(
					PM_Filter_Adapter::Factory::get_instance().create(
						Filter_Compose::Factory::get_instance().create(
							Filter_Function::Factory::get_instance().create<F_IDENTITY>(),
							m_prefix.get())), 0, -1);
			mon = m_top_mon;
			mon.get()->incr_ref_count();
		} else {
			mon = 0;
		}
		iter->second->accept(this);
		m_top_tgset.get()->incr_ref_count();
		seq.push_back(::std::make_pair(mon.get(), m_top_tgset.get()));
	}

	Smart_FQL_Node_Ptr<Path_Monitor_Expr> mon;
	if (n->get_suffix_monitor()) {
		n->get_suffix_monitor()->accept(this);
		mon = m_top_mon;
	}
	
	for (Test_Goal_Sequence::seq_t::const_iterator iter(seq.begin());
			iter != seq.end(); ++iter) {
		if (iter->first) iter->first->decr_ref_count();
		iter->second->decr_ref_count();
	}

	m_top_tgseq = Test_Goal_Sequence::Factory::get_instance().create(seq, mon.get());
	m_top_mon = 0;
	m_top_tgset = 0;
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

