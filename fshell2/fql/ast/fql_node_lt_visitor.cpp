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

/*! \file fshell2/fql/ast/fql_node_lt_visitor.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Apr 26 18:16:45 CEST 2009 
*/

#include <fshell2/fql/ast/fql_node_lt_visitor.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/not_implemented.hpp>
#endif

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

FQL_Node_Lt_Visitor::FQL_Node_Lt_Visitor() :
	m_other(0), m_lt(false) {}

FQL_Node_Lt_Visitor::~FQL_Node_Lt_Visitor() {}

bool FQL_Node_Lt_Visitor::operator()( FQL_Node const* left, FQL_Node const* right )
{
	return cmp_lt( left, right );
}
		
bool FQL_Node_Lt_Visitor::operator()( FQL_Node const& left, FQL_Node const& right )
{
	return cmp_lt( &left, &right );
}

template <typename T>
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(FQL_Node const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(FQL_Node, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

template <typename T>
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(Filter_Expr const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(Filter_Expr, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

template <typename T>
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(Path_Pattern_Expr const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(Path_Pattern_Expr, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

template <typename T>
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(Coverage_Pattern_Expr const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(Coverage_Pattern_Expr, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

template <typename T>
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(ECP_Atom const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(ECP_Atom, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

LT_VISIT_BUILDER_1(CP_Alternative);
LT_VISIT_BUILDER_1(CP_Concat);
LT_VISIT_BUILDER_1(Depcov);
LT_VISIT_BUILDER_1(Edgecov);
LT_VISIT_BUILDER_1(Filter_Compose);
LT_VISIT_BUILDER_1(Filter_Function);
LT_VISIT_BUILDER_1(Filter_Intersection);
LT_VISIT_BUILDER_1(Filter_Setminus);
LT_VISIT_BUILDER_1(Filter_Union);
LT_VISIT_BUILDER_1(Nodecov);
LT_VISIT_BUILDER_1(PP_Alternative);
LT_VISIT_BUILDER_1(PP_Concat);
LT_VISIT_BUILDER_1(Pathcov);
LT_VISIT_BUILDER_1(Predicate);
LT_VISIT_BUILDER_1(Quote);
LT_VISIT_BUILDER_1(Query);
LT_VISIT_BUILDER_1(Repeat);
LT_VISIT_BUILDER_1(Transform_Pred);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

