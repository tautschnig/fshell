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

/*! \file fshell2/fql/ast/fql_node.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:54 CEST 2009 
*/

#include <fshell2/fql/ast/fql_node.hpp>

#include <fshell2/fql/ast/fql_node_lt_visitor.hpp>
#include <fshell2/fql/ast/fql_ast_printer.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

bool FQL_Node_Lt_Compare::operator()(FQL_Node const * a, FQL_Node const* b) const {
	return FQL_Node_Lt_Visitor()(a, b);
}

::std::ostream & operator<<(::std::ostream & os, FQL_Node const& n) {
	FQL_AST_Printer p(os);
	n.accept(&p);
	return os;
}

FQL_Node::FQL_Node() :
	m_ref_count(0) {
}

FQL_Node::~FQL_Node() {
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

