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

/*! \file fshell2/fql/ast/pp_concat.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Aug  2 19:01:36 CEST 2009 
*/

#include <fshell2/fql/ast/pp_concat.hpp>
#include <fshell2/config/annotations.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#endif

#include <fshell2/fql/ast/ast_visitor.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

PP_Concat::PP_Concat(Path_Pattern_Expr * a, Path_Pattern_Expr * b) :
	m_pp_a(a), m_pp_b(b) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, m_pp_a);
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, m_pp_b);
}

void PP_Concat::accept(AST_Visitor * v) const {
	v->visit(this);
}

void PP_Concat::accept(AST_Visitor const * v) const {
	v->visit(this);
}

bool PP_Concat::destroy() {
	if (this->m_ref_count) return false;
	Factory::get_instance().destroy(this);
	m_pp_a->decr_ref_count();
	m_pp_a->destroy();
	m_pp_b->decr_ref_count();
	m_pp_b->destroy();
	return true;
}

PP_Concat::~PP_Concat() {
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

