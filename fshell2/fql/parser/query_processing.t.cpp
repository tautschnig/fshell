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

/*! \file fshell2/fql/parser/query_processing.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:39 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/parser/query_processing.hpp>
#include <fshell2/exception/query_processing_error.hpp>

#include <fshell2/fql/ast/query.hpp>

#define TEST_COMPONENT_NAME Query_Processing
#define TEST_COMPONENT_NAMESPACE fshell2::fql

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Query;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Query_Processing
 *
 */
void test_basic( Test_Data & data )
{
	::std::ostringstream os;
	::fshell2::fql::Query * q(0);
	Query_Processing qp;

	qp.parse(os, "cover edges(@file(\"bla.c\"))", &q);
	TEST_ASSERT(q != 0);
	os.str("");

	os << *q;
	TEST_ASSERT(data.compare("parsed_basic_query_1", os.str()));
	q->destroy();
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Query_Processing
 *
 */
void test_help( Test_Data & data )
{
	::std::ostringstream os;

	::fshell2::fql::Query_Processing::help(os);
	TEST_ASSERT(data.compare("query_help", os.str()));
	os.str("");
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Query_Processing
 *
 */
void test_invalid( Test_Data & data )
{
	::std::ostringstream os;
	::fshell2::fql::Query * q(0);
	Query_Processing qp;

	TEST_THROWING_BLOCK_ENTER;
	qp.parse(os, "bla bla bla", &q);
	TEST_THROWING_BLOCK_EXIT(::fshell2::Query_Processing_Error);
	
	TEST_THROWING_BLOCK_ENTER;
	qp.parse(os, "cover edges(@basicblockentry)->(@basicblockentry)", &q);
	TEST_THROWING_BLOCK_EXIT(::fshell2::Query_Processing_Error);

	TEST_ASSERT(0 == q);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Query_Processing
 *
 */
void test_use_case( Test_Data & data )
{
	::std::ostringstream os;
	::fshell2::fql::Query * q(0);
	Query_Processing qp;
	
	qp.parse(os, "cover edges(@file(\"bla.c\"))", &q);
	TEST_ASSERT(q != 0);
	os.str("");
	os << *q;
	TEST_ASSERT(data.compare("parsed_use_case_query_1", os.str()));
	q->destroy();
	
	qp.parse(os, "in @file(\"bla.c\") cover edges(id) passing @func(main)*", &q);
	TEST_ASSERT(q != 0);
	os.str("");
	os << *q;
	TEST_ASSERT(data.compare("parsed_use_case_query_2", os.str()));
	q->destroy();
	
	qp.parse(os, "in @file(\"bla.c\") cover edges(@BASICBLOCKENTRY) passing setminus(@file(\"bla.c\"),@func(unimplemented))*", &q);
	TEST_ASSERT(q != 0);
	os.str("");
	os << *q;
	TEST_ASSERT(data.compare("parsed_use_case_query_3", os.str()));
	q->destroy();
	
	qp.parse(os, "in @file(\"bla.c\") cover edges(@BASICBLOCKENTRY) -[ @func(main)+@func(other) ]> edges(@BASICBLOCKENTRY) passing @func(main)*.@func(other).id*", &q);
	TEST_ASSERT(q != 0);
	os.str("");
	os << *q;
	TEST_ASSERT(data.compare("parsed_use_case_query_4", os.str()));
	q->destroy();
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_basic, LEVEL_PROD );
TEST_NORMAL_CASE( &test_help, LEVEL_PROD );
TEST_NORMAL_CASE( &test_invalid, LEVEL_PROD );
TEST_NORMAL_CASE( &test_use_case, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
