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

/*! \file fshell2/fql/ast/fql_ast_printer.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Apr 20 23:54:01 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/ast/fql_ast_printer.hpp>

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


#define TEST_COMPONENT_NAME FQL_AST_Printer
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
 * @test A test of FQL_AST_Printer
 *
 */
void test( Test_Data & data )
{
	Filter_Expr * file(FQL_CREATE_FF1(F_FILE, "bla.c"));
	Filter_Expr * line(FQL_CREATE_FF1(F_LINE, 42));
	Filter_Expr * col(FQL_CREATE_FF1(F_COLUMN, 13));
	Filter_Expr * bb(FQL_CREATE_FF0(F_BASICBLOCKENTRY));

	Filter_Expr * intersec1(FQL_CREATE2(Filter_Intersection,
				FQL_CREATE2(Filter_Intersection, file, line),
				FQL_CREATE2(Filter_Intersection, col, bb)));

	Edgecov * e(FQL_CREATE1(Edgecov, intersec1));

	Query * q(FQL_CREATE3(Query, file, e, 0));

	::std::ostringstream os;
	FQL_AST_Printer printer(os);
	q->accept(&printer);
	TEST_ASSERT(data.compare("printed_query_1", os.str()));
	os.str("");

	Filter_Expr * cg(FQL_CREATE_FF0(F_CONDITIONGRAPH));
	Coverage_Pattern_Expr * p(FQL_CREATE2(Pathcov, cg, 1));

	Coverage_Pattern_Expr * union1(FQL_CREATE2(CP_Alternative, e, p));

	Query * q2(FQL_CREATE3(Query, 0, union1, 0));

	os << *q2;
	TEST_ASSERT(data.compare("printed_query_2", os.str()));
	os.str("");

	Filter_Expr * stt(FQL_CREATE_FF1(F_STMTTYPE, STT_IF | STT_CONDOP | STT_WHILE));
	Edgecov * estt(FQL_CREATE1(Edgecov, stt));

	Query * q3(FQL_CREATE3(Query, 0, estt, 0));

	os << *q3;
	TEST_ASSERT(data.compare("printed_query_3", os.str()));
	os.str("");
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
