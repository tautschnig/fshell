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

/*! \file fshell2/fql/ast/query.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:54 CEST 2009 
*/

#include <fshell2/fql/ast/query.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/invalid_argument.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Query::Query(Filter * prefix, Test_Goal_Sequence * cover,
		Restriction_Automaton * passing) :
	m_prefix(prefix), m_cover(cover), m_passing(passing) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, m_cover);
}

void Query::accept(AST_Visitor * v) const {
	v->visit(this);
}

void Query::accept(AST_Visitor const * v) const {
	v->visit(this);
}

bool Query::destroy() {
	if (this->m_ref_count) return false;
	Factory::get_instance().destroy(this);
	if (m_prefix) {
		m_prefix->decr_ref_count();
		m_prefix->destroy();
	}
	m_cover->decr_ref_count();
	m_cover->destroy();
	if (m_passing) {
		m_passing->decr_ref_count();
		m_passing->destroy();
	}
	return true;
}

Query::~Query() {
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

