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

/*! \file fshell2/instrumentation/goto_lt_compare.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Nov  7 15:23:04 CET 2010 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/instrumentation/goto_lt_compare.hpp>

#define TEST_COMPONENT_NAME GOTO_Lt_Compare
#define TEST_COMPONENT_NAMESPACE fshell2::instrumentation

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of GOTO_Lt_Compare
 *
 */
void test( Test_Data & data )
{
	GOTO_Lt_Compare lt_cmp;

	::goto_programt prg;
	::goto_programt::targett skip1(prg.add_instruction(SKIP));
	skip1->function = "func1";
	::goto_programt::targett skip2(prg.add_instruction(SKIP));
	skip2->function = "func1";
	prg.compute_location_numbers();
	TEST_ASSERT(lt_cmp(skip1, skip2));
	TEST_ASSERT(!lt_cmp(skip1, skip1));
	TEST_ASSERT(!lt_cmp(skip2, skip1));
	
	::goto_programt prg2;
	::goto_programt::targett skip3(prg.add_instruction(SKIP));
	skip3->function = "func2";
	prg2.compute_location_numbers();
	TEST_ASSERT(lt_cmp(skip1, skip3));
	TEST_ASSERT(lt_cmp(skip2, skip3));
	TEST_ASSERT(!lt_cmp(skip3, skip1));
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
