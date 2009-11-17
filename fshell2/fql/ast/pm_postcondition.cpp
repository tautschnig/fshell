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

/*! \file fshell2/fql/ast/pm_postcondition.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Aug  2 19:02:19 CEST 2009 
*/

#include <fshell2/fql/ast/pm_postcondition.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/invalid_argument.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

PM_Postcondition::PM_Postcondition(Path_Monitor_Expr * a, Predicate * pred) :
	m_path_monitor_expr(a), m_predicate(pred) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, m_path_monitor_expr);
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, m_predicate);
}

void PM_Postcondition::accept(AST_Visitor * v) const {
	v->visit(this);
}

void PM_Postcondition::accept(AST_Visitor const * v) const {
	v->visit(this);
}

bool PM_Postcondition::destroy() {
	if (this->m_ref_count) return false;
	Factory::get_instance().destroy(this);
	m_path_monitor_expr->decr_ref_count();
	m_path_monitor_expr->destroy();
	m_predicate->decr_ref_count();
	m_predicate->destroy();
	return true;
}

PM_Postcondition::~PM_Postcondition() {
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

