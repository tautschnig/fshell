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

/*! \file fshell2/macro/macro_processing.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:20 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/macro/macro_processing.hpp>
#include <fshell2/exception/macro_processing_error.hpp>

#define TEST_COMPONENT_NAME Macro_Processing
#define TEST_COMPONENT_NAMESPACE fshell2::macro

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_MACRO_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Macro_Processing
 *
 */
void test_basic( Test_Data & data )
{
	::fshell2::macro::Macro_Processing macro;

	TEST_ASSERT(macro.expand("bla") == "bla");
	TEST_ASSERT(macro.expand("#define alpha beta") == "");

	TEST_ASSERT(macro.expand("bla") == "bla");

	TEST_ASSERT_RELATION(macro.expand("bla bla // comment"), ==, "bla bla");
	TEST_ASSERT_RELATION(macro.expand(" // comment"), ==, "");
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Macro_Processing
 *
 */
void test_help( Test_Data & data )
{
	::fshell2::macro::Macro_Processing macro;

	::std::ostringstream os;
	Macro_Processing::help(os);
	TEST_ASSERT(data.compare("macro_help", os.str()));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Macro_Processing
 *
 */
void test_syntax_errors( Test_Data & data )
{
	::fshell2::macro::Macro_Processing macro;

	TEST_THROWING_BLOCK_ENTER;
	macro.expand("#define 123");
	TEST_THROWING_BLOCK_EXIT(::fshell2::Macro_Processing_Error);
	
	TEST_ASSERT(macro.expand("  #include \"blabla\"") == "#include \"blabla\"");
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Macro_Processing
 *
 */
void test_use_case_simple( Test_Data & data )
{
	::fshell2::macro::Macro_Processing macro;

	TEST_ASSERT(macro.expand("#define alpha beta") == "");
	TEST_ASSERT_RELATION(macro.expand("alpha"), ==, "beta");
	
	TEST_ASSERT(macro.expand("#define alpha beta") == "");
	TEST_THROWING_BLOCK_ENTER;
	macro.expand("#define alpha gamma");
	TEST_THROWING_BLOCK_EXIT(::fshell2::Macro_Processing_Error);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Macro_Processing
 *
 */
void test_use_case_complex( Test_Data & data )
{
	::fshell2::macro::Macro_Processing macro;

	TEST_ASSERT(macro.expand("#define alpha beta") == "");
	TEST_ASSERT_RELATION(macro.expand("alpha"), ==, "beta");
	
	TEST_ASSERT(macro.expand("#define gamma alpha") == "");
	TEST_ASSERT_RELATION(macro.expand("gamma"), ==, "beta");
	
	TEST_ASSERT(macro.expand("#define e(c, f) edgecov(c, f)") == "");
	TEST_ASSERT(macro.expand("#define s(A, B) setminus(A, B)") == "");
	TEST_ASSERT_RELATION(macro.expand("e(cfg, s(@line(1), @line(1)))"), ==,
			"edgecov(cfg, setminus(@line(1), @line(1)))");
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_MACRO_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_basic, LEVEL_PROD );
TEST_NORMAL_CASE( &test_help, LEVEL_PROD );
TEST_NORMAL_CASE( &test_syntax_errors, LEVEL_PROD );
TEST_NORMAL_CASE( &test_use_case_simple, LEVEL_PROD );
TEST_NORMAL_CASE( &test_use_case_complex, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
