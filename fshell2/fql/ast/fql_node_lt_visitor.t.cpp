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

/*! \file fshell2/fql/ast/fql_node_lt_visitor.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Apr 26 18:16:50 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/ast/fql_node_lt_visitor.hpp>

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

#define TEST_COMPONENT_NAME FQL_Node_Lt_Visitor
#define TEST_COMPONENT_NAMESPACE fshell2::fql

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of FQL_Node_Lt_Visitor
 *
 */
void test( Test_Data & data )
{
	Filter_Expr * file(FQL_CREATE_FF1(F_FILE, "bla.c"));
	Filter_Expr * line(FQL_CREATE_FF1(F_LINE, 42));
	Filter_Expr * label(FQL_CREATE_FF1(F_LABEL, "blabla"));
	Filter_Expr * con(FQL_CREATE_FF0(F_CONDITIONEDGE));

	Filter_Expr * intersec1(FQL_CREATE2(Filter_Intersection,
				FQL_CREATE2(Filter_Setminus, file, line),
				FQL_CREATE2(Filter_Union, label, con)));

	Edgecov * e(FQL_CREATE1(Edgecov, intersec1));

	Query * q(FQL_CREATE3(Query, file, e, 0));

	Filter_Expr * cg(FQL_CREATE_FF0(F_CONDITIONGRAPH));
	Coverage_Pattern_Expr * p(FQL_CREATE2(Pathcov, cg, 1));

	Coverage_Pattern_Expr * union1(FQL_CREATE2(CP_Alternative, e, p));

	Query * q2(FQL_CREATE3(Query, 0, union1, 0));

	TEST_ASSERT(FQL_Node_Lt_Compare()(file, line));
	TEST_ASSERT(!FQL_Node_Lt_Compare()(line, file));
	TEST_ASSERT(!FQL_Node_Lt_Compare()(file, file));
	
	TEST_ASSERT(FQL_Node_Lt_Compare()(file, intersec1));
	
	TEST_ASSERT(FQL_Node_Lt_Compare()(e, q));
	
	TEST_ASSERT(FQL_Node_Lt_Compare()(union1, e));
	TEST_ASSERT(FQL_Node_Lt_Compare()(q, q2));

	Predicate * p1(FQL_CREATE1(Predicate, new ::exprt()));
	Predicate * p2(FQL_CREATE1(Predicate, new ::exprt()));
	TEST_ASSERT(!FQL_Node_Lt_Compare()(p1, p2));
	TEST_ASSERT(!FQL_Node_Lt_Compare()(p2, p1));
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
