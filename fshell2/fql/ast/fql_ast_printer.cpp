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

/*! \file fshell2/fql/ast/fql_ast_printer.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Apr 20 23:53:53 CEST 2009 
*/

#include <fshell2/fql/ast/fql_ast_printer.hpp>
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

FQL_AST_Printer::FQL_AST_Printer(::std::ostream & os) :
	m_os(os) {
}

FQL_AST_Printer::~FQL_AST_Printer() {
}

void FQL_AST_Printer::visit(Query const* n) {
	if (n->get_prefix()) {
		m_os << "IN ";
		n->get_prefix()->accept(this);
		m_os << " ";
	}
	m_os << "COVER ";
	n->get_cover()->accept(this);
	if (n->get_passing()) {
		m_os << " PASSING ";
		n->get_passing()->accept(this);
	}
}

void FQL_AST_Printer::visit(Test_Goal_Sequence const* n) {
	if (n->get_sequence().front().first) {
		n->get_sequence().front().first->accept(this);
		m_os << " ";
	}
	n->get_sequence().front().second->accept(this);

	for (Test_Goal_Sequence::seq_t::const_iterator iter(++(n->get_sequence().begin()));
			iter != n->get_sequence().end(); ++iter) {
		m_os << " ";
		if (iter->first) {
			iter->first->accept(this);
			m_os << " ";
		}
		iter->second->accept(this);
	}

	if (n->get_suffix_automaton()) {
		m_os << " ";
		n->get_suffix_automaton()->accept(this);
	}
}

void FQL_AST_Printer::visit(Restriction_Automaton const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void FQL_AST_Printer::visit(Abstraction const* n) {
	m_os << "CFG";
	if (n->get_predicates().empty()) return;
	m_os << "[";
	(*n->get_predicates().begin())->accept(this);
	for (::std::set<Predicate *, FQL_Node_Lt_Compare>::const_iterator iter(++(n->get_predicates().begin()));
			iter != n->get_predicates().end(); ++iter) {
		m_os << ",";
		(*iter)->accept(this);
	}
	m_os << "]";
}

void FQL_AST_Printer::visit(Predicate const* n) {
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void FQL_AST_Printer::visit(TGS_Union const* n) {
	m_os << "UNION(";
	n->get_tgs_a()->accept(this);
	m_os << ",";
	n->get_tgs_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(TGS_Intersection const* n) {
	m_os << "INTERSECT(";
	n->get_tgs_a()->accept(this);
	m_os << ",";
	n->get_tgs_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(TGS_Setminus const* n) {
	m_os << "SETMINUS(";
	n->get_tgs_a()->accept(this);
	m_os << ",";
	n->get_tgs_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Edgecov const* n) {
	m_os << "EDGECOV(";
	n->get_abstraction()->accept(this);
	m_os << ",";
	n->get_filter()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Pathcov const* n) {
	m_os << "PATHCOV(";
	n->get_abstraction()->accept(this);
	m_os << ",";
	n->get_filter()->accept(this);
	m_os << "," << n->get_bound() << ")";
}

void FQL_AST_Printer::visit(Filter_Identity const* n) {
	m_os << "IDENITY";
}

void FQL_AST_Printer::visit(Filter_Complement const* n) {
	m_os << "COMPLEMENT(";
	n->get_filter()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Union const* n) {
	m_os << "UNION(";
	n->get_filter_a()->accept(this);
	m_os << ",";
	n->get_filter_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Intersection const* n) {
	m_os << "INTERSECT(";
	n->get_filter_a()->accept(this);
	m_os << ",";
	n->get_filter_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Setminus const* n) {
	m_os << "SETMINUS(";
	n->get_filter_a()->accept(this);
	m_os << ",";
	n->get_filter_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Enclosing_Scopes const* n) {
	m_os << "ENCLOSING_SCOPES(";
	n->get_filter()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Primitive_Filter const* n) {
	switch (n->get_filter_type()) {
		case F_FILE:
            m_os << "@FILE(\"";
			m_os << n->get_string_arg<F_FILE>();
			m_os << "\")";
            break;
		case F_LINE:
            m_os << "@LINE(";
			m_os << n->get_int_arg<F_LINE>();
			m_os << ")";
            break;
		case F_COLUMN:
            m_os << "@COLUMN(";
			m_os << n->get_int_arg<F_COLUMN>();
			m_os << ")";
            break;
		case F_FUNC:
            m_os << "@FUNC";
			m_os << n->get_string_arg<F_FUNC>();
			m_os << ")";
            break;
		case F_LABEL:
            m_os << "@LABEL(";
			m_os << n->get_string_arg<F_LABEL>();
			m_os << ")";
            break;
		case F_CALL:
            m_os << "@CALL(";
			m_os << n->get_string_arg<F_CALL>();
			m_os << ")";
            break;
		case F_CALLS:
            m_os << "@CALLS";
            break;
		case F_ENTRY:
            m_os << "@ENTRY(";
			m_os << n->get_string_arg<F_ENTRY>();
			m_os << ")";
            break;
		case F_EXIT:
            m_os << "@EXIT";
			m_os << n->get_string_arg<F_EXIT>();
			m_os << ")";
            break;
		case F_EXPR:
            m_os << "@EXPR(\"";
			m_os << n->get_string_arg<F_EXPR>();
			m_os << "\")";
            break;
		case F_REGEXP:
            m_os << "@REGEXP(\"";
			m_os << n->get_string_arg<F_REGEXP>();
			m_os << "\")";
            break;
		case F_BASICBLOCKENTRY:
            m_os << "@BASICBLOCKENTRY";
            break;
		case F_CONDITIONEDGE:
            m_os << "@CONDITIONEDGE";
            break;
		case F_DECISIONEDGE:
            m_os << "@DECISIONEDGE";
            break;
		case F_CONDITIONGRAPH:
            m_os << "@CONDITIONGRAPH";
            break;
	}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

