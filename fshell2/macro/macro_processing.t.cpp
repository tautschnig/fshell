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
void test( Test_Data & data )
{
	using ::fshell2::macro::Macro_Processing;

	TEST_ASSERT(Macro_Processing::get_instance().expand("bla") == "bla");
	TEST_ASSERT(Macro_Processing::get_instance().expand("#define alpha beta") == "");
	TEST_ASSERT_RELATION(Macro_Processing::get_instance().expand("alpha"), ==, "beta\n");

	::std::ostringstream os;
	Macro_Processing::help(os);
	TEST_ASSERT(data.compare("macro_help", os.str()));
	os.str("");

	TEST_THROWING_BLOCK_ENTER;
	Macro_Processing::get_instance().expand("#define 123");
	TEST_THROWING_BLOCK_EXIT(::fshell2::Macro_Processing_Error);
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_MACRO_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
