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

#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

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
	Filter_Expr * file(Filter_Function::Factory::get_instance().create<F_FILE>("bla.c"));
	Filter_Expr * line(Filter_Function::Factory::get_instance().create<F_LINE>(42));
	Filter_Expr * col(Filter_Function::Factory::get_instance().create<F_COLUMN>(13));
	Filter_Expr * bb(Filter_Function::Factory::get_instance().create<F_BASICBLOCKENTRY>());

	Filter_Expr * intersec1(Filter_Intersection::Factory::get_instance().create(
				Filter_Intersection::Factory::get_instance().create(file,line),
				Filter_Intersection::Factory::get_instance().create(col,bb)));

	Edgecov * e(Edgecov::Factory::get_instance().create(intersec1, 
				static_cast< Predicate::preds_t * >(0)));

	Test_Goal_Sequence::seq_t seq_list;
	seq_list.push_back(::std::make_pair<Path_Monitor_Expr *, Test_Goal_Set *>(0, e));
	Test_Goal_Sequence * s(Test_Goal_Sequence::Factory::get_instance().create(seq_list, 0));

	Query * q(Query::Factory::get_instance().create(file, s, 0));

	::std::ostringstream os;
	FQL_AST_Printer printer(os);
	q->accept(&printer);
	TEST_ASSERT(data.compare("printed_query_1", os.str()));
	os.str("");

	Filter_Expr * cg(Filter_Function::Factory::get_instance().create<F_CONDITIONGRAPH>());
	Pathcov * p(Pathcov::Factory::get_instance().create(cg, 1, 0));

	Test_Goal_Set * union1(TGS_Union::Factory::get_instance().create(e, p));

	Test_Goal_Sequence::seq_t seq_list2;
	seq_list2.push_back(::std::make_pair<Path_Monitor_Expr *, Test_Goal_Set *>(0, union1));
	Test_Goal_Sequence * s2(Test_Goal_Sequence::Factory::get_instance().create(seq_list2, 0));

	Query * q2(Query::Factory::get_instance().create(0, s2, 0));

	os << *q2;
	TEST_ASSERT(data.compare("printed_query_2", os.str()));
	os.str("");

	Filter_Expr * stt(Filter_Function::Factory::get_instance().create<F_STMTTYPE>(STT_IF | STT_CONDOP | STT_WHILE));
	Edgecov * estt(Edgecov::Factory::get_instance().create(stt, static_cast< Predicate::preds_t * >(0)));

	Test_Goal_Sequence::seq_t seq_list3;
	seq_list3.push_back(::std::make_pair<Path_Monitor_Expr *, Test_Goal_Set *>(0, estt));
	Test_Goal_Sequence * s3(Test_Goal_Sequence::Factory::get_instance().create(seq_list3, 0));

	Query * q3(Query::Factory::get_instance().create(0, s3, 0));

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
