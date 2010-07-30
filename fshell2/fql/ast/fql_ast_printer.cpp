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

#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pp_alternative.hpp>
#include <fshell2/fql/ast/pp_concat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/ast/transform_pred.hpp>

#include <cbmc/src/ansi-c/expr2c.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

FQL_AST_Printer::FQL_AST_Printer(::std::ostream & os) :
	m_os(os) {
}

FQL_AST_Printer::~FQL_AST_Printer() {
}

void FQL_AST_Printer::visit(CP_Alternative const* n) {
	m_os << "(";
	n->get_cp_a()->accept(this);
	m_os << "+";
	n->get_cp_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(CP_Concat const* n) {
	m_os << "(";
	n->get_cp_a()->accept(this);
	m_os << ".";
	n->get_cp_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Depcov const* n) {
	m_os << "DEPS(";
	n->get_filter_a()->accept(this);
	m_os << ",";
	n->get_filter_b()->accept(this);
	m_os << ",";
	n->get_filter_c()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Edgecov const* n) {
	m_os << "EDGES(";
	n->get_filter_expr()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Compose const* n) {
	m_os << "COMPOSE(";
	n->get_filter_expr_a()->accept(this);
	m_os << ",";
	n->get_filter_expr_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Function const* n) {
	switch (n->get_filter_type()) {
		case F_IDENTITY:
			m_os << "ID";
			break;
		case F_FILE:
            m_os << "@FILE('";
			m_os << n->get_string_arg<F_FILE>();
			m_os << "')";
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
            m_os << "@FUNC(";
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
            m_os << "@EXIT(";
			m_os << n->get_string_arg<F_EXIT>();
			m_os << ")";
            break;
		case F_EXPR:
            m_os << "@EXPR('";
			m_os << n->get_string_arg<F_EXPR>();
			m_os << "')";
            break;
		case F_REGEXP:
            m_os << "@REGEXP('";
			m_os << n->get_string_arg<F_REGEXP>();
			m_os << "')";
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
		case F_DEF:
            m_os << "@DEF(";
			m_os << n->get_string_arg<F_DEF>();
			m_os << ")";
            break;
		case F_USE:
            m_os << "@USE(";
			m_os << n->get_string_arg<F_USE>();
			m_os << ")";
            break;
		case F_STMTTYPE:
			{
            	m_os << "@STMTTYPE(";
				int types(n->get_int_arg<F_STMTTYPE>());
				bool not_first(false);
				for (unsigned i(0); types && i < sizeof(int)*8; ++i) {
					if (types & (1 << i)) {
						switch (1 << i) {
							case STT_IF:
								if (not_first) m_os << ","; else not_first = true;
								m_os << "IF";
								break;
							case STT_FOR:
								if (not_first) m_os << ","; else not_first = true;
								m_os << "FOR";
								break;
							case STT_WHILE:
								if (not_first) m_os << ","; else not_first = true;
								m_os << "WHILE";
								break;
							case STT_SWITCH:
								if (not_first) m_os << ","; else not_first = true;
								m_os << "SWITCH";
								break;
							case STT_CONDOP:
								if (not_first) m_os << ","; else not_first = true;
								m_os << "?:";
								break;
							case STT_ASSERT:
								if (not_first) m_os << ","; else not_first = true;
								m_os << "ASSERT";
								break;
						}
						types &= ~(1 << i);
					}
				}
				m_os << ")";
			}
            break;
	}
}

void FQL_AST_Printer::visit(Filter_Intersection const* n) {
	m_os << "(";
	n->get_filter_expr_a()->accept(this);
	m_os << "&";
	n->get_filter_expr_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Setminus const* n) {
	m_os << "SETMINUS(";
	n->get_filter_expr_a()->accept(this);
	m_os << ",";
	n->get_filter_expr_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Filter_Union const* n) {
	m_os << "(";
	n->get_filter_expr_a()->accept(this);
	m_os << "|";
	n->get_filter_expr_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(PP_Alternative const* n) {
	m_os << "(";
	n->get_pp_a()->accept(this);
	m_os << "+";
	n->get_pp_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(PP_Concat const* n) {
	m_os << "(";
	n->get_pp_a()->accept(this);
	m_os << ".";
	n->get_pp_b()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Nodecov const* n) {
	m_os << "NODES(";
	n->get_filter_expr()->accept(this);
	m_os << ")";
}

void FQL_AST_Printer::visit(Pathcov const* n) {
	m_os << "PATHS(";
	n->get_filter_expr()->accept(this);
	m_os << "," << n->get_bound();
	m_os << ")";
}

void FQL_AST_Printer::visit(Predicate const* n) {
	::contextt ctx;
	::namespacet ns(ctx);
	::std::string c_expr(::expr2c(*(n->get_expr()), ns));
	::std::string::size_type no_ws_start(0);
	while (::std::isspace(c_expr.at(no_ws_start))) ++no_ws_start;
	::std::string::size_type const prefix(c_expr.find("!PRED! = (_Bool)("));
	c_expr = c_expr.substr(::std::string::npos==prefix?no_ws_start:(prefix+17));
	m_os << "{" << c_expr.substr(0, c_expr.rfind(");\n")) << "}";
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

void FQL_AST_Printer::visit(Quote const* n) {
	m_os << "\"";
	n->get_pp()->accept(this);
	m_os << "\"";
}

void FQL_AST_Printer::visit(Repeat const* n) {
	int const lb(n->get_lower_bound());
	int const ub(n->get_upper_bound());
	m_os << "(";
	n->get_pp()->accept(this);
	if (-1 == ub) {
		if (0 == lb) m_os << "*";
		else m_os << ">=" << lb;
	} else {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == lb);
		m_os << "<=" << ub;
	}
	m_os << ")";
}

void FQL_AST_Printer::visit(Transform_Pred const* n) {
	m_os << "PRED(";
	n->get_filter_expr()->accept(this);
	for (Predicate::preds_t::const_iterator iter(n->get_predicates()->begin());
			iter != n->get_predicates()->end(); ++iter) {
		m_os << ",";
		(*iter)->accept(this);
	}
	m_os << ")";
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

