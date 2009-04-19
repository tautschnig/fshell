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

/*! \file fshell2/main/fshell2.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 15 11:58:54 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/main/fshell2.hpp>

#include <cerrno>
#include <fstream>
#include <readline/readline.h>

#include <cbmc/src/util/config.h>

#define TEST_COMPONENT_NAME FShell2
#define TEST_COMPONENT_NAMESPACE fshell2

FSHELL2_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of FShell2
 *
 */
void test_single( Test_Data & data )
{
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l(cmdline);
	::std::ostringstream os;
	
	TEST_ASSERT(::fshell2::FShell2::get_instance().process_line(l, os, "QUIT"));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of FShell2
 *
 */
void test_interactive( Test_Data & data )
{
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l(cmdline);
	::std::ostringstream os;
	
	char * tempname(::strdup("/tmp/queryXXXXXX"));
	TEST_CHECK(-1 != ::mkstemp(tempname));
	::std::ofstream of(tempname);
	TEST_CHECK(of.is_open());
	of << "show sourcecode all" << ::std::endl
		<< "#define bla blubb" << ::std::endl
		<< "quit" << ::std::endl;
	of.close();
		
	// open the input file
	FILE * in(fopen(tempname, "r"));
	TEST_CHECK(0 == errno);
	// change the readline input to the file
	rl_instream = in;

	::fshell2::FShell2::get_instance().interactive(l, os);

	// print and check the output
	TEST_TRACE(os.str());
	::std::string data_name(::diagnostics::internal::to_string(
				"interactive_query_out_", sizeof(long) * 8, "bit"));
	TEST_ASSERT(data.compare(data_name, os.str()));
	
	// change back to stdin
	rl_instream = 0;
	fclose(in);

	::unlink(tempname);
	free(tempname);
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_single, LEVEL_PROD );
TEST_NORMAL_CASE( &test_interactive, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
