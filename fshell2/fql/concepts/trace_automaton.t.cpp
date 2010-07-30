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

/*! \file fshell2/fql/concepts/trace_automaton.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Jul 28 16:00:02 CEST 2010 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/concepts/trace_automaton.hpp>

#define TEST_COMPONENT_NAME trace_automaton_t
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
 * @test A test of trace_automaton_t
 *
 */
void test( Test_Data & data )
{
	trace_automaton_t ta;
	ta_state_t const s1(ta.new_state());
	ta.initial().insert(s1);
	ta_state_t const s2(ta.new_state());
	ta.set_trans(s1, 0, s2);
	ta.set_trans(s2, 0, s2);
	ta_state_t const s3(ta.new_state());
	ta.set_trans(s1, -1, s3);
	ta.set_trans(s2, -1, s3);
	ta_state_t const s4(ta.new_state());
	ta.set_trans(s3, -1, s4);
	ta_state_t const s5(ta.new_state());
	ta.set_trans(s4, 1, s5);
	ta_state_t const s6(ta.new_state());
	ta.final(s6) = 1;
	ta.set_trans(s5, 2, s6);
	ta_state_t const s7(ta.new_state());
	ta.final(s7) = 1;
	ta.set_trans(s5, 2, s7);

	::std::ostringstream os;
	print_trace_automaton(ta, os);
	TEST_ASSERT(data.compare("trace_aut_1", os.str()));
	simplify_automaton(ta, false);
	os.str("");
	print_trace_automaton(ta, os);
	TEST_ASSERT(data.compare("trace_aut_2", os.str()));
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
