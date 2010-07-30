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

/*! \file fshell2/fql/normalize/normalization_visitor.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 29 16:41:50 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/normalize/normalization_visitor.hpp>

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

#define TEST_COMPONENT_NAME Normalization_Visitor
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
 * @test A test of Normalization_Visitor
 *
 */
void test_union_intersect( Test_Data & data )
{
	Path_Pattern_Expr * idstar(FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov, FQL_CREATE_FF0(F_IDENTITY)), 0, -1));
	idstar->incr_ref_count(); // re-used below, after normalize

	Filter_Expr * file(FQL_CREATE_FF1(F_FILE, "bla.c"));
	Filter_Expr * line(FQL_CREATE_FF1(F_LINE, 42));
	TEST_CHECK(FQL_Node_Lt_Compare()(file, line));

	Filter_Expr * intersec1(FQL_CREATE2(Filter_Intersection, line, file));
	intersec1->incr_ref_count(); // re-used below, after normalize

	Edgecov * e(FQL_CREATE1(Edgecov, intersec1));

	Query * q(FQL_CREATE3(Query, 0, e, 0));
	::std::ostringstream os1;
	os1 << *q;

	Filter_Expr * intersec2(FQL_CREATE2(Filter_Intersection, file, line));
	Edgecov * e2(FQL_CREATE1(Edgecov, intersec2));
	
	Query * q2(FQL_CREATE3(Query, 0, e2, idstar));
	::std::ostringstream os2;
	os2 << *q2;
	
	TEST_ASSERT_RELATION(os1.str(), !=, os2.str());

	Normalization_Visitor norm;
	norm.normalize(&q);
	os1.str("");
	os1 << *q;
	
	TEST_ASSERT_RELATION(os1.str(), ==, os2.str());
	
	Filter_Expr * union1(FQL_CREATE2(Filter_Union, intersec1, intersec2));
	
	Edgecov * e3(FQL_CREATE1(Edgecov, union1));

	Query * q3(Query::Factory::get_instance().create(0, e3, 0));
	os1.str("");
	os1 << *q3;
	TEST_CHECK(os1.str().find("|") != ::std::string::npos);
	norm.normalize(&q3);
	os1.str("");
	os1 << *q3;
	TEST_CHECK(::std::string::npos == os1.str().find("|"));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Normalization_Visitor
 *
 */
void test_prefix( Test_Data & data )
{
	Filter_Expr * file(FQL_CREATE_FF1(F_FILE, "bla.c"));
	Filter_Expr * line(FQL_CREATE_FF1(F_LINE, 42));

	Edgecov * e(FQL_CREATE1(Edgecov, line));

	Query * q(FQL_CREATE3(Query, file, e, 0));
	::std::ostringstream os1;
	os1 << *q;
	
	Filter_Expr * compose(FQL_CREATE2(Filter_Compose, line, file));

	Edgecov * e2(FQL_CREATE1(Edgecov, compose));

	Path_Pattern_Expr * idstar(FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov, FQL_CREATE_FF0(F_IDENTITY)), 0, -1));
	Query * q2(FQL_CREATE3(Query, 0, e2, idstar));
	::std::ostringstream os2;
	os2 << *q2;
	
	TEST_ASSERT_RELATION(os1.str(), !=, os2.str());

	Normalization_Visitor norm;
	norm.normalize(&q);
	os1.str("");
	os1 << *q;
	
	TEST_ASSERT_RELATION(os1.str(), ==, os2.str());
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_union_intersect, LEVEL_PROD );
TEST_NORMAL_CASE( &test_prefix, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
