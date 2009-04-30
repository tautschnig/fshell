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

#include <diagnostics/basic_exceptions/not_implemented.hpp>

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
	(*query)->accept(this);
	if (m_top_query.get() != *query) {
		(*query)->destroy();
		*query = m_top_query.get();
	}
	(*query)->incr_ref_count();
	m_top_query = 0;
	(*query)->decr_ref_count();
}

void Normalization_Visitor::visit(Query const* n) {
	Smart_FQL_Node_Ptr<Filter> prefix;
	if (n->get_prefix()) {
		n->get_prefix()->accept(this);
		prefix = m_top_filter;
	}

	n->get_cover()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Sequence> cover(m_top_tgseq);

	Smart_FQL_Node_Ptr<Restriction_Automaton> passing;
	if (n->get_passing()) {
		n->get_passing()->accept(this);
		passing = m_top_aut;
	}

	m_top_query = Query::Factory::get_instance().create(prefix.get(), cover.get(), passing.get());
	m_top_filter = 0;
	m_top_tgseq = 0;
	m_top_aut = 0;
}

void Normalization_Visitor::visit(Test_Goal_Sequence const* n) {
	Test_Goal_Sequence::seq_t seq;
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		Smart_FQL_Node_Ptr<Restriction_Automaton> aut;
		if (iter->first) {
			iter->first->accept(this);
			aut = m_top_aut;
			aut.get()->incr_ref_count();
		}
		iter->second->accept(this);
		m_top_tgset.get()->incr_ref_count();
		seq.push_back(::std::make_pair(aut.get(), m_top_tgset.get()));
	}

	Smart_FQL_Node_Ptr<Restriction_Automaton> aut;
	if (n->get_suffix_automaton()) {
		n->get_suffix_automaton()->accept(this);
		aut = m_top_aut;
	}
	
	for (Test_Goal_Sequence::seq_t::const_iterator iter(seq.begin());
			iter != seq.end(); ++iter) {
		if (iter->first) iter->first->decr_ref_count();
		iter->second->decr_ref_count();
	}

	m_top_tgseq = Test_Goal_Sequence::Factory::get_instance().create(seq, aut.get());
	m_top_aut = 0;
	m_top_tgset = 0;
}

