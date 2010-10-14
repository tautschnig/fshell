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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#endif

#include <fshell2/fql/ast/coverage_pattern_expr.hpp>
#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/ecp_atom.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_expr.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/fql_node.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/path_pattern_expr.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pp_alternative.hpp>
#include <fshell2/fql/ast/pp_concat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>
#include <fshell2/fql/ast/repeat.hpp>

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

#define MOVE_TO_CP  if (m_top_atom.get()) { m_top_cp = m_top_atom.get(); m_top_atom = 0; } DUMMY_FUNC
#define MOVE_TO_PP  if (m_top_atom.get()) { m_top_pp = m_top_atom.get(); m_top_atom = 0; } DUMMY_FUNC
	
void Normalization_Visitor::visit(CP_Alternative const* n) {
	n->get_cp_a()->accept(this);
	MOVE_TO_CP;
	Smart_FQL_Node_Ptr<Coverage_Pattern_Expr> cp_a(m_top_cp);
	n->get_cp_b()->accept(this);
	MOVE_TO_CP;
	Smart_FQL_Node_Ptr<Coverage_Pattern_Expr> cp_b(m_top_cp);

	if (FQL_Node_Lt_Compare()(cp_a.get(), cp_b.get())) {
		m_top_cp = FQL_CREATE2(CP_Alternative, cp_a.get(), cp_b.get());
	} else if (FQL_Node_Lt_Compare()(cp_b.get(), cp_a.get())) {
		m_top_cp = FQL_CREATE2(CP_Alternative, cp_b.get(), cp_a.get());
	} else {
		m_top_cp = cp_a;
	}
}

void Normalization_Visitor::visit(CP_Concat const* n) {
	n->get_cp_a()->accept(this);
	MOVE_TO_CP;
	Smart_FQL_Node_Ptr<Coverage_Pattern_Expr> cp_a(m_top_cp);
	n->get_cp_b()->accept(this);
	MOVE_TO_CP;
	Smart_FQL_Node_Ptr<Coverage_Pattern_Expr> cp_b(m_top_cp);

	m_top_cp = FQL_CREATE2(CP_Concat, cp_a.get(), cp_b.get());
}

void Normalization_Visitor::visit(Depcov const* n) {
	n->get_filter_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_a(m_top_filter);
	n->get_filter_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_b(m_top_filter);
	n->get_filter_c()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_c(m_top_filter);
	
	m_top_atom = FQL_CREATE3(Depcov, filter_a.get(), filter_b.get(), filter_c.get());
	m_top_filter = 0;
}

void Normalization_Visitor::visit(Edgecov const* n) {
	n->get_filter_expr()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter(m_top_filter);

	m_top_atom = FQL_CREATE1(Edgecov, m_prefix.get() ?
			FQL_CREATE2(Filter_Compose, filter.get(), m_prefix.get()) : filter.get());
	m_top_filter = 0;
}

void Normalization_Visitor::visit(Filter_Compose const* n) {
	n->get_filter_expr_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_a(m_top_filter);
	n->get_filter_expr_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter_b(m_top_filter);

	m_top_filter = Filter_Compose::Factory::get_instance().create(filter_a.get(), filter_b.get());
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

void Normalization_Visitor::visit(Nodecov const* n) {
	n->get_filter_expr()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter(m_top_filter);

	m_top_atom = FQL_CREATE1(Nodecov, m_prefix.get() ?
			FQL_CREATE2(Filter_Compose, filter.get(), m_prefix.get()) : filter.get());
	m_top_filter = 0;
}

void Normalization_Visitor::visit(PP_Alternative const* n) {
	n->get_pp_a()->accept(this);
	MOVE_TO_PP;
	Smart_FQL_Node_Ptr<Path_Pattern_Expr> pp_a(m_top_pp);
	n->get_pp_b()->accept(this);
	MOVE_TO_PP;
	Smart_FQL_Node_Ptr<Path_Pattern_Expr> pp_b(m_top_pp);
	
	if (FQL_Node_Lt_Compare()(pp_a.get(), pp_b.get())) {
		m_top_pp = FQL_CREATE2(PP_Alternative, pp_a.get(), pp_b.get());
	} else if (FQL_Node_Lt_Compare()(pp_b.get(), pp_a.get())) {
		m_top_pp = FQL_CREATE2(PP_Alternative, pp_b.get(), pp_a.get());
	} else {
		m_top_pp = pp_a;
	}
}

void Normalization_Visitor::visit(PP_Concat const* n) {
	n->get_pp_a()->accept(this);
	MOVE_TO_PP;
	Smart_FQL_Node_Ptr<Path_Pattern_Expr> pp_a(m_top_pp);
	n->get_pp_b()->accept(this);
	MOVE_TO_PP;
	Smart_FQL_Node_Ptr<Path_Pattern_Expr> pp_b(m_top_pp);

	m_top_pp = FQL_CREATE2(PP_Concat, pp_a.get(), pp_b.get());
}

void Normalization_Visitor::visit(Pathcov const* n) {
	n->get_filter_expr()->accept(this);
	Smart_FQL_Node_Ptr<Filter_Expr> filter(m_top_filter);

	m_top_atom = FQL_CREATE2(Pathcov, m_prefix.get() ?
			FQL_CREATE2(Filter_Compose, filter.get(), m_prefix.get()) :
				filter.get(), n->get_bound());
	m_top_filter = 0;
}

void Normalization_Visitor::visit(Predicate const* n) {
	m_top_atom = Predicate::Factory::get_instance().create(new ::exprt(*(n->get_expr())));
}

void Normalization_Visitor::visit(Query const* n) {
	m_prefix = 0;
	if (n->get_prefix()) {
		n->get_prefix()->accept(this);
		m_prefix = m_top_filter;
	}

	n->get_cover()->accept(this);
	MOVE_TO_CP;
	// IN ... prefix only applies to COVER clause
	m_prefix = 0;

	if (n->get_passing()) {
		n->get_passing()->accept(this);
		MOVE_TO_PP;
	} else
		m_top_pp = FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov, FQL_CREATE_FF0(F_IDENTITY)), 0, -1);

	m_top_query = Query::Factory::get_instance().create(0, m_top_cp.get(), m_top_pp.get());
	m_top_filter = 0;
	m_top_cp = 0;
	m_top_pp = 0;
}

void Normalization_Visitor::visit(Quote const* n) {
	Smart_FQL_Node_Ptr<Filter_Expr> prefix_bak(m_prefix);
	m_prefix = 0;
	n->get_pp()->accept(this);
	MOVE_TO_PP;
	m_prefix = prefix_bak;

	m_top_cp = FQL_CREATE1(Quote, m_top_pp.get());
	m_top_pp = 0;
}

void Normalization_Visitor::visit(Repeat const* n) {
	n->get_pp()->accept(this);
	MOVE_TO_PP;
	m_top_pp = FQL_CREATE3(Repeat, m_top_pp.get(), n->get_lower_bound(),
			n->get_upper_bound());
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

