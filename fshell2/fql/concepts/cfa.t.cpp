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

/*! \file fshell2/fql/concepts/cfa.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 11 11:14:19 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <diagnostics/basic_exceptions/invalid_argument.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/concepts/cfa.hpp>

#define TEST_COMPONENT_NAME CFA
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
 * @test A test of CFA class invariance
 *
 */
void test_invariance( Test_Data & data )
{
	::goto_programt prg;
	::goto_programt::targett i1(prg.add_instruction(SKIP));
	CFA::node_t n1(&prg, i1);
	::goto_programt::targett i2(prg.add_instruction(SKIP));
	CFA::node_t n2(&prg, i2);
	::goto_programt::targett i3(prg.add_instruction(SKIP));
	CFA::node_t n3(&prg, i3);
	::goto_programt::targett i4(prg.add_instruction(SKIP));
	CFA::node_t n4(&prg, i4);

	CFA::edge_t e1(n1, n2);
	CFA::edge_t e2(n2, n3);

	{
		CFA::edges_t es1;
		es1.insert(e1);
		es1.insert(e2);
		
		CFA::nodes_t ns1, ns2;
		ns1.insert(n4);
		ns2.insert(n1);

		CFA cfa;
		cfa.set_E(es1);
		cfa.set_disconnected_nodes(ns1);
		TEST_THROWING_BLOCK_ENTER;
		cfa.set_disconnected_nodes(ns2);
		TEST_THROWING_BLOCK_EXIT(::diagnostics::Invalid_Argument);
	}

	{
		CFA::edges_t es1;
		es1.insert(e1);
		
		CFA::nodes_t ns1, ns2, ns3, ns4;
		ns1.insert(n4);
		ns2.insert(n4);
		ns3.insert(n1);
		ns4.insert(n3);

		CFA cfa;
		cfa.set_E(es1);
		cfa.set_disconnected_nodes(ns1);
		cfa.set_I(ns2);
		cfa.set_I(ns3);
		TEST_THROWING_BLOCK_ENTER;
		cfa.set_I(ns4);
		TEST_THROWING_BLOCK_EXIT(::diagnostics::Invalid_Argument);
	}
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of CFA
 *
 */
void test( Test_Data & data )
{
	::goto_programt prg;
	::goto_programt::targett i1(prg.add_instruction(SKIP));
	CFA::node_t n1(&prg, i1);
	::goto_programt::targett i2(prg.add_instruction(SKIP));
	CFA::node_t n2(&prg, i2);
	::goto_programt::targett i3(prg.add_instruction(SKIP));
	CFA::node_t n3(&prg, i3);
	::goto_programt::targett i4(prg.add_instruction(SKIP));
	CFA::node_t n4(&prg, i4);
	::goto_programt::targett i5(prg.add_instruction(SKIP));
	CFA::node_t n5(&prg, i5);
	::goto_programt::targett i6(prg.add_instruction(SKIP));
	CFA::node_t n6(&prg, i6);

	CFA::edge_t e1(n1, n2);
	CFA::edge_t e2(n2, n3);
	CFA::edge_t e3(n3, n4);
	CFA::edge_t e4(n4, n5);
	CFA::edge_t e5(n5, n6);

	CFA::edges_t es1, es2;
	es1.insert(e1);
	es1.insert(e2);
	es1.insert(e5);
	es2.insert(e3);
	es2.insert(e4);

	CFA::nodes_t ns1;
	ns1.insert(n1);
	
	CFA cfa1, cfa2;
	cfa1.set_E(es1);
	TEST_CHECK(cfa1.get_E().size() == 3);
	cfa2.set_E(es2);
	TEST_CHECK(cfa2.get_E().size() == 2);

	cfa1.setunion(cfa2);
	TEST_ASSERT(cfa1.get_E().size() == 5);
	TEST_ASSERT(cfa1.get_disconnected_nodes().size() == 0);
	
	cfa1.setsubtract(cfa2);
	TEST_ASSERT(cfa1.get_E().size() == 1);
	TEST_ASSERT(cfa1.get_disconnected_nodes().size() == 1);
	
	cfa1.setintersection(cfa2);
	TEST_ASSERT(cfa1.get_E().size() == 0);
	TEST_ASSERT(cfa1.get_disconnected_nodes().size() == 0);
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_invariance, LEVEL_DEBUG );
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