void Normalization_Visitor::visit(Restriction_Automaton const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Normalization_Visitor::visit(Abstraction const* n) {
	Abstraction::preds_t preds;
	for (Abstraction::preds_t::const_iterator iter(n->get_predicates().begin());
			iter != n->get_predicates().end(); ++iter) {
		(*iter)->accept(this);
		if (preds.insert(m_top_pred.get()).second) m_top_pred.get()->incr_ref_count();
	}
	
	for (Abstraction::preds_t::const_iterator iter(preds.begin());
			iter != preds.end(); ++iter)
		(*iter)->decr_ref_count();

	m_top_abst = Abstraction::Factory::get_instance().create(preds);
	m_top_pred = 0;
	
}

void Normalization_Visitor::visit(Predicate const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
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

void Normalization_Visitor::visit(TGS_Setminus const* n) {
	n->get_tgs_a()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_a(m_top_tgset);
	n->get_tgs_b()->accept(this);
	Smart_FQL_Node_Ptr<Test_Goal_Set> tgs_b(m_top_tgset);

	m_top_tgset = TGS_Setminus::Factory::get_instance().create(tgs_a.get(), tgs_b.get());
}

void Normalization_Visitor::visit(Edgecov const* n) {
	n->get_abstraction()->accept(this);
	Smart_FQL_Node_Ptr<Abstraction> abst(m_top_abst);
	n->get_filter()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter(m_top_filter);

	m_top_tgset = Edgecov::Factory::get_instance().create(abst.get(), filter.get());
	m_top_abst = 0;
	m_top_filter = 0;
}

void Normalization_Visitor::visit(Pathcov const* n) {
	n->get_abstraction()->accept(this);
	Smart_FQL_Node_Ptr<Abstraction> abst(m_top_abst);
	n->get_filter()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter(m_top_filter);

	m_top_tgset = Pathcov::Factory::get_instance().create(abst.get(), filter.get(), n->get_bound());
	m_top_abst = 0;
	m_top_filter = 0;
}

void Normalization_Visitor::visit(Filter_Identity const* n) {
	m_top_filter = Filter_Identity::Factory::get_instance().create();
}

void Normalization_Visitor::visit(Filter_Complement const* n) {
	n->get_filter()->accept(this);
	m_top_filter = Filter_Complement::Factory::get_instance().create(m_top_filter.get());
}

void Normalization_Visitor::visit(Filter_Union const* n) {
	n->get_filter_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter_a(m_top_filter);
	n->get_filter_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter_b(m_top_filter);
	
	if (FQL_Node_Lt_Compare()(filter_a.get(), filter_b.get())) {
		m_top_filter = Filter_Union::Factory::get_instance().create(filter_a.get(), filter_b.get());
	} else if (FQL_Node_Lt_Compare()(filter_b.get(), filter_a.get())) {
		m_top_filter = Filter_Union::Factory::get_instance().create(filter_b.get(), filter_a.get());
	} else {
		m_top_filter = filter_a;
	}
}

void Normalization_Visitor::visit(Filter_Intersection const* n) {
	n->get_filter_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter_a(m_top_filter);
	n->get_filter_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter_b(m_top_filter);
	
	if (FQL_Node_Lt_Compare()(filter_a.get(), filter_b.get())) {
		m_top_filter = Filter_Intersection::Factory::get_instance().create(filter_a.get(), filter_b.get());
	} else if (FQL_Node_Lt_Compare()(filter_b.get(), filter_a.get())) {
		m_top_filter = Filter_Intersection::Factory::get_instance().create(filter_b.get(), filter_a.get());
	} else {
		m_top_filter = filter_a;
	}
}

void Normalization_Visitor::visit(Filter_Setminus const* n) {
	n->get_filter_a()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter_a(m_top_filter);
	n->get_filter_b()->accept(this);
	Smart_FQL_Node_Ptr<Filter> filter_b(m_top_filter);

	m_top_filter = Filter_Setminus::Factory::get_instance().create(filter_a.get(), filter_b.get());
}

void Normalization_Visitor::visit(Filter_Enclosing_Scopes const* n) {
	n->get_filter()->accept(this);
	m_top_filter = Filter_Enclosing_Scopes::Factory::get_instance().create(m_top_filter.get());
}

void Normalization_Visitor::visit(Primitive_Filter const* n) {
	switch (n->get_filter_type()) {
		case F_FILE:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_FILE>(
					n->get_string_arg<F_FILE>());
			break;
		case F_LINE:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_LINE>(
					n->get_int_arg<F_LINE>());
			break;
		case F_COLUMN:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_COLUMN>(
					n->get_int_arg<F_COLUMN>());
			break;
		case F_FUNC:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_FUNC>(
					n->get_string_arg<F_FUNC>());
			break;
		case F_LABEL:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_LABEL>(
					n->get_string_arg<F_LABEL>());
			break;
		case F_CALL:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_CALL>(
					n->get_string_arg<F_CALL>());
			break;
		case F_CALLS:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_CALLS>();
			break;
		case F_ENTRY:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_ENTRY>(
					n->get_string_arg<F_ENTRY>());
			break;
		case F_EXIT:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_EXIT>(
					n->get_string_arg<F_EXIT>());
			break;
		case F_EXPR:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_EXPR>(
					n->get_string_arg<F_EXPR>());
			break;
		case F_REGEXP:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_REGEXP>(
					n->get_string_arg<F_REGEXP>());
			break;
		case F_BASICBLOCKENTRY:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_BASICBLOCKENTRY>();
			break;
		case F_CONDITIONEDGE:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_CONDITIONEDGE>();
			break;
		case F_DECISIONEDGE:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_DECISIONEDGE>();
			break;
		case F_CONDITIONGRAPH:
			m_top_filter = Primitive_Filter::Factory::get_instance().create<F_CONDITIONGRAPH>();
			break;
	}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

