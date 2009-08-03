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
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(Filter const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(Filter, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

template <typename T>
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(Path_Monitor const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(Path_Monitor, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

template <typename T>
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit(Test_Goal_Set const* n)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

LT_VISIT_BUILDER_CPP_BASE(Test_Goal_Set, FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false));

LT_VISIT_BUILDER_1(Edgecov);
LT_VISIT_BUILDER_1(Filter_Complement);
LT_VISIT_BUILDER_1(Filter_Compose);
LT_VISIT_BUILDER_1(Filter_Enclosing_Scopes);
LT_VISIT_BUILDER_1(Filter_Function);
LT_VISIT_BUILDER_1(Filter_Intersection);
LT_VISIT_BUILDER_1(Filter_Setminus);
LT_VISIT_BUILDER_1(Filter_Union);
LT_VISIT_BUILDER_1(PM_Alternative);
LT_VISIT_BUILDER_1(PM_Concat);
LT_VISIT_BUILDER_1(PM_Filter_Adapter);
LT_VISIT_BUILDER_1(PM_Next);
LT_VISIT_BUILDER_1(PM_Repeat);
LT_VISIT_BUILDER_1(Pathcov);
LT_VISIT_BUILDER_1(Predicate);
LT_VISIT_BUILDER_1(Query);
LT_VISIT_BUILDER_1(Statecov);
LT_VISIT_BUILDER_1(TGS_Intersection);
LT_VISIT_BUILDER_1(TGS_Setminus);
LT_VISIT_BUILDER_1(TGS_Union);
LT_VISIT_BUILDER_1(Test_Goal_Sequence);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

